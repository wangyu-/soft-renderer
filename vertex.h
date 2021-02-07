#pragma once
#include <math.h>
#include <map>
using namespace std;
struct vertex_t //the type of vertex. can also be used as vector
//although it's 4d, most time we only use 3 dimensions
{
	//vector<double> v;
	double v[4]={0,0,0,1};

	double &x(){return v[0];}
	double &y(){return v[1];}
	double &z(){return v[2];}
	double &w(){return v[3];}

	/*vertex_t()
	{
		//v.push_back(0);
		//v.push_back(0);
		//v.push_back(0);
		//v.push_back(1.0);
	}*/

	void prt()
	{
		printf("<%.2f,%.2f,%.2f,%.2f>",x(),y(),z(),w());
	}
	void normal_inplace() //normalize the vertex inplace
	{
		x()/=w();
		y()/=w();
		z()/=w();
		w()=1.0;
	}	
	vertex_t normal_new()//normalize and generate a new vertex
	{
		vertex_t tmp=*this;
		tmp.w()=1.0;
		tmp.x()=x()/w();
		tmp.y()=y()/w();
		tmp.z()=z()/w();
		return tmp;
	}
	vertex_t mul_new(mat_t &m) //multiplication vertex by a matrix
	{
		vertex_t tmp;
		int i,j;
		for(i=0;i<4;i++)
		{
			double sum=0;
			for(int j=0;j<4;j++)
			{
				sum+=m.arr[i][j]*v[j];
			}
			tmp.v[i]=sum;
		}
		return tmp;
	}
	void mul_inplace(mat_t &m)//the inplace version of matrix multiplication
	{
		vertex_t tmp=mul_new(m);
		*this=tmp;
	}
	vertex_t minus_new(vertex_t &b) // vector minus
	{
		vertex_t res;
		res.v[3]=1;
		for(int i=0;i<3;i++)
		{
			res.v[i]=v[i]-b.v[i];
		}
		return res;
	}
	vertex_t add_new(const vertex_t &b) //vector add
	{
		vertex_t res;
		res.v[3]=1;
		for(int i=0;i<3;i++)
		{
			res.v[i]=v[i]+b.v[i];
		}
		return res;
	}
	vertex_t mul_new(double d) //multiply with a number
	{
		vertex_t res=*this;
		for(int i=0;i<3;i++)
		{
			res.v[i]*=d;
		}
		return res;
	}
	vertex_t unit_len_new() //get unit len vector
	{
		vertex_t res=*this;
		double sum=0;
		for(int i=0;i<3;i++)
		{
			sum+=res.v[i]*res.v[i];
		}
		double d=sqrt(sum);
		for(int i=0;i<3;i++)
		{
			res.v[i]/=d;
		}
		return res;
	}
	vertex_t cross_new(vertex_t &b) //cross product of vector
	{
		vertex_t res;
		vertex_t &a=*this;
		res.x()=a.y()*b.z()-b.y()*a.z();
		res.y()=- (a.x()*b.z()-b.x()*a.z());
		res.z()=a.x()*b.y()-b.x()*a.y();
		return res;
	}
	double dot(const vertex_t &b) //dot product of vector
	{
		double sum=0;
		for(int i=0;i<3;i++)
		{
			sum+=v[i]*b.v[i];			
		}
		return sum;
	}
	double distance(vertex_t &b)// distance from vertex to vertex
	{
		vertex_t tmp=b.minus_new(*this);
		return sqrt(tmp.dot(tmp));
	}
	double distance_to_line(vertex_t &b,vertex_t &c) //distance from vertex to line
	{
		vertex_t &a=*this;
		vertex_t ba=a.minus_new(b);
		vertex_t bc=c.minus_new(b);
		double len_ab_2=ba.dot(ba);
		double len_bp=ba.dot(bc.unit_len_new());
		double len_bp_2=len_bp*len_bp;
		double len_ap_2=len_ab_2-len_bp_2;
		if(eq(len_ap_2,0)) len_ap_2=0;
		assert(len_ap_2>=0);
		return sqrt(len_ap_2);
		
	}
	vertex_t ne_new() //get the negative vector
	{
		vertex_t o;
		return o.minus_new(*this);
	}
};

inline int cmp(const vertex_t &a,const vertex_t &b) // compare function, so that we can sort an array of vector with respect to x value
{
	return a.v[0]<b.v[0];
}

inline bool operator<(const vertex_t& a, const vertex_t& b)  //define the compare function of two vertex, so that we can use vertex as the key of a map
{
	if(a.v[0]!=b.v[0]) return a.v[0]<b.v[0];
	if(a.v[1]!=b.v[1]) return a.v[1]<b.v[1];
	if(a.v[2]!=b.v[2]) return a.v[2]<b.v[2];
	return a.v[3]<b.v[3];
}

struct face_t  //the type of face, face consists of idx of vertex and color
{
	vector<int> v; //index of each point
	vector<vertex_t> v_cache;//cache of vertex_t;
	vertex_t avg_cache;//cache of avg;
	double d_cache;//cache of d;
	vertex_t normal_cache; //cache of normal of the face
	map<int,int> vn_sign; //cache of wether the vertex norml is in same direction with normal_cache or not
	RgbColor c;  //color of polygon, not used in ray tracing
	rgb_t kd,ks,ka;  // diffusion specular and ambient parameter of face
	double alpha=5; 
	face_t()
	{
		c.r=c.g=c.b=0;
		while(c.r+c.g+c.b<50)   //generate a random color. if it's not bright enough ,re-generate.
		{
			c.r=random()%256;
			c.g=random()%256;
			c.b=random()%256;
		}
		kd.c[0]=kd.c[1]=kd.c[2]=0.45;  //diffusion is 0.45
		ks.c[0]=ks.c[1]=ks.c[2]=0.45;  //specular is 0.45
		ka.c[0]=ka.c[1]=ka.c[2]=0.2;  //ambient is 0.2
	}
	void prt()
	{
		printf("<");
		for(int i=0;i<v.size();i++)
			printf("%d ",v[i]);
		printf(">");
	}
};
