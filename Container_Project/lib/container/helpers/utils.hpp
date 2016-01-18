#ifndef HELPERS_HPP_
#define HELPERS_HPP_

#include <thread>
#include <atomic>

#include "../element.hpp"
#include "../globals.hpp"

namespace Utils
{
  /*
   * Sets an "internal free list"
   * Every Element is Free'd, and set to point to the next element
   * in the array. The last element of the array points to nothing.
   * */
  template <class T> void setInternalFreeList(Element<T>* element, size_t n)
  {
    for(size_t i = 0; i < n; ++i) {
      element[i].setNextAndState(i == n - 1 ? nullptr : element + i + 1, Element<T>::State::Free);
    }
  }

  /*
   * Takes an array of Elements and sets the first and last elements
   * to be of type State::Boundary
   * */
  template <class T> void markBoundaries(Element<T>* element, size_t n)
  {
    element[0].setNextAndState(nullptr, Element<T>::State::Boundary);
    element[n - 1].setNextAndState(nullptr, Element<T>::State::Boundary);
  }

  /*
   * Create a free-floating "block"
   * */
  template <class T> Block<T> createBlock(size_t size)
  {
    const size_t num_elements = size + 2;
    BlockPtr<int> block_ptr(new Element<int>[num_elements]);
    Utils::markBoundaries<int>(block_ptr.get(), num_elements);
    Utils::setInternalFreeList<int>(block_ptr.get() + 1, size);
    Block<T> block(std::move(block_ptr), size);
    return block;
  }

  /*
   * Link two elements together at specified indices
   * */
  template <class T> void linkElements(Element<T>* element1, Element<T>* element2, size_t idx1, size_t idx2)
  {
    element1[idx1].setNext(element2 + idx2);
    element2[idx2].setNext(element1 + idx1);
  }


  /*
   * End of namespace
   * */
}

#endif // HELPERS_HPP_
