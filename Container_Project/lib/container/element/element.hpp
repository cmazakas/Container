#ifndef ELEMENT_HPP_
#define ELEMENT_HPP_

#include "../globals.hpp"


template <class T> class Element
{
private:
  typename std::aligned_storage<sizeof(T), alignof(T)>::type data_[1];
  ElementState state_;

public:
  Element(void);
  Element(const Element& other);
  Element(Element&& other);
  Element& operator=(const Element& other);
  Element& operator=(Element&& other);
  ~Element(void);

  template <class ...Args> void emplace(Args&&... args);
};

/*
 * Construct an element in-place
 * */
template <class T> template <class ...Args> void Element<T>::emplace(Args&&... args)
{
  assert(state_ == ElementState::Free);
  new(data_) T(std::forward<Args>(args)...);
  state_ = ElementState::Alive;
}

/*
 * Default constructor
 * */
template <class T> Element<T>::Element(void)
{
  state_ = ElementState::Free;
}

/*
 * Copy constructor
 * */
template <class T> Element<T>::Element(const Element& other)
{
  const T& other_data = *reinterpret_cast<const T*>(other.data_);
  new(data_) T(other_data);
  state_ = ElementState::Alive;
}

/*
 * Move constructor
 * */
template <class T> Element<T>::Element(Element&& other)
{
  T&& other_data = std::move(*reinterpret_cast<T*>(other.data_));
  new(data_) T(std::forward<T>(other_data));
  state_ = ElementState::Alive;

  other.state_ = ElementState::Free;
}

/*
 * Destructor
 * */
template <class T> Element<T>::~Element(void)
{
  if (state_ == ElementState::Alive)
    reinterpret_cast<const T*>(data_)->~T();
}

#endif // ELEMENT_HPP_
