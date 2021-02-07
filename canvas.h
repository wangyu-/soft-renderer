void draw_pixel(int x,int y,unsigned char r,unsigned char g,unsigned char b);
void clear_canvas();
void flush_canvas();
void init_window(int width,int height,char *name);
void begin_loop();
void set_display_cb(void (*f)());
void set_keyboard_cb(void (*f)(int));

