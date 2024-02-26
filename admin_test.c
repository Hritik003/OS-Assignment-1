#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>

#define SHM_SIZE 1024

int main(){
    key_t key = ftok("admin.c",'A');
    if(key==-1){
        perror("error in ftok");
        exit(1);
    }

    int shmid;
    shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT);
    if(shmid<0){
        perror("error in shmget");
        exit(1);
    }
    printf("shared memory id for admin-hotel: %d\n",shmid);

    char *shmptr =(char *)shmat(shmid, (void*)0, 0);
    if(shmptr == (char *)(-1)){
        perror("error at shmat");
        exit(1);
    }
    *shmptr='N';

    char answer;
    while(1){
        printf("Do you want to close the hotel? Enter Y for Yes and N for No.......");
        scanf(" %c",&answer);

        if(answer == 'N' || answer == 'n'){
            *shmptr='N';
            continue;
        }
        else if(answer == 'Y' || answer == 'y'){
            *shmptr='Y';
            printf("Admin process terminated\n");
            break;
        }
        else{
            printf("\nInvalid input. Please enter Y or N.\n");
        }  
    }
    shmdt(shmptr);
    return 0;
}