Read me for my shell program


Full command lines are either taken from the console or a batch file.
A command struct or object is made to store command attributes 


my shell implements 8 built-in commands namely 
cd        changes the current directory
clr    Clears the screen 

dir    Prints the content of a current directory or specify directory 

environ   Print a selected few of environment variables, which are : path ,home ,display ,user 

echo       prints the user’s input 

help        display this readme file 

pause      pauses the system till the enter key is pressed

Any other commands are treated as external commands. To run these external commands ,a child process is created using the fork() Command .
Regardless of what type of command there are certain specific locations to what the command line could do such as ; I/O re direction ,pipe in and background commands .
redirect standard input to either the keyboard or file depending on if batchfile is present to get commands.

However, only output redirection Anne plain commands are implemented for are set of internal or built-in commands because pipe in and background implementation make little or no sense.Same with internal commands and input redirection
For internal output redirection and plain internal commands, there are methods to deal with such cases. Output for output returnable commands are stored in a string variable. The string variable is print it for plane commands or written to a file for output redirection.

EXTERNAL CASES
Plain commands
new child process is created using the fork () command which is then replaced with the exec() command, while the parent is waiting .

Piping
All processes in the pipe or Forked, And pipe () call Is used to redirect input or output From or to the pipe respectively .The parent waits for the child processes in this case 

Background
Child processes are created using the fork() command. However the parent process or our ongoing shell does not wait for the child processes .

Input | output redirection 
The dup2() call here is used to take standard input from my file or push standard output to a file.






