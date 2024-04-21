// base.cpp

#include <array>
#include <regex>

#include "tt_utility.h"
#include "tt_time.h"

#include "utility.h"
#include "exception.h"

#include "base.h"

using namespace GGXXACPRReplayOrganizer;



// -- PlayerNameSymbol ---------------------------------------------------
namespace {
  std::unordered_map<std::wstring, unsigned int> player_name_symbol_table;
}

PlayerNameSymbol
PlayerNameSymbol::GetSymbol( const std::wstring& name )
{
  if ( player_name_symbol_table.contains( name ) ) {
    return PlayerNameSymbol( player_name_symbol_table[name] );
  }
  unsigned int count = static_cast<unsigned int>( player_name_symbol_table.size() );
  player_name_symbol_table[name] = count;
  return PlayerNameSymbol( count );
}

PlayerNameSymbol::PlayerNameSymbol( void ) :
number_( std::numeric_limits<unsigned int>::max() )
{
}

PlayerNameSymbol::PlayerNameSymbol( unsigned int number ) :
number_( number )
{
}


// -- Character ----------------------------------------------------------
Character::Character( void ) :
value_( Character::Unknown )
{
}

Character::Character( Value value ) :
value_( value )
{
}

std::wstring
Character::ToFullName( void ) const
{
  static const std::unordered_map<Value, std::wstring> table = {
    {Character::Unknown,   L"UNKNOWN"},
    {Character::Sol,       L"SOL-BADGUY"},
    {Character::Ky,        L"KY-KISKE"},
    {Character::May,       L"MAY"},
    {Character::Millia,    L"MILLIA-RAGE"},
    {Character::Eddie,     L"EDDIE"},
    {Character::Potemkin,  L"POTEMKIN"},
    {Character::Chipp,     L"CHIPP-ZANUFF"},
    {Character::Faust,     L"FAUST"},
    {Character::Axl,       L"AXL-LOW"},
    {Character::Testament, L"TESTAMENT"},
    {Character::Baiken,    L"BAIKEN"},
    {Character::Jam,       L"JAM-KURADOBERI"},
    {Character::Johnny,    L"JONNY"},
    {Character::Anji,      L"ANJI-MITO"},
    {Character::Venom,     L"VENOM"},
    {Character::Dizzy,     L"DIZZY"},
    {Character::Slayer,    L"SLAYER"},
    {Character::INo,       L"I-NO"},
    {Character::Zappa,     L"ZAPPA"},
    {Character::Bridget,   L"BRIDGET"},
    {Character::RoboKy,    L"ROBO-KY"},
    {Character::Aba,       L"A.B.A"},
    {Character::OrderSol,  L"ORDER-SOL"},
    {Character::Kliff,     L"KLIFF-ANDERSN"},
    {Character::Justice,   L"JUSTICE"},
  };
  if ( table.contains( value_ ) ) {
    return table.at( value_ );
  }
  return table.at( Character::Unknown );
}

std::wstring
Character::ToShortName( void ) const
{
  static const std::unordered_map<Value, std::wstring> table = {
    {Character::Unknown,   L"UNKNOWN"},
    {Character::Sol,       L"SO"},
    {Character::Ky,        L"KY"},
    {Character::May,       L"MA"},
    {Character::Millia,    L"MI"},
    {Character::Eddie,     L"ED"},
    {Character::Potemkin,  L"PO"},
    {Character::Chipp,     L"CH"},
    {Character::Faust,     L"FA"},
    {Character::Axl,       L"AX"},
    {Character::Testament, L"TE"},
    {Character::Baiken,    L"BA"},
    {Character::Jam,       L"JA"},
    {Character::Johnny,    L"JO"},
    {Character::Anji,      L"AN"},
    {Character::Venom,     L"VE"},
    {Character::Dizzy,     L"DI"},
    {Character::Slayer,    L"SL"},
    {Character::INo,       L"IN"},
    {Character::Zappa,     L"ZA"},
    {Character::Bridget,   L"BR"},
    {Character::RoboKy,    L"RO"},
    {Character::Aba,       L"AB"},
    {Character::OrderSol,  L"OS"},
    {Character::Kliff,     L"KL"},
    {Character::Justice,   L"JU"},
  };
  if ( table.contains( value_ ) ) {
    return table.at( value_ );
  }
  return table.at( Character::Unknown );
}


unsigned int
Character::ToInteger( void ) const
{
  return value_;
}


Character
Character::GetNext( void ) const
{
  return static_cast<Value>( value_ + 1 );
}

// -- ReplayFile ---------------------------------------------------------
std::optional<ReplayFile>
ReplayFile::Parse( const std::wstring& path )
{
  ReplayFile replay_file;
  replay_file.path_ = path;
  std::wstring basename = Path::BaseName( path );
  const wchar_t* c = basename.c_str();

  auto to_integer = [&c] <int size> ( unsigned int& value ) -> bool {
    std::array<wchar_t, size + 1> buffer;
    buffer[size] = L'\0';
    for ( unsigned int i = 0; i < size; ++i ) {
      if ( *c == L'\0' ) {
        return false;
      }
      if ( NOT( ::iswdigit( *c ) ) ) {
        if ( i == 0 ) {
          return false;
        }
        buffer[i] = L'\0';
        break;
      }
      buffer[i] = *c;
      ++c;
    }
    bool ret = Utility::StringToInteger( buffer.data(), &value );
    if( NOT( ret ) ) {
      return false;
    }
    return true;
  };

  auto to_integer_4 = [&to_integer] ( unsigned int& value ) {
    return to_integer.operator()<4>( value );
  };
  auto to_integer_2 = [&to_integer] ( unsigned int& value ) {
    return to_integer.operator()<2>( value );
  };
  auto to_integer_8 = [&to_integer] ( unsigned int& value ) {
    return to_integer.operator()<8>( value );
  };

  if ( NOT( to_integer_4( replay_file.year_ ) ) ) {
    return std::nullopt;
  }
  if ( NOT( to_integer_2( replay_file.month_ ) ) ) {
    return std::nullopt;
  }
  if ( NOT( to_integer_2( replay_file.day_ ) ) ) {
    return std::nullopt;
  }
  if ( *c != L'_' ) {
    return std::nullopt;
  }
  ++c;
  if ( NOT( to_integer_8( replay_file.id_of_day_ ) ) ) {
    return std::nullopt;
  }
  if ( *c != L'_' ) {
    return std::nullopt;
  }
  ++c;

  auto to_character = [] ( const wchar_t* str ) -> Character {
    switch ( *reinterpret_cast<const unsigned int*>( str ) ) {
    case 0x004f0053: return Character::Sol; // SO
    case 0x0059004b: return Character::Ky; // KY
    case 0x0041004d: return Character::May; // MA
    case 0x0049004d: return Character::Millia; // MI
    case 0x00440045: return Character::Eddie; // ED
    case 0x004f0050: return Character::Potemkin; // PO
    case 0x00480043: return Character::Chipp; // CH
    case 0x00410046: return Character::Faust; // FA
    case 0x00580041: return Character::Axl; // AX
    case 0x00450054: return Character::Testament; // TE
    case 0x00410042: return Character::Baiken; // BA
    case 0x0041004a: return Character::Jam; // JA
    case 0x004f004a: return Character::Johnny; // JO
    case 0x004e0041: return Character::Anji; // AN
    case 0x00450056: return Character::Venom; // VE
    case 0x00490044: return Character::Dizzy; // DI
    case 0x004c0053: return Character::Slayer; // SL
    case 0x004e0049: return Character::INo; // IN
    case 0x0041005a: return Character::Zappa; // ZA
    case 0x00520042: return Character::Bridget; // BR
    case 0x004f0052: return Character::RoboKy; // RO
    case 0x00420041: return Character::Aba; // AB
    case 0x0053004f: return Character::OrderSol; // OS
    case 0x004c004b: return Character::Kliff; // KL
    case 0x0055004a: return Character::Justice; // JU
    default: return Character::Unknown;
    };
  };

  auto end_of_player_name = [&c, &replay_file, &to_character] ( bool first ) -> bool {
    const wchar_t* current = c;
    if ( *current != L'(' ) {
      return false;
    }
    ++current;

    std::array<wchar_t, 2> character_str;
    if ( NOT( ::iswalpha( *current ) ) ) {
      return false;
    }
    character_str[0] = *current;
    ++current;
    if ( NOT( ::iswalpha( *current ) ) ) {
      return false;
    }
    character_str[1] = *current;
    ++current;

    if ( *current != L')' ) {
      return false;
    }
    ++current;
    const wchar_t* pattern = first ? L"_vs_" : L".ggr";
    for ( unsigned int i = 0; i < 4; ++i ) {
      if( *current != pattern[i] ) {
        return false;
      }
      current++;
    }
    (first ? replay_file.first_player_character_ : replay_file.second_player_character_) = to_character( character_str.data() );
    return true;
  };
  auto end_of_first_player_name = [&end_of_player_name] ( void ) -> bool {
    return end_of_player_name( true );
  };
  auto end_of_second_player_name = [&end_of_player_name] ( void ) -> bool {
    return end_of_player_name( false );
  };

  while ( NOT( end_of_first_player_name() ) ) {
    replay_file.first_player_name_.append( 1, *c );
    c++;
  }
  replay_file.first_player_name_symbol_ = PlayerNameSymbol::GetSymbol( replay_file.first_player_name_ );

  // skip (ZZ)_vs_
  c += 8;

  while ( NOT( end_of_second_player_name() ) ) {
    replay_file.second_player_name_.append( 1, *c );
    c++;
  }
  replay_file.second_player_name_symbol_ = PlayerNameSymbol::GetSymbol( replay_file.second_player_name_ );

  // skip (ZZ).ggr
  c += 8;

  if ( *c != L'\0' ) {
    return std::nullopt;
  }
  return replay_file;
}



void
ReplayFile::ReadMetaData( void )
{
#pragma pack(push, 1)
  struct MetaData {
    std::byte signature_[12];
    uint16_t  metadata_size_;
    std::byte padding1_[12];

    uint16_t  year_;
    uint8_t   month_;
    uint8_t   day_;
    uint8_t   hour_;
    uint8_t   minute_;
    uint8_t   second_;
    std::byte padding2_[1];

    uint64_t P1_steam_ID_;
    uint64_t P2_steam_ID_;

    char P1_name_[32];
    char P2_name_[32];

    uint8_t P1_character_;
    uint8_t P2_character_;
    uint8_t extra_options_modified_;
    uint8_t single_or_team_;
    uint8_t PR_or_AC_;
    uint32_t timezone_bias_;
    uint8_t P1_rounds_won_;
    uint8_t P2_rounds_won_;
    uint8_t match_result_bitmask_;
    uint8_t ping_;
    uint32_t match_duration_;
    uint8_t P1_score_;
    uint8_t P2_score_;
    uint8_t P1_rank_;
    uint8_t P2_rank_;
    uint8_t winner_side_;
  };
#pragma pack(pop)

  if ( meta_data_loaded ) {
    return;
  }

  MetaData data;
  FILE* f;

  auto error_number = ::_wfopen_s( &f, path_.c_str(), L"rb" );
  if( error_number != 0 ) {
    file_read_error_occured_ = true;
    return;
  }
  TtUtility::DestructorCall obj( [&f] ( void ) { ::fclose( f ); } );

  auto ret = ::fread( &data, sizeof( MetaData ), 1, f );
  if ( ret != 1 ) {
    file_read_error_occured_ = true;
    return;
  }

  meta_data_loaded = true;

  hour_   = data.hour_;
  minute_ = data.minute_;
  second_ = data.second_;
  first_player_rounds_won_  = data.P1_rounds_won_;
  second_player_rounds_won_ = data.P2_rounds_won_;

  if ( data.metadata_size_ <= 0x5D ) {
    winner_side_ = WinnerSide::Unknown;
    first_player_rounds_won_ = 0;
    second_player_rounds_won_ = 0;
  }
  else if ( data.metadata_size_ <= 0x63 ) {
    if ( data.P1_rounds_won_ == data.P2_rounds_won_ ) {
      winner_side_ = WinnerSide::Draw;
    }
    else if ( data.P1_rounds_won_ > data.P2_rounds_won_ ) {
      winner_side_ = WinnerSide::P1;
    }
    else {
      winner_side_ = WinnerSide::P2;
    }
  }
  else {
    winner_side_ = static_cast<WinnerSide>( data.winner_side_ );
  }
}


std::wstring
ReplayFile::GetTimeString( void ) const
{
  return std::format( L"{:04d}/{:02d}/{:02d} {:02d}:{:02d}:{:02d}", year_, month_, day_, hour_, minute_, second_ );
}

GameResult
ReplayFile::GetGameResult( bool first_is_self )
{
  switch ( winner_side_ ) {
  case WinnerSide::P1:
    return first_is_self ? GameResult::Win : GameResult::Lose;

  case WinnerSide::P2:
    return first_is_self ? GameResult::Lose : GameResult::Win;

  case WinnerSide::Draw:
    return GameResult::Draw;

  case WinnerSide::Unknown: // walk through
  default:
    return GameResult::Unknown;
  }
}


std::wstring
ReplayFile::GetSelfName( bool first_is_self )
{
  return first_is_self ? first_player_name_ : second_player_name_;
}

PlayerNameSymbol
ReplayFile::GetSelfNameSymbol( bool first_is_self )
{
  return first_is_self ? first_player_name_symbol_ : second_player_name_symbol_;
}

Character
ReplayFile::GetSelfCharacter( bool first_is_self )
{
  return first_is_self ? first_player_character_ : second_player_character_;
}

std::wstring
ReplayFile::GetOpponentName( bool first_is_self )
{
  return first_is_self ? second_player_name_ : first_player_name_;
}

PlayerNameSymbol
ReplayFile::GetOpponentNameSymbol( bool first_is_self )
{
  return first_is_self ? second_player_name_symbol_ : first_player_name_symbol_;
}

Character
ReplayFile::GetOpponentCharacter( bool first_is_self )
{
  return first_is_self ? second_player_character_ : first_player_character_;
}


int
ReplayFile::FirstPlayerCompare( const ReplayFile* other ) const
{
  int tmp = other->first_player_name_.compare( this->first_player_name_ );
  if ( tmp != 0 ) {
    return tmp;
  }
  return static_cast<int>( other->first_player_character_.ToInteger() ) - static_cast<int>( this->first_player_character_.ToInteger() );
}

int
ReplayFile::SecondPlayreCompare( const ReplayFile* other ) const
{
  int tmp = other->second_player_name_.compare( this->second_player_name_ );
  if ( tmp != 0 ) {
    return tmp;
  }
  return static_cast<int>( other->second_player_character_.ToInteger() ) - static_cast<int>( this->second_player_character_.ToInteger() );
}

int
ReplayFile::TimeCompare( const ReplayFile* other ) const
{
  std::vector<unsigned int ReplayFile::*> v = {
    &ReplayFile::year_,
    &ReplayFile::month_,
    &ReplayFile::day_,
    &ReplayFile::hour_,
    &ReplayFile::minute_,
    &ReplayFile::second_,
  };
  for ( auto& one : v ) {
    if ( auto comp = this->*one <=> other->*one; comp != 0 ) {
      return comp > 0 ? 1 : -1;
    }
  }
  return 0;
}
