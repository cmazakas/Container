#include "test.hpp"
#include "../container.hpp"
#include "../helpers/utils.hpp"

void printTitle(const std::string& s)
{
  std::cout << "**" << std::string(s.length(), '*') << "**" << std::endl;
  std::cout << "  " << s << "  " << std::endl;
  std::cout << "**" << std::string(s.length(), '*') << "**\n" << std::endl;
}

/*
 * Make sure to test the helper functions as
 * these are the building blocks of the codebase
 * */
template <> void utilsTests<int>(void)
{
  /*
   * setInternalFreeList
   * */
  {
    const int size = 64;
    std::unique_ptr<Element<int> []> elements(new Element<int>[size]);
    Utils::setInternalFreeList(elements.get(), size);
    for(int i = 0; i < size - 1; ++i) {
      assert(elements[i].getState() == Element<int>::State::Free);
      assert(elements[i].getNext() == elements.get() + i + 1);
    }
    assert(elements[size - 1].getState() == Element<int>::State::Free);
    assert(elements[size - 1].getNext() == nullptr);
  }

  /*
   * markBoundaries
   * */
  {
    const int size = 16;
    std::unique_ptr<Element<int> []> elements(new Element<int>[size]);
    Utils::markBoundaries(elements.get(), size);
    assert(elements[0].getState() == Element<int>::State::Boundary);
    assert(elements[0].getNext() == nullptr);
    assert(elements[size - 1].getState() == Element<int>::State::Boundary);
    assert(elements[size - 1].getNext() == nullptr);
  }

  /*
   * createBlock
   * */
  {
    const int size = 64;
    std::unique_ptr<Element<int> []> block(std::move(Utils::createBlock<int>(size)));
    assert(block[0].getState() == Element<int>::State::Boundary);
    assert(block[0].getNext() == nullptr);
    for(int i = 1; i < size; ++i) {
      assert(block[i].getState() == Element<int>::State::Free);
      assert(block[i].getNext() == block.get() + i + 1);
    }
    assert(block[size - 1 + 1].getState() == Element<int>::State::Free);
    assert(block[size - 1 + 1].getNext() == nullptr);
    assert(block[size - 1 + 2].getState() == Element<int>::State::Boundary);
    assert(block[size - 1 + 2].getNext() == nullptr);
  }
  
  /*
   * linkBlocks
   * */
   {
     const int size = 64;
     const int num_elements = size + 2;
     std::unique_ptr<Element<int> []> block1(std::move(Utils::createBlock<int>(size)));
     decltype(block1) block2(std::move(Utils::createBlock<int>(size)));
     Utils::linkBlocks(block1.get(), block2.get(), num_elements - 1, 0);
     assert(block1[num_elements - 1].getNext() == block2.get());
     assert(block2[0].getNext() == block1.get() + num_elements - 1);
     
     Utils::linkBlocks(block1.get(), block2.get(), 0, num_elements - 1);
     assert(block1[0].getNext() == block2.get() + num_elements - 1);
     assert(block2[num_elements - 1].getNext() == block1.get());
   }
}

template <> void test<int>(void)
{
  {
    printTitle("Container<T>::Container Tests");

    Container<int> c;

    /*
     * Default constructor should build a container with an
     * array of blocks, set the first_ and last_ pointers of the
     * container and the state of the block should be
     * boundary -> [ free ] -> boundary.
     * There should also be a free list which spans the
     * inner boundary
     *
     */

    std::cout << "It should be constructed with a default capacity of 16 and have a size of 0" << std::endl;
    assert(c.blocks_.size() == 1);

    assert(c.size() == 0);

    typename Container<int>::Block& block = c.blocks_[0].first;
    typename Container<int>::size_type size = c.blocks_[0].second;
    typename Container<int>::ElementPtr curr = c.free_list_;

    assert(c.capacity() == size);
    assert(size == 16);
    assert(block[0].getState() == Element<int>::State::Boundary);
    assert(block[17].getState() == Element<int>::State::Boundary);

    for(decltype(size) i = 1; i < 17; ++i) {
      assert(curr);
      assert(block[i].getState() == Element<int>::State::Free);
      assert(block.get() + i == curr);

      curr = curr->getNext();
    }

    assert(c.begin() == c.end());
    std::cout << "Construction successful!\n" << std::endl;

    /*
     * We should also be able to emplace elements in the container
     */

    std::cout << "We should be able to insert element into the container" << std::endl;
    for(int i = 0; i < 16; ++i) {
      c.emplace(i);
    }

    typename Container<int>::iterator it = c.begin();
    decltype(size) i = 0;
    for(; it != c.end(); ++it, ++i) {
      assert(it.get()->getState() == Element<int>::State::Alive);
      assert(it.get() == block.get() + i + 1);
      assert((decltype(size)) * it == i);
    }
    assert(c.free_list_ == nullptr);
    std::cout << "We can!\n" << std::endl;
  }

  {
    printTitle("Bidirectional Iterator Testing");
    Container<int> c;

    const int size = 128;
    const int capacity = 160;

    /*
     * We should also be able to fully expand the capacity of the
     * container and it still be forwards and backwards iterable
     */

    for(int i = 0; i < size; ++i) {
      c.emplace(i);
    }

    std::cout << "We should be able to iterate forward" << std::endl;
    int i = 0;
    for(auto it = c.begin(); it != c.end(); ++it, ++i) {
      //      std::cout << *it << std::endl;
      assert(*it == i);
    }
    std::cout << "We can!\n" << std::endl;

    std::cout << "We should be able to iterate backward" << std::endl;
    --i;
    for(auto it = c.rend(); it != c.rbegin(); --it, --i) {
      //      std::cout << *it << std::endl;
      assert(*it == i);
    }
    std::cout << "We can!\n" << std::endl;

    assert(c.capacity() == capacity);

    assert(c.end().getState() == Element<int>::State::Boundary);
  }

  {
    printTitle("Container<T>::remove() Method Testing");

    Container<int> c;
    const int size = 128;
    const int capacity = 160;

    for(int i = 0; i < size; ++i) {
      c.emplace(i);
    }

    std::cout << "It should be able to remove every 4th element" << std::endl;
    int i = 0;
    for(auto it = c.begin(); it != c.end(); ++i, ++it) {
      if(i % 4 == 0) {
        c.remove(it); // .remove() invalidates iterators
      }
    }

    for(auto it = c.begin(); it != c.end(); ++it) {
      assert(*it % 4 != 0);
    }
    std::cout << "It does!\n" << std::endl;

    std::cout << "If we forward iterate, we should be able to remove the remaining elements" << std::endl;
    for(auto it = c.begin(); it != c.end(); ++it) {
      c.remove(it);
    }

    assert(c.size() == 0);
    assert(c.capacity() == capacity);
    std::cout << "We can!\n" << std::endl;

    std::cout << "We should be able to empty it if we reverse backwards as well" << std::endl;
    for(int i = 0; i < size; ++i) {
      c.emplace(i);
    }

    assert(c.size() == size);
    assert(c.capacity() == capacity);

    for(auto it = c.rend(); it != c.rbegin(); --it) {
      c.remove(it);
    }

    assert(c.size() == 0);
    std::cout << "We can!\n" << std::endl;

    std::cout << "The container should be able to be filled again" << std::endl;
    for(int i = 0; i < size; ++i) {
      c.emplace(i);
    }

    i = 0;
    for(auto it = c.begin(); it != c.end(); ++it, ++i) {
      assert(*it == i);
    }

    int j = size - 1;
    for(auto it = c.rend(); it != c.rbegin(); --it, --j) {
      assert(*it == j);
    }

    assert(c.size() == size);
    assert(c.capacity() == capacity);
    std::cout << "It can!\n" << std::endl;
  }

  {
    printTitle("'High Pressure' Testing");

    std::cout << "It should be able to support equal parts insertions and deletions without crashing" << std::endl;
    Container<int> cont;

    for(int i = 0; i < 15000; ++i) {
      cont.emplace(i);
    }

    auto it = cont.begin();
    while(it != cont.end() && it != cont.rbegin() && it != cont.rend()) {
      assert(it.getState() == Element<int>::State::Alive);
      assert(it.getState() == Element<int>::State::Alive);

      ++it;
      cont.remove(it);
      ++it;
      cont.emplace(1337);
      --it;
    }

    std::cout << "It can!\n" << std::endl;
  }
}

class Tetra
{
  private:
  std::unique_ptr<int[]> data_;

  public:
  Tetra(bool do_throw)
  {
    if(do_throw) {
      throw std::runtime_error("I'm a bad constructor!");
    } else {
      data_ = std::move(std::unique_ptr<int[]>(new int[16]));
    }
  }
};

template <> void test<Tetra>(void)
{
  printTitle("Exception Safety Testing");
  Container<Tetra> c;

  std::cout << "It should be able to handle non-trivial constructors that don't always throw" << std::endl;
  const int size = 16;
  for(int i = 0; i < size; ++i) {
    c.emplace(false);
  }

  assert(c.size() == size);
  assert(c.capacity() == 16);
  std::cout << "It can!\n" << std::endl;

  std::cout << "It should not mutate the container if the allocation fails" << std::endl;
  try {
    c.emplace(true);
  } catch(std::exception& e) {
    assert(c.size() == size);
    assert(c.capacity() == 16);
    assert(c.getBlockSize() == 32);
  }
  std::cout << "It doesn't\n" << std::endl;

  std::cout << "It should still be able to be allocated to aftewards though" << std::endl;
  for(int i = 0; i < 128; ++i) {
    c.emplace(false);
  }
  std::cout << "It can!\n" << std::endl;

  printTitle("Tests Passed!");
}
