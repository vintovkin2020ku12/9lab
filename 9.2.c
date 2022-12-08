#include "pthread.h"
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "pthreadVC2.lib")
#pragma comment(lib, "pthreadVCE2.lib")
#pragma comment(lib, "pthreadVSE2.lib")

/* Все параметры для простоты задаются константами */
static double a = 0.;     /* левый конец интервала */
static double b = 2.;     /* правый конец интервала */
static int n = 100000000; /* число точек разбиения */

/* Результат: интеграл */
static double total = 0.;

/* Объект типа mutex для синхронизации доступа к total */
static pthread_mutex_t total_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Общее количество процессов */
static int p;

double f(double x)
{
    return 4 - (x * x);
}

double integrate(double a, double b, double n)
{
    double stepSize = (b - a) / n;

    /* Finding Integration Value */
    double integration = f(a) + f(b);
    for (int i = 1; i <= n - 1; i++)
    {
        double k = a + i * stepSize;
        integration = integration + 2 * f(k);
    }
    integration = integration * stepSize / 2;
}

/* Функция, работающая в задаче с номером my_rank */
void *process_function(void *pa)
{
    /* номер текущей задачи */
    int my_rank = (int)pa;
    /* длина отрезка интегрирования для текущего процесса*/
    double len = (b - a) / p;
    /* число точек разбиения для текущего процесса */
    int local_n = n / p;
    /* левый конец интервала для текущего процесса */
    double local_a = a + my_rank * len;
    /* правый конец интервала для текущего процесса */
    double local_b = local_a + len;
    /* значение интеграла в текущем процессе */
    double integral;
    /* Вычислить интеграл в каждой из задач */

    integral = integrate(local_a, local_b, local_n);
    /* "захватить" mutex для работы с total */
    pthread_mutex_lock(&total_mutex); /* сложить все ответы */
    total += integral;                /* "освободить" mutex */
    pthread_mutex_unlock(&total_mutex);
    return 0;
}

// gcc -o main main.c -lpthread
int main(int argc, char *argv[])
{ /* массив идентификаторов созданных задач */
    pthread_t *threads;
    int i;

    if (argc != 2)
    {
        printf("Usage: %s <instances>\n", argv[0]);
        return 1;
    }

    /* Получаем количество процессов */
    p = (int)strtol(argv[1], 0, 10);
    if (!(threads = (pthread_t *)malloc(p * sizeof(pthread_t))))
    {
        printf("Not enough memory!\n");
        return 1;
    }

    /* Запускаем задачи */ for (i = 0; i < p; i++)
    {
        if (pthread_create(threads + i, 0, process_function, (void *)i))
        {
            printf("cannot create thread #%d!\n", i);
            return 2;
        }
    }

    /* Ожидаем окончания задач */
    for (i = 0; i < p; i++)
    {
        if (pthread_join(threads[i], 0))
            printf("cannot wait thread #%d!\n", i);
    }

    /* Освобождаем память */
    free(threads);
    printf("Integral from %lf to %lf = %.18lf\n", a, b, total);
    return 0;
}