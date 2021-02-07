#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

#include "comm.h"
#include "vertex.h"
#include "basic_draw.h"
#include "call_back.h"
#include "model.h"
#include "canvas.h"
#include "ray_tracing.h"
/***************************************************************************/

int main (int argc, char *argv[])
{
	if(argc<2)
	{
		printf("use:\n");
		printf("./this_program model1.obj\n");
		printf("./this_program model1.obj 800x600\n");
		return -1;
	}
	if(argc>=3)
	{
		int w=-1,h=-1;
		sscanf(argv[2],"%dx%d",&w,&h);
		if(w==-1 or h==-1)
		{
			printf("paremeter %s incorrect\n",argv[2]);
			return -1;
		}
		WIDTH=w;
		HEIGHT=h;
	}

	if(argc>=4)
	{
		sscanf(argv[3],"%d,%d,%d",&light_x,&light_y,&light_z);
	}

	LEFT=-(WIDTH/2-1);
	RIGHT=WIDTH/2;
	BOTTOM=-(HEIGHT/2-1);
	UP=HEIGHT/2;
        					      
	init_window(WIDTH,HEIGHT,argv[1]);    //create_window
	clear_canvas();
	set_display_cb(display);
	set_keyboard_cb(keyboard);
  
	g_color=g_white;

	model=read_model(argv[1]);  //read model
	add_light(light_x,light_y,light_z);
	
	//model.prt();
	//printf("<1>\n");
	//model=model.tri_new();
	//model.prt();//print details of model
	model.normal_inplace(); //normalize model so that w is equal to 1
	vertex_t center=model.get_center();  //get center of model
	model.translate(-center.x(),-center.y(),-center.z());// move model to center
	double max_radius=model.get_max_radius()*resize;	
	model.scale(1.*HEIGHT/2/max_radius,1.*HEIGHT/2/max_radius,1.*HEIGHT/2/max_radius);  //scale the model so that it fits into the screen

	printf("use 'wsad' and 'q' 'e' to rotate, use arrows and '[' ']' to move, use '-' '+' to scale \n");
	printf("use 'r' for toggling ray tracing, 'p' for pong smothing, 'b for bump mapping'\n");
	printf("use 'v' to toggle between orthogonal/perspective view, 'z' to toggle between wireframe/z-buffer mode, \n");
	printf("use SPACE to switch between 4 bump mapping patterns\n");

	flush_canvas();

	begin_loop();
}


