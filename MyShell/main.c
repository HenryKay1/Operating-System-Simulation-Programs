#include <stdio.h>
#include <string.h>
#include<unistd.h>
#include <dirent.h>   
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//Methods
char* runDir(char dir[20], int argNum);
char* runEnviron();
char* runHelp();
char* runEcho();
void runPause();

//Struct to store command and its attributes
struct Command{   // c means command

int cType;  //i.e Biult in or external
char cName[20];
 char *cArguments[15];
 char *apArguments[15];//after pipe arguments
 int argNum[15]; //track argments for backgrond
 
char Ofile[200]; //output file
char Ifile[200]; //input file
char Otype[2]; // Output type
int pipeP; // Pipe present i.e if pipe is present or not
char bPipe[20]; // Command before pipe
char aPipe[20]; // Command agter pipe
int bgP; //Background present
 char *bgCommands[15]; 

} ;

int main(int argc, char ** argv) {
/*if(argc >= 2){
//FILE *fptr;
int fptr = open(argv[1], O_RDONLY );
//fptr = open(argv[1], "r");
if(fptr == -1){
  printf("Batch file does not exist");
}
else{
char *line = NULL;
size_t len = 0;
ssize_t readLine;
dup2(fptr, 0);
while ((readLine = getline(&line, &len, stdin)) != -1) {
 //printf("Retrieved line of length %zu:\n", readLine);
 printf(line);


}

}
}*/

//Get line read variables 

char *line = NULL; // command input
size_t len = 0;
ssize_t read = 0;
int fptr;
int batchP =0;// Batch present flag
int invalidC; //batch and reglular conditions use this variable
int invalidA; //batch and reglular conditions use this variable
if(argc >= 2){
  fptr = open(argv[1], O_RDONLY );
  batchP =1;
//fptr = open(argv[1], "r");
if(fptr == -1){
  printf("Batch file does not exist");
return 0;
 
}
else{

dup2(fptr, 0); //redirect stdin to batch file
}
}








 //Get Command line
 if(argc == 1){
 printf("myShell> ");
 }
 else{
  printf("\n");
 }
 int ac =0;
 if(batchP){
 invalidC = 0; //Invalid Command flag 
 invalidA =0;
 }

while((read = getline(&line, &len, stdin) != -1) ) {

//printf("seg : %d %d\n", getpid(), invalidC);
ac++;
 
//Tokenizing with whitespace delimeters
const char delimeter[4] = " \t\n"; 
char *token = strtok(line, delimeter); 

//making sure commands attributes are added in command struct with no errors
//0 for biult in , 1 for external
int biultin = 0;
int external = 1;
//Command state variables
int state;
int cs = 0; 
int arg1 =1; 
int csE =2; //External Commad state 
int arg2 = 3;
int Ofiles =4; // Output file
int Ifiles = 5;// input file
int EXTERNAL =6;
int stop =0; // add string offset to external aguments to suit execvp

state = cs; //set initial state to command  state
struct Command command; // Initialize some int struct attributes to 0

command.pipeP =0;
command.bgP = 0;
stpcpy(command.bPipe, "0");
stpcpy(command.aPipe, "0");
stpcpy(command.Ofile, "0");
stpcpy(command.Otype, "0");
strcpy(command.Ifile,"0");

//reset counts after command is processed
int aCount =0; //argument count
int bgCount =0; //Background command count
int csCount = 0; // Command state count
int dirCount =0;


int done = 0; //boolean 0 is false, 1 is true . Continue processing flag
int caCount =0; // Current argument count before &
int over =0; //set limit for directory arguments
if(!batchP){
  invalidC =0;
  invalidA = 0; // Invalid Arguments
}
//int invalidC = 0; //Invalid Command flag 

int total =1;





while(token != NULL && !invalidA && !invalidC){


if (done){
printf(" Invalid Command");
invalidC =1;
//token = NULL;
 }

else if(state == cs){
csCount++;
  

 
  if((!strcmp(token,"help") || !strcmp(token,"pause") || !strcmp(token,"quit") || !strcmp(token,"environ")|| !strcmp(token,"dir") || !strcmp(token,"cd") || !strcmp(token,"echo") || !strcmp(token,"clr")) && !command.pipeP){
   // printf("biukt in\n");
  
  command.cType = biultin;
  strcpy(command.cName, token);
 char t[30] = "dd";
 //t = malloc(12);
 //char z[12] = "fff";

  


  state = arg1;
//printf(command.cName);
 }
else if((!strcmp(token,"help") || !strcmp(token,"pause") || !strcmp(token,"quit") || !strcmp(token,"environ")|| !strcmp(token,"dir") || !strcmp(token,"cd") || !strcmp(token,"echo") || !strcmp(token,"clr")) && command.pipeP){
 // printf(" INvalid Pipe");
invalidC = 1;


}
 else {
   int check = 1;

   // External check
   
  






if(check && command.pipeP){
command.cType = external;
strcpy(command.aPipe, token);
done = 1;
  //printf("d\n");
  state = arg2;



 
}
else if(check && !command.pipeP){
  command.cType = external;
  state = arg2;
  strcpy(command.cName, token);
  //printf("c");
  //printf(" state/token %d %s\n",state,token);
}
else{
  printf("Invalid command External");
  invalidC = 1;
}
   
 }
}
else if( state == arg1){
  //printf(token);
  //printf(command.cName);
  if(!(strcmp(token,"<"))){
    printf("No inputs here");
    invalidA =1; 
  }
  else if( ((!strcmp(token, ">")) || (!strcmp(token, ">>")) || (!strcmp(token, "|")) ) && command.bgP == 1){
   printf(" Invalid Argument");
    invalidA =1;
  }
 
 else if(!strcmp(command.cName, "quit")){
   printf(" Invalid Argument"); 
   invalidA =1;
   //printf(command.bgCommands[0]);
   

 }
 
 else if(!strcmp(command.cName, "pause")){
   printf(" Invalid Argument"); 
    invalidA =1;
  }
  else if(((!strcmp(command.cName, "help")) || (!strcmp(command.cName, "environ"))) && ((!strcmp(token, ">")) || (!strcmp(token, ">>"))) && command.bgP ==0){
   state = Ofiles;
   if (strcmp(token, ">")){
     strcpy(command.Otype, ">>");
   }
   else{
     strcpy(command.Otype, ">");
   }
  }
  
  
  else if(((!strcmp(command.cName, "help")) || (!strcmp(command.cName, "environ"))) && ((!strcmp(token, "|"))  && command.bgP ==0)){
   command.pipeP = 1;
   strcpy(command.bPipe, command.cName);
   state = cs;
  // printf("haha %s", command.bPipe);
  }
  else if(((!strcmp(command.cName, "help")) || (!strcmp(command.cName, "environ"))) && (!strcmp(token, "&"))) {
   command.bgP = 1;
   command.bgCommands[bgCount] = malloc(4);
   
   strcpy((char*)command.bgCommands[bgCount], command.cName);
   bgCount++;
   state = cs;
  
 // printf("helpbg %s\n", command.bgCommands[bgCount-1]);
  }

  else if(((!strcmp(command.cName, "help")) || (!strcmp(command.cName, "environ"))) && ((strcmp(token, ">")) || (strcmp(token, ">>")) || (strcmp(token, "&")) || (strcmp(token, "|")))){
   printf(" Invalid Argument "); 
    invalidA =1;
  }

  else if((!strcmp(command.cName, "echo")) && ((!strcmp(token, ">")) || (!strcmp(token, ">>")))  && command.bgP ==0){
    state = Ofiles;
     if (strcmp(token, ">")){
     strcpy(command.Otype, ">>");
   }
   else{
     strcpy(command.Otype, ">");
   }

  }
  else if((!strcmp(command.cName, "echo")) && (!strcmp(token, "|")   && command.bgP ==0)){
   command.pipeP = 1;
   strcpy(command.bPipe, command.cName);
   state = cs;

  }
  else if((!strcmp(command.cName, "echo")) && (!strcmp(token, "&")) ){
    command.bgP = 1;
    command.bgCommands[bgCount] = malloc(4);
   
   strcpy((char*)command.bgCommands[bgCount], command.cName);
   bgCount++;
   state = cs;

   // printf("Echobg %s\n", command.bgCommands[bgCount -1]);

  }
  else if((!strcmp(command.cName, "echo")) && ((strcmp(token, ">")) || (strcmp(token, ">>")) || (strcmp(token, "|")) || (strcmp(token, "&")) || (strcmp(token, "<")) )  && command.bgP ==0){
  command.cArguments[aCount] = malloc(4);
   
   strcpy((char*)command.cArguments[aCount], token);
   aCount++;
 //printf("Echargs %s\n", command.cArguments[aCount-1]);

  }
  else if((!strcmp(command.cName, "echo")) && ((!strcmp(token, ">")) || (!strcmp(token, ">>")))  && command.bgP ==0){
    state = Ofiles;
     if (strcmp(token, ">")){
     strcpy(command.Otype, ">>");
   }
   else{
     strcpy(command.Otype, ">");
   }

  }
 
 else if((!strcmp(command.cName, "clr"))){
printf("Invalid Argument");
invalidA = 1;
 }
 else if((!strcmp(command.cName, "cd")) && ((!strcmp(token, ">")) || (!strcmp(token, ">>")) || (!strcmp(token, "|")) || (!strcmp(token, "&")) )){


printf("Invalid Argument");
invalidA = 1; 
 }

 else if((!strcmp(command.cName, "cd")) && ((strcmp(token, ">")) || (strcmp(token, ">>")) || (strcmp(token, "|")) || (strcmp(token, "&")) )  && over ==0){

//check if directort exists
int dExists = 0;
if(!dExists){
  command.cArguments[aCount] = malloc(4);
   
   strcpy((char*)command.cArguments[aCount], token);
   aCount++;
 //printf("cdargs %s\n", command.cArguments[aCount-1]);
 over = 1;


}
else{
  printf("Directory %s doesn't exist",token);
  invalidC=1;
  
}

 }
 else if((!strcmp(command.cName, "cd")) && ((strcmp(token, ">")) || (strcmp(token, ">>")) || (strcmp(token, "|")) || (strcmp(token, "&")) )  && over ==1){

   printf("Too many arguments");
   invalidA = 1;

 }

 else if((!strcmp(command.cName, "dir")) && ((!strcmp(token, ">")) || (!strcmp(token, ">>"))) ){
    state = Ofiles;
     if (strcmp(token, ">")){
     strcpy(command.Otype, ">>");
   }
   else{
     strcpy(command.Otype, ">");
   }

 }
 else if((!strcmp(command.cName, "dir")) && ((!strcmp(token, "|")))  && command.bgP ==0){
   command.pipeP = 1;
   strcpy(command.bPipe, command.cName);
   state = cs;
 //printf("dirPipe %s\n", command.bPipe);
  }

  else if((!strcmp(command.cName, "dir")) && ((!strcmp(token, "&"))) ){
    command.bgP = 1;
   command.bgCommands[bgCount] = malloc(4);
   
   strcpy((char*)command.bgCommands[bgCount], command.cName);
   bgCount++;
   state = cs;
//printf("argDir %s\n", command.bgCommands[bgCount-1]);
  }

else if((!strcmp(command.cName, "dir")) && ((strcmp(token, ">")) || (strcmp(token, ">>")) || (strcmp(token, "|")) || (strcmp(token, "&")) )  && over ==0){
dirCount = 1;
//check if directory exists
int dExists = 0;
if(!dExists){
  command.cArguments[aCount] = malloc(4);
   
   strcpy((char*)command.cArguments[aCount], token);
   aCount++;
// printf("argsDir %s\n", command.cArguments[aCount-1]);
 over = 1;


}
else{
  printf("Directory %s doesn't exist",token);
  invalidC=1;
}

 }
 else if((!strcmp(command.cName, "dir")) && ((strcmp(token, ">")) || (strcmp(token, ">>")) || (strcmp(token, "|")) || (strcmp(token, "&")) )  && over ==1){

   printf("Too many arguments");
   invalidA = 1;
  
 }


}


else if(state == arg2){ //New state
//printf("WE");

if (done){
  printf("Invalid Command");
  invalidC =1;
}
 
 else if((!strcmp(token, ">") || !strcmp(token, ">>") || !strcmp(token, "|") || !strcmp(token, "&") || !strcmp(token, "<")) && command.pipeP){
printf("Invalid Command");
invalidC = 1;
 }
 else if((!strcmp(token, ">") || !strcmp(token, ">>") || !strcmp(token, "|") || !strcmp(token, "<") ) && command.bgP){
  printf("Invalid Command");
invalidC = 1;
 }
 else if((!strcmp(token, ">") || !strcmp(token, ">>") ) && !command.bgP){
  state = Ofiles;

   if (strcmp(token, ">")){
     strcpy(command.Otype, ">>");
   }
   else{
     strcpy(command.Otype, ">");
   }
 }
 else if(!strcmp(token, "<")  && !command.bgP){
  state = Ifiles;
  //printf("111111");
 }
 else if(!strcmp(token, "|")  && !command.pipeP  && !command.bgP ){
  strcpy(command.bPipe, command.cName);
  command.pipeP =1;
  state = cs;
 }
 else if(!strcmp(token, "&") ){
   stop = 0;
    // current argument count;

  //command.argNum[bgCount] = malloc(sizeof(int)); //Track number of arguments for Background
 command.argNum[bgCount] = aCount - caCount;
 caCount = aCount; //Update current count
 //printf("New array: %d\n",command.argNum[bgCount]);
   command.bgP = 1;
   command.bgCommands[bgCount] = malloc(4);
   
   strcpy((char*)command.bgCommands[bgCount], command.cName);
   bgCount++;
   state = cs;
  
  //printf("ExtPipe %s\n", command.bgCommands[bgCount-1]);
 }
 else if((strcmp(token, ">") || strcmp(token, ">>") || strcmp(token, "|") || strcmp(token, "&") || strcmp(token, "<"))){
  
  if(!stop){
 command.cArguments[aCount] = malloc(4);
  strcpy((char*)command.cArguments[aCount], command.cName);
  aCount++;

  command.cArguments[aCount] = malloc(4);
  strcpy((char*)command.cArguments[aCount], token);
   aCount++;
   stop = 1;  // if statement runs once to ass command to offset string
  }
  else{
     command.cArguments[aCount] = malloc(4);
  strcpy((char*)command.cArguments[aCount], token);
   aCount++;
  }
 //printf("Extargs %s\n", command.cArguments[aCount-1]);

 }

}
else if( state == Ofiles){ // New State

  strcpy(command.Ofile, token);
 // printf("otype %s",command.Otype);
 
}
else if( state == Ifiles){
  
  FILE *fp;
 fp = fopen(token, "r");
 if(fp == NULL){
   printf("File %s does not exist",token);
   invalidC = 1;
   //strcpy(command.Ifile, "-1"); 
 }
 else{
  strcpy(command.Ifile, token); 
  
 }
}

//get next token
//printf("%d|%d \n",bgCount,csCount);
token = strtok(NULL, delimeter); 
 

} //token while loop
// Check for incomplete commands

 if(strcmp(command.bPipe, "0") && !strcmp(command.aPipe, "0")){
  invalidC = 1;
  printf("Invalid Command");
}
else if((!strcmp(command.Otype, ">") || !strcmp(command.Otype, ">>")) && !strcmp(command.Ofile, "0") ){
  invalidC = 1;
  printf("Invalid Command");
}
else if(!strcmp(command.cName, "cd") && !aCount){
printf("Invalid Command");
invalidC =1;
}
else if((bgCount != csCount) && command.bgP){
  invalidC =1;
  printf("Invalid Command");
}
//--------------------------------------commands
if(!invalidA && !invalidC ){
  // printf("acont  %d",aCount);

  if(!command.cType && (!strcmp(command.Otype, ">") || !strcmp(command.Otype, ">>") )){
  //printf("here1");

if(!strcmp(command.Otype, ">") ){
//printf(command.cName);
FILE *fp1;
fp1 = fopen(command.Ofile, "w");
if(fp1 == NULL)
   {
      printf("Error truncating file");   
      invalidC =1;             
   }

else if(!strcmp(command.cName, "dir") ){
  //printf("here 3" );
  char out[2000];
  strcpy(out, runDir(command.cArguments[0], dirCount));
 fprintf(fp1,"%s",out);
   fclose(fp1);
}
else if(!strcmp(command.cName, "environ") ){

  char out[2000];
  strcpy(out, runEnviron());
  fprintf(fp1,"%s",out);
  fclose(fp1);

}
else if(!strcmp(command.cName, "help") ){
 // printf("here");
  char out[1000000];
  strcpy(out, runHelp());
 fprintf(fp1,"%s",out);
   fclose(fp1);

}
else if(!strcmp(command.cName, "echo") ){
  char out[2000];
  strcpy(out,runEcho(command.cArguments,aCount) );
 fprintf(fp1,"%s",out);
   fclose(fp1);

}

   

}
else{

FILE *fp1;
fp1 = fopen(command.Ofile, "a");
if(fp1 == NULL)
   {
      printf("Error Appending file");   
      invalidC =1;             
   }

else if(!strcmp(command.cName, "dir") ){
  //printf("here 3" );
  char out[2000];
  strcpy(out, runDir(command.cArguments[0], dirCount));
 fprintf(fp1,"%s",out);
 fprintf(fp1,"%s"," ");

   fclose(fp1);
}
else if(!strcmp(command.cName, "environ") ){

  char out[2000];
  strcpy(out, runEnviron());
  fprintf(fp1,"%s",out);
  fprintf(fp1,"%s"," ");
  fclose(fp1);

}
else if(!strcmp(command.cName, "help") ){
  char out[10000];
  strcpy(out, runHelp());
 fprintf(fp1,"%s",out);
 fprintf(fp1,"%s"," ");
   fclose(fp1);

}
else if(!strcmp(command.cName, "echo") ){
  char out[2000];
  strcpy(out,runEcho(command.cArguments,aCount) );
 fprintf(fp1,"%s",out);
 fprintf(fp1,"%s"," ");
   fclose(fp1);

}


}


}

else if(!command.bgP && !command.pipeP && !command.cType &&(strcmp(command.Otype, ">") || (strcmp(command.Otype, ">>")) )){
 // printf("%s\n", command.Otype);

if(!strcmp(command.cName, "cd")){
 // char s[100];
  if(!chdir(command.cArguments[aCount-1])){
    //printf(getcwd(s,100));
  }
  else{
    printf("Directory %s does not exist",command.cArguments[aCount-1])
    ;invalidC = 1;
  }

}
if(!strcmp(command.cName, "dir")){
 // printf("%d %s", dirCount, command.cArguments[0]);
printf(runDir(command.cArguments[0], dirCount));
}
if(!strcmp(command.cName, "environ")){
printf(runEnviron());
}
if(!strcmp(command.cName, "help")){
printf(runHelp());
}
if(!strcmp(command.cName, "clr")){
printf("\033[H\033[2J");

}
if(!strcmp(command.cName, "pause")){
  runPause();

}
if(!strcmp(command.cName, "echo")){
  //printf("here");
  printf(runEcho(command.cArguments,aCount));

}
if(!strcmp(command.cName, "quit")){
  //printf("here");
  return 0;

}
 
}

else if(command.cType && command.pipeP){ // Pipe execution
//printf("\nb/a %s | %s",command.bPipe,command.aPipe);

 int p1,p2;
  int pfds[2];
  pipe(pfds);
  p1 = fork();
  if(p1 == 0){
   
    close(1);     // close stdout 
    dup2( pfds[1], 1 );      // wire stdout into pipe's write end
     close( pfds[0] );     //close read end of pipe 
    close( pfds[1] ); 
     char* input[5] = {"ls",NULL};
     if(aCount){
command.cArguments[aCount] = NULL;
}
else {
  command.cArguments[aCount] = command.bPipe;
  command.cArguments[aCount+1] = NULL;
}
   printf("\n");
 execvp(command.bPipe,command.cArguments);

      //printf("Invalid command");
        printf("\n");
      invalidC =1;
    
  }
  else{
    //waitpid(-1, NULL, 0);
   p2 = fork();
  
  if(p2 == 0){
   
     close(0);     // close stdin
      dup2( pfds[0], 0 );     // wire stdin into pipe's read end 
      close( pfds[1] ); 
      close( pfds[0] ); 
       char* input[2] = {command.aPipe,NULL};
         printf("\n");
     execvp(command.aPipe, input);  
     //printf("Invalid command"); 
       printf("\n");
     invalidC = 1;

    
  }
  else{
  //parent
 
 
  close(pfds[0]);
  close(pfds[1]);
  //waitpid(-1, NULL, 0);
  wait(0);
  wait(0);
  
  close(pfds[0]);
  close(pfds[1]);
   //waitpid(-1, NULL, 0);
   if(invalidC){
  printf("Invalid Command ");}
  
  
  
    
  }



}}
else if(command.cType && command.bgP){


int * shared = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0); // sharable pointer between processes
*shared = 0;

for(int i = 0; i < bgCount; i++){

if(fork() ==0){
 
  char* vArgs[10];
 
 


  for(int a =0; a < command.argNum[i]+1; a++){
    if(command.argNum[i] == 0){
 
 vArgs[0] = command.bgCommands[i];
 vArgs[1] = NULL;

 
 }
  else{
   
   if(a == command.argNum[i]){
   vArgs[command.argNum[i]] = NULL;
   }
   else{
     //vArgs[a] = malloc(4);
        total = total + (a-total);
   vArgs[a] = command.cArguments[*shared];
   *shared = *shared +1;
   
     //printf("Result %s  %d",vArgs[0],*shared);
   //printf(vArgs[a]);

   }
   
    
  }
 
   //printf("\np %d  c %d", getppid(),getpid());
  //printf("\n");
 
  
 
 
 }
   printf("\n");
 execvp(vArgs[0], vArgs);
 printf("invalid Command ");
   printf("\n");
 invalidC =1;
}else{

}



}




}
else if(command.cType &&  !strcmp(command.Otype, ">")   ){


//printf("ready %d ",aCount);
int p1, p2;


command.cArguments[aCount] = NULL;
p1 = fork();
if(p1 == 0){ //child

int fd = open(command.Ofile, O_CREAT|O_WRONLY, S_IRWXU);
 dup2(fd, 1);// direct std out

 if(aCount == 0){
  command.cArguments[0] = command.cName;
  command.cArguments[1] = NULL;
}
close(fd);
  printf("\n");
execvp(command.cName, command.cArguments);
  printf("\n");
printf("Invalid command ");
invalidC = 1;

}
else{

}

}
else if(command.cType && !strcmp(command.Otype, ">>")  ){


//printf("ready %d ",aCount);
int p1, p2;


command.cArguments[aCount] = NULL;
p1 = fork();
if(p1 == 0){ //child

//int fd = open("testfile.txt", O_WRONLY | O_APPEND);


int fd = open(command.Ofile, O_WRONLY | O_APPEND);
 dup2(fd, 1); //Direct std out

 if(aCount == 0){
  command.cArguments[0] = command.cName;
  command.cArguments[1] = NULL;
}
close(fd);
  printf("\n");
execvp(command.cName, command.cArguments);
printf("Invalid command ");
  printf("\n");
invalidC = 1;

}
else{

}
//printf("parent");
//parent
}
else if(command.cType && strcmp(command.Ifile, "0")  ){
//printf("ready %d ",aCount);
int p1, p2;


command.cArguments[aCount] = NULL;
p1 = fork();
if(p1 == 0){ //child

 int fd = open(command.Ifile, O_RDONLY);
 dup2(fd, 0);// direct stdin

 if(aCount == 0){
  command.cArguments[0] = command.cName;
  command.cArguments[1] = NULL;
}
close(fd);
  printf("\n");
execvp(command.cName, command.cArguments);
  printf("\n");
printf("Invalid command");
invalidC = 1;
//printf("parent");
//parent
}
else{

}
}



else if(command.cType && !command.pipeP && !command.bgP &&(strcmp(command.Otype, ">") || strcmp(command.Otype, ">>") || strcmp(command.Ifile, "0"))){
  
  //printf(command.Ifile);
int execR = 1; // execvp return

 int pid = fork() ;

if (pid < 0){ // error!
 printf("fork did not execute");
 invalidC = 1;
 } else if (pid  == 0 ){ // child! 

//char* input[3] = {"-la", NULL}; 

if(aCount){
command.cArguments[aCount] = NULL;
}
else {
  command.cArguments[aCount] = command.cName;
  command.cArguments[aCount+1] = NULL;
}
   printf("\n");
   //invalidC = 1;

 execvp(command.cName,command.cArguments);
  
printf("Invalid Command");

 printf("\n");
//close(fptr);
invalidC = 1;
execR = 0;


 } else { // parent! 
 //wait(0);
waitpid(pid, NULL, 0);
//printf("parent\n");
//invalidC =0;

 }


}


}//valid check while

if(argc ==1){
printf("\nmyShell> ");
}
else{
    
  //printf("id %d   cn  %s \n",getpid(),command.cName);
 
}
} //command while loop

 

 // printf("haha %s",token);
 



  //return 0;
}





//end of main



char* runDir(char dir[], int argNum){
    char output[2000];
  strcpy(output, "");
   int count =0; // get rid of wierd dots firstly when reading
  

  if(!argNum){

  
   struct dirent *dEntery;  // Pointer for directory entry 
  
    // opendir() returns a pointer of DIR type.  
    DIR *dr = opendir("."); 
  
   
    while ((dEntery = readdir(dr)) != NULL) {
          count++;
      if(count >= 3){
      strcat(output, dEntery->d_name);
      strcat(output, "  ");
      }

           // printf("%s\n", dEntery->d_name); 
    }
    closedir(dr);    
   
  }
  
  else{
   

struct dirent *dEntery;  // Pointer for directory entry 
  
    // opendir() returns a pointer of DIR type.  
    DIR *dr = opendir(dir); 
  
    if (dr == NULL)  
    { 
       printf("Directory %s does not exist",dir);
       
       
    } 
   
  else{
    while ((dEntery = readdir(dr)) != NULL){
      count++;
      if(count >= 3){
      strcat(output, dEntery->d_name);
      strcat(output, "  ");
      }

       //printf("%s\n", dEntery->d_name); 
    } 
    //printf("  ahaha  %d  ",count);
    }
           
  
    closedir(dr);     


  }
  
 return output;
}
char* runEnviron(){
 char output[1000];
  strcpy(output, "");
    strcat(output, "PATH: ");
  strcat(output, getenv("PATH"));
  strcat(output, "\n");
  strcat(output, "HOME: ");
  strcat(output, getenv("HOME"));
    strcat(output, "\n");
  strcat(output, "LANG: ");
  strcat(output, getenv("LANG"));
    strcat(output, "\n");
  strcat(output, "DISPLAY: ");
  strcat(output, getenv("DISPLAY"));

  return output;

}
char* runHelp(){
   char output[10000];
  strcpy(output, "");
  
  //OPen file
    FILE *fp;
    char str[1000];
    char* filename = "README.txt";
 
    fp = fopen(filename, "r");
    
    while (fgets(str, 1000, fp) != NULL)
      strcat(output, str);
        //strcat(output, "");
       // printf("%s", str);
    fclose(fp);
    return output;

}
void runPause(){
 char *line = NULL; // command input
size_t len = 0;
ssize_t read = 0;


 //Get Command line
 printf("\n");
while((read = getline(&line, &len, stdin) != -1) && strcmp(line, "\n")){

}


}
char* runEcho(char *args[], int argum){
   char output[1000];
  strcpy(output, "");

  for(int i =0; i < argum; i++){
    strcat(output, args[i]);
     strcat(output, " ");
  }
return output;
}
