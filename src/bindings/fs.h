#ifndef _SRC_BINDINGS_FS_H_
#define _SRC_BINDINGS_FS_H_

#include <candor.h>
#include <uv.h>

namespace can {

class FSWrap : public candor::CWrapper {
 public:
  FSWrap(candor::Function* cb);
  ~FSWrap();

  static candor::Object* GetStatObject(uv_fs_t* req);
  static void HandleCallback(uv_fs_t* req);

  inline uv_fs_t* req() { return req_; }

  static const int magic;

 protected:
  static void HandleClose(uv_handle_t* handle);

  uv_fs_t* req_;
  candor::Handle<candor::Function> cb_;
};

class FS {
 public:
  static void Init(candor::Object* target);

 protected:
  static candor::Value* Open(uint32_t argc, candor::Value** argv);
  static candor::Value* Close(uint32_t argc, candor::Value** argv);
  static candor::Value* Read(uint32_t argc, candor::Value** argv);
  static candor::Value* Write(uint32_t argc, candor::Value** argv);
  static candor::Value* Stat(uint32_t argc, candor::Value** argv);
};

} // namespace can

#endif // _SRC_BINDINGS_FS_H_
