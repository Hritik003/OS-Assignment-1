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

struct customer_data{
    int customer_id;
    int orders[MAX_ORDERS];
    int num_orders;
};

struct customer_data orders[MAX_CUSTOMERS];


void print_customer_orders(struct customer_data *orders){
    for(int i=0;i<MAX_CUSTOMERS;i++){
        printf("Customer %d: \n",orders[i].customer_id);
        printf("Number of orders: %d\n",orders[i].num_orders);
        for(int j=0;j<orders[i].num_orders;j++){
            printf("%d ",orders[i].orders[j]);
        }
    }
}

int main(){
    int waiter_id;
    printf("Enter Waiter ID:");
    scanf("%d",&waiter_id);


    //shm segment
    key_t key = ftok("table.c",'T');
    if(key==-1){
        printf("error in ftok");
        return 1;
    }

    int shmid;
    struct customer_data *shmptr;
    
    shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT);
     if (shmid<0) {
        perror("shmget");
        exit(1);
    }

    shmptr = (struct customer_data *)shmat(shmid, NULL, 0);
    if (shmptr== (void *)(-1)) {
        perror("shmat");
        exit(1);
    }

    print_customer_orders(shmptr);

    if (shmdt(shmptr) == -1) {
        perror("error in detaching");
        exit(1);
    }

}