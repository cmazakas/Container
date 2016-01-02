#ifndef ELEMENT_HPP_
#define ELEMENT_HPP_

#include "globals.hpp"

template <class T> class Element
{
  public:
  enum State {
    Default,
    Alive,
    Free,
    Boundary
  };
  union Buffer
  {
    T data;
    Element* next;
    
    // write destructor
  };

  private:
  State state_;
  Buffer buffer_;

  public:
  Element(void)
  {
    SetState(State::Default);
    buffer_.next = nullptr;
  }

  ~Element(void)
  {
    if (state_ == State::Alive) {
      buffer_.data.~T();
    }
  }

  void SetState(State state)
  {
    state_ = state;
  }

  void SetData(T&& data)
  {
    buffer_.data = data;
    SetState(State::Alive);
  }

  void SetNext(Element* next)
  {
    assert(GetState() == State::Free || GetState() == State::Boundary);
    buffer_.next = next;
  }

  State GetState(void) const
  {
    return state_;
  }

  T GetData(void) const
  {
    assert(state_ == State::Alive);
    return buffer_.data;
  }

  T& GetDataByReference(void)
  {
    assert(state_ == State::Alive);
    return buffer_.data;
  }

  Element* GetNext(void)
  {
    // assert(state_ != State::Alive);
    return buffer_.next;
  }

  template <class... Args> void emplace(Args&&... args)
  {
    assert(state_ != State::Alive);
    new (&buffer_.data) T{ args... };
    state_ = State::Alive;
  }
};

#endif // ELEMENT_HPP_
