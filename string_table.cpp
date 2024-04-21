// string_table.cpp

#include "utility.h"
#include "exception.h"

#include "string_table.h"

using namespace GGXXACPRReplayOrganizer;


// -- StringTable --------------------------------------------------------
StringTable::ID::ID( unsigned int number ) :
number_( number )
{
}


std::wstring
StringTable::ID::Get( void ) const
{
  return StringTable::Get( *this );
}


namespace {
  std::optional<HMODULE> lib;
  std::wstring           current_language;
}

std::wstring
StringTable::GetUserDefaultLocaleString( void )
{
  if ( GGXXACPRReplayOrganizer::Utility::UserDefaultLocaleIsJapan() ) {
    return L"japanese";
  }
  if ( GGXXACPRReplayOrganizer::Utility::UserDefaultLocaleIsKorea() ) {
    return L"korean";
  }
  if ( GGXXACPRReplayOrganizer::Utility::UserDefaultLocaleIsChina() ) {
    return L"chinese_traditional";
  }
  return L"english";
}

void
StringTable::Initialize( const std::wstring& language )
{
  std::wstring dll_name = language;
  if ( language.empty() ) {
    dll_name = StringTable::GetUserDefaultLocaleString();
  }
  std::wstring path = Utility::GetLanguageDirectoryPath() + L"\\" + dll_name + L".dll";

  HMODULE tmp = ::LoadLibraryW( path.c_str() );
  if ( tmp == NULL ) {
    throw LanguageException( language );
  }
  if ( lib ) {
    ::FreeLibrary( lib.value() );
  }
  lib = tmp;
  current_language = language;
}

std::wstring
StringTable::GetCurrentLanguage( void )
{
  return current_language;
}

std::wstring
StringTable::Get( ID id )
{
  if ( lib ) {
    for ( size_t buf_size = 1024; ; ) {
      auto buf = std::make_unique<wchar_t[]>( buf_size );
      int ret = ::LoadStringW( lib.value(), id.number_, buf.get(), static_cast<int>( buf_size ) );
      if ( ret == 0 ) {
        break;
      }
      if ( ret < static_cast<int>( buf_size - 2 ) ) {
        return buf.get();
      }
      buf_size = static_cast<size_t>( buf_size * 1.5 );
    }
  }
  // ここは生メッセージ
  if ( Utility::UserDefaultLocaleIsJapan() ) {
    return L"メッセージの取得に失敗しました。ID = " + std::format( L"{}", id.number_ );
  }
  else {
    return L"Failed to get the message. ID = " + std::format( L"{}", id.number_ );
  }
}
