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

//------------------------------------------------------------------------------
//                  in2post module interface declarations
//------------------------------------------------------------------------------

namespace cop4530 {

  /**
  * Namespace in2post encapsulates functionality related to converting infix
  * expressions to postfix.
  *
  * This namespace is set up to provide a public interface and hide private
  * methods in an anonymous namespace.
  * The following definitions refer to the public interface of the namespace.
  */
  namespace in2post {

    /**
    * Converts the infix expression to postfix.
    *
    * Returns the postfix expression as a string, though the expression
    * is also maintained internally in a vector.
    */
    string convert(const string&);

    /**
    * Evaluates the postfix expression we have converted and returns a string
    * representation.
    *
    * NOTE: Only expressions with only numeric operands can be evaluated, so
    * any expression containing a variable is simply returned as-is
    */
    string evaluate();

  } // end of namespace cop4530::in2post

} // end of namespace cop4530


using namespace cop4530;

/**
* Checks if input came from redirection instead of user input.
*
* Works only on POSIX systems, but who codes on Windows anyway? #standardization
*/
bool input_redirected() {
  return !isatty(STDIN_FILENO);
}

//------------------------------------------------------------------------------
//                             main() method
//------------------------------------------------------------------------------

int main() {
  string line;          // string to hold current expression we're reading in

  cout << "Enter infix expression ('exit' to quit): ";

  // Loop through each line in stdin
  while (getline(cin, line) && line != "quit") {
    // Need to output a newline every iteration if input came from redirection
    // as we don't have one output as we do with non-redirected input when a
    // user hits enter.
    if (input_redirected()) {
      cout << endl;
    }
    
    cout << "Postfix expression: " << in2post::convert(line) << endl;
    cout << "Postfix evaluation: " << in2post::evaluate() << endl;
    cout << "Enter infix expression ('exit' to quit): ";
  }

  return 0;
}


//------------------------------------------------------------------------------
//                      in2post module definitions
//------------------------------------------------------------------------------

namespace cop4530 {

  /**
  * cop4530::in2post module definitions.
  */
  namespace in2post {

    /**
    * This anonymous namespace encapsulates private functionality to the in2post
    * module that we don't want exposed to the user.
    */
    namespace {

      //------------------------------------------------------------------------
      //                       Internal data members
      //------------------------------------------------------------------------

      // Stack for holding operators while converting
      Stack<string> operator_stack;

      // Stack for holding operands when evaluating postfix expression
      // Expressions with variable operands are never evaluated, so this holds
      // only a numeric value.
      Stack<double> operand_stack;

      bool has_vars;    // becomes true if expression contains variable operands

      string expression;                // string to hold expression we are converting
      vector<string> postfix_tokens;    // vector holding postfix expression's ops


      //------------------------------------------------------------------------
      //                 Internal (private) module methods
      //------------------------------------------------------------------------

      double apply_operation(const string& operation, double lhs, double rhs) {
        if (operation == "*") {
          return lhs * rhs;
        }
        else if (operation == "/") {
          return lhs / rhs;
        }
        else if (operation == "+") {
          return lhs + rhs;
        }
        else if (operation == "-") {
          return lhs - rhs;
        }

        return 0.0;
        //in2post::throw_error(in2post::err::INVALID_OPERATION);
      }

      string postfix_expression() {
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
      bool is_operand(const string& token) {
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

      bool is_operation(const string& token) {
        regex match_oper = regex("[\\+\\-\\*\\/]{1}");

        return regex_match(token, match_oper);
      }

      void evaluate_numerical_expression() {
        for (string& item : postfix_tokens) {
          if (is_operand(item)) {
            operand_stack.push(stod(item));
          }
          else if (is_operation(item)) {
            double rhs = operand_stack.top();
            operand_stack.pop();
            double lhs = operand_stack.top();
            operand_stack.pop();

            operand_stack.push(apply_operation(item, lhs, rhs));
          }
        }
      }

      void process_operand(const string& oper) {
        postfix_tokens.push_back(oper);
      }

      void process_operation(const string& oper) {
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

      void process_group_opened() {
        operator_stack.push("(");
      }

      void process_group_closed() {
        // Push operators until beginning of group (i.e. a '(') is found.
        while (operator_stack.top() != "(") {
          postfix_tokens.push_back(operator_stack.top());
          operator_stack.pop();
        }

        // Remove the '('
        operator_stack.pop();
      }


      vector<string> tokenize_infix_expression() {
        istringstream iss(expression);
        vector<string> infix_tokens;
        string token;

        while (getline(iss, token, ' ')) {
          infix_tokens.push_back(token);
        }

        return infix_tokens;
      }

      // Cleanup method to delete things when working with a new conversion
      void reset() {
        postfix_tokens.clear();
        operand_stack.clear();
        operator_stack.clear();
        expression = "";
        has_vars = false;
      }

      void process_infix_tokens(const vector<string>& infix_tokens) {
        // Loop through each token from the infix expression and process it
        // according to what kind of token it is.
        for (const string& token : infix_tokens) {
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
          // We've received an invalid token, so we should throw an error.
          else {
            break;
          }
        }

        // Almost finished. Add the remaining operations from the operator stack.
        while (!operator_stack.empty()) {
          postfix_tokens.push_back(operator_stack.top());
          operator_stack.pop();
        }
      }

    }   // end of anonymous namespace


    //--------------------------------------------------------------------------
    //               Module in2post public interace definitions
    //--------------------------------------------------------------------------

    /**
    * Convert the expression supplied as an argument to a postfix expression.
    *
    * Return the expression as a string.
    */
    string convert(const string& the_exp) {
      // We're dealing with a new expression here, so reset the module.
      reset();

      string postfix_exp = "";   // temp string for returning postfix exp
      expression = the_exp;

      vector<string> infix_tokens = tokenize_infix_expression();
      process_infix_tokens(infix_tokens);

      return postfix_expression();
    }

    /**
    * Evaluates postfix expression.
    *
    * Returns string value of the evaluation, which can be:
    *   1. Numerical value (if expression contains only numerical operands).
    *   2. Duplication of postfix expression (if expression contains variables).
    */
    string evaluate() {
      // Return the postfix expression if it contains any variables (since we
      // can't apply arithmetic to unknown values).
      if (has_vars) {
        string pfexp = postfix_expression();
        return pfexp + " = " + pfexp;
      }

      // Calculate the expression (stored in operand stack)
      evaluate_numerical_expression();

      string eval;     // holds the numerical evaluation (as a string)

      // If expression is evaluated without errors, then the operand stack will
      // contain a single element (the final value of the expression).
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

      return postfix_expression() + " = " + eval;
    }

  }   // end of namespace cop4530::in2post

}   // end of namespace cop4530
