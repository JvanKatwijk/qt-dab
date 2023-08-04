
#include	<math.h>

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  This function converts decimal degrees to radians             :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
double deg2rad (double deg) {
  return (deg * pi / 180);
}

double distance (double lat1, double lon1, double lat2, double lon2) {
double theta, dist;
theta = lon1 - lon2;

	dist = sin (deg2rad (lat1)) * sin (deg2rad (lat2)) +
	       cos (deg2rad (lat1)) * cos (deg2rad (lat2)) *
	                         cos (deg2rad (theta));
	dist = acos (dist);
	dist = rad2deg (dist);
	dist = dist * 60 * 1.1515;
	dist = dist * 1.609344;
	return (dist);
}

