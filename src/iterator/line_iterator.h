#ifndef SRC_ITERATOR_LINE_ITERATOR_H_
#define SRC_ITERATOR_LINE_ITERATOR_H_

#include <istream>
#include <string>

class LineReader {
  std::istream& is_;
  std::istream::pos_type start_pos_;

 public:
  explicit LineReader(std::istream& input_stream)
      : is_(input_stream), start_pos_(input_stream.tellg()) {}

  class iterator {
    std::istream* input_stream_ = nullptr;
    std::string line_;

   public:
    using difference_type = std::ptrdiff_t;
    using reference_type = const std::string&;
    using pointer = const std::string*;
    using value_type = std::string;
    using iterator_category = std::input_iterator_tag;

    iterator() = default;

    explicit iterator(std::istream* input_stream)
        : input_stream_(input_stream) {
      ++(*this);
    }

    reference_type operator*() const { return line_; }

    pointer operator->() const { return &line_; }

    iterator& operator++() {
      if (!std::getline(*input_stream_, line_)) {
        *this = iterator{};
      }
      return *this;
    }

    iterator operator++(int) {
      iterator tmp{*this};
      ++*this;
      return tmp;
    }

    bool operator==(const iterator& rhs) const {
      return input_stream_ == rhs.input_stream_;
    }

    bool operator!=(const iterator& rhs) const { return !operator==(rhs); }
  };

  iterator begin() {
    is_.seekg(start_pos_);
    return iterator{&is_};
  }

  iterator end() const { return iterator{}; }
};

namespace std {
template <>
struct iterator_traits<LineReader::iterator> {
  typedef LineReader::iterator::iterator_category iterator_category;
  typedef LineReader::iterator::value_type value_type;
  typedef LineReader::iterator::difference_type difference_type;
  typedef LineReader::iterator::pointer pointer;
  typedef LineReader::iterator::reference_type reference_type;
};
}  // namespace std

#endif  // SRC_ITERATOR_LINE_ITERATOR_H_
