// main.cpp

#include "tt_string.h"
#include "tt_message_box.h"

#include "common.h"
#include "utility.h"
#include "string_table.h"
#include "exception.h"
#include "main_frame.h"

static void show_unexpected_error_message_box( void );


int WINAPI
WinMain( HINSTANCE h_instance,
         HINSTANCE h_prev_instance,
         PSTR lp_cmd_line,
         int n_cmd_show )
{
  NOT_USE( h_instance );
  NOT_USE( h_prev_instance );
  NOT_USE( lp_cmd_line );
  NOT_USE( n_cmd_show );

  try {
    // 言語選択
    {
      std::wstring language = TtString::CP932ToUTF16( GGXXACPRReplayOrganizer::IniFileOperation::LoadLanguage() );
      if ( language.empty() ) {
        language = GGXXACPRReplayOrganizer::StringTable::GetUserDefaultLocaleString();
      }
      GGXXACPRReplayOrganizer::StringTable::Initialize( language );
    }

    GGXXACPRReplayOrganizer::Image::Initialize();

    GGXXACPRReplayOrganizer::MainFrame frame;
    frame.Create();
    frame.LoadPlacementFromIniFile();
    frame.LoadControlParameterFromIniFile();
    if ( frame.IsMinimized() ) {
      frame.Restore();
    }
    frame.Show();
    frame.IfReplayFileFolderAndSelfNameAreEmptyAutoInput();
    return TtForm::LoopDispatchMessage();
  }
  catch ( GGXXACPRReplayOrganizer::LanguageException& e ) {
    // ここだけは生メッセージ
    if ( GGXXACPRReplayOrganizer::Utility::UserDefaultLocaleIsJapan() ) {
      TtMessageBox::UTF16::Ok box;
      box.AppendMessage() << L"言語 DLL の読み込み時にエラーが発生しました。\r\n";
      box.AppendMessage() << L"言語 : " << e.GetLanguage();
      box.SetCaption( L"言語選択エラー" );
      box.SetIcon( TtMessageBox::Icon::ERROR );
      box.ShowDialog();
    }
    else {
      TtMessageBox::UTF16::Ok box;
      box.AppendMessage() << L"Error occurred while loading the language DLL.\r\n";
      box.AppendMessage() << L"Language : " << e.GetLanguage();
      box.SetCaption( L"Language selection error" );
      box.SetIcon( TtMessageBox::Icon::ERROR );
      box.ShowDialog();
    }
    return 3;
  }
  catch ( TtException& e ) {
    GGXXACPRReplayOrganizer::IniFileOperation::SaveErrorLogDump( e );
    show_unexpected_error_message_box();
    return 2;
  }
  catch ( ... ) {
    show_unexpected_error_message_box();
    return 1;
  }
}

static void
show_unexpected_error_message_box( void )
{
  TtMessageBox::UTF16::Ok box;
  box.SetMessage( GGXXACPRReplayOrganizer::StrT::Message::UnexpectedError.Get() );
  box.SetCaption( GGXXACPRReplayOrganizer::StrT::Message::UnexpectedErrorCaption.Get() );
  box.SetIcon( TtMessageBox::Icon::ERROR );
  box.ShowDialog();
}
