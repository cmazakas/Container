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
    Element<int> element;
    assert(element.getState() == ElementState::Free);

    const int value = 1337;

    element.setData(value);
    assert(element.getData() == value);
    assert(element.getState() == ElementState::Alive);

    Element<int> other((const Element<int>&) element);
    assert(element.getData() == value);
    assert(element.getState() == ElementState::Alive);
    assert(other.getData() == value);
    assert(other.getState() == ElementState::Alive);

    Element<int> moved((Element<int>&& ) std::move(element));
    assert(moved.getData() == value);
    assert(moved.getState() == ElementState::Alive);

    Element<int> copy_assigned = (const Element<int>& ) moved;
    assert(moved.getData() == value);
    assert(moved.getState() == ElementState::Alive);
    assert(copy_assigned.getData() == value);
    assert(copy_assigned.getState() == ElementState::Alive);

    Element<int> move_assigned = (Element<int>&& ) copy_assigned;
    assert(move_assigned.getData() == value);
    assert(move_assigned.getState() == ElementState::Alive);
  }
}
