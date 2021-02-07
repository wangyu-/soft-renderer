#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h> //For errno - the error number
#include <fcntl.h>
#include <assert.h>

#include <vector>
#include <string>

using namespace std;
const int MAX_WIDTH=1920;
const int MAX_HEIGHT=1080;

extern int WIDTH;
extern int HEIGHT;
extern int LEFT;
extern int RIGHT;
extern int BOTTOM;
extern int UP;

extern int light_x;  //light source
extern int light_y;
extern int light_z;

//const int MODE_ROTATE=0;  // rotate mode
//const int MODE_TRANS=1;   // translation mode
//const int MODE_SCALE=2;   // scale mode

const double EPS=0.000001;

extern int x_lll,y_lll;
extern int x_ll,y_ll;
extern int x_last,y_last;  //remember the last, last-last, last-last-last point 

extern int per;   //whether its perspective view or not
extern int wire;  //whether its wireframe mode or not
extern int ray_tracing;

extern int r_speed; //speed of rotate in degree/
extern int resize; //a factor to normalize the size of model 

//extern char g_mode;   //this indicates the current mode

extern double g_scale;   //records the scale of model
extern double t_x,t_y,t_z;  //records the position of model

extern int need_refresh;  // an optimization for performance
extern double D;  //the projection plane as well as clipping plane
extern int phong_smoothing;
extern int bump;
extern int bump_type;
const int bump_type_num=4;

extern int topo;
extern int real_time;

vector<string> string_to_vec(const char * s,const char * sp);
vector< vector <string> > string_to_vec2(const char * s);
int read_file(const char * file,string &output);
int prt_vec(vector<string> &a);
int prt_vec2(vector<vector<string> > &a);

long long get_current_time();   //a function to help analysis performance

inline int my_abs(int a)   // cal absolute value
{
	return a>=0?a:-a;
}

inline double my_abs_d(double a)   // cal absolute value
{
	return a>=0?a:-a;
}
inline int my_round(double a) // round function
{
	if(a>0) return a+0.5; 
	if(a<0)return a-0.5;
	return 0;
}

struct mat_t  //the type of matrix
{
	double arr[4][4];
};

struct RgbColor //the type of RGB color
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

struct rgb_t //the type of double rgb color
{
	double c[3]={0,0,0};
	double &r(){return c[0];}
	double &g(){return c[1];}
	double &b(){return c[2];}
};

extern mat_t ident;

/*
struct point3_t
{
	double x,y,z;
};
typedef point3_t vector3_t;*/

inline bool eq(double a,double b)
{
	return my_abs_d(b-a)<EPS;
}
