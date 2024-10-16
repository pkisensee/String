////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  CharUtil.h
//
//  Copyright © Pete Isensee (PKIsensee@msn.com).
//  All rights reserved worldwide.
//
//  Permission to copy, modify, reproduce or redistribute this source code is granted provided the 
//  above copyright notice is retained in the resulting source code.
// 
//  This software is provided "as is" and without any express or implied warranties.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <algorithm>
#include <array>
#include <cctype>
#include <climits>
#include <locale>
#include <ranges>

namespace // anonymous
{

struct FileCharMap
{
  char special;     // Special file name char
  char replacement; // Replacement char
};

// Invalid file name characters and reasonable replacement values
constexpr std::array<FileCharMap, 6> kBadFileChars =
{ {
  { ':',  '-'  },
  { '\"', '\'' },
  { '<',  '('  },
  { '>',  ')'  },
  { '|',  '.'  },
  { '/',  '\\' },
} };

// Wildcards and replacements
constexpr std::array<FileCharMap, 2> kWildcardChars =
{ {
  { '*',  '+' },
  { '?',  ' '},
} };

} // anonymous

namespace PKIsensee
{

template< typename C >
class CharUtilT
{
public:

  enum class AllowWildcards
  {
    No,
    Yes
  };

  enum class ConvertWildcards
  {
    No,
    Yes
  };
  
  static bool IsUpper( C c )
  {
    return !!std::isupper( c, mLocale );
  }

  static bool IsLower( C c )
  {
    return !!std::islower( c, mLocale );
  }

  static C ToUpper( C c )
  {
    return C( toupper( C( c ), mLocale ) );
  }

  static C ToLower( C c )
  {
    return C( tolower( C( c ), mLocale ) );
  }

  static C ForwardSlashToBackslash( C c )
  {
    return c == C( '/' ) ? C( '\\' ) : c;
  }

  static bool IsDigit( C c )
  {
    return !!std::isdigit( c, mLocale );
  }

  static bool IsNumeric( C c )
  {
    return IsDigit( c ) || ( c == C( '.' ) );
  }
  
  static bool IsAlpha( C c )
  {
    return !!std::isalpha( c, mLocale );
  }

  static bool IsAlphaNum( C c )
  {
    return !!std::isalnum( c, mLocale );
  }

  static bool IsPrintable( C c )
  {
    return !!std::isprint( c, mLocale );
  }
  
  static bool IsWhitespace( C c )
  {
    return !!std::isspace( c, mLocale );
  }

  static bool IsControlChar( C c )
  {
    return !!std::iscntrl( c, mLocale );
  }

  static bool IsExtendedAscii( C c )
  {
    return c < 0 || c > C(0x7F);
  }

  static bool IsGoodFileCharWildcardsOK( C c )
  {
    return IsGoodFileCharEx( c, AllowWildcards::Yes );
  }

  static bool IsGoodFileChar( C c )
  {
    return IsGoodFileCharEx( c, AllowWildcards::No );
  }
  
  static bool IsGoodFileCharEx( C c, AllowWildcards allowWildcards )
  {
    if ( IsControlChar(c) )
      return false;

    using namespace std::ranges;
    if ( any_of( kBadFileChars, [c](const auto& i) { return ( c == C(i.special) ); } ) )
      return false;

    if ( ( allowWildcards == AllowWildcards::No ) && IsWildcardFileChar(c) )
      return false;

    return true;
  }
  
  static bool IsWildcardFileChar( C c )
  {
    using namespace std::ranges;
    return any_of( kWildcardChars, [c]( const auto& i ) { return ( c == C( i.special ) ); } );
  }

  static C ToGoodFileCharConvertWildcards( C c )
  {
    return ToGoodFileCharEx( c, ConvertWildcards::Yes );
  }

  static C ToGoodFileChar( C c )
  {
    return ToGoodFileCharEx( c, ConvertWildcards::No );
  }
    
  static C ToGoodFileCharEx( C c, ConvertWildcards convertWildcards )
  {
    // Convert any control characters
    if( IsControlChar( c ) )
      return C( '!' );

    // Convert any invalid chars
    using namespace std::ranges;
    auto badChar = find_if( kBadFileChars, [c]( const auto& i ) { return ( c == C( i.special ) ); } );
    if( badChar != std::end( kBadFileChars ) )
      return C( badChar->replacement );

    // If converting wildcards, check them too
    if( convertWildcards == ConvertWildcards::Yes )
    {
      auto wildcard = find_if( kWildcardChars, [c]( const auto& i ) { return ( c == C( i.special ) ); } );
      if( wildcard != std::end( kWildcardChars ) )
        return C( wildcard->replacement );
    }

    // Character requires no conversion
    return c;
  }
  
private:

  inline static const std::locale& mLocale = std::locale::classic();

}; // class CharUtilT

using CharUtil = CharUtilT<char>;
using CharUtilW = CharUtilT<wchar_t>;

} // namespace PKIsensee

////////////////////////////////////////////////////////////////////////////////////////////////////