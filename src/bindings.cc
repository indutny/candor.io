#include <candor.h>
#include <stdlib.h> // NULL

#include "bindings.h"
#include "runtime.h" // GetRuntime()

namespace can {

using namespace candor;

void LoadBindings() {
  Object* bindings = Object::New();

  for (int i = 0; can_bindings[i].name != NULL; i++) {
    // Put interface into _bindings hashmap
    Object* obj = Object::New();

    can_bindings[i].init(obj);

    bindings->Set(can_bindings[i].name, obj);
  }

  // Expose `global._bindings` object
  GetRuntime()->Set("_bindings", bindings);
}

} // namespace can
