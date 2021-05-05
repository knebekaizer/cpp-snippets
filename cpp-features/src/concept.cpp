//! \file

#include <concepts>

struct ThreadPool {
  void enqueue( std::Invocable auto fun );
};

