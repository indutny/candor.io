#include "bindings/fs.h"
#include "bindings/buffer.h" // Buffer

#include <stdlib.h> // NULL
#include <string.h> // memcpy
#include <fcntl.h> // O_RDONLY and everything
#include <assert.h>

namespace can {

using namespace candor;

const int FSWrap::magic = 0;

#define SYNC_CALL(method, ...) \
    uv_fs_t _req; \
    _req.result = uv_fs_##method(uv_default_loop(), \
                                 &_req, \
                                 __VA_ARGS__, \
                                 NULL);

#define SYNC_REQ (&_req)

#define ASYNC_CALL(method, cb, ...) \
    FSWrap* wrap = new FSWrap(cb); \
    int r = uv_fs_##method(uv_default_loop(), \
                           wrap->req(), \
                           __VA_ARGS__, \
                           FSWrap::HandleCallback); \
    if (r) { \
      Value* argv[2] = { \
        String::New("FS request failed"), \
        Number::NewIntegral(r) \
      }; \
      cb->Call(2, argv); \
    }

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


Object* FSWrap::GetStatObject(uv_fs_t* req) {
  Object* res = Object::New();

  assert(req->fs_type == UV_FS_STAT);
  uv_statbuf_t* s = reinterpret_cast<uv_statbuf_t*>(req->ptr);

  res->Set("uid", Number::NewIntegral(s->st_uid));
  res->Set("gid", Number::NewIntegral(s->st_gid));
  res->Set("size", Number::NewIntegral(s->st_size));
  res->Set("atime", Number::NewIntegral(s->st_atime));
  res->Set("mtime", Number::NewIntegral(s->st_mtime));
  res->Set("ctime", Number::NewIntegral(s->st_ctime));

  return res;
}


void FSWrap::HandleCallback(uv_fs_t* req) {
  FSWrap* wrap = reinterpret_cast<FSWrap*>(req->data);

  Value* argv[2] = { Nil::New(), Number::NewIntegral(req->result) };

  if (req->fs_type == UV_FS_STAT) {
    argv[1] = GetStatObject(req);
  }

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
  if (argc < 3 ||
      !argv[0]->Is<String>() ||
      !argv[1]->Is<Number>() ||
      !argv[2]->Is<Number>()) {
    return Nil::New();
  }

  int flags = argv[1]->As<Number>()->IntegralValue();
  int mode = argv[2]->As<Number>()->IntegralValue();

  char* path = StringToChar(argv[0]);
  if (argc >= 4 && argv[3]->Is<Function>()) {
    Function* cb = argv[3]->As<Function>();
    ASYNC_CALL(open, cb, path, flags, mode);
    delete[] path;

    return Nil::New();
  } else {
    SYNC_CALL(open, path, flags, mode);
    delete[] path;

    return Number::NewIntegral(SYNC_REQ->result);
  }
}


Value* FS::Close(uint32_t argc, Value** argv) {
  // Input constraints
  if (argc < 1 || !argv[0]->Is<Number>()) {
    return Nil::New();
  }

  int fd = argv[0]->As<Number>()->IntegralValue();

  if (argc >= 2 && argv[1]->Is<Function>()) {
    Function* cb = argv[1]->As<Function>();
    ASYNC_CALL(close, cb, fd);
    return Nil::New();
  } else {
    SYNC_CALL(close, fd);
    return Number::NewIntegral(SYNC_REQ->result);
  }
}


Value* FS::Read(uint32_t argc, Value** argv) {
  // Input constraints
  if (argc < 4 ||
      !argv[0]->Is<Number>() ||
      !Buffer::HasInstance(argv[1]) ||
      !argv[2]->Is<Number>() ||
      !argv[3]->Is<Number>()) {
    return Nil::New();
  }

  int fd = argv[0]->As<Number>()->IntegralValue();
  Buffer* buff = CWrapper::Unwrap<Buffer>(argv[1]);
  int len = argv[2]->As<Number>()->IntegralValue();
  int offset = argv[3]->As<Number>()->IntegralValue();

  assert(offset + len <= buff->size());

  if (argc >= 5 && argv[4]->Is<Function>()) {
    Function* cb = argv[4]->As<Function>();
    ASYNC_CALL(read, cb, fd, buff->data(), len, offset);
    return Nil::New();
  } else {
    SYNC_CALL(read, fd, buff->data(), len, offset);
    return Number::NewIntegral(SYNC_REQ->result);
  }
}


Value* FS::Write(uint32_t argc, Value** argv) {
  // Input constraints
  if (argc < 4 ||
      !argv[0]->Is<Number>() ||
      !Buffer::HasInstance(argv[1]) ||
      !argv[2]->Is<Number>() ||
      !argv[3]->Is<Number>()) {
    return Nil::New();
  }

  int fd = argv[0]->As<Number>()->IntegralValue();
  Buffer* buff = CWrapper::Unwrap<Buffer>(argv[1]);
  int len = argv[2]->As<Number>()->IntegralValue();
  int offset = argv[3]->As<Number>()->IntegralValue();

  assert(offset + len <= buff->size());

  if (argc >= 5 && argv[4]->Is<Function>()) {
    Function* cb = argv[4]->As<Function>();
    ASYNC_CALL(write, cb, fd, buff->data(), len, offset);
    return Nil::New();
  } else {
    SYNC_CALL(write, fd, buff->data(), len, offset);
    return Number::NewIntegral(SYNC_REQ->result);
  }
}


Value* FS::Stat(uint32_t argc, Value** argv) {
  // Input constraints
  if (argc < 1 ||
      !argv[0]->Is<String>()) {
    return Nil::New();
  }

  char* path = StringToChar(argv[0]);
  if (argc >= 2 && argv[1]->Is<Function>()) {
    Function* cb = argv[1]->As<Function>();
    ASYNC_CALL(stat, cb, path);
    delete[] path;

    return Nil::New();
  } else {
    SYNC_CALL(stat, path);
    delete[] path;

    if (SYNC_REQ->result) return Nil::New();

    return FSWrap::GetStatObject(SYNC_REQ);
  }
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
  target->Set("close", Function::New(Close));
  target->Set("read", Function::New(Read));
  target->Set("write", Function::New(Write));
  target->Set("stat", Function::New(Stat));
}

} // namespace can
