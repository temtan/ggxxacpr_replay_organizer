// analyze_dialog.cpp

#include "tt_time.h"
#include "tt_message_box.h"

#include "common.h"
#include "exception.h"
#include "string_table.h"

#include "analyze_dialog.h"

using namespace GGXXACPRReplayOrganizer;


namespace {
  struct LocalCommandID {
    enum ID : int {
      InitializeEnd = 10001,
      Combo,
      List,

      SubList,
      RecordBaseList,
    };
  };
}

// -- AnalyzeList --------------------------------------------------------
void
AnalyzeList::InitializeColumn( void )
{
  auto make_new_column = [this] ( const std::wstring& text ) {
    AnalyzeList::Column column( this->MakeNewColumn() );
    column.SetTextUTF16( text );
  };
  namespace Column = StrT::Analyze::Column;
  make_new_column( Column::Opponent.Get() );
  make_new_column( Column::CountOfMatches.Get() );
  make_new_column( Column::Win.Get() );
  make_new_column( Column::WinningRate.Get() );
  make_new_column( Column::Lose.Get() );
  make_new_column( Column::Draw.Get() );
  make_new_column( Column::Unknown.Get() );
}


void
AnalyzeList::Sort( unsigned int column_index, bool ascending )
{
  this->TtListView::Sort<Analyze::ListEntry*>( [column_index] ( Analyze::ListEntry* x, Analyze::ListEntry* y ) -> int {
    switch ( column_index ) {
    default: // walk through
    case 0:
      return Analyze::ListEntry::Compare( *x, *y );

    case 1: return static_cast<int>( y->GetTotalCount() ) - static_cast<int>( x->GetTotalCount() );
    case 2: return static_cast<int>( y->GetTotalWin() ) - static_cast<int>( x->GetTotalWin() );
    case 3: {
      double tmp = (y->GetWinRatio() - x->GetWinRatio());
      return (tmp == 0.0) ? (0) : ( (tmp > 0.0) ? (1) : (-1) );
    }
    case 4: return static_cast<int>( y->GetTotalLose() ) - static_cast<int>( x->GetTotalLose() );
    case 5: return static_cast<int>( y->GetTotalDraw() ) - static_cast<int>( x->GetTotalDraw() );
    case 6: return static_cast<int>( y->GetTotalUnknown() ) - static_cast<int>( x->GetTotalUnknown() );
    }
  }, ascending );
}


void
AnalyzeList::Input( const Analyze::ListEntryEnumerable& enumerable )
{
  std::vector<const Analyze::ListEntry*> array;
  list_entry_for_total_.Reset( &enumerable );
  array.push_back( &list_entry_for_total_ );
  for ( auto& one : enumerable.GetListEntryArray() ) {
    array.push_back( one );
  }

  this->StopRedraw( [&] ( void ) {
    this->ClearItems();

    for ( auto& one : array ) {
      AnalyzeList::Item item( this->MakeNewItem() );
      item.SetParameter( one );
      item.SetSubItemTextUTF16( 0, one->GetName() );
      item.SetSubItemTextUTF16( 1, std::format( L"{}",       one->GetTotalCount() ) );
      item.SetSubItemTextUTF16( 2, std::format( L"{}",       one->GetTotalWin() ) );
      item.SetSubItemTextUTF16( 3, std::format( L"{:2.2f}%", one->GetWinRatio() * 100.0 ) );
      item.SetSubItemTextUTF16( 4, std::format( L"{}",       one->GetTotalLose() ) );
      item.SetSubItemTextUTF16( 5, std::format( L"{}",       one->GetTotalDraw() ) );
      item.SetSubItemTextUTF16( 6, std::format( L"{}",       one->GetTotalUnknown() ) );
    }
    for ( unsigned int i = 0; i < this->GetColumnCount(); ++i ) {
      this->GetColumn( i ).SetWidthAutoUseHeader();
    }
  } );
}


// -- AnalyzeDialog ------------------------------------------------------
AnalyzeDialog::AnalyzeDialog( const std::wstring& self_player_name, Core& core ) :
self_player_name_( self_player_name ),
core_( core )
{
}


bool
AnalyzeDialog::Created( void )
{
  this->SetText( std::string( APPLICATION_NAME ) + " Analyze Dialog" );
  this->SetIconAsSmall( Image::ICONS[Image::Index::MainSmall] );

  loading_label_.Create( {this} );
  progress_.Create( {this} );

  label_.Create( {this} );
  combo_.Create( {this, LocalCommandID::Combo} );
  list_.Create( {this, LocalCommandID::List} );
  list_.SetHasGridLines( true );
  list_.SetFullRowSelect( true );

  // -- サイズ変更時 -----
  this->RegisterWMSize( [this] ( int, int w, int h ) -> WMResult {
    loading_label_.SetPositionSize( 8, 12,    300, 20 );
    progress_.SetPositionSize(      8, 32, w - 16, 32 );

    label_.SetPositionSize(   8, 12,    120,     24 );
    combo_.SetPositionSize( 138,  8,    150,    150 );
    list_.SetPositionSize(    8, 32, w - 16, h - 40 );
    return {WMResult::Done};
  } );
  this->RegisterWMSizing( this->MakeMinimumSizedHandler( 232, 120 ) );
  {
    this->SetClientSize( 450, 600, false );
    WINDOWPLACEMENT placement;
    if ( IniFileOperation::LoadAnalyzeDialogPlacement( placement ) ) {
      this->SetWindowPlacement( placement );
    }
  }
  this->SetCenterRelativeToParent();


  initialize_thread_.SetFunction( [this] ( void ) {
    this->Initialize();
    this->PostMessage( WM_COMMAND, MAKEWPARAM( LocalCommandID::InitializeEnd, 0 ), 0 );
  } );
  initialize_thread_.Open();

  this->AddCommandHandler( LocalCommandID::InitializeEnd, [&] ( int, HWND ) -> WMResult {
    if ( initialize_thread_.IsValid() ) {
      initialize_thread_.Join();
      initialize_thread_.Close();
    }
    this->InitializeEnd();
    return {WMResult::Done};
  } );


  this->AddCommandHandler( LocalCommandID::Combo, [&] ( int code, HWND ) -> WMResult {
    if ( code == CBN_SELCHANGE ) {
      if ( combo_.GetSelectedIndex() == 0 ) {
        list_.Input( by_all_character_record_ );
      }
      else if ( combo_.GetSelectedIndex() == combo_.GetCount() - 1 ) {
        list_.Input( by_opponent_record_ );
      }
      else {
        list_.Input( by_character_record_.at( combo_table_[combo_.GetSelectedIndex()] ) );
      }
      last_sort_index_ = std::numeric_limits<unsigned int>::max();
    }
    return {WMResult::Done};
  } );

  this->AddNotifyHandler( LocalCommandID::List, [this] ( NMHDR* nmhdr ) -> WMResult {
    switch ( nmhdr->code ) {
      // カラムクリック
    case LVN_COLUMNCLICK: {
      this->SortList( reinterpret_cast<NMLISTVIEW*>( nmhdr )->iSubItem );
      return {WMResult::Done};
    }

      // カラムダブルクリック
    case LVN_ITEMACTIVATE: {
      AnalyzeList::Item item( list_.GetSelected() );
      if ( item.IsValid() && item.GetParameter()->Is<Analyze::ListEntryEnumerableAndForSubDialog>() ) {
        AnalyzeSubDialog dialog( item.GetParameter()->CastTo<Analyze::ListEntryEnumerableAndForSubDialog>() );
        dialog.ShowDialog( *this );
      }
      return {WMResult::Done};
    }

    }
    return {WMResult::Incomplete};
  } );

  this->RegisterWMClose( [this] ( void ) -> WMResult {
    if ( initialize_thread_.IsValid() ) {
      initialize_thread_.Terminate( 1 );
      initialize_thread_.Close();
    }
    IniFileOperation::SaveAnalyzeDialogPlacement( this->GetWindowPlacement() );
    return {WMResult::Incomplete}; // 処理差し込みの為
  }, true );

  loading_label_.SetTextUTF16( Utility::Format( StrT::Analyze::LoadingLabel.Get(), 0, core_.replay_files_.size() ) );
  label_.SetTextUTF16( StrT::Analyze::UsingCharacter.Get() );
  list_.InitializeColumn();

  loading_label_.Show();
  progress_.Show();

  return true;
}


void
AnalyzeDialog::SortList( unsigned int column_index )
{
  bool ascending = (column_index == last_sort_index_) ? NOT( last_ascending_ ) : true;

  last_sort_index_ = column_index;
  last_ascending_ = ascending;

  list_.Sort( column_index, ascending );
}


void
AnalyzeDialog::Initialize( void )
{
  if ( core_.replay_files_.empty() ) {
    return;
  }

  // プログレスバーの max が short なので切り詰める処理
  const unsigned int count = static_cast<unsigned int>( core_.replay_files_.size() );
  const unsigned short temp_max = [&] ( void ) -> unsigned short {
    unsigned int tmp = count;
    for ( unsigned int i = 0; i < 32; ++i ) {
      if ( tmp < 0xFFFF ) {
        return static_cast<unsigned short>( tmp );
      }
      tmp = tmp / 2;
    }
    return 0xFFFF;
  }();
  constexpr unsigned short division = 500;
  unsigned int step = ((temp_max - 1) / division) + 1;

  progress_.SetMaximum( static_cast<unsigned short>( step * division ) );
  progress_.SetStep( step );
  progress_.SetAmount( 0 );

  for ( unsigned int i = 0; auto& one : core_.replay_files_ ) {
    i += 1;
    one.ReadMetaData();

    if ( i % (step * (count / temp_max)) == 0 ) {
      progress_.StepIt();
      loading_label_.SetTextUTF16( Utility::Format( StrT::Analyze::LoadingLabel.Get(), i, count ) );
    }
  }

  auto self_player_name_symbol = PlayerNameSymbol::GetSymbol( self_player_name_ );
  for ( ReplayFile& file : core_.replay_files_ ) {
    if ( file.first_player_name_symbol_ == self_player_name_symbol ) {
      by_character_record_.SetDown( &file, true );
      by_all_character_record_.SetDown( &file, true );
      by_opponent_record_.SetDown( &file, true );
    }
    else if ( file.second_player_name_symbol_ == self_player_name_symbol ) {
      by_character_record_.SetDown( &file, false );
      by_all_character_record_.SetDown( &file, false );
      by_opponent_record_.SetDown( &file, false );
    }
    else {
      continue;
    }
  }
}

void
AnalyzeDialog::InitializeEnd( void )
{
  {
    unsigned int error_count = 0;
    for ( auto& one : core_.replay_files_ ) {
      if ( one.file_read_error_occured_ ) {
        error_count += 1;
      }
    }
    if ( error_count > 0 ) {
      TtMessageBox::UTF16::Ok box;
      box.SetMessage( Utility::Format( StrT::Analyze::ReplayFileLoadingErrorMessage.Get(), error_count ) );
      box.SetCaption( StrT::Analyze::ReplayFileLoadingErrorCaption.Get() );
      box.SetIcon( TtMessageBox::Icon::ERROR );
      box.ShowDialog( *this );
    }
  }

  loading_label_.Hide();
  progress_.Hide();

  {
    combo_.PushUTF16( StrT::Analyze::Combo::AllCharacter.Get() );
    for ( Character i = Character::Sol; i <= Character::Justice; i = i.GetNext() ) {
      if ( by_character_record_.contains( i ) ) {
        combo_table_[combo_.GetCount()] = i;
        combo_.PushUTF16( i.ToFullName() );
      }
    }
    combo_.PushUTF16( StrT::Analyze::Combo::ByOpponent.Get() );
    combo_.SetSelect( 0 );
    list_.Input( by_all_character_record_ );
    this->SortList( 0 );
  }

  label_.Show();
  combo_.Show();
  list_.Show();
}

// -- AnalyzeSubDialog ---------------------------------------------------
AnalyzeSubDialog::AnalyzeSubDialog( const Analyze::ListEntryEnumerableAndForSubDialog& enumerable ) :
enumerable_( enumerable )
{
}

bool
AnalyzeSubDialog::Created( void )
{
  this->SetTextUTF16( enumerable_.GetDescriptionForSubDialog() );

  label_.Create( {this} );
  list_.Create( {this, LocalCommandID::SubList} );
  list_.SetHasGridLines( true );
  list_.SetFullRowSelect( true );

  // -- サイズ変更時 -----
  this->RegisterWMSize( [this] ( int, int w, int h ) -> WMResult {
    label_.SetPositionSize(   8, 12, w - 16,     24 );
    list_.SetPositionSize(    8, 32, w - 16, h - 40 );
    return {WMResult::Done};
  } );
  this->RegisterWMSizing( this->MakeMinimumSizedHandler( 232, 120 ) );
  this->SetClientSize( parent_->GetClientWidth(), parent_->GetClientHeight(), false );
  this->SetCenterRelativeToParent();
  this->SetPosition( this->GetPoint().x + 24, this->GetPoint().y + 24 );

  this->AddNotifyHandler( LocalCommandID::SubList, [this] ( NMHDR* nmhdr ) -> WMResult {
    switch ( nmhdr->code ) {
      // カラムクリック
    case LVN_COLUMNCLICK: {
      this->SortList( reinterpret_cast<NMLISTVIEW*>( nmhdr )->iSubItem );
      return {WMResult::Done};
    }

      // カラムダブルクリック
    case LVN_ITEMACTIVATE: {
      AnalyzeList::Item item( list_.GetSelected() );
      if ( item.IsValid() && item.GetParameter()->Is<Analyze::RecordBase>() ) {
        AnalyzeRecordBaseDialog dialog( item.GetParameter()->CastTo<Analyze::RecordBase>() );
        dialog.ShowDialog( *this );
      }
      return {WMResult::Done};
    }

    }
    return {WMResult::Incomplete};
  } );

  label_.SetTextUTF16( enumerable_.GetDescriptionForSubDialog() );
  list_.InitializeColumn();

  list_.Input( enumerable_ );
  this->SortList( 0 );

  label_.Show();
  list_.Show();

  return true;
}

void
AnalyzeSubDialog::SortList( unsigned int column_index )
{
  bool ascending = (column_index == last_sort_index_) ? NOT( last_ascending_ ) : true;

  last_sort_index_ = column_index;
  last_ascending_ = ascending;

  list_.Sort( column_index, ascending );
}


// -- AnalyzeRecordBaseDialog --------------------------------------------
AnalyzeRecordBaseDialog::AnalyzeRecordBaseDialog( const Analyze::RecordBase& record_base ) :
record_base_( record_base )
{
}

bool
AnalyzeRecordBaseDialog::Created( void )
{
  this->SetTextUTF16( record_base_.GetDescriptionForRecordBaseDialog() );

  label_.Create( {this} );
  list_.Create( {this, LocalCommandID::RecordBaseList} );
  list_.SetHasGridLines( true );
  list_.SetFullRowSelect( true );

  // -- サイズ変更時 -----
  this->RegisterWMSize( [this] ( int, int w, int h ) -> WMResult {
    label_.SetPositionSize(   8, 12, w - 16,     24 );
    list_.SetPositionSize(    8, 32, w - 16, h - 40 );
    return {WMResult::Done};
  } );
  this->RegisterWMSizing( this->MakeMinimumSizedHandler( 232, 120 ) );
  this->SetClientSize( parent_->GetClientWidth(), parent_->GetClientHeight(), false );
  this->SetCenterRelativeToParent();
  this->SetPosition( this->GetPoint().x + 24, this->GetPoint().y + 24 );

  this->AddNotifyHandler( LocalCommandID::RecordBaseList, [this] ( NMHDR* nmhdr ) -> WMResult {
    switch ( nmhdr->code ) {
      // カラムクリック
    case LVN_COLUMNCLICK:
      this->SortList( reinterpret_cast<NMLISTVIEW*>( nmhdr )->iSubItem );
      return {WMResult::Done};
    }
    return {WMResult::Incomplete};
  } );

  label_.SetTextUTF16( record_base_.GetDescriptionForRecordBaseDialog() );
  this->InitializeList();
  this->SortList( 2 );

  label_.Show();
  list_.Show();

  return true;
}


void
AnalyzeRecordBaseDialog::InitializeList( void )
{
  auto make_new_column = [this] ( const std::wstring& text ) {
    UTF16ListView::Column column( list_.MakeNewColumn() );
    column.SetTextUTF16( text );
  };
  namespace Column = StrT::Analyze::Column;
  make_new_column( Column::FirstPlayer.Get() );
  make_new_column( Column::SecondPlayer.Get() );
  make_new_column( Column::Date.Get() );
  make_new_column( Column::FirstPlayerWin.Get() );
  make_new_column( Column::SecondPlayerWin.Get() );

  list_.ClearItems();
  for ( auto& replay : record_base_.replay_files_ ) {
    UTF16ListView::Item item( list_.MakeNewItem() );
    item.SetParameterAs( replay );
    item.SetSubItemTextUTF16( 0, std::format( L"{} ({})", replay->first_player_name_,  replay->first_player_character_.ToShortName() ) );
    item.SetSubItemTextUTF16( 1, std::format( L"{} ({})", replay->second_player_name_, replay->second_player_character_.ToShortName() ) );
    item.SetSubItemTextUTF16( 2, replay->GetTimeString() );
    item.SetSubItemTextUTF16( 3, std::format( L"{}", replay->first_player_rounds_won_ ) );
    item.SetSubItemTextUTF16( 4, std::format( L"{}", replay->second_player_rounds_won_ ) );
  }
  for ( unsigned int i = 0; i < list_.GetColumnCount(); ++i ) {
    list_.GetColumn( i ).SetWidthAutoUseHeader();
  }
}


void
AnalyzeRecordBaseDialog::SortList( unsigned int column_index )
{
  bool ascending = (column_index == last_sort_index_) ? NOT( last_ascending_ ) : true;

  last_sort_index_ = column_index;
  last_ascending_ = ascending;

  list_.TtListView::Sort<const ReplayFile*>( [column_index] ( const ReplayFile* x, const ReplayFile* y ) -> int {
    switch ( column_index ) {
    case 0: return y->FirstPlayerCompare( x );
    case 1: return y->SecondPlayreCompare( x );
    default: // walk through
    case 2: return y->TimeCompare( x );
    case 3: return static_cast<int>( y->first_player_rounds_won_ ) - static_cast<int>( x->first_player_rounds_won_ );
    case 4: return static_cast<int>( y->second_player_rounds_won_ ) - static_cast<int>( x->second_player_rounds_won_ );
    }
  }, ascending );
}
