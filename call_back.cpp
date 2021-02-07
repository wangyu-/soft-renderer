#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

#include "comm.h"
#include "model.h"
#include "canvas.h"

void keyboard ( int key)  // handle keyboard operation
{
	switch ( key ) {
		case 27:              // When Escape Is Pressed...
			exit ( 0 );   // Exit The Program
			break;        
		case '8':
			topo+=1;
			topo%=2;
			if(topo==1)
				printf("topo==1\n");
			else
				printf("topo==0\n");
			break;
		case ' ':
			bump_type+=1;
			bump_type%=bump_type_num;
			printf("bump_type==%d\n",bump_type);
			break;
		case 't':
			real_time+=1;
			real_time%=2;
			if(real_time==1)
				printf("real_time==1\n");
			else
				printf("real_time==0\n");
			break;
		case 'v':
			per+=1;
			per%=2;
			if(per==1)
				printf("now in perspective view\n");
			else
				printf("now in normal view\n");
			break;
		case 'p':
			phong_smoothing+=1;
			phong_smoothing%=2;
			if(phong_smoothing==1)
				printf("now in phong_smoothing\n");
			else
				printf("now in non-phong_smoothing\n");
			break;
		case 'b':
			bump+=1;
			bump%=2;
			if(bump==1)
				printf("now in bump\n");
			else
				printf("now in non-bump\n");
			break;
		case 'z':
			wire+=1;
			wire%=2;
			if(wire==1)
				printf("now in wireframe mode\n");
			else
				printf("now in z-buffer mode\n");
			break;
		case 'r':
			ray_tracing+=1;
			ray_tracing%=2;
			if(ray_tracing==1)
				printf("now in ray_tracing\n");
			else
				printf("now in non-tray_tracing\n");
			break;
		case 0:
			D+=20; 
			if(D>-20) D=-20;
			break;
		case 1:
			D-=20; 
		case '=': 
		case '+': 
			g_scale*=1.1;
			break;
		case '-': 
			g_scale*=0.9;
			break;
		case '[':
		       t_z-=10;	
		       break;
		case ']':
		       t_z+=10;	
		       break;
		case 'q': 
			model.rotate_z(r_speed);
                        break;
		case 'e':
			model.rotate_z(-r_speed);
                        break;
		case 'w':
			model.rotate_x(-r_speed);
                        break;
		case 's':
			model.rotate_x(r_speed);
                        break;
		case 'a':
			model.rotate_y(-r_speed);
                        break;
		case 'd':
			model.rotate_y(r_speed);
                        break;
		case 0xff51:  //left arrow
			t_x-=10;
			break;
		case 0xff53:  //right arrow
			t_x+=10;
			break;
		case 0xff54:  //down
			t_y-=10;
			break;
		case 0xff52: // up
			t_y+=10;
			break;
		case 0xffe3:
		case 0xff1b:
			exit(0);
		default:      
			break;
	}
	need_refresh=1;
	//if(key!='v'&&key!='z'&&key!='e'&&key!='r'&&key!='t')
	printf("key <%c>(%x) pressed, object is now at <%.1f,%.1f,%.1f>, scale=%.3f, projection and clipping plane is at z=%.2f\n",key,key,t_x,t_y,t_z,g_scale,D);	
}

void display ( )   // display the model, only when its necessary
{
	//if(!need_refresh) {usleep(1000*5);return ;}
	//printf("display called\n");
	//need_refresh=0;
	model_t new_model=model;
	new_model.scale(g_scale,g_scale,g_scale);  // scale the mode
	if(per==1)
		new_model.scale(1.8,1.8,1.8);   //scale more if we are in perspective mode
	new_model.translate(t_x,t_y,t_z); //translate themode	
	/*
	for(int i=0;i<new_model.vertexs.size();i++)
	{
		auto &t=new_model.vertexs[i];
		printf("%f %f %f\n",t.x(),t.y(),t.z());
	}*/
	clear_canvas();
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen
	if(ray_tracing==1)
	{
		//new_model=new_model.clip_new();
		new_model=new_model.tri_new();
		new_model.draw_rt();
	}
	else if(per==0)
	{
		new_model.draw_orth();  //handle perspective mode drawing
	}
	else 
	{
		new_model.draw_per(); //handle non-perspective mode drawing
	}
	flush_canvas();
	
	//glutSwapBuffers();         

}
