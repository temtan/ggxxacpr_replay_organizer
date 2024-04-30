// common.h

#pragma once

#include "tt_icon.h"
#include "tt_exception.h"


namespace GGXXACPRReplayOrganizer {
  const char* const VERSION = "1.0.1";
  const char* const APPLICATION_NAME = "GGXXACPR Replay Organizer";

  namespace Const {
  }

  // -- CommandID --------------------------------------------------------
  namespace CommandID {
    enum ID : int {
      // ƒtƒ@ƒCƒ‹
      Close = 2000,
    };

    namespace Control {
      enum ID : int {
        LanguageComboBox = 3000,

        ReplayFolderAutoButton,
        ReplayFolderReferenceButton,

        SelfNameAutoButton,

        AnalyzeButton,

        DateMoveButton,

        OpponentSearchButton,
        OpponentCopyButton,
        OpponentLimitCheck,

        OpponentList,
      };
    }
  }


  // -- IniFileOperation -------------------------------------------------
  namespace IniFileOperation {
    void SavePlacement( const WINDOWPLACEMENT& placement );
    bool LoadPlacement( WINDOWPLACEMENT& placement );

    void SaveLanguage( const std::string& language );
    std::string LoadLanguage( void );

    void SaveReplayFolderPath( const std::wstring& path );
    std::wstring LoadReplayFolderPath( void );

    void SaveDateTtemplate( const std::wstring& date_template );
    std::wstring LoadDateTemplate( void );

    void SaveSelfName( const std::wstring& self_name );
    std::wstring LoadSelfName( void );

    void SaveOpponentLowerLimit( bool flag );
    bool LoadOpponentLowerLimit( void );

    void SaveOpponentLowerLimitNumber( int number );
    int LoadOpponentLowerLimitNumber( void );

    void SaveAnalyzeDialogPlacement( const WINDOWPLACEMENT& placement );
    bool LoadAnalyzeDialogPlacement( WINDOWPLACEMENT& placement );

    void SaveErrorLogDump( TtException& e );
  }

  namespace Image {
    // -- Index ------------------------------------------------------------
    struct Index {
      enum Value : int {
        None = -1,
        Main,
        MainSmall,
      };
    };

    // -- Objects
    extern std::vector<TtIcon> ICONS;

    void Initialize( void );
  }
}
