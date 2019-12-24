CC = g++

V8_PATH=/home/mesibo/v8/v8
V8_LIBPATH=$(V8_PATH)/out.gn/x64.release.sample/obj/
CFLAGS       = -g -Wall
INCLUDES = -I$(V8_PATH) -I$(V8_PATH)/include -I$(V8_PATH)/include/libplatform  -Iinclude
LDFLAGS      = -lv8_monolith -L$(V8_LIBPATH) -pthread

RM = rm -f

SRC = $(wildcard *.cpp)

TARGET  = mesibo_v8_standalone 

all: $(TARGET)

clean: 
	$(RM) $(TARGET)

$(TARGET): $(SRC) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SRC) $(LDFLAGS) 

