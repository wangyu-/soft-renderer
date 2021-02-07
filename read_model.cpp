#include "comm.h"
#include "basic_draw.h"
#include "vertex.h"
#include "model.h"
int get_v_idx(string a)  //helper function to parse faces
{
	vector<string> vec;
	vec=string_to_vec(a.c_str(),"/");
	assert(vec.size()>=1);
	//printf("<%s>\n",vec[0].c_str());
	int idx=stoi(vec[0]);	
	assert(idx>=1);
	return idx-1;
}
model_t parse_vec2(vector<vector<string> > vec2) //conver a vector<vector<sring>> into a model
{
	model_t model;
	int i,j,k;
	for(int i=0;i<vec2.size();i++)
	{
		if(vec2[i].empty()) 
		{
			//printf("!!!\n");
			continue;
		}
		//for(j=0;j<vec2[i].size();j++)
		//	printf("<%s>",vec2[i][j].c_str());
		//printf("\n");
		if(vec2[i][0]=="v") //parse vertexs
		{
			vertex_t v;
			assert(vec2[i].size()==4||vec2[i].size()==5);

			v.x()=stod(vec2[i][1]);
			v.y()=stod(vec2[i][2]);
			v.z()=stod(vec2[i][3]);
			//printf("%f %f %f\n",v.x(),v.y(),v.z());
			if(vec2[i].size()==5)
			{
				v.w()=stod(vec2[i][4]);
			}
			v.normal_inplace();

			model.vertexs.push_back(v);
		}
		else if(vec2[i][0]=="f")// parse faces
		{
			face_t face;
			assert(vec2[i].size()>=4);
			for(int j=1;j<vec2[i].size();j++)
			{
				face.v.push_back(get_v_idx(vec2[i][j]));
			}	
			model.faces.push_back(face);
		}
	}
	//printf("<%d>\n",model.vertexs.size());
	return model;
}
model_t read_model(string file_name)// read a file and convert it into model
{
	string file_content;
	assert(read_file(file_name.c_str(),file_content)==0);
	vector<vector<string> > vec2 =string_to_vec2(file_content.c_str());
	//prt_vec2(vec2);
	return parse_vec2(vec2);
}
