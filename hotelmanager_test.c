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
#define MAX_TABLES 10

void calculate_profit(FILE *fptr,float t_earnings){
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
}

int main(){
    key_t termination_key = ftok("admin.c",'A');
    int termination_shmid = shmget(termination_key,SHM_SIZE,0666 | IPC_CREAT);
    if(termination_shmid<0){
        perror("error at termination shm");
        exit(1);
    }
    printf("shared memory id for hotel-admin: %d\n",termination_shmid);

    char *termination_shmptr =(char *)shmat(termination_shmid, (void*)0, 0);
    if(termination_shmptr == (char *)(-1)){
        perror("error in shmptr");
        exit(1);
    }

    int num_of_tables=0;
    printf("Enter the number of tables:");
    scanf(" %d",&num_of_tables);
    printf("\n");

    float t_earnings=0.0;
    FILE *fptr;   
    fptr = fopen("earnings.txt","w");
    if(fptr== NULL){
        perror("unable to open file");
        exit(1);
    }

    /*create shms for waiters*/
    int waiter_shmids[MAX_TABLES+1];
    int *waiter_shmptrs[MAX_TABLES+1];
    for(int i=1;i<=num_of_tables;i++){
        key_t waiter_shmkey=ftok("hotelmanager_test.c",i);
        if(waiter_shmkey==-1){
            printf("error in ftok\n");
            exit(1);
        }

        int waiter_shmid=shmget(waiter_shmkey, 1024, 0666 | IPC_CREAT);
        if(waiter_shmid<0){
            perror("shmget error");
            exit(1);
        }

        int *waiter_shmptr=(int *)shmat(waiter_shmid, (void*)0, 0);
        if(waiter_shmptr==(int *)(-1)){
            perror("shmptr error");
            exit(1);
        }
        memset(waiter_shmptr,0,SHM_SIZE);
        waiter_shmids[i]=waiter_shmid;
        waiter_shmptrs[i]=waiter_shmptr;

        printf("Created shm with id: %d\n",waiter_shmids[i]);
    }
    while(1){//loop until the termination signal is given by the admin
        for(int i=1;i<=num_of_tables;i++){
            if(waiter_shmptrs[i][1]!=0){
                printf("Earning from Table %d: %d INR\n", i, waiter_shmptrs[i][1]);
                fprintf(fptr,"Earning from Table %d: %d INR\n", i, waiter_shmptrs[i][1]);
                t_earnings+=waiter_shmptrs[i][1];      

                waiter_shmptrs[i][1]=0;
                // if(*termination_shmptr=='Y'){
                //     waiter_shmptrs[i][0]=-1;
                // }
            }
        }
        if(*termination_shmptr=='Y'){
            // if(){continue;}
            int count_processing=0;
            for(int i=1;i<=num_of_tables;i++){
                if(waiter_shmptrs[i][3]!=100){count_processing++;}
            }
            if(count_processing!=0){count_processing=0;continue;}
            printf("Termination signal is given by the admin\n");
            break;
        }
        else{continue;}
    }

    printf("\n<---------------------------CALCULATING THE INCOME, EXPENSES AND PROFIT--------------------------->\n");    

    calculate_profit(fptr,t_earnings);
    fclose(fptr); 
    printf("Thank you for visiting the Hotel!");

    for(int i=1;i<=num_of_tables;i++){
        // *waiter_shmptrs[i]=0;
        waiter_shmptrs[i][0]=-1;
        // memset(waiter_shmptrs[i],0,SHM_SIZE);
        shmdt(waiter_shmptrs[i]);
        shmctl(waiter_shmids[i],IPC_RMID,NULL);
    }

    shmdt(termination_shmptr);
    shmctl(termination_shmid,IPC_RMID,NULL);

    return 0;
}