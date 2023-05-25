#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FILE_NAME "matriz.txt"
#define DIGITS_AFTER_DECIMAL 5

void generateDiagonallyDominantMatrix(int size, double **matrix)
{
    unsigned int seed = (unsigned int)time(NULL);

    for (int i = 0; i < size; i++)
    {
        double sum = 0.0;
        for (int j = 0; j < size; j++)
        {
            if (i != j)
            {
                matrix[i][j] = (double)(random() % 10); // Gera um número aleatório entre 0 e 99
                sum += abs(matrix[i][j]);
            }
        }
        matrix[i][i] = sum + 1 + (double)(random() % 6); // Gera um número aleatório maior que a soma dos outros elementos da linha
    }
}

void saveMatrixToFile(int size, double **matrix)
{
    FILE *file = fopen(FILE_NAME, "w");

    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo '%s'!", FILE_NAME);
        return;
    }

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            fprintf(file, "%.*f\n", DIGITS_AFTER_DECIMAL, matrix[i][j]); // Imprime com o número de dígitos após o ponto decimal definido pela constante
        }
    }

    fclose(file);
    printf("Matriz salva com sucesso no arquivo '%s'\n", FILE_NAME);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Uso: %s <tamanho da matriz>\n", argv[0]);
        return 1;
    }

    int size = atoi(argv[1]);
    double **matrix = (double **)malloc(size * sizeof(double *));
    for (int i = 0; i < size; i++)
    {
        matrix[i] = (double *)malloc(size * sizeof(double));
    }

    srandom((unsigned int)time(NULL)); // Inicializa a semente para a função random()

    generateDiagonallyDominantMatrix(size, matrix);
    saveMatrixToFile(size, matrix);

    for (int i = 0; i < size; i++)
    {
        free(matrix[i]);
    }
    free(matrix);

    return 0;
}
