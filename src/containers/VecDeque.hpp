//
// Created by Matt Blair on 2019-03-04.
//

#pragma once

#include <cassert>
#include <vector>

namespace stock {

/**
 * A double-ended queue implemented with a growable ring buffer.
 *
 * This queue has `O(1)` amortized inserts and removals from both ends. Note that the current implementation does not
 * construct or destroy objects in `push` or `pop` operations, it only performs assignment.
 *
 * @tparam T The contained type.
 */
template<typename T>
class VecDeque {

public:

  explicit VecDeque(size_t capacity = 0);

  bool empty() const;

  bool full() const;

  size_t size() const;

  size_t capacity() const;

  void clear();

  T& front();

  const T& front() const;

  T& back();

  const T& back() const;

  void pop_front();

  void pop_back();

  void push_front(const T& value);

  void push_back(const T& value);

protected:

  using Container = std::vector<T>;

  Container m_container;
  size_t m_begin = 0;
  size_t m_end = 0;

  void grow_if_needed();
};

template<typename T>
VecDeque<T>::VecDeque(size_t capacity) {
  m_container.resize(capacity + 1);
}

template<typename T>
bool VecDeque<T>::empty() const {
  return m_begin == m_end;
}

template<typename T>
bool VecDeque<T>::full() const {
  return m_container.size() - size() == 1;
}

template<typename T>
size_t VecDeque<T>::size() const {
  if (m_end >= m_begin) {
    return m_end - m_begin;
  }
  return (m_container.size() - m_begin) + m_end;
}

template<typename T>
size_t VecDeque<T>::capacity() const {
  return m_container.size() - 1;
}

template<typename T>
void VecDeque<T>::clear() {
  m_container.clear();
  m_begin = 0;
  m_end = m_begin;
}

template<typename T>
T& VecDeque<T>::front() {
  assert(m_begin != m_end);
  return m_container[m_begin];
}

template<typename T>
const T& VecDeque<T>::front() const {
  assert(m_begin != m_end);
  return m_container[m_begin];
}

template<typename T>
T& VecDeque<T>::back() {
  assert(m_begin != m_end);
  if (m_end == 0) {
    return m_container.back();
  }
  return m_container[m_end - 1];
}

template<typename T>
const T& VecDeque<T>::back() const {
  assert(m_begin != m_end);
  if (m_end == 0) {
    return m_container.back();
  }
  return m_container[m_end - 1];
}

template<typename T>
void VecDeque<T>::pop_front() {
  assert(m_begin != m_end);
  if (++m_begin == m_container.size()) {
    m_begin = 0;
  }
}

template<typename T>
void VecDeque<T>::pop_back() {
  assert(m_begin != m_end);
  if (m_end == 0) {
    m_end = m_container.size();
  }
  m_end--;
}

template<typename T>
void VecDeque<T>::push_front(const T& value) {
  grow_if_needed();
  if (m_begin == 0) {
    m_begin = m_container.size();
  }
  m_begin--;
  m_container[m_begin] = value;
}

template<typename T>
void VecDeque<T>::push_back(const T& value) {
  grow_if_needed();
  m_container[m_end] = value;
  m_end++;
  if (m_end == m_container.size()) {
    m_end = 0;
  }
}

template<typename T>
void VecDeque<T>::grow_if_needed() {
  if (full()) {
    // Create an expanded container to replace our full one.
    Container expanded(m_container.size() * 2);
    // Move existing elements into new container.
    size_t write = 0;
    auto read = m_begin;
    while (read != m_end) {
      if (read == m_container.size()) {
        read = 0;
      } else {
        expanded[write] = std::move(m_container[read]);
        read++;
        write++;
      }
    }
    // Update iterators.
    m_begin = 0;
    m_end = write;
    // Swap containers.
    m_container.swap(expanded);
  }
}

} // namespace stock
