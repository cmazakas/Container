#include "utils.hpp"

namespace Utils
{
void spinLock( std::atomic_flag& lock )
{
  while( lock.test_and_set( std::memory_order_acquire ) ) {
  }
}

/*
 * Create an array of size N of atomic_flags
 * */
LockPtr createLockArray( size_t size )
{
  LockPtr locks{ new std::atomic_flag[size] };

  for( size_t i = 0; i < size; ++i ) {
    new ( locks.get() + i ) std::atomic_flag{ ATOMIC_FLAG_INIT };
  }

  return locks;
}

/*
 * End of namespace
 * */
}
