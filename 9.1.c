#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>

int g = 0;

void* increment(void* arg){
    int count = *(int*) arg;
    for(int i = 0;i<count;i++){
        g++;
    }
    fprintf(stdout, "g++ = %d\n", g);
}

void* decrement(void* arg){
    int count = *(int*) arg;
    for(int i = 0;i<count;i++){
        g--;
    }
    fprintf(stdout, "g-- = %d\n", g);
}

int main(int argc, char** argv){
    // if(argc != 3){
    //     fprintf(stderr, "Enter count thread and count operation!!!!\n");
    //     return 0;
    // }
    int size_thread = 5;
    int count_operation = 10;
    if(argv[1] != NULL)
        size_thread = atoi(argv[1]);
    if(argv[2] != NULL)
        count_operation = atoi(argv[2]);
    pthread_t thread1[size_thread], thread2[size_thread];

    for(int i = 0; i<size_thread;i++){
        if(pthread_create(&thread1[i], NULL, &increment, &count_operation) != 0){
            fprintf(stderr, "Error create thread[%d]\n", i);
        }
        if(pthread_create(&thread2[i], NULL, &decrement, &count_operation) != 0){
            fprintf(stderr, "Error create thread[%d]\n", i);
        }
    }
    for(int i = 0; i<size_thread;i++){
    if(pthread_join(thread1[i], NULL) != 0){
            fprintf(stderr, "Join error thread1[%d]\n", i);
            return 1;
        }
    if(pthread_join(thread2[i], NULL) != 0){
            fprintf(stderr, "Join error thread2[%d]\n",i);
            return 1;
        }
    }

    fprintf(stdout,"FILAN MAIN THRED meaning = %d\n", g);

    return 0;
}
