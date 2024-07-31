////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  TestString.cpp
//
//  Copyright © Pete Isensee (PKIsensee@msn.com).
//  All rights reserved worldwide.
//
//  Permission to copy, modify, reproduce or redistribute this source code is granted provided 
//  the above copyright notice is retained in the resulting source code.
// 
//  This software is provided "as is" and without any express or implied warranties.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CharUtil.h"
#include "StrUtil.h"
#include <cassert>

using namespace PKIsensee;

extern "C" { __declspec( dllimport ) void __stdcall DebugBreak(); }

// Macros
#ifdef _DEBUG
#define test(e) assert(e)
#else
#define test(e) static_cast<void>( (e) || ( DebugBreak(), 0 ) )
#endif

void TestChar()
{
  test( CharUtil::ForwardSlashToBackslash( '/' ) == '\\' );
  test( CharUtilW::ForwardSlashToBackslash( L'b' ) == L'b' );
  test( CharUtil::IsAlpha( 'a' ) );
  test( CharUtilW::IsAlpha( L'a' ) );
  test( !CharUtil::IsAlpha( '1' ) );
  test( CharUtil::IsPrintable( 'x' ) );
  test( !CharUtilW::IsPrintable( 1 ) );
  test( CharUtilW::IsExtendedAscii( 0xAAAA ) );
  test( !CharUtilW::IsExtendedAscii( L'Z' ) );
  test( CharUtil::IsExtendedAscii( -1 ) );
  test( !CharUtil::IsExtendedAscii( 'a' ) );
  test( !CharUtil::IsGoodFileChar( ':' ) );
  test( CharUtil::IsGoodFileChar( 'a' ) );
  test( !CharUtil::IsGoodFileChar( 1 ) );
  test( CharUtil::IsGoodFileCharEx( 'z', CharUtil::AllowWildcards::No ) );
  test( !CharUtil::IsGoodFileCharEx( '?', CharUtil::AllowWildcards::No ) );
  test( CharUtil::IsWildcardFileChar( '*' ) );
  test( !CharUtil::IsWildcardFileChar( '!' ) );
  test( CharUtil::ToGoodFileChar( 'a' ) == 'a' );
  test( CharUtil::ToGoodFileChar( '<' ) == '(' );
  test( CharUtil::ToGoodFileChar( '?' ) == '?' );
  test( CharUtil::ToGoodFileCharConvertWildcards( 1 ) == '!' );
  test( CharUtil::ToGoodFileCharConvertWildcards( '*' ) == '+' );
  test( CharUtil::ToGoodFileCharConvertWildcards( 'z' ) == 'z' );
  test( CharUtil::ToGoodFileCharEx( '*', CharUtil::ConvertWildcards::No ) == '*' );
  test( CharUtil::ToUpper( 'a' ) == 'A' );
  test( CharUtil::ToLower( 'A' ) == 'a' );
  test( CharUtilT<char>::ToUpper( 'a' ) == 'A' );
  test( CharUtilT<wchar_t>::ToLower( L'A' ) == L'a' );
  test( CharUtilW::ToUpper( L'a' ) == L'A' );
  test( CharUtilW::ToLower( L'A' ) == L'a' );
}

void TestString()
{
  test( StringUtil::GetUtf8( L"" ) == std::string( "" ) );
  test( StringUtil::GetUtf8( L"widestring" ) == std::string( "widestring" ) );
  test( StringUtil::GetUtf16( "" ) == std::wstring( L"" ) );
  test( StringUtil::GetUtf16( "narrowstring" ) == std::wstring( L"narrowstring" ) );

  std::string xml( "&" );
  StrUtil::ToXmlSafe( xml );
  test( xml == "&amp;" );
  test( StrUtil::GetXmlSafe( "&" ) == "&amp;" );
  test( StrUtilW::GetXmlSafe( L"\"" ) == L"&quot;" );
    
  std::string trim( "  abc  " );
  StrUtil::ToTrimmed( trim, " " );
  test( trim == "abc" );
  test( StrUtil::GetTrimmed( "  abc  ", " " ) == "abc" );
  test( StrUtil::GetTrimmed( "", " " ) == "" );

  trim.assign( "abc " );
  StrUtil::ToTrimmed( trim, " " );
  test( trim == "abc" );
  test( StrUtil::GetTrimmed( "abc    ", " " ) == "abc" );
  test( StrUtil::GetTrimmed( "abc", " " ) == "abc" );

  trim.assign( "abc abc xyz cbbacbaa cccc" );
  StrUtil::ToTrimmed( trim, "abc " );
  test( trim == "xyz" );
  test( StrUtil::GetTrimmed( "abc abc xyz cbbacbaa cccc", "abc " ) == "xyz" );
    
  trim.assign( "  abc  " );
  StrUtil::ToTrimmedLeading( trim, " " );
  test( trim == "abc  " );
  test( StrUtil::GetTrimmedLeading( "  abc  ", " " ) == "abc  " );
  test( StrUtil::GetTrimmedLeading( "", " " ) == "" );
    
  trim.assign( "abc abc xyz cbbacbaa cccc" );
  StrUtil::ToTrimmedLeading( trim, "abc " );
  test( trim == "xyz cbbacbaa cccc" );
  test( StrUtil::GetTrimmedLeading( "abc abc xyz cbbacbaa cccc", "abc " ) == "xyz cbbacbaa cccc" );
    
  trim.assign( "    " );
  StrUtil::ToTrimmedTrailing( trim, " " );
  test( trim == "" );
  test( StrUtil::GetTrimmedTrailing( "    ", " " ) == "" );
  test( StrUtil::GetTrimmedTrailing( "", " " ) == "" );

  trim.assign( "  abc  " );
  StrUtil::ToTrimmedTrailing( trim, " " );
  test( trim == "  abc" );
  test( StrUtil::GetTrimmedTrailing( "  abc  ", " " ) == "  abc" );
  test( StrUtil::GetTrimmedTrailing( "", " " ) == "" );

  trim.assign( "abc abc xyz cbbacbaa cccc" );
  StrUtil::ToTrimmedTrailing( trim, "abc " );
  test( trim == "abc abc xyz" );
  test( StrUtil::GetTrimmedTrailing( "abc abc xyz cbbacbaa cccc", "abc " ) == "abc abc xyz" );

  test( StrUtil::IsNumeric( "1234" ) );
  test( !StrUtil::IsNumeric( "" ) );
  test( !StrUtil::IsNumeric( "-12.34" ) );

  test( StrUtil::IsAlphaNum( "abcABC1234" ) );
  test( !StrUtil::IsAlphaNum( "" ) );
  test( !StrUtil::IsAlphaNum( "-12.34" ) );
  test( !StrUtil::IsAlphaNum( "Body-Double" ) );

  test( StrUtil::IsPrintable( "abc" ) );
  test( !StrUtil::IsPrintable( std::string( 1, char(1) ) ) );

  test( !StrUtil::IsExtendedAscii( "abc" ) );
  test( StrUtilW::IsExtendedAscii( std::wstring( 1, 0xAA ) ) );

  test( !StrUtil::IsGoodFileName( "<jlo>", StrUtil::AllowWildcards::Yes ) );
  test( StrUtil::IsGoodFileName( "jlo", StrUtil::AllowWildcards::Yes ) );
  test( !StrUtilW::IsGoodFileName( L"con*.jpg", StrUtilW::AllowWildcards::No ) );
  test( StrUtilW::IsGoodFileName( L"co?.jpg", StrUtilW::AllowWildcards::Yes ) );

  test( StrUtil::ContainsWildcard( "j*" ) );
  test( StrUtil::ContainsWildcard( "?j" ) );
  test( !StrUtil::ContainsWildcard( "klm" ) );

  std::string file( "<jlo>.bad" );
  StrUtil::ToGoodFileName( file, StrUtil::ConvertWildcards::Yes );
  test( file == "(jlo).bad" );
  test( StrUtil::GetGoodFileName( "<jlo>.bad", StrUtil::ConvertWildcards::No ) == "(jlo).bad" );
  test( StrUtil::GetGoodFileName( "<jlo*>.bad", StrUtil::ConvertWildcards::No ) == "(jlo*).bad" );
  test( StrUtilW::GetGoodFileName( L"<jlo?>.bad", StrUtilW::ConvertWildcards::Yes ) == L"(jlo ).bad" );
  test( StrUtilW::GetGoodFileName( L"<jlo?>.bad", StrUtilW::ConvertWildcards::Remove ) == L"(jlo).bad" );
  test( StrUtilW::GetGoodFileName( L"<*jlo>.bad", StrUtilW::ConvertWildcards::Remove ) == L"(jlo).bad" );

  std::string up( "aBcDeFGhiJKL123" );
  StrUtil::ToUpper( up );
  test( up == "ABCDEFGHIJKL123" );
  test( StrUtil::GetUpper( "aBcDeFGhiJKL123" ) == "ABCDEFGHIJKL123" );
    
  StrUtil::ToLower( up );
  test( up == "abcdefghijkl123" );
  test( StrUtil::GetLower( "aBcDeFGhiJKL123" ) == "abcdefghijkl123" );

  test( StrUtil::GetDurationStr( 123456789 ) == "1428d:21h:33m:09s" );
  test( StrUtil::GetDurationStr( 123456 ) == "34h:17m:36s" );
  test( StrUtil::GetDurationStr( 1 ) == "00m:01s" );
}

void TestStrList()
{
  StrList a;
  a.push_back( "abc" );
  test( a.front() == "abc" );
  a.front() = "xyz";
  test( a.front() == "xyz" );
  for( const auto &i : a )
    test( i == "xyz" );
  test( !a.ContainsEmptyStrings() );
  test( a.GetCharCount() == 3 );
  test( !a.empty() );
  a.insert( a.begin(), a.begin(), a.end() );
  test( a.GetCharCount() == 6 );
  a.push_back( "" );
  test( a.ContainsEmptyStrings() );
  test( a.find( "xyz" ) );
  test( a.find( "" ) );
  test( !a.find( "abc" ) );
  StrList b;
  test( a != b );
  b = a;
  test( a == b );
  b.front() = "zzz";
  test( a != b );
}

int __cdecl main()
{
  TestChar();
  TestString();
  TestStrList();
}

////////////////////////////////////////////////////////////////////////////////////////////////////