// utility.h

#pragma once

#include <vector>
#include <string>

#include "ttl_define.h"


namespace GGXXACPRReplayOrganizer::Utility {
  std::vector<std::wstring> DirectoryGetGGREntriesUTF16( const std::wstring& root );
  std::vector<std::wstring> DirectoryGetGGREntriesRecursiveUTF16( const std::wstring& root );
  std::vector<std::wstring> DirectoryGetFoldersUTF16( const std::wstring& folder_path );

  bool StringToInteger( const std::wstring& str, unsigned int* ret );
  unsigned int StringToInteger( const std::wstring& str );

  bool UserDefaultLocaleIsJapan( void );
  bool UserDefaultLocaleIsKorea( void );
  bool UserDefaultLocaleIsChina( void );
  std::wstring GetLanguageDirectoryPath( void );
  std::vector<std::wstring> DirectoryGetLanguageDLLUTF16( void );

  std::wstring ExpandEnvironmentStringUTF16( const std::wstring str );

  // -- std::format ŒÝŠ· -------------------------------------------------
  template <class... Args>
  auto MakeFormatArguments( const std::string&, Args&&... args ) {
    return std::make_format_args( args... );
  }

  template <class... Args>
  auto MakeFormatArguments( const std::wstring&, Args&&... args ) {
    return std::make_wformat_args( args... );
  }

  template <class STRING_TYPE, class... Args>
  STRING_TYPE Format( const STRING_TYPE& format, Args&&... args ) {
    return std::vformat( format, MakeFormatArguments( format, args... ) );
  }

  template <class STRING_TYPE, class... Args>
  STRING_TYPE Format( const typename STRING_TYPE::value_type* format, Args&&... args ) {
    return Format( STRING_TYPE( format ), args... );
  }
}


namespace GGXXACPRReplayOrganizer::Path {
  bool IsDirectory( const std::wstring& path );
  bool FileExists( const std::wstring& path );

  std::wstring EditPath( const std::wstring& path, bool f_drive, bool f_dir, bool f_fname, bool f_ext );
  std::wstring DirName( const std::wstring& path );
  std::wstring BaseName( const std::wstring& path, const std::wstring& suffix = L"" );

  std::wstring GetExecutingFilePath( void );
  std::wstring GetExecutingDirectoryPath( void );
}
