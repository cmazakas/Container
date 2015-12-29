#ifndef CONTAINERITERATOR_HPP_
#define CONTAINERITERATOR_HPP_

#include "globals.hpp"
#include "container.hpp"

template <class T> class Container;

template <class T> class ContainerIterator
{
  private:
  friend class Container<T>;

  Container<T>& container_;
  Element<T>* element_;

  void FindFirstAlive(void);
  void FindNextAlive(void);
  void FindPrevAlive(void);

  public:
  ContainerIterator(Container<T>& container, Element<T>* element);

  Element<T>* get(void);

  void operator++(void);
  void operator--(void);
  T& operator*(void);
  bool operator==(ContainerIterator<T>&& other);
  bool operator!=(ContainerIterator<T>&& other);
};

/* Implementations */

template <class T> void ContainerIterator<T>::FindFirstAlive(void)
{
  if (element_->GetState() != Element<T>::State::Alive)
    FindNextAlive();
}

template <class T> void ContainerIterator<T>::FindNextAlive(void)
{
  auto state = Element<T>::State::Default;

  while (state != Element<T>::State::Alive) {
    if (state == Element<T>::State::Boundary) {
      if (element_ == container_.first_) {
        ++element_;
        state = element_->GetState();
        continue;
      }

      auto next = element_->GetNext();

      if (next == nullptr) {
        assert(element_ == container_.last_);
        return;
      }

      assert(next->GetState() == Element<T>::State::Boundary);

      element_ = next + 1;
      state = element_->GetState();
      continue;
    }

    ++element_;
    state = element_->GetState();
  }
}

template <class T> void ContainerIterator<T>::FindPrevAlive(void)
{
  auto state = Element<T>::State::Default;

  while (state != Element<T>::State::Alive) {
    if (state == Element<T>::State::Boundary) {
      if (element_ == container_.last_) {
        --element_;
        state = element_->GetState();
        continue;
      }

      auto next = element_->GetNext();

      if (next == nullptr) {
        assert(element_ == container_.first_);
        return;
      }

      assert(next->GetState() == Element<T>::State::Boundary);

      element_ = next - 1;
      state = element_->GetState();
      continue;
    }

    --element_;
    state = element_->GetState();
  }
}

template <class T>
ContainerIterator<T>::ContainerIterator(Container<T>& container, Element<T>* element)
    : container_(container)
{
  element_ = element;
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
