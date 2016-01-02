#ifndef CONTAINERITERATOR_HPP_
#define CONTAINERITERATOR_HPP_

#include "globals.hpp"
#include "container.hpp"

template <class T> void AssertIsBoundary(Element<T>* elem)
{
  assert(elem->GetState() == Element<T>::State::Boundary);
}

template <class T> void AssertIsBoundary(typename Element<T>::State state)
{
  assert(state == Element<T>::State::Boundary);
}

template <class T> class Container; // forward declaration

template <class T> class ContainerIterator
{
  private:
  friend class Container<T>;

  Container<T>& container_;
  // data
  Element<T>* element_;
  typename Element<T>::State state_;
  void (ContainerIterator<T>::*forward_)(void);
  void (ContainerIterator<T>::*backward_)(void);
  void (ContainerIterator<T>::*stay_)(void);

  // methods
  void AdvancePointer(void);
  void RetractPointer(void);
  void StayInPlace(void);
  void step(
      void (ContainerIterator<T>::*forward_or_stay)(void),
      void (ContainerIterator<T>::*stay_or_backward)(void),
      void (ContainerIterator<T>::*forward_or_backward)(void)
  );
  void FindFirstAlive(void);
  void FindNextAlive(void);
  void FindPrevAlive(void);

  public:
  ContainerIterator(Container<T>& container, Element<T>* element);

  Element<T>* get(void);

  void operator++(void);
  void operator--(void);
  ContainerIterator operator-(size_t n);
  ContainerIterator operator+(size_t n);
  T& operator*(void);
  bool operator==(ContainerIterator<T>&& other);
  bool operator!=(ContainerIterator<T>&& other);
};

/* Implementations */

template <class T>
ContainerIterator<T>::ContainerIterator(Container<T>& container, Element<T>* element)
    : container_(container)
{
  element_ = element;
  assert(element_);

  state_ = element_->GetState();
  assert(state_ != Element<T>::State::Default);
  
  forward_ = &ContainerIterator<T>::AdvancePointer;
  backward_ = &ContainerIterator<T>::RetractPointer;
  stay_ = &ContainerIterator<T>::StayInPlace;
}

template <class T> void ContainerIterator<T>::AdvancePointer(void)
{
  ++element_;
  state_ = element_->GetState();
}

template <class T> void ContainerIterator<T>::RetractPointer(void)
{
  --element_;
  state_ = element_->GetState();
}

template <class T> void ContainerIterator<T>::StayInPlace(void)
{
  return;
}

template <class T> void ContainerIterator<T>::step(
    void (ContainerIterator<T>::*forward_or_stay)(void),
    void (ContainerIterator<T>::*stay_or_backward)(void),
    void (ContainerIterator<T>::*forward_or_backward)(void)
)
{
  // boundary is a bit more complex than non-boundary...
  if (state_ == Element<T>::State::Boundary) {
    
    // will either take step forward or stay in place    
    if (element_ == container_.first_) {
      (this->*forward_or_stay)();
      
    // will either stay in place or take a 
    } else if (element_ == container_.last_) {
      (this->*stay_or_backward)();
      
    // will either take step forward or backward (i.e. hop blocks)
    } else {
      AssertIsBoundary<T>(element_);
      AssertIsBoundary<T>(state_);
      
      auto next = element_->GetNext();
      AssertIsBoundary(next);
      assert(next->GetNext() == element_); // assert the 2 way association
      
      element_ = next;
      (this->*forward_or_backward)();
    }
    
  // will either take step forward or backward
  } else {
    (this->*forward_or_backward)();
  }
}

template <class T> void ContainerIterator<T>::FindNextAlive(void)
{
  step(forward_, stay_, forward_);

  while (state_ != Element<T>::State::Alive && element_ != container_.last_) {
    step(forward_, stay_, forward_);
  }
}

template <class T> void ContainerIterator<T>::FindFirstAlive(void)
{
  if (element_->GetState() != Element<T>::State::Alive)
    FindNextAlive();
}

template <class T> void ContainerIterator<T>::FindPrevAlive(void)
{
  step(stay_, backward_, backward_);
  
  while (state_ != Element<T>::State::Alive && element_ != container_.first_) {
    step(stay_, backward_, backward_);
  }
}

template <class T> Element<T>* ContainerIterator<T>::get(void)
{
  return element_;
}

template <class T> void ContainerIterator<T>::operator++(void)
{
  FindNextAlive();
}

template <class T> void ContainerIterator<T>::operator--(void)
{
  FindPrevAlive();
}

template <class T> ContainerIterator<T> ContainerIterator<T>::operator-(size_t n)
{
  ContainerIterator<T> tmp = ContainerIterator<T>(container_, element_);
  for (auto i = 0; i < n; ++i) {
    tmp.FindPrevAlive();
  }
  
  return tmp;
}

template <class T> ContainerIterator<T> ContainerIterator<T>::operator+(size_t n)
{
  for (auto i = 0; i < n; ++i) {
    FindNextAlive();
  }
}

template <class T> T& ContainerIterator<T>::operator*(void)
{
  assert(element_->GetState() == Element<T>::State::Alive);

  return element_->GetDataByReference();
}

template <class T> bool ContainerIterator<T>::operator==(ContainerIterator<T>&& other)
{
  return element_ == other.get();
}

template <class T> bool ContainerIterator<T>::operator!=(ContainerIterator<T>&& other)
{
  return element_ != other.get();
}

#endif // CONTAINERITERATOR_HPP_
