#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()
#include <time.h>



void error(const char *msg) {
  perror(msg);
  exit(0);
}

// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address,
                        int portNumber,
                        char* hostname){

        // Clear out the address struct
        memset((char*) address, '\0', sizeof(*address));

        // The address should be network capable
        address->sin_family = AF_INET;
        // Store the port number
        address->sin_port = htons(portNumber);

        // Get the DNS entry for this host name
        struct hostent* hostInfo = gethostbyname(hostname);
        if (hostInfo == NULL)
          {
            fprintf(stderr, "CLIENT: ERROR, no such host\n");
            exit(0);
          }


        // Copy the first IP address from the DNS entry to sin_addr.s_addr
        memcpy((char*) &address->sin_addr.s_addr,
              hostInfo->h_addr_list[0],
              hostInfo->h_length);
}



int main(int argc, char *argv[]) {

  int socketFD, portNumber, charsWritten, charsRead;
  struct sockaddr_in serverAddress;
  char buffer[512];


  // Check usage & args
  if (argc < 4) {
    fprintf(stderr,"Bad arguementfrom client\n");
    exit(0);
  }




  //reads in file for plaintext
   FILE *file = fopen(argv[1],"r");
   if(file == NULL)
   {
     fprintf(stderr,"Plain Text cant open\n");
     exit(1);
   };

  char plainText[999999];
  char message_char;
  int i = 0;
  int pcounter = 0;
  while(1)
  {
    message_char = fgetc(file);
    if(message_char == EOF)
    {
      plainText[i] = '\0';
      break;
    }
    else
    {
      plainText[i] = message_char;
      i++;
      pcounter++;
    }

  }

  fclose(file);







  //reads in file for key
   FILE *file2 = fopen(argv[2],"r");
   if(file2 == NULL)
   {
     fprintf(stderr,"Key cant be read\n");
     exit(1);
   };

  char key[999999];
  char key_char;
  int keyCounter = 0;
  i = 0;
  while(1)
  {
    key_char = fgetc(file2);
    if(key_char == EOF)
    {
      key[i] = '\n';
      break;
    }
    else
    {
      key[i] = key_char;
      i++;
      keyCounter++;
    }

  }

  fclose(file2);




//check to see if key and message match
  if(keyCounter < pcounter)
  {
     fprintf(stderr,"Bad key\n");
     exit(1);
  }

  // Create a socket
  socketFD = socket(AF_INET, SOCK_STREAM, 0);
  if (socketFD < 0){
    error("CLIENT: ERROR opening socket");
  }

   // Set up the server address struct
  setupAddressStruct(&serverAddress, atoi(argv[3]),"localhost");

  // Connect to server
  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
    error("CLIENT: ERROR connecting");
  }


  // // Get input message from user
  // printf("CLIENT: Enter text to send to the server, and then hit enter: ");
  // // Clear out the buffer array
  // memset(buffer, '\0', sizeof(buffer));
  // // Get input from the user, trunc to buffer - 1 chars, leaving \0
  // fgets(buffer, sizeof(buffer) - 1, stdin);
  // // Remove the trailing \n that fgets adds
  // buffer[strcspn(buffer, "\n")] = '\0';




  // Send message to server
  // Write to the server
  //Clears out buffer to be used again
  charsWritten = send(socketFD, plainText, strlen(plainText), 0);
  if (charsWritten < 0){
    error("CLIENT: ERROR writing to socket");
  }
  if (charsWritten < strlen(plainText)){
    printf("CLIENT: WARNING: Not all data written to socket!\n");
  }
  memset(plainText, '\0', sizeof(plainText));



  charsWritten = send(socketFD, key, strlen(key), 0);
  if (charsWritten < 0){
    error("CLIENT: ERROR writing KEY to socket");
  }
  if (charsWritten < strlen(plainText)){
    printf("CLIENT: WARNING: Not all KEY data written to socket!\n");
  }
  memset(key, '\0', sizeof(key));



  // Read data from the socket, leaving \0 at end
  charsRead = recv(socketFD, plainText, sizeof(plainText) - 1, 0);
  if (charsRead < 0){
    error("CLIENT: ERROR reading from socket");
  }
  printf("CLIENT: I received this from the server: \"%s\"\n", plainText);

  close(socketFD);
  return 0;
}
