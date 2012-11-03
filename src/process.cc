#include <candor.h>
#include "process.h"
#include "runtime.h"

namespace can {

using namespace candor;

void InitProcess(int argc, char** argv) {
  Object* process = Object::New();
  Array* aargv = Array::New();

  for (int i = 0; i < argc; i++) {
    aargv->Set(i, String::New(argv[i]));
  }

  process->Set("argc", Number::NewIntegral(argc));
  process->Set("argv", aargv);

  GetRuntime()->Set("process", process);
}

} // namespace can
