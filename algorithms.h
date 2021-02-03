#include <functional>
#include <iterator>

#ifndef ALGORITHMS_H_
#define ALGORITHMS_H_

template<typename ForwardIt>
bool IsSorted(ForwardIt begin, ForwardIt end) {
  if (begin != end) {
    for (ForwardIt element = begin, next_element = ++begin;
         next_element != end; element++, next_element++) {
      if (*next_element < *element) {
        return false;
      }
    }
  }
  return true;
}

template<typename ForwardIt>
ForwardIt MaxElement(ForwardIt begin, ForwardIt end) {
  if (begin == end) {
    return end;
  }
  ForwardIt max_element = begin;
  for (ForwardIt iterator = begin; iterator != end; iterator++) {
    max_element = (*max_element < *iterator) ? iterator : max_element;
  }
  return max_element;
}

template<typename ForwardIt>
typename std::iterator_traits<ForwardIt>::value_type Accumulate(
    ForwardIt begin, ForwardIt end) {
  using value_type = typename std::iterator_traits<ForwardIt>::value_type;
  if (begin == end) {
    return value_type();
  }
  value_type result{};
  for (ForwardIt iterator = begin; iterator != end; ++iterator) {
    result += *iterator;
  }
  return result;
}

template<typename ForwardIt, typename UnaryPredicate>
int CountIf(ForwardIt begin, ForwardIt end, UnaryPredicate predicate) {
  if (begin == end) {
    return 0;
  }
  int counter = 0;
  for (ForwardIt iterator = begin; iterator != end; iterator++) {
    if (predicate(*iterator)) {
      counter++;
    }
  }
  return counter;
}

template<typename ForwardIt>
int CountIfNot(ForwardIt begin, ForwardIt end, bool (*p_function)(const
    typename std::iterator_traits<ForwardIt>::value_type&)) {
  if (begin == end) {
    return 0;
  }
  int counter = 0;
  for (ForwardIt iterator = begin; iterator != end; iterator++) {
    if (!p_function(*iterator)) {
      counter++;
    }
  }
  return counter;
}

template<typename ForwardIt>
ForwardIt FindIf(ForwardIt begin, ForwardIt end, std::function<bool(const
    typename std::iterator_traits<ForwardIt>::value_type&)> function) {
  if (begin == end) {
    return end;
  }
  for (ForwardIt iterator = begin; iterator != end; iterator++) {
    if (function(*iterator)) {
      return iterator;
    }
  }
  return end;
}

#endif  // ALGORITHMS_H_
