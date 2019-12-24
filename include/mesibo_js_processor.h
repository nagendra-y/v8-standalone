#pragma once

#include "module.h"

#include <string.h>
#include <iostream>
#include <v8.h>
#include <include/libplatform/libplatform.h>

using std::pair;
using std::string;
using std::cout;
using std::endl;

using v8::Context;
using v8::EscapableHandleScope;
using v8::External;
using v8::Function;
using v8::FunctionTemplate;
using v8::Global;
using v8::HandleScope;
using v8::Isolate;
using v8::Local;
using v8::MaybeLocal;
using v8::Name;
using v8::NamedPropertyHandlerConfiguration;
using v8::NewStringType;
using v8::Object;
using v8::ObjectTemplate;
using v8::PropertyCallbackInfo;
using v8::Script;
using v8::String;
using v8::TryCatch;
using v8::Value;


#define HTTP_BUFFER_LEN (64 * 1024)
#define MODULE_LOG_LEVEL_0VERRIDE 0

/** Message Parmeters **/
#define MESSAGE_AID 				"aid"
#define MESSAGE_ID 				"id"
#define MESSAGE_REFID 				"refid"
#define MESSAGE_GROUPID 			"groupid"
#define MESSAGE_FLAGS 				"flags"
#define MESSAGE_TYPE 				"type"
#define MESSAGE_EXPIRY 				"expiry"
#define MESSAGE_TO_ONLINE 			"to_online"
#define MESSAGE_TO 				"to"
#define MESSAGE_FROM 				"from"

/** HTTP options **/
#define HTTP_PROXY 				"proxy"
#define HTTP_CONTENT_TYPE 			"content_type"
#define HTTP_EXTRA_HEADER 			"extra_header"
#define HTTP_USER_AGENT 			"user_agent"
#define HTTP_REFERRER 				"referrer"
#define HTTP_ORIGIN 				"origin"
#define HTTP_COOKIE 				"cookie"
#define HTTP_ENCODING 				"encoding"
#define HTTP_CACHE_CONTROL 			"cache_control"
#define HTTP_ACCEPT 				"accept"
#define HTTP_ETAG 				"etag"
#define HTTP_IMS 				"ims"
#define HTTP_MAXREDIRECTS 			"maxredirects"
#define HTTP_CONN_TIMEOUT 			"conn_timeout"
#define HTTP_HEADER_TIMEOUT 			"header_timeout"
#define HTTP_BODY_TIMEOUT 			"body_timeout"
#define HTTP_TOTAL_TIMEOUT 			"total_timeout"
#define HTTP_RETRIES 				"retries"
#define HTTP_SYNCHRONOUS 			"synchronous"

/** Listener functions **/
#define MESIBO_LISTENER_ON_MESSAGE 		"Mesibo_onMessage"
#define MESIBO_LISTENER_ON_MESSAGE_STATUS 	"Mesibo_onMessageStatus"
#define MESIBO_LISTENER_ON_HTTP_RESPONSE 	"Mesibo_onHttpResponse"

/** Mesibo module Helper functions **/
#define MESIBO_MODULE_MESSAGE                   "mesibo_message"
#define MESIBO_MODULE_HTTP                      "mesibo_http"
#define MESIBO_MODULE_LOG 			"mesibo_log"  

typedef struct v8_config_s v8_config_t;
typedef struct http_context_s http_context_t;

class MesiboJsProcessor {
	public:
		MesiboJsProcessor(mesibo_module_t* mod, Isolate* isolate, const char* script, int log_level)
			:mod_(mod), isolate_(isolate), script_(script), log_(log_level), lastchanged_(0) {}
		virtual int Initialize();
		virtual ~MesiboJsProcessor() { };
		int ExecuteJsFunction(Local<Context>& context,
				const char* func_name, int argc, Local<Value> argv[]);

		//Callbacks to Javascript
		mesibo_int_t OnMessage(mesibo_message_params_t p, const char* message,
				mesibo_uint_t len);
		mesibo_int_t OnMessageStatus(mesibo_message_params_t p, mesibo_uint_t status);

		//Callables from Javscript
		static void LogCallback(const v8::FunctionCallbackInfo<v8::Value>& args); 
		static void MessageCallback(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void HttpCallback(const v8::FunctionCallbackInfo<v8::Value>& args);

		void SetCallables(Local<ObjectTemplate> & global);
		
		//Debug-Interface	
		static int Log(const char* format, ...);

		//Module Configuration
		mesibo_module_t* mod_;
		const char* script_;//The /full/path/to/script
		int log_; //log-level

		//V8 Isolate & Context
		Isolate* GetIsolate() { return isolate_; }
		Local<Context> GetContext(); //Will return allocated context from base context 

	private:
		int ExecuteScript(Local<String> script, Local<ObjectTemplate> global);
		MaybeLocal<String> ReadFile(Isolate* isolate, const string& name);

		Isolate* isolate_;
		Global<Context> context_; //Load base context from here 
		int lastchanged_;

		
		//Utility methods for wrapping C++ objects as JavaScript objects,
		// and going back again.

		/*Messaging*/
		Local<Object> WrapMessageParams(Local<Context>& context , 
				mesibo_message_params_t* p);
		static mesibo_message_params_t UnwrapMessageParams(Isolate* isolate, 
				Local<Context> context, Local<Value> arg_params);

		/*Http*/
		static mesibo_http_option_t* UnwrapHttpOptions(Isolate* isolate, 
				Local<Context> context, Local<Value> arg_options);
		static http_context_t* BundleHttpCallback(Isolate* isolate, 
				Local<Context> context, Local<Value> js_cb, 
				Local<Value> js_cbdata, mesibo_module_t* mod);	

		int WrapParamUint(Local<Context>& context, Local<Object> &js_params, 
				const char* key, mesibo_uint_t value);
		static mesibo_uint_t UnwrapParamUint(Isolate* isolate, 
				Local<Context> context, const char* param_name, 
				Local<Object>params);

		int WrapParamString(Local<Context>& context, Local<Object> &js_params, 
				const char* key, const char* value);
		static std::string UnwrapParamString(Isolate* isolate, 
				Local<Context> context, const char* param_name,
				Local<Object>params);

		static Local<Value> GetParamValue(Isolate* isolate, 
				Local<Context> context, const char* param_name,
				Local<Object>params);


		static mesibo_uint_t UnwrapUint(Isolate* isolate,
				Local<Context>context, Local<Value>integer); 

		static std::string UnwrapString(Isolate* isolate, 
				Local<Context> context, Local<Value> byte_string);


};

/**
 * Sample V8 Module Configuration
 * Refer sample.conf
 */
struct v8_config_s{
	const char* script;
	int log; //log level
	long last_changed; // Time Stamp
	MesiboJsProcessor* ctx; // v8 context
	std::unique_ptr<v8::Platform> platform; //v8 Platform
};

struct http_context_s {
	char buffer[HTTP_BUFFER_LEN];
	int datalen;	
	MesiboJsProcessor* ctx_;
	mesibo_module_t* mod;

	Isolate* isolate;
	Local<Context> context;

	Local<Value> http_cb; //Js HTTP Callback function
	Local<Value> http_cbdata; //Js HTTP Callback Data 
};

