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



int main(){
    int waiter_id;
    printf("Enter Waiter ID:");
    scanf("%d",&waiter_id);

    //printf("Check 0");
    //shm segment

    
    key_t key = ftok("table.c",waiter_id);//for table
    key_t key_hotel = ftok("hotelmanager.c",waiter_id);//for hotelmanager


    if(key==-1){
        printf("error in ftok");
        return 1;
    }

    if(key_hotel==-1){
        printf("error in ftok");
        return 1;
    }

    int shmid;
    int *shmptr;

    int shmid_hotel;
    int *shmptr_hotel;
    
    shmid = shmget(key, SHM_SIZE, 0666);
    printf("Shared memory segment for the table-waiter: %d \n",shmid);
     if (shmid<0) {
        perror("shmget");
        exit(1);
    }

    shmid_hotel = shmget(key_hotel, SHM_SIZE, 0666);
    printf("Shared memory segment for the hotel-waiter: %d \n",shmid_hotel);
     if (shmid_hotel<0) {
        perror("shmget");
        exit(1);
    }


    shmptr = shmat(shmid, NULL, 0);
    if (shmptr== NULL) {
        perror("shmat");
        exit(1);
    }

    shmptr_hotel = shmat(shmid_hotel, NULL, 0);
    if (shmptr_hotel== NULL) {
        perror("shmat");
        exit(1);
    }
    shmptr[100]=8888;


while (1) {
    if(shmptr[19]==2222){
        shmptr_hotel[19]=2222;
    }
    if (shmptr[19]==9999) {
        printf("Processing orders...\n");
        int bill=0;
        int i=1;
        while (shmptr[i]!=-1) { 
            switch (shmptr[i]) {
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
                default:
                    printf("Invalid order number: %d\n", shmptr[i]);
                    bill=-1; 
                    break;
            }
            if (bill==-1) break; 
            i++;
        }

        if (bill!=-1) {
            printf("Total bill calculated: %d INR\n", bill);
            shmptr[0]=bill;
            shmptr_hotel[0]=bill;
            shmptr_hotel[19]=9999;
            
        } else {
            printf("Invalid orders detected, bill not calculated.\n");
            shmptr[0]=-404; 
        }
        shmptr[19]=8888;
        
        

        
    }
                
        sleep(1);
        //  shmptr[0]=8888;
        if(shmptr[0]==500){
            exit(1);
        }
    }

    if (shmdt(shmptr)==-1) {
            perror("error in detaching");
            exit(1);
        }

}