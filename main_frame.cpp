// main_frame.cpp

#include "tt_folder_browser_dialog.h"
#include "tt_message_box.h"
#include "tt_thread.h"
#include "tt_time.h"

#include "common.h"
#include "utility.h"
#include "string_table.h"
#include "exception.h"
#include "analyze_dialog.h"

#include "main_frame.h"

using namespace GGXXACPRReplayOrganizer;


// -- MainFrame ----------------------------------------------------------
MainFrame::MainFrame( void )
{
}

DWORD
MainFrame::GetStyle( void )
{
  return WS_OVERLAPPEDWINDOW;
}

DWORD
MainFrame::GetExtendedStyle( void )
{
  return 0;
}


bool
MainFrame::Created( void )
{
  this->SetIconAsLarge( Image::ICONS[Image::Index::Main] );
  this->SetText( std::string( APPLICATION_NAME ) + " " + VERSION );

  language_label_.Create( {this} );
  language_combo_.Create( {this, CommandID::Control::LanguageComboBox} );
  replay_folder_group_.Create( {this} );
  replay_folder_auto_button_.Create( {this, CommandID::Control::ReplayFolderAutoButton} );
  replay_folder_edit_.Create( {this} );
  replay_folder_ref_button_.Create( {this, CommandID::Control::ReplayFolderReferenceButton} );
  replay_folder_done_label_.Create( {this} );
  self_name_group_.Create( {this} );
  self_name_auto_button_.Create( {this, CommandID::Control::SelfNameAutoButton} );
  self_name_edit_.Create( {this} );
  analyze_button_.Create( {this, CommandID::Control::AnalyzeButton} );
  date_template_group_.Create( {this} );
  date_template_title_label_.Create( {this} );
  date_template_history_label_.Create( {this} );
  date_template_combo_.Create( {this} );
  date_template_notice_label_.Create( {this} );
  date_move_button_.Create( {this, CommandID::Control::DateMoveButton} );
  opponent_search_button_.Create( {this, CommandID::Control::OpponentSearchButton} );
  opponent_copy_button_.Create( {this, CommandID::Control::OpponentCopyButton} );
  opponent_limit_check_.Create( {this, CommandID::Control::OpponentLimitCheck} );
  opponent_limit_edit_.Create( {this} );
  opponent_list_.Create( {this, CommandID::Control::OpponentList} );

  opponent_list_.SetHasCheckBox( true );
  opponent_list_.SetHasGridLines( true );
  opponent_list_.SetFullRowSelect( true );
  opponent_list_.MakeNewColumn();
  opponent_list_.MakeNewColumn();
  // Text 設定は後

  // -- サイズ変更時 -----
  this->RegisterWMSize( [this] ( int, int w, int h ) -> WMResult {
    language_label_.SetPositionSize(                    8,   8,         80,      20 );
    language_combo_.SetPositionSize(                   88,   4,        160,     180 );
    replay_folder_group_.SetPositionSize(               8,  40,     w - 16,      76 );
    replay_folder_auto_button_.SetPositionSize(        16,  60,        120,      20 );
    replay_folder_edit_.SetPositionSize(               16,  84,     w - 92,      20 );
    replay_folder_done_label_.SetPositionSize(        148,  64,        220,      20 );
    replay_folder_ref_button_.SetPositionSize(     w - 72,  84,         54,      20 );
    self_name_group_.SetPositionSize(                   8, 126,        300,      76 );
    self_name_auto_button_.SetPositionSize(            16, 146,        120,      20 );
    self_name_edit_.SetPositionSize(                   16, 170,        280,      20 );
    analyze_button_.SetPositionSize(              w - 140, 150,        120,      48 );
    date_template_group_.SetPositionSize(               8, 220,     w - 16,     148 );
    date_template_title_label_.SetPositionSize(        16, 240,        300,      20 );
    date_template_history_label_.SetPositionSize(      16, 264,         52,      20 );
    date_template_combo_.SetPositionSize(              68, 260,     w - 84,     100 );
    date_template_notice_label_.SetPositionSize(       20, 288,        350,      20 );
    date_move_button_.SetPositionSize(                 32, 312,     w - 64,      40 );
    opponent_search_button_.SetPositionSize(           24, 388, w / 2 - 40,      32 );
    opponent_copy_button_.SetPositionSize(          w / 2, 388, w / 2 - 40,      32 );
    opponent_limit_check_.SetPositionSize(             24, 428,        250,      20 );
    opponent_limit_edit_.SetPositionSize(             284, 428,         50,      20 );
    opponent_list_.SetPositionSize(                    16, 456,     w - 32, h - 464 );

    return {WMResult::Done};
  } );
  this->RegisterWMSizing( this->MakeMinimumSizedHandler( 476, 600 ) );
  this->SetClientSize( 500, 700, false );

  // ハンドラ登録
  this->RegisterHandlers();

  this->ResetTextOfControls();

  this->ResetLanguageList();

  date_template_combo_.PushUTF16( L"YYYY\\YYYYMM\\YYYYMMDD" );
  date_template_combo_.PushUTF16( L"YYYYMM\\YYYYMMDD" );

  language_label_.Show();
  language_combo_.Show();
  replay_folder_group_.Show();
  replay_folder_auto_button_.Show();
  replay_folder_edit_.Show();
  replay_folder_ref_button_.Show();
  // replay_folder_done_label_.Show(); 初期状態では見せない
  self_name_group_.Show();
  self_name_auto_button_.Show();
  self_name_edit_.Show();
  analyze_button_.Show();
  date_template_group_.Show();
  date_template_title_label_.Show();
  date_template_history_label_.Show();
  date_template_combo_.Show();
  date_template_notice_label_.Show();
  date_move_button_.Show();
  opponent_search_button_.Show();
  opponent_copy_button_.Show();
  opponent_limit_check_.Show();
  opponent_limit_edit_.Show();
  opponent_list_.Show();

  return true;
}


void
MainFrame::ResetTextOfControls( void )
{
  namespace Main = StringTable::Main;
  language_label_.SetTextUTF16( Main::Language.Get() );

  replay_folder_group_.SetTextUTF16( Main::ReplayFolderGroup.Get() );
  replay_folder_auto_button_.SetTextUTF16( Main::ReplayFolderAutoButton.Get() );
  replay_folder_ref_button_.SetTextUTF16( Main::ReplayFolderReferenceButton.Get() );
  replay_folder_done_label_.SetTextUTF16( Main::ReplayFolderDoneLabel.Get() );

  self_name_group_.SetTextUTF16( Main::SelfNameGroup.Get() );
  self_name_auto_button_.SetTextUTF16( Main::SelfNameAutoButton.Get() );

  analyze_button_.SetTextUTF16( Main::AnalyzeButton.Get() );

  date_template_group_.SetTextUTF16( Main::DateTemplateGroup.Get() );
  date_template_title_label_.SetTextUTF16( Main::DateTemplateTitle.Get() );
  date_template_history_label_.SetTextUTF16( Main::DateTemplateHistory.Get() );
  date_template_notice_label_.SetTextUTF16( Main::DateTemplateNotice.Get() );
  date_move_button_.SetTextUTF16( Main::DateMoveButton.Get() );

  opponent_search_button_.SetTextUTF16( Main::OpponentSearchButton.Get() );
  opponent_copy_button_.SetTextUTF16( Main::OpponentCopyButton.Get() );

  opponent_limit_check_.SetTextUTF16( Main::OpponentLimitCheck.Get() );

  {
    OpponentList::Column column( opponent_list_.GetColumn( 0 ) );
    column.SetTextUTF16( Main::OpponentListColumnName.Get() );
    column.SetWidthAutoUseHeader();
  }
  {
    OpponentList::Column column( opponent_list_.GetColumn( 1 ) );
    column.SetTextUTF16( Main::OpponentListColumnCount.Get() );
    column.SetWidthAutoUseHeader();
  }
}

void
MainFrame::ResetLanguageList( void )
{
  language_combo_.Clear();

  std::vector<std::wstring> path_array = Utility::DirectoryGetLanguageDLLUTF16();
  for ( auto& path : path_array ) {
    language_combo_.PushUTF16( Path::EditPath( path, false, false, true, false ) );
  }
  for ( unsigned int i = 0; i < static_cast<unsigned int>( language_combo_.GetCount() ); ++i ) {
    if ( language_combo_.GetTextAtUTF16( i ) == StringTable::GetCurrentLanguage() ) {
      language_combo_.SetSelect( i );
      break;
    }
  }
}


void
MainFrame::RegisterHandlers( void )
{
  this->AddCommandHandler( CommandID::Control::LanguageComboBox, [this] ( int code, HWND ) -> WMResult {
    if ( code == CBN_SELCHANGE ) {
      std::wstring current = language_combo_.GetTextAtUTF16( language_combo_.GetSelectedIndex() );
      if ( current != StringTable::GetCurrentLanguage() ) {
        try {
          StringTable::Initialize( current );
        }
        catch ( LanguageException& e ) {
          TtMessageBox::UTF16::Ok box;
          box.SetMessage( Utility::Format( StrT::Message::LanguageChangeError.Get(), e.GetLanguage() ) );
          box.SetCaption( StrT::Message::LanguageChangeErrorCaption.Get() );
          box.SetIcon( TtMessageBox::Icon::ERROR );
          box.ShowDialog( *this );
        }
        this->ResetTextOfControls();
      }
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::Control::AnalyzeButton, [&] ( int, HWND ) -> WMResult {
    this->SearchReplayFilesIfYet();

    if ( this->CheckReplayFolderAndSelfNameAreEnteredAndShowMessageBox() ) {
      AnalyzeDialog dialog( self_name_edit_.GetTextUTF16(), core_ );
      dialog.ShowDialog( *this );
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::Control::ReplayFolderAutoButton, [&] ( int, HWND ) -> WMResult {
    this->AutoInputToReplayFileFolder();
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::Control::ReplayFolderReferenceButton, [&] ( int, HWND ) -> WMResult {
    UTF16FolderBrowserDialog dialog;
    dialog.description_ = StrT::Main::ReplayFolderReferenceFolderDialogDescription.Get();
    dialog.selected_path_ = replay_folder_edit_.GetTextUTF16();
    if ( dialog.ShowDialog( *this ) ) {
      replay_folder_edit_.SetTextUTF16( dialog.selected_path_ );
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::Control::SelfNameAutoButton, [&] ( int, HWND ) -> WMResult {
    this->AutoInputToSelfName();
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::Control::DateMoveButton, [&] ( int, HWND ) -> WMResult {
    this->SearchReplayFilesIfYet();

    if ( this->CheckReplayFolderAndSelfNameAreEnteredAndShowMessageBox() ) {
      TtMessageBox::UTF16::YesNo box;
      box.SetMessage( Utility::Format( StrT::Main::DateMoveConfirmMessage.Get(), Core::ApplyDateTemplateToday( date_template_combo_.GetTextUTF16() ) ) );
      box.SetCaption( StrT::Main::DateMoveConfirmCaption.Get() );
      box.SetIcon( TtMessageBox::Icon::QUESTION );

      if ( box.ShowDialog( *this ) == TtMessageBox::Result::YES ) {
        this->ErrorHandling( [this] ( void ) {
          unsigned int count = 0;
          core_.replay_file_moved_ = [&count] ( const std::wstring&, const std::wstring& ) -> void {
            count += 1;
          };
          core_.MoveReplayFiles( replay_folder_edit_.GetTextUTF16(), date_template_combo_.GetTextUTF16() );

          TtMessageBox::UTF16::Ok done_box;
          done_box.SetMessage( Utility::Format( StrT::Main::DateMoveDoneMessage.Get(), count ) );
          done_box.SetCaption( StrT::Main::DateMoveDoneCaption.Get() );
          done_box.SetIcon( TtMessageBox::Icon::INFORMATION );
          done_box.ShowDialog( *this );
        } );
      }
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::Control::OpponentSearchButton, [&] ( int, HWND ) -> WMResult {
    this->SearchReplayFilesIfYet();

    if ( this->CheckReplayFolderAndSelfNameAreEnteredAndShowMessageBox() ) {
      opponent_list_.ClearItems();

      static TtFunctionThread thread( [this] ( void ) {
        auto already_players = Core::GetOpponentNamesFromPlayerFolder( replay_folder_edit_.GetTextUTF16() );

        for ( Core::PlayerCount& tmp : core_.GetOpponentPlayerCountListReplayFiles( self_name_edit_.GetTextUTF16() ) ) {
          if ( opponent_limit_check_.GetCheck() && tmp.count_ < Utility::StringToInteger( opponent_limit_edit_.GetTextUTF16() ) ) {
            break;
          }
          OpponentList::Item item( opponent_list_.MakeNewItem() );
          item.SetSubItemTextUTF16( 0, tmp.name_ );
          item.SetSubItemTextUTF16( 1, std::format( L"{}", tmp.count_ ) );
          item.SetChecked( already_players.contains( tmp.name_ ) );
        }
        opponent_list_.GetColumn( 0 ).SetWidthAutoUseHeader();
        opponent_list_.GetColumn( 1 ).SetWidthAutoUseHeader();
        opponent_search_button_.SetEnabled( true );
        opponent_copy_button_.SetEnabled( true );
      } );
      if ( thread.IsValid() ) {
        thread.Join();
        thread.Close();
      }
      opponent_search_button_.SetEnabled( false );
      opponent_copy_button_.SetEnabled( false );
      thread.Open( true );
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::Control::OpponentCopyButton, [&] ( int, HWND ) -> WMResult {
    this->SearchReplayFilesIfYet();

    if ( this->CheckReplayFolderAndSelfNameAreEnteredAndShowMessageBox() ) {
      TtMessageBox::UTF16::YesNo box;
      box.SetMessage( StrT::Main::OpponentCopyConfirmMessage.Get() );
      box.SetCaption( StrT::Main::OpponentCopyConfirmCaption.Get() );
      box.SetIcon( TtMessageBox::Icon::QUESTION );

      if ( box.ShowDialog( *this ) == TtMessageBox::Result::YES ) {
        std::vector<std::wstring> tmp;
        for ( unsigned int i = 0; i < opponent_list_.GetItemCount(); ++i ) {
          OpponentList::Item item( opponent_list_.GetItem( i ) );
          if ( item.IsChecked() ) {
            tmp.push_back( item.GetTextUTF16() );
          }
        }
        this->ErrorHandling( [this, &tmp] ( void ) {
          core_.CopyReplayFilesOpponentPlayerForEachFolder( replay_folder_edit_.GetTextUTF16(), self_name_edit_.GetTextUTF16(), tmp );

          TtMessageBox::UTF16::Ok done_box;
          done_box.SetMessage( StrT::Main::OpponentCopyDoneMessage.Get() );
          done_box.SetCaption( StrT::Main::OpponentCopyDoneCaption.Get() );
          done_box.SetIcon( TtMessageBox::Icon::INFORMATION );
          done_box.ShowDialog( *this );
        } );
      }
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::Control::OpponentLimitCheck, [&] ( int code, HWND ) -> WMResult {
    if ( code == BN_CLICKED ) {
      opponent_limit_edit_.SetEnabled( opponent_limit_check_.GetCheck() );
    }
    return {WMResult::Done};
  } );

  this->AddNotifyHandler( CommandID::Control::OpponentList, [this] ( NMHDR* nmhdr ) -> WMResult {
    switch ( nmhdr->code ) {
      // カラムクリック
    case LVN_COLUMNCLICK:
      unsigned int column_index = reinterpret_cast<NMLISTVIEW*>( nmhdr )->iSubItem;
      bool same_as_last = column_index == sort_info_.index_;
      bool default_direction = column_index != 0;
      bool first = same_as_last ? NOT( sort_info_.first_ ) : true;
      bool ascending = (default_direction != first);

      auto compare = [this, column_index] ( unsigned int x, unsigned int y ) -> int {
        OpponentList::Item x_item( opponent_list_.GetItem( x ) );
        auto x_text = x_item.GetSubItemTextUTF16( column_index );
        OpponentList::Item y_item( opponent_list_.GetItem( y ) );
        auto y_text = y_item.GetSubItemTextUTF16( column_index );
        if ( column_index == 0 ) {
          return x_text.compare( y_text );
        }
        else {
          return std::stoi( x_text ) - std::stoi( y_text );
        }
      };
      opponent_list_.SortByIndex( compare, ascending );

      sort_info_.index_ = column_index;
      sort_info_.first_ = first;
      return {WMResult::Done};
    }
    return {WMResult::Incomplete};
  } );

  // -- 終了時
  this->RegisterWMClose( [this] ( void ) -> WMResult {
    IniFileOperation::SavePlacement( this->GetWindowPlacement() );

    IniFileOperation::SaveLanguage( language_combo_.GetText() );
    IniFileOperation::SaveReplayFolderPath( replay_folder_edit_.GetTextUTF16() );
    IniFileOperation::SaveDateTtemplate( date_template_combo_.GetTextUTF16() );
    IniFileOperation::SaveSelfName( self_name_edit_.GetTextUTF16() );
    IniFileOperation::SaveOpponentLowerLimit( opponent_limit_check_.GetCheck() );
    IniFileOperation::SaveOpponentLowerLimitNumber( Utility::StringToInteger( opponent_limit_edit_.GetTextUTF16() ) );

    return {WMResult::Incomplete}; // 処理差し込みの為
  }, false );
}


void
MainFrame::IfReplayFileFolderAndSelfNameAreEmptyAutoInput( void )
{
  if ( replay_folder_edit_.GetTextUTF16().empty() && self_name_edit_.GetTextUTF16().empty() ) {
    TtMessageBox::UTF16::YesNo box;
    box.SetMessage( StrT::Main::AutoInputConfirmMessage.Get() );
    box.SetCaption( StrT::Main::AutoInputConfirmCaption.Get() );
    box.SetIcon( TtMessageBox::Icon::QUESTION );
    if ( box.ShowDialog( *this ) == TtMessageBox::Result::YES ) {
      this->AutoInputToReplayFileFolder();
      this->AutoInputToSelfName();
    }
  }
}

void
MainFrame::AutoInputToReplayFileFolder( void )
{
  const std::wstring replay_folder_base = L"%USERPROFILE%\\Documents\\ARC SYSTEM WORKS\\GGXXAC\\Replays";
  std::wstring replay_folder = Utility::ExpandEnvironmentStringUTF16( replay_folder_base );
  replay_folder_edit_.SetTextUTF16( replay_folder );
}

void
MainFrame::AutoInputToSelfName( void )
{
  this->SearchReplayFilesIfYet();
  if ( this->CheckReplayFolderIsEnteredAndShowMessageBox() ) {
    if ( core_.replay_files_.empty() ) {
      TtMessageBox::UTF16::Ok box;
      box.SetMessage( StrT::Main::ReplayFileIsEmptyMessage.Get() );
      box.SetCaption( StrT::Main::ReplayFileIsEmptyCaption.Get() );
      box.SetIcon( TtMessageBox::Icon::ERROR );
      box.ShowDialog( *this );
    }
    else {
      std::wstring name = core_.GetAllPlayerCountListFromReplayFiles().front().name_;

      TtMessageBox::UTF16::YesNo box;
      box.SetMessage( Utility::Format( StrT::Main::SelfNameAutoConfirmMessage.Get(), name ) );
      box.SetCaption( StrT::Main::SelfNameAutoConfirmCaption.Get() );
      box.SetIcon( TtMessageBox::Icon::QUESTION );
      if ( box.ShowDialog( *this ) == TtMessageBox::Result::YES ) {
        self_name_edit_.SetTextUTF16( name );
      }
    }
  }
}


void
MainFrame::LoadPlacementFromIniFile( void )
{
  WINDOWPLACEMENT placement;
  if ( IniFileOperation::LoadPlacement( placement ) ) {
    this->SetWindowPlacement( placement );
  }
}


void
MainFrame::LoadControlParameterFromIniFile( void )
{
  replay_folder_edit_.SetTextUTF16( IniFileOperation::LoadReplayFolderPath() );
  date_template_combo_.SetTextUTF16( IniFileOperation::LoadDateTemplate() );
  self_name_edit_.SetTextUTF16( IniFileOperation::LoadSelfName() );
  opponent_limit_check_.SetCheck( IniFileOperation::LoadOpponentLowerLimit() );
  opponent_limit_edit_.SetEnabled( opponent_limit_check_.GetCheck() );
  opponent_limit_edit_.SetTextUTF16( std::format( L"{}", IniFileOperation::LoadOpponentLowerLimitNumber() ) );
}


void
MainFrame::SearchReplayFilesIfYet( void )
{
  if ( core_.replay_files_.empty() ) {
    this->SearchReplayFiles();
  }
}

void
MainFrame::SearchReplayFiles( void )
{
  core_.SearchReplayFiles( replay_folder_edit_.GetTextUTF16() );
  if ( NOT( core_.replay_files_.empty() ) ) {
    replay_folder_auto_button_.SetEnabled( false );
    replay_folder_edit_.SetEnabled( false );
    replay_folder_ref_button_.SetEnabled( false );
    replay_folder_done_label_.Show();
  }
}


bool
MainFrame::CheckReplayFolderIsEnteredAndShowMessageBox( void )
{
  if ( replay_folder_edit_.GetTextUTF16().empty() ) {
    TtMessageBox::UTF16::Ok box;
    box.SetMessage( StrT::Main::ReplayFolderIsEmptyMessage.Get() );
    box.SetCaption( StrT::Main::ReplayFolderIsEmptyCaption.Get() );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    box.ShowDialog( *this );
    return false;
  }
  return true;
}

bool
MainFrame::CheckSelfNameEditIsEnteredAndShowMessageBox( void )
{
  if ( self_name_edit_.GetTextUTF16().empty() ) {
    TtMessageBox::UTF16::Ok box;
    box.SetMessage( StrT::Main::SelfNameIsEmptyMessage.Get() );
    box.SetCaption( StrT::Main::SelfNameIsEmptyCaption.Get() );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    box.ShowDialog( *this );
    return false;
  }
  return true;
}

bool
MainFrame::CheckReplayFolderAndSelfNameAreEnteredAndShowMessageBox( void )
{
  if ( this->CheckReplayFolderIsEnteredAndShowMessageBox() ) {
    if ( this->CheckSelfNameEditIsEnteredAndShowMessageBox() ) {
      return true;
    }
  }
  return false;
}


void
MainFrame::ErrorHandling( std::function<void ( void )> function )
{
  try {
    function();
  }
  catch ( Exception& e ) {
    TtMessageBox::UTF16::Ok box;
    box.SetMessage( e.GetMessage() );
    box.SetCaption( StrT::Message::ErrorCaption.Get() );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    box.ShowDialog( *this );
  }
}

