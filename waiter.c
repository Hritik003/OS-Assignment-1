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
    printf("Enter Waiter ID 12:");
    scanf("%d",&waiter_id);

    //printf("Check 0");
    //shm segment
    key_t key = ftok("table.c",waiter_id);
    if(key==-1){
        printf("error in ftok");
        return 1;
    }

    int shmid;
    int *shmptr;
    
    shmid = shmget(key, SHM_SIZE, 0666);
    printf("%d ",shmid);
     if (shmid<0) {
        perror("shmget");
        exit(1);
    }


    shmptr = shmat(shmid, NULL, 0);
    if (shmptr== NULL) {
        perror("shmat");
        exit(1);
    }
 
    int num_of_customers = shmptr[0];
    int valid=200;
    int bill=0;

    int i=1;
    while(num_of_customers!=0){
        if(shmptr[i]==(-1)){
            num_of_customers--;
            continue;
        }
        else if(shmptr[i]<1 || shmptr[i]>4){
            valid=-404;
            break;
        }
        else{
            if(shmptr[i]==1)bill+=30;
            else if(shmptr[i]==2)bill+=40;
            else if(shmptr[i]==3)bill+=25;
            else bill+=30;
        }

        i++;
    }

    memset(shmptr,0,SHM_SIZE);

    if(valid == -404){
        printf("invalid order placed by the customer");
        shmptr[0]=valid;
    }
    else{
        printf("bill amount sent :%d",bill);
        shmptr[0]=bill;
    }
            
    if (shmdt(shmptr) == -1) {
        perror("error in detaching");
        exit(1);
    } 

}