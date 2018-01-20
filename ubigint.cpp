// $Id: ubigint.cpp,v 1.14 2016-06-23 17:21:26-07 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <string>
using namespace std;

#include "ubigint.h"
#include "debug.h"

ubigint::ubigint (unsigned long that): ubig_value (that) {
   /* If zero is passed as an parameter, do nothing*/
   if (that == 0){
      return;
   }
   /* converts unsigned long to string */
   String tmp = to_string(that);

   /* Iterates backwards over string ubig_value and pushes into vector ubigvalue */
   for(auto i=tmp.rbegin(); i != tmp.rend(); i++){
      ubig_value.push_back(i*);
   }
}

ubigint::ubigint (const string& that): ubig_value(0) {
   DEBUGF ('~', "that = \"" << that << "\"");
   for (char digit: that) {
      if (not isdigit (digit)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      //ubig_value = ubig_value * 10 + digit - '0';
   }
   /* Iterates backwards over string ubig_value and pushes into vector ubigvalue */
   for(auto i=that.rbegin(); i != that.rend(); i++){
      ubig_value.push_back(i*);
   }
}

ubigint ubigint::operator+ (const ubigint& that) const {
   /* Create a new ubigint vector */
   /* While loop to add digits pairwise from low order end to high order end */
   /* Carry over: for any sum greater or equal to 10, take the remainder and add the carry to the next digit */
   /* Push_back new digits to ubigint */
   /* Second while loop to match the remaining digits in the longer numbers with zeros */
   return ubigint (ubig_value + that.ubig_value);
}

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   /* Create a new ubigint vector */
   /* While loop to subtract digits pairwise from low order end to high order end */
   /* Carry over: if the left digit is smaller than the right digit, add 10 to the digit and set the borrow to the next digit to -1 */
   /* Push_back new digits to ubigint */
   /* Second while loop to match the remaining digits in the longer numbers with zeros */
   /* Pop_back all high order zeros from the vector before returning it */
   return ubigint (ubig_value - that.ubig_value);
}

ubigint ubigint::operator* (const ubigint& that) const {
   /* Allocate a new vector whose size is equal to the sum of the sizes of the other two operands */
   /* Perform an outer loop over one argument */
   /* Perform an inner loop over the other arg adding the new partial products to the product p */
   /* 
      p = m+n
      for i in [0,m)
         c = 0
         for j in [0,n)
            d = p+uv+c
            p = d rem 10
            c = floor of d/10
         p = c
   */
   return ubigint (ubig_value * that.ubig_value);
}

void ubigint::multiply_by_2() {
   ubig_value *= 2;
}

void ubigint::divide_by_2() {
   ubig_value /= 2;
}


struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, ubigint divisor) {
   // Note: divisor is modified so pass by value (copy).
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   /* Check if the signs are the same and the lengths of the vectors are the same */
   if (is_negative == that.is_negative) {
      /* If yes, run down both vectors and return false as soon as a difference is found */
      
      /* Otherwise, return true */
      return true;
   }
   /* If not, return false */
   return false;
}

bool ubigint::operator< (const ubigint& that) const {
   /* If the signs are different, negative number is less than positive number */
   /* If the signs are the same and the lengths are different */
      /* If nums are pos, shorter num is less */
      /* If nums are neg, longer num is less */
   /* If the signs are the same and the lengths are the same */
      /* Run down parallel vectors from high order end to low order end */
      /* When difference is found, return true/false as appropriate */
      /* If no difference is found, return false */

   return ubig_value < that.ubig_value;
}

ostream& operator<< (ostream& out, const ubigint& that) { 
   /* Make fn print out numbers the way dc(1) does */
   if (that.is_negative) {
      out << "-";
   }
   out << that.ubig_value;
   return out;
}
