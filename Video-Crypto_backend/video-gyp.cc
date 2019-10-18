#include <node.h>
#include <mpeg.cpp>

namespace Video {

  using namespace v8;

  static void HelloWorld(const FunctionCallbackInfo<Value>& args) {
    // isolate当前的V8执行环境，每个isolate执行环境相互独立
    Isolate* isolate = args.GetIsolate();
    // 设定返回值
    Mpeg mpeg = Mpeg();
    // std::string infile = args[0].As<String>().Utf8Value();
    // std::string outfile = args[1].As<String>().Utf8Value();
    // mpeg.getWaterMark("/Users/kevin/Desktop/ffmpegDemo/cmake-build-debug/test.mp4");
    mpeg.encryptFrame("/Users/kevin/Desktop/ffmpegDemo/cmake-build-debug/test.mp4", "/Users/kevin/Desktop/ffmpegDemo/cmake-build-debug/test1.mp4");
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, "Hello, World!"));
  }

  static void init(Local<Object> exports, Local<Object> module) {
    // 设定module.exports为HelloWorld函数
    NODE_SET_METHOD(module, "exports", HelloWorld);
  }
  // 所有的 Node.js 插件必须以以下形式模式的初始化函数
  NODE_MODULE(NODE_GYP_MODULE_NAME, init)

}

