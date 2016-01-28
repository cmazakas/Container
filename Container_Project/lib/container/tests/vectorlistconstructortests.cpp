#include "./test.hpp"
#include "../vectorlist/vectorlist.hpp"

template <> void vectorListConstructorTests<int>(void)
{
  /*
   * It should be default constructible
   * */
  {
    VectorList<int> vector_list;
    assert(vector_list.capacity() == INITIAL_BLOCK_SIZE);
    assert(vector_list.size() == 0);
  }
}
