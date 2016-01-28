#include "../helpers/utils.hpp"

template <class T> class VectorList;

/*
 * Return a copy of the private size_ variable
 * */
template <class T> size_t VectorList<T>::size( void ) const
{
  return size_;
}

/*
 * Return a copy of the private capacity_ variable
 * */
template <class T> size_t VectorList<T>::capacity( void ) const
{
  return capacity_;
}

/*
 * Append a new Block to Blocks
 * */
template <class T> void VectorList<T>::pushBlock( void )
{
  Block<T> block{ std::move( Utils::createBlock<T>( block_size_ ) ) };

  auto functor = [&]() -> void {
    if( !blocks_.empty() ) {
      Block<T>& last_block = blocks_.back();
      Element<T>* a = std::get<BlockPtr<T> >( last_block ).get() + std::get<size_t>( last_block ) + 1;
      Element<T>* b = std::get<BlockPtr<T> >( block ).get();

      Utils::linkElements( a, b );
    }

    blocks_.emplace_back( std::move( block ) );

    capacity_ += block_size_;
    block_size_ += BLOCK_INCREMENT;
  };

  //  Utils::spinLockExecutor( functor );
  //
  //  // append to free list
  //
  //  std::thread::id curr_id = std::this_thread::get_id();
  //
  //  if ( free_lists_.find( curr_id ) != free_lists_.end() ) {
  //
  //  }
}
