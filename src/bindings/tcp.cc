#include "tcp.h"
#include <string.h> //memcpy

namespace can {

using namespace candor;

const int TCP::magic = 0;


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


void TCP::Init(Object* target) {
  target->Set("new", Function::New(TCP::New));
  target->Set("bind", Function::New(TCP::Bind));
  target->Set("listen", Function::New(TCP::Listen));
}

} // namespace can
