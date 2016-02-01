#include "./test.hpp"
#include "../element/element.hpp"

/*
 * TODO: Make sure exception safety is added to testing suite
 * */

/*
 * We should have some basic getters and setters
 * Afer all, using generic storage does not yield
 * the prettiest native interface.
 * */

template <> void elementTests<int>( void )
{
  /*
   * We should be able to create an element that
   * contains both state and a value
   * */
  {
    const int value = 1337;

    Element<int> element;
    element.emplace( value );

    assert( element.getState() == ElementState::Alive );
    assert( element.getData() == value );

    element.clear();
    assert( element.getState() == ElementState::Free );

    Element<int> other_element;

    element.setNext( &other_element );
    other_element.setNext( &element );

    assert( element.getNext() == &other_element );
    assert( element.getState() == ElementState::Free );
    assert( other_element.getState() == ElementState::Free );
    assert( other_element.getNext() == &element );
  }
}

/*
 * This is a test primarily created to prove things
 * such as rule of 5 compliance with non-trivial types
 * as well as tests for the strong exception guarantee
 * */

/*
 * As a side note, I'm not decent enough to overload
 * the new operator so I used valgrind to manually
 * track the number of allocations and frees
 * */

template <> void elementTests<NonTrivial>( void )
{
  /*
   * It should be default constructible
   * */
  {
    Element<NonTrivial> element;
    element.emplace(); // +1 allocation = 1
  }

  /*
   * It should be move-constructible
   * */
  {
    Element<NonTrivial> element;
    element.emplace(); // +1 allocation = 2

    Element<NonTrivial> other( std::move( element ) );
  }

  /*
   * It should be copy-constructible
   * */
  {
    Element<NonTrivial> element;
    element.emplace(); // +1 allocation = 3

    Element<NonTrivial> other( (const Element<NonTrivial>&)element ); // +1 allocation = 4
  }

  /*
   * It should be move-assignable
   * */
  {
    Element<NonTrivial> element;
    element.emplace(); // +1 allocation = 5

    Element<NonTrivial> other = std::move( element );
  }

  /*
   * It should be copy-assignable
   * */
  {
    Element<NonTrivial> element;
    element.emplace(); // +1 allocation = 6

    Element<NonTrivial> other = element; // +1 allocation = 7
  }

  /*
   * Ensure with memory tracker that there are
   * 7 allocations and 7 frees
   * */
}
