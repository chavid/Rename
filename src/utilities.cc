
#include "utilities.hh"

#include <algorithm>
#include <sstream>
#include <set>
#include <cctype>

std::string lower( std::string str )
 {
  std::transform(str.begin(),str.end(),str.begin(),
   [](unsigned char c){ return std::tolower(c) ; } ) ;
  return str ;
 }

std::string escape( std::string str )
 {
  const std::set<char> special {'.', '*', '+', '[', ']', '^', '$', '\\', '{', '}', '(', ')', '|'} ;
  std::ostringstream result ;
  std::for_each(str.begin(),str.end(),[&](unsigned char c)
   {
    if ( special.contains(c) )
     { result << "\\" ; }
    result << c ;
   });
  return result.str() ;
 }
