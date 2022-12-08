#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>


struct arr{
    int* array;
    int size;
    pthread_rwlock_t q_lock;
};

int arr_init(struct arr *qp, int s_arr){
    int err;
    qp->size = s_arr;
    qp->array = (int*)malloc(sizeof(int) * qp->size);
    err = pthread_rwlock_init(&qp->q_lock, NULL);
    if(err != 0){
        return(err);
    }
    return 0;
}


int rand_num(struct arr* qp){
    srand(time(NULL));
    return rand() % qp->size;
}

void* arr_status(struct arr* qp){
    for(int i = 0;i<qp->size;i++)
        fprintf(stdout, "array[%d] = %d\n",i, qp->array[i]);
}

void* reader(struct arr* qp){
    pthread_rwlock_wrlock(&qp->q_lock);
    int rand_n = rand_num(qp);
    fprintf(stdout,"arr[%d] = %d",rand_n, qp->array[rand_n]);
    pthread_rwlock_unlock(&qp->q_lock);
    return NULL;
}

void* writer(struct arr* qp){
    if(pthread_rwlock_rdlock(&qp->q_lock) != 0){
        return NULL;
    }
    int rand_n = rand_num(qp);
    qp->array[rand_n] = rand_n;
    pthread_rwlock_unlock(&qp->q_lock);
    return NULL;
}

int main(int argc, char** argv){
    struct arr* arr;
    if(atoi(argv[1]) <= 0){
        fprintf(stderr, "Size must be > 0\n");
        return 1;
    }
    if(argv[1] == NULL)
        arr_init(arr, 10);
    else{
        arr_init(arr, atoi(argv[1]));
    }
    int wSize = 10, rSize = 20;
    pthread_t thread_write[wSize], thread_read[rSize];
    pthread_attr_t attr;
    pthread_t thread_atr;

    for(int i = 0; i<wSize;i++){
        if(pthread_create(&thread_write[i], NULL, &writer, &arr) != 0){
            fprintf(stderr, "Error create threadwrite[%d]\n", i);
        }
    }
    for(int i = 0; i<rSize;i++){
        if(pthread_create(&thread_read[i], NULL, &reader, &arr) != 0){
            fprintf(stderr, "Error create threadread[%d]\n", i);
        }
    }

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&thread_atr, &attr, &arr_status, NULL);
    pthread_attr_destroy(&attr);

    for(int i = 0; i<wSize;i++){
    if(pthread_join(thread_write[i], NULL) != 0){
            fprintf(stderr, "Join error threadwriter[%d]\n", i);
            return 1;
        }
    }

    for(int i = 0; i<rSize;i++){
    if(pthread_join(thread_read[i], NULL) != 0){
            fprintf(stderr, "Join error threadreader[%d]\n", i);
            return 1;
        }
    }
    return 0;
}