/*
* 探究多线程编程
* pthread_create(),pthread_join()与pthread_detach()
* 互斥锁
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>
#include <ctime>

/*
*Lab1：证明线程运行时线程之间的乱序性
*/
// void *doWork(void *arg){
//     pthread_detach(pthread_self());
//     sleep(rand()%5);
//     int *index = (int *) arg;
//     printf("pthread id is %ld and the val of index is %d.\n",pthread_self(),*index);
//     return nullptr;
// }

// int main(){
//     //int calculator = 0;
//     srand((int)time(0));
//     int arg[100];
//     for(int i=0; i<100; i++){
//         pthread_t tid;
//         arg[i] = i;
//         pthread_create(&tid,NULL,doWork,&arg[i]);
//     }
//     sleep(8);
//     //printf("the val of calculator is %d\n",calculator);
//     return 0;
// }

/*
*Lab2:
*加入了互斥锁
*使用pthread_detach()
*/
// pthread_mutex_t mtx;

// void *doWork(void *arg){
//     pthread_detach(pthread_self());
//     //sleep(rand()%5);
//     int *calculator = (int*)arg;
//     pthread_mutex_lock(&mtx);
//     *calculator += 1;
//     printf("pthread id is %ld and the val of index is %d.\n",pthread_self(),*calculator);
//     pthread_mutex_unlock(&mtx);
//     return nullptr;
// }

// int main(){
//     //srand((int)time(0));
//     pthread_mutex_init(&mtx,NULL);
//     int calculator = 0;
//     for(int i=0; i<100; i++){
//         pthread_t tid;
//         pthread_create(&tid,NULL,doWork,&calculator);
//     }
//     //sleep(8);
//     pthread_mutex_lock(&mtx);
//     printf("the val of calculator is %d\n",calculator);
//     pthread_mutex_unlock(&mtx);
//     return 0;
// }

/*
Lab3:
*尝试pthread_join()
*答案是会阻塞在pthread_join函数这里
*线程已经退出了的话使用pthread_join也不会报错
*/
void *doWork(void *arg)
{
    //pthread_detach(pthread_self());
    sleep(1);
    int *index = (int *)arg;
    printf("pthread id is %ld and the val of index is %d.\n", pthread_self(), *index);
    return nullptr;
}

int main()
{
    //int calculator = 0;
    //srand((int)time(0));
    int arg[10];
    pthread_t tids[10];
    for (int i = 0; i < 10; i++)
    {
        pthread_t tid;
        arg[i] = i;
        pthread_create(&tid, NULL, doWork, &arg[i]);
        tids[i] = tid;
    }
    sleep(2);
    //线程已经退出了的话使用pthread_join也不会报错
    for (int i = 0; i < 10; i++)
    {
        pthread_join(tids[i], NULL);
    }
    //sleep(8);
    //printf("the val of calculator is %d\n",calculator);
    return 0;
}