#ifndef TEST_HPP_
#define TEST_HPP_

#include "../globals.hpp"
#include "../helpers/utils.hpp"

//class Tetra;
//
//template <class U> void test(void);
//template <> void test<int>(void);
//template <> void test<Tetra>(void);

template <class U> void utilsTests(void);
template <> void utilsTests<int>(void);

template <class U> void vectorListConstructorTests(void);
template <> void vectorListConstructorTests<int>(void);

template <class U> void elementTests(void);
template <> void elementTests<int>(void);

#endif // TEST_HPP_
