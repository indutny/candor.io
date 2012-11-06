#include <candor.h>
#include <string.h> // strncmp
#include "process.h"
#include "runtime.h"

namespace can {

using namespace candor;

void InitProcess(int argc, char** argv) {
  Object* process = Object::New();
  Array* aargv = Array::New();

  int j = 0;
  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "--print-hir") == 0) {
      Isolate::EnableHIRLogging();
    } else if (strcmp(argv[i], "--print-lir") == 0) {
      Isolate::EnableLIRLogging();
    } else {
      aargv->Set(j++, String::New(argv[i]));
    }
  }

  process->Set("argc", Number::NewIntegral(j));
  process->Set("argv", aargv);

  GetRuntime()->Set("process", process);
}

} // namespace can
