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




struct sockaddr_in dst;
int sock_fd;

void echo_rqst(u_int16_t req,unsigned n)
{
  char buff[BUFF_LEN];
  int i;

  struct icmphdr *icmp_h;
  
  // set the time to live acordindingly
  setsockopt(sock_fd,IPPROTO_IP,IP_TTL,&req,sizeof(req));
  
  //Configuramos los parametros icmp
  
   memset(buff,0,BUFF_LEN);
  icmp_h=(struct icmphdr *)(buff);
  
  icmp_h->type=ICMP_ECHO;
  
  icmp_h->code=0;
  
  icmp_h->un.echo.id=31337;
  //the request secuence number
  icmp_h->un.echo.sequence=req;
  //We calculate the checksum
  icmp_h->checksum=in_cksum((uint16_t*)buff,DATA_LEN);



  for ( n;n>0; n--)
  {
    if(sendto(sock_fd,buff,DATA_LEN,0,(struct sockaddr*)&dst,sizeof(dst))<0)
    {
      perror("sendto");
    }
    
    usleep(1000*100);
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
  
  
 unsigned char i;
  int res;
  struct timeval waiting_time;
  
  
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
  
  // Set the destination address
  dst.sin_family=AF_INET;
  dst.sin_port=htons(IPPROTO_ICMP);
  dst.sin_addr.s_addr=*((uint32_t *)host->h_addr_list[0]);
  bzero(dst.sin_zero, 8);

 ///Create new RAW icmp socket
  
 sock_fd=socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);
 FD_SET(sock_fd,&padre);
 if(sock_fd<0)
 {
   perror("RAW socket creation failed  ");
   exit(1);
 }






ip_h=(struct iphdr *)buf;


rcv_h=(struct icmphdr *)(buf+20+8+20);
n_icmp_h=(struct icmphdr *)(buf+20);
ip_h= (struct iphdr *)buf;

struct in_addr src;

printf("Traceroute %s (%s):\n",
  
       inet_ntoa(dst.sin_addr),host->h_name);

i=1;



echo_rqst((u_int16_t)i,3);


do
{
  
  
  
  //copy the set since select changes it
  tmp=padre;
  
  // we set the waiting time on every iteration because select may change the timeval
  waiting_time.tv_sec=1;
  waiting_time.tv_usec=0;
  
  res=select(sock_fd+1,&tmp,NULL,NULL,&waiting_time);
  
  if(res<0)
  {
    perror("select");
    
    continue;
  }
  
  else if(res==0)
    
  {
    // If in 3 secs we dont receive any reply we pass to the next node 
     printf("%u)\t* * * * * \n",i);
     i++;
     echo_rqst((u_int16_t)i,3);
   
  
     continue;
  }
  

  
  if(recvfrom(sock_fd,buf,BUFF_LEN,0,(struct sockaddr*)&from,&len)<0)
  {
    perror("recvfrom");
  }
  
 
 // if we receive a response and is the one that we were expecting proccess it
 if((n_icmp_h->type==ICMP_TIME_EXCEEDED || n_icmp_h->type==ICMP_ECHOREPLY )&&i==rcv_h->un.echo.sequence)
  {
    src.s_addr= ip_h->saddr;
    
    // we convert ip to hostname if possible :
    char *f="";
    
    host=gethostbyaddr(&src,4,AF_INET);
    if (host)
    {
     f=host->h_name;
    }
      
     
    printf("%u)\tReceived %d bytes from %s (%+16s): %s \n",
	    i,ntohs(ip_h->tot_len),f,
	    inet_ntoa(src),ip_location(inet_ntoa(src))
	  );
   
   
    // go to the next node
    i++;
    
    echo_rqst((u_int16_t)i,3);
  
  
  
  
  }
  
  
  
}while(n_icmp_h->type!=ICMP_ECHOREPLY  || src.s_addr != ((struct sockaddr_in)dst).sin_addr.s_addr);
 
    
    return 0;
}
