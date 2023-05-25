#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

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

void jacobiMethod(const double **matrix, const double *vector, double *result, double *performanceMetric)
{
    double *x = (double *)malloc(N * sizeof(double));     // Vetor inicial
    double *x_new = (double *)malloc(N * sizeof(double)); // Vetor atualizado
    int iterations = 0;

    while (iterations < MAX_ITERATIONS)
    {
        // Atualiza os valores de x
        for (int i = 0; i < N; i++)
        {
            double sum = vector[i];
            for (int j = 0; j < N; j++)
            {
                if (j != i)
                {
                    sum -= matrix[i][j] * x[j];
                }
            }
            x_new[i] = sum / matrix[i][i];
        }

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

int main()
{
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

    readMatrixFromFile(matrix);
    readVectorFromFile(vector);

    gettimeofday(&start, NULL);

    jacobiMethod((const double **)matrix, vector, result, &performanceMetric);

    gettimeofday(&end, NULL);
    elapsedTime = calculateElapsedTime(start, end);

    saveResultToFile(result, "resultados.txt");
    printf("Número de Iterações: %.2lf\n", performanceMetric);
    printf("Tempo de Execução (ms): %.2lf\n", elapsedTime);

    for (int i = 0; i < N; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
    free(vector);
    free(result);

    return 0;
}
