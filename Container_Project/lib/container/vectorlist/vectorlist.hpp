#ifndef VECTORLIST_HPP_
#define VECTORLIST_HPP_

#include "../globals.hpp"
#include "../element/element.hpp"
#include "../tests/test.hpp"

template <class T> class VectorList
{
private:
  Blocks<T> blocks_;
  FreeLists<T> free_lists_;
  Element<T>* first_;
  Element<T>* last_;


  size_t size_;
  size_t capacity_;
  size_t block_size_;

  void pushBlock(void);

public:
  VectorList(void);
  VectorList(const VectorList& other);
  VectorList(VectorList&& other);
  VectorList& operator=(const VectorList& other);
  VectorList& operator=(VectorList&& other);
  friend void vectorListConstructorTests<int>(void);

  size_t size(void) const;
  size_t capacity(void) const;
};

#include "./constructors.hpp"
#include "./methods.hpp"

#endif // VECTORLIST_HPP_
