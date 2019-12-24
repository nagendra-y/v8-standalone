
#include "include/mesibo_js_processor.h"
#include <iostream>
#include <map>
#include <v8.h>

using std::map;

void ParseOptions(int argc,
                  char* argv[],
                  map<string, string>* options,
                  string* file) {
  for (int i = 1; i < argc; i++) {
    string arg = argv[i];
    size_t index = arg.find('=', 0);
    if (index == string::npos) {
      *file = arg;
    } else {
      string key = arg.substr(0, index);
      string value = arg.substr(index+1);
      (*options)[key] = value;
    }
  }
}

int main (int argc, char* argv[]){
	std::cout << v8::V8::GetVersion() << std::endl;
	
	std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
	v8::V8::InitializePlatform(platform.get());
	v8::V8::Initialize();
	
	string file;
	map<string, string> options;
	ParseOptions(argc, argv, &options, &file);
	if (file.empty()) {
		fprintf(stderr, "No script was specified.\n");
		return MESIBO_RESULT_FAIL;
	}

	v8::Isolate::CreateParams createParams;
	createParams.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
	v8::Isolate* isolate = v8::Isolate::New(createParams);

	const char* script_path = file.c_str(); 
	int log_level = 0;
	
	mesibo_module_t* mod = NULL;
	MesiboJsProcessor* mesibo_js = new MesiboJsProcessor(mod, isolate, script_path, log_level);

	mesibo_js->Initialize();

	return MESIBO_RESULT_OK;
}

