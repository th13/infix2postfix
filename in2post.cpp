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

#include <cstdlib>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unistd.h>
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

  } // end of namespace in2post

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

/**
* Main program loop.
*
* Prompts user for an infix expression, utilizes the in2post module to convert &
* evaluate the expression, and outputs the results to standard out.
* Creates a program loop that reprompts the user for input until an exit symbol
* is supplied.
*/
void in2post_program_loop() {
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
}

//------------------------------------------------------------------------------
//                             main() method
//------------------------------------------------------------------------------

int main() {
  in2post_program_loop();
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
    //--------------------------------------------------------------------------
    //                      in2post error handling
    //--------------------------------------------------------------------------

    /**
    * Namespace cop4530::in2post::error
    *
    * Contains error handling methods and error codes related to converting and
    * evaluating infix/postfix expressions.
    */
    namespace error {
      enum ErrorCode {
        ERR_DEFAULT,
        ERR_INVALID_TOKEN,
        ERR_INVALID_OPERATION,
        ERR_INVALID_OPERAND
      };

      // Encapsulates ae error into a standard format consisting of a code and a
      // standard message for that error
      typedef struct Error {
        ErrorCode code;
        string message;

        // Allow an error to be initialized
        Error(ErrorCode code, string message) {
          this->code = code;
          this->message = message;
        }
      } Error;

      // List of errors with their code and message
      vector<Error> errors = {
        Error(ERR_DEFAULT, "An error with the in2post module occured."),
        Error(ERR_INVALID_TOKEN, "Expression contains invalid token."),
        Error(ERR_INVALID_OPERATION, "Supplied operation is not supported."),
        Error(ERR_INVALID_OPERAND, "Operand must be a numerical value or proper identifier.")
      };

      /**
      * Returns reference to error with corresponding error code.
      */
      const Error& get_error(ErrorCode ec) {
        for (Error& err : errors) {
          if (err.code == ec) {
            return err;
          }
        }

        // Default error
        return errors[0];
      }

      /**
      * "Throws" an error by outputing the error to the console and exiting the
      * program with a non-successful status code.
      */
      void throw_error(ErrorCode ec) {
        cerr << "\nError: " << get_error(ec).message << endl;
        exit(EXIT_FAILURE);
      }

    }   // end of namespace error

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

      /**
      * Applies specified operation to the two numerical values supplied.
      * Throws an error if operation is invalid.
      */
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

        error::throw_error(error::ERR_INVALID_OPERATION);
        return 0.0;
      }

      /**
      * Returns a stringified representation of the postfix expression.
      *
      * NOTE: An ending space is output with the current logic.
      * TODO: Possibly fix the above note, removing the trailing space.
      */
      string postfix_expression() {
        string postfix_exp = "";

        // Generate string of postfix expression
        for (string& item : postfix_tokens) {
          postfix_exp += item + " ";
        }

        return postfix_exp;
      }


      /**
      * Returns true if token is a valid number or variable operand.
      * Additionally sets a flag if it is a variable so we don't try to evaluate the
      * expression later on.
      */
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

      /**
      * Returns true if the supplied token is a valid operation as defined by
      * a regular expression.
      */
      bool is_operation(const string& token) {
        regex match_oper = regex("[\\+\\-\\*\\/]{1}");

        return regex_match(token, match_oper);
      }

      /**
      * Evaluate a numerical expression (i.e. one without variable identifiers)
      * from a set of input tokens.
      *
      * Utilizes a stack to store results of all the latest subexpression
      * calculated, until eventually only 1 element in the operand stack remains
      * (i.e. the final value of the expression).
      * TODO: If invalid tokens are found, throw an error.
      */
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

      /**
      * Add operand to postfix token list.
      */
      void process_operand(const string& oper) {
        postfix_tokens.push_back(oper);
      }

      /**
      * Adds proper postfix tokens when current input token is an operation
      * based on the algorithm specified in the project description &
      * requirements document.
      */
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

      /**
      * Append a group-opening character to the operator stack.
      */
      void process_group_opened() {
        operator_stack.push("(");
      }

      /**
      * Adds all the operators in the current group to the postfix token list.
      */
      void process_group_closed() {
        // Push operators until beginning of group (i.e. a '(') is found.
        while (operator_stack.top() != "(") {
          postfix_tokens.push_back(operator_stack.top());
          operator_stack.pop();
        }

        // Remove the '('
        operator_stack.pop();
      }

      /**
      * Returns a vector of the tokens of an infix expression.
      * Token is defined to be a space-separated string.
      */
      vector<string> tokenize_infix_expression() {
        istringstream iss(expression);
        vector<string> infix_tokens;
        string token;

        while (getline(iss, token, ' ')) {
          infix_tokens.push_back(token);
        }

        return infix_tokens;
      }

      /**
      * Cleanup method to reset the module state when working with a new
      * expression to convert. Only needs to be called in the convert() method,
      * as repeated calls to evaluate() should return the same result (cached
      * from the initial call).
      */
      void reset() {
        postfix_tokens.clear();
        operand_stack.clear();
        operator_stack.clear();
        expression = "";
        has_vars = false;
      }

      /**
      * Runs through every token in an infix expression string and generates a
      * postfix expression as a list of tokens.
      */
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
            error::throw_error(error::ERR_INVALID_TOKEN);
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

  }   // end of namespace in2post

}   // end of namespace cop4530
