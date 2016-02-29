/**
* COP4530 Project 3
* in2post.cpp
*
* Author: Trevor Helms
*
* Program that converts infix arithmetic expressions to postfix expressions
* accepting both numerical values (integers and real numbers) and variables.
* If the expression contains solely numerical values, the result of the postfix
* expression is printed out. If the expression contains variables, only the
* postfixed expression is printed.
*
* Input is accepted through stdin and output through stdout. The postfix
* expression is generated through an algorithm utilizing a stack to store
* expression components.
*/

#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>
#include "stack.h"

using namespace std;

namespace cop4530 {

  /**
  * The following class employs a singleton pattern to encapsulate functionality
  * related to converting an infix arithmetic expression into a postfix
  * expression.
  */
  class In2Post {
    public:
      // Converts the infix expression to postfix
      // Returns the postfix expression as a string, though the expression
      // is also maintained internally in a vector
      string&& convert(const string&);

      // Evaluates the postfix expression we have converted
      // NOTE: Only expressions with only numeric operands can be evaluated
      double evaluate();

      // Returns instance of In2Post class. Allows only a single instance to be
      // created, so returns the same instance every time.
      static In2Post* instance();

    private:
      // Static instance of class to facilitate singleton pattern
      static In2Post* the_instance;

      // Stack for holding operators while converting
      Stack<string> operator_stack;

      // Stack for holding operands when evaluating postfix expression
      // Expressions with variable operands are never evaluated, so this holds
      // only a numeric value.
      Stack<double> operand_stack;

      string expression;   // string to hold expression we are converting
      vector<string> infix_tokens;  // vector holding infix expression's ops
      vector<string> postfix_tokens;  // vector holding postfix expression's ops

      // Method to split an expression into individual parts using a single
      // space as a delimeter
      // Returns pointer to vector of parts we create in the method.
      void exp_split();

      // Method to cleanup the instance when we're working on a new conversion
      void cleanup();

      // Private constructors so class cannot be instantiated from the outside
      In2Post();
      In2Post(const In2Post&);
      ~In2Post();                           // destructor
      In2Post& operator=(const In2Post&);   // assignment operator
  };

  // Initialize the_stance to be null
  In2Post* In2Post::the_instance = nullptr;

} // end of namespace cop4530


using namespace cop4530;

// Main method
int main() {
  In2Post* in2post = In2Post::instance(); // get instance of in2post
  string line;  // string to hold current expression we're reading in

  // Loop through each line in stdin
  while (std::getline(cin, line)) {
    cout << in2post->convert(line) << endl;
  }

  return 0;
}


// Convert the expression passed into the method
// Move the temporarily created string to the return value
string&& In2Post::convert(const string& the_exp) {
  cleanup();    // start with a fresh instance

  string postfix_exp;   // temp string for returning postfix exp
  expression = the_exp;
  exp_split();

  // TODO: This loop is MONOLITHIC.
  // Make this to be no longer the case.
  for (string& token : infix_tokens) {
    // Match numbers and variables
    if (regex_match(token, regex("[0-9]+(\\.?[0-9]+)?")) ||
        regex_match(token, regex("[a-zA-Z]+[0-9a-zA-Z_]*"))) {
      // Immediately push these to the postfix expression
      postfix_tokens.push_back(token);
      postfix_exp += token + " ";
    }
    // Match open parenthesis
    else if (regex_match(token, regex("\\("))) {
      operator_stack.push(token);
      cout << "open paren" << endl;
    }
    // Match operators
    else if (regex_match(token, regex("[\\+\\-\\*\\/]{1}"))) {
      // Push stack to postfix expression until the following conditions are
      // met
      cout << token << " matched operator" << endl;
      while (!operator_stack.empty()) {
        const string& current = operator_stack.top();
        if (!regex_match(current, regex("\\("))) {
          if (token == "*" || token == "/") {
            if (current != "+" && current != "-") {
              postfix_tokens.push_back(current);
              postfix_exp += current + " ";
              operator_stack.pop();
            }
            else {
              break;
            }
          }
          else {
            postfix_tokens.push_back(current);
            postfix_exp += current + " ";
            operator_stack.pop();
          }
        }
        else {
          break;
        }
      }

      operator_stack.push(token);
    }
    else if (token == ")") {
      while (operator_stack.top() != "(") {
        postfix_tokens.push_back(operator_stack.top());
        postfix_exp += operator_stack.top() + " ";
        operator_stack.pop();
      }

      operator_stack.pop();
    }

  }

  // Reached end of input expression string. Pop the stack until empty.
  while (!operator_stack.empty()) {
    postfix_tokens.push_back(operator_stack.top());
    postfix_exp += operator_stack.top() + " ";
    operator_stack.pop();
  }

  return move(postfix_exp);
}

void In2Post::exp_split() {
  istringstream iss(expression);
  string token;

  while (getline(iss, token, ' ')) {
    infix_tokens.push_back(token);
  }
}

double In2Post::evaluate() {
  // TODO: Evaluate a postfix expression
  return 0.0;
}

// Default In2Post constructor
In2Post::In2Post() {
}

// Destructor
In2Post::~In2Post() {
  cleanup();
}

// Cleanup method to delete things when working with a new conversion
void In2Post::cleanup() {
  infix_tokens.clear();
  postfix_tokens.clear();
  expression = "";
}

// Returns the same instance of In2Post every time
In2Post* In2Post::instance() {
  if (!the_instance) {
    the_instance = new In2Post;
  }

  return the_instance;
}
