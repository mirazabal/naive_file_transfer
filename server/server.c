#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <sys/stat.h>

int main(void)
{
  const int port = 8080;
  char buf[64] = {0};

  int const sockfd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
  assert(sockfd != -1 && "Error creating socket");

  struct sockaddr_in serv_addr = {.sin_family = AF_INET,
    .sin_port = htons(port),
    .sin_addr.s_addr = INADDR_ANY};

  int rc = bind(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
  assert(rc != -1 && "Error while binding. Address already in use?");

  struct sockaddr_in cli_addr;
  socklen_t len = sizeof(cli_addr); 
  rc = recvfrom(sockfd, buf, 64, 0, (struct sockaddr *)&cli_addr,&len);
  assert(rc > -1 && rc < 64 && "Buffer overflow");

  printf("File name is : %s\n",buf);

//////////////////////

  int size = 0;
  rc = recvfrom(sockfd, &size, sizeof(int), 0, (struct sockaddr *)&cli_addr, &len);
  assert(rc == sizeof(int));

  printf("File size is : %d\n",size);

//////////////////////

  char* data = calloc(1, size);
  assert(data != NULL && "Memory exhausted!");

  rc = recvfrom(sockfd,data,size,0,(struct sockaddr *)&cli_addr, &len);
  assert(rc == size);

  FILE* fptr = fopen(buf, "wb");
  rc = fwrite(data,size,1,fptr);
  assert(rc == 1);
  printf("File received \n");

//////////////////////


  fclose(fptr);
  free(data);
  close(sockfd);

  /*
  int const mode = strtol("0755", 0, 8);
  rc = chmod(buf, mode);
  assert(rc > -1);
*/

  return 0;

}

