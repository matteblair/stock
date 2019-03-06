//
// Created by Matt Blair on 2019-03-04.
//

#pragma once

#include <cassert>
#include <cstddef>
#include <memory>

namespace stock {

/**
 * A double-ended queue implemented with a growable ring buffer.
 *
 * This queue has `O(1)` amortized inserts and removals from both ends. Note that the current implementation does not
 * construct or destroy objects in `push` or `pop` operations, it only performs assignment.
 *
 * @tparam T The contained type.
 */
template<typename T, class Allocator = std::allocator<T>>
class VecDeque {

public:

  explicit VecDeque(size_t capacity = 0);

  ~VecDeque();

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

  void push_front(T&& value);

  void push_back(const T& value);

  void push_back(T&& value);

protected:

  T* m_buffer = nullptr;
  size_t m_capacity = 0;
  size_t m_begin = 0;
  size_t m_end = 0;

  void grow_if_needed();
};

template<typename T, class Allocator>
VecDeque<T, Allocator>::VecDeque(size_t capacity) {
  m_capacity = capacity + 1;
  m_buffer = Allocator().allocate(m_capacity);
}

template<typename T, class Allocator>
VecDeque<T, Allocator>::~VecDeque() {
  Allocator().deallocate(m_buffer, m_capacity);
}

template<typename T, class Allocator>
bool VecDeque<T, Allocator>::empty() const {
  return m_begin == m_end;
}

template<typename T, class Allocator>
bool VecDeque<T, Allocator>::full() const {
  return m_capacity - size() == 1;
}

template<typename T, class Allocator>
size_t VecDeque<T, Allocator>::size() const {
  if (m_end >= m_begin) {
    return m_end - m_begin;
  }
  return (m_capacity - m_begin) + m_end;
}

template<typename T, class Allocator>
size_t VecDeque<T, Allocator>::capacity() const {
  return m_capacity - 1;
}

template<typename T, class Allocator>
void VecDeque<T, Allocator>::clear() {
  auto read = m_begin;
  while (read != m_end) {
    if (read == m_capacity) {
      read = 0;
    } else {
      m_buffer[read].~T();
      read++;
    }
  }
  m_begin = 0;
  m_end = m_begin;
}

template<typename T, class Allocator>
T& VecDeque<T, Allocator>::front() {
  assert(m_begin != m_end);
  return m_buffer[m_begin];
}

template<typename T, class Allocator>
const T& VecDeque<T, Allocator>::front() const {
  assert(m_begin != m_end);
  return m_buffer[m_begin];
}

template<typename T, class Allocator>
T& VecDeque<T, Allocator>::back() {
  assert(m_begin != m_end);
  if (m_end == 0) {
    return m_buffer[m_capacity - 1];
  }
  return m_buffer[m_end - 1];
}

template<typename T, class Allocator>
const T& VecDeque<T, Allocator>::back() const {
  assert(m_begin != m_end);
  if (m_end == 0) {
    return m_buffer[m_capacity - 1];
  }
  return m_buffer[m_end - 1];
}

template<typename T, class Allocator>
void VecDeque<T, Allocator>::pop_front() {
  assert(m_begin != m_end);
  if (++m_begin == m_capacity) {
    m_begin = 0;
  }
}

template<typename T, class Allocator>
void VecDeque<T, Allocator>::pop_back() {
  assert(m_begin != m_end);
  if (m_end == 0) {
    m_end = m_capacity;
  }
  m_end--;
}

template<typename T, class Allocator>
void VecDeque<T, Allocator>::push_front(const T& value) {
  grow_if_needed();
  if (m_begin == 0) {
    m_begin = m_capacity;
  }
  m_begin--;
  new(&m_buffer[m_begin]) T(value);
}

template<typename T, class Allocator>
void VecDeque<T, Allocator>::push_front(T&& value) {
  grow_if_needed();
  if (m_begin == 0) {
    m_begin = m_capacity;
  }
  m_begin--;
  new(&m_buffer[m_begin]) T(std::move(value));
}

template<typename T, class Allocator>
void VecDeque<T, Allocator>::push_back(const T& value) {
  grow_if_needed();
  new(&m_buffer[m_end]) T(value);
  m_end++;
  if (m_end == m_capacity) {
    m_end = 0;
  }
}

template<typename T, class Allocator>
void VecDeque<T, Allocator>::push_back(T&& value) {
  grow_if_needed();
  new(&m_buffer[m_end]) T(std::move(value));
  m_end++;
  if (m_end == m_capacity) {
    m_end = 0;
  }
}

template<typename T, class Allocator>
void VecDeque<T, Allocator>::grow_if_needed() {
  if (full()) {
    // Create an expanded container to replace our full one.
    size_t expandedCapacity = m_capacity * 2;
    T* expanded = Allocator().allocate(expandedCapacity);
    // Move existing elements into new container.
    size_t write = 0;
    auto read = m_begin;
    while (read != m_end) {
      if (read == m_capacity) {
        read = 0;
      } else {
        // Construct the entry in the new buffer using placement-new.
        new(&expanded[write]) T(std::move(m_buffer[read]));
        // Destruct the entry in the old buffer.
        m_buffer[read].~T();
        read++;
        write++;
      }
    }
    // Update iterators.
    m_begin = 0;
    m_end = write;
    // Swap containers.
    Allocator().deallocate(m_buffer, m_capacity);
    m_buffer = expanded;
    m_capacity = expandedCapacity;
  }
}

} // namespace stock
