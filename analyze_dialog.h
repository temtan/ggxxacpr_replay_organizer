// analyze_dialog.h

#include <optional>

#include "tt_thread.h"
#include "tt_progress.h"

#include "utf16_window.h"
#include "core.h"
#include "analyze_base.h"


namespace GGXXACPRReplayOrganizer {
  // -- AnalyzeList ------------------------------------------------------
  class AnalyzeList : public UTF16ListView {
  public:
    class Item : public UTF16ListView::Item {
    public:
      explicit Item( const TtListViewItem& other ) : UTF16ListView::Item( other ) {};

      void SetParameter( const Analyze::ListEntry* entry ) {
        this->SetParameterAs<const Analyze::ListEntry*>( entry );
      }
      const Analyze::ListEntry* GetParameter( void ) {
        return this->GetParameterAs<const Analyze::ListEntry*>();
      }
    };

    void InitializeColumn( void );

    void Sort( unsigned int column_index, bool ascending );

    void Input( const Analyze::ListEntryEnumerable& enumerable );

  private:
    Analyze::ListEntryForTotal list_entry_for_total_;
  };

  // -- AnalyzeDialog ----------------------------------------------------
  class AnalyzeDialog : public UTF16Dialog {
  public:
    explicit AnalyzeDialog( const std::wstring& self_player_name, Core& core );

    virtual DWORD GetStyle( void ) override {
      return WS_DLGFRAME | WS_CAPTION | WS_SIZEBOX | WS_SYSMENU;
    }
    virtual DWORD GetExtendedStyle( void ) {
      return WS_EX_TOOLWINDOW;
    }

    virtual bool Created( void ) override;

    void SortList( unsigned int column_index );

    void Initialize( void );
    void InitializeEnd( void );

  private:
    std::wstring self_player_name_;
    Core& core_;

    Analyze::ByCharacter::Record    by_character_record_;
    Analyze::ByAllCharacter::Record by_all_character_record_;
    Analyze::ByOpponent::Record     by_opponent_record_;

    UTF16Window<TtStatic> loading_label_;
    TtProgressSmooth      progress_;

    using UseCharacterLabel = TtWindowWithStyle<UTF16Window<TtStatic>, SS_RIGHT>;
    UseCharacterLabel label_;
    AnalyzeComboBox   combo_;
    AnalyzeList       list_;

    TtFunctionThread initialize_thread_;

    unsigned int last_sort_index_ = std::numeric_limits<unsigned int>::max();
    bool last_ascending_;

    std::unordered_map<unsigned int, Character> combo_table_;
  };

  // -- AnalyzeSubDialog -------------------------------------------------
  class AnalyzeSubDialog : public UTF16Dialog {
  public:
    explicit AnalyzeSubDialog( const Analyze::ListEntryEnumerableAndForSubDialog& enumerable );

    virtual DWORD GetStyle( void ) override {
      return WS_DLGFRAME | WS_CAPTION | WS_SIZEBOX | WS_SYSMENU;
    }
    virtual DWORD GetExtendedStyle( void ) {
      return WS_EX_TOOLWINDOW;
    }

    virtual bool Created( void ) override;

    void SortList( unsigned int column_index );

  private:
    const Analyze::ListEntryEnumerableAndForSubDialog& enumerable_;

    UTF16Window<TtStatic> label_;
    AnalyzeList           list_;

    unsigned int last_sort_index_ = std::numeric_limits<unsigned int>::max();
    bool last_ascending_;
  };

  // -- AnalyzeRecordBaseDialog ------------------------------------------
  class AnalyzeRecordBaseDialog : public UTF16Dialog {
  public:
    explicit AnalyzeRecordBaseDialog( const Analyze::RecordBase& record_base );

    virtual DWORD GetStyle( void ) override {
      return WS_DLGFRAME | WS_CAPTION | WS_SIZEBOX | WS_SYSMENU;
    }
    virtual DWORD GetExtendedStyle( void ) {
      return WS_EX_TOOLWINDOW;
    }

    virtual bool Created( void ) override;

    void InitializeList( void );
    void SortList( unsigned int column_index );

  private:
    const Analyze::RecordBase& record_base_;

    UTF16Window<TtStatic> label_;
    UTF16ListView         list_;

    unsigned int last_sort_index_ = std::numeric_limits<unsigned int>::max();
    bool last_ascending_;
  };
}
