#include "../globals.hpp"
#include "test.hpp"

void atomicArrayTests( void )
{
  const int size = 8;
  std::unique_ptr<int[]> p( new int[size] );
  std::unique_ptr<std::atomic_flag[]> locks( new std::atomic_flag[size] );

  for( int i = 0; i < size; ++i ) {
    p[i] = 0;
    new ( locks.get() + i ) std::atomic_flag{ ATOMIC_FLAG_INIT };
  }

  const int num_threads = 2;
  std::vector<std::thread> threads;
  threads.reserve( num_threads );

  std::unique_ptr<std::atomic_int[]> yields{ new std::atomic_int[size] };

  for( int i = 0; i < size; ++i ) {
    new ( yields.get() + i ) std::atomic_int{ 0 };
  }

  const int num_trials = 50000;

  for( int i = 0; i < num_threads; ++i ) {
    // our testing thread's function...
    threads.emplace_back( [=, &locks, &yields, &p]() -> void {
      // create random array index generator
      std::random_device rd;
      std::mt19937 gen{ rd() };
      std::uniform_int_distribution<> range{ 0, size - 1 };

      // iterate the number of trials
      for( int j = 0; j < num_trials; ++j ) {
        int tmp = range( gen );

        // track yields
        while( locks[tmp].test_and_set( std::memory_order_acquire ) ) {
          std::atomic_fetch_add( yields.get() + tmp, 1 );
        }

        // increment aligned array
        ++p[tmp];

        locks[tmp].clear( std::memory_order_release );
      }
    } );
  }

  for( auto& t : threads )
    t.join();

  int sum = 0;

  for( int i = 0; i < size; ++i ) {
    sum += p[i];
  }

  assert( sum == num_threads * num_trials );

  for( int i = 0; i < size; ++i ) {
    std::cout << "num. yields for index " << i << " : " << yields[i] << std::endl;
    std::cout << "num. increments for index " << i << " : " << p[i] << std::endl;
  }
}
