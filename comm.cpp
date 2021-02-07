#include "comm.h"

int WIDTH=1024;
int HEIGHT=768;   // height and width of the window

int LEFT;
int RIGHT;
int BOTTOM;
int UP;

int light_x=200;
int light_y=100;
int light_z=0;

mat_t ident={{
{1,0,0,0},
{0,1,0,0},
{0,0,1,0},
{0,0,0,1},
}};//define an identity matrix

int x_lll=0,y_lll=0;
int x_ll=0,y_ll=0;
int x_last=0,y_last=0;  //remember the last, last-last, last-last-last point 

int per=1;   //whether its perspective view or not
int wire=1;  //whether its wireframe mode or not
int ray_tracing=0;

int r_speed=3; //speed of rotate in degree
int resize=1.2*2; //a factor to normalize the size of model 

//char g_mode=MODE_ROTATE;   //this indicates the current mode

double g_scale=1.0;   //records the scale of model
double t_x=0.0,t_y=0.0,t_z=-HEIGHT*1.5;  //records the position of model

int need_refresh=1;  // an optimization for performance
double D=-HEIGHT/2.0;  //the projection plane as well as clipping plane

vector<string> string_to_vec(const char * s,const char * sp) {
	  vector<string> res;
	  string str=s;
	  char *p = strtok ((char *)str.c_str(),sp);
	  while (p != NULL)
	  {
		 res.push_back(p);
	    //printf ("%s\n",p);
	    p = strtok(NULL, sp);
	  }

	 /* for(int i=0;i<(int)res.size();i++)
	  {
		  printf("<<%s>>\n",res[i].c_str());
	  }*/
	  return res;
}

vector< vector <string> > string_to_vec2(const char * s)
{
	vector< vector <string> > res;
	vector<string> lines=string_to_vec(s,"\n");
	for(int i=0;i<int(lines.size());i++)
	{
		vector<string> tmp;
		tmp=string_to_vec(lines[i].c_str(),"\t ");
		res.push_back(tmp);
	}
	return res;
}
int read_file(const char * file,string &output)
{
	const int max_len=50*1024*1024;
   // static char buf[max_len+100];
	string buf0;
	buf0.reserve(max_len+200);
	char * buf=(char *)buf0.c_str();
	buf[max_len]=0;
    //buf[sizeof(buf)-1]=0;
	int fd=open(file,O_RDONLY);
	if(fd==-1)
	{
		 printf("read_file %s fail\n",file);
		 return -1;
	}
	int len=read(fd,buf,max_len);
	if(len==max_len)
	{
		buf[0]=0;
        printf("%s too long,buf not large enough\n",file);
        return -2;
	}
	else if(len<0)
	{
		buf[0]=0;
        printf("%s read fail %d\n",file,len);
        return -3;
	}
	else
	{
		buf[len]=0;
		output=buf;
	}
	return 0;
}

int prt_vec(vector<string> &a)
{
	int i,j,k;
	for(i=0;i<a.size();i++)
	{
		printf("%s ",a[i].c_str());
	}
	printf("\n");
}
int prt_vec2(vector<vector<string> > &a)
{
	for(int i=0;i<a.size();i++)
		prt_vec(a[i]);
}

long long get_current_time()   //a function to help analysis performance
{
        timespec tmp_time;
        clock_gettime(CLOCK_MONOTONIC, &tmp_time);
        return tmp_time.tv_sec*1000l*1000*1000+tmp_time.tv_nsec;
}
