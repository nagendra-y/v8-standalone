all:
	g++ -g -I/home/mesibo/v8/v8 -I/home/mesibo/v8/v8/include/ -Iincude -I/home/mesibo/v8/v8/include/libplatform processor.cpp test-processor.cpp -o out_mesibo_v8_standalone -lv8_monolith -L/home/mesibo/v8/v8/out.gn/x64.release.sample/obj/ -pthread
