#ifndef TEST_HPP_
#define TEST_HPP_

#include "../globals.hpp"
#include "../helpers/utils.hpp"

/*
 * Dummy class used for testing
 * */
class NonTrivial
{
  public:
  int* data_;

  NonTrivial( void )
  {
    data_ = new int[16];
  }

  NonTrivial( const NonTrivial& other )
  {
    data_ = new int[16];

    for( size_t i = 0; i < 16; ++i )
      data_[i] = other.data_[i];
  }

  NonTrivial( NonTrivial&& other )
  {
    data_ = other.data_;
    other.data_ = nullptr;
  }

  ~NonTrivial( void )
  {
    if( data_ != nullptr ) delete[] data_;
  }

  NonTrivial& operator=( const NonTrivial& other )
  {
    data_ = new int[16];

    for( size_t i = 0; i < 16; ++i )
      data_[i] = other.data_[i];

    return *this;
  }

  NonTrivial& operator=( NonTrivial&& other )
  {
    data_ = other.data_;
    other.data_ = nullptr;
    return *this;
  }
};

/*
 * Testing functions
 * */
template <class U>
void utilsTests( void );
template <>
void utilsTests<int>( void );

template <class U>
void vectorListConstructorTests( void );
template <>
void vectorListConstructorTests<int>( void );

template <class U>
void elementTests( void );
template <>
void elementTests<int>( void );
template <>
void elementTests<NonTrivial>( void );

void atomicArrayTests( void );
void atomicStructArrayTests( void );

#endif // TEST_HPP_
