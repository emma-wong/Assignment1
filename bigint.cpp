// $Id: bigint.cpp,v 1.76 2016-06-14 16:34:24-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"

bigint::bigint (long that): uvalue (that), is_negative (that < 0) {
   DEBUGF ('~', this << " -> " << uvalue)
}

bigint::bigint (const ubigint& uvalue, bool is_negative):
                uvalue(uvalue), is_negative(is_negative) {
}

bigint::bigint (const string& that) {
   is_negative = that.size() > 0 and that[0] == '_';
   uvalue = ubigint (that.substr (is_negative ? 1 : 0));
}

bigint bigint::operator+ () const {
   return *this;
}

bigint bigint::operator- () const {
   return {uvalue, not is_negative};
}

bigint bigint::operator+ (const bigint& that) const {
   /* Declare result */
   ubigint result;

   /* If uvalues are the same sign, add */
  if(is_negative == that.is_negative){
   result = uvalue + that.uvalue;
   return {result, is_negative};
  }
  /* By getting to this far in the function we know the uvalues are of different
   * signs. If they have the same value, the result is zero */
  if(uvalue == that.uvalue){
    result = uvalue - that.uvalue;
    return {result, false};
  }
  /* If one is greater than the other we subtract the smaller value from the larger value
   * and return the sign of the greater number */
  if(uvalue > that.uvalue){
    result = uvalue - that.uvalue;
    return {result, is_negative};
  }else{
    result = that.uvalue - uvalue;
    return {result, that.is_negative};
  }

}

bigint bigint::operator- (const bigint& that) const {
   ubigint result;

   /* If the argument and the local uvalue are of different signs, we add and pass the
    * sign of the leftmost operand (local is_negative) */
   if(is_negative != that.is_negative){
    result = uvalue + that.uvalue;
    return {result, is_negative};
   }
   /* By reaching this part of the function we know uvalue and that.uvalue are of the same sign.
    * if they are the same, then result is zero */
   if(uvalue == that.uvalue){
    result = uvalue - that.uvalue;
    return {result, false};
   }
   
   /* If the left operand is greater, we subtract normally and pass the sign, however if the right
    * operand is greater, we subtract subtract the left operand from the right and negate the sign boolean  */
   if(uvalue > that.uvalue){
    result = uvalue - that.uvalue;
    return {result, is_negative};
   }else{
    result = that.uvalue - uvalue;
    return {result, !that.is_negative};
   }
}

bigint bigint::operator* (const bigint& that) const {
  bigint result = uvalue * that.uvalue;

  /* if they're of the same sign, the result is always positive, otherwise negative */
  if(is_negative == that.is_negative){
   return {result, false};
  }else{
    return {result, true};
  }

}

bigint bigint::operator/ (const bigint& that) const {
   bigint result = uvalue / that.uvalue;

   /* Same as '*' */
   if(is_negative == that.is_negative){
   return {result, false};
  }else{
    return {result, true};
  }

   return result;
}

bigint bigint::operator% (const bigint& that) const {
   bigint result = uvalue % that.uvalue;
   /* always return the sign of the lefthand uvalue */
   return {result, is_negative};
}

bool bigint::operator== (const bigint& that) const {
   return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const {
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue > that.uvalue
                      : uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {
   return out << "bigint(" << (that.is_negative ? "-" : "+")
              << "," << that.uvalue << ")";
}
