#include "wmc_trace.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <inttypes.h>
#include <algorithm>
#include <vector>
#include <iomanip>

using namespace std;

/*
key=xml_uid
 q	region	shows	clicks	query
 u	region	url 	shows	clicks	query

sitesearch
----------
key=<search_id>
 q	region	queries	clicks	isWeb	query
 u	region	url 	shows	showsPos	clicks	clicksPos	isWeb	query
 
  wmc
 -----
 region 	url 	shows	showsPos	clicks	clicksPos	query

*/

const char* s1 = "83310		u	11193	www.study.ru/songs/kiss/asylum/7719.html	1	23	0	0	1	feed night";
const char* fname = "83310.dat";

int main( void )
{
	char	buf[1024];
	tr_stream << "This is a test";
	
	FILE* 	fp = fopen(fname, "r");
	if (!fp) { err_stream << " Can't open " << fname;  return 1;}
	int 	rc;
	int region, shows,clicks, isWeb;
	float  showsPos, clicksPos;
	char url[1024], query[1024];
	int ln = 0;
	while (!feof(fp)) {
		++ln;
		region = -1;
		if (7 == (rc = fscanf(fp, "u\t%d\t%s\t%d\t%f\t%d\t%f\t%d\t", &region, url, &shows, &showsPos, &clicks, &clicksPos, &isWeb))) {
			fgets(query, sizeof(query), fp);
			tr_stream << ln << "\tU:  rc=" << rc << ";\t" << region;
		} else if (3 == (rc = fscanf(fp, "q\t%d\t%d\t%d\t", &region, &shows, &clicks, &isWeb))) {
			fgets(buf, sizeof(buf), fp);
			tr_stream << ln << "\tQ:  rc=" << rc << ";\t" << region;
		} else if (rc == EOF) {
			break; // check errno 
		} else {
			fgets(buf, sizeof(buf), fp);
			tr_stream << ln << "\tUnknown: rc=" << rc << ";\t" << buf;
		}
	}

	tr_stream << "Test finished";
	return 0;
}


