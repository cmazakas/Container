#include "test.hpp"
#include "container.hpp"

template <> void test<int>(void)
{
  {
    Container<int> c;

    /*
     * Default constructor should build a container with an
     * array of blocks, set the first_ and last_ pointers of the
     * container and the state of the block should be
     * boundary -> [ free ] -> boundary.
     * Uhere should also be a free list which spans the 
     * inner boundary
     *
     */

    assert(c.blocks_.size() == 1);

    assert(c.size() == 0);

    typename Container<int>::Block& block = c.blocks_[0].first;
    typename Container<int>::size_type size = c.blocks_[0].second;
    typename Container<int>::ElementPtr curr = c.free_list_;  

    assert(c.capacity() == size);  
    assert(size == 16);
    assert(block[0].GetState() == Element<int>::State::Boundary);
    assert(block[17].GetState() == Element<int>::State::Boundary);
      
    for (decltype(size) i = 1; i < 17; ++i) {
      assert(curr);
      assert(block[i].GetState() == Element<int>::State::Free);
      assert(block.get() + i == curr);
      
      curr = curr->GetNext();
    }
    
    /*
     * We should also be able to emplace elements in the container
     */
     
    for (int i = 0; i < 16; ++i) {
      c.emplace(i);
    }
    
    typename Container<int>::iterator it = c.begin();
    decltype(size) i = 0;
    for ( ; it != c.end(); ++it, ++i) {
      std::cout << it.get() << " : " << i << std::endl;
      assert(it.get()->GetState() == Element<int>::State::Alive);
      assert(it.get() == block.get() + i + 1);
      assert((decltype(size) ) *it == i);
    }
    assert(c.free_list_ == nullptr);    
  }
  
  {
    std::cout << "\nBi-Directional Iterator Testing" << std::endl;
    Container<int> c;
    
    /*
     * We should also be able to fully expand the capacity of the
     * container and it still be forwards and backwards iterable
     */
    
    for (int i = 0; i < 128; ++i) {
      c.emplace(i);
    }
    
    std::cout << "forward" << std::endl;
    int i = 0;
    for (auto it = c.begin(); it != c.end(); ++it, ++i) {
      std::cout << *it << std::endl;
      assert(*it == i);
    }
    std::cout << std::endl;
    
    std::cout << "backward" << std::endl;
    --i;
    for (auto it = c.rend(); it != c.rbegin(); --it, --i) {
      std::cout << *it << std::endl;
      assert(*it == i);
    }
    std::cout << std::endl;
    
    assert(c.capacity() == 160);
  }
}
