// core.cpp

#include <regex>
#include <format>

#include "tt_time.h"

#include "utility.h"
#include "exception.h"

#include "core.h"

#pragma comment(lib, "Shell32.lib")

using namespace GGXXACPRReplayOrganizer;


namespace {
  void move_file( const std::wstring& from, const std::wstring& to ) {
    if ( ::MoveFileW( from.c_str(), to.c_str() ) == 0 ) {
      throw FileOperationException( from, to, L"MoveFile" );
    }
  }

  void copy_file( const std::wstring& from, const std::wstring& to, bool fail_if_exists ) {
    if ( ::CopyFileW( from.c_str(), to.c_str(), fail_if_exists ) == 0 ) {
      throw FileOperationException( from, to, L"CopyFile" );
    }
  }

  bool make_directory_of_file_if_not_exist( const std::wstring& path ) {
    std::wstring dir_path = Path::DirName( path );
    if ( NOT( Path::IsDirectory( dir_path ) ) ) {
      return ::SHCreateDirectoryExW( nullptr, dir_path.c_str(), nullptr ) == ERROR_SUCCESS;
    }
    return true;
  }

  const std::wstring history_folder_name = L"history";
  const std::wstring player_folder_name = L"player";
};



// -- Core ---------------------------------------------------------------
std::wstring
Core::ApplyDateTemplate( const std::wstring& path_template, unsigned int year, unsigned int month, unsigned int day )
{
  static std::wregex year_regex( L"YYYY" );
  static std::wregex month_regex( L"MM" );
  static std::wregex day_regex( L"DD" );

  std::wstring result = path_template;
  result = std::regex_replace( result, year_regex,  std::format( L"{:04d}", year ) );
  result = std::regex_replace( result, month_regex, std::format( L"{:02d}", month ) );
  result = std::regex_replace( result, day_regex,   std::format( L"{:02d}", day ) );
  return result;
}

std::wstring
Core::ApplyDateTemplateToday( const std::wstring& path_template )
{
  TtTime now = TtTime::GetNow();
  return ApplyDateTemplate( path_template, now.GetYear(), now.GetMonth(), now.GetDayOfMonth() );
}


std::unordered_map<std::wstring, bool>
Core::GetOpponentNamesFromPlayerFolder( const std::wstring& replay_folder_path )
{
  std::wstring player_folder_path = replay_folder_path + L"\\" + player_folder_name;
  make_directory_of_file_if_not_exist( player_folder_path );

  std::unordered_map<std::wstring, bool> tmp;
  for ( auto& path : Utility::DirectoryGetFoldersUTF16( replay_folder_path + L"\\" + player_folder_name ) ) {
    tmp[Path::BaseName( path )] = true;
  }
  return tmp;
}


Core::Core( void )
{
}

void
Core::SearchReplayFiles( const std::wstring& replay_folder_path )
{
  auto replay_file_path_array = Utility::DirectoryGetGGREntriesUTF16( replay_folder_path );
  {
    std::wstring history_folder_path = replay_folder_path + L"\\" + history_folder_name;
    if ( Path::IsDirectory( history_folder_path ) ) {
      auto tmp = Utility::DirectoryGetGGREntriesRecursiveUTF16( history_folder_path );
      std::copy( tmp.begin(), tmp.end(), std::back_inserter( replay_file_path_array ) );
    }
  }

  replay_files_.clear();
  for ( auto& path : replay_file_path_array ) {
    std::optional<ReplayFile> tmp = ReplayFile::Parse( path );
    if ( tmp ) {
      replay_files_.push_back( tmp.value() );
    }
  }
}


void
Core::MoveReplayFiles( const std::wstring& replay_folder_path, const std::wstring& path_template )
{
  std::wstring new_path_base = replay_folder_path + L"\\" + history_folder_name + L"\\";

  for ( auto& replay_file : replay_files_ ) {
    std::wstring new_path( new_path_base );
    new_path.append( this->ApplyDateTemplate( path_template, replay_file.year_, replay_file.month_, replay_file.day_ ) );
    new_path.append( L"\\" + Path::BaseName( replay_file.path_ ) );

    if ( new_path == replay_file.path_ ) {
      continue;
    }
    make_directory_of_file_if_not_exist( new_path );
    move_file( replay_file.path_, new_path );
    if ( replay_file_moved_ ) {
      replay_file_moved_( replay_file.path_, new_path );
    }
    replay_file.path_ = new_path;
  }
}


namespace {
  std::list<Core::PlayerCount> hash_to_sorted_list( std::unordered_map<std::wstring, Core::PlayerCount>& hash ) {
    std::list<Core::PlayerCount> tmp;
    for ( auto& [name_, counter] : hash ) {
      for ( auto it = tmp.begin(); ; ++it ) {
        if ( it == tmp.end() ) {
          tmp.push_back( counter );
          break;
        }
        if ( counter.count_ >= it->count_ ) {
          tmp.insert( it, counter );
          break;
        }
      }
    }
    return tmp;
  }
}


std::list<Core::PlayerCount>
Core::GetAllPlayerCountListFromReplayFiles( void )
{
  std::unordered_map<std::wstring, PlayerCount> hash;

  for ( auto& replay_file : replay_files_ ) {
    auto register_player = [&hash] ( const std::wstring& name ) {
      if ( NOT( hash.contains( name ) ) ) {
        hash[name].name_ = name;
      }
      hash[name].count_ += 1;
    };
    register_player( replay_file.first_player_name_ );
    register_player( replay_file.second_player_name_ );
  }

  return hash_to_sorted_list( hash );
}


std::list<Core::PlayerCount>
Core::GetOpponentPlayerCountListReplayFiles( const std::wstring& self_player_name )
{
  std::unordered_map<std::wstring, PlayerCount> hash;
  for ( auto& replay_file : replay_files_ ) {
    auto register_player = [&hash] ( const std::wstring& name ) {
      if ( NOT( hash.contains( name ) ) ) {
        hash[name].name_ = name;
      }
      hash[name].count_ += 1;
    };

    if ( replay_file.first_player_name_ == self_player_name ) {
      register_player( replay_file.second_player_name_ );
    }
    if ( replay_file.second_player_name_ == self_player_name ) {
      register_player( replay_file.first_player_name_ );
    }
  }
  return hash_to_sorted_list( hash );
}


void
Core::CopyReplayFilesOpponentPlayerForEachFolder( const std::wstring& replay_folder_path,
                                                  const std::wstring& self_player_name,
                                                  const std::vector<std::wstring>& opponent_list )
{
  std::wstring path_base = replay_folder_path + L"\\" + player_folder_name + L"\\";
  std::unordered_map<std::wstring, std::wstring> opponent_to_folder_path;
  for ( auto& opponent : opponent_list ) {
    opponent_to_folder_path[opponent] = path_base + opponent;
  }
  for ( auto& replay_file : replay_files_ ) {
    auto copy_replay_file = [&] ( const std::wstring& opponent, ReplayFile& replay_file ) {
      if ( opponent_to_folder_path.contains( opponent ) ) {
        auto new_path = opponent_to_folder_path[opponent] + L"\\" + Path::BaseName( replay_file.path_ );
        if ( NOT( Path::FileExists( new_path ) ) ) {
          make_directory_of_file_if_not_exist( new_path );
          copy_file( replay_file.path_, new_path, true );
        }
      }
    };

    if ( replay_file.first_player_name_ == self_player_name ) {
      copy_replay_file( replay_file.second_player_name_, replay_file );
    }
    if ( replay_file.second_player_name_ == self_player_name ) {
      copy_replay_file( replay_file.first_player_name_, replay_file );
    }
  }
}
