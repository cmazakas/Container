#include "utils.hpp"

namespace Utils
{
void spinLock( std::atomic_flag& lock )
{
  while( lock.test_and_set( std::memory_order_acquire ) ) {
  }
}
}
