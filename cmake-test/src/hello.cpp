#include<hello.hpp>

Cat::Cat() {
  std::cout << "a cat is constructed.\n";
}

Cat::~Cat() {
  std::cout << "a cat is destructed.\n";
}

auto Cat::miao() -> void {
  std::cout << "miao~miao~maio~\n";
}