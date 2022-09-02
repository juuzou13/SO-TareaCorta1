#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define ITERATIONS 100

int N;
int **matrix1;
int **matrix2;
int **result;
pthread_t *threads;

void printMat(int **mat, char *name)
{
    printf("%s:\n", name);
    printf("\n");
    for (int row = 0; row < N; row++)
    {
        printf("[ ");
        for (int col = 0; col < N; col++)
        {
            printf("%d", mat[row][col]);
            if (col + 1 != N)
            {
                printf(", ");
            }
        }
        printf(" ]\n");
    }
    printf("\n");
}

void writeMat(FILE *file, int **mat)
{
    for (int row = 0; row < N; row++)
    {
        fprintf(file, "[ ");
        for (int col = 0; col < N; col++)
        {
            fprintf(file, "%d", mat[row][col]);
            if (col + 1 != N)
            {
                fprintf(file, ", ");
            }
        }
        fprintf(file, " ]\n");
    }
}

void createMatrixFile(int **matrix, int iteration)
{
    FILE *textFile;
    char filename[50];
    char number[5];
    char path[200];

    if (iteration < 10)
    {
        sprintf(number, "0%i", iteration);
    }
    else
    {
        sprintf(number, "%i", iteration);
    }

    sprintf(filename, "Mat_%s", number);
    sprintf(path, "./Matrices/%s.txt", filename);

    textFile = fopen(path, "w");
    writeMat(textFile, matrix);
    fclose(textFile);
}

void *multiply(void *data)
{
    int row = *((int *)data);
    int value = 0;

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            value += matrix1[row][j] * matrix2[j][i];
        }
        result[row][i] = value;
        value = 0;
    }

    free(data);
}

void product()
{

    for (int row = 0; row < N; row++)
    {
        int *data = (int *)malloc(sizeof(int));
        *data = row;
        pthread_create(&(threads[row]), NULL, multiply, (void *)data);
    }

    for (int row = 0; row < N; row++)
    {
        pthread_join(threads[row], NULL);
    }
}

int main()
{

    srand(time(NULL));
    double execution_time = 0.0;
    double average_time = 0.0;
    struct timespec start, end;
    FILE *statsFile;

    printf("Ingrese un entero: ");
    scanf("%d", &N);

    if (access("./Stats.txt", F_OK) == 0)
    {
        remove("./Stats.txt");
    }

    matrix1 = (int **)malloc(N * sizeof(int *));
    matrix2 = (int **)malloc(N * sizeof(int *));
    result = (int **)malloc(N * sizeof(int *));

    for (int i = 0; i < N; i++)
    {
        matrix1[i] = (int *)malloc(N * sizeof(int));
        matrix2[i] = (int *)malloc(N * sizeof(int));
        result[i] = (int *)malloc(N * sizeof(int));
    }

    threads = (pthread_t *)malloc(N * sizeof(pthread_t));

    for (int iteration = 0; iteration < ITERATIONS; iteration++)
    {
        for (int row = 0; row < N; row++)
        {
            for (int col = 0; col < N; col++)
            {
                matrix1[row][col] = (rand() % 20) - 10;
                matrix2[row][col] = (rand() % 20) - 10;
            }
        }

        clock_gettime(CLOCK_REALTIME, &start);
        product();
        clock_gettime(CLOCK_REALTIME, &end);
        double execution_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;

        average_time += execution_time;

        statsFile = fopen("./Stats.txt", "a");

        fprintf(statsFile, "\n----------------------------------------------------\n");

        fprintf(statsFile, "Iteración %i:\n\n", iteration + 1);
        fprintf(statsFile, "Tiempo: %fs", execution_time);

        fprintf(statsFile, "\n----------------------------------------------------\n");
        fclose(statsFile);

        createMatrixFile(result, iteration);
    }

    average_time /= ITERATIONS;

    statsFile = fopen("./Stats.txt", "a");
    fprintf(statsFile, "\n\n\nTiempo de ejecución promedio en %i iteraciones: %fs\n\n", ITERATIONS, average_time);
    fclose(statsFile);

    free(threads);

    for (int i = 1; i < N; i++)
    {
        free(matrix1[i]);
        free(matrix2[i]);
        free(result[i]);
    }

    free(matrix1);
    free(matrix2);
    free(result);

    return 0;
}