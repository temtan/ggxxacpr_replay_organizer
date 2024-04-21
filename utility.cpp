// utility.cpp

#pragma warning(push)
#  pragma warning( disable: 4005 )
#  include <shlwapi.h>
#pragma warning(pop)

#include "tt_windows_h_include.h"

#include "tt_utility.h"
#include "tt_path.h"
#include "tt_exception.h"

#include "utility.h"

#pragma comment(lib, "shlwapi.lib")

using namespace GGXXACPRReplayOrganizer;


// -- Utility ------------------------------------------------------------
namespace {
  void directory_get_entries( std::vector<std::wstring>& array, const std::wstring& target, bool recursive, std::function<bool ( const std::wstring& )> filter ) {
    if ( NOT( Path::IsDirectory( target ) ) ) {
      return;
    }

    WIN32_FIND_DATAW find_data;

    ::SetLastError( NO_ERROR );
    HANDLE search_handle = ::FindFirstFileExW( (target + L"\\*").c_str(), FindExInfoStandard, &find_data, FindExSearchLimitToDirectories, nullptr, 0 );
    if ( search_handle == INVALID_HANDLE_VALUE ) {
      if ( ::GetLastError() != ERROR_FILE_NOT_FOUND && ::GetLastError() != ERROR_ACCESS_DENIED ) {
        throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::FindFirstFileEx ) );
      }
      return;
    }
    TtUtility::DestructorCall closer( [search_handle] ( void ) { ::FindClose( search_handle ); } );

    do {
      std::wstring tmp = target + L"\\" + find_data.cFileName;
      if ( filter( tmp ) ) {
        array.push_back( tmp );
      }
      if ( NOT( tmp.ends_with( L"\\." ) ) && NOT( tmp.ends_with( L"\\.." ) ) ) {
        if ( recursive && find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
          directory_get_entries( array, tmp, recursive, filter );
        }
      }
      ::SetLastError( NO_ERROR );
    } while ( ::FindNextFileW( search_handle, &find_data ) != 0 );
  };

  void directory_get_ggr_entries( std::vector<std::wstring>& array, const std::wstring& target, bool recursive ) {
    directory_get_entries( array, target, recursive, [] ( const std::wstring& path ) { return path.ends_with( L".ggr" ); } );
  }
}

std::vector<std::wstring>
Utility::DirectoryGetGGREntriesUTF16( const std::wstring& root )
{
  std::vector<std::wstring> array;
  directory_get_ggr_entries( array, root, false );
  return array;
}

std::vector<std::wstring>
Utility::DirectoryGetGGREntriesRecursiveUTF16( const std::wstring& root )
{
  std::vector<std::wstring> array;
  directory_get_ggr_entries( array, root, true );
  return array;
}

std::vector<std::wstring>
Utility::DirectoryGetFoldersUTF16( const std::wstring& folder_path )
{
  std::vector<std::wstring> array;
  auto filter = [] ( const std::wstring& path ) -> bool {
    return Path::IsDirectory( path );
  };
  directory_get_entries( array, folder_path, false, filter );
  return array;
}


template std::wstring Utility::Format<std::wstring>( const std::wstring& );
template std::string Utility::Format<std::string>( const std::string& );


bool
Utility::StringToInteger( const std::wstring& str, unsigned int* ret )
{
  if ( str.empty() ) {
    return false;
  }
  wchar_t* endptr;
  errno = 0;
  *ret = static_cast<unsigned int>( ::wcstoll( str.c_str(), &endptr, 10 ) );
  return *endptr == '\0' && errno == 0;
}

unsigned int
Utility::StringToInteger( const std::wstring& str )
{
  unsigned int ret = 0;
  Utility::StringToInteger( str, &ret );
  return ret;
}


bool
Utility::UserDefaultLocaleIsJapan( void )
{
  return ::GetUserDefaultLCID() == 0x0411;
}

bool
Utility::UserDefaultLocaleIsKorea( void )
{
  return ::GetUserDefaultLCID() == 0x0412;
}

bool
Utility::UserDefaultLocaleIsChina( void )
{
  switch ( GetUserDefaultLCID() ) {
  case 0x0c04: // Hong Kong 
  case 0x1404: // Macao
  case 0x0804: // PRC
  case 0x1004: // Singapore
  case 0x0404: // Taiwan
    return true;
  default: return false;
  }
}

std::wstring
Utility::GetLanguageDirectoryPath( void )
{
  return Path::GetExecutingDirectoryPath() + L"\\lang";
}

std::vector<std::wstring>
Utility::DirectoryGetLanguageDLLUTF16( void )
{
  std::vector<std::wstring> array;
  std::wstring target = Utility::GetLanguageDirectoryPath();
  WIN32_FIND_DATAW find_data;

  ::SetLastError( NO_ERROR );
  HANDLE search_handle = ::FindFirstFileExW( (target + L"\\*.dll").c_str(), FindExInfoStandard, &find_data, FindExSearchLimitToDirectories, nullptr, 0 );
  if ( search_handle == INVALID_HANDLE_VALUE ) {
    if ( ::GetLastError() != ERROR_FILE_NOT_FOUND && ::GetLastError() != ERROR_ACCESS_DENIED ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::FindFirstFileEx ) );
    }
    return array;
  }
  TtUtility::DestructorCall closer( [search_handle] ( void ) { ::FindClose( search_handle ); } );

  do {
    array.push_back( target + L"\\" + find_data.cFileName );
    ::SetLastError( NO_ERROR );
  } while ( ::FindNextFileW( search_handle, &find_data ) != 0 );

  return array;
}


std::wstring
Utility::ExpandEnvironmentStringUTF16( const std::wstring str )
{
  const size_t buffer_length = 1024 * 4;
  wchar_t buf[buffer_length];
  auto ret = ::ExpandEnvironmentStringsW( str.c_str(), buf, buffer_length );
  if ( ret <= buffer_length ) {
    return buf;
  }
  auto heap_buf = std::make_unique<wchar_t[]>( ret + 1 );
  ::ExpandEnvironmentStringsW( str.c_str(), heap_buf.get(), ret + 1 );
  return heap_buf.get();
}


// -- Path ---------------------------------------------------------------
bool
Path::IsDirectory( const std::wstring& path )
{
  return ::PathIsDirectoryW( path.c_str() );
}

bool
Path::FileExists( const std::wstring& path )
{
  return ::PathFileExistsW( path.c_str() );
}


std::wstring
Path::EditPath( const std::wstring& path, bool f_drive, bool f_dir, bool f_fname, bool f_ext )
{
  wchar_t drive[MAX_PATH], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
  _wsplitpath_s( path.c_str(), drive, sizeof( drive ), dir, sizeof( dir ), fname, sizeof( fname ), ext, sizeof( ext ) );

  std::wstring tmp;
  tmp.append( f_drive ? drive : L"" );
  tmp.append( f_dir   ? dir   : L"" );
  tmp.append( f_fname ? fname : L"" );
  tmp.append( f_ext   ? ext   : L"" );
  return tmp;
}



std::wstring
Path::DirName( const std::wstring& path )
{
  std::wstring tmp = Path::EditPath( path, true, true, false, false );
  if ( tmp.size() == 0 ) {
    tmp = L".";
  }
  else if ( tmp[tmp.size() - 1] == L'\\' || tmp[tmp.size() - 1] == L'/' ) {
    tmp.erase( tmp.size() - 1 );
  }
  return tmp;
}

std::wstring
Path::BaseName( const std::wstring& path, const std::wstring& suffix )
{
  return Path::EditPath( path, false, false, true, (suffix.empty() || NOT( path.ends_with( suffix ) )) );
}


std::wstring
Path::GetExecutingFilePath( void )
{
  wchar_t buf[MAX_PATH + 1];
  ::GetModuleFileNameW( nullptr, buf, sizeof( buf ) );
  return std::wstring( buf );
}


std::wstring
Path::GetExecutingDirectoryPath( void )
{
  return Path::DirName( Path::GetExecutingFilePath() );
}

