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
    shmid = shmget(key,SHM_SIZE,0666 | IPC_CREAT);
    printf("shared memory id for admin-hotel: %d\n",shmid);
    if(shmid<0){
        perror("error in shmget");
        exit(1);
    }

    char *shmptr = (char *)shmat(shmid, NULL , 0);
    if(shmptr == (char *)(-1)){
        perror("error at shmat");
        exit(1);
    }

    while(1){
        printf("Do you want to close the hotel? Enter Y for Yes and N for No.\n");
        char answer;
        scanf(" %c",&answer);

        if(answer == 'N' || answer == 'n'){
            continue;
        }
        else if(answer == 'Y' || answer == 'y'){
            strcpy(shmptr,"CLOSE");


            if(shmdt(shmptr)==-1){
                perror("error in shmdt");
                exit(1);
            }


            printf("Admin process terminated");
            break;
        }
        else{
            printf("Invalid input. Please enter Y or N.\n");
        }

        
    }
}
