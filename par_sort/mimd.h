#define NUM_NODES MIMD_Num_Nodes()
#define NODE_NUMBER MIMD_Node_Number()

#ifdef CM5

#include <cm/cmmd.h>

#ifdef CP_CODE 
#define HOST 1
#else
#define HOST 0
#endif

#ifndef HOST
#define HOST 0
#endif

#define TOP_NODE (NUM_NODES-1)
#define BOTTOM_NODE (0)

#define IS_TOP_NODE (NODE_NUMBER == TOP_NODE)
#define IS_BOTTOM_NODE (NODE_NUMBER == BOTTOM_NODE)

#define BIG_MSG_SIZE (1<<20)

#define BLOCKING 1

#ifndef FSEND
#define FSEND 0
#endif

#ifndef FSEND_SIZE
#define FSEND_SIZE 4
#endif


#if HOST
#define MIMD_Panic CM_panic
#else
#define MIMD_Panic CMPN_panic
#endif


#if HOST
#define MIMD_Start() CMMD_enable()

#else
#define MIMD_Start() (CMMD_enable_host(), \
		      CMMD_node_timer_clear(0), \
		      CMMD_node_timer_start(0))
#endif

#define MIMD_Node_Number() CMMD_self_address()
#define MIMD_Num_Nodes()   CMMD_partition_size()

#define SSend(to,ptr,size) (CMMD_send_block(to,0,ptr,size)==0?(size):CMMD_bytes_sent())
#define FSend(to,ptr,size) (CMMD_send_short(to,0,ptr,size),(size))


#if (FSEND && !HOST)
#define CM5_Send(to,ptr,size) (((size)>FSEND_SIZE || (to)==CMMD_host_node())?SSend(to,ptr,size):FSend(to,ptr,size))
#else
#define CM5_Send(to,ptr,size) SSend(to,ptr,size)
#endif

#define CM5_Receive(from,ptr,size) (CMMD_receive_block(from,0,ptr,size)==0?(size):CMMD_bytes_received())


#define MIMD_Host_Broad(ptr,size) CMMD_bc_from_host(ptr,size)
#define MIMD_Recv_Broad_From_Host(ptr,size) CMMD_receive_bc_from_host(ptr,size)

#define MIMD_Broad(ptr,size) CMMD_bc_to_nodes(ptr,size)
#define MIMD_Broad_Recv(from,ptr,size) CMMD_receive_bc_from_node(ptr,size)

#define MIMD_Recv_From_Host(ptr,size) CM5_Receive(CMMD_host_node(),ptr,size)
#define MIMD_Send_To_Host(ptr,size) CM5_Send(CMMD_host_node(),ptr,size)

#define MIMD_Host_Recv(from,ptr,size) CM5_Receive(from,ptr,size)
#define MIMD_Host_Send(to,ptr,size) CM5_Send(to,ptr,size)

#define MIMD_Send(to,ptr,size) CM5_Send(to,ptr,size)
#define MIMD_Send_Up(ptr,size) CM5_Send(NODE_NUMBER+1,ptr,size)
#define MIMD_Send_Down(ptr,size) CM5_Send(NODE_NUMBER-1,ptr,size)

#define MIMD_Recv(from,ptr,size) CM5_Receive(from,ptr,size)
#define MIMD_Recv_Up(ptr,size) CM5_Receive(NODE_NUMBER+1,ptr,size)
#define MIMD_Recv_Down(ptr,size) CM5_Receive(NODE_NUMBER-1,ptr,size)

#define MIMD_Host_Msg_Pending(node) CMMD_msg_pending(node,0)
#define MIMD_Host_Msg_Sender() CMMD_msg_sender()

#define MIMD_Sum_Int(val,ptr) *(ptr) = CMMD_reduce_int(val,CMMD_combiner_add)
#if DOUBLE
#define MIMD_Sum_Float(val,ptr) *(ptr) = CMMD_reduce_float(val,CMMD_combiner_add)
#else
#define MIMD_Sum_Float(val,ptr) *(ptr) = CMMD_reduce_double(val,CMMD_combiner_add)
#endif

#define MIMD_Max_Int(val) CMMD_reduce_int(val,CMMD_combiner_max)

#if !HOST
double __time_return;
#define MIMD_Uptime() (CMMD_node_timer_stop(0),__time_return=CMMD_node_timer_busy(0),CMMD_node_timer_start(0),__time_return)
#endif

#endif



#ifdef CAP

#ifndef HOST
#define HOST 0
#endif


#if HOST
#include <chost.c7.h>
#else
#include <ccell.c7.h>
#endif

#define NCELLS 128
#define TID 30

#define TOP_NODE (NUM_NODES-1)
#define BOTTOM_NODE (0)

#define IS_TOP_NODE (NODE_NUMBER == TOP_NODE)
#define IS_BOTTOM_NODE (NODE_NUMBER == BOTTOM_NODE)

#define BIG_MSG_SIZE (1<<17)

#define BLOCKING 0

#if HOST
#define MIMD_Panic(s) (fprintf(stderr,s),fflush(stderr),exit(1))
#else
#define MIMD_Panic(s) (fprintf(stderr,s),fflush(stderr),cell_exit(1))
#endif


#if HOST
#define NUM_CELLS (getenv("NCELLS")?atoi(getenv("NCELLS")):NCELLS)
#define MIMD_Start() (cconfxy(1,NUM_CELLS),ccreat(CELL_PROG,TID,NULL),initstat(1))
#else
#define MIMD_Start()
#endif

#if HOST
#define HOST_CELL getcid()
#else
#define HOST_CELL gethcid()
#endif

#define MIMD_Node_Number() getcid()
#define MIMD_Num_Nodes()   getncel()

#ifndef FSEND
#define FSEND 0
#endif

#ifndef FSEND_SIZE
#define FSEND_SIZE 5*(1<<10)
#endif


#if HOST
#define CAP_Send(to,ptr,size) (l_asend(to,TID,0,ptr,size)==1?(size):0)
#define CAP_Recv(from,ptr,size) (l_arecv(from,TID,0),readmsg(ptr,size))
#else

#define SSend(to,ptr,size) (l_asend(to,TID,0,ptr,size)==1?(size):0)
#define SRecv(from,ptr,size) (l_arecv(from,TID,0),readmsg(ptr,size))
#define FSend(to,ptr,size) xy_send(0,(to)-NODE_NUMBER,0,ptr,size)
#define FRecv(from,ptr,size) xy_recvs(0,(from)-NODE_NUMBER,0,ptr,size)

#if !FSEND
#define CAP_Send(to,ptr,size) SSend(to,ptr,size)
#define CAP_Recv(from,ptr,size) SRecv(from,ptr,size)
#else
#define CAP_Send(to,ptr,size) ((size)>FSEND_SIZE?SSend(to,ptr,size):FSend(to,ptr,size))
#define CAP_Recv(from,ptr,size) ((size)>FSEND_SIZE?SRecv(from,ptr,size):FRecv(from,ptr,size))
#endif
#endif

#define MIMD_Host_Broad(ptr,size) cbroad(TID,0,ptr,size)
#define MIMD_Recv_Broad_From_Host(ptr,size) (h_recv(0),readmsg(ptr,size))

#define MIMD_Broad(ptr,size) xy_brd(NODE_NUMBER,ptr,size)
#define MIMD_Broad_Recv(from,ptr,size) xy_brd(from,ptr,size)

#define MIMD_Recv_From_Host(ptr,size) (h_recv(0),readmsg(ptr,size))
#define MIMD_Send_To_Host(ptr,size) h_send(0,ptr,size)

#define MIMD_Host_Recv(from,ptr,size) CAP_Recv(from,ptr,size)
#define MIMD_Host_Send(to,ptr,size) CAP_Send(to,ptr,size)

#define MIMD_Send(to,ptr,size) CAP_Send(to,ptr,size)
#define MIMD_Send_Up(ptr,size) CAP_Send(NODE_NUMBER+1,ptr,size)
#define MIMD_Send_Down(ptr,size) CAP_Send(NODE_NUMBER-1,ptr,size)

#define MIMD_Recv(from,ptr,size) CAP_Recv(from,ptr,size)
#define MIMD_Recv_Up(ptr,size) CAP_Recv(NODE_NUMBER+1,ptr,size)
#define MIMD_Recv_Down(ptr,size) CAP_Recv(NODE_NUMBER-1,ptr,size)

#define MIMD_Sum_Int(val,ptr) xy_isum(val,ptr)
#if DOUBLE
#define MIMD_Sum_Float(val,ptr) xy_dsum(val,ptr)
#else
#define MIMD_Sum_Float(val,ptr) xy_fsum(val,ptr)
#endif

int _imaxval,_imaxnode;

#define MIMD_Max_Int(val) (xy_imax(val,NODE_NUMBER,&_imaxval,&_imaxnode),_imaxval)

#define MIMD_Host_Msg_Pending(node) (1)
/* l_aqrecv(node,TID,0) */
#define MIMD_Host_Msg_Sender() getmcid()

double dgettime();
#define MIMD_Uptime() dgettime()

#endif
