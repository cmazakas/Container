#ifndef ELEMENT_HPP_
#define ELEMENT_HPP_

#include "../globals.hpp"


template <class T> class Element
{
private:
  std::aligned_storage<sizeof(T), alignof(T)> buffer_;
  ElementState state_;

public:
  Element(void);
  Element(const Element& other);
  Element(Element&& other);
  Element& operator=(const Element& other);
  Element& operator=(Element&& other);
  ~Element(void);

  T getData(void) const;
  ElementState getState(void) const;
  void setData(const T& t);
  void setData(T&& t);
};

/*
 * Return the underlying data of the Element
 * */
template <class T> T Element<T>::getData(void) const
{
  return *((T* ) &buffer_);
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
//  const T& other_data = other.getData();
//  setData(other_data);
  new(&buffer_) std::aligned_storage<sizeof(T), alignof(T)>(other.buffer_);
  state_ = other.state_;
}

/*
 * Move constructor
 * */
template <class T> Element<T>::Element(Element&& other)
{
  T&& other_data = std::move(other.getData());
  setData(other_data);
  state_ = other.state_;
}

/*
 * Copy assignment
 * */
template <class T> Element<T>& Element<T>::operator=(const Element& other)
{
  const T& other_data = other.getData();
  *((T* ) &other.buffer_) = other_data;
  return *this;
}

/*
 * Move assignment
 * */
template <class T> Element<T>& Element<T>::operator=(Element&& other)
{
  T&& other_data = other.getData();
  *((T* ) &other.buffer_) = other_data;
  return *this;
}

/*
 * Destructor
 * */
template <class T> Element<T>::~Element(void)
{
  if (state_ == ElementState::Alive) {
    T* t = (T* ) &buffer_;
    t->~T();
  }

  state_ = ElementState::Free;
}

/*
 * Get the current state of the Element
 * */
template <class T> ElementState Element<T>::getState(void) const
{
  return state_;
}

/*
 * Set the state of the element
 * */
template <class T> void Element<T>::setData(const T& t)
{
  new(&buffer_) T(t);
  state_ = ElementState::Alive;
}

template <class T> void Element<T>::setData(T&& t)
{
  new(&buffer_) T(t);
  state_ = ElementState::Alive;
}

#endif // ELEMENT_HPP_
