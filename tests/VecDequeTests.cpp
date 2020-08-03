//
// Created by Matt Blair on 2019-03-05.
//
#include "catch.hpp"
#include "containers/VecDeque.hpp"
#include <algorithm>

#define TAGS "[VecDeque]"

using namespace stock;

template<typename T> std::vector<T> testValues();

template<> std::vector<int> testValues<int>() {
  return {8, 6, 7, 5, 3, 0, 9};
}

template<> std::vector<std::string> testValues<std::string>() {
  return {"This", "is", "Major", "Tom", "to", "ground", "control"};
}

TEMPLATE_TEST_CASE("VecDeque correctly reports size", TAGS, int, std::string) {

  VecDeque<TestType> a;

  const auto input = testValues<TestType>();

  REQUIRE(a.empty());
  REQUIRE(a.size() == 0);

  for (const auto& i : input) {
    a.push_back(i);
  }

  REQUIRE(a.size() == input.size());
  REQUIRE(a.capacity() >= a.size());

  a.clear();

  REQUIRE(a.empty());
  REQUIRE(a.size() == 0);
}

TEMPLATE_TEST_CASE("VecDeque returns values in the correct order", TAGS, int, std::string) {

  VecDeque<TestType> a;

  const auto input = testValues<TestType>();

  std::vector<TestType> output;

  SECTION("When pushed to back", TAGS) {
    for (const auto& i : input)   {
      a.push_back(i);
    }

    SECTION("And popped from back", TAGS) {
      while (!a.empty()) {
        auto i = a.back();
        a.pop_back();
        output.push_back(i);
      }
      // Output order should be reversed from original.
      std::reverse(output.begin(), output.end());
      CHECK(input == output);
    }

    SECTION("And popped from front", TAGS) {
      while (!a.empty()) {
        auto i = a.front();
        a.pop_front();
        output.push_back(i);
      }
      CHECK(input == output);
    }
  }

  SECTION("When pushed to front", TAGS) {
    for (const auto& i : input) {
      a.push_front(i);
    }

    SECTION("And popped from back", TAGS) {
      while (!a.empty()) {
        auto i = a.back();
        a.pop_back();
        output.push_back(i);
      }
      CHECK(input == output);
    }

    SECTION("And popped from front", TAGS) {
      while (!a.empty()) {
        auto i = a.front();
        a.pop_front();
        output.push_back(i);
      }
      // Output order should be reversed from original.
      std::reverse(output.begin(), output.end());
      CHECK(input == output);
    }
  }

}

TEMPLATE_TEST_CASE("VecDeque indexing operator", TAGS, int, std::string) {

  VecDeque<TestType> a;

  const auto input = testValues<TestType>();

  for (const auto& i : input) {
    a.push_back(i);
  }

  std::vector<TestType> output;

  for (size_t i = 0; i < input.size(); i++) {
    output.push_back(a[i]);
  }

  CHECK(input == output);
}

TEMPLATE_TEST_CASE("VecDeque copy constructor", TAGS, int, std::string) {

  VecDeque<TestType> a;

  const auto input = testValues<TestType>();

  for (const auto& i : input) {
    a.push_back(i);
  }

  VecDeque<TestType> b(a);

  REQUIRE(a.size() == b.size());

  std::vector<TestType> output;

  for (size_t i = 0; i < input.size(); i++) {
    output.push_back(b[i]);
  }

  CHECK(input == output);
}

TEMPLATE_TEST_CASE("VecDeque self-assignment", TAGS, int, std::string) {

  VecDeque<TestType> a;

  const auto input = testValues<TestType>();

  for (const auto& i : input) {
    a.push_back(i);
  }

  a = a;

  std::vector<TestType> output;

  for (size_t i = 0; i < a.size(); i++) {
    output.push_back(a[i]);
  }

  CHECK(input == output);
}

TEMPLATE_TEST_CASE("VecDeque move constructor", TAGS, int, std::string) {

  VecDeque<TestType> a;

  const auto input = testValues<TestType>();

  for (const auto& i : input) {
    a.push_back(i);
  }

  VecDeque<TestType> b(std::move(a));

  REQUIRE(a.size() == 0);
  REQUIRE(b.size() == input.size());

  std::vector<TestType> output;

  for (size_t i = 0; i < input.size(); i++) {
    output.push_back(b[i]);
  }

  CHECK(input == output);
}