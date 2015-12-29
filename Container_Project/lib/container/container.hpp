#ifndef CONTAINER_HPP_
#define CONTAINER_HPP_

#include "globals.hpp"
#include "element.hpp"
#include "container-iterator.hpp"

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

  void PushBlock(void);

  public:
  Container(void);
  template <class U> friend void test(void);

  template <class... Args> void emplace(Args&&... args)
  {
    if (!free_list_)
      PushBlock();

    try
    {
      auto next = free_list_->GetNext();
      free_list_->emplace(args...);
      assert(free_list_->GetState() == Element<T>::State::Alive);
      free_list_ = next;
      ++size_;
    }
    catch (std::exception& e)
    {
      // TODO: Add cleanup if a new block was appended
      throw e;
    }
  }

  size_type size(void) const;
  size_type capacity(void) const;

  ContainerIterator<T> begin(void)
  {
    auto it = ContainerIterator<T>(*this, first_ + 1);
    it.FindFirstAlive();
    return it;
  }

  ContainerIterator<T> end(void)
  {
    return ContainerIterator<T>(*this, last_);
  }
};

template <class T> Container<T>::Container(void)
{
  first_ = nullptr;
  last_ = nullptr;
  free_list_ = nullptr;

  size_ = 0;
  capacity_ = 0;
  block_size_ = INTIAL_BLOCK_SIZE;

  PushBlock();
}

template <class T> void Container<T>::PushBlock(void)
{
  const size_type num_boundary_points = 2;
  const size_type true_block_size = block_size_ + num_boundary_points;
  const size_type first_idx = 0;
  const size_type last_idx = true_block_size - 1;

  Block block(new Element<T>[true_block_size]);

  // set boundary info first
  block[first_idx].SetState(Element<T>::State::Boundary);
  block[last_idx].SetState(Element<T>::State::Boundary);

  if (!blocks_.empty()) {
    Block& last_block = blocks_.back().first;
    size_type last_block_size = blocks_.back().second + num_boundary_points;

    last_block[last_block_size - 1].SetNext(block.get());
    block[0].SetNext(last_block.get() + last_block_size - 1);
  } else {
    first_ = block.get();
  }

  last_ = block.get() + last_idx;

  // iterate the internals of the block
  for (size_type i = 1; i < last_idx; ++i) {
    block[i].SetState(Element<T>::State::Free);
    block[i].SetNext(i + 1 == last_idx ? nullptr : block.get() + i + 1);
  }

  // do free list stuff
  if (free_list_) {
    free_list_->SetNext(block.get() + 1);
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

template <class T = int> void test(void)
{
  Container<T> c;

  /*
   * Default constructor should build a container with an
   * array of blocks, set the first_ and last_ pointers of the
   * container and the state of the block should be
   * boundary -> [ free ] -> boundary.
   * There should also be a free list which spans the 
   * inner boundary
   *
   */

  assert(c.blocks_.size() == 1);

  assert(c.size() == 0);

  typename Container<T>::Block& block = c.blocks_[0].first;
  typename Container<T>::size_type size = c.blocks_[0].second;
  typename Container<T>::ElementPtr curr = c.free_list_;  

  assert(c.capacity() == size);  
  assert(size == 16);
  assert(block[0].GetState() == Element<T>::State::Boundary);
  assert(block[17].GetState() == Element<T>::State::Boundary);
    
  for (decltype(size) i = 1; i < 17; ++i) {
    assert(curr);
    assert(block[i].GetState() == Element<T>::State::Free);
    assert(block.get() + i == curr);
    curr = curr->GetNext();
  }
  
  /*
   * We should also be able to emplace elements in the container
   */
   
  for (T i = 0; i < 16; ++i) {
    c.emplace(i);
  }
  
  typename Container<T>::iterator it = c.begin();
  decltype(size) i = 0;
  for ( ; it != c.end(); ++it, ++i) {
    assert(it.get() == block.get() + i + 1);
    assert(*it == i);
  }
  assert(c.free_list_ == nullptr);
  
  /*
   * We should also be able to fully expand the capacity of the
   * container and it still be forwards and backwards iterable
   */ 
   
}

#endif // CONTAINER_HPP_
