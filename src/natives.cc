#include <candor.h>
#include <stdlib.h> // NULL

#include "natives.h"
#include "runtime.h" // GetRuntime()
#include "can_natives.h"

namespace can {

using namespace candor;

void LoadNatives() {
  Object* runtime = GetRuntime();
  Object* natives = Object::New();

  // Expose `global._natives` object
  runtime->Set("_natives", natives);

  for (int i = 0; can_natives[i].source != NULL; i++) {
    Function* fn = Function::New(can_natives[i].name,
                                 can_natives[i].source,
                                 can_natives[i].size);
    fn->SetContext(runtime);

    // Put interface into _natives hashmap
    natives->Set(can_natives[i].name, fn->Call(0, NULL));
  }
}

} // namespace can
