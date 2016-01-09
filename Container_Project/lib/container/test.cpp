#include "test.hpp"
#include "container.hpp"

template <> void test<int>(void)
{
  {
    std::cout<< "\nContainer<T>::Container tests\n" << std::endl;
    
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
      
    for (decltype(size) i = 1; i < 17; ++i) {
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
    for (int i = 0; i < 16; ++i) {
      c.emplace(i);
    }
    
    typename Container<int>::iterator it = c.begin();
    decltype(size) i = 0;
    for ( ; it != c.end(); ++it, ++i) {
//      std::cout << it.get() << " : " << i << std::endl;
      assert(it.get()->getState() == Element<int>::State::Alive);
      assert(it.get() == block.get() + i + 1);
      assert((decltype(size) ) *it == i);
    }
    assert(c.free_list_ == nullptr);
    std::cout << "We can!\n" << std::endl;
  }
  
  {
    std::cout << "\nBi-Directional Iterator Testing" << std::endl;
    Container<int> c;
    
    const int size = 128;
    const int capacity = 160;
    
    /*
     * We should also be able to fully expand the capacity of the
     * container and it still be forwards and backwards iterable
     */
    
    for (int i = 0; i < size; ++i) {
      c.emplace(i);
    }
    
    std::cout << "We should be able to iterate forward" << std::endl;
    int i = 0;
    for (auto it = c.begin(); it != c.end(); ++it, ++i) {
//      std::cout << *it << std::endl;
      assert(*it == i);
    }
    std::cout << "We can!\n" << std::endl;
    
    std::cout << "We should be able to iterate backward" << std::endl;
    --i;
    for (auto it = c.rend(); it != c.rbegin(); --it, --i) {
//      std::cout << *it << std::endl;
      assert(*it == i);
    }
    std::cout << "We can!\n" << std::endl;
    
    assert(c.capacity() == capacity);
    
    assert(c.end().getState() == Element<int>::State::Boundary);
  }
  
  {
    std::cout << "\nContainer<T>::remove() method testing\n" << std::endl;
    
    Container<int> c;
    const int size = 128;
    const int capacity = 160;
    
    for (int i = 0; i < size; ++i) {
      c.emplace(i);
    }
    
    std::cout << "It should be able to remove every 4th element" << std::endl;
    int i = 0;
    for (auto it = c.begin(); it != c.end(); ++i, ++it) {
      if (i % 4 == 0) {
        c.remove(it); // .remove() invalidates iterators
      } 
    }
    
    for (auto it = c.begin(); it != c.end(); ++it) {
//      std::cout << *it << std::endl;
      assert(*it % 4 != 0);
    }
    std::cout << "It does!\n" << std::endl;
    
    std::cout << "If we forward iterate, we should be able to remove the remaining elements" << std::endl;
    for (auto it = c.begin(); it != c.end(); ++it) {
      c.remove(it);
    }
    
    assert(c.size() == 0);
    assert(c.capacity() == capacity);
    std::cout << "We can!\n" << std::endl;
    
    std::cout << "We should be able to empty it if we reverse backwards as well" << std::endl;
    for (int i = 0; i < size; ++i) {
      c.emplace(i);
    }
    
    assert(c.size() == size);
    assert(c.capacity() == capacity);
    
    for (auto it = c.rend(); it != c.rbegin(); --it) {
      c.remove(it);
    }
    
    assert(c.size() == 0);
    std::cout << "We can!\n" << std::endl;
    
    std::cout << "The container should be able to be filled again" << std::endl;
    for (int i = 0; i < size; ++i) {
      c.emplace(i);
    }
    
    i = 0;
    for (auto it = c.begin(); it != c.end(); ++it, ++i) {
      assert(*it == i);
    }
    
    int j = size - 1;
    for (auto it = c.rend(); it != c.rbegin(); --it, --j) {
      assert(*it == j);
    }
    
    assert(c.size() == size);
    assert(c.capacity() == capacity);
    std::cout << "It can!\n" << std::endl;
    
    std::cout << "Tests passed!" << std::endl;
  }
}
