/**
* COP4530 Project 3
* stack.h
*
* Author: Trevor Helms
*
* Interface file for Stack class, which utilizes a vector under the hood to
* implement a generic stack. Standard stack methods are supported in addition to
* three comparison operations.
*/

#ifndef STACK_H
#define STACK_H

#include <iostream>
#include <vector>

namespace cop4530 {

  template <typename T>
  class Stack {
    public:
      // Constructors
      Stack();                      // default constructor
      Stack(const Stack<T>& rhs);   // copy constructor
      Stack(Stack<T>&& rhs);        // move constructor

      // Destructor
      ~Stack();

      // Copy assignment operator
      Stack<T>& operator=(const Stack<T>& rhs);

      // Move assignment operator
      Stack<T>& operator=(Stack<T>&& rhs);

      // Member functions
      bool empty() const;      // check if stack is empty
      void clear();            // delete all elements in the stack
      int size() const;        // number of elements in the stack

      void push(const T& x);   // add a copy of x to the stack
      void push(T&& x);        // move x to the stack
      void pop();              // delete most recent element on the stack

      T& top();                // returns reference to most recent element
      const T& top() const;    // returns const reference to most recent element

      // Print out elements in the stack, oldest -> newest
      void print(std::ostream& os, char ofc = ' ') const;

    private:
      std::vector<T> v;          // internal vector for storing the stack elements

      // Friend functions for comparing stacks
      // Overload equality operator
      template <typename U>
      friend bool operator==(const Stack<U>& a, const Stack<U>& b);

      // Overload inequality operator
      template <typename U>
      friend bool operator!=(const Stack<U>& a, const Stack<U>& b);

      // Overload less-than-or-equal operator
      // Returns true if every element in the first stack is less than or equal to
      // its corresponding element in the second stack. Runs until the first stack
      // is empty
      template <typename U>
      friend bool operator<=(const Stack<U>& a, const Stack<U>& b);

  };  // end of class stack

  // Non-member stack functions
  // Prints the stack to an ostream by invoking the print() method
  template <typename T>
  std::ostream& operator<<(std::ostream& os, const Stack<T>& stack);

  // Include implementation file
  #include "stack.hpp"

} // end of namespace cop4530

#endif
