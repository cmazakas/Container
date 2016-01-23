#include "./test.hpp"
#include "../helpers/utils.hpp"

template <> void utilsTests<int>(void)
{
  const int num_elements = 16;
  /*
   * It should set an internal freelist of a
   * contiguous allocation of Elements where
   * the next pointer points to the next element
   * in memory except for the last which will point
   * to nullptr
   * */
  {
    BlockPtr<int> block_ptr(new Element<int>[num_elements]);

    Utils::setInternalFreeList(block_ptr.get(), num_elements);

    for (int i = 0; i < num_elements - 1; ++i) {
      assert(block_ptr[i].getState() == ElementState::Free);
      assert(block_ptr[i].getNext() == block_ptr.get() + i + 1);
    }

    assert(block_ptr[num_elements - 1].getState() == ElementState::Free);
    assert(block_ptr[num_elements - 1].getNext() == nullptr);
  }

  /*
   * It should be able to mark a contiguous allocation of Elements
   * with boundary Elements
   * */
  {
    BlockPtr<int> block_ptr(new Element<int>[num_elements]);
    Utils::markBoundaries(block_ptr.get(), num_elements);

    assert(block_ptr[0].getState() == ElementState::Boundary);
    assert(block_ptr[num_elements - 1].getState() == ElementState::Boundary);
  }

  /*
   * We should be able to compose the above two functions to create
   * new blocks as we need
   * */
  {
    Block<int> block {std::move(Utils::createBlock<int>((size_t ) num_elements))};
    assert(std::get<size_t>(block) == num_elements);

    auto& block_ptr = std::get<BlockPtr<int>>(block);

    assert(block_ptr[0].getState() == ElementState::Boundary);
    assert(block_ptr[num_elements + 1].getState() == ElementState::Boundary);
    assert(block_ptr[0].getNext() == nullptr);
    assert(block_ptr[num_elements + 1].getNext() == nullptr);

    for (int i = 0; i < num_elements - 1; ++i) {
      assert(block_ptr[i + 1].getState() == ElementState::Free);
      assert(block_ptr[i + 1].getNext() == block_ptr.get() + i + 2);
    }

    assert(block_ptr[num_elements].getState() == ElementState::Free);
    assert(block_ptr[num_elements].getNext() == nullptr);
  }
}
