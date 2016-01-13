#ifndef TEST_HPP_
#define TEST_HPP_

#include <iostream>

class Tetra;

template <class U> void test(void);
template <> void test<int>(void);
template <> void test<Tetra>(void);

void printTitle(const std::string &s);

#endif // TEST_HPP_
