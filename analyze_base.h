// analyze_base.h

#include "exception.h"
#include "base.h"


namespace GGXXACPRReplayOrganizer::Analyze {
  // -- 集計データ用 -----------------------------------------------------
  // -- ListEntry -----
  class ListEntry {
  public:
    static int Compare( const ListEntry& x, const ListEntry& y ) {
      if ( auto ret = x.Compare( y ); ret ) {
        return ret.value();
      }
      if ( auto ret = y.Compare( x ); ret ) {
        return (-1) * ret.value();
      }
      throw GGXXACPR_REPLAY_ORGANIZER_INTERNAL_EXCEPTION;
    }

    template <class TYPE>
    bool Is( void ) const { return dynamic_cast<const TYPE*>( this ) != 0; }

    template <class TYPE>
    const TYPE& CastTo( void ) const {
      if ( NOT( this->Is<TYPE>() ) ) {
        throw GGXXACPR_REPLAY_ORGANIZER_INTERNAL_EXCEPTION;
      }
      return *dynamic_cast<const TYPE*>( this );
    }

    virtual std::optional<int> Compare( const ListEntry& ) const = 0;

    virtual std::wstring GetName( void ) const = 0;
    virtual unsigned int GetTotalWin( void ) const = 0;
    virtual unsigned int GetTotalLose( void ) const = 0;
    virtual unsigned int GetTotalDraw( void ) const = 0;
    virtual unsigned int GetTotalUnknown( void ) const = 0;

    unsigned int GetTotalCount( void ) const { return this->GetTotalWin() + this->GetTotalLose() + this->GetTotalDraw() + this->GetTotalUnknown(); }
    double GetWinRatio( void ) const { return static_cast<double>( this->GetTotalWin() ) / static_cast<double>( this->GetTotalCount() ); }
  };

  // -- ListEntryEnumerable -----
  class ListEntryEnumerable : public ListEntry {
  public:
    virtual std::vector<const ListEntry*> GetListEntryArray( void ) const = 0;

    template <class X, class Y, class Z>
    std::vector<const ListEntry*> GetListEntryArrayFromHash( const std::unordered_map<X, Y, Z>& hash ) const {
      std::vector<const ListEntry*> array;
      for ( auto& one : hash ) {
        array.push_back( &one.second );
      }
      return array;
    }
  };

  // -- ListEntryEnumerableAndForSubDialog -----
  class ListEntryEnumerableAndForSubDialog : public ListEntryEnumerable {
  public:
    virtual std::wstring GetDescriptionForSubDialog( void ) const = 0;
  };

  // -- ListEntryForTotal -----
  class ListEntryForTotal : public ListEntry {
  public:
    explicit ListEntryForTotal( void ) : body_( nullptr ) {}
    void Reset( const ListEntryEnumerable* body ) { body_ = body; }

    virtual std::optional<int> Compare( const ListEntry& other ) const { return body_->Compare( other ); }

    virtual std::wstring GetName( void ) const override { return L"TOTAL"; }
    virtual unsigned int GetTotalWin( void ) const override { return body_->GetTotalWin(); }
    virtual unsigned int GetTotalLose( void ) const override { return body_->GetTotalLose(); }
    virtual unsigned int GetTotalDraw( void ) const override { return body_->GetTotalDraw(); }
    virtual unsigned int GetTotalUnknown( void ) const override { return body_->GetTotalUnknown(); }

  private:
    const ListEntryEnumerable* body_;
  };

  // -- RecordBase -----
  class RecordBase : public ListEntry {
  public:
    virtual std::wstring GetDescriptionForRecordBaseDialog( void ) const = 0;

    virtual unsigned int GetTotalWin( void ) const override   { return win_; }
    virtual unsigned int GetTotalLose( void ) const override  { return lose_; }
    virtual unsigned int GetTotalDraw( void ) const override  { return draw_; }
    virtual unsigned int GetTotalUnknown( void ) const override  { return unknown_; }

    void SetDown( ReplayFile* replay, bool first_is_self ) {
      switch ( replay->GetGameResult( first_is_self ) ) {
      case GameResult::Win:  win_  += 1; break;
      case GameResult::Lose: lose_ += 1; break;
      case GameResult::Draw: draw_ += 1; break;
      case GameResult::Unknown: // walk through
      default: unknown_ += 1; break;
      }
      replay_files_.push_back( replay );
    }

    unsigned int win_  = 0;
    unsigned int lose_ = 0;
    unsigned int draw_ = 0;
    unsigned int unknown_ = 0;

    std::vector<ReplayFile*> replay_files_;
  };

  // -- キャラ毎 -----
  namespace ByCharacter {
    struct RecordOfOpponent : public RecordBase {
      explicit RecordOfOpponent( const std::wstring& opponent_name, const std::wstring& description_for_record_base );

      virtual std::optional<int> Compare( const ListEntry& other ) const override;

      virtual std::wstring GetName( void ) const override;

      virtual std::wstring GetDescriptionForRecordBaseDialog( void ) const;

      std::wstring opponent_name_;
      std::wstring description_for_record_base_;
    };

    // opponent_name => RecordOfOpponent
    struct RecordOfOpponentCharacter : public std::unordered_map<PlayerNameSymbol, RecordOfOpponent>,
                                       public ListEntryEnumerableAndForSubDialog {
      explicit RecordOfOpponentCharacter( Character opponent_character, const std::wstring description_for_sub );

      virtual std::wstring GetDescriptionForSubDialog( void ) const override;

      virtual std::optional<int> Compare( const ListEntry& other ) const override;

      virtual std::wstring GetName( void ) const override;
      virtual unsigned int GetTotalWin( void ) const override;
      virtual unsigned int GetTotalLose( void ) const override;
      virtual unsigned int GetTotalDraw( void ) const override;
      virtual unsigned int GetTotalUnknown( void ) const override;

      virtual std::vector<const ListEntry*> GetListEntryArray( void ) const override;

      void SetDown( ReplayFile* replay, bool first_is_self );

      Character    opponent_character_;
      std::wstring description_for_sub_;
    };

    // opponent_character => RecordOfOpponentCharacter
    struct RecordOfSelfCharacter : public std::unordered_map<Character, RecordOfOpponentCharacter>,
                                   public ListEntryEnumerable {
      explicit RecordOfSelfCharacter( Character self_character );

      virtual std::optional<int> Compare( const ListEntry& other ) const override;

      virtual std::wstring GetName( void ) const override;
      virtual unsigned int GetTotalWin( void ) const override;
      virtual unsigned int GetTotalLose( void ) const override;
      virtual unsigned int GetTotalDraw( void ) const override;
      virtual unsigned int GetTotalUnknown( void ) const override;

      virtual std::vector<const ListEntry*> GetListEntryArray( void ) const override;

      void SetDown( ReplayFile* replay, bool first_is_self );

      Character self_character_;
    };

    // self_character => RecordOfSelfCharacter
    struct Record : public std::unordered_map<Character, RecordOfSelfCharacter>,
                    public ListEntryEnumerable {
      virtual std::optional<int> Compare( const ListEntry& other ) const override;

      virtual std::wstring GetName( void ) const override;
      virtual unsigned int GetTotalWin( void ) const override;
      virtual unsigned int GetTotalLose( void ) const override;
      virtual unsigned int GetTotalDraw( void ) const override;
      virtual unsigned int GetTotalUnknown( void ) const override;

      virtual std::vector<const ListEntry*> GetListEntryArray( void ) const override;

      void SetDown( ReplayFile* replay, bool first_is_self );
    };
  }

  // -- 全キャラ -----
  namespace ByAllCharacter {
    using RecordOfOpponent = ByCharacter::RecordOfOpponent;

    struct RecordOfOpponentCharacter : public ByCharacter::RecordOfOpponentCharacter {
      using ByCharacter::RecordOfOpponentCharacter::RecordOfOpponentCharacter;

      void SetDown( ReplayFile* replay, bool first_is_self );
    };

    // opponent_character => RecordOfOpponentCharacter
    struct Record : public std::unordered_map<Character, RecordOfOpponentCharacter>,
                    public ListEntryEnumerable {

      virtual std::optional<int> Compare( const ListEntry& other ) const override;

      virtual std::wstring GetName( void ) const override;
      virtual unsigned int GetTotalWin( void ) const override;
      virtual unsigned int GetTotalLose( void ) const override;
      virtual unsigned int GetTotalDraw( void ) const override;
      virtual unsigned int GetTotalUnknown( void ) const override;

      virtual std::vector<const ListEntry*> GetListEntryArray( void ) const override;

      void SetDown( ReplayFile* replay, bool first_is_self );
    };
  }

  // -- 対戦相手毎 -----
  namespace ByOpponent {
    struct RecordOfCharacter : public RecordBase {
      explicit RecordOfCharacter( Character self, Character opponent, const std::wstring& description_for_record_base );

      virtual std::optional<int> Compare( const ListEntry& other ) const override;

      virtual std::wstring GetName( void ) const override;

      virtual std::wstring GetDescriptionForRecordBaseDialog( void ) const;

      Character self_;
      Character opponent_;

      std::wstring description_for_record_base_;
    };

    struct CharacterHash {
      size_t operator ()( const std::pair<Character, Character>& val ) const noexcept {
        return static_cast<size_t>( val.first.ToInteger() ) * 100 + static_cast<size_t>( val.second.ToInteger() );
      }
    };

    // std::pair<self_character, opponent_character> => RecordOfCharacter
    struct RecordOfOpponent : public std::unordered_map<std::pair<Character, Character>, RecordOfCharacter, CharacterHash>,
                              public ListEntryEnumerableAndForSubDialog {
      explicit RecordOfOpponent( const std::wstring& opponent_name, const std::wstring& self_name );

      virtual std::wstring GetDescriptionForSubDialog( void ) const override;

      virtual std::optional<int> Compare( const ListEntry& other ) const override;

      virtual std::wstring GetName( void ) const override;
      virtual unsigned int GetTotalWin( void ) const override;
      virtual unsigned int GetTotalLose( void ) const override;
      virtual unsigned int GetTotalDraw( void ) const override;
      virtual unsigned int GetTotalUnknown( void ) const override;

      virtual std::vector<const ListEntry*> GetListEntryArray( void ) const override;

      void SetDown( ReplayFile* replay, bool first_is_self );

      std::wstring opponent_name_;
      std::wstring self_name_;
    };

    // opponent_name => RecordOfOpponent
    struct Record : public std::unordered_map<PlayerNameSymbol, RecordOfOpponent>,
                    public ListEntryEnumerable {

      virtual std::optional<int> Compare( const ListEntry& other ) const override;

      virtual std::wstring GetName( void ) const override;
      virtual unsigned int GetTotalWin( void ) const override;
      virtual unsigned int GetTotalLose( void ) const override;
      virtual unsigned int GetTotalDraw( void ) const override;
      virtual unsigned int GetTotalUnknown( void ) const override;

      virtual std::vector<const ListEntry*> GetListEntryArray( void ) const override;

      void SetDown( ReplayFile* replay, bool first_is_self );
    };
  }
}
