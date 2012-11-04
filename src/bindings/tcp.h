#ifndef _SRC_BINDINGS_NET_H_
#define _SRC_BINDINGS_NET_H_

#include <candor.h>
#include <uv.h>
#include <sys/types.h> // size_t
#include <stdlib.h> // NULL

namespace can {

class TCP : public candor::CWrapper {
 public:
  TCP() : candor::CWrapper(&magic) {
  }

  static void Init(candor::Object* target);

  static bool HasInstance(candor::Value* value);

  // Magic word
  static const int magic;

 protected:
  static void OnConnection(uv_stream_t* server, int status);

  static candor::Value* New(uint32_t argc, candor::Value** argv);
  static candor::Value* Bind(uint32_t argc, candor::Value** argv);
  static candor::Value* Listen(uint32_t argc, candor::Value** argv);

  uv_tcp_t handle_;

  candor::Handle<candor::Function> connection_cb_;
};

} // namespace can

#endif // _SRC_BINDINGS_NET_H_
