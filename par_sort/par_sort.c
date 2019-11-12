#define DBL(a) (a)

/*
  Code to sort on a parallel machine.
  Written by Andrew Tridgell September 1992 in collaboration with
  Richard Brent

  This program is Copyright Andrew Tridgell 1992.

  This program may be used for non-commercial purposes. If you wish to
  use this program for commercial purposes or as part of a commercial
  product then please contact Andrew Tridgell.

  There is no warranty with this product.  

  last updated 20th May 1994

  contact:
     Andrew Tridgell
     Computer Sciences Laboratory
     Research School of Physical Sciences and Engineering
     Australian National University
     GPO BOX 4, Canberra City, A.C.T, 2601, Australia
     Tel: (06) 249-3064
     Fax: (06) 249-1884

     Internet: Andrew.Tridgell@anu.edu.au

  The main function in this file is:
    int parallel_sort(void **data,int N,int el_size,int (*comparison)());

  which has similar syntax to the library function qsort(), with the
  proviso that comparison() cannot assume cell dependant pointers, so to
  sort strings they would have to be of a fixed length and strcmp() could be
  the comparison function.

  the function returns the number of items in this node after sorting,
  and may alter the pointer *data.

  The function is called from all the cells simultaneously. They
  should pass to the function a pointer to a pointer to the data that
  they have and an integer saying how many they have (not how many there
  are total).

  TODO:

  Remove the restriction on a reallocable pointer

  Store in block form without re-arrangement

*/


#if 0
#define memcpy amemmove
#define qsort gnu_qsort
#endif

/* should I use a radix sort for the local sort?
NOTE: This will cause the memory use to double and won't work for 
a general data type. It is only recommended for types such as 32 bit integers */
#if RADIX
#define qsort radix_sort
void radix_sort(void *p,int len,int el_size,int (*fn)());
#endif


#ifndef SIMPLE
#define SIMPLE 0
#endif

/* shall I do dynamic balancing ? */
#ifndef BALANCE
#define BALANCE 0
#endif

/* shall I do a final slow check, which will guarantee a 
   sort for any data - and also check that it's working ? */
#ifndef GUARANTEE_SORT
#define GUARANTEE_SORT 0
#endif

/* shall I use a hyper pre-sort ? */
#ifndef HYPER_SORT
#define HYPER_SORT 1
#endif

/* shall I use batchers sort ? */
#ifndef BATCHERS
#define BATCHERS 1
#endif

/* shall I use a bitonic sort ? */
#ifndef BITONIC
#define BITONIC 0
#endif

/* shall I try fast find techniques in the merge ? */
#ifndef FAST_MERGE
#define FAST_MERGE 1
#endif

/* shall I test each merge step for correctness */
#ifndef MERGE_TEST
#define MERGE_TEST 0
#endif

/* shall I try to use an unbalanced merge ? */
#ifndef UNBALANCED_MERGE
#define UNBALANCED_MERGE 1
#endif

/* shall I pre-balance with a hypercube balance ? */
#ifndef HYPER_BALANCE
#define HYPER_BALANCE 1
#endif

/* shall I do a complete balance after the pre-balance? */
#ifndef COMPLETE_BALANCE
#define COMPLETE_BALANCE 1
#endif

/* shall I used a sliced pre-merge ? */
#ifndef SLICED
#define SLICED 0
#endif

/* shall I work out the number of elements needed exactly */
#ifndef FIND_EXACT
#define FIND_EXACT 1
#endif

/* shall I pad cells to get an even balance ? */
#ifndef PAD_CELLS
#define PAD_CELLS 0
#endif

/* shall I use infinity padding ? */
#ifndef INFINITY_PADDING
#define INFINITY_PADDING 1
#endif

/* shall I assume that the data is balanced ? */
#ifndef ASSUME_BALANCED
#define ASSUME_BALANCED 0
#endif

/* shall I assume integer data only ? */
#ifndef INTEGER
#define INTEGER 0
#endif

/* shall I keeps stats on the sort ? */
#ifndef STATS
#define STATS 0
#endif

/* shall I acknowledge big messages ? */
#ifndef ACKNOWLEDGE_BIG_MSGS
#define ACKNOWLEDGE_BIG_MSGS 0
#endif

/* shall I acknowledge all messages ? */
#ifndef ACKNOWLEDGE_ALL_MSGS
#define ACKNOWLEDGE_ALL_MSGS 0
#endif

/* shall I split big messages ? */
#ifndef SPLIT_BIG_MSGS
#define SPLIT_BIG_MSGS 0
#endif

/* this file defines the following function */
int parallel_sort(void **data,int N,int el_size,int (*comparison)());

#include <stdio.h>
#include "mimd.h"

#ifdef __GNUC__
int usleep();
int printf();
int free();
#endif

#if 0
#define DBUG(s) printf("\n node %d  %s\n",NODE_NUMBER,s)
#else
#define DBUG(s)
#endif

/* a few basic defs */
typedef enum {False=0,True=1} BOOL;
void *malloc();
#define QSORT_CAST int (*)()
#define FN
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif
#define IS_ODD(i) (((unsigned)i) & 1)


/* these macros provide all the element manipulation. Special purpose ones
   are used for integer sorts */
#if !INTEGER
#define COMPARE(p1,p2) PSI.comparison(p1,p2)
#define ELEMENT(ptr,i) ((char *)(ptr) + (i)*(EL_SIZE))
#define COPYEL(p1,p2) memcpy(p1,p2,EL_SIZE)
#define COPYEL_INC(p1,p2) \
  (COPYEL(p1,p2),p1=ELEMENT(p1,1),p2=ELEMENT(p2,1))
#define COPYELS(p1,p2,n) memcpy(p1,p2,(n)*EL_SIZE)
#else
int iii;
#define COMPARE(p1,p2) (*((int *)(p1)) - *((int *)(p2)))
#define ELEMENT(ptr,i) ((void *)((int *)(ptr) + (i)))
#define COPYEL(p1,p2) (*((int *)p1) = *((int *)p2))
#define COPYEL_INC(p1,p2) \
  (COPYEL(p1,p2),p1=ELEMENT(p1,1),p2=ELEMENT(p2,1))
#define COPYELS(p1,p2,n) memcpy(p1,p2,(n)*EL_SIZE)
#endif
#define PTR(a,b) ELEMENT(a,b)
#define el_vector(n) malloc((n)*EL_SIZE)

#define EL_SIZE PSI.el_size

/* this struct contains info needed for a parallel sort, it must be shared
between several functions which is why it is global - this means that
parallel sort can not be used recursively! */
struct
{
  int (*comparison)();
  int local_size;
  int el_size;
  void *data1;
  int malloc_size1;
  void *data2;
  int malloc_size2;
  void *padding;
  int pad_size;
  int max_size;
}
parallel_sort_info;
#define PSI parallel_sort_info

#if STATS
/* the following bits allow for easy routine timing */
enum timers
{
  T_ALL,
  T_MAKESMALLER,
  T_MERGE,
  T_FIND,
  T_QSORT,
  T_HYPER,
  T_COMMS,
  T_FIND_EXACT,
  T_SWAP_MEM,
  T_INSIT1,
  T_INSIT2,
  T_SIMPLE,
  T_INSIT3,
  T_SMALL_MERGE,
  T_HYPER_SORT,
  T_BATCHERS_SORT,
  T_NONE};

typedef char label[20];

label timer_labels[T_NONE] =
{
  "ALL",
  "MAKESMALLER",
  "MERGE",
  "FIND",
  "QSORT",
  "HYPERBALANCE",
  "WAITING+COMMS",
  "FIND_EXACT",
  "SWAP_MEM",
  "T_INSIT1",
  "T_INSIT2",
  "SIMPLE",
  "REARRANGE",
  "SMALL_MERGE",
  "HYPER_SORT",
  "BATCHERS_SORT"
};

struct
{
  BOOL started;
  double time_started;
  double cum_time;
} timers_struct[T_NONE];


struct
{
  int messages;
  int num_guarantee_loops;
}
stats;
#endif

#if (STATS && !HOST)
#define ts(x) timers_struct[x]
#define TS(x) (ts(x).started=True,ts(x).time_started=MIMD_Uptime())
#define TE(x) (ts(x).started==False?printf("Not started!\n"):(ts(x).started=False,ts(x).cum_time+=(MIMD_Uptime()-ts(x).time_started)))
#else
#define TS(x)
#define TE(x)
#endif


/*******************************************************************
calloc some memory - trying for the max amount less than or equal to
the requested amount
********************************************************************/
FN void *max_calloc(int *nelem,int elsize)
{
void *realloc();
void *p=NULL;
int high = *nelem,low=1;
int trial=high;
while (high>low)
  {
    if (p)
      p = realloc(p,trial*elsize);
    else
      p = malloc(trial*elsize);
    if (p)
      low = trial;
    else 
      high = trial-1;
    trial = (high+low)/2;
  }
if (!p) p = malloc(trial*elsize);
*nelem = trial;
return(p);
}




void fmemcpy(void *to,void *from,int size)
{
  int N;
  char *c1=to,*c2=from;
  int dorotate=0,dofloat=0,dodouble=0,doshort=0;
  int block=0;
  
  /* trivial case */
  if (c1 == c2) return;
  
  {
    int diff = (int)to - (int)from;
    if (diff<0) diff = -diff;
    switch (diff%sizeof(double))
      {
      case 0:
	dodouble=1;
	dofloat=1;
	doshort=1;
	dorotate=0;
	block=8;
	break;
      case 1:
      case 3:
      case 5:
      case 7:
	dodouble=0;
	dofloat=0;
	doshort=0;
	dorotate=1;
	block=1;
	break;
      case 2:
      case 6:
	dodouble=0;
	dofloat=0;
	doshort=1;
	dorotate=0;
	block=2;
	break;
      case 4:
	dodouble=0;
	dofloat=1;
	doshort=1;
	dorotate=0;
	block=4;
	break;
      }
  }
  
  /* do the first few bytes */
  N = block - ((int)c1 % block);
  if (N != block)
    {
      size -= N;
      while (N--) *c1++ = *c2++;
    }


    
#define MEMTYPE double
  if (dodouble)
    {
      MEMTYPE *p1=(MEMTYPE *)c1,*p2=(MEMTYPE *)c2;
      MEMTYPE d0,d1,d2,d3,d4,d5,d6,d7;
      N = size/sizeof(MEMTYPE);
      while (N >= 8)
	{
	  d0=p2[0];d1=p2[1];d2=p2[2];d3=p2[3];
	  d4=p2[4];d5=p2[5];d6=p2[6];d7=p2[7];
	  p1[0]=d0;p1[1]=d1;p1[2]=d2;p1[3]=d3;
	  p1[4]=d4;p1[5]=d5;p1[6]=d6;p1[7]=d7;
	  p1 += 8;p2 += 8;N -= 8;
	}
      c1 = (char *)p1;
      c2 = (char *)p2;
      size = size % (sizeof(MEMTYPE)*8);
    }

#undef MEMTYPE
#define MEMTYPE float

  if (dofloat)
    {
      MEMTYPE *p1=(MEMTYPE *)c1,*p2=(MEMTYPE *)c2;
      MEMTYPE d0,d1,d2,d3,d4,d5,d6,d7;
      N = size/sizeof(MEMTYPE);
      while (N >= 8)
	{
	  d0=p2[0];d1=p2[1];d2=p2[2];d3=p2[3];
	  d4=p2[4];d5=p2[5];d6=p2[6];d7=p2[7];
	  p1[0]=d0;p1[1]=d1;p1[2]=d2;p1[3]=d3;
	  p1[4]=d4;p1[5]=d5;p1[6]=d6;p1[7]=d7;
	  p1 += 8;p2 += 8;N -= 8;
	}
      c1 = (char *)p1;
      c2 = (char *)p2;
      size = size % (sizeof(MEMTYPE)*8);
    }

#undef MEMTYPE
#define MEMTYPE short

  if (doshort)
    {
      MEMTYPE *p1=(MEMTYPE *)c1,*p2=(MEMTYPE *)c2;
      MEMTYPE d0,d1,d2,d3,d4,d5,d6,d7;
      N = size/sizeof(MEMTYPE);
      while (N >= 8)
	{
	  d0=p2[0];d1=p2[1];d2=p2[2];d3=p2[3];
	  d4=p2[4];d5=p2[5];d6=p2[6];d7=p2[7];
	  p1[0]=d0;p1[1]=d1;p1[2]=d2;p1[3]=d3;
	  p1[4]=d4;p1[5]=d5;p1[6]=d6;p1[7]=d7;
	  p1 += 8;p2 += 8;N -= 8;
	}
      c1 = (char *)p1;
      c2 = (char *)p2;
      size = size % (sizeof(MEMTYPE)*8);
    }

#undef MEMTYPE
#define MEMTYPE unsigned

  if (dorotate)
    {
      N = size;
      
      /* align the source on a MEMTYPE block */
      while (N && (int)c2%sizeof(MEMTYPE)!=0)
	{
	  *c1++ = *c2++;
	  N--;
	}
      
      /* now work in blocks of 8 MEMTYPEs */
      {
	int rotation = ((int)c1%sizeof(MEMTYPE));
	MEMTYPE d0,d1,d2,d3,d4,d5,d6,d7;
	MEMTYPE head,tail;
	MEMTYPE *p1=(MEMTYPE *)(c1-rotation),*p2=(MEMTYPE *)c2;
	int rot8 = rotation*8;
	int rev8 = (sizeof(MEMTYPE)-rotation)*8;
	
	while (N>=8*sizeof(MEMTYPE))
	  {
	    /* load the 8 MEMTYPEs */
	    d0=p2[0];d1=p2[1];d2=p2[2];d3=p2[3];
	    d4=p2[4];d5=p2[5];d6=p2[6];d7=p2[7];
	    
#define ROTATE(from,to) to=((to)>>rot8) | ((from)<<rev8)
	    
	    /* rotate them if necessary */
	    if (rotation)
	      {
		head = p1[0]>>rev8;
		tail = p1[8]<<rot8;
		ROTATE(d7,tail);
		ROTATE(d6,d7);ROTATE(d5,d6);ROTATE(d4,d5);ROTATE(d3,d4);
		ROTATE(d2,d3);ROTATE(d1,d2);ROTATE(d0,d1);
		ROTATE(head,d0);
		p1[8]=tail;
	      }
	    
	    /* and store them */
	    p1[0]=d0;p1[1]=d1;p1[2]=d2;p1[3]=d3;
	    p1[4]=d4;p1[5]=d5;p1[6]=d6;p1[7]=d7;
	    
	    p1 += 8;
	    p2 += 8;
	    N-=8*sizeof(MEMTYPE);
	  }
	c1=((char *)p1) + rotation;
	c2=(char *)p2;
	size = N;
      }
    }
  
  /* cleanup */
  N = size;
  while (N--)
    *c1++ = *c2++;
  
}


#define memcpy fmemcpy


/*******************************************************************
swap some memory
********************************************************************/
FN void memswp(void *p1,void *p2,int size)
{
  register float *d1 = p1,*d2 = p2;

  {
    register float a1,a2,a3,a4;
    register float b1,b2,b3,b4;
    while (size>(4*sizeof(float)))
      {
	a1=d1[0];a2=d1[1];a3=d1[2];a4=d1[3];
	b1=d2[0];b2=d2[1];b3=d2[2];b4=d2[3];
	d2[0]=a1;d2[1]=a2;d2[2]=a3;d2[3]=a4;
	d1[0]=b1;d1[1]=b2;d1[2]=b3;d1[3]=b4;
	d1+=4;d2+=4;
	size -= 4*sizeof(float);
      }
  }
  
  {
    register float a1,b1;
    while (size>sizeof(float))
      {
	a1=d1[0];
	b1=d2[0];
	d2[0]=a1;
	d1[0]=b1;
	d1++;d2++;
	size -= sizeof(float);
      }
  }

  {
    register char a1,b1;
    register char *c1=(char *)d1,*c2=(char *)d2;
    while (size>0)
      {
	a1= *c1;
	b1= *c2;
	*c2=a1;
	*c1=b1;
	c1++;
	c2++;
	size--;
      }
  }
}

#if !HOST
#define MYSORT_P 128

/*******************************************************************
a makesmaller for mysort
********************************************************************/
FN void mysort_makesmaller(void *data,int n1,int n2)
{
void insitu_merge();
char *p1,*p2;
int l1,l2;
int N = PSI.local_size;
int P = MYSORT_P;
int l1_kept,l2_kept;

l1 = l2 = N/P;
if (n1 < N%P) l1++;
if (n2 < N%P) l2++;

p1 = PTR(data,n1*(N/P) + MIN(n1,N%P));
p2 = PTR(data,n2*(N/P) + MIN(n2,N%P));

{
  int L1 = l1,L2 = l2;
  int min=MAX(L1-L2,0),max=L1,proposed;
  while (min < max)
    {
      proposed = (min+max)/2 + 1;
      if (COMPARE(PTR(p1,proposed-1),PTR(p2,L1-proposed)) > 0)
	max = proposed - 1;
      else
	min = proposed;
    }
  l1_kept = min;
  l2_kept = L2 - (L1 - l1_kept);
}

memswp(PTR(p1,l1_kept),p2,(l1 - l1_kept)*EL_SIZE);

insitu_merge(p1,l1_kept,l2-l2_kept,0);
insitu_merge(p2,l1-l1_kept,l2_kept,0);
}


/*******************************************************************
an attempted faster sorting algorithm
********************************************************************/
FN void mysort(void *base)
{
int i;
int l;
int P = MYSORT_P;
int N = PSI.local_size;
void hyper_sort();
void batchers_sort();
char *p;

for (i=0;i<P;i++)
  {
    l = N/P;
    if (i < N%P) l++;
    
    p = PTR(base,i*(N/P) + MIN(i,N%P));
    qsort(p,l,EL_SIZE,PSI.comparison);
  }
    
hyper_sort(0,base,MYSORT_P,mysort_makesmaller);
batchers_sort(base,MYSORT_P,mysort_makesmaller);
}
#endif


/*******************************************************************
panic
********************************************************************/
FN void MM_Panic(const char *msg)
{
MIMD_Panic(msg);
}

#if HOST
/*******************************************************************
wait for a message
********************************************************************/
FN void MM_wait_for_message(int node,int useconds)
{
do 
  usleep(useconds);
while (!MIMD_Host_Msg_Pending(node));
}
#endif

/*******************************************************************
sum integers from all nodes to the host
********************************************************************/
FN int MM_sum_bc_int(int v)
{
int sum=0;
#if HOST
MM_wait_for_message(BOTTOM_NODE,1000);
MIMD_Recv(BOTTOM_NODE,&sum,sizeof(sum));
return(sum + v);
#else
MIMD_Sum_Int(v,&sum);
if (IS_BOTTOM_NODE)
  MIMD_Send_To_Host(&sum,sizeof(sum));
return(sum);
#endif
}

/*******************************************************************
broadcast to next node down
********************************************************************/
FN void MM_bc(void *data,int size)
{
#if HOST
MIMD_Host_Broad(data,size);
#else
MIMD_Recv_Broad_From_Host(data,size);
#endif
}

/*******************************************************************
broadcast an integer
********************************************************************/
FN void MM_bc_integer(int v)
{
  MM_bc(&v,sizeof(v));
}

#define ACKNOWLEDGE_MSGS (ACKNOWLEDGE_BIG_MSGS || ACKNOWLEDGE_ALL_MSGS)
/*******************************************************************
send a big message
********************************************************************/
FN void MM_Send_Big(int to,char *ptr,int size)
{
TS(T_COMMS);
while (size > 0)
  {
#if SPLIT_BIG_MSGS    
    int to_send = MIN(BIG_MSG_SIZE,size);
#else
    int to_send = size;
#endif
    int sent = MIMD_Send(to,ptr,to_send);
    ptr += to_send; /* note: to_send used to be sent, but there seems */
    size -= to_send;/* to be problems with macros and getting sent back - peterb 2/4/93*/
#if (ACKNOWLEDGE_BIG_MSGS && !ACKNOWLEDGE_ALL_MSGS)
    if (size>0) 
#endif
#if ACKNOWLEDGE_MSGS
      {
	int ack;
	MIMD_Recv(to,&ack,sizeof(ack));
	if (ack != 1)
	  MM_Panic("message acknowledge failed!");
      }
#endif
  }
TE(T_COMMS);
}

/*******************************************************************
recv a big message
********************************************************************/
FN void MM_Recv_Big(int from,char *ptr,int size)
{
TS(T_COMMS);
while (size > 0)
  {
#if SPLIT_BIG_MSGS    
    int to_recv = MIN(BIG_MSG_SIZE,size);
#else
    int to_recv = size;
#endif
    int recvd = MIMD_Recv(from,ptr,to_recv);
    ptr += recvd;
    size -= recvd;
#if (ACKNOWLEDGE_BIG_MSGS && !ACKNOWLEDGE_ALL_MSGS)
    if (size>0) 
#endif
#if ACKNOWLEDGE_MSGS
      {
	int ack=1;
	MIMD_Send(from,&ack,sizeof(ack));
      }
#endif
  }
TE(T_COMMS);
}


/*******************************************************************
sort using floyds heapsort
********************************************************************/
FN int sort_floyds(int *LL, int j)
{
register int *L = LL-1;
  int i=1;
register int	rt,lt,x,y;
register int	temp;

	lt = j/2 +1;
	rt = j;
	for (;;) {
	  if (lt > i) {
	    lt--;
	    temp = L[lt];
	  }
	  else {
	    temp = L[rt];
	    L[rt] = L[i];
	    rt--;
	    if (rt == i) {
	      L[i] = temp;
	      return(1);
	    }
	  }
	  y = lt;
	  do {
	    x = y;
	    y *= 2;
	    if (y < rt)  { if (L[y] < L[y+1])  y++; }
	    if (y <=rt)  { L[x] = L[y]; }
	  }  while (y <= rt);
	  y = x;
	  x = y/2;
	  for (;(temp > L[x]) && (y != lt); ) {
	    L[y] = L[x];
	    y = x;
	    x = y/2;
	  }
	  L[y] = temp;
	}
}

#if 1

/*******************************************************************
sort using floyds heapsort
********************************************************************/
FN int sort_floyds_general(void *LL,int j)
{
  int i=1;
  register int	rt,lt,x,y;
#if INTEGER
  register int temp;
  register int *l = PTR(LL,-1);
  #define L(x) l[x]
  #define TOTEMP(x) (temp=L(x))
  #define FROMTEMP(x) (L(x)=temp)
  #define LT(x1,x2) (L(x1)<L(x2))
  #define LT_TEMP(x) (L(x)<temp)
  #define COPYL(x1,x2) (L(x1)=L(x2))
#else
  static void *temp=NULL;
  register void *L = PTR(LL,-1);
  #define L(x) PTR(L,x)
  #define TOTEMP(x) COPYEL(temp,L(x))
  #define FROMTEMP(x) COPYEL(L(x),temp)
  #define LT(x1,x2) (COMPARE(L(x1),L(x1))<0)
  #define LT_TEMP(x) (COMPARE(L(x),temp)<0)
  #define COPYL(x1,x2) COPYEL(L(x1),L(x2))
  if (!temp) temp=el_vector(1);
#endif
	lt = j/2 +1;
	rt = j;
	for (;;) {
	  if (lt > i) {
	    lt--;
	    TOTEMP(lt);
	  }
	  else {
	    TOTEMP(rt);
	    COPYL(rt,i);
	    rt--;
	    if (rt == i) {
	      FROMTEMP(i);
	      return(1);
	    }
	  }
	  y = lt;
	  do {
	    x = y;
	    y *= 2;
	    if (y < rt)  { if (LT(y,y+1))  y++; }
	    if (y <=rt)  { COPYL(x,y); }
	  }  while (y <= rt);
	  y = x;
	  x = y/2;
	  for (;LT_TEMP(x) && (y != lt); ) {
	    COPYL(y,x);
	    y = x;
	    x = y/2;
	  }
	  FROMTEMP(y);
	}
}
#endif

/*******************************************************************
exchange data between 2 nodes
********************************************************************/
FN void MM_exchange_data(int node,void *mydata,void *hisdata,int mysize,int hissize)
{
if (node > NODE_NUMBER)
  {
#if !BLOCKING
    if (mysize>0)
      MM_Send_Big(node,mydata,mysize);
    if (hissize>0)
      MM_Recv_Big(node,hisdata,hissize);
#else
    if (hissize>0)
      MM_Recv_Big(node,hisdata,hissize);
    if (mysize>0)
      MM_Send_Big(node,mydata,mysize);
#endif

  }
if (node < NODE_NUMBER)
  {
    if (mysize>0)
      MM_Send_Big(node,mydata,mysize);
    if (hissize>0)
      MM_Recv_Big(node,hisdata,hissize);
  }
}


/*******************************************************************
share some data with neighboring cells
********************************************************************/
FN void MM_share_with_neighbours(void *mup,void *mdn,void *up,void *dn,int smup,int smdn,int sup,int sdn)
{
if (NODE_NUMBER%2 == 0)
  {    
    if (!IS_TOP_NODE)
      MM_exchange_data(NODE_NUMBER+1,mup,up,smup,sup);
    if (!IS_BOTTOM_NODE)
      MM_exchange_data(NODE_NUMBER-1,mdn,dn,smdn,sdn);
  }
else
  {
    if (!IS_BOTTOM_NODE)
      MM_exchange_data(NODE_NUMBER-1,mdn,dn,smdn,sdn);
    if (!IS_TOP_NODE)
      MM_exchange_data(NODE_NUMBER+1,mup,up,smup,sup);
  }
}

/*******************************************************************
broadcast receive an integer
********************************************************************/
FN int MM_bc_receive_integer()
{
  int i;
  MM_bc(&i,sizeof(i));
  return(i);
}


#if !HOST

/*******************************************************************
a safe realloc
********************************************************************/
void *safe_realloc(void *ptr,int size)
{
void *realloc();
if (size==0) 
  {
    free(ptr);
    return(NULL);
  }
if (ptr)
  ptr = realloc(ptr,size);
else
  ptr = malloc(size);
if (!ptr) MM_Panic("memory error in realloc\n");
return(ptr);
}


/*******************************************************************
malloc or re-alloc a pointer to give a specified size. If desired_size
is 0 then the block is freed and cur_size is reset
********************************************************************/
void *expand_pointer(void **ptr,int *cur_size,int desired_size)
{
if (desired_size == 0)
  {
    if (*ptr) free(*ptr);
    *ptr = NULL;
    *cur_size = 0;
    return(*ptr);
  }

if (*ptr && (desired_size <= *cur_size)) return(*ptr);


*ptr = (void *)safe_realloc(*ptr,desired_size);

*cur_size = desired_size;
return(*ptr);
}



/*******************************************************************
a sorting algorithm not designed to actually sort the data but to make
it more sorted. It operates particularly well in parallel. It calls
the merge exchange algorithm along the edges of a hypercube
********************************************************************/
FN void hyper_sort(int base,void *data,int number,void (*makesmaller)())
{
int i;
if (number==1) return;
for (i=0;i<(number/2);i++)
  makesmaller(data,base+i,base+i+((number+1)/2));

#if 1
hyper_sort(base+number/2,data,(number+1)/2,makesmaller);
hyper_sort(base,data,number - (number+1)/2,makesmaller);
#else
hyper_sort(base+(number+1)/2,data,number/2,makesmaller);
hyper_sort(base,data,number - number/2,makesmaller);
#endif
}

/*******************************************************************
use batcher's method to sort some data. makesmaller should be like this:
void makesmaller(void *data,int n1,int n2)
when called it should ensure that element n1 is smaller then element n2
data will always just be the same as data
See Knuth Vol 3
********************************************************************/
void batchers_sort(void *data,int N,void (*makesmaller)())
{
  int	p, initq, q, r, d, x;

  for (p=1; (1<<(p+1))<=N+1; p++);
  p = 1<<p;
  
  for (initq=p; p>0; p/=2) 
    {
      q = initq;
      r = 0;
      d = p;
      do 
	{
	  for (x=0; x<N-d; x++)
	    if ( (x & p) == r) 
	      makesmaller(data,x,x+d);
	  d = q - p;
	  q /= 2;
	  r = p;
	} while (q != p/2);
    }
}


/*******************************************************************
bitonic sort for already sorted lists of elements. See Fox
********************************************************************/
void bitonic_sort(void *data,int N,void (*makesmaller)())
{
#define BIT(x,i) ((((x)>>(i)) & 1) != 0)
  int d;
  int i,j;
  int offset;

  /* find cube dimension */
  for (d=1; (1<<(d+1))<=N+1; d++);

  for (i=1;i<=d;i++)
    {
      for (j=(i-1);j>=0;j--)
	{
	  offset = 1<<j;
	  if (NODE_NUMBER & offset) offset = -offset;

	  if (BIT(NODE_NUMBER,i) != BIT(NODE_NUMBER,j))
	    makesmaller(data,NODE_NUMBER + offset,NODE_NUMBER);
	  else
	    makesmaller(data,NODE_NUMBER,NODE_NUMBER + offset);
	}
    }
}

#if RADIX 
/* The following defines a simple radix sort routine. This is not used
by default. It is not particularly optimised, and probably could be
done much better */

#define RTYPE unsigned short
#define BITS (8*sizeof(RTYPE))
#define BINS (1<<BITS)

unsigned char **radix_hist_byte=NULL;
int  **radix_hist=NULL;
void *radix_ptr_hist[BINS];

/*******************************************************************
histogram the data ready for a radix sort
********************************************************************/
void radix_histogram(RTYPE *data,int stride,int N)
{
  int i,j;

  /* initialise the bins */
  for (j=0;j<stride;j++)
    for (i=0;i<(int)BINS;i++)
      radix_hist_byte[j][i] = radix_hist[j][i] = 0;

  /* fill em up */
  for (i=0;i<N;i++)
    for (j=0;j<stride;j++)
      {
	if ((++radix_hist_byte[j][*data]) == 0)
	  radix_hist[j][*data] += 256;
	data++;
      }

  for (j=0;j<stride;j++)
    for (i=0;i<(int)BINS;i++)
      radix_hist[j][i] += radix_hist_byte[j][i];
}


/*******************************************************************
play with pointers for the radix sort
********************************************************************/
void ptr_histogram(void *data,int j)
{
  int i;
  for (i=0;i<(int)BINS;i++)
    {
      radix_ptr_hist[i] = data;
      data = ELEMENT(data,radix_hist[j][i]);
    }
}

/*******************************************************************
redistribute the data
********************************************************************/
void copy_data(void *data,RTYPE *keys,int stride,int N)
{
  while (N--)
    {
      COPYEL_INC(radix_ptr_hist[*keys],data);
      keys += stride;
    }
}


/*******************************************************************
the body of the radix sort
********************************************************************/
void radix_sort1(void *data,int N,int el_size)
{
  void *data1 = data;
  void *data2 = (void *)malloc(el_size*N);
  void *tmp;
  int i;
  int stride = el_size / sizeof(RTYPE);

  radix_hist = (int **)malloc(sizeof(int *)*stride);
  radix_hist_byte = (unsigned char **)malloc(sizeof(unsigned char *)*stride);
  for (i=0;i<stride;i++)
    {
      if (!(radix_hist[i] = (int *)malloc(sizeof(int)*BINS)))
	{
	  printf("can't malloc radix histogram\n");
	  exit(1);
	}
      if (!(radix_hist_byte[i] = (unsigned char *)
	    malloc(sizeof(unsigned char)*BINS)))
	{
	  printf("can't malloc radix byte histogram\n");
	  exit(1);
	}
    }

  if (!data2)
    {
      printf("Can't allocate memory for data2 in radix_sort1()\n");
      exit(1);
    }

  radix_histogram((RTYPE *)data1,stride,N);

  for (i=stride-1;i>=0;i--)
    {
      /* create the pointer histogram */
      ptr_histogram(data2,i);

      /* make the new data */
      copy_data(data1,((RTYPE *)data1)+i,stride,N);
      
      tmp = data2; data2 = data1; data1 = tmp;
    }
  
  if (data2 == data)
    {
      COPYELS(data,data1,N);
      free(data1);
    }
  else
    {
      free(data2);    
    }  

  for (i=0;i<stride;i++)
    {
      free(radix_hist[i]);
      free(radix_hist_byte[i]);
    }
  free(radix_hist);
  free(radix_hist_byte);
  radix_hist = NULL;
  radix_hist_byte = NULL;
}


/*******************************************************************
a radix sort made to look like qsort()
********************************************************************/
void radix_sort(void *p,int len,int el_size,int (*fn)())
{
  radix_sort1(p,len,el_size);
}
#endif


/*******************************************************************
balance a pair of nodes by exchanging elements
********************************************************************/
void pair_balance(void **pd,int n1,int n2)
{
int other_size;
int other_node = (n1==NODE_NUMBER?n2:n1);
int new_size,total_size;

if (n1 != NODE_NUMBER && n2 != NODE_NUMBER) return;

/* find out each others sizes */
#if ASSUME_BALANCED
other_size = PSI.local_size;
#else
MM_exchange_data(other_node,&PSI.local_size,&other_size,sizeof(int),sizeof(int));
#endif

total_size = PSI.local_size + other_size;
new_size = total_size/2;

if (IS_ODD(total_size) && (NODE_NUMBER<other_node))
  new_size++;

if (new_size == PSI.local_size) return;

if (new_size > PSI.local_size)
  {
    *pd = safe_realloc(*pd,new_size*EL_SIZE);
    MM_Recv_Big(other_node,((char *)(*pd))+PSI.local_size*EL_SIZE,
	      (new_size-PSI.local_size)*EL_SIZE);
  }
if (new_size < PSI.local_size)
  {
    MM_Send_Big(other_node,((char *)(*pd))+new_size*EL_SIZE,
	      (PSI.local_size-new_size)*EL_SIZE);
    *pd = safe_realloc(*pd,new_size*EL_SIZE);
  }

PSI.local_size = new_size;
}


/*******************************************************************
balance the nodes using the hypercube balance
********************************************************************/
void hypercube_balance(int base,int number,void **pd)
{
int i;
if (number==1) return;
for (i=0;i<(number/2);i++)
  pair_balance(pd,base+i,base+i+((number+1)/2));

hypercube_balance(base+number/2,(number+1)/2,pd);
hypercube_balance(base,number - (number+1)/2,pd);

}


 
/********************************************************************
  complete the balancing
********************************************************************/
FN void complete_balance(void **pd)
{
  int n;
  int total,target;
  MIMD_Sum_Int(PSI.local_size,&total);  
  PSI.max_size = MIMD_Max_Int(PSI.local_size);
  
  if (total % NUM_NODES == 0)
    target = total / NUM_NODES;
  else
    target = PSI.max_size;
  
  if (IS_TOP_NODE)
    {
      int i;
      target = total - target*(NUM_NODES-1);
      
      for (i=BOTTOM_NODE;i<TOP_NODE;i++)
 	{
 	  MIMD_Recv(i,&n,sizeof(n));
 	  if (n>0)
 	    {
 	      n = MIN(n,PSI.local_size);
 	      MIMD_Send(i,&n,sizeof(n));
 	      MIMD_Send(i,PTR(*pd,PSI.local_size-n),n*EL_SIZE);
 	      PSI.local_size -= n;
 	    }
 	  if (n<0)
 	    {
 	      MIMD_Send(i,&n,sizeof(n));
 	      *pd = safe_realloc(*pd,(PSI.local_size-n)*EL_SIZE);	      
 	      MIMD_Recv(i,PTR(*pd,PSI.local_size),-n*EL_SIZE);
 	      PSI.local_size -= n;
 	    }
 	}
    }
  else
    {
      n = target - PSI.local_size;
      MIMD_Send(TOP_NODE,&n,sizeof(n));
      if (n>0)
 	{
 	  MIMD_Recv(TOP_NODE,&n,sizeof(n));
 	  PSI.local_size += n;
 	  *pd = safe_realloc(*pd,PSI.local_size*EL_SIZE);
 	  MIMD_Recv(TOP_NODE,PTR(*pd,PSI.local_size-n),n*EL_SIZE);
 	}
      if (n<0)
 	{
 	  MIMD_Recv(TOP_NODE,&n,sizeof(n));
 	  PSI.local_size += n;
 	  MIMD_Send(TOP_NODE,PTR(*pd,PSI.local_size),-n*EL_SIZE);
 	  *pd = safe_realloc(*pd,PSI.local_size*EL_SIZE);
 	}
    }  
}


/*******************************************************************
if the data is unbalanced pad it with cell 0's first element
********************************************************************/
void pad_cells(void **pd)
{
int max,sum;
max = MIMD_Max_Int(PSI.local_size);
MIMD_Sum_Int(PSI.local_size,&sum);
if (max == sum/NUM_NODES) return;

PSI.pad_size = max*NUM_NODES - sum;

if (IS_BOTTOM_NODE)
  {
    PSI.padding = (void *)malloc(EL_SIZE);
    COPYELS(PSI.padding,*pd,1);
    MIMD_Broad(PSI.padding,EL_SIZE);
  }
else
  {
    PSI.padding = (void *)malloc(EL_SIZE);
    MIMD_Broad_Recv(BOTTOM_NODE,PSI.padding,EL_SIZE);
  }

if (PSI.local_size == max) return;

*pd = safe_realloc(*pd,max*EL_SIZE);

while (PSI.local_size < max)
  {
    COPYELS(
	    ELEMENT(*pd,PSI.local_size),
	    PSI.padding,1);	    
    PSI.local_size++;
  }
}

/*******************************************************************
unpad the cells after the sort
********************************************************************/
void unpad_cells(void **pd)
{
int i=0,first,total;
int last_cell;

#define EL(i) ((void *)(((char *)*pd) + (i)*EL_SIZE))
if (PSI.pad_size == 0) return;

/* find the padding */
while (i<PSI.local_size && memcmp(EL(i),PSI.padding,EL_SIZE) != 0) i++;

if (i==PSI.local_size) 
  {
    MIMD_Max_Int(0);
    return;
  }

last_cell = MIMD_Max_Int(NODE_NUMBER);
first = i;

/* find where the padding ends */
total=1;
i++;
while (i<PSI.local_size)
  if (memcmp(EL(i++),PSI.padding,EL_SIZE) == 0) total++;

if (NODE_NUMBER == last_cell) total--;

for (i=first;i<PSI.local_size && total>0;i++)
  if (memcmp(EL(i),PSI.padding,EL_SIZE) == 0)
    {
      int j=i+1;
      while (j<PSI.local_size && memcmp(EL(j),PSI.padding,EL_SIZE) == 0) j++;
      if (j-i > total) j--;

      if (j!=PSI.local_size)
	COPYELS(EL(i),EL(j),PSI.local_size-j);

      PSI.local_size -= (j-i);
      total -= (j-i);
    }

*pd = safe_realloc(*pd,PSI.local_size*EL_SIZE);

}


/*******************************************************************
swap memory areas between two cells
********************************************************************/
FN void swap_memory(int node,void *ptr,int to_receive,int to_send)
{
  TS(T_SWAP_MEM);

/* printf("\nnode %d   with %d  to_recv %d   to_send %d\n",NODE_NUMBER,node,to_receive,to_send); */
#ifndef CM5
  {
    int BufSize = 64*(1<<10);
    
    if (NODE_NUMBER < node)
      {
	char *Rptr = ((char *)ptr) + to_receive;
	char *Sptr = ((char *)ptr) + to_send;
#if BLOCKING
	void *buf = malloc(BufSize);
#endif
	int recv,send;
	while (to_receive>0 || to_send>0)
	  {
	    recv = MIN(to_receive,BufSize);
	    send = MIN(to_send,BufSize);
#if BLOCKING
	    if (send) memcpy(buf,Sptr-send,send);
	    if (recv) MIMD_Recv(node,Rptr-recv,recv);
	    if (send) MIMD_Send(node,buf,send);
#else
#if 0
	    if (send) FSend(node,Sptr-send,send);
	    if (recv) FRecv(node,Rptr-recv,recv);
#else
	    if (send) MIMD_Send(node,Sptr-send,send);
	    if (recv) MIMD_Recv(node,Rptr-recv,recv);
#endif
#endif
	    to_receive -= recv;
	    to_send -= send;	    
	    Rptr -= recv;
	    Sptr -= send;	    
	  }
#if BLOCKING
	free(buf);
#endif
      }
    else
      {
	char *Rptr = ((char *)ptr) + to_receive;
	char *Sptr = ((char *)ptr) + to_send;
	int recv,send;
	while (to_receive>0 || to_send>0)
	  {
	    recv = MIN(to_receive,BufSize);
	    send = MIN(to_send,BufSize);
#if 0
	    if (send) FSend(node,Sptr-send,send);
	    if (recv) FRecv(node,Rptr-recv,recv);
#else
	    if (send) MIMD_Send(node,Sptr-send,send);
	    if (recv) MIMD_Recv(node,Rptr-recv,recv);
#endif
	    to_receive -= recv;
	    to_send -= send;
	    Rptr -= recv;
	    Sptr -= send;
	  }
      }
  }
#else
#if 1
  {
    int BufSize = 30*(1<<10);
    char *Cptr = (char *)ptr;
    while (to_receive>0 || to_send>0)
      {
	int recv = MIN(to_receive,BufSize);
	int send = MIN(to_send,BufSize);  
	CMMD_send_and_receive(node,0,Cptr,recv,node,0,Cptr,send);
	to_receive -= recv;
	to_send -= send;
	Cptr += BufSize;
      }
  }
#else
  CMMD_send_and_receive(node,1,ptr,to_receive,node,1,ptr,to_send);
#endif
#endif

  TE(T_SWAP_MEM);
}



/*******************************************************************
simple merge code - must be fast
no boundary checking is done and the number of elements taken from 
list1 is returned
********************************************************************/
FN int SimpleMerge(void *dest,void *src1,void *src2,int n)
{
#if INTEGER
#define DOONE(x) (i1<i2?(x = i1,i1= *(++ss1)):(x = i2,i2= *(++ss2)))
{
  register int N = n;
  int *ss1=src1,*ss2=src2,*d=dest;
  int i1= *ss1,i2= *ss2;
  int d0,d1,d2,d3;
  int d4,d5,d6,d7; 

  TS(T_SIMPLE);

  while (N >= 8)
    {	
      DOONE(d0); DOONE(d1); DOONE(d2); DOONE(d3); 
      DOONE(d4); DOONE(d5); DOONE(d6); DOONE(d7); 
      d[0] = d0;d[1] = d1;d[2] = d2;d[3] = d3;
      d[4] = d4;d[5] = d5;d[6] = d6;d[7] = d7; 
      d+=8;N-=8;
    }

  TE(T_SIMPLE);

  while (N--)
    DOONE(*d++);

  
  return(ss1-(int *)src1);
}
#else
{
  int N = n;
  int elsize = EL_SIZE;
  int ii;
  int (*cmp)() = PSI.comparison;

  TS(T_SIMPLE);

  if (elsize%sizeof(double)==0 && 
      ((int)src1)%sizeof(double)==0 && 
      ((int)src2)%sizeof(double)==0 &&
      ((int)dest)%sizeof(double)==0)
    {
      double *ss1=src1,*ss2=src2,*d=dest;
      elsize /= sizeof(double);
      switch (elsize)
	{
	case 1:
	  while (N--)
	    *d++ = (cmp(ss1,ss2)<0?*ss1++:*ss2++);
	  break;
	case 2:
	  {
	    while (N--)
	      {
		if (cmp(ss1,ss2)<0)
		  {
		    d[0] = ss1[0];d[1] = ss1[1];
		    ss1+=2;
		  }
		else
		  {
		    d[0] = ss2[0];d[1] = ss2[1];
		    ss2+=2;
		  }
		d+=2;
	      }
	  }
	  break;
	default:
	  while (N--)
	    if (cmp(ss1,ss2)<0) 
	      {ii=elsize;while (ii--) *d++ = *ss1++;}
	    else 
	      {ii=elsize;while (ii--) *d++ = *ss2++;}
	  break;
	}
      TE(T_SIMPLE);
      return(((char *)ss1 - (char *)src1)/EL_SIZE);
    }

  if (elsize==sizeof(float) && 
      ((int)src1)%sizeof(float)==0 && 
      ((int)src2)%sizeof(float)==0 &&
      ((int)dest)%sizeof(float)==0)
    {
      float *ss1=src1,*ss2=src2,*d=dest;
      elsize /= sizeof(float);
      while (N--)
	*d++ = (cmp(ss1,ss2)<0?*ss1++:*ss2++);
      TE(T_SIMPLE);
      return(((char *)ss1 - (char *)src1)/EL_SIZE);
    }

    {
      char *ss1=src1,*ss2=src2,*d=dest;
      while (N--)
	{
	  if (cmp(ss1,ss2)<0)
	    {
	      memcpy(d,ss1,elsize);
	      d+=elsize;
	      ss1+=elsize;
	    }
	  else
	    {
	      memcpy(d,ss2,elsize);
	      d+=elsize;
	      ss2+=elsize;
	    }
	}
      TE(T_SIMPLE);
      return(((char *)ss1 - (char *)src1)/EL_SIZE);
    }
}
#endif
}


/*******************************************************************
shift some elements - may overlap!
********************************************************************/
FN void ShiftEls(void *dest,void *src,int N)
{
int i;
int block = ((char *)dest - (char *)src)/EL_SIZE;

if (dest == src) return;

if (block < 0) block = -block;

if ((char *)dest < (char *)src)
  {
    for (i=0;i<N;i+=block)
      COPYELS(PTR(dest,i),PTR(src,i),MIN(block,N-i));
  }
else
  {
    int todo;
    for (i=N;i>0;i-=block)
      {
	todo = MIN(block,i+1);
	COPYELS(PTR(dest,i-todo),PTR(src,i-todo),todo);
      }
  }
}

/*******************************************************************
unbalanced merge of two lists - best when L1 or L2 is small
********************************************************************/
FN void UnbalancedMerge(void *data,int L1,int L2,int gap)
{
  int *slots;
  int N = L1 + L2;
  int smallL = MIN(L1,L2);
  int bigL = MAX(L1,L2);
  void *smallP,*bigP;

  TS(T_SMALL_MERGE);
  
  slots = malloc(sizeof(int)*smallL);
  
  if (L1 < L2) 
    {
      smallP = data;
      bigP = PTR(data,L1+gap);
    }
  else
    {
      bigP = data;
      smallP = PTR(data,L1+gap);
    }
  
  /* create the slots list - this says where each element in 
     smallList will fit into the big list */
  {
    int start=0;
    int i;
    for (i=0;i<smallL;i++)
      {
	int min=start,max=bigL,proposed;
	while (min<max)
	  {
	    proposed = (min+max)/2 + 1;
	    if (COMPARE(PTR(smallP,i),PTR(bigP,proposed-1))<0)
	      max = proposed-1;
	    else
	      min = proposed;
	  }

	slots[i] = min;
	start = min;
      }
  }
  
  /* now fit the elements into the bigger list */
  if (L1 < L2)
    {
      int slot=0;
      int num_taken=0;
      int num_done=0;
      void *TempBuf = malloc(smallL*EL_SIZE);
      COPYELS(TempBuf,smallP,smallL);

      while (slot<smallL)
	{
	  int todo = slots[slot]-num_taken;
	  ShiftEls(PTR(data,num_done),PTR(bigP,num_taken),todo);
	  num_done += todo;
	  num_taken += todo;
	  COPYEL(PTR(data,num_done),PTR(TempBuf,num_done-num_taken));
	  num_done++;
	  slot++;
	}
      if (num_done < N)
	ShiftEls(PTR(data,num_done),PTR(bigP,num_taken),N-num_done);
      free(TempBuf);
    }
  else
    {
      int slot=smallL-1;
      int num_taken=0;
      int num_done=0;
      void *TempBuf = malloc(smallL*EL_SIZE);
      COPYELS(TempBuf,smallP,smallL);

      while (slot>=0)
	{
	  int todo = bigL-slots[slot]-num_taken;
	  ShiftEls(PTR(data,N-num_done-todo),
		   PTR(bigP,bigL-num_taken-todo),
		   todo);
	  num_done += todo;
	  num_taken += todo;
	  COPYEL(PTR(data,N-num_done-1),
		 PTR(TempBuf,smallL-(num_done-num_taken)-1));
	  num_done++;
	  slot--;
	}
      ShiftEls(data,bigP,N-num_done);
      free(TempBuf);
    }


  free(slots);
TE(T_SMALL_MERGE);
}


/*******************************************************************
merge 2 lists into one with minimum memory overhead
********************************************************************/
void insitu_merge(void *data,int L1,int L2,int gap)
{
int N = L1 + L2;
int B;
int bListLen1,bListLen2,bListLen3;
int NumTemp=3;
int TailSize;
BOOL TailDone;
typedef struct
{
  int start,length;
} bListStruct;

bListStruct *bList1,*bList2,*bList3;
void *TailBuf;
void *TempBuf;
double sqrt(double );


#define COPYP(p1,p2) COPYEL(p1,p2)
#define COPYP_INC(p1,p2) COPYEL_INC(p1,p2)
#define COPYPN(p1,p2,n) COPYELS(p1,p2,n)
#define LESSTHANP(p1,p2) (COMPARE(p1,p2)<0)

#if MERGE_TEST
void *TestBuf;
#endif

if (L2==0) return;

if (L1==0 && gap>0)
{
  ShiftEls(data,PTR(data,gap),L2);
  return;
}


#if MERGE_TEST
TestBuf = el_vector(N);
memcpy(TestBuf,data,L1*EL_SIZE);
memcpy(PTR(TestBuf,L1),PTR(data,L1+gap),L2*EL_SIZE);
{
  int i;
  if (COMPARE(PTR(TestBuf,L1-1),PTR(TestBuf,L1))==0)
    {
      printf("\n%d overlap equal problem L1=%d L2=%d gap=%d\n",NODE_NUMBER,L1,L2,gap);
/*      *(char *)0 = 1; */
    }

  for (i=0;i<N-1;i++)
    if (COMPARE(PTR(TestBuf,i),PTR(TestBuf,i+1))==0)
      {
	printf("\n%d equal at %d L1=%d L2=%d gap=%d\n",NODE_NUMBER,i,L1,L2,gap);
/*	*(char *)0 = 1; */
      }
}
qsort(TestBuf,N,EL_SIZE,(QSORT_CAST)PSI.comparison);
{
  int i;
  for (i=0;i<N-1;i++)
    if (COMPARE(PTR(TestBuf,i),PTR(TestBuf,i+1))==0)
      {
	printf("\n%d equal after at %d L1=%d L2=%d gap=%d\n",NODE_NUMBER,i,L1,L2,gap);
/*	*(char *)0 = 1; */
      }
}
#endif

B = MIN(N,(int)sqrt((double)N)*4);
/* B = MIN(B,(128*1024/EL_SIZE)/4); */


#if UNBALANCED_MERGE
if (L1 <= B || L2 <= B)
     {
       UnbalancedMerge(data,L1,L2,gap);
#if MERGE_TEST
       {
	 int i;
	 for (i=0;i<N;i++)
	   if (COMPARE(PTR(data,i),PTR(TestBuf,i)) != 0)
	     {
	       printf("\n%d failed on i=%d  L1=%d  L2=%d  gap=%d\n",NODE_NUMBER,i,L1,L2,gap);
/*	       *((char *)0) = 1; */
	     }
	 free(TestBuf);
       }
#endif
       return;
     }
#endif

TS(T_INSIT1);

/* INITIALISATION: allocate memory */
TempBuf = el_vector(NumTemp*B);

bList1 = (bListStruct *)malloc((L1/B + 2)*sizeof(bListStruct));
bList2 = (bListStruct *)malloc((L2/B + 2)*sizeof(bListStruct));
bList3 = (bListStruct *)malloc((L1/B + L2/B + 5)*sizeof(bListStruct));

TailSize = L1 % B;
TailBuf = el_vector(TailSize);
TailDone = (TailSize == 0);


/* STAGE 1: Create the 2 source block lists and initialise the 3rd */
{
  int L,S,pos;
  bListStruct *list;

  list=bList1; L=L1 - TailSize; S=0; pos=0;
  while (L)
    {
      list[pos].start = S + pos * B;
      list[pos].length = MIN(B,L);
      L -= list[pos].length;
      pos++;
    }
  bListLen1=pos;

  /* copy the tail from list 1 */
  COPYPN(TailBuf,PTR(data,bListLen1*B),TailSize);

  list=bList2; L=L2; S=L1+gap; pos=0;
  while (L)
    {
      list[pos].start = S + pos * B;
      list[pos].length = MIN(B,L);
      L -= list[pos].length;
      pos++;
    }
  bListLen2=pos;

  bListLen3=0;
}

TE(T_INSIT1);
TS(T_INSIT2);

/* STAGE 2: merge the two lists into list3 */
{
  void *pL1,*pL2,*pL3;
  int avail1,avail2;
  int space3;
  int sPos1,sPos2;
  int availPos3,dPos3;
  BOOL empty1,empty2;

  /* select the first blocks in list 1 and 2 */
  if (bListLen1 > 0)
    {
      pL1 = PTR(data,bList1[0].start);
      avail1 = bList1[0].length;
      sPos1 = 0;
      empty1 = False;
    }
  else
    {
      pL1 = TailBuf;
      avail1 = TailSize;
      TailDone = True;
      sPos1 = 0;
      empty1 = False;
    }

  pL2 = PTR(data,bList2[0].start);
  avail2 = bList2[0].length;
  sPos2 = 0;
  empty2 = False;

  /* initially select the TempBuf */
  bList3[0].start = -1;
  bList3[0].length = NumTemp*B;
  availPos3 = 1;
  pL3 = TempBuf;
  space3 = NumTemp*B;
  dPos3 = 0;
  

  while (!empty1 || !empty2)
    {
      int MinToDo = space3;
      if (!empty1) MinToDo = MIN(MinToDo,avail1);
      if (!empty2) MinToDo = MIN(MinToDo,avail2);

      space3 -= MinToDo;
      if (empty2 || empty1)
	{
	  if (empty1)
	    {
	      COPYPN(pL3,pL2,MinToDo);
	      pL2 = PTR(pL2,MinToDo);
	      avail2 -= MinToDo;
	    }
	  else
	    {
	      COPYPN(pL3,pL1,MinToDo);
	      pL1 = PTR(pL1,MinToDo);
	      avail1 -= MinToDo;
	    }
	  pL3 = PTR(pL3,MinToDo);
	}
      else
	{
	  int FrompL1 = SimpleMerge(pL3,pL1,pL2,MinToDo);
	  avail1 -= FrompL1;
	  avail2 -= (MinToDo-FrompL1);
	  pL3 = PTR(pL3,MinToDo);
	  pL1 = PTR(pL1,FrompL1);
	  pL2 = PTR(pL2,MinToDo-FrompL1);
	}

      /* have I emptied list 1 ? */
      if (avail1==0 && !empty1)
	{
	  /* make the just emptied block available */
	  if (TailSize == 0 || !TailDone)
	    {
/*	      int k = dPos3+1; */
	      int k = availPos3;

	      memcpy(&bList3[k+1],&bList3[k],(availPos3-k)*sizeof(bList3[k]));

	      bList3[k].start = bList1[sPos1].start;
	      bList3[k].length = bList1[sPos1].length;
	      availPos3++;
	      sPos1++;
	    }
	  
	  /* are there no blocks left ? */
	  if (sPos1 == bListLen1) 
	    {
	      if (!TailDone)
		{
		  pL1 = TailBuf;
		  avail1 = TailSize;
		  TailDone = True;
		}
	      else
		empty1 = True;
	    }
	  else
	    {
	      /* start using the next block */
	      pL1 = PTR(data,bList1[sPos1].start);
	      avail1 = bList1[sPos1].length;
	    }
	}
      
      /* have I emptied list 2 ? */
      if (avail2==0 && !empty2)
	{
/*	  int k = dPos3+1; */
	  int k = availPos3;

	  memcpy(&bList3[k+1],&bList3[k],(availPos3 - k)*sizeof(bList3[k]));

	  /* make the just emptied block available */	
	  bList3[k].start = bList2[sPos2].start - (TailSize + gap);
	  bList3[k].length = bList2[sPos2].length;	
	  if (sPos2 == (bListLen2-1))
	    bList3[k].length += TailSize;
	  if (bList3[k].length > B)
	    {
	      bList3[availPos3+1].length = bList3[k].length - B;
	      bList3[availPos3+1].start = bList3[k].start + B;
	      bList3[k].length = B;
	      availPos3++;
	    }		
  
	  /* don't make odd sized blocks available */
	  if (bList3[availPos3].length == B)
	    availPos3++;
	  sPos2++;

	  /* are there no blocks left ? */
	  if (sPos2 == bListLen2) 
	    empty2 = True;
	  else
	    {
	      /* start using the next block */
	      pL2 = PTR(data,bList2[sPos2].start);
	      avail2 = bList2[sPos2].length;
	    }
	}

      /* have I filled list 3 ? */
      if (space3 == 0)
	{
	  /* start using the next block */
	  dPos3++;
	  if (dPos3 == availPos3)
	    printf("\n%d merge error - dPos3==availPos3! %d\n",NODE_NUMBER,dPos3);
	  pL3 = PTR(data,bList3[dPos3].start);
	  space3 = bList3[dPos3].length;
	}
    }

  /* we may not have filled list 3 completely */
  bList3[dPos3].length -= space3;

  /* move the last elements to the last block in list 2 */
  COPYPN(PTR(data,N-bList3[dPos3].length),
	 PTR(pL3,-bList3[dPos3].length),
	 bList3[dPos3].length);
  bList3[dPos3].start = N - bList3[dPos3].length;

  bListLen3 = dPos3;
}
TE(T_INSIT2);
TS(T_INSIT3);
/* STAGE 3: re-arrange to give final result */
{
  typedef struct
    {
      int where_hiding;
      int what_now;
      BOOL occupied;
    } block_struct;
  block_struct *blocks;
  int num_blocks = N / B;
  int i;
  BOOL all_done = False;

  blocks = (block_struct *)malloc(num_blocks * sizeof(block_struct));
  for (i=0;i<num_blocks;i++)
    blocks[i].occupied = False;
  /* the temp blocks first */
  for (i=0;i<MIN(num_blocks,NumTemp);i++)
    blocks[i].where_hiding = -(i+1);
  for (i=1;i<bListLen3;i++)
    {
      blocks[i-1+NumTemp].where_hiding = bList3[i].start/B;
      blocks[blocks[i-1+NumTemp].where_hiding].what_now = i-1+NumTemp;
      blocks[blocks[i-1+NumTemp].where_hiding].occupied = True;
    }
  
  while (!all_done)
    {
      BOOL no_holes = False;
      all_done = True;
      while (!no_holes)
	{
	  no_holes = True;
	  for (i=0;i<num_blocks;i++)
	    {
	      while (!blocks[i].occupied)
		{
		  int from = blocks[i].where_hiding;
		  no_holes = False;
		  if (from >= 0)
		    {
		      COPYPN(PTR(data,i*B),PTR(data,from*B),B);
		      blocks[from].occupied = False;
		    }
		  else
		    COPYPN(PTR(data,i*B),PTR(TempBuf,-(from+1)*B),B);
		  
		  blocks[i].occupied = True;
		  blocks[i].what_now = i;
		  blocks[i].where_hiding = i;		  
		  if (from >= 0)
		    i = from;
		}
	    }
	}
      for (i=0;i<num_blocks && all_done;i++)
	if (blocks[i].what_now != i)
	  {
	    all_done = False;
	    COPYPN(TempBuf,PTR(data,i*B),B);
	    blocks[i].occupied = False;
	    blocks[blocks[i].what_now].where_hiding = -1;	    
	  }
    }
  free(blocks);
}
TE(T_INSIT3);
free(TempBuf);
free(bList1);
free(bList2);
free(bList3);
if (TailBuf) free(TailBuf);
#if MERGE_TEST
       {
	 int i;
	 for (i=0;i<N;i++)
	   if (COMPARE(PTR(data,i),PTR(TestBuf,i)) != 0)
	     {
	       printf("\n%d failed on i=%d  L1=%d  L2=%d  gap=%d\n",NODE_NUMBER,i,L1,L2,gap);
/*	       *(char *)0 = 1; */
	     }
	 free(TestBuf);
       }
#endif
}


/*******************************************************************
find the last element of p1 where comparison < 0
********************************************************************/
int find_last_change(char *p1,char *p2,int N1,int N2,int limit)
{
if (N1==1) return(1);
/* TS(T_FIND); */

p2 += (N2-1)*EL_SIZE;
limit = MIN(limit,N1);

#if FAST_MERGE
#define NEXTTRIAL minc
{
  int minc=1,trial=1,maxc=limit,step=1;
  while (maxc>minc)
    {
      trial = MIN(trial,maxc-1);
      if (COMPARE(ELEMENT(p1,N1-1-trial),p2)<0)
	{
	  maxc=trial;
	  trial=NEXTTRIAL;
	  step=1;
	}
      else
	{
	  minc=trial+1;
	  trial+=step;
	  step*=2;
	}
    }
/*  TE(T_FIND); */
  return(minc);
}
#else
{
int count;
for (count=1;count<limit;count++)
  if (COMPARE(ELEMENT(p1,N1-1-count),p2)<0) 
    {
/*      TE(T_FIND); */
      return(count);
    }
/* TE(T_FIND); */
return(limit);
}
#endif
}

/*******************************************************************
find the first element of p1 where comparison > 0
********************************************************************/
int find_first_change(char *p1,char *p2,int N1,int N2,int limit)
{
if (N1==1) return(1);
/* TS(T_FIND); */
limit = MIN(limit,N1);

#if FAST_MERGE
{
  int minc=1,trial=1,maxc=limit,step=1;
  while (maxc>minc)
    {
      trial = MIN(trial,maxc-1);
      if (COMPARE(ELEMENT(p1,trial),p2)>0)
	{
	  maxc=trial;
	  trial=NEXTTRIAL;
	  step=1;
	}
      else
	{
	  minc=trial+1;
	  trial+=step;
	  step*=2;
	}
    }
/*   TE(T_FIND); */
  return(minc);
}
#else
{
int count;
for (count=1;count<limit;count++)
  if (COMPARE(ELEMENT(p1,count),p2)>0) 
    {
/*      TE(T_FIND); */
      return(count);
    }
/* TE(T_FIND); */
return(limit);
}
#endif
}


/*******************************************************************
merge together 2 sorted data sets - d1 ends up holding the top N3 values 
********************************************************************/
void merge_sort_top(void **d1,void *d2,int N1,int N2,int N3)
{
#if SIMPLE
void *d3 = malloc((N1+N2)*EL_SIZE);
memcpy(d3,*d1,N1*EL_SIZE);
memcpy(PTR(d3,N1),d2,N2*EL_SIZE);
qsort(d3,N1+N2,EL_SIZE,PSI.comparison);
*d1 = safe_realloc(*d1,N3*EL_SIZE);
memcpy(*d1,d3,N3*EL_SIZE);
if (d3) free(d3);
#else

void *d3;
int n1=0,n2=0,n3=0;

BOOL do_p1=False;

TS(T_MERGE);

if ((n3<N3) && (n1<N1) && (n2<N2))
  do_p1 = (COMPARE(*d1,d2)<0);

d3 = expand_pointer(&PSI.data1,&PSI.malloc_size1,EL_SIZE*N3);

while ((n3<N3) && (n1<N1) && (n2<N2))
  if (do_p1)
    {
      int count=find_first_change(PTR(*d1,n1),PTR(d2,n2),N1-n1,N2-n2,
				  N3-n3);
      COPYELS(PTR(d3,n3),PTR(*d1,n1),count);
      n1 += count;
      n3 += count;
      do_p1 = !do_p1;
    }
  else
    {
      int count=find_first_change(PTR(d2,n2),PTR(*d1,n1),N2-n2,N1-n1,
				  N3-n3);
      COPYELS(PTR(d3,n3),PTR(d2,n2),count);
      n2 += count;
      n3 += count;
      do_p1 = !do_p1;
    }

if (n3 < N3)
  {
   if (n1<N1) COPYELS(PTR(d3,n3),PTR(*d1,n1),N3-n3);
   if (n2<N2) COPYELS(PTR(d3,n3),PTR(d2,n2),N3-n3);
  }

{
  void *oldp = *d1;
  *d1 = d3;
  PSI.data1=NULL;
  PSI.malloc_size1=0;
  free(oldp);
}
TE(T_MERGE);
#endif
}


/*******************************************************************
merge together 2 sorted data sets - d2 ends up holding the bottom N3 values 
********************************************************************/
void merge_sort_bottom(void *d1,void **d2,int N1,int N2,int N3)
{
#if SIMPLE
void *d3 = malloc((N1+N2)*EL_SIZE);
memcpy(d3,d1,N1*EL_SIZE);
memcpy(PTR(d3,N1),*d2,N2*EL_SIZE);
qsort(d3,N1+N2,EL_SIZE,PSI.comparison);
*d2 = safe_realloc(*d2,N3*EL_SIZE);
memcpy(*d2,PTR(d3,(N1+N2)-N3),N3*EL_SIZE);
if (d3) free(d3);
#else

void *d3;
int n1=N1,n2=N2,n3=N3;

BOOL do_p1 = False;

TS(T_MERGE);

if ((n3>0) && (n1>0) && (n2>0))
  do_p1 = (COMPARE(ELEMENT(d1,n1-1),ELEMENT(*d2,n2-1))>0);

d3 = expand_pointer(&PSI.data1,&PSI.malloc_size1,EL_SIZE*N3);

while ((n3>0) && (n1>0) && (n2>0))
  if (do_p1)
    {
      int count=find_last_change(d1,*d2,n1,n2,n3); 
      COPYELS(PTR(d3,n3-count),PTR(d1,n1-count),count);
      n1 -= count;
      n3 -= count;
      do_p1 = !do_p1;
    }
  else
    {
      int count=find_last_change(*d2,d1,n2,n1,n3); 
      COPYELS(PTR(d3,n3-count),PTR(*d2,n2-count),count);
      n2 -= count;
      n3 -= count;
      do_p1 = !do_p1;
    }

if (n3 > 0)
  {
    if (n1>0) COPYELS(d3,PTR(d1,n1-n3),n3);
    if (n2>0) COPYELS(d3,PTR(*d2,n2-n3),n3);
  }

{
  void *oldp = *d2;
  *d2 = d3;
  PSI.data1=NULL;
  PSI.malloc_size1=0;
  free(oldp);
}
TE(T_MERGE);
#endif
}

#if !SLICED
/*******************************************************************
a 2 node version of batcher_makesmaller with dynamic balancing
********************************************************************/
void batcher_makesmaller(void **data,int n1,int n2)
{
void *other_data;
int other_node = (NODE_NUMBER==n1?n2:n1);
int other_size;  /* other guys data size */
int new_local_size,new_other_size,total_size;
int to_send,to_receive;

/* node numbers might not start at 0 */
n1 += BOTTOM_NODE;
n2 += BOTTOM_NODE;


/* only do work if n1==NODE_NUMBER || n2=NODE_NUMBER */
if (n1 != NODE_NUMBER && n2 != NODE_NUMBER) return;

/* find out each others size */
#if ASSUME_BALANCED
other_size = PSI.local_size;
#else
MM_exchange_data(other_node,&PSI.local_size,&other_size,sizeof(int),sizeof(int));
#endif

/* balance the data */
total_size = PSI.local_size + other_size;
if (total_size==0) return;

#if !INFINITY_PADDING
#if BALANCE
new_other_size = (total_size+(NODE_NUMBER<other_node?0:1))/2;
new_local_size = total_size - new_other_size;
#else
new_local_size = PSI.local_size;
new_other_size = other_size;
#endif
#else
if (NODE_NUMBER == n1)
  {
    new_local_size = MIN(total_size,PSI.max_size);
    new_other_size = total_size - new_local_size;
  }
else
  {
    new_other_size = MIN(total_size,PSI.max_size);
    new_local_size = total_size - new_other_size;
  }
#endif

to_receive = other_size;
to_send = PSI.local_size;

#if FIND_EXACT
TS(T_FIND_EXACT);
if (NODE_NUMBER == n2)
  {
    int proposed=1;
    int N = new_other_size;
    while (proposed>0)
      {
	MIMD_Recv(n1,&proposed,sizeof(int));
	if (proposed > 0)
	  MIMD_Send(n1,PTR(*data,N-proposed),EL_SIZE);
      }
    to_receive = other_size + proposed;
    to_send = new_other_size - other_size + to_receive;
  }
else
  {
    int N = new_local_size,L1 = PSI.local_size,L2 = other_size;
    int min=MAX(N-L2,0),max=MIN(L1,N),proposed;
    void *TempEl;
    TempEl = malloc(EL_SIZE);
    while (min < max)
      {
	proposed = (min+max)/2 + 1;
	MIMD_Send(n2,&proposed,sizeof(int));
	MIMD_Recv(n2,TempEl,EL_SIZE);
	if (COMPARE(PTR(*data,proposed-1),TempEl) > 0)
	  max = proposed - 1;
	else
	  min = proposed;
      }
    free(TempEl);
    to_send = PSI.local_size - min;
    to_receive = new_local_size - PSI.local_size + to_send;
    proposed = -min;
    MIMD_Send(n2,&proposed,sizeof(int));
  }

TE(T_FIND_EXACT);
#endif
#if !FIND_EXACT
/* allocate some memory to hold the other data - if necessary */
other_data = expand_pointer(&PSI.data2,&PSI.malloc_size2,EL_SIZE*to_receive);

if (NODE_NUMBER == n1)
  MM_exchange_data(other_node,PTR(*data,PSI.local_size-to_send),other_data,
		   to_send*EL_SIZE,
		   to_receive*EL_SIZE);
else
  MM_exchange_data(other_node,*data,other_data,
		   to_send*EL_SIZE,
		   to_receive*EL_SIZE);

if (NODE_NUMBER == n1)
  {
    merge_sort_top(data,other_data,PSI.local_size,to_receive,new_local_size,
		   EL_SIZE);
#if STATS
    stats.messages += 4;
#endif
  }
else
  {
    merge_sort_bottom(other_data,data,to_receive,PSI.local_size,new_local_size,
		      EL_SIZE);
  }
#else

if (new_local_size > PSI.local_size)
  {
    int local_size = PSI.local_size*EL_SIZE;
    *data = expand_pointer(data,&local_size,new_local_size*EL_SIZE);
  }

if (NODE_NUMBER == n1)
  swap_memory(other_node,PTR(*data,PSI.local_size-to_send),
	      to_receive*EL_SIZE,to_send*EL_SIZE);
else
  swap_memory(other_node,*data,
	      to_receive*EL_SIZE,to_send*EL_SIZE);

if (NODE_NUMBER == n1)
  insitu_merge(*data,new_local_size-to_receive,to_receive,0); 
else
  insitu_merge(*data,to_receive,new_local_size-to_receive,to_send-to_receive); 


#endif
PSI.local_size = new_local_size;

}
#else
/*******************************************************************
find how many slices of dat2 are needed for the merge
********************************************************************/
FN int find_last_slice(void *slices1,void *slices2,int hunk1,int hunk2,int size_needed,int num_slices,int tail1,int tail2)
{
int pos1=0,pos2=0;
#define SL(d,i) ELEMENT(d,i)

if (size_needed<=((num_slices*hunk1)+tail1) && 
    COMPARE(SL(slices1,num_slices),SL(slices2,0)) <= 0)
  return(0);

while (size_needed > 0 && pos1<num_slices && pos2<num_slices)
  {
    if (COMPARE(SL(slices1,pos1),SL(slices2,pos2)) <= 0)
      {
	pos1++;
	size_needed -= hunk1;
      }
    else
      {
	pos2++;
	size_needed -= hunk2;
      }
  }
if (pos1 == num_slices && size_needed>0)
  pos2 += size_needed / hunk2;

return(MIN(pos2+1,num_slices));
}

/*******************************************************************
find how many slices of dat2 are needed for the merge
********************************************************************/
FN int find_first_slice(void *slices1,void *slices2,int hunk1,int hunk2,int size_needed,int num_slices,int tail1,int tail2)
{
int pos1=num_slices,pos2=num_slices;
#define SL(d,i) ELEMENT(d,i)

if (size_needed<=((num_slices*hunk1)+tail1) && 
    COMPARE(SL(slices1,0),SL(slices2,num_slices)) >= 0)
  return(num_slices);

while (size_needed > 0 && pos1>0 && pos2>0)
  {
    if (COMPARE(SL(slices1,pos1-1),SL(slices2,pos2-1)) >= 0)
      {
	pos1--;
	size_needed -= hunk1;
      }
    else
      {
	pos2--;
	size_needed -= hunk2;
      }
  }

if (pos1 == 0 && size_needed>0)
  pos2 -= size_needed / hunk2;

return(MAX(pos2-1,0));
}



/*******************************************************************
a 2 node version of batcher_makesmaller with dynamic balancing
and slicing
********************************************************************/
void batcher_makesmaller(void **data,int n1,int n2)
{
#define SLICES 256
static void *sliced_data=NULL,*local_slices=NULL;
void *other_data;
int other_node = (NODE_NUMBER==n1?n2:n1);
int other_size;  /* other guys data size */
int new_local_size,new_other_size,total_size;
static int slice_size=0;
int first_slice,last_slice;
int other_first_slice,other_last_slice;
int hunk1 = PSI.local_size/SLICES;
int hunk2;
int i;
int other_size2,local_size2;
BOOL slicing;

/* node numbers might not start at 0 */
n1 += BOTTOM_NODE;
n2 += BOTTOM_NODE;

/* only do work if n1==NODE_NUMBER || n2=NODE_NUMBER */
if (n1 != NODE_NUMBER && n2 != NODE_NUMBER) return;

TS(T_MAKESMALLER);

/* find out each others size */
#if ASSUME_BALANCED
other_size = PSI.local_size;
#else
MM_exchange_data(other_node,&PSI.local_size,&other_size,sizeof(int),sizeof(int));
#endif

hunk2 = other_size/SLICES;

slicing = (hunk1>0 && hunk2>0);

/* allocate sliced data */
if (slicing && slice_size != EL_SIZE)
  {
    slice_size = EL_SIZE;
    if (sliced_data) free(sliced_data);
    if (local_slices) free(local_slices);
    sliced_data = malloc(EL_SIZE*(SLICES+1));
    local_slices = malloc(EL_SIZE*(SLICES+1));
  }

/* balance the data */
total_size = PSI.local_size + other_size;
if (total_size==0) return;

#if !INFINITY_PADDING
#if BALANCE
new_other_size = (total_size+(NODE_NUMBER<other_node?0:1))/2;
new_local_size = total_size - new_other_size;
#else
new_local_size = PSI.local_size;
new_other_size = other_size;
#endif
#else
if (NODE_NUMBER == n1)
  {
    new_local_size = MIN(total_size,PSI.max_size);
    new_other_size = total_size - new_local_size;
  }
else
  {
    new_other_size = MIN(total_size,PSI.max_size);
    new_local_size = total_size - new_other_size;
  }
#endif


/* make up the local slices */
if (slicing)
for (i=0;i<SLICES;i++)
  COPYELS(ELEMENT(local_slices,i),
	 ELEMENT(*data,i*hunk1),
	 EL_SIZE,1);

COPYELS(ELEMENT(local_slices,SLICES),
	ELEMENT(*data,PSI.local_size-1),
	EL_SIZE,1);

/* exchange slices */
if (slicing)
MM_exchange_data(other_node,local_slices,sliced_data,
		 (SLICES+1)*EL_SIZE,
		 (SLICES+1)*EL_SIZE);


/* decide on the first and last slice needed */
first_slice=0;
last_slice = SLICES;
other_first_slice = 0;
other_last_slice = SLICES;


if (slicing)
  {
    int tail1 = PSI.local_size % SLICES;
    int tail2 = other_size % SLICES;
    if (NODE_NUMBER == n1)
      {
	last_slice = find_last_slice(local_slices,sliced_data,hunk1,
				     hunk2,new_local_size,
				     SLICES,tail1,tail2);
	other_first_slice = find_first_slice(sliced_data,local_slices,hunk2,
				     hunk1,new_other_size,
				     SLICES,tail2,tail1);
      }
    else
      {
	other_last_slice = find_last_slice(sliced_data,local_slices,hunk2,
					   hunk1,new_other_size,
					   EL_SIZE,SLICES,tail2,tail1);
	first_slice = find_first_slice(local_slices,sliced_data,hunk1,
				       hunk2,new_local_size,
				       EL_SIZE,SLICES,tail1,tail2);
      }
  }

if (first_slice == last_slice)
  other_size2 = 0;
else
  other_size2 = (last_slice==SLICES?
		 (other_size-first_slice*hunk2):
		 (last_slice-first_slice)*hunk2);

if (other_first_slice == other_last_slice)
  local_size2 = 0;
else
  local_size2 = (other_last_slice==SLICES?
		 (PSI.local_size-other_first_slice*hunk1):
		 (other_last_slice-other_first_slice)*hunk1);


/* allocate some memory to hold the other data - if necessary */
other_data = expand_pointer(&PSI.data2,&PSI.malloc_size2,EL_SIZE*other_size2);

/* exchange the actual data */
MM_exchange_data(other_node,
		 ELEMENT(*data,other_first_slice*hunk1),
		 other_data,
		 local_size2*EL_SIZE,
		 other_size2*EL_SIZE);


if (NODE_NUMBER == n1)
  {
    if (other_size2 > 0)
      merge_sort_top(data,other_data,PSI.local_size,
			  other_size2,new_local_size,
			  EL_SIZE);
#if STATS
    stats.messages += 4;
#endif
  }
else
  {
    if (other_size2 > 0)
      merge_sort_bottom(other_data,data,other_size2,
			     PSI.local_size,new_local_size,
			     EL_SIZE);
  }

PSI.local_size = new_local_size;
TE(T_MAKESMALLER);
}
#endif

/*******************************************************************
guarantee sorted - a very slow sorting method that always works. It is
used to sort an almost sorted data set, and is fast if the data set
is nearly sorted. This could probably be made much faster.

It works by looping until all elements in this node are less than the
node above and greater than the node below. makesmaller() is called to
fix up cases that are not sorted. Once all nodes indicate that they
did nothing at the last step this node will exit.
********************************************************************/
void guarantee_sorted(void **ptr,void (*makesmaller)())
{
void *data;
int done=0;
int sum_done;
void *el1=NULL,*el2=NULL,*d1=NULL,*d2=NULL;
void *last_el;
int size1=0,size2=0;
int g_loops=0;

el1 = (void *)malloc(EL_SIZE);
el2 = (void *)malloc(EL_SIZE);
d1 = (void *)malloc(EL_SIZE);
d2 = (void *)malloc(EL_SIZE);

if (!el1 || !el2 || !d1 || !d2) 
  MM_Panic("memory error in guarantee_sort\n");

while (!done)
  {
    data = *ptr;

    last_el = ((char *)data) + (PSI.local_size-1)*EL_SIZE;

    MM_share_with_neighbours(&PSI.local_size,&PSI.local_size,&size2,&size1,
			     sizeof(int),sizeof(int),sizeof(int),sizeof(int));
    MM_share_with_neighbours(last_el,data,el2,el1,
			     (PSI.local_size>0?EL_SIZE:0),
			     (PSI.local_size>0?EL_SIZE:0),
			     (size2>0?EL_SIZE:0),
			     (size1>0?EL_SIZE:0));
    done=1;
    
    if (PSI.local_size>0)
      {
	COPYELS(d1,data,1);
	COPYELS(d2,last_el,1);
      }

    if (NODE_NUMBER%2)
      {
	if (!IS_BOTTOM_NODE && PSI.local_size>0 && 
	    (size1==0 || COMPARE(el1,d1)>0))
	  {done=0;makesmaller(ptr,NODE_NUMBER-1,NODE_NUMBER);}
	if (!IS_TOP_NODE && size2>0 && 
	    (PSI.local_size==0 || COMPARE(d2,el2)>0))
	  {done=0;makesmaller(ptr,NODE_NUMBER,NODE_NUMBER+1);}
      }
    else
      {
	if (!IS_TOP_NODE && size2>0 && 
	    (PSI.local_size==0 || COMPARE(d2,el2)>0))
	  {done=0;makesmaller(ptr,NODE_NUMBER,NODE_NUMBER+1);}
	if (!IS_BOTTOM_NODE && PSI.local_size>0 && 
	    (size1==0 || COMPARE(el1,d1)>0))
	  {done=0;makesmaller(ptr,NODE_NUMBER-1,NODE_NUMBER);}
      }

    MIMD_Sum_Int(done,&sum_done);
    done = (sum_done==NUM_NODES);
    if (!done) g_loops++;

if (IS_BOTTOM_NODE && !done && g_loops>0)
  printf(".");

if (IS_BOTTOM_NODE && done && g_loops>0)
  printf("needed %d guarantee loops!\n",g_loops);
#if STATS
    if (!done)
      stats.num_guarantee_loops++;
#endif

  }
if (el1) free(el1);
if (el2) free(el2);
if (d1) free(d1);
if (d2) free(d2);
}


/*******************************************************************
sort some data on a parallel machine
*data is a pointer to this nodes portion of the data
N is the number of elements on this node
EL_SIZE is the size in bytes of each element
comparison is a user_supplied function. It should take pointers to 2 
elements and return:
    -1 if el1 < el2
     0 if el1 = el2
     1 if el1 > el2

the number of elements in this node is returned, after dynamic balancing
********************************************************************/
int parallel_sort(void **data,int N,int el_size,int (*comparison)())
{
TS(T_ALL);
PSI.comparison = comparison;
PSI.local_size = N;
PSI.el_size = el_size;

PSI.data1 = NULL;
PSI.data2 = NULL;
PSI.padding = NULL;
PSI.pad_size = 0;

DBUG("starting");

#if STATS
stats.messages = 0;
stats.num_guarantee_loops = 0;
{
  int i;
  for (i=0;i<T_NONE;i++)
    ts(i).cum_time = 0.0;
}
#endif

TS(T_HYPER);
/* pre-balance the cells */
#if HYPER_BALANCE
hypercube_balance(0,NUM_NODES,data);
#endif

#if COMPLETE_BALANCE
complete_balance(data);
#endif

TE(T_HYPER);

/* pad the cells */
#if PAD_CELLS
pad_cells(data);
#endif


#if INFINITY_PADDING
PSI.max_size = MIMD_Max_Int(PSI.local_size);
#endif

/* sort the local node */
TS(T_QSORT);
if (PSI.local_size > 1)
     qsort(*data,PSI.local_size,EL_SIZE,(QSORT_CAST)PSI.comparison); 

TE(T_QSORT);

#if HYPER_SORT
TS(T_HYPER_SORT);
hyper_sort(BOTTOM_NODE,data,NUM_NODES,batcher_makesmaller);
TE(T_HYPER_SORT);
#endif

#if BATCHERS
TS(T_BATCHERS_SORT);
batchers_sort(data,NUM_NODES,batcher_makesmaller);
TE(T_BATCHERS_SORT);
#endif

#if BITONIC
bitonic_sort(data,NUM_NODES,batcher_makesmaller);
#endif

#if GUARANTEE_SORT
guarantee_sorted(data,batcher_makesmaller);
#endif

/* unpad the cells */
#if PAD_CELLS
unpad_cells(data);
#endif


/* If I allocated anything then free it */
expand_pointer(&PSI.data1,&PSI.malloc_size1,0);
expand_pointer(&PSI.data2,&PSI.malloc_size2,0);

/* return the number of items in this node */
TE(T_ALL);

DBUG("ending");

return(PSI.local_size);
}
#endif

#if STATS
/*******************************************************************
report the timing stats
********************************************************************/
FN void report_stats(int N)
{
  int i;
  double times[T_NONE];
#if HOST
  MIMD_Recv(BOTTOM_NODE,times,sizeof(double)*T_NONE);
  for (i=0;i<T_NONE;i++)
    printf("%s %g (%.2f%%)\n",
	   timer_labels[i],
	   times[i]*1000.0/NUM_NODES,
	   100.0*times[i]/times[T_ALL]);
  {
    int P = NUM_NODES;
    double log(double );
    double theory = times[T_QSORT] * log((double)N) / log((1.0*N)/P);
    double fit1 = 
      (0.161169 + 4.71084e-6*N - 0.0212139*log(N) + 2.56064e-6*N*log(N))/P;
    double fit2 =
      (0.0144046 - 1.70054e-6*N - 0.00174052*log(N) + 1.58039e-6*N*log(N))/P;
    
    printf("theory=%g  fit=(%g,%g) speedup=(%g,%g)\n",
	   theory*1000/P,
	   1000*fit1,1000*fit2,
	   P*fit1/(times[T_ALL]/P),
	   P*fit2/(times[T_ALL]/P));
  }
#else
  for (i=0;i<T_NONE;i++)
    MIMD_Sum_Float(timers_struct[i].cum_time,&times[i]);
  if (IS_BOTTOM_NODE)
    MIMD_Send_To_Host(times,sizeof(double)*T_NONE);
#endif
}
#endif
