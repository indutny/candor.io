#ifndef _SRC_BINDINGS_FS_H_
#define _SRC_BINDINGS_FS_H_

#include <candor.h>
#include <uv.h>

namespace can {

class FSWrap : public candor::CWrapper {
 public:
  FSWrap(candor::Function* cb);
  ~FSWrap();

  static void HandleCallback(uv_fs_t* req);

  inline uv_fs_t* req() { return req_; }

 protected:
  static void HandleClose(uv_handle_t* handle);

  uv_fs_t* req_;
  candor::Handle<candor::Function> cb_;
};

candor::Value* FSOpen(uint32_t argc, candor::Value** argv);

void LoadFSBinding(candor::Object* target);

} // namespace can

#endif // _SRC_BINDINGS_FS_H_
