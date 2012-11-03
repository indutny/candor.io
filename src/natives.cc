#include <candor.h>
#include <stdlib.h> // NULL

#include "natives.h"
#include "runtime.h" // GetRuntime()
#include "can_natives.h"

namespace can {

using namespace candor;

void LoadNatives() {
  Handle<Object> natives(Object::New());

  // Expose `global._natives` object
  GetRuntime()->Set("_natives", *natives);

  for (int i = 0; can_natives[i].source != NULL; i++) {
    Function* fn = Function::New(can_natives[i].name,
                                 can_natives[i].source,
                                 can_natives[i].size);

    if (Isolate::GetCurrent()->HasError()) {
      Isolate::GetCurrent()->PrintError();
      abort();
    }
    fn->SetContext(GetRuntime());

    // Put interface into _natives hashmap
    natives->Set(can_natives[i].name, fn->Call(0, NULL));
  }
}

} // namespace can
