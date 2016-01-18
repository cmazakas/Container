template <class T> class VectorList;

template <class T> VectorList<T>::VectorList(void)
{
  blocks_.reserve(INITIAL_BLOCK_SIZE);

  first_ = nullptr;
  last_ = nullptr;

  size_ = 0;
  capacity_ = 0;
  block_size_ = INITIAL_BLOCK_SIZE;
  pushBlock();
}
