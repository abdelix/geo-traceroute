#include <stdio.h>
#include <stdlib.h>

#include <netdb.h> 
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "geoloc_ip.h"

#include "icmp_chksum.h"

#define STR_LEN 120
#define BUFF_LEN 500
#define DATA_LEN 44



char buff[BUFF_LEN];
struct sockaddr_in dst;
int sock_fd;

void echo_rqst(u_int16_t req)
{
  struct icmphdr *icmp_h;
  //Configuramos los parametros icmp

icmp_h=(struct icmphdr *)(buff);
 
icmp_h->type=ICMP_ECHO;
icmp_h->code=0;
icmp_h->checksum=0;

icmp_h->un.echo.id=31337;
icmp_h->un.echo.sequence=req;
//Calculamos el checksum
icmp_h->checksum=in_cksum((uint16_t*)buff,DATA_LEN);



bzero(dst.sin_zero, 8);
if(sendto(sock_fd,buff,DATA_LEN,0,(struct sockaddr*)&dst,sizeof(dst))<0)
{
  perror("sendto");
}
  
}

int main(int argc, char **argv) {
  
  char str[STR_LEN];
  char buf[BUFF_LEN];
  fd_set padre,tmp;
  
  
  struct hostent *host;
  struct icmphdr *rcv_h,*n_icmp_h;
  struct iphdr *ip_h;
  
  struct sockaddr_in from;
  socklen_t len;
  FD_ZERO(&padre);
  init_GeoLocIp();
  
  
 unsigned char i;
  int res;
  struct timeval espera;
  
  
  if(argc!=2)
  {
   
    fprintf(stderr,"ERROR:incorrect number of arguments \n");
   
  }
  /** Convertimos nombre a ip **/
  host=gethostbyname(argv[1]);
  if(!host)
  {
   perror("Error en gethostbyname ");
    exit(1);
  }
  
  dst.sin_family=AF_INET;
  dst.sin_port=htons(IPPROTO_ICMP);
  dst.sin_addr.s_addr=*((uint32_t *)host->h_addr_list[0]);

 ///Creamos un nuevo socket "crudo" y con protocolo ICMP
 sock_fd=socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);
 FD_SET(sock_fd,&padre);
 if(sock_fd<0)
 {
   perror("RAW socket creation failed  ");
   exit(1);
 }

///Ponemos a 0 el buffer

memset(buff,0,BUFF_LEN);





ip_h=(struct iphdr *)buf;

i=1;
rcv_h=(struct icmphdr *)(buf+20+8+20);
n_icmp_h=(struct icmphdr *)(buf+20);
ip_h= (struct iphdr *)buf;

struct in_addr src;

printf("Traceroute %s (%s):\n",
       inet_ntoa(dst.sin_addr),host->h_name);

do
{
  
  setsockopt(sock_fd,IPPROTO_IP,IP_TTL,&i,sizeof(i));
  echo_rqst((u_int16_t)i);
  //echo_rqst((u_int16_t)i);
  //echo_rqst((u_int16_t)i);
  
  tmp=padre;
  espera.tv_sec=3;
  espera.tv_usec=0;
  
  res=select(sock_fd+1,&tmp,NULL,NULL,&espera);
  
  if(res<0)
  {
    perror("select");
    continue;
  }
  
  else if(res==0)
    
  {
     printf("%u)\t* * * * * \n",i);
     i++;
     continue;
  }
  
 do{
  
  if(recvfrom(sock_fd,buf,BUFF_LEN,0,(struct sockaddr*)&from,&len)<0)
  {
    perror("recvfrom");
  }
  
 
//  printf("seq = %u\n",rcv_h->un.echo.sequence);
 }while(i!=rcv_h->un.echo.sequence &&( n_icmp_h->type!=ICMP_ECHOREPLY)); 
 
 if((n_icmp_h->type==ICMP_TIME_EXCEEDED || n_icmp_h->type==ICMP_ECHOREPLY ))
  {
    src.s_addr= ip_h->saddr;
   
  host=gethostbyaddr(&from,sizeof(from),AF_INET);
   
   printf("%u)\tReceived %d bytes from %s:  ",
	  i,ntohs(ip_h->tot_len),
	  inet_ntoa(src)
	 );
   
   ip_location(inet_ntoa(src));
   
  i++;
  }
  
  
  
}while(n_icmp_h->type!=ICMP_ECHOREPLY);
 
    
    return 0;
}
