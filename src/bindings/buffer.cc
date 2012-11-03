#include "buffer.h"

#include <string.h> // memcpy

namespace can {

using namespace candor;

const int Buffer::magic = 0;

bool Buffer::HasInstance(Value* value) {
  return CWrapper::HasClass(value, &magic);
}


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


Value* Buffer::Stringify(uint32_t argc, Value** argv) {
  if (argc < 1 || !Buffer::HasInstance(argv[0])) return String::New("");

  Buffer* b = CWrapper::Unwrap<Buffer>(argv[0]);
  return String::New(b->data(), b->size());
}


Value* Buffer::Length(uint32_t argc, Value** argv) {
  if (argc < 1 || !Buffer::HasInstance(argv[0])) return Number::NewIntegral(0);

  Buffer* b = CWrapper::Unwrap<Buffer>(argv[0]);
  return Number::NewIntegral(b->size());
}


Value* Buffer::Slice(uint32_t argc, Value** argv) {
  if (argc < 1 || !Buffer::HasInstance(argv[0])) return Nil::New();

  Buffer* b = CWrapper::Unwrap<Buffer>(argv[0]);
  Buffer* c;
  if (argc == 1) {
    // Just return a copy
    c = new Buffer(b->size());
    memcpy(c->data(), b->data(), b->size());
  } else if (argc >= 2 && argv[1]->Is<Number>()) {
    int begin = argv[1]->As<Number>()->IntegralValue();

    // Oob - return empty buffer
    if (begin > b->size()) {
      c = new Buffer(0);
    } else if (argc >= 3 && argv[2]->Is<Number>()) {
      int end = argv[2]->As<Number>()->IntegralValue();

      if (end > b->size()) end = b->size();

      // slice(10, 1) or slice(10, 10)
      if (end <= begin) {
        c = new Buffer(0);
      } else {
        c = new Buffer(end - begin);
        memcpy(c->data(), b->data() + begin, end - begin);
      }
    } else {
      c = new Buffer(b->size() - begin);
      memcpy(c->data(), b->data() + begin, b->size() - begin);
    }
  }
  return c->Wrap();
}


void Buffer::Init(Object* target) {
  target->Set("new", Function::New(Buffer::New));
  target->Set("length", Function::New(Buffer::Length));
  target->Set("stringify", Function::New(Buffer::Stringify));
  target->Set("slice", Function::New(Buffer::Slice));
}

} // namespace can
