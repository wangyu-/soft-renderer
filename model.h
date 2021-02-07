#pragma once

#include "vertex.h"

vector<vertex_t> clip_vec(vector<vertex_t> &input,double d);  //clip a polygon, z=d.  any part of the polygon is clipped if z>d

struct model_t  //the type of model
{	
	vector<vertex_t> vertexs; //vertexs(vertice) of model
	vector<vector<int> > inv_idx;
	vector<vertex_t> v_norm_cache;
	vector<face_t> faces; //faces of model
	void prt();
	void normal_inplace();  //noraml all vertexs so that w value is equal to 1
	vertex_t get_center();  //get center of model
//call normalize before using this
	void mul_inplace(mat_t &m);//multiply the model by a matrix
	void translate(double xx,double yy,double zz);  //translate the model
	double get_max_abs(); // get the max absolute value of the model
	double get_max_radius(); //get the max radius of the model
	void scale(double xx,double yy,double zz); //scale the model
	void rotate_z(double theta); //rotate around z
	void rotate_x(double theta); //rotate around x
	void rotate_y(double theta); //rotate around y
	void draw_wire();  //draw in wireframe mode
	void draw_face_fill(vector<vertex_t> &vec,RgbColor c);  //draw a polygon in z-buffer mode
	void draw_fill(); //draw the model in z-buffer mode
	void draw_rt(); //draw the model with ray-tracing 
	void draw_comm(); //common function for drawing in non-perspective and perspective
	void draw_orth(); //draw in non-perspective mode
	void draw_per(); //draw in perspective mode
	void gen_cache(); //gen caches of models such as vertex_normal
	model_t clip_new();  //clip the model and generate a new model
	model_t tri_new(); //turn model into triangles and generate a new model
};

model_t read_model(string file_name);// read model
extern model_t model;
