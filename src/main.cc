#include <stdio.h> // frpintf, fflush
#include <stdint.h> // uint32_t
#include <string.h> // strlen
#include <uv.h>
#include <candor.h>

#include "runtime.h"
#include "process.h"
#include "bindings.h"
#include "natives.h"

using namespace can;

int main(int argc, char** argv) {
  candor::Isolate isolate;

  InitProcess(argc, argv);
  LoadBindings();
  LoadNatives();

  uv_run(uv_default_loop());

  fflush(stdout);
  return 0;
}
