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

int main() {

  int to_client;
  int from_client;

  while (1) {
    from_client = server_handshake( &to_client );

    
    char clientmsg[BUFFER_SIZE];

    while (read(from_client, clientmsg, BUFFER_SIZE)) {
      printf("Processing\n");
      toUpper(clientmsg);
      write(to_client, clientmsg, BUFFER_SIZE);
    }
  }
}