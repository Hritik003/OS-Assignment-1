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
    printf("The orders are: ");
    for(int i=3;i<orders[1]+3;i++){
        printf("%d ",orders[i]);
    }
    printf("\n");
}

int compute_bill(int *orders){
    int bill=0;
    for(int i=3;i<orders[1]+3;i++){
        switch (orders[i])
        {
            case 1:
                bill+=30;printf("Bill is getting computed order by order %d...\n",bill);break;
            
            case 2:
                bill+=40;printf("Bill is getting computed order by order %d...\n",bill);break;

            case 3:
                bill+=25;printf("Bill is getting computed order by order %d...\n",bill);break;
            
            case 4:
                bill+=30;printf("Bill is getting computed order by order %d...\n",bill);break;
            
            default:{orders[1]=100;return -404;}
        }
        // sleep(1);
    }
    return bill;
}

void waiter_table_process(int *shmptr,int *shmptr_hotel,int waiter_id,int num_of_customers){
    int total_bill=compute_bill(shmptr);
    if(total_bill==-404){
        printf("Invalid Order! Please enter again.\n");
        while(shmptr[1]==100){
            
        }
        shmptr[0]=num_of_customers;
        waiter_table_process(shmptr,shmptr_hotel,waiter_id,num_of_customers);
    }

    if(total_bill!=-404){
        shmptr[0]=total_bill;
        *shmptr_hotel=total_bill;
        printf("The total bill amount for table %d is %d INR.\n",waiter_id,total_bill);
        
        print_customer_orders(shmptr);
    }
}

int main(){
    int waiter_id;
    printf("Enter Waiter ID:");
    scanf("%d",&waiter_id);

    key_t key_hotel=ftok("hotelmanager.c",waiter_id);
    int shmid_hotel;
    int *shmptr_hotel;
    shmid_hotel=shmget(key_hotel,SHM_SIZE,0666 | IPC_CREAT);
    printf("Shared memory segment for the hotel-waiter: %d \n",shmid_hotel);
     if (shmid_hotel<0) {
        perror("shmget");
        exit(1);
    }
    shmptr_hotel = shmat(shmid_hotel, NULL, 0);
    if (shmptr_hotel== NULL) {
        perror("shmat");
        exit(1);
    }
    memset(shmptr_hotel,0,SHM_SIZE);

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
    memset(shmptr,0,SHM_SIZE);

    while(1){
        printf("Waiting for the order...\n");
        while(shmptr[2]==0){}
        waiter_table_process(shmptr,shmptr_hotel,waiter_id,shmptr[0]);
        memset(shmptr,0,SHM_SIZE);
        // if(shmptr[0]==101)continue;
        if(shmptr[0]==102){break;}
        else if(*shmptr_hotel==103){break;}
    }

    shmdt(shmptr);
    shmctl(shmid,IPC_RMID,NULL);

    return 0;
}