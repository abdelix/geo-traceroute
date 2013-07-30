

#include "geoloc_ip.h"

GeoIP          *gi;

static const char * _mk_NA( const char * p ){
 return p ? p : "N/A";
}

int init_GeoLocIp()
{
   gi = GeoIP_open("/home/abdel/projects/traceroute/GeoLiteCity.dat", GEOIP_INDEX_CACHE);

  if (gi == NULL) {
    fprintf(stderr, "GeopIP:Error opening database\n");
    return 1;
  }
  GeoIP_set_charset(gi, GEOIP_CHARSET_UTF8);
  return 0;
}



int ip_location(char *host)
{
 
  
  GeoIPRecord    *gir;
  
  const char     *time_zone = NULL;
  char          **ret;
 

 

 

  
    gir = GeoIP_record_by_name(gi, (const char *) host);

    if (gir != NULL) {
      ret = GeoIP_range_by_ip(gi, (const char *) host);
      time_zone = GeoIP_time_zone_by_country_and_region(gir->country_code, gir->region);
      printf("%s\t%s\t%s\n",
	     _mk_NA(gir->country_code),
	     _mk_NA(GeoIP_region_name_by_code(gir->country_code, gir->region)),
	     _mk_NA(gir->city));
      GeoIP_range_by_ip_delete(ret);
      GeoIPRecord_delete(gir);
    }
    else printf("Location not found\n");
  
  
  
  return 0;

}

void finish_geolocip()
{
  GeoIP_delete(gi);
}