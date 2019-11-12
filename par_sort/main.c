/* simple calling routines to test parallel_sort() routine */


#include	<stdio.h>
#include	<malloc.h>
#include	<signal.h>
#include        <sys/timeb.h>
#include "mimd.h"

#if CAP && HOST
#include <sys/time.h>
#endif

#define SAVE_SORT 0
#define SINGLE_TEST 0
#define SORT_TEST 0
#define BALANCED 1
#define CHECKSUM 1

/* shall I keeps stats on the sort ? */
#ifndef STATS
#define STATS 0
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

typedef char string[16];
int strcmp();

#if STRING
#define TYPE string
#define comparison strcmp
#else
#define TYPE int
#define comparison int_comparison
#endif

#define ELSIZE sizeof(TYPE)



#ifdef __GNUC__
int ftime();
int printf();
int srandom();
int random();
int fprintf();
char *getenv();
int atoi();
int fflush();
#endif



struct timeb tp1,tp2;
/*******************************************************************
reset the program timer
********************************************************************/
void reset_timer(void)
{
ftime(&tp1);
}

/*******************************************************************
write the time elapsed since the last reset timer
********************************************************************/
void report_timer(void)
{
ftime(&tp2);
printf("elapsed = %d ms\n",(int)(1000*(tp2.time - tp1.time) + (tp2.millitm - tp1.millitm)));
}


/*******************************************************************
comparison function for integer sort
********************************************************************/
int int_comparison(int *i1,int *i2)
{
return(*i1 - *i2);
}

/*******************************************************************
seed random number generator
********************************************************************/
void start_random(void)
{
srandom(time(NULL)*getpid());
}


/*******************************************************************
generate some values for sort testing
********************************************************************/
int generate_values(int seed,int *pN,TYPE **pd)
{
  int i;
  int N;

  srandom(seed*(NUM_NODES + 1) + NODE_NUMBER*131);
  for (i=0;i<NODE_NUMBER;i++) random();
  
  /* *pN *= 1+ random()%10; 
     (*pN) += (random()%(2*NUM_NODES)) - NUM_NODES;
     (*pN) = MAX(*pN,0);
     */

#if !BALANCED
  *pN = random() % (2 * (*pN)); 
#endif

  N = *pN;

  if (N>0)
    *pd = (TYPE *)malloc(ELSIZE*N);

  if (!(*pd))
    {
      /* couldn't alloc - return error */
      return 1;
    }

#if STRING
  {
    string *d = *pd;
    for (i=0;i<N;i++)
      {
	int r;
	char *rc = (char *)&r;
	char *p = &d[i][0];
	int j=ELSIZE;
	int k;
	while (j>=sizeof(int))
	  {
	    r = random();
	    for (k=0;k<sizeof(int);k++)
	      p[k] = 'a' + rc[k]%26;
	    j -= sizeof(int);
	    p += sizeof(int);
	  }
	while (j--)
	  *p++ = 'a' + (char)(random()%26);
	p[-1] = 0;
      }
  }  
#else
  {
    int *d = *pd;
    for (i=0;i<N;i++)
      d[i] = random();
  }
#endif

/* everything ok - return 0 */
  return 0;
}


/*******************************************************************
gather some integers and write to disk
********************************************************************/
void gather_write(char *fname,TYPE *d,int N)
{
#if HOST
  int fclose();
  FILE *fopen();
  int N1;
  int i,j;
  TYPE *data=(TYPE *)malloc(ELSIZE);
  FILE *file = fopen(fname,"w");
  for (i=0;i<NUM_NODES;i++)
    {
      MIMD_Recv(i,&N1,sizeof(N1));
      data = (TYPE *)realloc(data,ELSIZE*N1);
      MIMD_Recv(i,data,ELSIZE*N1);
      for (j=0;j<N1;j++)
#if STRING
	fprintf(file,"%s\n",data[j]);
#else
	fprintf(file,"%d\n",data[j]);
#endif
    }
fclose(file);
free(data);
#else
MIMD_Send_To_Host(&N,sizeof(N));
MIMD_Send_To_Host(d,ELSIZE*N);
#endif
}

/*******************************************************************
test whether sorted correctly
********************************************************************/
void sort_testit(TYPE *d,int N0)
{
#if HOST
int N1;
int i;
static TYPE *data=NULL;
static int N;
TYPE *data2=NULL;
int testit=1;
TYPE *p;

if (!data) 
  {
    testit=0;
    N = N0;
    data=(TYPE *)malloc(ELSIZE*N);
    p = data;
  }
else
  {
    data2=(TYPE *)malloc(ELSIZE*N);
    p = data2;
  }

for (i=0;i<NUM_NODES;i++)
  {
    MIMD_Recv(i,&N1,sizeof(N1));
    if (N1) MIMD_Recv(i,p,ELSIZE*N1);
    p += N1;
  }

if (testit)
  {
    int fflush();
    int qsort();
    printf("testing\n");
    fflush(stdout);
    qsort(data,N,ELSIZE,comparison);

    for (i=0;i<N;i++)
#if STRING
      if (comparison(data[i],data2[i])!=0)
#else
      if (data[i] != data2[i])
#endif
	printf("failed on element %d\n",i);

    free(data);
    free(data2);
    data = NULL;
    N = 0;
  }
#else
MIMD_Send_To_Host(&N0,sizeof(N0));
if (N0) MIMD_Send_To_Host(d,ELSIZE*N0);
#endif
}

#if !HOST
/*******************************************************************
find a checksum across all the data
********************************************************************/
double checksum(void *d,int N)
{
double sum=0;
int i;
char *c = (char *)d;

for (i=0;i<N*ELSIZE;i++)
  sum += c[i];

MIMD_Sum_Float(sum,&sum);
return(sum);
}
#endif

/*******************************************************************
test the sorting capabilities for integers
********************************************************************/
int sort_test(int N)
{
  int seed;
#if HOST
  int tot,totalN;
  
  start_random();
  
  MM_bc_integer(N);
  MM_bc_integer(seed=(getenv("SEED")?atoi(getenv("SEED")):(random()%100000))); 

  if (MM_sum_bc_int(0) != 0)
    {
      printf("error allocating elements on cells - aborting\n");
      return 1;
    }

  totalN = MM_sum_bc_int(0);

#if SAVE_SORT
  gather_write("before",NULL,totalN);
#endif

#if SORT_TEST
  sort_testit(NULL,totalN);
#endif

  printf("elements = %d seed = %d  ",totalN,seed);
  fflush(stdout);
  reset_timer();
  tot = MM_sum_bc_int(0);
  report_timer();

#if SAVE_SORT
  gather_write("after",NULL,0);
#endif

#if SORT_TEST
  sort_testit(NULL,0);
#endif

#if STATS
  report_stats(totalN);
#endif


  fflush(stdout);
  
  return 0;

#else
  TYPE *d;
  int N1;
  static int pass=0;
#if CHECKSUM
  double csum;
#endif

  pass++;

  N1 = MM_bc_receive_integer();
  seed = MM_bc_receive_integer();

  N = N1;

  if (generate_values(seed,&N,&d) != 0)
    {
      MM_sum_bc_int(1);
      return 1;
    }
  else
    MM_sum_bc_int(0);

  MM_sum_bc_int(N);

#if CHECKSUM
  csum = checksum(d,N);
#endif

#if SAVE_SORT
  gather_write(NULL,d,N);
#endif

#if SORT_TEST
  sort_testit(d,N);
#endif

  N = parallel_sort((void **)&d,N,ELSIZE,comparison);
  MM_sum_bc_int(0);

#if CHECKSUM
  {
    double csum2 = checksum(d,N);
    if (csum != csum2 && IS_BOTTOM_NODE)
      { 
	printf("checksum error! %g %g\n",csum,csum2);
	fflush(stdout);
      }
  }
#endif



#if SAVE_SORT
  gather_write(NULL,d,N);
#endif

#if SORT_TEST
  sort_testit(d,N);
#endif

#if STATS
  report_stats(N);
#endif

  free(d);
  return 0;

#endif
}


void myexit(void)
{
exit(1);
}


/*******************************************************************
main routine
********************************************************************/
int main(int argc,char *argv[])
{
  double atof(char *);
  int i=1;
  int N=524288;
  double factor=1.1;


  MIMD_Start();
#if HOST

#if CAP
  /* set a timer expiry for exit in 5 mins */
  if (!getenv("NOKILL"))
    {
      static struct itimerval oldt,newt;
      newt.it_value.tv_sec = 270;
      newt.it_value.tv_usec = 0;
      signal(SIGALRM,(void (*)())myexit);
      setitimer(ITIMER_REAL,&newt,&oldt);
    }
#endif

  if (argc>1)
    N=atoi(argv[1]);

  if (argc>2)
    i=atoi(argv[2]);

  if (argc>3)
    factor = atof(argv[3]);

  printf("sorting %d elements per node %d times with an increment factor of %g\n",N,i,factor);

  while (i--)
    {
      if (sort_test(N) != 0)
	{
	  printf("Error sorting %d elements\n",N);
	  return 1;
	}
      N *= factor;
    }

#else
  while (sort_test(N) == 0);
#endif
return(0);
}
