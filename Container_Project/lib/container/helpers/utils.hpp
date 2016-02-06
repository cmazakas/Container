#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <thread>
#include <atomic>

#include "../globals.hpp"

template <class T>
class Element;

namespace Utils
{

LockPtr createLockArray( size_t size );

/*
 * Execute functions with a spinlock as the
 * functional mutex
 * */
void spinLock( std::atomic_flag& lock );
template <typename T>
struct Tag
{
};

template <typename F>
auto spinLockExecutorHelper( const F& f, std::atomic_flag& lock, Tag<void> ) -> void
{
  spinLock( lock );
  f();
}

template <typename F, typename R>
auto spinLockExecutorHelper( const F& f, std::atomic_flag& lock, Tag<R> ) -> R
{
  spinLock( lock );
  return f();
}

template <class F>
auto spinLockExecutor( const F& f, std::atomic_flag& l ) -> decltype( f() )
{
  return spinLockExecutorHelper( f, l, Tag<decltype( f() )>{} );
}

/*
 * Sets an "internal free list"
 * Every Element is Free'd, and set to point to the next element
 * in the array. The last element of the array points to nothing.
 * */
template <class T>
void setInternalFreeList( Element<T>* elements, size_t size )
{
  for( size_t i = 0; i < size; ++i ) {
    elements[i].setNext( i == size - 1 ? nullptr : elements + i + 1 );
  }
}

/*
 * Takes an array of Elements of size N and sets the first
 * and last Elements to be of state Boundary
 * */
template <class T>
void markBoundaries( Element<T>* elements, size_t n )
{
  auto first = elements + 0;
  auto last = elements + n - 1;

  first->makeBoundary();
  first->setNext( nullptr );

  last->makeBoundary();
  last->setNext( nullptr );
}

/*
 * Create a free-floating "Block" of capacity N
 * */
template <class T>
Block<T> createBlock( size_t size )
{
  size_t num_elements = size + 2;
  BlockPtr<T> block_ptr( new Element<T>[num_elements] );
  setInternalFreeList( block_ptr.get() + 1, size );
  markBoundaries( block_ptr.get(), num_elements );

  Block<T> block( std::move( block_ptr ), std::move( createLockArray( num_elements ) ), size );
  return block;
}

/*
 * Link two elements together
 * */
template <class T>
void linkElements( Element<T>* a, Element<T>* b )
{
  a->setNext( b );
  b->setNext( a );
}

/*
 * End of namespace
 * */
}

#endif // UTILS_HPP_
