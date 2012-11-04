#ifndef _SRC_BINDINGS_BUFFER_H_
#define _SRC_BINDINGS_BUFFER_H_

#include <candor.h>
#include <sys/types.h> // size_t
#include <stdlib.h> // NULL

namespace can {

class Buffer : public candor::CWrapper {
 public:
  Buffer(ssize_t size) : candor::CWrapper(&magic), size_(size > 0 ? size : 0) {
    data_ = new char[size_];
  }

  ~Buffer() {
    delete[] data_;
    data_ = NULL;
  }

  inline char* data() { return data_; }
  inline ssize_t size() { return size_; }

  static void Init(candor::Object* target);

  static bool HasInstance(candor::Value* value);

  // Magic word
  static const int magic;

 protected:
  static candor::Value* New(uint32_t argc, candor::Value** argv);
  static candor::Value* Length(uint32_t argc, candor::Value** argv);
  static candor::Value* Stringify(uint32_t argc, candor::Value** argv);
  static candor::Value* Slice(uint32_t argc, candor::Value** argv);
  static candor::Value* Concat(uint32_t argc, candor::Value** argv);

  char* data_;
  ssize_t size_;
};

} // namespace can

#endif // _SRC_BINDINGS_BUFFER_H_
