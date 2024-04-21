// string_table.h

#pragma once

#include "string_table_id.h"


namespace GGXXACPRReplayOrganizer::StringTable {
  class ID {
  public:
    ID( unsigned int number );
    std::wstring Get( void ) const;

    unsigned int number_;
  };

  std::wstring GetUserDefaultLocaleString( void );
  void Initialize( const std::wstring& language );
  std::wstring GetCurrentLanguage( void );
  std::wstring Get( ID id );

  // ---------------------------------------------------------------------
  // main frame
  namespace Main {
    static const ID Language                    = RSID_STR_MAIN_LANGUAGE;
    static const ID ReplayFolderGroup           = RSID_STR_MAIN_REPLAY_FOLDER_GROUP;
    static const ID ReplayFolderAutoButton      = RSID_STR_MAIN_REPLAY_FOLDER_AUTO;
    static const ID ReplayFolderReferenceButton = RSID_STR_MAIN_REPLAY_FOLDER_REF_BUTTON;
    static const ID ReplayFolderDoneLabel       = RSID_STR_MAIN_REPLAY_FOLDER_DONE_LABEL;
    static const ID SelfNameGroup               = RSID_STR_MAIN_SELF_NAME_GROUP;
    static const ID SelfNameAutoButton          = RSID_STR_MAIN_SELF_NAME_AUTO_BUTTON;
    static const ID AnalyzeButton               = RSID_STR_MAIN_ANALYZE_BUTTON;
    static const ID DateTemplateGroup           = RSID_STR_MAIN_DATE_TEMPLATE_GROUP;
    static const ID DateTemplateTitle           = RSID_STR_MAIN_DATE_TEMPLATE_TITLE;
    static const ID DateTemplateHistory         = RSID_STR_MAIN_DATE_TEMPLATE_HISTORY;
    static const ID DateTemplateNotice          = RSID_STR_MAIN_DATE_TEMPLATE_NOTICE;
    static const ID DateMoveButton              = RSID_STR_MAIN_DATE_MOVE_BUTTON;
    static const ID OpponentSearchButton        = RSID_STR_MAIN_OPPONENT_SEARCH_BUTTON;
    static const ID OpponentCopyButton          = RSID_STR_MAIN_OPPONENT_COPY_BUTTON;
    static const ID OpponentLimitCheck          = RSID_STR_MAIN_OPPONENT_LIMIT_CHECK;
    static const ID OpponentListColumnName      = RSID_STR_MAIN_OPPONENT_LIST_COLUMN_NAME;
    static const ID OpponentListColumnCount     = RSID_STR_MAIN_OPPONENT_LIST_COLUMN_COUNT;

    static const ID ReplayFolderReferenceFolderDialogDescription = RSID_STR_MAIN_REPLAY_FOLDER_REF_FOLDER_DIALOG_DESCRIPTION;

    static const ID AutoInputConfirmCaption    = RSID_STR_MAIN_AUTO_INPUT_CONFIRM_CAP;
    static const ID AutoInputConfirmMessage    = RSID_STR_MAIN_AUTO_INPUT_CONFIRM_MSG;
    static const ID DateMoveConfirmCaption     = RSID_STR_MAIN_DATE_MOVE_CONFIRM_CAP;
    static const ID DateMoveConfirmMessage     = RSID_STR_MAIN_DATE_MOVE_CONFIRM_MSG;
    static const ID DateMoveDoneCaption        = RSID_STR_MAIN_DATE_MOVE_DONE_CAP;
    static const ID DateMoveDoneMessage        = RSID_STR_MAIN_DATE_MOVE_DONE_MSG;
    static const ID OpponentCopyConfirmCaption = RSID_STR_MAIN_OPPONENT_COPY_CONFIRM_CAP;
    static const ID OpponentCopyConfirmMessage = RSID_STR_MAIN_OPPONENT_COPY_CONFIRM_MSG;
    static const ID OpponentCopyDoneCaption    = RSID_STR_MAIN_OPPONENT_COPY_DONE_CAP;
    static const ID OpponentCopyDoneMessage    = RSID_STR_MAIN_OPPONENT_COPY_DONE_MSG;
    static const ID SelfNameAutoConfirmCaption = RSID_STR_MAIN_SELF_NAME_AUTO_CONFIRM_CAP;
    static const ID SelfNameAutoConfirmMessage = RSID_STR_MAIN_SELF_NAME_AUTO_CONFIRM_MSG;
    static const ID ReplayFolderIsEmptyCaption = RSID_STR_MAIN_REPLAY_FOLDER_IS_EMPTY_CAP;
    static const ID ReplayFolderIsEmptyMessage = RSID_STR_MAIN_REPLAY_FOLDER_IS_EMPTY_MSG;
    static const ID ReplayFileIsEmptyCaption   = RSID_STR_MAIN_REPLAY_FILE_IS_EMPTY_CAP;
    static const ID ReplayFileIsEmptyMessage   = RSID_STR_MAIN_REPLAY_FILE_IS_EMPTY_MSG;
    static const ID SelfNameIsEmptyCaption     = RSID_STR_MAIN_SELF_NAME_IS_EMPTY_CAP;
    static const ID SelfNameIsEmptyMessage     = RSID_STR_MAIN_SELF_NAME_IS_EMPTY_MSG;
  }

  // ---------------------------------------------------------------------
  // analyze dialog
  namespace Analyze {
    static const ID AllCharacter = RSID_STR_ANALYZE_ALL_CHARACTER;

    namespace Column {
      static const ID Opponent        = RSID_STR_ANALYZE_LIST_COLUMN_OPPONENT;
      static const ID CountOfMatches  = RSID_STR_ANALYZE_LIST_COLUMN_COUNT_OF_MATCHES;
      static const ID Win             = RSID_STR_ANALYZE_LIST_COLUMN_WIN;
      static const ID WinningRate     = RSID_STR_ANALYZE_LIST_COLUMN_WINNING_RATE;
      static const ID Lose            = RSID_STR_ANALYZE_LIST_COLUMN_LOSE;
      static const ID Draw            = RSID_STR_ANALYZE_LIST_COLUMN_DRAW;
      static const ID Unknown         = RSID_STR_ANALYZE_LIST_COLUMN_UNKNOWN;
      static const ID FirstPlayer     = RSID_STR_ANALYZE_LIST_COLUMN_1P;
      static const ID SecondPlayer    = RSID_STR_ANALYZE_LIST_COLUMN_2P;
      static const ID Date            = RSID_STR_ANALYZE_LIST_COLUMN_DATE;
      static const ID FirstPlayerWin  = RSID_STR_ANALYZE_LIST_COLUMN_1P_WIN;
      static const ID SecondPlayerWin = RSID_STR_ANALYZE_LIST_COLUMN_2P_WIN;
    }

    namespace Combo {
      static const ID AllCharacter = RSID_STR_ANALYZE_COMBO_ALL_CHARACTER;
      static const ID ByOpponent   = RSID_STR_ANALYZE_COMBO_BY_OPPONENT;
    }

    static const ID LoadingLabel   = RSID_STR_ANALYZE_LOADING_LABEL;
    static const ID UsingCharacter = RSID_STR_ANALYZE_USING_CARACTER;

    static const ID ReplayFileLoadingErrorCaption = RSID_STR_ANALYZE_REPLAY_FILE_LOADING_ERROR_CAP;
    static const ID ReplayFileLoadingErrorMessage = RSID_STR_ANALYZE_REPLAY_FILE_LOADING_ERROR_MSG;
  }

  // ---------------------------------------------------------------------
  namespace Message {
    static const ID InternalError              = RSID_STR_MSG_INTERNAL_ERROR;
    static const ID UnexpectedError            = RSID_STR_MSG_UNEXPECTED_ERROR;
    static const ID UnexpectedErrorCaption     = RSID_STR_MSG_UNEXPECTED_ERROR_CAP;
    static const ID LanguageChangeError        = RSID_STR_MSG_LANGUAGE_CHANGE_ERROR;
    static const ID LanguageChangeErrorCaption = RSID_STR_MSG_LANGUAGE_CHANGE_ERROR_CAP;

    static const ID ErrorCaption       = RSID_STR_MSG_ERROR_CAPTION;
    static const ID FileOperationError = RSID_STR_MSG_FILE_OPERATION_ERROR;
  }
}

// ó™èÃ
namespace GGXXACPRReplayOrganizer {
  namespace StrT = StringTable;
}
