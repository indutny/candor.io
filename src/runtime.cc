#include "runtime.h"

#include <stdlib.h> // NULL
#include <stdio.h> // fprintf
#include <string.h> // strncpy
#include <sys/types.h> // size_t

namespace can {

using namespace candor;

void PrettyPrint(Value* value) {
  String* str;
  Array* keys;

  switch (value->Type()) {
   case Value::kNil:
    fprintf(stdout, "\033[1;30m" "nil" "\033[0m");
    break;
   case Value::kCData:
    fprintf(stdout, "\033[1;31m" "cdata" "\033[0m");
    break;
   case Value::kFunction:
    fprintf(stdout, "\033[1;36m" "function" "\033[0m");
    break;
   case Value::kNumber:
   case Value::kBoolean:
    str = value->ToString();
    fprintf(stdout,
            "\033[0;34m" "%.*s" "\033[0m",
            str->Length(),
            str->Value());
    break;
   case Value::kString:
    str = value->ToString();
    fprintf(stdout,
            "\033[1;32m" "\"" "\033[0;32m" "%.*s" "\033[1;32m" "\"" "\033[0m",
            str->Length(),
            str->Value());
    break;
   case Value::kObject:
    keys = value->As<Object>()->Keys();
    fprintf(stdout, "{ ");
    for (int64_t i = 0; i < keys->Length(); i++) {
      Value* key = keys->Get(i);
      Value* prop = value->As<Object>()->Get(key);

      str = key->ToString();
      fprintf(stdout,
              key->Is<Number>() ? "[%.*s]" : "%.*s",
              str->Length(),
              str->Value());

      fprintf(stdout, ": ");

      PrettyPrint(prop);

      if (i < keys->Length() - 1) fprintf(stdout, ", ");
    }
    fprintf(stdout, " }");
    break;
   case Value::kArray:
    fprintf(stdout, "[ ");
    for (int64_t i = 0; i < value->As<Array>()->Length(); i++) {
      Value* val = value->As<Array>()->Get(i);

      PrettyPrint(val);

      if (i < value->As<Array>()->Length() - 1) {
        fprintf(stdout, ", ");
      }
    }
    fprintf(stdout, " ]");
    break;
   default:
    abort();
  }
}


Value* CLog(uint32_t argc, Value* argv[]) {
  for (uint32_t i = 0; i < argc; i++) {
    PrettyPrint(argv[i]);
  }
  fprintf(stdout, "\n");

  return Nil::New();
}


Value* CCompile(uint32_t argc, Value* argv[]) {
  String* filename = (argc >= 1 ? argv[0] : Nil::New())->ToString();
  String* code = (argc >= 2 ? argv[1] : Nil::New())->ToString();

  char name[1024];
  size_t len = sizeof(name) - 1;

  if (filename->Length() < len) {
    len = filename->Length();
  }
  strncpy(name, filename->Value(), len);

  Function* res = Function::New(name, code->Value(), code->Length());

  if (Isolate::GetCurrent()->HasError()) {
    Isolate::GetCurrent()->PrintError();
    return Nil::New();
  }

  res->SetContext(GetRuntime());

  return res;
}


Value* CLoadBuiltin(uint32_t argc, Value* argv[]) {
  return Nil::New();
}


static Object* cached_runtime = NULL;


Object* GetRuntime() {
  if (cached_runtime != NULL) return cached_runtime;

  Object* res = Object::New();

  res->Set("log", Function::New(CLog));

  res->Set("compile", Function::New(CCompile));
  res->Set("builtin", Function::New(CLoadBuiltin));

  cached_runtime = res;

  return res;
}

} // namespace can