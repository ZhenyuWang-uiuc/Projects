#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>

#define PI 3.141592653587

using namespace std;

/* calc_dist_v2 */
double dist(double st_lat, double st_long, double en_lat, double en_long) {
    double lat, lon, x, y, ret; 
    lat = st_lat - en_lat;
    lat *= (PI/180.0); //finding difference of latitudes in radians
    lon = st_long - en_long;
    lon *= (PI/180.0); //finding difference of longitudes in radians
    x = sin(lat/2)*sin(lat/2) + cos(st_lat*(PI/180.0))*cos(en_lat*(PI/180.0))*sin(lon/2)*sin(lon/2);
    y = 2*atan2(sqrt(x), sqrt(1-x));
    ret = 6371 * y * 0.6213712;
  
    return ret; // returns distance in miles
}

