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
  } else if (argv[0]->Is<String>()) {
    String* str = argv[0]->As<String>();
    b = new Buffer(str->Length());
    memcpy(b->data(), str->Value(), b->size());
  } else if (Buffer::HasInstance(argv[0])) {
    return argv[0];
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
  Buffer* c = new Buffer(0);

  if (argc == 1) {
    // Just return a buffer
  } else if (argc >= 2 && argv[1]->Is<Number>()) {
    int begin = argv[1]->As<Number>()->IntegralValue();

    if (begin > b->size()) {
      // Oob - return empty buffer
    } else if (argc >= 3 && argv[2]->Is<Number>()) {
      int end = argv[2]->As<Number>()->IntegralValue();

      if (end > b->size()) end = b->size();

      // slice(10, 1) or slice(10, 10)
      if (end <= begin) {
        // Empty buffer
      } else {
        c->size_ = end - begin;
        c->data_ = b->data_ + begin;
      }
    } else {
      c->size_ = b->size() - begin;
      c->data_ = b->data_ + begin;
    }
  }
  return c->Wrap();
}


Value* Buffer::Concat(uint32_t argc, Value** argv) {
  if (argc < 1 || !argv[0]->Is<Array>()) return Nil::New();

  Array* arr = argv[0]->As<Array>();
  int total = 0;

  if (argc < 2 || !argv[1]->Is<Number>()) {
    // Calculate total
    for (int i = 0; i < arr->Length(); i++) {
      if (!Buffer::HasInstance(arr->Get(i))) continue;
      Buffer* current = CWrapper::Unwrap<Buffer>(arr->Get(i));

      total += current->size();
    }
  } else {
    total = argv[1]->As<Number>()->IntegralValue();
  }

  Buffer* result = new Buffer(total);
  int offset = 0;
  for (int i = 0; i < arr->Length(); i++) {
    if (!Buffer::HasInstance(arr->Get(i))) continue;
    Buffer* current = CWrapper::Unwrap<Buffer>(arr->Get(i));

    int bytes = current->size();
    if (offset + bytes > result->size()) bytes = result->size() - offset;

    memcpy(result->data(), current->data(), bytes);
    offset += bytes;
  }

  return result->Wrap();
}


Value* Buffer::HasInstance(uint32_t argc, Value** value) {
  if (argc < 1) return Nil::New();
  return Boolean::New(HasInstance(value[0]));
}


void Buffer::Init(Object* target) {
  target->Set("new", Function::New(Buffer::New));
  target->Set("length", Function::New(Buffer::Length));
  target->Set("stringify", Function::New(Buffer::Stringify));
  target->Set("slice", Function::New(Buffer::Slice));
  target->Set("concat", Function::New(Buffer::Concat));
  target->Set("hasInstance", Function::New(Buffer::HasInstance));
}

} // namespace can
