#include "pipe_networking.h"

char* toUpper(char input[]) {
  char* start = input;
  int i;
  for (i = 0; i < strlen(input); i++) {
    if (input[i] >= 'a' && input[i] <= 'z')
    input[i] -= 32;
  }
  return start;
}

/*=========================
  server_handshake
  args: int * to_client
  Performs the server side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.
  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
  int from_client = 0;
  mkfifo(WKP, 0644); // server creates a WKP and waits for connection
  printf("WKP made, waiting for connection\n");


  while (1) {
    
    from_client = open(WKP, O_RDONLY);
    char clientpid[HANDSHAKE_BUFFER_SIZE];
    int received = read(from_client, clientpid, HANDSHAKE_BUFFER_SIZE); // server receives client's msg (it's pid) and removes WKP
    if (received == -1) {
      printf("Error: %s",strerror(errno));
      return 0;
    }
    printf("Client message received. Pid: %s. Removing WKP\n", clientpid);
    remove(WKP);


    printf("Connecting to client FIFO and sending initial acknowledgement message\n");
    *to_client = open(clientpid, O_WRONLY); //Server connects to client FIFO
    int check = write(*to_client, "to client", HANDSHAKE_BUFFER_SIZE); //sends an initial acknowledgment message
    if (check <= 0) {
      printf("Error: %s",strerror(errno));
      return 0;
    }


    read(from_client, clientpid, HANDSHAKE_BUFFER_SIZE);
    printf("Received client message: %s\n", clientpid);


    printf("Handshake success!\n");


    while (1) {
      read(from_client, clientpid, HANDSHAKE_BUFFER_SIZE);
      toUpper(clientpid);
      write(*to_client, clientpid, HANDSHAKE_BUFFER_SIZE);
    }
  }

  return from_client;
}


/*=========================
  client_handshake
  args: int * to_server
  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.
  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  int from_server = 0;
  char mypid[HANDSHAKE_BUFFER_SIZE];
  printf("Creating private FIFO\n");
  sprintf(mypid, "%d", getpid() );
  printf("My pid: %s\n", mypid);
  mkfifo(mypid, 0644); //client creates a private FIFO


  printf("Connecting to server and sending private FIFO name. Waiting for response\n");
  *to_server = open(WKP, O_WRONLY);
  if (!*to_server) {
      printf("Error: %s",strerror(errno));
      return 0;
  }
  int check = write(*to_server, mypid, sizeof(mypid));
  if (check <= 0) {
      printf("Error: %s",strerror(errno));
      return 0;
  }


  printf("Receiving server message. Removing private FIFO\n");
  from_server = open(mypid, O_RDONLY);
  char servermsg[HANDSHAKE_BUFFER_SIZE];
  check = read(from_server, servermsg, HANDSHAKE_BUFFER_SIZE);
  if (check <= 0) {
      printf("Error: %s",strerror(errno));
      return 0;
  }
  printf("Server message: %s\n", servermsg);
  remove(mypid);


  printf("Sending response to server\n");
  write(*to_server, "to client", HANDSHAKE_BUFFER_SIZE);

  
  printf("Handshake success!\n");


  char line[HANDSHAKE_BUFFER_SIZE];

  while (1) {
    printf("Input: ");
    fgets(line, HANDSHAKE_BUFFER_SIZE, stdin);
    write(*to_server, line, HANDSHAKE_BUFFER_SIZE);
    read(from_server, servermsg, HANDSHAKE_BUFFER_SIZE);
    printf("Processed message: %s\n", servermsg);
  }


  return from_server;
}
