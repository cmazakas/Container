#ifndef TEST_HPP_
#define TEST_HPP_

#include "../globals.hpp"
#include "../helpers/utils.hpp"

class NonTrivial
{
private:
  int *data_;

public:
  NonTrivial(void) {
    data_ = new int[16];
  }

  ~NonTrivial(void) {
    delete[] data_;
  }
};

template <class U> void utilsTests(void);
template <> void utilsTests<int>(void);

template <class U> void vectorListConstructorTests(void);
template <> void vectorListConstructorTests<int>(void);

template <class U> void elementTests(void);
template <> void elementTests<int>(void);
template <> void elementTests<NonTrivial>(void);

#endif // TEST_HPP_
