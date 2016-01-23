#ifndef HELPERS_HPP_
#define HELPERS_HPP_

#include <thread>
#include <atomic>

#include "../globals.hpp"
#include "../element/element.hpp"

namespace Utils
{
  /*
   * Sets an "internal free list"
   * Every Element is Free'd, and set to point to the next element
   * in the array. The last element of the array points to nothing.
   * */
  template <class T> void setInternalFreeList(Element<T>* elements, size_t size)
  {
    for (size_t i = 0; i < size; ++i) {
      elements[i].setNext(i == size - 1 ? nullptr : elements + i + 1);
    }
  }

  /*
   * Takes an array of Elements of size N and sets the first
   * and last Elements to be of state Boundary
   * */
  template <class T> void markBoundaries(Element<T>* elements, size_t n)
  {
    auto first = elements + 0;
    auto last = elements + n - 1;

    first->makeBoundary();
    first->setNext(nullptr);

    last->makeBoundary();
    last->setNext(nullptr);
  }


  /*
   * Create a free-floating "Block" of capacity N
   * */
  template <class T> Block<T> createBlock(size_t size)
  {
    size_t num_elements = size + 2;
    BlockPtr<T> block_ptr(new Element<T>[num_elements]);
    setInternalFreeList(block_ptr.get() + 1, size);
    markBoundaries(block_ptr.get(), num_elements);

    Block<T> block(std::move(block_ptr), size);
    return block;
  }

  /*
   * Link two elements together at specified indices
   * */
//  template <class T> void linkElements(Element<T>* element1, Element<T>* element2, size_t idx1, size_t idx2)
//  {
//    element1[idx1].setNext(element2 + idx2);
//    element2[idx2].setNext(element1 + idx1);
//  }


  /*
   * End of namespace
   * */
}

#endif // HELPERS_HPP_
