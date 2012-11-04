#ifndef _SRC_BINDINGS_NET_H_
#define _SRC_BINDINGS_NET_H_

#include <candor.h>
#include <uv.h>
#include "buffer.h" // Buffer
#include <sys/types.h> // size_t
#include <stdlib.h> // NULL

namespace can {

// Forward declarations
class TCPWrite;

class TCP : public candor::CWrapper {
 public:
  TCP() : candor::CWrapper(&magic), closing_(false) {
    Ref();
  }

  static void Init(candor::Object* target);

  static bool HasInstance(candor::Value* value);

  // Magic word
  static const int magic;

 protected:
  static void OnConnection(uv_stream_t* server, int status);
  static void OnClose(uv_handle_t* server);
  static uv_buf_t AllocCallback(uv_handle_t* handle, size_t size);
  static void ReadCallback(uv_stream_t* stream, ssize_t nread, uv_buf_t buf);

  static candor::Value* New(uint32_t argc, candor::Value** argv);
  static candor::Value* Bind(uint32_t argc, candor::Value** argv);
  static candor::Value* Listen(uint32_t argc, candor::Value** argv);
  static candor::Value* Close(uint32_t argc, candor::Value** argv);
  static candor::Value* ReadStart(uint32_t argc, candor::Value** argv);
  static candor::Value* Write(uint32_t argc, candor::Value** argv);

  bool closing_;
  uv_tcp_t handle_;

  candor::Handle<candor::Function> connection_cb_;
  candor::Handle<candor::Function> close_cb_;
  candor::Handle<candor::Function> read_cb_;

  friend class TCPWrite;
};

class TCPWrite : public candor::CWrapper {
 public:
  TCPWrite(TCP* tcp, Buffer* b, candor::Function* cb);
  ~TCPWrite();

  // Magic word
  static const int magic;

 protected:
  static void OnWrite(uv_write_t* req, int status);

  TCP* tcp_;
  Buffer* b_;
  candor::Handle<candor::Function> cb_;
  uv_write_t req_;
  uv_buf_t buf_;
};

} // namespace can

#endif // _SRC_BINDINGS_NET_H_
