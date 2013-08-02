#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include <curl/curl.h>

#include "nxjson.h"

/**
 *  Here are the functions used to get the location of an ip.
 * 
 * 
 * 
 * 
 * */

#define LIST_SIZE 249



unsigned char * out_buf=NULL;
size_t idx=0;
  



size_t write_data(void *buff,size_t size,size_t mult,void *userdata)
{

  
  
  unsigned char * buf=(unsigned char *)buff;
  size=size*mult;
  size_t written=0;
  unsigned char * aux;
  
  
  aux=realloc(out_buf,size+idx+1);
  if(!aux && size!=0)
  {
    //error
    
    printf("Error allocatin mem\n");
    return -1;
    
  }
  
  else 
  {
    out_buf=aux;
  }
  
  
  while(written < size)
  {
    out_buf[idx]=*buf;
    
    buf++;
    idx++;
    written++;
  }
  
  out_buf[idx]=0;
  
  return written;
}

char * get(char *url)
{
  char curl_error[CURL_ERROR_SIZE];
  CURL * manejador=curl_easy_init();
  out_buf=NULL;
  idx=0;
  curl_easy_setopt(manejador,CURLOPT_URL,url);
  curl_easy_setopt(manejador,CURLOPT_USERAGENT,"geo-traceroute abdlix.github.io");
  curl_easy_setopt(manejador,CURLOPT_WRITEFUNCTION,write_data);
  curl_easy_setopt(manejador,CURLOPT_ERRORBUFFER,curl_error);
   if( curl_easy_perform(manejador))
    {
      printf("Error downloading data:\n%s\n",curl_error);
      free(out_buf);
      return NULL;
      
    }
    
    return out_buf;
    
}


/**
 * 
 *  This function gets the locatio of an ip by asking to : http://ip-api.com/json/
 *  geting the reponse and parsing it.
 * 
 *  @return : an string describing de location example : "United States, California, Mountain View"
 *            if not found returns the messsage "Not found:{error message }"
 * 
 **/

char * ip_location(char *ip)
{
  char url[256];
  char *ret_val;
  sprintf(url,"http://ip-api.com/json/%s",ip);
  char *json_str=get(url);
  char *country,*region,*city;
  const nx_json  *json_data=nx_json_parse_utf8(json_str);

  nx_json *prop;
  
  if( !strcmp(nx_json_get(json_data,"status")->text_value,"success"))
  {
    //exito
    country=nx_json_get(json_data,"country")->text_value;
    region=nx_json_get(json_data,"regionName")->text_value;
    city=nx_json_get(json_data,"city")->text_value;
    
    ret_val=malloc(6+strlen(country)+strlen(region)+strlen(city));
    sprintf(ret_val,"%s, %s, %s",country,region,city);
    
  }
  else 
  {
    //failure
    prop=nx_json_get(json_data,"message");
    ret_val=malloc(12+strlen(prop->text_value));
    sprintf(ret_val,"Not Found:%s",(prop->text_value));
    
  }
  
  
  
  free(json_str);
  nx_json_free(json_data);
  
  return ret_val;
  
}


