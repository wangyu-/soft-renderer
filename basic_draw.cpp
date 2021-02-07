#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

#include "comm.h"
#include "vertex.h"
#include "basic_draw.h"
#include "canvas.h"


RgbColor g_color,g_black={.r=0,.g=0,.b=0},g_white={.r=255,.g=255,.b=255};// g_color indictes the current color for drawing

void write_pixel_rgb_d(int x,int y,rgb_t c) // write a pixel using the given RGB color c
{
	draw_pixel(x,y,c.r()*255,c.g()*255,c.b()*255);
}
void write_pixel_rgb(int x,int y,RgbColor c) // write a pixel using the given RGB color c
{
	draw_pixel(x,y,c.r,c.g,c.b);
}
void write_pixel(int x, int y, double intensity)//write a pixel using the current color "g_color"
{	
	draw_pixel(x,y,g_color.r*intensity,g_color.g*intensity,g_color.b*intensity);
}

/***************************************************************************/
void line_dda( int x1,int y1,int x2, int y2 )  //draw a line using dda
{
	int inv_xy=0;
	if(x2-x1+y2-y1<0)
	{
		swap(x1,x2);
		swap(y1,y2);
	}	
	if(my_abs(y2-y1)>my_abs(x2-x1))
	{
		swap(x1,y1);
		swap(x2,y2);
		inv_xy=1;
	}
	int x=x1;
	double y=y1;
	double y_inc=(y2-y1)*1.0/(x2-x1);
        while(x<=x2)
        {
		if(!inv_xy)
                	write_pixel(x,y,1.0);
		else
			write_pixel(y,x,1.0);
		x++;
		y+=y_inc;
        }
}
void line( int x1,int y1,int x2, int y2 )  //draw a line using midpoint
{
	//printf("<%d,%d,%d,%d>\n",x1,y1,x2,y2);
	int inv_xy=0;
	if(x1>x2)
	{
		swap(x1,x2);
		swap(y1,y2);
	}
	if(x2-x1+y2-y1<0)
	{
		swap(x1,x2);
		swap(y1,y2);
	}	
	if(my_abs(y2-y1)>my_abs(x2-x1))
	{
		swap(x1,y1);
		swap(x2,y2);
		inv_xy=1;
	}
        int x=x1,y=y1;
        int a=y2-y1;int b=x1-x2;
        int dd=2*a+b;
	int y_inc=1;
	if(a<0)
	{
		a=-a;
		y_inc=-y_inc;
	}
        while(x<=x2)
        {
		if(!inv_xy)
                	write_pixel(x,y,1.0);
		else
			write_pixel(y,x,1.0);
                if(dd<0)
                {
                        x=x+1;
                        dd=dd+2*a;
                }
                else
                {
                        x=x+1;
                        y=y+y_inc;
                        dd=dd+2*a+2*b;
                }
        }
}
void line_new(double x1,double y1,double x2,double y2)// line drawing function which rounds double to integer
{
	//printf("<%.2f,%.2f,%.2f,%.2f>\n",x1,y1,x2,y2);
	line(my_round(x1),my_round(y1),my_round(x2),my_round(y2));
}
double z_buf[MAX_HEIGHT][MAX_WIDTH]={0};   //data structure of of z-buffer
void init_z_buf()   // init z-buffer
{
	for(int i=0;i<HEIGHT;i++)
	
		for(int j=0;j<WIDTH;j++)
		{
			z_buf[i][j]=-1e10;
		}
}

void z_write(int x,int y,double z,RgbColor c)  //write with z-buffer
{
	if(x<LEFT||x>RIGHT||y<BOTTOM||y>UP) return;

	if(z>z_buf[y-BOTTOM][x-LEFT])
	{
		z_buf[y-BOTTOM][x-LEFT]=z;
		write_pixel_rgb(x,y,c);
	}	
}
void scan_line(int y, int x1,int x2,double z1,double z2,RgbColor c)  //a scanline drawing function, will be used in filling a polyon
{
	//printf("<scanline:%d %d>\n",x1,x2);
	if(x1>x2)
	{
		int tmp=x1;
		x1=x2;
		x2=tmp;
		double tmp2=z1;
		z1=z2;
		z2=tmp;
	}
	double z;
	if(x1==x2)
	{
		z=max(z1,z2);
		z_write(x1,y,z,c);
		return;
	}
	else
	{
		double z_inc=(z2-z1)/(x2-x1);
		double z=z1;
		if(x1<LEFT) x1=LEFT;
		if(x2>RIGHT) x2=RIGHT;
		
		for(int x=x1;x<=x2;x++,z+=z_inc)
		{
			z_write(x,y,z,c);
		}
		
		//z_write(x1,y,z1,c);
		//z_write(x2,y,z2,c);
	}
}
