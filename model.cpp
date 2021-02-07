#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

#include "comm.h"
#include "basic_draw.h"
#include "vertex.h"
#include "model.h"

#include <algorithm>
using namespace std;
model_t model;

vector<vertex_t> clip_vec(vector<vertex_t> &input, double d) //clip a polygon, z=d.  any part of the polygon is clipped if z>d
//implemented using Sutherland–Hodgman algorithm :  https://en.wikipedia.org/wiki/Sutherland%E2%80%93Hodgman_algorithm
{
	//return input;
	vector<vertex_t> output;
	if (input.size() < 3)
		return output;
	for (int i = 0; i < input.size(); i++)
	{
		vertex_t prev = input[(i + input.size() - 1) % input.size()];
		vertex_t current = input[i];
		/*if(prev.z()>current.z())
		{
			auto tmp=prev;prev=current;current=tmp;
		}*/
		vertex_t is;
		is.z() = d;
		is.x() = prev.x() + (current.x() - prev.x()) * (d - prev.z()) / (current.z() - prev.z());
		is.y() = prev.y() + (current.y() - prev.y()) * (d - prev.z()) / (current.z() - prev.z());
		if (current.z() <= d)
		{
			if (prev.z() > d)
			{
				output.push_back(is);
			}
			output.push_back(current);
		}
		else if (prev.z() <= d)
		{
			output.push_back(is);
		}
	}
	return output;
}

void model_t::draw_face_fill(vector<vertex_t> &vec, RgbColor c) //draw a polygon in z-buffer mode
{
	//assert(vec0.size()>=3);
	if (vec.size() < 3)
		return;

	double y_min = 1e10;
	double y_max = -1e10;
	for (int i = 0; i < vec.size(); i++) //get ymax and ymin of polygon
	{
		if (vec[i].y() > y_max)
			y_max = vec[i].y();
		if (vec[i].y() < y_min)
			y_min = vec[i].y();
	}
	vec.push_back(vec[0]);
	int y_l = my_round(y_min);
	int y_h = my_round(y_max);
	if (y_l < BOTTOM)
		y_l = BOTTOM;
	if (y_h > UP)
		y_h = UP;
	//printf("<y_l=%d,y_h=%d>\n",y_l,y_h);
	for (int y = y_l; y <= y_h; y++) // scanline algorithm, scan through ymin and ymax
	{
		vector<vertex_t> i_vec;
		for (int i = 1; i < vec.size(); i++)
		{
			vertex_t v1 = vec[i - 1];
			vertex_t v2 = vec[i];
			if (v1.y() > v2.y())
			{
				auto tmp = v1;
				v1 = v2;
				v2 = tmp;
			}
			if (v1.y() == v2.y())
				continue;
			if (y < v1.y() || y > v2.y())
				continue;
			vertex_t ii;
			ii.y() = y;
			ii.x() = v1.x() + (v2.x() - v1.x()) * (y - v1.y()) / (v2.y() - v1.y());
			ii.z() = v1.z() + (v2.z() - v1.z()) * (y - v1.y()) / (v2.y() - v1.y());
			//printf("<x=%.2f ,v1.x=%.2f,v2.x=%.2f v1.y=%.2f v2.y=%.2f>\n",ii.x(),v1.x(),v2.x(),v1.y(),v2.y());
			i_vec.push_back(ii);
		}
		//printf("<<%d>>\n",i_vec.size());
		if (i_vec.size() % 2 != 0)
		{
			printf("error:i_vec.size() is not even\n");
			continue;
		}
		sort(i_vec.begin(), i_vec.end(), cmp);
		for (int i = 0; i < i_vec.size(); i += 2)
		{
			vertex_t v1 = i_vec[i];
			vertex_t v2 = i_vec[i + 1];
			//printf("%f %f",v1.x(),v2.x());
			scan_line(y, my_round(v1.x()), my_round(v2.x()), v1.z(), v2.z(), c);
			//printf("<y=%d,x1=%d,x2=%d>\n",y,my_round(v1.x()),my_round(v2.x()));
		}
	}
	//printf("\n");
}

model_t model_t::clip_new() //clip the mode and generate a new model
{
	//return *this;
	model_t new_model;
	map<vertex_t, int> mp; //use this map to find dumpicate vertice of faces
	int cnt = 0;
	for (int i = 0; i < faces.size(); i++)
	{
		vector<vertex_t> vec;
		for (int j = 0; j < faces[i].v.size(); j++)
		{
			vec.push_back(vertexs[faces[i].v[j]]);
		}
		if (vec.size() < 3)
			continue;
		auto vec2 = clip_vec(vec, D);
		if (vec2.size() < 3)
			continue;
		face_t face;
		face.c = faces[i].c;
		for (int j = 0; j < vec2.size(); j++)
		{
			auto it = mp.find(vec2[j]);
			int idx;
			if (it == mp.end())
			{
				idx = cnt++;
				mp[vec2[j]] = idx;
				new_model.vertexs.push_back(vec2[j]);
			}
			else
			{
				idx = it->second;
			}
			face.v.push_back(idx);
		}
		new_model.faces.push_back(face);
	}
	return new_model;
}

void model_t::prt()
{
	printf("=======model begin========\n");
	for (auto it = vertexs.begin(); it != vertexs.end(); it++)
		it->prt();
	printf("\n");
	printf("--------------------------\n");
	for (auto it = faces.begin(); it != faces.end(); it++)
		it->prt();
	printf("\n");
	printf("=======model end========\n");
}
void model_t::normal_inplace() //noraml all vertexs so that w value is equal to 1
{
	for (int i = 0; i < vertexs.size(); i++)
		vertexs[i].normal_inplace();
}
vertex_t model_t::get_center() //get center of model
							   //call normalize before using this
{
	vertex_t sum;
	for (int i = 0; i < vertexs.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			sum.v[j] += vertexs[i].v[j];
		}
	}
	vertex_t avg = sum;
	for (int i = 0; i < 3; i++)
	{
		avg.v[i] /= vertexs.size();
	}
	return avg;
}
void model_t::mul_inplace(mat_t &m) //multiply the model by a matrix
{
	for (int i = 0; i < vertexs.size(); i++)
		vertexs[i].mul_inplace(m);
}
void model_t::translate(double xx, double yy, double zz) //translate the model
{
	mat_t m = ident;
	m.arr[0][3] = xx;
	m.arr[1][3] = yy;
	m.arr[2][3] = zz;
	mul_inplace(m);
}
double model_t::get_max_abs() // get the max absolute value of the model
{
	double max_abs = 0;
	for (int i = 0; i < vertexs.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			double a = my_abs(vertexs[i].v[j]);
			if (a > max_abs)
				max_abs = a;
		}
	}
	return max_abs;
}
double model_t::get_max_radius() //get the max radius of the model
{
	double max_radius = 0;
	for (int i = 0; i < vertexs.size(); i++)
	{
		double sum = 0;
		for (int j = 0; j < 3; j++)
		{
			sum += vertexs[i].v[j] * vertexs[i].v[j];
		}
		double sqrt_sum = sqrt(sum);
		if (sqrt_sum > max_radius)
			max_radius = sqrt_sum;
	}
	return max_radius;
}
void model_t::scale(double xx, double yy, double zz) //scale the model
{
	mat_t m = ident;
	m.arr[0][0] = xx;
	m.arr[1][1] = yy;
	m.arr[2][2] = zz;
	mul_inplace(m);
}
void model_t::rotate_z(double theta) //rotate around z
{
	theta = theta * 3.1415926 / 180;
	mat_t m = ident;
	m.arr[0][0] = cos(theta);
	m.arr[1][1] = cos(theta);
	m.arr[1][0] = sin(theta);
	m.arr[0][1] = -sin(theta);
	mul_inplace(m);
}
void model_t::rotate_x(double theta) //rotate around x
{
	theta = theta * 3.1415926 / 180;
	mat_t m = ident;
	m.arr[1][1] = cos(theta);
	m.arr[2][2] = cos(theta);
	m.arr[2][1] = sin(theta);
	m.arr[1][2] = -sin(theta);
	mul_inplace(m);
}
void model_t::rotate_y(double theta) //rotate around y
{
	theta = theta * 3.1415926 / 180;
	mat_t m = ident;
	m.arr[0][0] = cos(theta);
	m.arr[2][2] = cos(theta);
	m.arr[2][0] = -sin(theta);
	m.arr[0][2] = sin(theta);
	mul_inplace(m);
}
void model_t::draw_wire() //draw in wireframe mode
{
	g_color = g_white;
	for (int i = 0; i < faces.size(); i++)
	{
		face_t &face = faces[i];
		vector<vertex_t> vec;
		for (int j = 0; j < face.v.size(); j++)
		{
			vec.push_back(vertexs[face.v[j]]);
		}
		if (vec.size() < 3)
			continue;
		vec.push_back(vec[0]);

		for (int j = 1; j < vec.size(); j++)
		{
			line_new(vec[j - 1].x(), vec[j - 1].y(), vec[j].x(), vec[j].y());
		}
	}
}
void model_t::draw_fill() //draw the moddel in z-buffer mode
{
	init_z_buf();
	for (int i = 0; i < faces.size(); i++)
	{
		face_t &face = faces[i];
		vector<vertex_t> vec;
		for (int j = 0; j < face.v.size(); j++)
		{
			vec.push_back(vertexs[face.v[j]]);
		}
		draw_face_fill(vec, face.c);
	}
}
void model_t::draw_comm() //common function for drawing in non-perspective and perspective
{
	model_t new_model = *this;
	//new_model.translate(WIDTH / 2.0, HEIGHT / 2.0, 0);

	if (wire == 1)
	{
		new_model.draw_wire();
	}
	else
	{
		new_model.draw_fill();
	}
	//glutSwapBuffers();
}
void model_t::draw_orth() //draw in non-perspective mode
{
	model_t new_model = (*this).clip_new();
	new_model.draw_comm();
}
void model_t::draw_per() //draw in perspective mode
{
	model_t new_model = (*this).clip_new();
	//new_model.scale(2,2,2);
	//new_model.translate(0,0,-1.5*HEIGHT);
	model_t model_bak = new_model;
	mat_t m = ident;
	m.arr[3][3] = 0;
	m.arr[3][2] = 1 / D;
	new_model.mul_inplace(m);
	new_model.normal_inplace();
	for (int i = 0; i < new_model.vertexs.size(); i++)
		new_model.vertexs[i].z() = model_bak.vertexs[i].z();
	new_model.draw_comm();
}
int topo=1;
void model_t::gen_cache()   //gen cache values for vertexs for further use
{
	for(int i=0;i<faces.size();i++)
	{
		faces[i].v_cache.clear();
		for(int j=0;j<faces[i].v.size();j++)
		{
			faces[i].v_cache.push_back(vertexs[faces[i].v[j]]);
		}
		assert(faces[i].v_cache.size()==3);
		vertex_t &a=faces[i].v_cache[0];
		vertex_t &b=faces[i].v_cache[1];
		vertex_t &c=faces[i].v_cache[2];
		vertex_t ab=b.minus_new(a);
		vertex_t ac=c.minus_new(a);
		faces[i].normal_cache=ab.cross_new(ac).unit_len_new();  // normal of triangle
		vertex_t avg;
		avg=avg.add_new(a.mul_new(1./3.0));
		avg=avg.add_new(b.mul_new(1./3.0));
		avg=avg.add_new(c.mul_new(1./3.0));
		double min_d=1e100;
		double d;
		d=avg.distance_to_line(a,b);
		if(d<min_d) min_d=d;
		d=avg.distance_to_line(b,c);
		if(d<min_d) min_d=d;
		d=avg.distance_to_line(a,c);
		if(d<min_d) min_d=d;
		faces[i].avg_cache=avg;    //average of triangle
		faces[i].d_cache=min_d;    //max radius from avg

	}
	//v_norm_cache.clear();
	//inv_idx.clear();
	v_norm_cache.resize(vertexs.size());
	inv_idx.resize(vertexs.size());
	vertex_t zero;
	for(int i=0;i<vertexs.size();i++)
	{
		inv_idx[i].clear();
		v_norm_cache[i]=zero;
	}
	for(int i=0;i<faces.size();i++)
	{
		faces[i].vn_sign.clear();
		for(int j=0;j<faces[i].v.size();j++)
		{
			inv_idx[faces[i].v[j]].push_back(i);  //build inv_idx from vertex to face
		}
	}
	/*for(int i=0;i<vertexs.size();i++)
	{
		if(inv_idx[i].size()==0) continue;
		vertex_t normal0=faces[inv_idx[i][0]].normal_cache;
		v_norm_cache[i]=normal0;
		for(int j=1;j<inv_idx[i].size();j++)
		{
			int &idx=inv_idx[i][j];
			vertex_t normal=faces[idx].normal_cache;
			if(normal0.dot(normal)<0)
			{
				normal=normal.ne_new();				
			}
			v_norm_cache[i]=v_norm_cache[i].add_new(normal);
		}
		v_norm_cache[i]=v_norm_cache[i].unit_len_new();
	}*/
	long single_cnt=0;
	long fb_cnt=0;
	long basic_cnt=0;
	long bad_cnt=0;
	long topo_cnt=0;
	for(int i=0;i<vertexs.size();i++) // cacluate vertex normal and record the sign of vertex normal
	//we use 3 method for detection the right direction of face when calculate vertex normal:
	//1. base on the topology
	//2. base on the sum of edges with the common vertex
	//3. a fallback method
	//when first method fails, it will try to use the second method. and so on...
	// method 1 is the most accuracy one, but it requires the faces to have a well topology
	// method 2 success most of the time, but it fails when all faces are on a plane
	// method 3 almost always success
	{
		if(inv_idx[i].size()==0) continue;
		if(inv_idx[i].size()==1)    //trival case, a vertexs has only one face
		{
			v_norm_cache[i]=faces[inv_idx[i][0]].normal_cache;
			faces[inv_idx[i][0]].vn_sign[i]=1;
			single_cnt++;
			continue;
		}
		if(topo)   // calculate vertex normal base on topology ,very complex....
		{
			//bool used[inv_idx[i].size()]={};
			//used[0]=1;
			//int current=
			struct edge_t
			{
				int u,v;
				int used=0;
				int face_idx;
				int sign=0;
				void swap()
				{
					int tmp=u;
					u=v;
					v=tmp;
				}
			};
			vector<edge_t> edges;
			map<int,int> mp;
				
			for (int j = 0; j < inv_idx[i].size(); j++)
			{
				int &idx=inv_idx[i][j];
				face_t &face=faces[idx];	
				vector<int> tmp;
				for(int k=0;k<3;k++)	
				{
					if(face.v[k]==i) continue;	
					tmp.push_back(face.v[k]);
					if(mp.find(face.v[k])==mp.end())
					{
						mp[face.v[k]]=0;
					}
					mp[face.v[k]]++;
				}
				assert(tmp.size()==2);
				edge_t e;
				e.u=tmp[0];
				e.v=tmp[1];
				e.face_idx=idx;
				edges.push_back(e);
			}
			for(int j=0;j<edges.size();j++)
			{
				//printf("<%d,%d>",edges[j].u,edges[j].v);
			}
			int one_cnt=0;
			int start_v=edges[0].u;
			int not_ok=0;
			for(auto it=mp.begin();it!=mp.end();it++)
			{
				if(it->second==1) {one_cnt++;start_v=it->first;}
				if(it->second>2) one_cnt+=100;
			}
			if(one_cnt>2) not_ok=1;
			//if(not_ok) printf("not ok! ");
			if(!not_ok)
			{	
				int current=start_v;
				int used_cnt=0;
				vertex_t sum,a;
				a=vertexs[i];
				while(1)
				{
					int found=0;
					for(int j=0;j<edges.size();j++)
					{
						if(edges[j].used) continue;
						if(edges[j].v==current)
						{
							edges[j].swap();
						};
						if(edges[j].u==current)
						{
								edges[j].used=1;
								used_cnt++;
								current=edges[j].v;
								found=1;
								vertex_t b=vertexs[edges[j].u];
								vertex_t c=vertexs[edges[j].v];
								vertex_t ab=b.minus_new(a);
								vertex_t ac=c.minus_new(a);
								vertex_t normal=ab.cross_new(ac).unit_len_new();
								sum=sum.add_new(normal);
								int sign=1;
								if(normal.dot(faces[edges[j].face_idx].normal_cache)<0) sign=-1;
								edges[j].sign=sign;
						}
					}
					if(!found) break;
				}
				if(used_cnt==edges.size())
				{
					v_norm_cache[i] = sum.unit_len_new();
					if (!eq(v_norm_cache[i].dot(v_norm_cache[i]), 0))
					{
						//printf("ok!\n");
						topo_cnt++;
						for(int j=0;j<edges.size();j++)
						{
							faces[edges[j].face_idx].vn_sign[i]=edges[j].sign;
						}
						continue;
					}
					else
					{		
						//printf("not ok2! ");
					}
					
				}
				else
				{
					//printf("not ok3! <%d>",used_cnt);
				}
			}
			//else printf("not ok4!");
		}	
		//printf("\n");
		if(1) //method 2 for caclulate the vertex norml
		{
			vertex_t sum;
			for (int j = 0; j < inv_idx[i].size(); j++)
			{
				int &idx=inv_idx[i][j];
				face_t &face=faces[idx];
				for(int k=0;k<face.v.size();k++)
				{
					int &idx_v=face.v[k];
					if(idx_v==i) continue;
					vertex_t tmp = vertexs[idx_v];
					tmp = tmp.minus_new(vertexs[i]).unit_len_new();
					sum = sum.add_new(tmp);
				}

			}
			if(!eq(sum.dot(sum),0) &&!eq(sum.dot(faces[inv_idx[i][0]].normal_cache),0)) 
			{
				v_norm_cache[i]=zero;
				vector<int> sign;
				for (int j = 0; j < inv_idx[i].size(); j++)
				{
					int &idx = inv_idx[i][j];
					vertex_t normal = faces[idx].normal_cache;
					if (sum.dot(normal) > 0)
					{
						sign.push_back(-1);
						normal = normal.ne_new();
					}
					else	sign.push_back(1);
					v_norm_cache[i] = v_norm_cache[i].add_new(normal);
				}
				v_norm_cache[i] = v_norm_cache[i].unit_len_new();
				if(!eq(v_norm_cache[i].dot(v_norm_cache[i]),0))
				{
					basic_cnt++;
					for (int j = 0; j < inv_idx[i].size(); j++)
					{
						int &idx = inv_idx[i][j];
						faces[idx].vn_sign[i]=sign[j];
					}
					continue;
				}
			}
		}
		if(1) //method 3 of vertex normal (the last method)
		{
			//printf("use fallback vertex normal\n");
			vertex_t normal0=faces[inv_idx[i][0]].normal_cache;
			v_norm_cache[i]=normal0;
			vector<int> sign;
			for(int j=1;j<inv_idx[i].size();j++)
			{
				int &idx=inv_idx[i][j];
				vertex_t normal=faces[idx].normal_cache;
				if(normal0.dot(normal)<0)
				{
					sign.push_back(-1);
					normal=normal.ne_new();				
				} else sign.push_back(1);
				v_norm_cache[i]=v_norm_cache[i].add_new(normal);
			}
			v_norm_cache[i]=v_norm_cache[i].unit_len_new();
			if(!eq(v_norm_cache[i].dot(v_norm_cache[i]),0))	
			{
				fb_cnt++;
				for (int j = 0; j < inv_idx[i].size(); j++)
				{
					int &idx = inv_idx[i][j];
					faces[idx].vn_sign[i]=sign[j];
				}
				continue;
			}
		}
		if(1)
		{
			for (int j = 0; j < inv_idx[i].size(); j++)
			{
				int &idx = inv_idx[i][j];
				faces[idx].vn_sign[i]=2;
			}
			//if(eq(v_norm_cache[i].dot(v_norm_cache[i]),0))	
			//{
			bad_cnt++;
			//}
		}
	}
	if(topo_cnt>0)
	{
		printf("%ld topo normal\n",topo_cnt);
	}
	if(basic_cnt>0)
	{
		printf("%ld basic normal\n",basic_cnt);
	}
	if(fb_cnt>0)
	{
		printf("%ld fallback vertex normal used\n",fb_cnt);
	}
	if(bad_cnt>0)
	{
		printf("%ld bad normal\n",bad_cnt);
	}
}

void tri_rec(vector<int> &vec,int begin, int end,vector< vector<int> > &res) //break a face into triangles, rec way, not used
{
	int size=end-begin;
	assert(size>=3);
	if(size==3)
	{
		vector<int> tmp;
		tmp.push_back(vec[begin]);
		tmp.push_back(vec[begin+1]);
		tmp.push_back(vec[begin+2]);
		res.push_back(tmp);
		return ;
	}
	int mid=begin+(end-begin)/2;
	tri_rec(vec,begin,mid+1,res);
	int tmp=vec[mid-1];
	vec[mid-1]=vec[begin];
	tri_rec(vec,mid-1,end,res);
	vec[mid-1]=tmp;
}
void tri(vector<int> &vec,vector< vector<int> > &res) //a very simple algorithm to break a face into triangles
{
	assert(vec.size()>=3);
	for(int i=0;i<vec.size()-2;i++)
	{
		vector<int> tmp;
		tmp.push_back(vec[0]);
		tmp.push_back(vec[i+1]);
		tmp.push_back(vec[i+2]);
		res.push_back(tmp);
	}
}
struct a_t //unit test of tri function
{
	a_t()
	{
		vector< vector<int> > res;
		vector<int> test={1,2,3,4,5,6,7,8,9};
		tri(test,res);
		for(int i=0;i<res.size();i++)
		{
				assert(res[i].size()==3);
				printf("<%d,%d,%d>\n",res[i][0],res[i][1],res[i][2]);
		}
	}
};
model_t model_t::tri_new() //break faces into triangles and gen a new model
{
	
	model_t new_model= (*this);
	new_model.faces.clear();
	for(int i=0;i<faces.size();i++)
	{
		vector< vector<int> > res;
		vector<int> tmp=faces[i].v;
		tri(tmp,res);
		for(int j=0;j<res.size();j++)
		{
			face_t face=faces[i];
			face.v=res[j];
			vertex_t a,b,c,ab,ac;
			a=vertexs[res[j][0]];
			b=vertexs[res[j][1]];
			c=vertexs[res[j][2]];
			ab=b.minus_new(a);
			ac=c.minus_new(a);
			vertex_t cross=ab.cross_new(ac);
			if(eq(cross.dot(cross),0))	continue;
			new_model.faces.push_back(face);
		}
	}
	return new_model;
}
