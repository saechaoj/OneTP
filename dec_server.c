#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>




int cExchange (char c){
	if (c == ' '){
		return 26;
	}
	else {
		return (c - 'A');
	}
	return 0;
}



char iExchange(int i){
	if (i == 26){
		return ' ';
	}
	else {
		return (i + 'A');
	}
}

//decriptes message using the key
void denc(char* text, char* key)
{

  char x;
  int i;
  for(i = 0; text[i] != '\n'; i++)
  {

    char y = cExchange(text[i]) - cExchange(key[i]);

	  		text[i] = iExchange(y);

  }

  text[i] = '\0';

}


// Error function used for reporting issues
void error(const char *msg) {
  perror(msg);
  exit(1);
}

// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address,
                        int portNumber){

  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address));

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);
  // Allow a client at any address to connect to this server
  address->sin_addr.s_addr = INADDR_ANY;
}

int main(int argc, char *argv[]){
  int connectionSocket, charsRead, status;
  // char buffer[256];
  // char* newMessage[256];
  // char key[256];
  struct sockaddr_in serverAddress, clientAddress;
  socklen_t sizeOfClientInfo = sizeof(clientAddress);
  pid_t pid;

  // Check usage & args
  if (argc < 2) {
    fprintf(stderr,"USAGE: %s port\n", argv[0]);
    exit(1);
  }

  // Create the socket that will listen for connections
  int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listenSocket < 0) {
    error("ERROR opening socket");
  }

  // Set up the address struct for the server socket
  setupAddressStruct(&serverAddress, atoi(argv[1]));

  // Associate the socket to the port
  if (bind(listenSocket,
          (struct sockaddr *)&serverAddress,
          sizeof(serverAddress)) < 0){
    error("ERROR on binding");
  }

  // Start listening for connetions. Allow up to 5 connections to queue up
  listen(listenSocket, 5);

  // Accept a connection, blocking if one is not available until one connects
  while(1){
    // Accept the connection request which creates a connection socket
    connectionSocket = accept(listenSocket,
                (struct sockaddr *)&clientAddress,
                &sizeOfClientInfo);
    if (connectionSocket < 0){
      error("ERROR on accept");
    }

    printf("SERVER: Connected to client running at host %d port %d\n",
                          ntohs(clientAddress.sin_addr.s_addr),
                          ntohs(clientAddress.sin_port));






    //new process that sends and recieves from client side
    pid = fork();
		switch (pid)
    {
		  case -1:
      {
				error("Hull Breach! Couldn't fork!\n");
			}


			case 0:
      {
        char buffer[512];
        char message[10000];
        char key[10000];
        int c = 0;

        memset(buffer, '\0', 512);

        charsRead = recv(connectionSocket, buffer, 511, 0);
        if (charsRead < 0){
          error("ERROR reading from socket");
        }
        printf("SERVER: I received this from the client: \"%s\"\n", buffer);
        strcat(message,buffer);

        //key set
        memset(buffer, '\0', 512);
        charsRead = recv(connectionSocket, buffer, 511, 0);
         if (charsRead < 0){
                  error("ERROR reading from socket");
                }
        printf("SERVER: I received this from the client: \"%s\"\n", buffer);
        strcat(key,buffer);

        denc(message,key);


        memset(buffer, '\0', 512);



        // Send a Success message back to the client
        charsRead = send(connectionSocket,
                        message, sizeof(message), 0);
        if (charsRead < 0){
          error("ERROR writing to socket");
      }

      exit(0);
    }

      default:
      {

        pid_t actualpid = waitpid(pid, &status, WNOHANG);

      }

    }

    // Close the connection socket for this client
    close(connectionSocket);
  }
  // Close the listening socket
  close(listenSocket);
  return 0;
}
