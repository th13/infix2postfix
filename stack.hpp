/**
* COP4530 Project 3
* stack.hpp
*
* Implementation file for Stack class defined in stack.h. Much of the
* implementation defers to methods previously defined in the STL vector class.
*/

using namespace cop4530;

// Default constructor
template <typename T>
Stack<T>::Stack() {}

// Copy constructor
template <typename T>
Stack<T>::Stack(const Stack<T>& rhs) {
  this->v = rhs.v;
}

// Move constructor
template <typename T>
Stack<T>::Stack(Stack<T>&& rhs) {
  this->v = rhs.v;
}

// Destructor
template <typename T>
Stack<T>::~Stack() {}

// Copy assignment operator
template <typename T>
Stack<T>& Stack<T>::operator=(const Stack<T>& rhs) {
  if (this != &rhs) {
    this->v = rhs.v;
  }

  return *this;
}

// Move assignment operator
template <typename T>
Stack<T>& Stack<T>::operator=(Stack<T>&& rhs) {
  if (this != &rhs) {
    this->v = rhs.v;
  }

  return *this;
}


// Check if stack is empty
template <typename T>
bool Stack<T>::empty() const {
  return this->v.empty();
}

// Delete all elements in the stack
template <typename T>
void Stack<T>::clear() {
  this->v.clear();
}

// Get the number of elements in the stack
template <typename T>
int Stack<T>::size() const {
  return this->v.size();
}

// Add a copy of x to the stack
template <typename T>
void Stack<T>::push(const T& x) {
  this->v.push_back(x);
}

// Move x to the stack
template <typename T>
void Stack<T>::push(T&& x) {
  this->v.push_back(x);
}

// Delete element on top of stack
template <typename T>
void Stack<T>::pop() {
  this->v.pop_back();
}

// Get a reference to the item on top of the stack
template <typename T>
T& Stack<T>::top() {
  return this->v.back();
}

// Get a const reference to the item on top of the stack
template <typename T>
const T& Stack<T>::top() const {
  return this->v.back();
}

// Print out elements in the stack, oldest -> newest
template <typename T>
void Stack<T>::print(std::ostream& os, char ofc) const {
  if (!this->v.empty()) {
    os << this->v[0];

    for (int i = 1; i < v.size(); i++) {
      os << ofc << v[i];
    }
  }
}

// Print stack to an ostream by invoking print() method
template <typename T>
std::ostream& operator<<(std::ostream& os, const Stack<T>& stack) {
  stack.print(os);
  return os;
}

// Check if two stack are equal
// Two stacks are equal if they have the same elements in the same order
template <typename U>
bool operator==(const Stack<U>& a, const Stack<U>& b) {
  // Stacks must have the same number of elements to be candidates for equality
  if (a.size() != b.size()) {
    return false;
  }

  for (int i = 0; i < a.size(); i++) {
    if (a.v[i] != b.v[i]) {
      return false;
    }
  }

  return true;
}

// Check if two stacks are NOT equal
template <typename U>
bool operator!=(const Stack<U>& a, const Stack<U>& b) {
  return !(a == b);
}

// Check if first stack is less than the second stack
template <typename U>
bool operator<=(const Stack<U>& a, const Stack<U>& b) {
  for (int i = 0; i < a.size(); i++) {
    if (a.v[i] > b.v[i]) {
      return false;
    }
  }

  return true;
}
