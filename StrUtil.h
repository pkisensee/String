////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  StrUtil.h
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
#include <cassert>
#include <array>
#include <algorithm>
#include <limits>
#include <numeric>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

#include "CharUtil.h"
#include "Util.h"

namespace // anonymous
{

struct XmlMarkup
{
  char           symbol;        // e.g. '&'
  const char    *xmlCode;       // e.g. "&amp;"
  const wchar_t *xmlWideCode;   // e.g. L"&amp;" TODO use this
};

constexpr std::array<XmlMarkup, 5> kXmlReplace =
{ {
  { '&',  "&amp;",  L"&amp;"  },
  { '<',  "&lt;",   L"&lt;"   },
  { '>',  "&gt;",   L"&gt;"   },
  { '\"', "&quot;", L"&quot;" },
  { '\'', "&apos;", L"&apos;" },
} };

} // anonymous

namespace PKIsensee
{

////////////////////////////////////////////////////////////////////////////////////////////////////

// Conversions

namespace StringUtil {

template<typename T, typename F>
inline T TransformTo( F str ) noexcept
{
  return { std::begin( str ), std::end( str ) };
}

#pragma warning(push)
#pragma warning(disable: 4244) // ignore loss of data; it's expected

inline std::string GetUtf8( std::wstring_view wstr )
{
  return TransformTo<std::string>( wstr );
}
#pragma warning(pop)

inline std::wstring GetUtf16( std::string_view str )
{
  return TransformTo<std::wstring>( str );
}

} // StringUtil

////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename C>
class StrUtilT
{
private:

  using strT = std::basic_string<C>;

public:

	enum class AllowWildcards
	{
		No,
		Yes
	};

  enum class ConvertWildcards
  {
    No,
    Yes,
    Remove
  };

  // Replace special characters with XML markup
  //
  //    &  -->  &amp;
  //    <  -->  &lt;
  //    >  -->  &gt;
  //    "  -->  &quot;
  //    '  -->  &apos;

  static void ToXmlSafe( strT& str )
  {
    size_t index = 0;
    for( const auto& specialXml : kXmlReplace )
    {
      // Replace each special character with the proper code
      auto pos = str.find( C( specialXml.symbol ) );
      while( pos != strT::npos )
      {
        if constexpr( sizeof(strT::value_type) == 1 )
          str.replace( pos, 1, kXmlReplace[ index ].xmlCode );
        else
          str.replace( pos, 1, kXmlReplace[ index ].xmlWideCode );
        pos = str.find( C( specialXml.symbol ), pos+1 );
      }
      ++index;
    }
  }

  static strT GetXmlSafe( const strT& str )
  {
    auto r( str );
    ToXmlSafe( r );
    return r;
  }

  // Trim leading characters
  // e.g. to trim leading white space, call ToTrimmedLeading( str, " \t" )
  
  static void ToTrimmedLeading( strT& str, const strT& trimCharset )
  {
    // Find the first character that's not in the character set. If all characters
    // are in the set, clear the string and bail out.
    auto firstNot = str.find_first_not_of( trimCharset );
    if( firstNot == strT::npos )
    {
      str.clear();
      return;
    }
    
    // The resulting string is trimmed of leading characters in the set
    str.assign( str, firstNot, str.size() - firstNot + 1 );
  }

  static strT GetTrimmedLeading( const strT& str, const strT& trimCharset )
  {
    auto r( str );
    ToTrimmedLeading( r, trimCharset );
    return r;
  }
  
  // Trim trailing characters
  // e.g. to trim trailing white space, call ToTrimmedTrailing( str, " \t" )
  
  static void ToTrimmedTrailing( strT& str, const strT& trimCharset )
  {
    // Find the last character that's not in the character set
    auto lastNot = str.find_last_not_of( trimCharset );
    str.resize( (lastNot == strT::npos) ? 0 : (lastNot + 1) );
  }

  static strT GetTrimmedTrailing( const strT& str, const strT& trimCharset )
  {
    auto r( str );
    ToTrimmedTrailing( r, trimCharset );
    return r;
  }
  
  // Trim leading and trailing characters
  // e.g. to trim leading/trailing white space, call ToTrimmed( str, " \t" )
  
  static void ToTrimmed( strT& str, const strT& trimCharset )
  {
    // Find the first character that's not in the character set. If all characters
    // are in the set, clear the string and bail out.
    auto firstNot = str.find_first_not_of( trimCharset );
    if( firstNot == strT::npos )
    {
      str.clear();
      return;
    }
    
    // Find the last character that's not in the character set
    auto lastNot = str.find_last_not_of( trimCharset );
    assert( lastNot != strT::npos );
    
    // The resulting string is trimmed of leading and trailing characters in the set
    str.assign( str, firstNot, lastNot - firstNot + 1 );
  }

  static strT GetTrimmed( const strT& str, const strT& trimCharset )
  {
    auto r( str );
    ToTrimmed( r, trimCharset );
    return r;
  }

  static bool IsDigit( const strT& str )
  {
    return !str.empty() && std::ranges::all_of( str, CharUtilT<C>::IsDigit );
  }

  static bool IsNumeric( const strT& str )
  {
    if( str.empty() )
      return false;

    // allow leading minus sign
    using namespace std::ranges;
    if( str[0] == C( '-' ) )
    {
      auto substr = subrange( std::begin( str ) + 1, std::end( str ) );
      return all_of( substr, CharUtilT<C>::IsNumeric );
    }

    return all_of( str, CharUtilT<C>::IsNumeric );
  }

  static bool IsAlphaNum( const strT& str )
  {
    return !str.empty() && std::ranges::all_of( str, CharUtilT<C>::IsAlphaNum );
  }

  static bool IsPrintable( const strT& str )
  {
    return !str.empty() && std::ranges::all_of( str, CharUtilT<C>::IsPrintable );
  }

  static bool IsExtendedAscii( const strT& str )
  {
    return !str.empty() && std::ranges::all_of( str, CharUtilT<C>::IsExtendedAscii );
  }
  
  static bool IsGoodFileName( const strT& str, AllowWildcards allowWildcards )
  {
    if( allowWildcards == AllowWildcards::No )
      return std::ranges::all_of( str, CharUtilT<C>::IsGoodFileChar );

    return std::ranges::all_of( str, CharUtilT<C>::IsGoodFileCharWildcardsOK );
  }
  
  static bool ContainsWildcard( const strT& str )
  {
    return std::ranges::any_of( str, CharUtilT<C>::IsWildcardFileChar );
  }
  
  static void ToGoodFileName( strT& str, ConvertWildcards convertWildcards )
  {
    using namespace std::ranges;
    auto result = std::begin( str );
    switch( convertWildcards )
    {
    default:
      [[fallthrough]];
    case ConvertWildcards::No:
      transform( str, result, CharUtilT<C>::ToGoodFileChar );
      break;
    case ConvertWildcards::Yes:
      transform( str, result, CharUtilT<C>::ToGoodFileCharConvertWildcards );
      break;
    case ConvertWildcards::Remove:
      transform( str, result, CharUtilT<C>::ToGoodFileChar );
      auto [removedBeg, removedEnd] = remove_if( str, CharUtilT<C>::IsWildcardFileChar );
      str.erase( removedBeg, removedEnd );
      break;
    }
  }
  
  static strT GetGoodFileName( const strT& str, ConvertWildcards convertWildcards )
  {
    auto r( str );
    ToGoodFileName( r, convertWildcards );
    return r;
  }

  static void ToUpper( strT& str )
  {
    std::ranges::transform( str, std::begin(str), CharUtilT<C>::ToUpper );
  }

  static void ToLower( strT& str )
  {
    std::ranges::transform( str, std::begin(str), CharUtilT<C>::ToLower );
  }
  
  static strT GetUpper( const strT& str )
  {
    auto r( str );
    ToUpper( r );
    return r;
  }

  static strT GetLower( const strT& str )
  {
    auto r( str );
    ToLower( r );
    return r;
  }

  // Format as DDd:HHh:MMm:SSs
  static strT GetDurationStr( uint64_t totalSeconds, uint64_t minDays = 3uL )
  {
    const auto kSecondsPerHour = uint64_t(60 * 60);
    const auto kHoursPerDay = 24uL;
    const auto kSecondsPerDay = kSecondsPerHour * kHoursPerDay;

    auto totalHours = totalSeconds / kSecondsPerHour;
    auto totalDays = totalHours / kHoursPerDay;

    using namespace std::literals;
    const std::string_view kHhMmSs{ "{:%Hh:%Mm:%Ss}"sv };
    const std::string_view kMmSs  {     "{:%Mm:%Ss}"sv } ;

    // Only include days if there are at least minDays (e.g. 3)
    if( totalDays >= minDays )
    {
      strT duration{ std::format( "{}d:", totalDays ) };
      totalSeconds -= totalDays * kSecondsPerDay;
      std::chrono::seconds sec{ totalSeconds };
      duration += std::vformat( kHhMmSs, std::make_format_args( sec ) );
      return duration;
    }

    // Don't include hours unless there is at least one
    std::string_view timeFormat{ totalHours == 0uL ? kMmSs : kHhMmSs };
    std::chrono::seconds sec{ totalSeconds };
    return std::vformat( timeFormat, std::make_format_args( sec ) );
  }

}; // StrUtilT

////////////////////////////////////////////////////////////////////////////////////////////////////

using StrUtil = StrUtilT<char>;
using StrUtilW = StrUtilT<wchar_t>;

////////////////////////////////////////////////////////////////////////////////////////////////////

// vector of std::string
template< typename C >
class StrListT
{
private:

  using strT = std::basic_string< C >;
  using List = std::vector< strT >;
  
public:

  using value_type      = typename List::value_type;
  using size_type       = typename List::size_type;
  using difference_type = typename List::difference_type;
  using iterator        = typename List::iterator;
  using const_iterator  = typename List::const_iterator;
  using reference       = value_type&;
  using const_reference = const value_type&;
    
public:

  StrListT() = default;
  StrListT( const StrListT& ) = default;
  StrListT( StrListT&& ) = default;
  StrListT& operator=( const StrListT& ) = default;
  StrListT& operator=( StrListT&& ) = default;

  template<typename InIt>
  StrListT( InIt start, InIt end ) : list_( start, end ) {}

  iterator begin()              { return list_.begin(); }
  const_iterator begin() const  { return list_.begin(); }
  iterator end()                { return list_.end(); }
  const_iterator end() const    { return list_.end(); }
  reference front()             { return list_.front(); }
  const_reference front() const { return list_.front(); }
  
  bool empty() const     { return list_.empty(); }
  size_type size() const { return list_.size(); }
	
  void push_back( const strT& str ) { list_.push_back( str ); }
  void clear() { list_.clear(); }

  template< class InIt >
  void insert( iterator where, InIt first, InIt last )
  {
    list_.insert( where, first, last );
  }

  bool find( const strT& str ) const // TODO contains?
  {
    return std::ranges::contains( list_, str );
  }
  
	bool ContainsEmptyStrings() const
	{
    return std::ranges::any_of( list_, []( const auto& str ) { return str.empty(); } );
	}

  size_t GetCharCount() const
  {
    auto sumStrSizes = []( size_t count, const strT& rhs )
      {
        return count + rhs.size();
      };

    // &&& update to std::ranges::accumulate when adopted into the std
    return std::accumulate( begin(), end(), size_t( 0 ), sumStrSizes );
  }

private:

  List list_;	

}; // StrListT

template< typename C >
bool operator == ( const StrListT<C>& lhs, const StrListT<C>& rhs )
{
  if( lhs.size() != rhs.size() )
    return false;
  for( auto i = lhs.begin(), j = rhs.begin(); i != lhs.end(); ++i, ++j )
    if ( *i != *j )
      return false;
  return true;
}

using StrList = StrListT<char>;
using StrListW = StrListT<wchar_t>;

} // PKIsensee

////////////////////////////////////////////////////////////////////////////////////////////////////
