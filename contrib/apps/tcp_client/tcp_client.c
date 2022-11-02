#include "tcp_client.h"
 
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/api.h"
#include <lwip/sockets.h>
#include <time.h>
 
#define PORT            6001
#define IP_ADDR        "124.70.50.239"
 
#define IPERF_BUFSZ         (4 * 1024)
#define COUNT               100
 
static void tcp_client(void *thread_param)
{
  int sock = -1,i;
  struct sockaddr_in client_addr;
  uint8_t* send_buf;
  clock_t begin, end;
  clock_t start, stop;
  double cost = 0;
  uint64_t sentlen = 0;
  int count = COUNT;
  
  send_buf = (uint8_t *) malloc(IPERF_BUFSZ);
  if (!send_buf) 
    return ;
  
  for(i = 0; i < IPERF_BUFSZ; i ++)
    send_buf[i] = i & 0xff;
  
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
      printf("Socket error\n");
      free(send_buf);
      return;
  }

  client_addr.sin_family = AF_INET;
  client_addr.sin_port = htons(PORT);
  client_addr.sin_addr.s_addr = inet_addr(IP_ADDR);
  memset(&(client_addr.sin_zero), 0, sizeof(client_addr.sin_zero));

  if (connect(sock, (struct sockaddr*)&client_addr, sizeof(struct sockaddr)) == -1)
  {
      printf("Connect failed!\n");
      closesocket(sock);
      free(send_buf);
      return;
  }

  printf("Connect to tcp server successful!\n");
 
  start = clock();
  while(count--)
  {
      begin = clock();
      write(sock, send_buf, IPERF_BUFSZ);
      end = clock();
      cost = (double)(end - begin) / CLOCKS_PER_SEC;
      printf("Real-time speed:%.2lf KB/s\n", IPERF_BUFSZ / 1024 / cost);
  }
  stop = clock();
  cost = (double)(stop - start) / CLOCKS_PER_SEC;
  printf("===Average Speed:%.2lf KB/s===\n", COUNT * IPERF_BUFSZ / 1024 / cost);
  closesocket(sock);
  free(send_buf);
}
 
void tcp_client_init(void)
{
  sys_thread_new("tcp_client", tcp_client, NULL, 2048, 8);
}