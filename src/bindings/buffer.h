#ifndef _SRC_BINDINGS_BUFFER_H_
#define _SRC_BINDINGS_BUFFER_H_

#include <candor.h>
#include <sys/types.h> // size_t
#include <stdlib.h> // NULL

namespace can {

class Buffer : public candor::CWrapper {
 public:
  Buffer(size_t size) : candor::CWrapper(&magic), size_(size) {
    data_ = new char[size];
  }

  ~Buffer() {
    delete[] data_;
    data_ = NULL;
  }

  inline char* data() { return data_; }
  inline size_t size() { return size_; }

  static void New(uint32_t argc, candor::Value** argv);
  static void Init(candor::Object* target);

  // Magic word
  static const int magic;

 protected:
  char* data_;
  size_t size_;
};

} // namespace can

#endif // _SRC_BINDINGS_BUFFER_H_
