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
      string convert(const string&);

      // Evaluates the postfix expression we have converted and returns a string
      // representation
      // NOTE: Only expressions with only numeric operands can be evaluated, so
      // any expression containing a variable is simply returned as-is
      string evaluate();

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

      bool has_vars;    // becomes true if expression contains variable operands

      string expression;   // string to hold expression we are converting
      vector<string> infix_tokens;  // vector holding infix expression's ops
      vector<string> postfix_tokens;  // vector holding postfix expression's ops

      // Method to split an expression into individual parts using a single
      // space as a delimeter
      void exp_split();

      // Returns string value of postfix expression
      string postfix_expression();

      // Methods to process specific conversion steps
      void process_operand(const string& oper);
      void process_operation(const string& oper);
      void process_group_opened();    // group opened when we receive a '('
      void process_group_closed();    // group is closed if we receive a ')'

      // Boolean functions to check conditions of the supplied token
      bool is_operand(const string& token);     // check operand (num or variable)
      bool is_operation(const string& token);   // check operation (+ - * /)

      // Execute the specified operation on the operands and return the value
      double do_operation(double lhs, double rhs, const string& oper);

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

  cout << "Enter infix expression ('exit' to quit): ";

  // Loop through each line in stdin
  while (getline(cin, line)) {
    cout << "\n";
    cout << "Postfix expression: " << in2post->convert(line) << endl;
    cout << "Postfix evaluation: " << in2post->evaluate() << endl;
    cout << "Enter infix expression ('exit' to quit): ";
  }

  cout << endl;

  return 0;
}

// Evaluates postfix expression and returns string value
string In2Post::evaluate() {
  if (has_vars) {
    string pfexp = postfix_expression();
    return pfexp + " = " + pfexp;
  }

  string eval = "";

  for (string& item : postfix_tokens) {
    if (is_operand(item)) {
      operand_stack.push(stod(item));
    }
    else if (is_operation(item)) {
      double rhs = operand_stack.top();
      operand_stack.pop();
      double lhs = operand_stack.top();
      operand_stack.pop();

      operand_stack.push(do_operation(lhs, rhs, item));
    }
  }

  if (operand_stack.size() == 1) {
    eval = to_string(operand_stack.top());

    // Formatting to remove trailing 0's
    while (eval.back() == '0') {
      eval.pop_back();
    }

    // Remove . if no decimals
    if (eval.back() == '.') {
      eval.pop_back();
    }
  }

  return eval;
}

double In2Post::do_operation(double lhs, double rhs, const string& oper) {
  if (oper == "*") {
    return lhs * rhs;
  }
  else if (oper == "/") {
    return lhs / rhs;
  }
  else if (oper == "+") {
    return lhs + rhs;
  }
  else if (oper == "-") {
    return lhs - rhs;
  }

  return 0.0;
}

string In2Post::postfix_expression() {
  string postfix_exp = "";

  // Generate string of postfix expression
  for (string& item : postfix_tokens) {
    postfix_exp += item + " ";
  }

  return postfix_exp;
}


// Returns true if token is a valid number or variable operand
// Additionally sets a flag if it is a variable so we don't try to evaluate the
// expression later on
bool In2Post::is_operand(const string& token) {
  regex match_num = regex("[0-9]+(\\.?[0-9]+)?");
  regex match_var = regex("[a-zA-Z]+[0-9a-zA-Z_]*");

  // First check to see if token is a variable, because we need to flag this so
  // we don't try to evaluate the expression later on
  if (regex_match(token, match_var)) {
    has_vars = true;
    return true;
  }

  // Otherwise return whether it's a num or not
  return regex_match(token, match_num);
}

bool In2Post::is_operation(const string& token) {
  regex match_oper = regex("[\\+\\-\\*\\/]{1}");

  return regex_match(token, match_oper);
}

void In2Post::process_operand(const string& oper) {
  postfix_tokens.push_back(oper);
}

void In2Post::process_operation(const string& oper) {
  // Push stack to postfix expression until the following conditions are
  // met
  while (!operator_stack.empty()) {
    const string& current = operator_stack.top();
    if (current != "(") {
      if (oper == "*" || oper == "/") {
        if (current != "+" && current != "-") {
          postfix_tokens.push_back(current);
          operator_stack.pop();
        }
        else {
          break;
        }
      }
      else {
        postfix_tokens.push_back(current);
        operator_stack.pop();
      }
    }
    else {
      break;
    }
  }

  operator_stack.push(oper);
}

void In2Post::process_group_opened() {
  operator_stack.push("(");
}

void In2Post::process_group_closed() {
  // Push operators until beginning of group (i.e. a '(') is found.
  while (operator_stack.top() != "(") {
    postfix_tokens.push_back(operator_stack.top());
    operator_stack.pop();
  }

  // Remove the '('
  operator_stack.pop();
}


// Convert the expression passed into the method
// Move the temporarily created string to the return value
string In2Post::convert(const string& the_exp) {
  cleanup();    // start with a fresh instance

  string postfix_exp = "";   // temp string for returning postfix exp
  expression = the_exp;
  exp_split();

  for (string& token : infix_tokens) {
    // Match numbers and variables
    if (is_operand(token)) {
      process_operand(token);
    }
    // Match beginning of a group
    else if (token == "(") {
      process_group_opened();
    }
    // Match operators
    else if (is_operation(token)) {
      process_operation(token);
    }
    // Match closing of a group
    else if (token == ")") {
      process_group_closed();
    }
  }

  // Reached end of input expression string. Pop the stack until empty.
  while (!operator_stack.empty()) {
    postfix_tokens.push_back(operator_stack.top());
    operator_stack.pop();
  }


  return postfix_expression();
}

void In2Post::exp_split() {
  istringstream iss(expression);
  string token;

  while (getline(iss, token, ' ')) {
    infix_tokens.push_back(token);
  }
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
  operand_stack.clear();
  operator_stack.clear();
  expression = "";
  has_vars = false;
}

// Returns the same instance of In2Post every time
In2Post* In2Post::instance() {
  if (!the_instance) {
    the_instance = new In2Post;
  }

  return the_instance;
}
