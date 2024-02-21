#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>

#define READ_END 0
#define WRITE_END 1
#define MAX_CUSTOMERS 5
#define SHM_SIZE 1024
#define MAX_ORDERS 10

void print_customer_orders(int *orders){
    for(int i=2;i<orders[1]+2;i++){
        printf("%d ",orders[i]);
    }
    printf("\n");
}

int compute_bill(int *orders){
    int bill=0;
    for(int i=2;i<orders[1]+2;i++){
        switch (orders[i])
        {
            case 1:
                bill+=30;
                break;
            
            case 2:
                bill+=40;
                break;

            case 3:
                bill+=25;
                break;
            
            case 4:
                bill+=30;
                break;
            
            default:{return -404;}
        }
    }
    return bill;
}

int main(){
    int waiter_id;
    printf("Enter Waiter ID:");
    scanf("%d",&waiter_id);

    //shm segment
    key_t key = ftok("table.c",waiter_id);
    if(key==-1){
        printf("error in ftok");
        return 1;
    }

    int shmid;
    int *shmptr;
    
    shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT);
     if (shmid<0) {
        perror("shmget");
        exit(1);
    }

    shmptr = (int *)shmat(shmid, NULL, 0);
    if (shmptr== (int *)(-1)) {
        perror("shmat");
        exit(1);
    }

    int total_bill=compute_bill(shmptr);
    shmptr[0]=total_bill;
    printf("The total bill amount for table %d is %d INR.\n",waiter_id,total_bill);

    print_customer_orders(shmptr);

    if (shmdt(shmptr) == -1) {
        perror("error in detaching");
        exit(1);
    }

}