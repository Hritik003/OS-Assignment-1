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
#define MAX_TABLE_SIZE 10

void display_menu(){
    printf("<-------------------------------Menu--------------------------------->\n");
    printf(" \n");
    FILE *fptr;
    fptr = fopen("menu.txt","r");
    if (!fptr) {
        perror("Error occured in opening the menu.txt file");
        exit(1);
    }
    char cuisine[100];
    while (fgets(cuisine, 100, fptr) != NULL) {
        printf("%s\n", cuisine);
    }
    fclose(fptr);
    printf("<---------------------------------------------------------------->\n");
}

void customer_order(int pipe_handler[], int customer_id, int i){
    //customer closes the read end
    close(pipe_handler[READ_END]);

    //taking order from the customer
    int order_num;
    int j=0;
    int count_of_orders=0;
    
    
    printf("Customer %d, enter the serial number(s) of the item(s) to order from the menu. Enter -1 when done:\n", customer_id);
    
    while(1){
        scanf("%d",&order_num);
        write(pipe_handler[WRITE_END], &order_num, sizeof(order_num));

        if (order_num == -1) {
            write(pipe_handler[WRITE_END], &order_num, sizeof(order_num));
            break;
        }
    
    }

    //customer closes the write end
    close(pipe_handler[WRITE_END]);
    exit(0);
}



int main(){
    int table_number;
    printf("Enter Table Number:");
    scanf("%d",&table_number);

    //shared memory
    key_t key = ftok("table.c",table_number);
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
    printf("%d\n",shmid);
    shmptr = (int *)shmat(shmid, NULL, 0);
    if (shmptr== NULL) {
        perror("shmat");
        exit(1);
    }

    memset(shmptr,0,SHM_SIZE);

    int pipe_handler[MAX_CUSTOMERS][2];
    int customer_pids[MAX_CUSTOMERS];

    while(1)
    {
        int num_of_customers;
        printf("Enter Number of Customers at Table (maximum no. of customers can be 5):\n");
        scanf("%d",&num_of_customers);
        if(num_of_customers==0){
            shmptr[19]==2222;
        }
        shmptr[19]=8888;
        shmptr[0]=num_of_customers;

        if(num_of_customers==-1){
            perror("exitting since the number of customers entered is -1");
            exit(1);
        }


        display_menu();

        wrong_order:
        for(int i=0;i<num_of_customers;i++){
            

            if(pipe(pipe_handler[i]) == -1){
                perror("Error in creating the pipe");
                exit(1);
            }

            pid_t pid = fork();
            if(pid == 0){
                printf("customer %d pid is: %d\n",i+1,getpid());
                printf("table pid is: %d\n",getppid());
                customer_order(pipe_handler[i], i + 1,i);
            }
            else if(pid > 0){
                customer_pids[i]=pid;
                close(pipe_handler[i][WRITE_END]); 
                // printf("this is parent");
            }
            else{
                perror("Error in forking a process for the customer");
            }
            wait(NULL);

        }

        int j=1;
        for (int i=0; i<num_of_customers; i++) {
            int status;
            waitpid(customer_pids[i], &status, 0);
            int order_num;
            int check= 0;
            printf("\n Order recieved from Customer %d: ", i + 1);
            int count=0;
            
            
            while (read(pipe_handler[i][READ_END], &order_num, sizeof(order_num)) > 0 ) {
                if(order_num!=-1){
                    count=count+1;
                    shmptr[j++]=order_num;
                    printf("%d ", order_num);
                    check = 1;
                    
                }
            }
            if(!check) printf("No order recieved from customer %d",i+1);
            shmptr[j++]=-1;
            
            printf("\n");
            close(pipe_handler[i][READ_END]);
        }
        shmptr[19]=9999;
        

        for(int i=0;i<20;i++){
            printf("%d ",shmptr[i]);
        }

        printf("MESSAGE STORED\n");
        sleep(5);
        printf("<---------------------------------------------------------->\n");
        printf("Waiting for the bill from the waiter...\n");
        if(shmptr[0]!= -404){

            printf("The bill amount is: %d\n",shmptr[0]);
            printf("\n");

            int *earnings;
            key_t key;
            int shmid;

            key = ftok("hotelmanager.c",table_number);
            shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT);
            if(shmid<0){
                perror("error in shmget");
                continue;
            }

            earnings = (int *)shmat(shmid,NULL,0);
            if (earnings==(int *)(-1)) {
                perror("shmat");
                continue;
            }

            earnings[0] = shmptr[0];


            printf("<------------------order done-------------------->");
            printf("\n");
            printf("Do you wish to allot customers again?: ");
            int answer;
            scanf("%d",&answer);
            if(answer==1){
                continue;
            }
            else{
                shmptr[0]=500;
                printf("table terminated since there are no customers");
                exit(1);
            }
        
        }
        else{
            shmptr[0]=num_of_customers;
            printf("wrong order number placed \n");
            goto wrong_order;


        }
    }




}