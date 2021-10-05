#include <assert.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/wait.h>

static inline
bool file_exists(char const *filename) {
  struct stat buffer;
  return (stat(filename, &buffer) == 0);
}

static inline
size_t file_size(char const* filename)
{
  struct stat st;
  const int rc = stat(filename, &st);
  assert(rc != -1);
  return st.st_size;
}


int main(void)
{
  char const * addr = "127.0.0.1";
  int const port = 8080;

  int const sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
  assert(sockfd > -1 && "Error creating the socket");

  struct sockaddr_in cli_addr = {  .sin_family = AF_INET,
                                   .sin_port = htons(port)}; 

  int rc = inet_aton(addr, &cli_addr.sin_addr);
  assert(rc == 1 && "Invalid string for the address IPv4");

  printf("Enter file name: \n");
  char buffer[64] = {0};
  fgets(buffer,64,stdin);
  buffer[strlen(buffer)-1] = '\0';
 
  assert(file_exists(buffer) == true && "File not found!");

  rc = sendto(sockfd,buffer, strlen(buffer) , 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
  assert(rc == (int)strlen(buffer));

////

  FILE* fptr=fopen(buffer,"r");
  assert(fptr != NULL && "Unable to open file");

  int const size = file_size(buffer);
  printf("Size of the file = %d \n", size);

  socklen_t len = sizeof(cli_addr); 
  rc = sendto(sockfd, &size, sizeof(int), 0, (struct sockaddr *)&cli_addr, len);
  assert(rc == sizeof(int));

  char* data = calloc(1, size);
  assert(data != NULL && "Memory exhausted");

  rc = fread(data,size,1,fptr);
  assert(rc == 1);

  rc = sendto(sockfd, data, size, 0, (struct sockaddr *)&cli_addr, len);
  assert(rc == size);

  fclose(fptr);
  free(data);
  close(sockfd);

  return 0;
}

