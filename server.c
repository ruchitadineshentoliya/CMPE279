// Server side C/C++ program to demonstrate Socket programming

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/wait.h>
#include <pwd.h>
#define PORT 80

char *hello = "Hello from server";
void send_message(const char* new_socket_str);

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    
    pid_t currentPid;
    int value;
    struct passwd* id;
    char *name = "nobody";

    if(argc == 1) {

    	// Creating socket file descriptor
    	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    	{
        		perror("socket failed");
        		exit(EXIT_FAILURE);
    	}

    	// Attaching socket to port 80
    	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    	{
        		perror("setsockopt");
        		exit(EXIT_FAILURE);
    	}
    	address.sin_family = AF_INET;
    	address.sin_addr.s_addr = INADDR_ANY;
    	address.sin_port = htons( PORT );

    	// Forcefully attaching socket to the port 80
    	if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    	{
        		perror("bind failed");
        		exit(EXIT_FAILURE);
    	}
    	if (listen(server_fd, 3) < 0)
    	{
        		perror("listen");
        		exit(EXIT_FAILURE);
    	}
    	if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    	{
        		perror("accept");
        		exit(EXIT_FAILURE);
    	}
    
    	//Forking a child process
    	printf("Starting to fork!!!\n");
    	currentPid = fork(); //fork will return pid of child to parent and status(0 - successful/-1 otherwise)

    	if(currentPid == 0) {

       		//Assignment 2 starts here
       		printf("Child process is running with id : %d \n", getuid());
       		printf("Exec the server child\n");
       		char socketIdStr[20];
       		sprintf(socketIdStr, "%d", new_socket);
		char *args[] = {"./server", socketIdStr, NULL};
                 if(execvp(args[0], args) < 0) {
			perror("Exec failed");
			exit(EXIT_FAILURE);
		}
	} else if(currentPid > 0) {
       		wait(NULL);
       		printf("Parent is running\n");
    	} else {
       		printf("Fork failed\n");
       		_exit(2);

    	}
     }
     else 
     {

       id = getpwnam("nobody"); //get id of nobody user      

       if(id == NULL) {
          printf("Cannot find uid for nobody user\n");
          return 0;
       }

       value = setuid(id -> pw_uid);
       printf("Return value after setUid: %d\n", value);
       printf("Child id after privilege drop is: %d\n", getuid());

       if(value == 0) {
         
	//Privilege drop successful
   	printf("Child privilege dropped\n");
	send_message(argv[1]);
       
       } else {
         printf("Error dropping privileges\n");
         return 0;
       }
    } 
    return 0;

}

void send_message(const char* new_socket_str) {

	int new_socket, valread;
	char buffer[1024] = {0};
	new_socket = atoi(new_socket_str);
	valread = read( new_socket , buffer, 1024);
	if(valread < 0) {
		perror("read failed\n");
		exit(EXIT_FAILURE);
	}
       	printf("%s\n",buffer );
       	send(new_socket , hello , strlen(hello) , 0 );
       	printf("Hello message sent\n");
}
