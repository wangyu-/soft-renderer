#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

#include "comm.h"
#include "basic_draw.h"
#include "vertex.h"
#include "model.h"
#include "canvas.h"


struct light_t   // the type of light sources
{
    vertex_t v;
    rgb_t i;
};

vector<light_t> lights; // vector of light sources

void add_light(int x,int y,int z)  //init light source(s)
{
	lights.clear();
	light_t light;
	light.i.r()=1.0;
	light.i.g()=1.0;
	light.i.b()=1.0;

	light.v.x() = x;
	light.v.y() = y;
	light.v.z() = z;
	lights.push_back(light);

	/*light.v.x() = -500;
	  light.v.y() = 0;
	  light.v.z() = -480;
	  lights.push_back(light);*/

}


inline double det3( vertex_t &a, vertex_t &b, vertex_t &c) //compute the det of 3x3 matrix (break into 3 3x1 vectors)
{
    double sum1=a.x()*(b.y()*c.z()-c.y()*b.z());
    double sum2=b.x()*(a.y()*c.z()-c.y()*a.z());
    double sum3=c.x()*(a.y()*b.z()-b.y()*a.z());
    //printf("%f\n",sum1-sum2+sum3);
    return sum1-sum2+sum3;
}
double find_intersection(vertex_t ro, vertex_t rd, vector<vertex_t>& tri,double &beta,double &gamma) //find intersection of the ray with a triangle
{
    assert(tri.size()==3);
    vertex_t &a=tri[0];
    vertex_t &b=tri[1];
    vertex_t &c=tri[2];

    vertex_t neb=a.minus_new(b);
    vertex_t nec=a.minus_new(c);
    vertex_t ar=a.minus_new(ro);
    double det_a=det3(neb,nec,rd);
    if(eq(det_a,0)) return -1;
    double det_t=det3(neb,nec,ar);
    double t=det_t/det_a;
    if(eq(t,0)||t<0) return -1;
    double det_c=det3(neb,ar,rd);
    gamma=det_c/det_a;
    if( !eq(gamma,0)&&gamma <0 ) return -1;
    if( !eq(gamma,1)&&gamma >1 ) return -1;
    double det_b=det3(ar,nec,rd);

    beta=det_b/det_a;

    if( (!eq(beta,0)&&beta<0) ) return -1;
    if( (!eq(beta,1)&&beta>1) ) return -1;

    if(!eq(beta+gamma,1)&& (beta+gamma>1)) return -1;

    //vertex_t left=ro.add_new(rd.mul_new(t));

    //vertex_t right=a.add_new(b.minus_new(a).mul_new(beta)).add_new(c.minus_new(a).mul_new(gamma));

    //printf("<%.2f %.2f %.2f>, <%.2f %.2f %.2f>\n",left.x(),left.y(),left.z(),right.x(),right.y(),right.z());
    

    return t;
}
double find_nearest_intersection(vertex_t ro, vertex_t rd, model_t &model,int &idx,int exclude,double & beta,double &gamma)  //find the nearest intersection from a set of triangle
{
        double inf=1e100;
        double t_min=inf;
        auto &faces=model.faces;
        for(int j=0;j<faces.size();j++)
        {
            if(exclude==j) continue;
            double tmp_beta,tmp_gamma;
            double t=find_intersection(ro,rd,faces[j].v_cache,tmp_beta,tmp_gamma);            
            if(t<0) continue;
            if(t<t_min) 
            {
               t_min=t;
               idx=j;
               beta=tmp_beta;
               gamma=tmp_gamma;
		if(exclude!=-1) return t_min;  //optimization, for shadow ray, we do not need to find the nearest intersection
            }
        }
        if(t_min==inf) return -1;
        return t_min;
}
struct pixel_t   //the type of pixel on ray-tracing virtual screen
{
    int x,y;
};
int phong_smoothing=0;   //switch of phong_smoothing
int bump=0;  // switch of bump mapping
int bump_type=0;  // type of bump mapping
int real_time=0;   // enable real time drawing or not

int ray_tracing_pixel(model_t &model,pixel_t &pixel) //do ray-tracing for a given pixel and a model
{
        vertex_t ro;
        ro.x()=pixel.x;
        ro.y()=pixel.y;
        ro.z()=D;
        ro.w()=1;
        vertex_t rd;
        vertex_t original;
        if(per==1)
        {
            rd=ro.minus_new(original).unit_len_new();
        }
        else
        {
            rd.x()=0;rd.y()=0;
            rd.z()=-1;rd.w()=1;
        }
        int idx;
        double beta,gamma;
        double t=find_nearest_intersection(ro,rd,model,idx,-1,beta,gamma); 
        if(t<0) return -1;
        
        face_t &face=model.faces[idx];
        vertex_t normal0 = model.faces[idx].normal_cache;
        vertex_t normal1 = normal0;
        if(phong_smoothing) //handle phone_smoothing
        {
            vertex_t na, nb, nc,sum,zero;
            na = model.v_norm_cache[face.v[0]];
            nb = model.v_norm_cache[face.v[1]];
            nc = model.v_norm_cache[face.v[2]];
            if(1)
            {
		//printf("<%d %d %d>\n",face.vn_sign[face.v[0]],face.vn_sign[face.v[1]],face.vn_sign[face.v[2]]);
                if (face.vn_sign[face.v[0]] < 0)
                    na = na.ne_new();
                if (face.vn_sign[face.v[1]] < 0)
                    nb = nb.ne_new();
                if (face.vn_sign[face.v[2]] < 0)
                    nc = nc.ne_new();
            }
		/*
            if(1)
            {
                if (na.dot(normal1) < 0)
                    na = na.ne_new();
                if (nb.dot(normal1) < 0)
                    nb = nb.ne_new();
                if (nc.dot(normal1) < 0)
                    nc = nc.ne_new();
            }*/
            double w_a = 1 - beta - gamma;
            double w_b = beta;
            double w_c = gamma;
            sum=sum.add_new(na.mul_new(w_a));
            sum=sum.add_new(nb.mul_new(w_b));
            sum=sum.add_new(nc.mul_new(w_c));
            sum=sum.unit_len_new();
            normal1=sum;
        }
	if(normal0.dot(rd)>0)
	{
		normal0=normal0.ne_new();
		normal1=normal1.ne_new();
	}


        vertex_t intersec=ro.add_new(rd.mul_new(t));
        rgb_t sum;
        for(int i=0;i<lights.size();i++)//for each lights, do ray tracing for the shadow ray
        {
            for(int j=0;j<3;j++)
            {
                sum.c[j]+=lights[i].i.c[j]*face.ka.c[j];  //calculate ambient light
            }
            vertex_t &ro_new = intersec;
            vertex_t rd_ray_ne = lights[i].v.minus_new(ro_new).unit_len_new();

            vertex_t rd_ray = rd_ray_ne.ne_new();
            vertex_t normal0_copy = normal0;
            vertex_t normal1_copy = normal1;
	    if(bump)  //handle bump mapping
		{
			if(bump_type==0||bump_type==1) // bump and dimp
			{
				double d_range=face.d_cache*0.6;
				double d=intersec.distance(face.avg_cache);
				if(d<=d_range)
				{
					vertex_t n_edge=intersec.minus_new(face.avg_cache).unit_len_new();
					if(bump_type==0) n_edge=n_edge.ne_new();
					vertex_t n_center=normal1_copy;
					n_edge=n_edge.add_new(n_center.mul_new(2)).unit_len_new();
					double w_edge=d/d_range;
					w_edge=pow(w_edge,3);
					//double w_center=(d_range-d)/d_range;
					double w_center=1-w_edge;
					//w_center=sqrt(w_center);
					normal1_copy=n_edge.mul_new(w_edge).add_new(n_center.mul_new(w_center)).unit_len_new();
				}
			}
			else if(bump_type==2) // circle with wave effect
			{
				double d_range=face.d_cache*0.8;
				double d=intersec.distance(face.avg_cache);
				if(d<=d_range)
				{
					normal1_copy.x()+=sin(10*3.14159*beta);
					normal1_copy=normal1_copy.unit_len_new();
				}
			}
			else if(bump_type==3)// wave effect for the whole triangle
			{
					normal1_copy.x()+=sin(10*3.14159*beta);
					normal1_copy=normal1_copy.unit_len_new();	
			}
		}
            if(0)
            {
                if (normal1_copy.dot(rd) > 0)
                {
                    normal1_copy = normal1_copy.ne_new();
                }
            }
            double proj = normal1_copy.dot(rd_ray_ne);
            //printf("%f\n", proj);
            //assert(eq(proj, 0) || proj > 0);
            if(!phong_smoothing||1)
            {
                if (eq(proj, 0) || proj < 0)     // if projection is negative, then no specular and diffusion
                    continue;
            }
            int idx2;
            double dummy_beta,dummy_gamma;
            double t2 = find_nearest_intersection(ro_new, rd_ray_ne, model, idx2,idx,dummy_beta,dummy_gamma);  //do ray tracing for shadow ray
            if(!phong_smoothing||1)
            {
                if(eq(t2,0)||t2>0) 
                    continue;
            }

            vertex_t normal_mirror = normal1_copy.mul_new(proj * 2);
            vertex_t reflect_ray = normal_mirror.minus_new(rd_ray_ne);

            double cos_theta=normal1_copy.dot(rd_ray_ne);
            if(cos_theta<0) cos_theta=0;
            double cos_fye=rd.ne_new().dot(reflect_ray);
            if(cos_fye<0) cos_fye=0;
            cos_fye=pow(cos_fye,face.alpha);
            for(int j=0;j<3;j++)
            {
                sum.c[j]+=lights[i].i.c[j]*face.kd.c[j]*cos_theta;  // diffusion light
                sum.c[j]+=lights[i].i.c[j]*face.ks.c[j]*cos_fye;  //specular light
            }
        }

        for (int j = 0; j < 3; j++)
        {
	    assert(sum.c[j]>=0);
            if(sum.c[j] >=1) sum.c[j]=0.9999;
        }
        write_pixel_rgb_d(pixel.x, pixel.y, sum);  //draw pixel
        return 1;
}
bool visited[MAX_HEIGHT+10][MAX_WIDTH+10];

int dir_num=8;
int dir_x[]={1,-1,0,0,1,1,-1,-1};
int dir_y[]={0,0,1,-1,1,-1,-1,1};

void flood_fill(model_t &model,pixel_t pixel,vector<pixel_t> &queue)  //a flood fill optimization for ray tracing
{
    if(pixel.x<=LEFT||pixel.x>=RIGHT||pixel.y<=BOTTOM||pixel.y>=UP) return;
    int new_y=pixel.y-BOTTOM,new_x=pixel.x-LEFT;
    assert(new_y>=0&&new_y<HEIGHT);
    assert(new_x>=0&&new_x<WIDTH);
    if (visited[new_y][new_x]==1) return;
    visited[new_y][new_x]=1;
    int ret=ray_tracing_pixel(model,pixel);
    if(ret!=-1)
    {
        for(int j=0;j<dir_num;j++)
        {
            pixel_t new_pixel;
            new_pixel.x=pixel.x+dir_x[j];
            new_pixel.y=pixel.y+dir_y[j];
            queue.push_back(new_pixel);

            //flood_fill(model,new_pixel,queue);
        }
    }
}
void model_t::draw_rt()  //method of draw ray tracing
{
    //draw_fill();
    memset(visited,0,sizeof(visited));
    //vector<pixel_t> screen;
    gen_cache();
    //double sum=0;
    /*
    for(int x=LEFT; x<=RIGHT; x++) 
    {
        for(int y=BOTTOM; y<=UP; y++)
        {
            pixel_t pixel;
            //pixel.w()=1;
            //pixel.z()=D;
            pixel.x=x;
            pixel.y=y;
            screen.push_back(pixel);
        }
    }*/
    vector<pixel_t> flood_queue;
    flood_queue.reserve(HEIGHT*WIDTH*10);
    for(int i=0;i<vertexs.size();i++)
    {
        pixel_t pixel;
        double z=vertexs[i].z();
        pixel.x= my_round( vertexs[i].x()*D/z ); 
        pixel.y= my_round( vertexs[i].y()*D/z ); 
        flood_queue.push_back(pixel);   //add all vertexs as flood seeds;
        for(int j=0;j<dir_num;j++)
        {
            pixel_t new_pixel;
            new_pixel.x=pixel.x+dir_x[j];
            new_pixel.y=pixel.y+dir_y[j];
            flood_queue.push_back(new_pixel); //add all neighboor as flood seeds
        }
    }
    for(int i=0;i<faces.size();i++)
    {
        assert(faces[i].v_cache.size()==3);
        double avg_x=0,avg_y=0,avg_z=0;
        for(int j=0;j<3;j++)
        {
            avg_x+=faces[i].v_cache[j].x()/3;
            avg_y+=faces[i].v_cache[j].y()/3;
            avg_z+=faces[i].v_cache[j].z()/3;
        }
        pixel_t pixel;
        pixel.x= my_round( avg_x  *D/avg_z  ); 
        pixel.y= my_round( avg_y  *D/avg_z  ); 
        flood_queue.push_back(pixel);  // add all average as flood seeds;
        for(int j=0;j<dir_num;j++)
        {
            pixel_t new_pixel;
            new_pixel.x=pixel.x+dir_x[j];
            new_pixel.y=pixel.y+dir_y[j];
            flood_queue.push_back(new_pixel);// add all neightboor of avg as flood seeds
        }
    }
    for(int i=0;i<flood_queue.size();i++)   //do flood fill
    {
        flood_fill(*this,flood_queue[i],flood_queue);
	if(real_time&&i%10000==0) flush_canvas();                                      // Draw Frame Buffer 
    }
    if(real_time) printf("<done>\n");
    /*
    for(int i=0;i<screen.size();i++)
    {
        ray_tracing_pixel(*this,screen[i]);
    }*/
}
