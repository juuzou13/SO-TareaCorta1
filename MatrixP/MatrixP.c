#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>

#define ITERATIONS 100

int N;
int **matrix1;
int **matrix2;
int value;
static int **result;

void printMat(int **mat, char *name)
{
    printf("\n%s:\n", name);
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

void product()
{
    int fd[2];
    pipe(fd);

    pid_t processes[N];

    int *row_result = (int *)malloc(sizeof(int) * N);

    for (int row = 0; row < N; row++)
    {
        processes[row] = fork();

        if (processes[row] == 0)
        {

            for (int reps = 0; reps < N; reps++)
            {
                value = 0;
                for (int col = 0; col < N; col++)
                {
                    value += matrix1[row][col] * matrix2[col][reps];
                }
                row_result[reps] = value;
            }

            memcpy(result[row], row_result, sizeof(int) * N);

            exit(0);
        }
    }

    for (int j = 0; j < N; j++)
    {
        int status;
        waitpid(processes[j], &status, 0);
    }

    close(fd[1]);
    close(fd[0]);
    free(row_result);
}

int main()
{

    FILE *statsFile;

    srand(time(NULL));

    double excTimes[ITERATIONS];
    double currTime = 0;
    double avgTime = 0.0;
    double execution_time = 0.0;
    int matval = 0;

    printf("Ingrese un entero: ");
    scanf("%d", &N);

    result = (int **)malloc(N * sizeof(int *));
    matrix1 = (int **)malloc(N * sizeof(int *));
    matrix2 = (int **)malloc(N * sizeof(int *));

    for (int i = 0; i < N; i++)
    {
        matrix1[i] = (int *)malloc(N * sizeof(int));
        matrix2[i] = (int *)malloc(N * sizeof(int));

        result[i] = (int *)malloc(N * sizeof(int));
        result[i] = mmap(NULL, sizeof *result, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    }

    if (access("./Stats.txt", F_OK) == 0)
    {
        remove("./Stats.txt");
    }

    struct timeval stop, start;

    for (int i = 0; i < ITERATIONS; i++)
    {
        for (int row = 0; row < N; row++)
        {
            for (int col = 0; col < N; col++)
            {
                matval = (rand() % 20) - 10;
                matrix1[row][col] = matval;

                matval = (rand() % 20) - 10;
                matrix2[row][col] = matval;
            }
        }

        // printf("Iteration: %d\n", i);
        // printMat(matrix1, "Matriz 1");
        // printMat(matrix2, "Matriz 2");

        gettimeofday(&start, NULL);
        product();
        gettimeofday(&stop, NULL);

        currTime = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);

        avgTime += currTime;

        statsFile = fopen("./Stats.txt", "a");

        fprintf(statsFile, "\n----------------------------------------------------\n");

        fprintf(statsFile, "Iteración %i:\n\n", i + 1);
        fprintf(statsFile, "Tiempo: %fs", currTime);

        fprintf(statsFile, "\n----------------------------------------------------\n");
        fclose(statsFile);

        createMatrixFile(result, i);
    }

    avgTime /= ITERATIONS;

    statsFile = fopen("./Stats.txt", "a");
    fprintf(statsFile, "\n\n\nTiempo de ejecución promedio en %i iteraciones: %fs\n\n", ITERATIONS, avgTime);
    fclose(statsFile);

    free(matrix1);
    free(matrix2);

    for (int i = 0; i < N; i++)
    {
        munmap(result[i], sizeof *result);
    }

    free(result);
    munmap(result, sizeof *result);

    return 0;
}