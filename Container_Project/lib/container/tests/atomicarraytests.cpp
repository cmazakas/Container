#include "../globals.hpp"
#include "test.hpp"
#include "../helpers/utils.hpp"

void atomicArrayTests( void )
{
  const int size = 8;
  std::unique_ptr<int[]> p( new int[size] );
  LockPtr locks{ std::move( Utils::createLockArray( size ) ) };

  for( int i = 0; i < size; ++i ) {
    p[i] = 0;
  }

  const int num_threads = 4;
  std::vector<std::thread> threads;
  threads.reserve( num_threads );

  std::unique_ptr<std::atomic_int[]> yields{ new std::atomic_int[size] };

  for( int i = 0; i < size; ++i ) {
    new ( yields.get() + i ) std::atomic_int{ 0 };
  }

  const int num_trials = 500000;

  for( int i = 0; i < num_threads; ++i ) {
    // our testing thread's function...
    threads.emplace_back( [=, &locks, &yields, &p](void) -> void {
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

void atomicStructArrayTests( void )
{
  struct Wrapper
  {
    std::atomic_flag lock;
    int data;

    Wrapper( void )
        : lock{ ATOMIC_FLAG_INIT }
        , data{ 0 }
    {
    }
  };

  const int size = 16;
  std::unique_ptr<Wrapper[]> wrappers{ new Wrapper[size] };

  const int num_threads = 4;
  std::vector<std::thread> threads;
  threads.reserve( num_threads );

  std::unique_ptr<std::atomic_int[]> yields{ new std::atomic_int[size] };

  for( int i = 0; i < size; ++i ) {
    new ( yields.get() + i ) std::atomic_int{ 0 };
  }

  const int num_trials = 500000;

  for( int i = 0; i < num_threads; ++i ) {
    // our testing thread's function...
    threads.emplace_back( [=, &yields, &wrappers](void) -> void {
      // create random array index generator
      std::random_device rd;
      std::mt19937 gen{ rd() };
      std::uniform_int_distribution<> range{ 0, size - 1 };

      // iterate the number of trials
      for( int j = 0; j < num_trials; ++j ) {
        int tmp = range( gen );

        // track yields
        while( wrappers[tmp].lock.test_and_set( std::memory_order_acquire ) ) {
          std::atomic_fetch_add( yields.get() + tmp, 1 );
        }

        // increment aligned array
        ++wrappers[tmp].data;

        wrappers[tmp].lock.clear( std::memory_order_release );
      }
    } );
  }

  for( auto& t : threads )
    t.join();

  int sum = 0;

  for( int i = 0; i < size; ++i ) {
    sum += wrappers[i].data;
  }

  assert( sum == num_threads * num_trials );

  for( int i = 0; i < size; ++i ) {
    std::cout << "num. yields for index " << i << " : " << yields[i] << std::endl;
    std::cout << "num. increments for index " << i << " : " << wrappers[i].data << std::endl;
  }
}
