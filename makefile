all:
	g++ --std=c++11 -o render main.cpp canvas.cpp basic_draw.cpp ray_tracing.cpp model.cpp read_model.cpp call_back.cpp comm.cpp -I. -lX11 -ggdb -O2 -ggdb 
clean:
	rm -f render
