#include "./test.hpp"
#include "../element/element.hpp"

template <> void elementTests<int>(void)
{
  /*
   * We should be able to create an element that
   * contains both state and a value
   *
   * We also enforce compliance with the rule of 5
   *
   * TODO: Make sure exception-safety and non-trivial types are covered
   * */
  {
    const int value = 1337;

    /* Default construction
     * */
    Element<int> element;
    element.emplace(value);


  }
}

template <> void elementTests<NonTrivial>(void)
{
  /*
   * It should be move-constructible
   * */
  {
    Element<NonTrivial> element;
    element.emplace();

    Element<NonTrivial> other(std::move(element));
  }
}
