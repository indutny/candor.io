#include "bindings/fs.h"

#include <stdlib.h> // NULL
#include <string.h> // memcpy
#include <fcntl.h> // O_RDONLY and everything
#include <candor.h>
#include <uv.h>

namespace can {

using namespace candor;

const int FSWrap::magic = 0;

FSWrap::FSWrap(Function* cb) : CWrapper(&magic), cb_(cb) {
  req_ = new uv_fs_t();
  req_->data = this;
  Ref();
}


FSWrap::~FSWrap() {
  cb_.Unref();
  if (req_ == NULL) return;
  delete req_;
}


void FSWrap::HandleCallback(uv_fs_t* req) {
  FSWrap* wrap = reinterpret_cast<FSWrap*>(req->data);

  Value* argv[2] = { Nil::New(), Number::NewIntegral(req->result) };
  wrap->cb_->Call(2, argv);
  wrap->cb_.Unref();
  wrap->Unref();
}


void FSWrap::HandleClose(uv_handle_t* handle) {
  delete handle;
}


char* StringToChar(Value* value) {
  String* str = value->As<String>();

  char* res = new char[str->Length() + 1];
  memcpy(res, str->Value(), str->Length());
  res[str->Length()] = 0;

  return res;
}


Value* FS::Open(uint32_t argc, Value** argv) {
  // Input constraints
  if (argc < 4 ||
      !argv[0]->Is<String>() ||
      !argv[1]->Is<Number>() ||
      !argv[2]->Is<Number>() ||
      !argv[3]->Is<Function>()) {
    return Nil::New();
  }

  Function* cb = argv[3]->As<Function>();
  FSWrap* wrap = new FSWrap(cb);

  char* path = StringToChar(argv[0]);
  int r = uv_fs_open(uv_default_loop(),
                     wrap->req(),
                     path,
                     argv[1]->As<Number>()->IntegralValue(),
                     argv[2]->As<Number>()->IntegralValue(),
                     FSWrap::HandleCallback);
  delete[] path;

  if (r) {
    Value* argv[2] = { String::New("Request failed"), Number::NewIntegral(r) };
    cb->Call(2, argv);
  }

  return Nil::New();
}


Value* FS::OpenSync(uint32_t argc, Value** argv) {
  // Input constraints
  if (argc < 3 ||
      !argv[0]->Is<String>() ||
      !argv[1]->Is<Number>() ||
      !argv[2]->Is<Number>()) {
    return Nil::New();
  }

  uv_fs_t req;
  char* path = StringToChar(argv[0]);
  int r = uv_fs_open(uv_default_loop(),
                     &req,
                     path,
                     argv[1]->As<Number>()->IntegralValue(),
                     argv[2]->As<Number>()->IntegralValue(),
                     NULL);
  delete[] path;

  return Number::NewIntegral(r);
}


Value* FS::Close(uint32_t argc, Value** argv) {
  // Input constraints
  if (argc < 2 || !argv[0]->Is<Number>() || !argv[1]->Is<Function>()) {
    return Nil::New();
  }

  Function* cb = argv[1]->As<Function>();
  FSWrap* wrap = new FSWrap(cb);

  int r = uv_fs_close(uv_default_loop(),
                      wrap->req(),
                      argv[0]->As<Number>()->IntegralValue(),
                      FSWrap::HandleCallback);

  if (r) {
    Value* argv[2] = { String::New("Request failed"), Number::NewIntegral(r) };
    cb->Call(2, argv);
  }

  return Nil::New();
}


Value* FS::CloseSync(uint32_t argc, Value** argv) {
  // Input constraints
  if (argc < 1 || !argv[0]->Is<Number>()) {
    return Nil::New();
  }

  uv_fs_t req;
  int r = uv_fs_close(uv_default_loop(),
                      &req,
                      argv[0]->As<Number>()->IntegralValue(),
                      NULL);

  return Number::NewIntegral(r);
}


Value* FS::Read(uint32_t argc, Value** argv) {
  return Nil::New();
}


void FS::Init(Object* target) {
  // Exports flags
  Object* flags = Object::New();
  flags->Set("O_RDONLY", Number::NewIntegral(O_RDONLY));
  flags->Set("O_WRONLY", Number::NewIntegral(O_WRONLY));
  flags->Set("O_CREAT", Number::NewIntegral(O_CREAT));
  flags->Set("O_APPEND", Number::NewIntegral(O_APPEND));
  flags->Set("O_RDWR", Number::NewIntegral(O_RDWR));
  target->Set("flags", flags);

  // Exports functions
  target->Set("open", Function::New(Open));
  target->Set("openSync", Function::New(OpenSync));
  target->Set("close", Function::New(Close));
  target->Set("closeSync", Function::New(CloseSync));
  target->Set("read", Function::New(Read));
}

} // namespace can
