#ifndef SRC_TEMPLATE_DISTANCE_H_
#define SRC_TEMPLATE_DISTANCE_H_

#include <iterator>

template <typename Iterator>
struct Category {
  using type = typename Iterator::iterator_category;
};

template <typename Iterator>
struct Category<Iterator*> {
  using type = std::random_access_iterator_tag;
};

template <typename Iterator>
auto DistanceImpl(Iterator begin, Iterator end, std::input_iterator_tag) {
  typename Iterator::difference_type i = 0;
  for (auto iter = begin; iter != end; ++iter) {
    i++;
  }
  return i;
}

template <typename Iterator>
auto DistanceImpl(Iterator begin, Iterator end,
                  std::random_access_iterator_tag) {
  return end - begin;
}

template <typename Iterator>
auto Distance(Iterator begin, Iterator end) {
  return DistanceImpl(begin, end, typename Category<Iterator>::type{});
}

#endif  // SRC_TEMPLATE_DISTANCE_H_
