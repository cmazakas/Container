#ifndef CONTAINER_IMPLEMENTATION_HPP_
#define CONTAINER_IMPLEMENTATION_HPP_

#include "globals.hpp"
#include "helpers/utils.hpp"

/*
 * Implementations
 *
 */

template <class T> Container<T>::Container(void)
{
  blocks_.reserve(32);
  
  first_ = nullptr;
  last_ = nullptr;
  free_list_ = nullptr;

  size_ = 0;
  capacity_ = 0;
  block_size_ = INTIAL_BLOCK_SIZE;

  pushBlock();
}

template <class T> template <class... Args> void Container<T>::emplace(Args&&... args)
{   
  bool pushed_block = false;

  if(!free_list_) {
    pushBlock();
    pushed_block = true;
  }

  auto next = free_list_->getNext();

  try {
    free_list_->emplace(args...);
    assert(free_list_->getState() == Element<T>::State::Alive);

    free_list_ = next;
    ++size_;
  } catch(std::exception& e) {
    if(pushed_block) {
      popBlock();
    }
    assert(free_list_ != next);
    throw e;
  }
}

template <class T> void Container<T>::remove(iterator& it)
{
  auto element = it.get();

  assert(element->getState() == Element<T>::State::Alive);

  // set next ptr to free list head
  auto old_free_list = free_list_;
  element->setNextAndState(free_list_, Element<T>::State::Free);
  free_list_ = element;
  
  --size_;
  
  assert(free_list_->getNext() == old_free_list);
  assert(free_list_ != old_free_list);
  assert(free_list_->getState() == Element<T>::State::Free);
}

/*
 * Add a new block to the container in an atomic way
 * */
template <class T> void Container<T>::newPushBlock(void)
{
  assert(size_ == capacity_);
  
  size_type size = block_size_;
  Block block_ptr_(std::move(Utils::createBlock<T>(size)));
  
  
}

template <class T> void Container<T>::pushBlock(void)
{
  assert(free_list_ == nullptr);

  const size_type num_boundary_points = 2;
  const size_type true_block_size = block_size_ + num_boundary_points;
  const size_type first_idx = 0;
  const size_type last_idx = true_block_size - 1;

  Block block(new Element<T>[true_block_size]);

  // set boundary info first
  block[first_idx].setState(Element<T>::State::Boundary);
  block[last_idx].setState(Element<T>::State::Boundary);

  if(!blocks_.empty()) {
    Block& last_block = blocks_.back().first;
    size_type last_block_size = blocks_.back().second + num_boundary_points;

    last_block[last_block_size - 1].setNext(block.get());
    block[0].setNext(last_block.get() + last_block_size - 1);
  } else {
    first_ = block.get();
  }

  last_ = block.get() + last_idx;

  // iterate the internals of the block
  for(size_type i = 1; i < last_idx; ++i) {
    block[i].setState(Element<T>::State::Free);
    block[i].setNext(i + 1 == last_idx ? free_list_ : block.get() + i + 1);
  }
  free_list_ = block.get() + 1;
  assert(free_list_);

  blocks_.emplace_back(std::move(block), block_size_);
  capacity_ += block_size_;
  block_size_ += BLOCK_INCREMENT;
}

template <class T> void Container<T>::popBlock(void)
{
  block_size_ -= BLOCK_INCREMENT;
  capacity_ -= block_size_;
  free_list_ = nullptr;
  blocks_.pop_back();
}

template <class T> typename Container<T>::size_type Container<T>::size(void) const
{
  return size_;
}

template <class T> typename Container<T>::size_type Container<T>::capacity(void) const
{
  return capacity_;
}

template <class T> typename Container<T>::size_type Container<T>::getBlockSize(void) const
{
  return block_size_;
}

template <class T> typename Container<T>::iterator Container<T>::begin(void)
{
  auto it = iterator(*this, first_);
  it.findFirstAlive();
  return it;
}

template <class T> typename Container<T>::iterator Container<T>::end(void)
{
  return iterator(*this, last_);
}

template <class T> typename Container<T>::iterator Container<T>::rbegin(void)
{
  return iterator(*this, first_);
}

template <class T> typename Container<T>::iterator Container<T>::rend(void)
{
  auto it = iterator(*this, last_);
  it.findPrevAlive();
  return it;
}

#endif // CONTAINER_IMPLEMENTATION_HPP_
