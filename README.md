## geo-traceroute
By Abdelfetah Hadij \(<abdel.14@gmail.com>\)

A traceroute wich gives you the geographical location of the intemediate nodes.



#### Features

* Works on any unix like system.
* Geolocation based on ip's of nodes.

#### To- do 
* Implment  a subset of original traceroute comandline options


#### Get the source

[Go to GitHub Repository](http://github.com/abdelix/geo-traceroute)
  
  
[Download git repository as zip](https://github.com/abdelix/geo-traceroute/archive/master.zip)

#### Dependencies

The dependencies are :
* Standard Unix libs
* [nxjson](https://bitbucket.org/yarosla/nxjson/overview) : A lib for parsing json
* [curl](http://curl.haxx.se/‎)


#### Build

In a shell you must run in the main directory of the project :
  
	$ make


#### Run
In order to run you must either run it as root or set the coresponding capabilities to the executable(as root)

I

	$ ./geo-traceroute
