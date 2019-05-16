//client code
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdio.h> 
// #include <iostream.h>

#define IP_PROTOCOL 0 
#define IP_ADDRESS "127.0.0.1" // localhost 
#define PORT_NO 15050 
#define NET_BUF_SIZE 32
#define cipherKey 'S' 
#define sendrecvflag 0


FILE *fptr;

//this funtion clears the buffer 
void clearBuf(char* b) { 
	int i; 
	for (i = 0; i < NET_BUF_SIZE; i++) 
		b[i] = '\0'; 
} 

//deciphering function
char Cipher(char ch){ 
	return ch ^ cipherKey; 
} 

// function to receive file 
int receive_File(char* buffer, int s, int print_or_put) { 
	//print_or_put 1 -> Print, 2 -> Put
	char ch; 
	for (int i = 0; i < s; i++) { 
		ch = buffer[i]; 
		ch = Cipher(ch); 
		if (ch == EOF) 
			return 1;//entire file received
		else
		{
			if(print_or_put == 1)
				printf("%c", ch);//if listall
			else if(print_or_put == 2)
				putc(ch, fptr);//if send
		}
	} 
	return 0; 
} 

//driver code 
int main() 
{
	//initializations
	int sockfd, nBytes; 
	// char *hello = "Hello from client";
    // char buffer[1024] = {0};	
	struct sockaddr_in addr_con; 
	int addrlen = sizeof(addr_con); 
	addr_con.sin_family = AF_INET; 
	addr_con.sin_port = htons(PORT_NO); 
	addr_con.sin_addr.s_addr = inet_addr(IP_ADDRESS); 
	char command_buffer[NET_BUF_SIZE]; 
	FILE* fp; 

	//creating the socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 
					IP_PROTOCOL); 

	if(sockfd < 0)
		printf("\nfile descriptor not received!!\n"); 
	else
		printf("\nfile descriptor %d received\n", sockfd); 	//socket creation being verified

	while (1)
	{
		clearBuf(command_buffer);//buffer cleared
		scanf("%[^\n]%*c", command_buffer); 
		//command received

		if(strcmp("listall", command_buffer) == 0)
		{
			//implementation of listall
			sendto(sockfd, command_buffer, NET_BUF_SIZE, 
			sendrecvflag, (struct sockaddr*)&addr_con, 
			addrlen);
			
			while (1)
			{ 
				//receive data
				clearBuf(command_buffer); 
				nBytes = recvfrom(sockfd, command_buffer, NET_BUF_SIZE, 
								sendrecvflag, (struct sockaddr*)&addr_con, 
								&addrlen); 

				//take action based on command type
				if (receive_File(command_buffer, NET_BUF_SIZE, 1)) { 
					break; 
				} 
			}
			continue;
		}

		//checks if invalid command

		char file_name[100];
		int i;
		for(i = 0; i < 100; ++i)
			file_name[i] = '\0';

		//preprocessing
		if(command_buffer[0] == 's' && command_buffer[1] == 'e' && command_buffer[2] == 'n' && command_buffer[3] == 'd' && command_buffer[4] == ' ')
		{
			for(i = 5; command_buffer[i] != '\0'; ++i)
				file_name[i - 5] = command_buffer[i];

			for(int j = i - 5; j < 100; ++j)
				file_name[j] = '\0';

			for(int j = 0; j < 100; ++j)
				command_buffer[j] = file_name[j];
		}

		else{
			printf("Invalid Command\n");
			continue;
		}
		strcpy(file_name, command_buffer);
		
		//all parameters initialized


		//send request for file
		sendto(sockfd, command_buffer, NET_BUF_SIZE, 
			sendrecvflag, (struct sockaddr*)&addr_con, 
			addrlen);

		//pointer for file to be written onto
		fptr = fopen(command_buffer, "ab+");

		while (1)
		{ 
			//receive data
			clearBuf(command_buffer); 
			nBytes = recvfrom(sockfd, command_buffer, NET_BUF_SIZE, 
							sendrecvflag, (struct sockaddr*)&addr_con, 
							&addrlen); 

			//process according to command
			if (receive_File(command_buffer, NET_BUF_SIZE, 2)) { 
				break; 
			} 
		}

		fclose(fptr);

		//Check if File Not Found
		FILE *fileStream = fopen(file_name, "r");
		char fileText[100];
		fgets(fileText, 100, fileStream);
		if(strcmp(fileText, "File Not Found!") == 0){
			printf("File Does Not Exist!\n");
			remove(file_name);
		}
		else
			printf("File Successfully Downloaded \n\n");
		fclose(fileStream);
	}
	return 0; 
} 
