#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"

double get_rand() { return (double)rand() / (RAND_MAX / 2) - 1; }

int main(int argc, char* argv[]) {
  int size, rank;
  int total, sum = 0, recv_sum = 0;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (!rank) {
    scanf("%d", &total);
  }
  MPI_Bcast(&total, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (rank) {
    srand(rank);
    for (int i = 0; i < total; ++i) {
      double x = get_rand();
      double y = get_rand();
      if (x * x + y * y <= 1) sum++;
    }
  }
  MPI_Reduce(&sum, &recv_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  if (rank == 0) {
    printf("%f\n", (double)recv_sum / (total * (size - 1)) * 4);
  }

  MPI_Finalize();
}