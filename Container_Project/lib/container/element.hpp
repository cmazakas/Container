#ifndef ELEMENT_HPP_
#define ELEMENT_HPP_

#include "globals.hpp"

template <class T> class Element
{
  public:
  enum State {
    Default,    // 0
    Alive,      // 1
    Free,       // 2
    Boundary    // 3
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
    setState(State::Free);
    buffer_.next = nullptr;
  }

  ~Element(void)
  {
    if (state_ == State::Alive) {
      buffer_.data.~T();
    }
    
    buffer_.next = nullptr;
    state_ = Free;
  }

  void setState(State state)
  {
    state_ = state;
  }

  void setData(T&& data)
  {
    buffer_.data = data;
    setState(State::Alive);
  }

  /*
   * 
   * setNext() does NOT set the state of the element
   * because it's a valid state for either a boundary
   * or a free element to contain a next pointer
   * so we leave it up to the caller to manage state
   */
  void setNext(Element* next)
  {
    if (next)
      assert(next->getState() == State::Free || next->getState() == State::Boundary);
    else
      assert(next == nullptr);
   
    if (state_ == Alive) {
      buffer_.data.~T();
    }
    
    buffer_.next = next;
  }

  void setNextAndState(Element *next, State state) 
  {
    setNext(next);
    setState(state);
  }

  State getState(void) const
  {
    return state_;
  }

  T getData(void) const
  {
    assert(state_ == State::Alive);
    return buffer_.data;
  }

  T& getDataByReference(void)
  {
    assert(state_ == State::Alive);
    return buffer_.data;
  }

  Element* getNext(void)
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
