#include "tcp.h"
#include <string.h> //memcpy

namespace can {

using namespace candor;

const int TCP::magic = 0;
const int TCPWrite::magic = 0;


bool TCP::HasInstance(Value* value) {
  return CWrapper::HasClass(value, &magic);
}


Value* TCP::New(uint32_t argc, Value** argv) {
  TCP* t = new TCP();

  int r = uv_tcp_init(uv_default_loop(), &t->handle_);
  if (r) return Nil::New();

  t->handle_.data = t;

  return t->Wrap();
}


Value* TCP::Bind(uint32_t argc, Value** argv) {
  if (argc < 3 || !TCP::HasInstance(argv[0]) ||
      !argv[1]->Is<Number>() || !argv[2]->Is<String>()) {
    return Nil::New();
  }

  int port = argv[1]->As<Number>()->IntegralValue();
  TCP* t = CWrapper::Unwrap<TCP>(argv[0]);
  String* ip = argv[2]->As<String>();

  char ip_address[64];
  size_t len = ip->Length();
  if (len > sizeof(ip_address) - 1) len = sizeof(ip_address) - 1;

  memcpy(ip_address, ip->Value(), len);
  ip_address[len] = 0;
  struct sockaddr_in address = uv_ip4_addr(ip_address, port);
  int r = uv_tcp_bind(&t->handle_, address);

  return Number::NewIntegral(r);
}


void TCP::OnConnection(uv_stream_t* server, int status) {
  Value* argv[2] = { Nil::New(), Nil::New() };
  if (status) {
    argv[0] = Number::NewIntegral(status);
  } else {
    Value* res = TCP::New(NULL, NULL);
    TCP* t = CWrapper::Unwrap<TCP>(res);

    int r = uv_accept(server, reinterpret_cast<uv_stream_t*>(&t->handle_));
    if (r) {
      argv[0] = Number::NewIntegral(r);
    } else {
      argv[1] = res;
    }
  }

  TCP* s = reinterpret_cast<TCP*>(server->data);
  if (!s->connection_cb_.IsEmpty()) s->connection_cb_->Call(2, argv);
}


Value* TCP::Listen(uint32_t argc, Value** argv) {
  if (argc < 3 || !TCP::HasInstance(argv[0]) ||
      !argv[1]->Is<Number>() || !argv[2]->Is<Function>()) {
    return Nil::New();
  }

  TCP* t = CWrapper::Unwrap<TCP>(argv[0]);
  int backlog = argv[1]->As<Number>()->IntegralValue();
  int r = uv_listen(reinterpret_cast<uv_stream_t*>(&t->handle_),
                    backlog,
                    OnConnection);
  t->connection_cb_.Wrap(argv[2]->As<Function>());

  return Number::NewIntegral(r);
}


void TCP::OnClose(uv_handle_t* handle) {
  TCP* s = reinterpret_cast<TCP*>(handle->data);

  if (!s->close_cb_.IsEmpty()) {
    s->close_cb_->Call(0, NULL);
  }
  s->Unref();
  s->connection_cb_.Unwrap();
  s->close_cb_.Unwrap();
  s->read_cb_.Unwrap();
}


uv_buf_t TCP::AllocCallback(uv_handle_t* handle, size_t size) {
  // XXX Use slab allocator
  uv_buf_t res;
  res.base = new char[size];
  res.len = size;

  return res;
}


void TCP::ReadCallback(uv_stream_t* stream, ssize_t nread, uv_buf_t buf) {
  TCP* t = reinterpret_cast<TCP*>(stream->data);

  Buffer* b = new Buffer(buf.base, nread);

  Value* argv[2] = { Number::NewIntegral(nread), b->Wrap() };
  if (!t->read_cb_.IsEmpty()) t->read_cb_->Call(2, argv);
}


Value* TCP::ReadStart(uint32_t argc, Value** argv) {
  if (argc < 2 || !TCP::HasInstance(argv[0]) || !argv[1]->Is<Function>()) {
    return Nil::New();
  }

  TCP* t = CWrapper::Unwrap<TCP>(argv[0]);
  Function* cb = argv[1]->As<Function>();

  t->read_cb_.Wrap(cb);
  int r = uv_read_start(reinterpret_cast<uv_stream_t*>(&t->handle_),
                        AllocCallback,
                        ReadCallback);

  return Number::NewIntegral(r);
}


TCPWrite::TCPWrite(TCP* tcp, Buffer* b, Function* cb) : CWrapper(&magic),
                                                        tcp_(tcp),
                                                        b_(b),
                                                        cb_(cb) {
  Ref();
  b->Ref();

  buf_.base = b->data();
  buf_.len = b->size();

  req_.data = this;
  tcp_->Ref();
  uv_write(&req_,
           reinterpret_cast<uv_stream_t*>(&tcp_->handle_),
           &buf_,
           1,
           OnWrite);
}


TCPWrite::~TCPWrite() {
  b_->Unref();
  tcp_->Unref();
}


void TCPWrite::OnWrite(uv_write_t* req, int status) {
  Value* argv[1] = { Nil::New() };
  if (status) {
    argv[0] = Number::NewIntegral(status);
  }

  TCPWrite* w = reinterpret_cast<TCPWrite*>(req->data);
  w->cb_->Call(2, argv);
  w->Unref();
}


Value* TCP::Write(uint32_t argc, Value** argv) {
  if (argc < 3 ||
      !TCP::HasInstance(argv[0]) ||
      !Buffer::HasInstance(argv[1]) ||
      !argv[2]->Is<Function>()) {
    return Nil::New();
  }

  TCP* t = CWrapper::Unwrap<TCP>(argv[0]);
  Buffer* b = CWrapper::Unwrap<Buffer>(argv[1]);
  Function* cb = argv[2]->As<Function>();

  new TCPWrite(t, b, cb);

  return Nil::New();
}


Value* TCP::Close(uint32_t argc, Value** argv) {
  if (argc < 2 || !TCP::HasInstance(argv[0]) || !argv[1]->Is<Function>()) {
    return Nil::New();
  }

  Function* cb = argv[1]->As<Function>();
  TCP* t = CWrapper::Unwrap<TCP>(argv[0]);

  // Already closing/closed - just return
  if (t->closing_) {
    return Number::NewIntegral(-1);
  }

  t->closing_ = true;
  t->close_cb_.Wrap(cb);
  uv_close(reinterpret_cast<uv_handle_t*>(&t->handle_), OnClose);

  return Nil::New();
}


void TCP::Init(Object* target) {
  target->Set("new", Function::New(TCP::New));
  target->Set("bind", Function::New(TCP::Bind));
  target->Set("listen", Function::New(TCP::Listen));
  target->Set("close", Function::New(TCP::Close));
  target->Set("readStart", Function::New(TCP::ReadStart));
  target->Set("write", Function::New(TCP::Write));
}

} // namespace can
