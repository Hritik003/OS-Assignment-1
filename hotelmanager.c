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
    int tables;
    float t_earnings=0.0;
    FILE *fptr;    
    

    printf("Enter the Total Number of Tables at the Hotel: ");
    scanf("%d",&tables);

    //printf("check0");
    fptr = fopen("earnings.txt","w");
    if(fptr== NULL){
        perror("unable to open file");
        exit(1);
    }
    //printf("check1");

    key_t termination_key = ftok("admin.c",'A');
    int termination_shmid = shmget(termination_key,SHM_SIZE,0666);
    printf("%d",termination_shmid);
    if(termination_shmid<0){
        perror("error at termination shm");
        exit(1);
    }

    char *termination_shmptr =(char *)shmat(termination_shmid, NULL, 0);
    if(termination_shmptr == (char *)-1){
        perror("error in shmptr");
        exit(1);
    }

    strcpy(termination_shmptr,"");

    int closed =0;


    for(int i=1;i<=tables;i++){

        int *earnings;
        key_t key;
        int shmid;

        key = ftok("hotelmanager.c",i);
        shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT);
        if(shmid<0){
            perror("errorin shmget");
            continue;
        }

        earnings = (int *)shmat(shmid,NULL,0);
        if (earnings==(int *)(-1)) {
            perror("shmat");
            continue;
        }

        while(*earnings==0 && strcmp(termination_shmptr, "CLOSE") != 0){
            sleep(1);
        }

        if(strcmp(termination_shmptr, "CLOSE") == 0){
            closed=1;
        }

        fprintf(fptr,"Earning from Table %d: %.2f INR\n", i, earnings[0]);
        t_earnings+= earnings[0];
        
        shmdt(earnings);
        if(closed){
            shmctl(shmid, IPC_RMID, NULL);
        }
        
    }
    float profit=0.0;
    float wages=0.0;

    wages = t_earnings*0.4;
    profit = t_earnings-wages;

    fprintf(fptr, "Total Earnings of Hotel: %.2f INR\n", t_earnings);
    fprintf(fptr, "Total Wages of Waiters: %.2f INR\n", wages);
    fprintf(fptr, "Total Profit: %.2f INR\n", profit);
    fclose(fptr);

    printf("Total Earnings of Hotel: %.2f INR\n", t_earnings);
    printf("Total Wages of Waiters: %.2f INR\n", wages);
    printf("Total Profit: %.2f INR\n", profit);

    printf("Thank you for visiting the Hotel!\n");

    return 0;

}