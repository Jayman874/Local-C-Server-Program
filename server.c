/**
 * Skeleton file for server.c
 * 
 * You are free to modify this file to implement the server specifications
 * as detailed in Assignment 3 handout.
 * 
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ctype.h>
// Necessary header files

/**
 * Function whic appends all values of the inputed text file to
 * a char array which can then by displayed to the client
 * 
 * char* msg: The array to append too
 * char c: value to be appended
 * 
 */
void append(char* msg, char c) {
        int length = strlen(msg);
        msg[length] = c;
        msg[length+1] = '\0';
}

/**
 * The main function should be able to accept a command-line argument
 * argv[0]: program name
 * argv[1]: port number
 * 
 * Read the assignment handout for more details about the server program
 * design specifications.
 */ 

int main(int argc, char *argv[]){
    int loopBack = 1;
    int fd = socket(AF_INET, SOCK_STREAM, 0); // creates the socket with the system call
        if (fd == -1){ // checks to see whether socket can be created
        printf("Cannot create socket");
        exit(0);
    }
    struct sockaddr_in addr; 
    addr.sin_family = AF_INET; //sets domain
    addr.sin_addr.s_addr = INADDR_ANY; // sets any address
    unsigned short port = atoi(argv[1]); // get port number from comannd line argument
    if (port < 1024 || argv[1] == NULL){
        printf("Port number is less than 1024 or no port number specified");
        return -1;
    }
    addr.sin_port = htons(port); // gets user inputed port number 
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0){ // binds the socket to the address
        printf("Could not bind socket");
        exit(0);
    }
    while (loopBack == 1){
        if (listen(fd, SOMAXCONN) < 0){ //listens for a conncetion 
            printf("Could not listen to connections");
            exit(0);
        }
        struct sockaddr_in client_addr;
        int addrlen = sizeof(client_addr); // gets size of client address
        int client_fd = accept(fd, (struct sockaddr *)&client_addr, (socklen_t*)&addrlen); // gets client file discripter
        if(client_fd < 0){ // checks that connection can be accepted 
        printf("Cannot accept connection");    
        exit(0);
        }
        char msg[] = "HELLO\n";
        ssize_t s = send(client_fd, msg, strlen(msg), 0);
        if (s < 0){ // checks that message can be sent to client 
            printf("Could not send message");
            exit(0);
        }
        char incoming[100];
        while (loopBack == 1){
            memset(incoming, 0, 100);
            ssize_t r = recv(client_fd, incoming, 100, 0);
            if (r <= 0){ // checks you can recieve the message 
                printf("Could not receive message");
                close(client_fd);
                exit(0);
            }
            connect(client_fd, (struct sockaddr *)&client_addr, addrlen); // connect client to server 
            printf("Received message: %s", incoming);   
            char const *exit_p = "BYE";
            char const *get = "GET";
            char const *put = "PUT";
            for (int i = 0; i < strlen(incoming); i++){ // get all value to upper case so it is case insensitive
                incoming[i] = toupper(incoming[i]);
            }
            if (strstr(incoming, exit_p) != NULL){ // if the command is BYE close the connection
                printf("Client connection closed\n");
                *incoming = '\0'; // clear incoming
                close(client_fd);
                break; // break out of first while loop and go back to listening for connections
            } else if (strstr(incoming, get) != NULL){ // if command is GET 
                FILE *in;
                char str;
                for (int i = 0; i < strlen(incoming); i++){ // set value to lower case 
                    incoming[i] = tolower(incoming[i]);
                }
                char file[20];
                char command[20];
                sscanf(incoming, "%s %s", command, file); // get the command and file inputed 
                in = fopen(file, "r"); // open the file for reading
                if (file[0] == 0){  // if not file is given print error message
                    char msg1[] = "Server 505 Get Error\n";
                    ssize_t s2 = send(client_fd, msg1, strlen(msg1), 0);
                    if (s2 < 0){ // checks that message can be sent to client 
                        printf("Could not send message");
                        exit(0);
                    }
                }
                else if (in == NULL){ // if file cannot be opened print error message 
                    char msg2[] = "Server 404 Not Found\n";
                    ssize_t s3 = send(client_fd, msg2, strlen(msg2), 0);
                    if (s3 < 0){ // checks that message can be sent to client 
                        printf("Could not send message");
                        exit(0);
                    }
                }
                char msg3[] = "Server 200 OK\n";
                char msg4[] = "\n";
                char msg5[100];
                str = fgetc(in); 
                while (str != EOF){ // while it has not reached the end of the file 
                    append(msg5, str); // append each value to a char array
                    str = fgetc(in);
                }
                char msg6[] = "\n";
                char msg7[] = "\n";
                ssize_t s4 = send(client_fd, msg3, strlen(msg3), 0); // send contents of file to the client 
                ssize_t s5 = send(client_fd, msg4, strlen(msg4), 0);
                ssize_t s6 = send(client_fd, msg5, strlen(msg5), 0);
                ssize_t s7 = send(client_fd, msg6, strlen(msg6), 0);
                ssize_t s8 = send(client_fd, msg7, strlen(msg7), 0);
                if (s6 < 0){ // checks that message can be sent to client 
                    printf("Could not send message");
                    exit(0);
                }
                *msg5 = '\0'; // clear all arrays 
                *file = '\0';
                *command = '\0';
                *incoming = '\0';
                fclose(in); // close the file 
            }
        }
    }
return 0;
}

