#include "pipe_networking.h"


int main() {

  int to_server;
  int from_server;

  from_server = client_handshake( &to_server );

  char line[HANDSHAKE_BUFFER_SIZE];

  while (1) {
    printf("Input: ");
    fgets(line, HANDSHAKE_BUFFER_SIZE, stdin);
    write(to_server, line, HANDSHAKE_BUFFER_SIZE);
    read(from_server, line, HANDSHAKE_BUFFER_SIZE);
    printf("Processed response: %s\n", line);
  }
}