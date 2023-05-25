#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <mpi.h>

#define N 2048              // Tamanho da matriz
#define MAX_ITERATIONS 5000 // Número máximo de iterações

void readMatrixFromFile(double **matrix)
{
    FILE *file = fopen("m2048.txt", "r");

    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo da matriz!\n");
        return;
    }

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            fscanf(file, "%lf", &matrix[i][j]);
        }
    }

    fclose(file);
}

void readVectorFromFile(double *vector)
{
    FILE *file = fopen("v2048.txt", "r");

    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo do vetor!\n");
        return;
    }

    for (int i = 0; i < N; i++)
    {
        fscanf(file, "%lf", &vector[i]);
    }

    fclose(file);
}

void saveResultToFile(const double *result, const char *filename)
{
    FILE *file = fopen(filename, "w");

    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }

    for (int i = 0; i < N; i++)
    {
        fprintf(file, "%f\n", result[i]);
    }

    fclose(file);
}

double calculateElapsedTime(const struct timeval start, const struct timeval end)
{
    double elapsedTime = (end.tv_sec - start.tv_sec) * 1000.0; // segundos para milissegundos
    elapsedTime += (end.tv_usec - start.tv_usec) / 1000.0;     // microssegundos para milissegundos
    return elapsedTime;
}

double calculateNorm(const double *vector)
{
    double norm = 0.0;

    for (int i = 0; i < N; i++)
    {
        norm += vector[i] * vector[i];
    }

    return sqrt(norm);
}

void jacobiMethod(double **matrix, const double *vector, double *result, double *performanceMetric, int rank, int world)
{
    double *x = (double *)malloc(N * sizeof(double));     // Vetor inicial
    double *x_new = (double *)malloc(N * sizeof(double)); // Vetor atualizado
    int iterations = 0;
    int submn = N / world;
    int last = (rank == (world - 1) && N % world > 0) ? N % world : 0;

    while (iterations < MAX_ITERATIONS)
    {
        // Atualiza os valores de x
        for (int k = 0; k < submn + last; k++)
        {
            double sum = vector[k + rank * submn];
            for (int i = 0; i < N; i++)
            {
                if (i != k + rank * submn)
                {
                    sum -= matrix[k + rank * submn][i] * x[i];
                }
            }
            x_new[k + rank * submn] = sum / matrix[k + rank * submn][k + rank * submn];
        }

        // Sincronização dos vetores x
        MPI_Allgather(x_new, submn + last, MPI_DOUBLE, x_new, submn + last, MPI_DOUBLE, MPI_COMM_WORLD);

        // Atualiza o vetor x
        for (int i = 0; i < N; i++)
        {
            x[i] = x_new[i];
        }

        iterations++;
    }

    *performanceMetric = (double)iterations;
    for (int i = 0; i < N; i++)
    {
        result[i] = x_new[i];
    }

    free(x);
    free(x_new);
}

int main(int argc, char *argv[])
{
    int rank, world;
    double **matrix = (double **)malloc(N * sizeof(double *)); // Matriz
    for (int i = 0; i < N; i++)
    {
        matrix[i] = (double *)malloc(N * sizeof(double));
    }

    double *vector = (double *)malloc(N * sizeof(double)); // Vetor B
    double *result = (double *)malloc(N * sizeof(double));
    double performanceMetric;
    struct timeval start, end;
    double elapsedTime;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        readMatrixFromFile(matrix);
        readVectorFromFile(vector);
    }

    MPI_Bcast(matrix[0], N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(vector, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    gettimeofday(&start, NULL);

    jacobiMethod(matrix, vector, result, &performanceMetric, rank, world);

    gettimeofday(&end, NULL);
    elapsedTime = calculateElapsedTime(start, end);

    double maxPerformanceMetric;
    MPI_Reduce(&performanceMetric, &maxPerformanceMetric, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        saveResultToFile(result, "resultados.txt");
        printf("Número de Iterações: %.2lf\n", maxPerformanceMetric);
        printf("Tempo de Execução (ms): %.2lf\n", elapsedTime);
    }

    for (int i = 0; i < N; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
    free(vector);
    free(result);

    MPI_Finalize();

    return 0;
}