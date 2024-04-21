// core.h

#pragma once

#include <string>
#include <unordered_map>

#include "base.h"


namespace GGXXACPRReplayOrganizer {
  // -- Core -------------------------------------------------------------
  class Core {
  public:
    static std::wstring ApplyDateTemplate( const std::wstring& path_template, unsigned int year, unsigned int month, unsigned int day );
    static std::wstring ApplyDateTemplateToday( const std::wstring& path_template );

    static std::unordered_map<std::wstring, bool> GetOpponentNamesFromPlayerFolder( const std::wstring& replay_folder_path );

  public:
    explicit Core( void );

    void SearchReplayFiles( const std::wstring& replay_folder_path );

    void MoveReplayFiles( const std::wstring& replay_folder_path, const std::wstring& path_template );

    struct PlayerCount {
      std::wstring name_;
      unsigned int count_;
    };
    std::list<PlayerCount> GetAllPlayerCountListFromReplayFiles( void );
    std::list<PlayerCount> GetOpponentPlayerCountListReplayFiles( const std::wstring& self_player_name );

    void CopyReplayFilesOpponentPlayerForEachFolder( const std::wstring& replay_folder_path,
                                                     const std::wstring& self_player_name,
                                                     const std::vector<std::wstring>& opponent_list );

  public:
    std::vector<ReplayFile> replay_files_;

    // callback
    std::function<void ( const std::wstring& from, const std::wstring& to )> replay_file_moved_;
  };
}
