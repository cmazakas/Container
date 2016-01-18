#ifndef CONTAINER_HPP_
#define CONTAINER_HPP_

#include "globals.hpp"
#include "element.hpp"
#include "container-iterator.hpp"
#include "tests/test.hpp"

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
  typedef std::unordered_map<std::thread::id, ElementPtr> FreeLists;

  private:
  Blocks blocks_;

  FreeLists free_lists_;

  ElementPtr first_;
  ElementPtr last_;
  ElementPtr free_list_;

  size_type size_;
  size_type capacity_;
  size_type block_size_;

  void pushBlock(void);
  void popBlock(void);
  void newPushBlock(void);

  std::function<void(void)> push_block_ = std::bind(&Container::pushBlock, this);
  std::function<void(void)> pop_block_ = std::bind(&Container::popBlock, this);

  public:
  Container(void);

  template <class... Args> void emplace(Args&&... args);
  void remove(iterator& it);
  void clear(void); // have I defined this yet?

  size_type size(void) const;
  size_type capacity(void) const;
  size_type getBlockSize(void) const;

  iterator begin(void);
  iterator end(void);
  iterator rbegin(void);
  iterator rend(void);

  friend void test<int>(void);
};

#include "container-implementation.hpp"

#endif // CONTAINER_HPP_
