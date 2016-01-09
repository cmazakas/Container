#ifndef CONTAINERITERATOR_HPP_
#define CONTAINERITERATOR_HPP_

#include "globals.hpp"
#include "container.hpp"

template <class T> void assertIsBoundary(Element<T>* elem)
{
  assert(elem->getState() == Element<T>::State::Boundary);
}

template <class T> void assertIsBoundary(typename Element<T>::State state)
{
  assert(state == Element<T>::State::Boundary);
}

template <class T> class Container; // forward declaration

template <class T> class ContainerIterator
{
  private:
  friend class Container<T>;

  Container<T>& container_;
  Element<T>* element_;
  
  // personally, I hate using runtime constructs
  // to handle program logic but I've yet to figure
  // out a way to get the behavior I want without them
  // so function pointers are necessary for now
  void (ContainerIterator<T>::*forward_)(void);
  void (ContainerIterator<T>::*backward_)(void);
  void (ContainerIterator<T>::*stay_)(void);

  // methods
  void advancePointer(void);
  void retractPointer(void);
  void stayInPlace(void);
  void step(
      void (ContainerIterator<T>::*forward_or_stay)(void),
      void (ContainerIterator<T>::*stay_or_backward)(void),
      void (ContainerIterator<T>::*forward_or_backward)(void)
  );
  void findFirstAlive(void);
  void findNextAlive(void);
  void findPrevAlive(void);

  public:
  ContainerIterator(Container<T>& container, Element<T>* element);

  Element<T>* get(void);
  typename Element<T>::State getState(void) const;

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

  assert(element_->getState() != Element<T>::State::Default);
  
  forward_ = &ContainerIterator<T>::advancePointer;
  backward_ = &ContainerIterator<T>::retractPointer;
  stay_ = &ContainerIterator<T>::stayInPlace;
}

template <class T> void ContainerIterator<T>::advancePointer(void)
{
  ++element_;
}

template <class T> void ContainerIterator<T>::retractPointer(void)
{
  --element_;
}

template <class T> void ContainerIterator<T>::stayInPlace(void)
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
  if (element_->getState() == Element<T>::State::Boundary) {
    
    // will either take step forward or stay in place    
    if (element_ == container_.first_) {
      (this->*forward_or_stay)();
      
    // will either stay in place or take a 
    } else if (element_ == container_.last_) {
      (this->*stay_or_backward)();
      
    // will either take step forward or backward (i.e. hop blocks)
    } else {
      assertIsBoundary<T>(element_);
      
      auto next = element_->getNext();
      assertIsBoundary(next);
      assert(next->getNext() == element_); // assert the 2 way association
      
      element_ = next;
      (this->*forward_or_backward)();
    }
    
  // will either take step forward or backward
  } else {
    (this->*forward_or_backward)();
  }
}

template <class T> void ContainerIterator<T>::findNextAlive(void)
{
  step(forward_, stay_, forward_);

  while (element_->getState() != Element<T>::State::Alive && element_ != container_.last_) {
    step(forward_, stay_, forward_);
  }
}

template <class T> void ContainerIterator<T>::findFirstAlive(void)
{
  if (element_->getState() != Element<T>::State::Alive)
    findNextAlive();
}

template <class T> void ContainerIterator<T>::findPrevAlive(void)
{
  step(stay_, backward_, backward_);
  
  while (element_->getState() != Element<T>::State::Alive && element_ != container_.first_) {
    step(stay_, backward_, backward_);
  }
}

template <class T> Element<T>* ContainerIterator<T>::get(void)
{
  return element_;
}

template <class T> typename Element<T>::State ContainerIterator<T>::getState(void) const {
  return element_->getState();
}

template <class T> void ContainerIterator<T>::operator++(void)
{
  findNextAlive();
}

template <class T> void ContainerIterator<T>::operator--(void)
{
  findPrevAlive();
}

template <class T> ContainerIterator<T> ContainerIterator<T>::operator-(size_t n)
{
  ContainerIterator<T> tmp = ContainerIterator<T>(container_, element_);
  for (size_t i = 0; i < n; ++i) {
    tmp.findPrevAlive();
  }
  
  return tmp;
}

template <class T> ContainerIterator<T> ContainerIterator<T>::operator+(size_t n)
{
  for (size_t i = 0; i < n; ++i) {
    findNextAlive();
  }
}

template <class T> T& ContainerIterator<T>::operator*(void)
{
  assert(element_->getState() == Element<T>::State::Alive);

  return element_->getDataByReference();
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
