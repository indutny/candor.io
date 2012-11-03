#include "buffer.h"

namespace can {

using namespace candor;

const int Buffer::magic = 0;

Value* Buffer::New(uint32_t argc, Value** argv) {
  if (argc < 1) return Nil::New();

  Buffer* b = NULL;

  if (argv[0]->Is<Number>()) {
    b = new Buffer(argv[0]->As<Number>()->IntegralValue());
  } else {
    return Nil::New();
  }

  return b->Wrap();
}


void Buffer::Init(Object* target) {
  target->Set("new", Function::New(Buffer::New));
}

} // namespace can
