//only one instance of this is created
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>

#define MAX_CUSTOMERS 5
#define SHM_SIZE 1024
#define MAX_ORDERS 10

int main(){
    int num_tables;
    printf("Enter the Total Number of Tables at the Hotel: ");
    scanf("%d",&num_tables);

    //shm segment
    key_t key = ftok("hotelmanager.c",'A');
    if(key==-1){
        printf("error in ftok");
        return 1;
    }

    int shmid=shmget(key, 1024, 0666 | IPC_CREAT);
    if(shmid<0){
        perror("shmget error");
        exit(1);
    }

    int *shmptr_earnings=(int *)shmat(shmid, (void*)0, 0);
    if(shmptr_earnings==(int *)(-1)){
        perror("shmptr error");
        exit(1);
    }
    memset(shmptr_earnings,0,SHM_SIZE);

    shmptr_earnings[0]=num_tables;
    

    return 0;
}