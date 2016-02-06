#ifndef GLOBALS_HPP_
#define GLOBALS_HPP_

#define INITIAL_BLOCK_SIZE 16
#define BLOCK_INCREMENT 16

#include <vector>
#include <utility>
#include <memory>
#include <exception>
#include <cassert>
#include <iostream>
#include <functional>
#include <atomic>
#include <thread>
#include <unordered_map>
#include <iomanip>
#include <type_traits>
#include <random>

template <class T>
class Element;

template <class T>
using BlockPtr = std::unique_ptr<Element<T> []>;

typedef std::unique_ptr<std::atomic_flag[]> LockPtr;

template <class T>
using Block = std::tuple<BlockPtr<T>, LockPtr, size_t>;

template <class T>
using Blocks = std::vector<Block<T> >;

template <class T>
using FreeLists = std::unordered_map<std::thread::id, Element<T>*>;

enum ElementState { Free, Alive, Boundary };

#endif // GLOBALS_HPP_
