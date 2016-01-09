#ifndef CONTAINER_HPP_
#define CONTAINER_HPP_

#include "globals.hpp"
#include "element.hpp"
#include "container-iterator.hpp"
#include "test.hpp"

template <class T> class Container
{
  private:
  friend class ContainerIterator<T>;

  public:
  typedef size_t size_type;
  typedef ContainerIterator<T> iterator;

  typedef Element<T>* ElementPtr;
  typedef std::unique_ptr<Element<T> []> Block;
  typedef std::vector<std::pair<Block, size_type> > Blocks;

  private:
  Blocks blocks_;

  ElementPtr first_;
  ElementPtr last_;
  ElementPtr free_list_;

  size_type size_;
  size_type capacity_;
  size_type block_size_;

  void pushBlock(void);

  public:
  Container(void);
  friend void test<int>(void);

  template <class... Args> void emplace(Args&&... args);
  void remove(iterator& it);
  void clear(void);
  size_type size(void) const;
  size_type capacity(void) const;
  
  iterator begin(void)
  {
    auto it = iterator(*this, first_);
    it.findFirstAlive();
    return it;
  }

  iterator end(void)
  {
    return iterator(*this, last_);
  }
  
  iterator rbegin(void)
  {
    return iterator(*this, first_);
  }
  
  iterator rend(void)
  {
    auto it = iterator(*this, last_);
    it.findPrevAlive();
    return it;
  }
};

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
  if (!free_list_)
    pushBlock();

  try {
    auto next = free_list_->getNext();
    free_list_->emplace(args...);
    assert(free_list_->getState() == Element<T>::State::Alive);
    free_list_ = next;
    ++size_;
  } catch (std::exception& e) {
    // TODO: Add cleanup if a new block was appended
    throw e;
  }
}

template <class T> void Container<T>::remove(iterator& it)
{
  auto element = it.get();
  
  assert(element->getState() == Element<T>::State::Alive);
  
  // set next ptr to free list head
  element->setNextAndState(free_list_, Element<T>::State::Free);
  free_list_ = element;
  --size_;
}

template <class T> void Container<T>::pushBlock(void)
{
  const size_type num_boundary_points = 2;
  const size_type true_block_size = block_size_ + num_boundary_points;
  const size_type first_idx = 0;
  const size_type last_idx = true_block_size - 1;

  Block block(new Element<T>[true_block_size]);

  // set boundary info first
  block[first_idx].setState(Element<T>::State::Boundary);
  block[last_idx].setState(Element<T>::State::Boundary);

  if (!blocks_.empty()) {
    Block& last_block = blocks_.back().first;
    size_type last_block_size = blocks_.back().second + num_boundary_points;

    last_block[last_block_size - 1].setNext(block.get());
    block[0].setNext(last_block.get() + last_block_size - 1);
  } else {
    first_ = block.get();
  }

  last_ = block.get() + last_idx;

  // iterate the internals of the block
  for (size_type i = 1; i < last_idx; ++i) {
    block[i].setState(Element<T>::State::Free);
    block[i].setNext(i + 1 == last_idx ? nullptr : block.get() + i + 1);
  }

  // do free list stuff
  if (free_list_) {
    free_list_->setNext(block.get() + 1);
  } else {
    free_list_ = block.get() + 1;
  }

  assert(free_list_);

  // finally mutate the container
  // we must use move because Block is a unique_ptr and
  // ownership must be explicitly transferred
  blocks_.emplace_back(std::move(block), block_size_);

  capacity_ += block_size_;
  block_size_ += BLOCK_INCREMENT;
}

template <class T> typename Container<T>::size_type Container<T>::size(void) const
{
  return size_;
}

template <class T> typename Container<T>::size_type Container<T>::capacity(void) const
{
  return capacity_;
}

#endif // CONTAINER_HPP_
