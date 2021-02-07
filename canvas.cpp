#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>


XImage *image; 
Display *display;

Window newWin;
GC graphicsContext; 

void (*display_cb) ();
void (*keyboard_cb) (int );

void set_display_cb(void (*f)())
{
	display_cb=f;
}

void set_keyboard_cb(void (*f)(int ))
{
	keyboard_cb=f;
}

void draw_pixel0(int x,int y,unsigned char r,unsigned char g,unsigned char b)
{
	int color=0;
	color=r;
	color<<=8;
	color+=g;
	color<<=8;
	color+=b;
	XPutPixel(image, x, y, color);
}
void draw_pixel(int x,int y,unsigned char r,unsigned char g,unsigned char b)
{
	x+=image->width/2;
	y+=image->height/2;
	y=image->height-y;
	if(x<0||y<0) return;
	if(x>=image->width||y>=image->height) return;
	draw_pixel0(x,y,r,g,b);

	//printf("<%d,%d,%d,%d,%d>\n",x,y,int(r),int(g),int(b));
}

void clear_canvas()
{
	int pixel=0;
	memset(image->data,0,sizeof(int)*image->width*image->height);
	/*
	for (int x = 0; x < image->width; x++)
	{
		for (int y = 0; y < image->height; y++)
		{
			draw_pixel0(x,y,0,0,0);
		}
	}*/

}

void flush_canvas()
{
	XPutImage(display, newWin, graphicsContext, image, 0, 0, 0, 0, image->width, image->height);
}


void init_window(int width,int height,char * name)
{
	display = XOpenDisplay(0);
	if(!display)
	{
		printf("failed to open display 0\n");
		exit(-1);
	}
	int screen = DefaultScreen(display);
	graphicsContext = DefaultGC(display, screen);
	Window rootWin = RootWindow(display, screen);

	// Create new window and subscribe to events
	long blackPixel = BlackPixel(display, screen);
	long whitePixel = WhitePixel(display, screen);
	newWin = XCreateSimpleWindow(display, rootWin, 0, 0, width, height, 1, blackPixel, whitePixel);
	XMapWindow(display, newWin);
	XSelectInput(display, newWin, KeyPressMask);
	XStoreName(display, newWin, name);

	Visual *visual = DefaultVisual(display,0);

	char *data = (char*)malloc(width*height*sizeof(int));

	image = XCreateImage(display,visual,DefaultDepth(display,0),ZPixmap,
			0,data,width,height,32,0);
	//XAutoRepeatOff(display);

	// Main event loop for new window
	//XImage *image;
}
long long get_current_time_ms()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	//printf("second:%ld\n",tv.tv_sec);  //秒
	//printf("millisecond:%ld\n",tv.tv_sec*1000 + tv.tv_usec/1000);  //毫秒
	return tv.tv_sec*1000 + tv.tv_usec/1000;
}
void begin_loop()
{
	XEvent event;

	Atom WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(display, newWin, &WM_DELETE_WINDOW, 1);
	
	display_cb();
	long long last_time=0;
	while(1)
	{
		//printf("------------\n");
		do
		{
			XNextEvent(display, &event);
			if (event.type == KeyPress)
			{
				long long new_time=get_current_time_ms();
				//printf("%lld %lld\n",last_time,new_time);
				//if(new_time-last_time<30) continue;
				int a=event.xkey.keycode;
				char buff[8];
				KeySym symLS;
				KeySym sym=XLookupKeysym(&event.xkey, 0);

				keyboard_cb((int)(sym));
				last_time=new_time;
			}
			else if (event.xclient.data.l[0] == WM_DELETE_WINDOW)
			{
				XCloseDisplay(display);
				goto end;
			}
		}
		while (XPending(display) > 0);
		/*
		else if (event.type == Expose)
		{
			printf("expose\n");
		} else if (event.type == NoExpose)
		{
			printf("no expose\n");
		}*/
		display_cb();
	}
end:;
}
