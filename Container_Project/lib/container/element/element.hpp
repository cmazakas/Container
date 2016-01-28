#ifndef ELEMENT_HPP_
#define ELEMENT_HPP_

#include <functional>

#include "../globals.hpp"
#include "../helpers/utils.hpp"

/*
 * Necessary compile-time construct.
 * If T is less than the size of a pointer,
 * we have issues when we try to set the
 * storage of the Element to a pointer...
 * */
template <class T> constexpr size_t getSize()
{
  return sizeof( T ) < sizeof( T* ) ? sizeof( T* ) : sizeof( T );
}

/*
 * Our Element class which contains either a type T
 * or a pointer to another Element
 * */
template <class T> class Element
{
  private:
  typename std::aligned_storage<getSize<T>(), alignof( T )>::type data_[1];
  ElementState state_;
  std::atomic_flag lock_;

  public:
  Element( void );
  Element( const Element& other );
  Element( Element&& other );
  Element& operator=( const Element& other );
  Element& operator=( Element&& other );
  ~Element( void );

  void testFunction( const Element& other )
  {
    const T& other_data = *reinterpret_cast<const T*>( other.data_ );
    new ( data_ ) T{ other_data };
    state_ = ElementState::Alive;
  }

  template <class... Args> void emplace( Args&&... args );
  void setNext( Element* next );
  void makeBoundary( void );
  void clear( void );

  T getData( void ) const;
  Element* getNext( void );
  ElementState getState( void ) const;
};

/*
 * Default constructor
 * */
template <class T> Element<T>::Element( void )
{
  state_ = ElementState::Free;
}

/*
 * Copy constructor
 * */
template <class T> Element<T>::Element( const Element& other )
{
  auto mf = std::mem_fn( &Element::testFunction );
  Utils::spinLockExecutor<decltype( mf )>( mf, lock_ );
  const T& other_data = *reinterpret_cast<const T*>( other.data_ );
  new ( data_ ) T{ other_data };
  state_ = ElementState::Alive;
}

/*
 * Move constructor
 * */
template <class T> Element<T>::Element( Element&& other )
{
  T&& other_data = std::move( *reinterpret_cast<T*>( other.data_ ) );
  new ( data_ ) T{ std::forward<T>( other_data ) };
  state_ = ElementState::Alive;

  other.state_ = ElementState::Free;
}

/*
 * Copy assignment
 * */
template <class T> Element<T>& Element<T>::operator=( const Element& other )
{
  *reinterpret_cast<T*>( data_ ) = *reinterpret_cast<T*>( other.data_ );
  state_ = ElementState::Alive;
}

/*
 * Move assignment
 * */
template <class T> Element<T>& Element<T>::operator=( Element&& other )
{
  T&& other_data = std::move( *reinterpret_cast<T*>( other.data_ ) );
  *reinterpret_cast<T*>( data_ ) = std::forward<T>( other_data );
  state_ = ElementState::Alive;
}

/*
 * Destructor
 * */
template <class T> Element<T>::~Element( void )
{
  if( state_ == ElementState::Alive ) reinterpret_cast<const T*>( data_ )->~T();
}

/*
 * Construct an element in-place
 * */
template <class T> template <class... Args> void Element<T>::emplace( Args&&... args )
{
  assert( state_ == ElementState::Free );
  new ( data_ ) T{ std::forward<Args>( args )... };
  state_ = ElementState::Alive;
}

/*
 * Assign an Element* value to the storage of an Element
 * */
template <class T> void Element<T>::setNext( Element* next )
{
  assert( state_ == ElementState::Free || state_ == ElementState::Boundary );
  new ( data_ ) Element*( next );
  assert( state_ == ElementState::Free || state_ == ElementState::Boundary );
}

/*
 * Change an element's state to a boundary
 * Will not clear() the Element
 * */
template <class T> void Element<T>::makeBoundary( void )
{
  state_ = ElementState::Boundary;
}

/*
 * Obtain a copy of the value T stored in the Element
 * */
template <class T> T Element<T>::getData( void ) const
{
  assert( state_ == ElementState::Alive );
  return *reinterpret_cast<const T*>( data_ );
}

/*
 * Obtain the value of the "next" pointer stored in the Element
 * */
template <class T> Element<T>* Element<T>::getNext( void )
{
  assert( state_ == ElementState::Boundary || state_ == ElementState::Free );
  Element* next = *reinterpret_cast<Element**>( data_ );
  return next;
}

/*
 * Get the current state of the Element
 * */
template <class T> ElementState Element<T>::getState( void ) const
{
  return state_;
}

/*
 * Clears the current Element
 * */
template <class T> void Element<T>::clear( void )
{
  assert( state_ == ElementState::Alive );
  reinterpret_cast<const T*>( data_ )->~T();
  state_ = ElementState::Free;
}

#endif // ELEMENT_HPP_
