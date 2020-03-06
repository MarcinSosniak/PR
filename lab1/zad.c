#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define AMOUNT_OF_SIZES 28
#define MSG_COUNT 1000
#define RETRIES 2 

//int sizes[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576};
int sizes[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 268435456};

int msg_count = MSG_COUNT;
void send_single(void* buff, int bytes )
{
  MPI_Send(buff, bytes, MPI_UNSIGNED_CHAR, 1, 0, MPI_COMM_WORLD);
  MPI_Recv(buff, bytes, MPI_UNSIGNED_CHAR,1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
}

void recv_single(void* buff,int bytes)
{
  MPI_Recv(buff, bytes, MPI_UNSIGNED_CHAR,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
  MPI_Send(buff, bytes, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
}

void ssend_single(void* buff, int bytes )
{
  MPI_Ssend(buff, bytes, MPI_UNSIGNED_CHAR, 1, 0, MPI_COMM_WORLD);
  MPI_Recv(buff, bytes, MPI_UNSIGNED_CHAR,1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
}


double test_sender(int data_size,int amount)
{
  unsigned char* buff= malloc(data_size + 1000+1);
  long long i;
  double time0=MPI_Wtime();
  for(i=0;i<amount ;i++)
  {
    buff[0] = i%255;
    send_single(buff,data_size);
    //printf("%d\n",buff[0]);
  }
  double final_time= MPI_Wtime()-time0;
  free(buff);
  return final_time;
}

double test_ssender(int data_size,int amount)
{
  unsigned char* buff= malloc(data_size + 1000+1);
  long long i;
  double time0=MPI_Wtime();
  for(i=0;i<amount ;i++)
  {
    buff[0] = i%255;
    ssend_single(buff,data_size);
    //printf("%d\n",buff[0]);
  }
  double final_time= MPI_Wtime()-time0;
  free(buff);
  return final_time;
}

double test_recv(int data_size,int amount)
{
  char* buff= malloc(data_size + 1000+1);
  long long i;  
  for(i=0;i<amount ;i++)
  {
    recv_single(buff,data_size);
  }
  free(buff);
}



void sender()
{
  MPI_Barrier(MPI_COMM_WORLD);
  //char* txt= malloc(1000);
  // strcpy(txt,"XD");
  //send_single(txt,3);
  printf("RETRIES = %d, MSG_COUNT=%d\n\n",RETRIES,msg_count);
  int ri;
  int i;
  double out[AMOUNT_OF_SIZES];
  for(i=0;i<AMOUNT_OF_SIZES;i++)
  {
    out[i]=0;
  }
  for(ri=0 ;ri< RETRIES;ri++)
  {
    for(i=0;i<AMOUNT_OF_SIZES;i++)
    {
      out[i]+=test_sender(sizes[i],msg_count);
    }
  } 
  printf("MPI_Send : \n");
  for(i=0;i<AMOUNT_OF_SIZES;i++)
  {
    printf("%d %f ms\n",sizes[i],(((1000*out[i])/RETRIES)/MSG_COUNT));
  }
  // Ssend test
  for(i=0;i<AMOUNT_OF_SIZES;i++)
  {
    out[i]=0;
  }
  for(ri=0 ;ri< RETRIES;ri++)
  {
    for(i=0;i<AMOUNT_OF_SIZES;i++)
    {
      out[i]+=test_ssender(sizes[i],msg_count);
    }
  } 
  printf("MPI_Ssend : \n");
  for(i=0;i<AMOUNT_OF_SIZES;i++)
  {
    printf("%d %f ms\n",sizes[i],(((1000*out[i])/RETRIES)/MSG_COUNT));
  } 
}

void reciever()
{
  MPI_Barrier(MPI_COMM_WORLD);
 // char* buf= malloc(1000);
  //recv_single(buf,3);
  //buf[1000-1]='\0';
  //printf("%s\n",buf);
  int ri;
  int i;
  for(ri=0 ;ri< RETRIES;ri++)
  {
    for(i=0;i<AMOUNT_OF_SIZES;i++)
    {
      test_recv(sizes[i],msg_count);
    }
  }
  for(ri=0 ;ri< RETRIES;ri++)
  {
    for(i=0;i<AMOUNT_OF_SIZES;i++)
    {
      test_recv(sizes[i],msg_count);
    }
  } 
}

void prepare_globals(int argc, char** argv)
{
  if(argc<= 1) return;
  msg_count = atoi(argv[1]);
}


int main(int argc, char** argv) {
  MPI_Init(NULL, NULL);
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  // We are assuming at least 2 processes for this task
  if (world_size < 2) {
    fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
    MPI_Abort(MPI_COMM_WORLD, 1); 
  }
  prepare_globals(argc,argv); 
  if (world_rank == 0) {
    sender();
  } else if (world_rank == 1) {
    reciever();
  }
  MPI_Finalize();
}
