// base.h

#pragma once

#include <optional>
#include <string>

#include "tt_enum.h"


namespace GGXXACPRReplayOrganizer {
  enum class WinnerSide : unsigned int {
    Unknown = 0,
    P1      = 1,
    P2      = 2,
    Draw    = 3,
  };

  enum class GameResult : unsigned int {
    Unknown = 0,
    Win     = 1,
    Lose    = 2,
    Draw    = 3,
  };

  // -- PlayerNameSymbol -------------------------------------------------
  class PlayerNameSymbol {
  public:
    static PlayerNameSymbol GetSymbol( const std::wstring& name );

  public:
    explicit PlayerNameSymbol( void );

  private:
    explicit PlayerNameSymbol( unsigned int number );

  public:
    friend bool operator ==( const PlayerNameSymbol&, const PlayerNameSymbol& ) = default;
    friend auto operator <=>( const PlayerNameSymbol&, const PlayerNameSymbol& ) = default;

    struct Hash {
      size_t operator () ( const PlayerNameSymbol& c ) const { return std::hash<unsigned int>()( c.number_ ); }
    };

  private:
    unsigned int number_;
  };

  // -- Character --------------------------------------------------------
  class Character {
  public:
    enum Value {
      Unknown = 0,
      Sol,
      Ky,
      May,
      Millia,
      Axl,
      Potemkin,
      Chipp,
      Eddie,
      Baiken,
      Faust,
      Testament,
      Jam,
      Anji,
      Johnny,
      Venom,
      Dizzy,
      Slayer,
      INo,
      Zappa,
      Bridget,
      RoboKy,
      Aba,
      OrderSol,
      Kliff,
      Justice,
    };

    explicit Character( void );
    Character( Value value );

    friend bool operator ==( const Character&, const Character& ) = default;
    friend auto operator <=>( const Character&, const Character& ) = default;

    struct Hash {
      size_t operator () ( const Character& c ) const { return std::hash<unsigned int>()( c.value_ ); }
    };

    std::wstring ToFullName( void ) const;
    std::wstring ToShortName( void ) const;

    unsigned int ToInteger( void ) const;

    Character GetNext( void ) const;

  private:
    Value value_;
  };

  // -- ReplayFile -------------------------------------------------------
  class ReplayFile {
  public:
    static std::optional<ReplayFile> Parse( const std::wstring& path );

  public:
    void ReadMetaData( void );

    std::wstring GetTimeString( void ) const;

    GameResult GetGameResult( bool first_is_self );

    std::wstring     GetSelfName( bool first_is_self );
    PlayerNameSymbol GetSelfNameSymbol( bool first_is_self );
    Character        GetSelfCharacter( bool first_is_self );
    std::wstring     GetOpponentName( bool first_is_self );
    PlayerNameSymbol GetOpponentNameSymbol( bool first_is_self );
    Character        GetOpponentCharacter( bool first_is_self );

    int FirstPlayerCompare( const ReplayFile* other ) const;
    int SecondPlayreCompare( const ReplayFile* other ) const;
    int TimeCompare( const ReplayFile* other ) const;

  public:
    std::wstring path_;
    bool meta_data_loaded = false;
    bool file_read_error_occured_ = false;

    unsigned int year_;
    unsigned int month_;
    unsigned int day_;
    unsigned int id_of_day_;
    unsigned int hour_;
    unsigned int minute_;
    unsigned int second_;

    std::wstring     first_player_name_;
    PlayerNameSymbol first_player_name_symbol_;
    Character        first_player_character_;
    std::wstring     second_player_name_;
    PlayerNameSymbol second_player_name_symbol_;
    Character        second_player_character_;
    unsigned int     first_player_rounds_won_;
    unsigned int     second_player_rounds_won_;
    WinnerSide       winner_side_;
  };
}

template <>
class std::hash<GGXXACPRReplayOrganizer::Character> : public GGXXACPRReplayOrganizer::Character::Hash {};

template <>
class std::hash<GGXXACPRReplayOrganizer::PlayerNameSymbol> : public GGXXACPRReplayOrganizer::PlayerNameSymbol::Hash {};
