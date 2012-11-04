#ifndef _SRC_BINDINGS_H
#define _SRC_BINDINGS_H

#include "bindings/fs.h"
#include "bindings/buffer.h"
#include "bindings/tcp.h"
#include <candor.h>

namespace can {

struct can_binding_s {
  const char* name;
  void (*init)(candor::Object*);
};

static const struct can_binding_s can_bindings[] = {
  { "fs", FS::Init },
  { "buffer", Buffer::Init },
  { "tcp", TCP::Init },
  { NULL, NULL }
};

void LoadBindings();

} // namespace can

#endif // _SRC_BINDINGS_H
