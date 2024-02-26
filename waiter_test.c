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

int compute_bill(int *shmptr_table){
    int bill=0;
    for(int i=3;i<shmptr_table[1]+3;i++){
        switch (shmptr_table[i])
        {
        case 1:
            bill+=30;printf("Bill is getting computed order by order %d...\n",bill);break;

        case 2:
            bill+=40;printf("Bill is getting computed order by order %d...\n",bill);break;

        case 3:
            bill+=25;printf("Bill is getting computed order by order %d...\n",bill);break;

        case 4:
            bill+=30;printf("Bill is getting computed order by order %d...\n",bill);break;
        
        default:{shmptr_table[19]=-1;return -404;}
        }
    }
    // shmptr_table[17]=-1;
    return bill;
}

int main(){
    int waiter_id;
    printf("Enter the waiter id:");
    scanf(" %d",&waiter_id);

    //waiter-hotelmanager segment
    key_t key_hotel=ftok("hotelmanager_test.c",waiter_id);
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
    
    //waiter-table segment
    key_t key_table = ftok("table_test.c",waiter_id);
    if(key_table==-1){
        printf("error in ftok");
        return 1;
    }

    int shmid_table;
    int *shmptr_table;
    
    shmid_table = shmget(key_table, SHM_SIZE, 0666 | IPC_CREAT);
     if (shmid_table<0) {
        perror("shmget");
        exit(1);
    }

    shmptr_table = (int *)shmat(shmid_table, NULL, 0);
    if (shmptr_table == (int *)(-1)) {
        perror("shmat");
        exit(1);
    }
    memset(shmptr_table,0,SHM_SIZE);

    while(1){
        printf("Waiting for the order...\n");
        while (shmptr_table[2]==0)
        {
            shmptr_hotel[3]=-1;
            //wait till all the customers have given their orders
            if(shmptr_table[20]==-1){
                shmptr_table[20]=0;
                printf("No more customers will be there at table %d\n",waiter_id);
                shmdt(shmptr_table);
                shmctl(shmid_table,IPC_RMID,NULL);
                exit(1);
            }
        }
        shmptr_table[2]=0;
        int total_bill=compute_bill(shmptr_table);
        if(total_bill!=-404){
            shmptr_hotel[1]=total_bill;
            shmptr_hotel[3]=100;
            shmptr_table[18]=total_bill;
            printf("The total bill amount for table %d is %d INR.\n",waiter_id,total_bill);
            printf("<----------------------------------------->\n");
        }
        else if(total_bill==-404){
            printf("Invalid Order! Please enter again.\n");
            continue;
        }

        if(shmptr_hotel[0]==-1){
            printf("Termination command given by the hotel manager\n");
            break;
        }
        else if(shmptr_table[20]==-1){
            printf("No more customers will be there at table %d\n",waiter_id);
            shmptr_table[20]=0;
            break;
        }
    }

    // memset(shmptr_table,0,SHM_SIZE);
    shmdt(shmptr_table);
    shmctl(shmid_table,IPC_RMID,NULL);

    shmdt(shmptr_hotel);
    // shmctl(shmid_hotel,IPC_RMID,NULL);

    return 0;
}