#include <stdio.h> 
#include <stdlib.h> 
#include "Event.h"
#include "fifo.h" 
#include "pq.h"
  //global variables
int SEED;
int INIT_TIME;
int FIN_TIME;
int ARRIVE_MIN;
int ARRIVE_MAX;
float QUIT_PROB;
int CPU_MIN;
int CPU_MAX;
int DISK1_MIN;
int DISK1_MAX;
int DISK2_MIN;
int DISK2_MAX;
float NETWORK_PROB;
int NETWORK_MIN;
int NETWORK_MAX;

int currTime;
int currEvent;
int currJobID;

//Booleans 0 = idle 1 = in use
 int cpuStatus =0;
 int disk1Status =0;
 int disk2Status =0;
 int networkStatus =0;

int clock;
int jobNum =1;

//jobs set
int job_arrival =1;
  int job_arrive_cpu = 2;
  int job_exit_cpu = 3;
  int job_terminates = 4;
  int job_arrive_disk1 = 5;
  int job_exit_disk1 = 6;
  int job_arrive_disk2 = 7;
  int job_exit_disk2 = 8;
  int job_arrive_network = 9;
  int job_exit_network = 10;
  int jobs_done = 11;


//methods
 int rng(int lower, int upper);
 void printSimulation(int time, int jobID, int eventype);
 int max(int num1, int num2);
 
 FILE *fp;
 FILE *sfp;

 //STAT variables
 float CPUT;
 float diskT;
 float disk2T;
 float networkT;


 int totaldisk1time = 0;
 int totalCPUtime = 0;
 int totaldisk2time = 0;
 int totalNetworktime = 0;
 
 int CPUtotakchange =0;
 int disk1totakchange =0;
 int disk2totakchange =0;
 int networktotakchange =0;
 
 int CPUMaxsize = 0;
 int disk1Maxsize = 0;
 int disk2Maxsize = 0;
 int networkMaxsize = 0;


 
 int CPUsize = 0;
 int disk1size = 0;
 int disk2size = 0;
 int networksize = 0;

int CPUResponseTime =0;
int d1ResponseTime =0;
int d2ResponseTime =0;
int netResponseTime =0;

int MaxCPUresponses = 0;
int Maxd1responses = 0;
int Maxd2responses = 0;
int Maxnetresponses = 0;

int prevoiusCPUfinish = 0;
int prevoiusd1finish = 0;
int prevoiusd2finish = 0;
int prevoiusnetfinish = 0;

int cpuJobs = 0;
int d1Jobs = 0;
int d2Jobs= 0;
int netJobs= 0;


int main(void) {
 

 fp = fopen("LOG.txt", "w");
  sfp = fopen("STAT.txt", "w");



//Store Config file variables
FILE *config = fopen("config.txt", "r");
char currentC[58];

fscanf(config,"SEED %d",&SEED);
fgets(currentC, 58, config);
fscanf(config,"INIT_TIME %d",&INIT_TIME);
fgets(currentC, 58, config);
fscanf(config,"FIN_TIME %d",&FIN_TIME);
fgets(currentC, 58, config);
fscanf(config,"ARRIVE_MIN %d",&ARRIVE_MIN);
fgets(currentC, 58, config);
fscanf(config,"ARRIVE_MAX %d",&ARRIVE_MAX);
fgets(currentC, 58, config);
fscanf(config,"QUIT_PROB %f",&QUIT_PROB);
fgets(currentC, 58, config);
fscanf(config,"CPU_MIN %d",&CPU_MIN);
fgets(currentC, 58, config);
fscanf(config,"CPU_MAX %d",&CPU_MAX);
fgets(currentC, 58, config);
fscanf(config,"DISK1_MIN %d",&DISK1_MIN);
fgets(currentC, 58, config);
fscanf(config,"DISK1_MAX %d",&DISK1_MAX);
fgets(currentC, 58, config);
fscanf(config,"DISK2_MIN %d",&DISK2_MIN);
fgets(currentC, 58, config);
fscanf(config,"DISK2_MAX %d",&DISK2_MAX);
fgets(currentC, 58, config);
fscanf(config,"NETWORK_PROB %f",&NETWORK_PROB);
fgets(currentC, 58, config);
fscanf(config,"NETWORK_MIN %d",&NETWORK_MIN);
fgets(currentC, 58, config);
fscanf(config,"NETWORK_MAX %d",&NETWORK_MAX);

fclose(config);


// add initial and last event
clock = INIT_TIME;
struct event a;
a.time=INIT_TIME; //temp
a.event_type= job_arrival ;
a.job_id = jobNum;
jobNum++;
struct event z;
z.time = FIN_TIME; //temp
z.event_type= jobs_done;
z.job_id = -1;


//FIFO queues
queue* CPU =  malloc(sizeof(queue));
queue* disk1= malloc(sizeof(queue));;
queue* disk2 = malloc(sizeof(queue));;
queue* network = malloc(sizeof(queue));;


initialize(CPU);
//printf("%d",CPU->count);
initialize(disk1);
initialize(disk2);
initialize(network);



 //Initial Seedind
  srand(SEED); 
  //srand(time(0));
 
  //printf(" %d" ,rng(2,5));

//First initialization and push

Node* pQueue = initialize1(a.event_type, a.job_id, a.time);
push(&pQueue, z.event_type, z.job_id , z.time);

//enqueue(CPU, currJobID);
fprintf(fp, "Starting Simulation\n");
while(!isEmpty(&pQueue) && getTime(&pQueue)< FIN_TIME){

//printf("PLease %d",getjob_id(&currentEvent));
currEvent = getEvent_id(&pQueue);
currTime = getTime(&pQueue);
currJobID = getjob_id(&pQueue);


//printf("jk %d\n",currJobID);
//printf("\n EventId  %d\n",eventype);

printSimulation(currTime, currJobID, currEvent);

printf("\nTime %d : Job %d : Event %d \n",currTime,currJobID,currEvent);
clock = currTime;

pop(&pQueue);


 switch(currEvent){
   case 1:
   {
     if(cpuStatus == 1){
       enqueue(CPU, currJobID);
       
       CPUMaxsize++;
       CPUsize ++;
       CPUtotakchange += CPU->count;
      // printf("\nstatus 1 or empty\n");
     }
     else{
    //enqueue(CPU, currJobID);

    
  // currJobID = dequeue(CPU);
   struct event newP;
   newP.time = currTime;
   newP.event_type= job_arrive_cpu;
  
   newP.job_id = currJobID;
   push(&pQueue, newP.event_type, newP.job_id, newP.time);
   cpuStatus=1;
   //enqueue(CPU, currJobID);
      // printf("\nCount %d\n",CPU->count);
   
    //printf("emoty");
  }
  //Might no be important
  /*else{
    struct event cpuArrival;
    cpuArrival.event_type = job_arrive_cpu;
    cpuArrival.time = currTime;
    
    cpuArrival.job_id = currJobID;

    //printf("\nJob id %d\n",getjob_id(&currentEvent));
    //printf("\nJ Time %d\n",getTime(&currentEvent));
    //printf("\nEvent%d\n",getEvent_id(&currentEvent));


    cpuStatus =1;
    push(&pQueue, cpuArrival.event_type, cpuArrival.job_id, cpuArrival.time);

  }*/
  //Make sure jobs flow in
  struct event newArrival;

  newArrival.job_id = jobNum;
  jobNum++;
  newArrival.time = currTime + rng(ARRIVE_MIN,ARRIVE_MAX);
  newArrival.event_type = job_arrival;
   push(&pQueue, newArrival.event_type, newArrival.job_id, newArrival.time);
   
   }
 
  
  //printf(" top event type %d\n", getEvent_id(&pQueue));
   break;
   case 2: 
 {
   struct event finish;

  
  finish.time = currTime + rng(CPU_MIN,  CPU_MAX);
  if(currTime== 0){
  CPUResponseTime += (finish.time -currTime) ;
  
  }
  else{
CPUResponseTime += (currTime-prevoiusCPUfinish);
 CPUResponseTime += (finish.time -currTime );

  }
  prevoiusCPUfinish = finish.time;
  MaxCPUresponses = max(MaxCPUresponses, CPUResponseTime);
  
 totalCPUtime += (finish.time -currTime) ;


  finish.job_id = currJobID;
 // printf("CPU count %d",finish.job_id);
  finish.event_type = job_exit_cpu;
  cpuJobs ++;
  
  push(&pQueue, finish.event_type, finish.job_id, finish.time);
  //printf("\nHey\n");
   //cpuStatus= 1;
  }
 
   
   //handle_job_arrive_cpu(pQueue);
   break;
   case 3:
   {
     cpuStatus = 0;
    float prob =(float) rng(0,100)/100;
    //printf("float %f", prob);
    if(prob <= QUIT_PROB){
      struct event termin;
      termin.job_id = currJobID;
      termin.time = currTime;
      termin.event_type = job_terminates;
      push(&pQueue, termin.event_type, termin.job_id, termin.time);
      break;
    }
    else{
      float prob =(float) rng(0,100)/100;
      if(prob <= NETWORK_PROB){
        if(networkStatus ==1){
          enqueue(network, currJobID);
          networkMaxsize++;
          networktotakchange += network->count;
           networksize ++;
        }
        else{
        struct event goNet;
      goNet.job_id = currJobID;
      goNet.time = currTime ;
      goNet.event_type = job_arrive_network;
      push(&pQueue, goNet.event_type, goNet.job_id, goNet.time);
      networkStatus =1;
      }
      break;
      }
      else{
        if(disk1->count <= disk2->count ){
        if(disk1Status ==1){
          enqueue(disk1, currJobID);
          disk1Maxsize++;
          disk1totakchange += disk1->count;
           disk1size ++;
        }
        else{
        struct event goDisk1;
        goDisk1.job_id = currJobID;
        goDisk1.time = currTime;
        goDisk1.event_type = job_arrive_disk1;
       push(&pQueue, goDisk1.event_type, goDisk1.job_id, goDisk1.time);
       disk1Status = 1;
        }
       break;
        }
      
        
       else if(disk2->count < disk1->count ) {
         if(disk2Status ==1){
          enqueue(disk2, currJobID);
          disk2Maxsize++;
          disk2totakchange += disk2->count;
           disk2size ++;
        }
        else{
         struct event goDisk2;
        goDisk2.job_id = currJobID;
        goDisk2.time = currTime;
        goDisk2.event_type = job_arrive_disk2;
       push(&pQueue, goDisk2.event_type, goDisk2.job_id, goDisk2.time);
        disk2Status = 1;

        }
       break;
       }
      

      }
    }
    

   }
   break;
   case 4:
   {
      dequeue(CPU);
      
       //CPUResponseTime += (currTime);
      CPUMaxsize--;
      CPUtotakchange += CPU->count;
      CPUsize++;
      struct event newArrival;

  newArrival.job_id = jobNum;
  jobNum++;
  newArrival.time = currTime + rng(ARRIVE_MIN,ARRIVE_MAX);
  newArrival.event_type = job_arrival;
  push(&pQueue, newArrival.event_type, newArrival.job_id, newArrival.time);
  cpuStatus =1;
      

   }
   break;
   case 5:
   {
    struct event findisk1;
        findisk1.job_id = currJobID;
        findisk1.time = currTime +rng(DISK1_MIN,DISK1_MAX);
        totaldisk1time  +=  (findisk1.time -currTime);
        findisk1.event_type = job_exit_disk1;
      
        d1Jobs++;

    if(currTime == prevoiusCPUfinish){
   d1ResponseTime +=( findisk1.time -currTime) ;
   
  }
  else{
   d1ResponseTime += (currTime-prevoiusd1finish);
   
  }
  prevoiusd1finish= findisk1.time;
Maxd1responses = max(Maxd1responses, d1ResponseTime);
       push(&pQueue, findisk1.event_type, findisk1.job_id, findisk1.time);
   //printf("Arrived disk 1");
   }
   //handle_job_arrive_disk1();
   break;
   case 6:
   {
 disk1Status =0;
 if(cpuStatus == 1){
   enqueue(disk1, currJobID);
   disk1Maxsize++;
    disk1size ++;
    disk1totakchange += disk1->count;
   //printf("Cpu bsy\n");
 }
 else if(cpuStatus == 0 && isempty(disk1)){
   struct event newP;
   newP.time = currTime;
   newP.event_type= job_arrive_cpu;
  
   newP.job_id = currJobID;
   push(&pQueue, newP.event_type, newP.job_id, newP.time);
   cpuStatus=1;
     // printf("Cpu not bsy\n");
   
 }
 else if( !isempty(disk1)&& cpuStatus == 0){
   //dequeue(disk1);
   struct event goDisk1;
        goDisk1.job_id = dequeue(disk1);
        disk1Maxsize--;
        disk1totakchange += disk1->count;
        disk1size++;
        goDisk1.time = currTime;
        goDisk1.event_type = job_arrive_disk1;
       push(&pQueue, goDisk1.event_type, goDisk1.job_id, goDisk1.time);
       disk1Status = 1;
       //printf("other");

 }
   }
   //handle_job_exit_disk1();
   break;
   case 7: 
{
    struct event findisk2;
        findisk2.job_id = currJobID;
        findisk2.time = currTime +rng(DISK2_MIN,DISK2_MAX);
        totaldisk2time  +=  (findisk2.time -currTime );
        findisk2.event_type = job_exit_disk2;
        d2Jobs++;
         if(currTime == prevoiusCPUfinish){
   d2ResponseTime += (findisk2.time -currTime) ;
  }
  else{
   d2ResponseTime += (currTime-prevoiusd2finish);
   d2ResponseTime += (findisk2.time -currTime) ;
  }
  prevoiusd2finish= findisk2.time;
    Maxd2responses = max(Maxd2responses, d2ResponseTime);

      
       push(&pQueue, findisk2.event_type, findisk2.job_id, findisk2.time);

   }
   //handle_job_arrive_disk2();
   break;
   case 8:
    {
 disk2Status =0;
 if(cpuStatus == 1){
   enqueue(disk2, currJobID);
   disk2Maxsize++;
   disk2totakchange += disk2->count;
    disk2size ++;
   //printf("Cpu bsy\n");
 }
 else if(cpuStatus == 0 && isempty(disk2)){
   struct event newP;
   newP.time = currTime;
   newP.event_type= job_arrive_cpu;
   
   newP.job_id = currJobID;
   push(&pQueue, newP.event_type, newP.job_id, newP.time);
   cpuStatus=1;
      //printf("Cpu not bsy\n");
   
 }
 else if( !isempty(disk2)&& cpuStatus == 0){
   //dequeue(disk2);
   struct event godisk2;
        godisk2.job_id = dequeue(disk2);
        disk2Maxsize--;
        disk2totakchange += disk2->count;
        disk2size++;
        godisk2.time = currTime;
        godisk2.event_type = job_arrive_disk2;
       push(&pQueue, godisk2.event_type, godisk2.job_id, godisk2.time);
       disk2Status = 1;
       printf("other");

 }
   }
   //handle_job_exit_disk2();
   break;
   case 9:
   {
    struct event gonet;
        gonet.job_id = currJobID;
        gonet.time = currTime + rng(NETWORK_MIN,NETWORK_MAX);
        totalNetworktime  +=  (gonet.time -currTime );
        gonet.event_type = job_exit_network;
        netJobs++;

         if(currTime == prevoiusCPUfinish){
   netResponseTime +=( gonet.time -currTime) ;
  }
  else{
   netResponseTime += (currTime-prevoiusd1finish);
   netResponseTime += (gonet.time -currTime) ;
  }
  prevoiusnetfinish= gonet.time;
    Maxnetresponses = max(Maxnetresponses, netResponseTime);

       push(&pQueue, gonet.event_type, gonet.job_id, gonet.time);


   }
  // handle_job_arrive_network();
   break;
   case 10:
   networkStatus = 0;

 if(cpuStatus == 1){
   enqueue(network, currJobID);
   networkMaxsize++;
   networktotakchange += network->count;
    networksize ++;
   //printf("Cpu bsy for net\n");
 }
 else if(cpuStatus == 0 && isempty(network)){
   struct event newP;
   newP.time = currTime;
   newP.event_type= job_arrive_cpu;
  
   newP.job_id = currJobID;
   push(&pQueue, newP.event_type, newP.job_id, newP.time);
   cpuStatus=1;
     // printf("Cpu not bsy for net\n");
   
 }
 else if( !isempty(network)&& cpuStatus == 0){
   //dequeue(disk2);
   struct event goNet;
        goNet.job_id = dequeue(network);
        networkMaxsize--;
        networktotakchange += network->count;
        networksize++;
        goNet.time = currTime;
        goNet.event_type = job_arrive_network;
       push(&pQueue, goNet.event_type, goNet.job_id, goNet.time);
       networkStatus = 1;
     //  printf("other net");

 }

   //handle_job_exit_network();
   break;
   case 11:
   {
     
     return 0;
     }
   //handle_jobs_done();
   break;



 }
}
//CPUResponseTime += totalCPUtime;


printf("\n\nTime on component vs time in use  %d | %d\n" , CPUResponseTime,totalCPUtime);
printf("Time on component vs time in use  %d | %d\n" , d1ResponseTime,totaldisk1time);
printf("Time on component vs time in use   %d | %d\n" , d2ResponseTime,totaldisk2time);
printf("Time on component vs time in use   %d | %d\n" , netResponseTime,totalNetworktime);

printf("countCPUQ :%d\ncountdisk1Q :%d\ncountdisk2Q :%d\ncountNetworkQ :%d\n ",CPU->count,disk1->count,disk2->count,network->count);
fprintf(fp, "Simulation Ended\n");
fprintf(sfp, "-nan = 0\n\n");
fprintf(sfp, "          QUEUE MAX AND AVERAGES\n");

fprintf(sfp, "Average size of CPU queue: %f\n",(float)CPUtotakchange/CPUsize);
fprintf(sfp, "Max size of CPU queue: %d\n",CPUMaxsize);
fprintf(sfp, "Average size of disk 1 queue: %f\n",(float)disk1totakchange/disk1size);
fprintf(sfp, "Max size of disk 1 queue: %d\n",disk1Maxsize);
fprintf(sfp, "Average size of disk 2 queue: %f\n",(float)disk2totakchange/disk2size);
fprintf(sfp, "Max size of disk 2 queue: %d\n",disk2Maxsize);
fprintf(sfp, "Average size of Network queue: %f\n",(float)networktotakchange/networksize);
fprintf(sfp, "Max size of Network queue: %d\n",networkMaxsize);


fprintf(sfp, "\n          UTILIZATION\n");
fprintf(sfp, "Utilization of CPU %f\n",(float)totalCPUtime/currTime);
fprintf(sfp, "Utilization of disk 1 %f\n",(float)totaldisk1time/currTime);
fprintf(sfp, "Utilization of disk 2 %f\n",(float)totaldisk2time/currTime);
fprintf(sfp, "Utilization of network %f\n",(float)totalNetworktime/currTime);
fprintf(sfp, "\n          ResponseTime and Maxes\n");
fprintf(sfp, "CPU response time Average :  %f\n",(float)CPUResponseTime/cpuJobs);
fprintf(sfp, "CPU response time Max :  %d\n",MaxCPUresponses);
fprintf(sfp, "disk 1 response time Average :  %f\n",(float)d1ResponseTime/d1Jobs);
fprintf(sfp, "disk 1 response time Max :  %d\n",Maxd1responses);
fprintf(sfp, "disk 2 response time Average :  %f\n",(float)d2ResponseTime/d2Jobs);
fprintf(sfp, "disk 2 response time Max :  %d\n",Maxd2responses);
fprintf(sfp, "network response time Average :  %f\n",(float)netResponseTime/netJobs);
fprintf(sfp, "network response time Max :  %d\n",Maxnetresponses);



fprintf(sfp, "\n          Throughputs\n");
fprintf(sfp, "Cpu throughput  %f\n",(float)cpuJobs/totalCPUtime);
fprintf(sfp, "Disk 1 throughput  %f\n",(float)d1Jobs/totaldisk1time);
fprintf(sfp, "Disk 2 throughput  %f\n",(float)d2Jobs/totaldisk2time);
fprintf(sfp, "Network throughput  %f\n",(float)netJobs/totalNetworktime);







fclose(fp);
fclose(sfp);
return 0;
}


int rng(int min, int max){
 
    
       int RandomNum = (rand() % (max - min + 1)) + min; 
        return RandomNum; 
     
}

void printSimulation(int time, int JobID, int eventType){
  
 
  switch(eventType){
  case 1: 
  fprintf(fp,"At %d, job %d arrives the system\n",time, JobID);
  break;
  case 2: 
  fprintf(fp,"At %d, job %d arrives the CPU\n",time, JobID);
  break;
  case 3: 
  fprintf(fp,"At %d, job %d exits the CPU\n",time, JobID);
  break;
  case 4: 
  fprintf(fp,"At %d, job %d terminates\n",time, JobID);
  break;
  case 5: 
  fprintf(fp,"At %d, job %d arrives disk 1\n",time, JobID);
  break;
  case 6: 
  fprintf(fp,"At %d, job %d exits disk 1\n",time, JobID);
  break;
  case 7:
   fprintf(fp,"At %d, job %d arrives disk 2\n",time, JobID);
  break;
  case 8: 
  fprintf(fp,"At %d, job %d exits disk 2\n",time, JobID);
  break;
  case 9: 
  fprintf(fp,"At %d, job %d arrives Network\n",time, JobID);
  break;
  case 10: 
  fprintf(fp,"At %d, job %d exit Network\n",time, JobID);
  break;
  case 11: 
  fprintf(fp,"At %d, job %d exits sytem\n",time, JobID);
  break;

  }

   //fprintf(fp, "This is testing for fprintf...\n");
   //fputs("This is testing for fputs...\n", fp)


} 
int max(int a, int b){
if(a>b){
return a;
}
else
{
return b;
}

}


