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
#define MAX_ORDERS 10
#define SHM_SIZE 1024

int pipe_handler[MAX_CUSTOMERS][2];
int customer_pids[5]={0};

void display_menu(){
    printf("Menu: \n");
    FILE *fptr;
    fptr = fopen("menu.txt","r");
    if (!fptr) {
        perror("Error occured in opening the menu.txt file");
        exit(1);
    }
    char cuisine[100];
    while (fgets(cuisine, 100, fptr) != NULL) {
        printf("%s", cuisine);
    }
    fclose(fptr);
    printf("\n");
}

void customer_order(int i){
    if(pipe(pipe_handler[i])==-1){
        perror("Error in creating the pipe");
        exit(1);
    }

    //creating a process
    pid_t pid = fork();

    if(pid == 0){
        //child process
        int order_num;
        printf("table pid is: %d\n",getppid());
        printf("Enter the serial number(s) of the item(s) to order from the menu. Enter -1 when done:");
        // int j=0;
        while(scanf("%d",&order_num)&&order_num!=-1){
            // if(order_num<0 || order_num>4){printf("Please enter a valid order\n");continue;}
            close(pipe_handler[i][READ_END]);
            
            write(pipe_handler[i][WRITE_END],&order_num,sizeof(order_num));
            // ordersarr[j++]=order_num;
        }
        close(pipe_handler[i][WRITE_END]);
        exit(0);
    }

    else if(pid>0){
        customer_pids[i]=pid;
        close(pipe_handler[i][WRITE_END]);
    }

    else{
        perror("Error in forking a process for the customer");
    }
    printf("\n");
}

void customer_create(int *shmptr,int table_number,int num_of_customers){
    //creating customer processes and seperate pipe for customer and table process
    for(int i=0;i<num_of_customers;i++){
        customer_order(i);
        wait(NULL);
    }

    int j=3;
    shmptr[0]=num_of_customers;
    for (int i=0; i<num_of_customers; i++) {
        int status;
        waitpid(customer_pids[i], &status, 0);
        int order_num;
        int check=0;
        printf("\nOrder recieved from Customer %d: ", i + 1);

        while (read(pipe_handler[i][READ_END], &order_num, sizeof(order_num)) > 0 ) {
            if(order_num!=-1){
                printf("%d ", order_num);
                shmptr[j++]=order_num;
                check = 1;
            }
        }
        // shmptr[1]=j-3;
        // shmptr[2]=1;
        if(!check) printf("No order recieved from customer %d",i+1);
        printf("\n");
        close(pipe_handler[i][READ_END]);
    }
    shmptr[1]=j-3;
    shmptr[2]=1;
    printf("\nOrders for Table %d:received\n", table_number);
    
    printf("MESSAGE STORED\n");
    printf("<---------------------------------------------------------->\n");
    printf("Waiting for the bill from the waiter...\n");
    
    sleep(20);
    printf("shm status: ");
    for(int i=0;i<shmptr[1]+3;i++){
        printf("%d ",shmptr[i]);
    }
    printf("\n");
    if(shmptr[1]!=100)printf("The bill amount is: %d INR\n",shmptr[0]);
    else {
        // take order from customer again
        printf("\nInvalid order! Please enter again.\n");
        memset(shmptr,0,SHM_SIZE);
        customer_create(shmptr,table_number,num_of_customers);
    }
}

int main(){
    int table_number;
    printf("Enter Table Number:");
    scanf("%d",&table_number);

    // ftok to generate unique key for shared memory
    key_t key=ftok("table.c",table_number);
    if(key==-1){
        printf("error in ftok\n");
        return 1;
    }

    int shmid=shmget(key, 1024, 0666 | IPC_CREAT);
    if(shmid<0){
        perror("shmget error");
        exit(1);
    }

    int *shmptr=(int *)shmat(shmid, (void*)0, 0);
    if(shmptr==(int *)(-1)){
        perror("shmptr error");
        exit(1);
    }
    memset(shmptr,0,SHM_SIZE);

    while(1){
        //asking the number of customers
        int num_of_customers;
        printf("Enter Number of Customers at Table (maximum no. of customers can be 5):");
        scanf("%d",&num_of_customers);
        if(num_of_customers==-1)return 0;//exit

        //displaying the contents of the menu
        display_menu();

        customer_create(shmptr,table_number,num_of_customers);
        
        memset(shmptr,0,SHM_SIZE);

        printf("Do you wish to seat a new set of customers?(Enter 'Y' or 'N')...........");
        char ch;
        scanf("%c",&ch);
        if(ch=='Y'){
            // shmptr[0]=101;
            shmptr[2]=0;
            continue;
        }
        else if(ch=='N'){shmptr[0]=102;break;}
    }

    shmdt(shmptr);
    shmctl(shmid,IPC_RMID,NULL);

    return 0;
}