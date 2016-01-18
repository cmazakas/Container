#include "../helpers/utils.hpp"

template <class T> class VectorList;

/*
 * Return a copy of the private size_ variable
 * */
template <class T> size_t VectorList<T>::size(void) const
{
  return size_;
}

/*
 * Return a copy of the private capacity_ variable
 * */
template <class T> size_t VectorList<T>::capacity(void) const
{
  return capacity_;
}

/*
 * Append a new Block to Blocks
 * */
template <class T> void VectorList<T>::pushBlock(void)
{
  Block<T> block = Utils::createBlock<T>(block_size_);

  if (blocks_.empty() == false) {
    Block<T>& last_block = blocks_.back();
    Utils::linkElements(
      std::get<BlockPtr<T>>(block).get(),
      std::get<BlockPtr<T>>(last_block).get(),
      0,
      std::get<size_t>(last_block) + 1
    );
  }

  capacity_ += block_size_;
  blocks_.emplace_back(std::move(block));
  block_size_ += BLOCK_INCREMENT;
}
