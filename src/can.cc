#include <stdio.h> // frpintf, fflush
#include <stdint.h> // uint32_t
#include <string.h> // strlen
#include <candor.h>
#include <uv.h>

#include "can_natives.h"
#include "runtime.h"

using candor::Isolate;
using candor::Function;

using namespace can;

int main(int argc, char** argv) {
  Isolate isolate;

  Function* code = Function::New("main",
                                 can_natives[0].source,
                                 can_natives[0].size);

  // Self-checking
  if (isolate.HasError()) {
    isolate.PrintError();
    return 1;
  }

  code->SetContext(can::GetRuntime());
  code->Call(0, NULL);

  uv_run(uv_default_loop());

  fflush(stdout);
  return 0;
}
