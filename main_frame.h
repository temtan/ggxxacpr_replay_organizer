// main_frame.h

#pragma once

#include "tt_window_controls.h"

#include "utf16_window.h"
#include "core.h"


namespace GGXXACPRReplayOrganizer {
  // -- MainFrame --------------------------------------------------------
  class MainFrame : public UTF16Form {
  public:
    explicit MainFrame( void );

    virtual DWORD GetStyle( void ) override;
    virtual DWORD GetExtendedStyle( void ) override;
    virtual bool  Created( void ) override;

    void ResetTextOfControls( void );
    void ResetLanguageList( void );

    void RegisterHandlers( void );

    void IfReplayFileFolderAndSelfNameAreEmptyAutoInput( void );
    void AutoInputToReplayFileFolder( void );
    void AutoInputToSelfName( void );

    void LoadPlacementFromIniFile( void );
    void LoadControlParameterFromIniFile( void );

    void SearchReplayFilesIfYet( void );
    void SearchReplayFiles( void );

    bool CheckReplayFolderIsEnteredAndShowMessageBox( void );
    bool CheckSelfNameEditIsEnteredAndShowMessageBox( void );
    bool CheckReplayFolderAndSelfNameAreEnteredAndShowMessageBox( void );

    void ErrorHandling( std::function<void ( void )> function );

  private:
    Core core_;

    struct {
      unsigned int index_ = std::numeric_limits<unsigned int>::max();
      bool         first_;
    } sort_info_;

    UTF16Window<TtStatic> language_label_;
    LanguageComboBox      language_combo_;

    UTF16Window<TtGroup>  replay_folder_group_;
    UTF16Window<TtButton> replay_folder_auto_button_;
    UTF16Window<TtEdit>   replay_folder_edit_;
    UTF16Window<TtButton> replay_folder_ref_button_;
    UTF16Window<TtStatic> replay_folder_done_label_;

    UTF16Window<TtGroup>  self_name_group_;
    UTF16Window<TtButton> self_name_auto_button_;
    UTF16Window<TtEdit>   self_name_edit_;

    UTF16Window<TtButton> analyze_button_;

    UTF16Window<TtGroup>  date_template_group_;
    UTF16Window<TtStatic> date_template_title_label_;
    UTF16Window<TtStatic> date_template_history_label_;
    DateTemplateComboBox  date_template_combo_;
    UTF16Window<TtStatic> date_template_notice_label_;
    UTF16Window<TtButton> date_move_button_;

    UTF16Window<TtButton>     opponent_search_button_;
    UTF16Window<TtButton>     opponent_copy_button_;
    UTF16Window<TtCheckBox>   opponent_limit_check_;
    UTF16Window<TtEditNumber> opponent_limit_edit_;

    OpponentList opponent_list_;
  };
}
