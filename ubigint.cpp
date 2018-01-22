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
   string tmp = to_string(that);

   /* Iterates backwards over string ubig_value and pushes into vector ubigvalue */
   for(auto i=tmp.rbegin(); i != tmp.rend(); i++){
      ubig_value.push_back(*i);
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
      ubig_value.push_back(*i);
   }
}

ubigint::ubigint(ubigvalue_t that): ubig_value (that){}

ubigint ubigint::operator+ (const ubigint& that) const {
   /* Create a new ubigint vector */
   ubigvalue_t a;
   /* While loop to add digits pairwise from low order end to high order end */
   /* Carry over: for any sum greater or equal to 10, take the remainder and add the carry to the next digit */
   /* Push_back new digits to ubigint */
   int bigSize;
   // int smallSize;
   if(ubig_value.size() >= that.ubig_value.size()){
      bigSize = ubig_value.size();
      // smallSize = that.ubig_value.size();
   }else{
      bigSize = that.ubig_value.size();
      // smallSize = ubig_value.size();
   }

   bool carry = false;
   for(int i=0; i<bigSize; i++){
      int sum;
      if(i>ubig_value.size()){
         sum = that.ubig_value[i];
      }else if(i>that.ubig_value.size()){
         sum = ubig_value[i];
      }else{
         sum = ubig_value[i] + that.ubig_value[i];
      }

      if(carry) sum += 1;
      if(sum >= 10){
         sum = sum % 10;
         carry = true;
      }else{
         carry = false;
      }

      a.push_back(sum);

   }

   if(carry) {
      a.push_back(1);
   }
   return a;
}

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   /* Create a new ubigint vector */
   ubigvalue_t a;
   /* While loop to subtract digits pairwise from low order end to high order end */
   /* Carry over: if the left digit is smaller than the right digit, add 10 to the digit and set the borrow to the next digit to -1 */
   /* Push_back new digits to ubigint */
   int size = ubig_value.size();

   bool borrow = false;
   for(int i=0; i<size; i++){
      int difference;
      int leftDigit = ubig_value[i];
      if(borrow) leftDigit -= 1;
      if(i>that.ubig_value.size()){
         difference = leftDigit;
      }else{
         if(leftDigit<that.ubig_value[i]){
            difference = (10+leftDigit) - that.ubig_value[i];
            borrow = true;
         }else{
            difference = leftDigit - that.ubig_value[i];
            borrow = false;
         }
      }

      a.push_back(difference);

   }
   /* Pop_back all high order zeros from the vector before returning it */
   for(int i=a.size()-1; i>0; i--){
      if(a[i] != 0)break;
      a.pop_back();
   }
   return a;
}

ubigint ubigint::operator* (const ubigint& that) const {
   /* Allocate a new vector whose size is equal to the sum of the sizes of the other two operands */
   ubigvalue_t p;
   for(int i=0; i<(ubig_value.size() + that.ubig_value.size()); i++){
      p.push_back(0);
   }
   /* Perform an outer loop over one argument */
   for(int i=0; i<ubig_value.size(); i++){
      int carry = 0;
      /* Perform an inner loop over the other arg adding the new partial products to the product p */
      for(int j=0; j<that.ubig_value.size(); j++){
         int digit = p[i+j] + ubig_value[i]*that.ubig_value[j] + carry;
         p[i+j] = digit % 10;
         carry = digit/10;
      }
      p[i+ubig_value.size()] = carry;
   }
   return p;
}

void ubigint::multiply_by_2() {
   /* Basically the same thing as regular multiplication but just adding the value of ubig_value to itself */
   int carry = 0;
   for(int i = 0; i<ubig_value.size(); i++) {
      int digit = ubig_value[i] + ubig_value[i] + carry;
      carry = 0;
      if (digit >= 10) {
         carry = digit/10;
         digit = digit%10;
      }
      ubig_value[i] = digit; 
   }
   if (carry > 0) {
      ubig_value.push_back(carry);
   }
   /* Pop_back all high order zeros from the vector before returning it */
   for(int i=ubig_value.size()-1; i>0; i--){
      if(ubig_value[i] != 0)break;
      ubig_value.pop_back();
   }
}

void ubigint::divide_by_2() {
   int remainder = 0;
   for(int i = ubig_value.size(); i>=0; i--) {
      int digit = 0;
      if(remainder >0) {
         digit = digit + remainder;
         remainder = 0;
      }
      digit = digit + ubig_value[i]/2;
      if (ubig_value[i]%2 !=0) {
         remainder = 5;
      }
      ubig_value[i] = digit; 
   }
   /* Pop_back all high order zeros from the vector before returning it */
   for(int i=ubig_value.size()-1; i>0; i--){
      if(ubig_value[i] != 0)break;
      ubig_value.pop_back();
   }
}


struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, ubigint divisor) {
   // Note: divisor is modified so pass by value (copy).
   ubigint zero = 0;
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 =1;
   ubigint quotient =0;
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
   if (/*is_negative == that.is_negative &&*/ ubig_value.size() == that.ubig_value.size()) {
      /* If yes, run down both vectors and return false as soon as a difference is found */
      for(int i =0; i<ubig_value.size(); i++) {
         if (ubig_value[i] != that.ubig_value[i]) {
            return false;
         }
      }
      /* Otherwise, return true */
      return true;
   }
   /* If not, return false */
   return false;
}

bool ubigint::operator< (const ubigint& that) const {
   /* If the lengths are different */
   if ((ubig_value.size() != that.ubig_value.size())) {
      /* shorter num is less */
      if(ubig_value.size() < that.ubig_value.size()) {
         return true;
      }
      return false;
   }
   /* If the lengths are the same */
   else if ((ubig_value.size() == that.ubig_value.size())) {
      /* Run down parallel vectors from high order end to low order end */
      for (int i = 0; i<ubig_value.size(); i++) {
         /* When difference is found, return true/false as appropriate */
         if(ubig_value[i] != that.ubig_value[i]){
            if (ubig_value[i] < that.ubig_value[i]) {
               return true;
            }
            else {
               return false;
            }
         }
      }
      /* If no difference is found, return false */
      return false;
   }
}

/*ostream& operator<< (ostream& out, const ubigint& that) { */
   /* Make fn print out numbers the way dc(1) does */
   /*if (that.is_negative) {
      out << "-";
   }
   out << that.ubig_value;
   return out;
}*/