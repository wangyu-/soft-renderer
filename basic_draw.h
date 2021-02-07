#include "comm.h"

extern RgbColor g_color,g_black,g_white;// g_color indictes the current color for drawing
void line_dda( int x1,int y1,int x2, int y2 );  //draw a line using dda
void line( int x1,int y1,int x2, int y2 );  //draw a line using midpoint
void line_new(double x1,double y1,double x2,double y2);// line drawing function which rounds double to integer
void write_pixel_rgb(int x,int y,RgbColor c); // write a pixel using the given RGB color c
void write_pixel_rgb_d(int x,int y,rgb_t c); // write a pixel using the given RGB color c
extern double z_buf[MAX_HEIGHT][MAX_WIDTH];
void init_z_buf();   // init z-buffer

void z_write(int x,int y,double z,RgbColor c);  //write with z-buffer
void scan_line(int y, int x1,int x2,double z1,double z2,RgbColor c);  //a scanline drawing function, will be used in filling a polyon
