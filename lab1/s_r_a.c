#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void sender()
{
  int number;
  int buffer[1000];
  MPI_Buffer_attach(buffer, 1000*sizeof(int));

  number = 1;
  MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
  
  number = 2;
  MPI_Ssend(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

  number = 3;
  MPI_Bsend(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

  number = 4;
  MPI_Rsend(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
  
  number=5;
  MPI_Request req;
  MPI_Isend(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD,&req);
  MPI_Wait(&req,MPI_STATUS_IGNORE);
  int test_res;
  MPI_Test(&req,&test_res,MPI_STATUS_IGNORE);
  printf("Proccess 0 received fallbakc status after waiting:%d",test_res);



}

void reciever()
{
  int number=0;
  while(number != 4)
  {
    MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Process 1 received number %d from process 0\n", number);
  }
  while(1)
  {
    MPI_Request req;
    MPI_Irecv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &req);
    int test_res;
    MPI_Test(&req,&test_res,MPI_STATUS_IGNORE);
    if(test_res!= 0)
    {
      printf("recived final umber %d\n",number);
      break;
    } 
  }
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
  if (world_rank == 0) {
    sender();
  } else if (world_rank == 1) {
    reciever();
  }
  MPI_Finalize();
}
