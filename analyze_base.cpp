// analyze_base.cpp

#include "string_table.h"

#include "analyze_base.h"

using namespace GGXXACPRReplayOrganizer;


// -- 集計データ用 -------------------------------------------------------
namespace {
  // auto get_sum_of = [] ( auto& collection, std::function<unsigned int ( const typename decltype( collection )::value_type& )> func ) -> unsigned int {
  auto get_sum_of = [] ( auto& collection, auto func ) -> unsigned int {
    unsigned int sum = 0;
    for ( auto& tmp : collection ) {
      sum += func( tmp.second );
    }
    return sum;
  };
}


// -- ByCharacter::RecordOfOpponent -----
Analyze::ByCharacter::RecordOfOpponent::RecordOfOpponent( const std::wstring& opponent_name, const std::wstring& description_for_record_base ) :
opponent_name_( opponent_name ),
description_for_record_base_( description_for_record_base )
{
}


std::optional<int>
Analyze::ByCharacter::RecordOfOpponent::Compare( const ListEntry& other ) const
{
  if ( other.Is<RecordOfOpponent>() ) {
    return opponent_name_.compare( other.CastTo<RecordOfOpponent>().opponent_name_ );
  }
  return std::nullopt;
}


std::wstring
Analyze::ByCharacter::RecordOfOpponent::GetName( void ) const
{
  return opponent_name_;
}

std::wstring
Analyze::ByCharacter::RecordOfOpponent::GetDescriptionForRecordBaseDialog( void ) const
{
  return description_for_record_base_;
}


// -- ByCharacter::RecordOfOpponentCharacter -----
Analyze::ByCharacter::RecordOfOpponentCharacter::RecordOfOpponentCharacter( Character opponent_character, const std::wstring description_for_sub ) :
std::unordered_map<PlayerNameSymbol, RecordOfOpponent>(),
opponent_character_( opponent_character ),
description_for_sub_( description_for_sub )
{
}


std::wstring
Analyze::ByCharacter::RecordOfOpponentCharacter::GetDescriptionForSubDialog( void ) const
{
  return description_for_sub_;
}


std::optional<int>
Analyze::ByCharacter::RecordOfOpponentCharacter::Compare( const ListEntry& other ) const
{
  if ( other.Is<RecordOfOpponentCharacter>() ) {
    return opponent_character_.ToInteger() - other.CastTo<RecordOfOpponentCharacter>().opponent_character_.ToInteger();
  }
  if ( other.Is<RecordOfOpponent>() ) {
    return -1;
  }
  return std::nullopt;
}


std::wstring
Analyze::ByCharacter::RecordOfOpponentCharacter::GetName( void ) const
{
  return opponent_character_.ToFullName();
}

unsigned int
Analyze::ByCharacter::RecordOfOpponentCharacter::GetTotalWin( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.win_; } );
}

unsigned int
Analyze::ByCharacter::RecordOfOpponentCharacter::GetTotalLose( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.lose_; } );
}

unsigned int
Analyze::ByCharacter::RecordOfOpponentCharacter::GetTotalDraw( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.draw_; } );
}

unsigned int
Analyze::ByCharacter::RecordOfOpponentCharacter::GetTotalUnknown( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.unknown_; } );
}


std::vector<const Analyze::ListEntry*>
Analyze::ByCharacter::RecordOfOpponentCharacter::GetListEntryArray( void ) const
{
  return this->GetListEntryArrayFromHash( *this );
}

void
Analyze::ByCharacter::RecordOfOpponentCharacter::SetDown( ReplayFile* replay, bool first_is_self )
{
  if ( NOT( this->contains( replay->GetOpponentNameSymbol( first_is_self ) ) ) ) {
    std::wstring description = std::format(
      L"{} ({}) vs {} ({})",
      replay->GetSelfName( first_is_self ),     replay->GetSelfCharacter( first_is_self ).ToFullName(),
      replay->GetOpponentName( first_is_self ), replay->GetOpponentCharacter( first_is_self ).ToFullName() );

    this->emplace( std::piecewise_construct,
                   std::forward_as_tuple( replay->GetOpponentNameSymbol( first_is_self ) ),
                   std::forward_as_tuple( replay->GetOpponentName( first_is_self ), description ) );
  }
  this->at( replay->GetOpponentNameSymbol( first_is_self ) ).SetDown( replay, first_is_self );
}


// -- ByCharacter::RecordOfSelfCharacter -----
Analyze::ByCharacter::RecordOfSelfCharacter::RecordOfSelfCharacter( Character self_character ) :
std::unordered_map<Character, RecordOfOpponentCharacter>(),
self_character_( self_character )
{
}

std::optional<int>
Analyze::ByCharacter::RecordOfSelfCharacter::Compare( const ListEntry& other ) const
{
  if ( other.Is<RecordOfSelfCharacter>() ) {
    return self_character_.ToInteger() - other.CastTo<RecordOfSelfCharacter>().self_character_.ToInteger();
  }
  if ( other.Is<RecordOfOpponentCharacter>() ) {
    return -1;
  }
  return std::nullopt;
}


std::wstring
Analyze::ByCharacter::RecordOfSelfCharacter::GetName( void ) const
{
  return self_character_.ToFullName();
}

unsigned int
Analyze::ByCharacter::RecordOfSelfCharacter::GetTotalWin( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.GetTotalWin(); } );
}

unsigned int
Analyze::ByCharacter::RecordOfSelfCharacter::GetTotalLose( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.GetTotalLose(); } );
}

unsigned int
Analyze::ByCharacter::RecordOfSelfCharacter::GetTotalDraw( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.GetTotalDraw(); } );
}

unsigned int
Analyze::ByCharacter::RecordOfSelfCharacter::GetTotalUnknown( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.GetTotalUnknown(); } );
}


std::vector<const Analyze::ListEntry*>
Analyze::ByCharacter::RecordOfSelfCharacter::GetListEntryArray( void ) const
{
  return this->GetListEntryArrayFromHash( *this );
}


void
Analyze::ByCharacter::RecordOfSelfCharacter::SetDown( ReplayFile* replay, bool first_is_self )
{
  if ( NOT( this->contains( replay->GetOpponentCharacter( first_is_self ) ) ) ) {
    std::wstring description = std::format(
      L"{} ({}) vs {}",
      replay->GetSelfName( first_is_self ), replay->GetSelfCharacter( first_is_self ).ToFullName(),
      replay->GetOpponentCharacter( first_is_self ).ToFullName() );

    this->emplace( std::piecewise_construct,
                   std::forward_as_tuple( replay->GetOpponentCharacter( first_is_self ) ),
                   std::forward_as_tuple( replay->GetOpponentCharacter( first_is_self ), description ) );
  }
  this->at( replay->GetOpponentCharacter( first_is_self ) ).SetDown( replay, first_is_self );
}


// -- ByCharacter::Record -----
std::optional<int>
Analyze::ByCharacter::Record::Compare( const ListEntry& other ) const
{
  if ( other.Is<Record>() ) {
    throw GGXXACPR_REPLAY_ORGANIZER_INTERNAL_EXCEPTION;
  }
  if ( other.Is<RecordOfSelfCharacter>() ) {
    return -1;
  }
  return std::nullopt;
}


std::wstring
Analyze::ByCharacter::Record::GetName( void ) const
{
  return L"ALL";
}

unsigned int
Analyze::ByCharacter::Record::GetTotalWin( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.GetTotalWin(); } );
}

unsigned int
Analyze::ByCharacter::Record::GetTotalLose( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.GetTotalLose(); } );
}

unsigned int
Analyze::ByCharacter::Record::GetTotalDraw( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.GetTotalDraw(); } );
}

unsigned int
Analyze::ByCharacter::Record::GetTotalUnknown( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.GetTotalUnknown(); } );
}


std::vector<const Analyze::ListEntry*>
Analyze::ByCharacter::Record::GetListEntryArray( void ) const
{
  return this->GetListEntryArrayFromHash( *this );
}


void
Analyze::ByCharacter::Record::SetDown( ReplayFile* replay, bool first_is_self )
{
  if ( NOT( this->contains( replay->GetSelfCharacter( first_is_self ) ) ) ) {
    this->emplace( replay->GetSelfCharacter( first_is_self ), replay->GetSelfCharacter( first_is_self ) );
  }
  this->at( replay->GetSelfCharacter( first_is_self ) ).SetDown( replay, first_is_self );
}


// -- ByAllCharacter::RecordOfOpponentCharacter -----
void
Analyze::ByAllCharacter::RecordOfOpponentCharacter::SetDown( ReplayFile* replay, bool first_is_self )
{
  if ( NOT( this->contains( replay->GetOpponentNameSymbol( first_is_self ) ) ) ) {
    std::wstring description = std::format(
      L"{} ({}) vs {} ({})",
      replay->GetSelfName( first_is_self ), StrT::Analyze::AllCharacter.Get(),
      replay->GetOpponentName( first_is_self ), replay->GetOpponentCharacter( first_is_self ).ToFullName() );

    this->emplace( std::piecewise_construct,
                   std::forward_as_tuple( replay->GetOpponentNameSymbol( first_is_self ) ),
                   std::forward_as_tuple( replay->GetOpponentName( first_is_self ), description ) );
  }
  this->at( replay->GetOpponentNameSymbol( first_is_self ) ).SetDown( replay, first_is_self );
}


// -- ByAllCharacter::Record -----
std::optional<int>
Analyze::ByAllCharacter::Record::Compare( const ListEntry& other ) const
{
  if ( other.Is<Record>() ) {
    throw GGXXACPR_REPLAY_ORGANIZER_INTERNAL_EXCEPTION;
  }
  if ( other.Is<RecordOfOpponentCharacter>() ) {
    return -1;
  }
  return std::nullopt;
}

std::wstring
Analyze::ByAllCharacter::Record::GetName( void ) const
{
  return L"ALL";
}

unsigned int
Analyze::ByAllCharacter::Record::GetTotalWin( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.GetTotalWin(); } );
}

unsigned int
Analyze::ByAllCharacter::Record::GetTotalLose( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.GetTotalLose(); } );
}

unsigned int
Analyze::ByAllCharacter::Record::GetTotalDraw( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.GetTotalDraw(); } );
}

unsigned int
Analyze::ByAllCharacter::Record::GetTotalUnknown( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.GetTotalUnknown(); } );
}


std::vector<const Analyze::ListEntry*>
Analyze::ByAllCharacter::Record::GetListEntryArray( void ) const
{
  return this->GetListEntryArrayFromHash( *this );
}


void
Analyze::ByAllCharacter::Record::SetDown( ReplayFile* replay, bool first_is_self )
{
  if ( NOT( this->contains( replay->GetOpponentCharacter( first_is_self ) ) ) ) {
    std::wstring description = std::format(
      L"{} ({}) vs {}",
      replay->GetSelfName( first_is_self ), StrT::Analyze::AllCharacter.Get(),
      replay->GetOpponentCharacter( first_is_self ).ToFullName() );

    this->emplace( std::piecewise_construct,
                   std::forward_as_tuple( replay->GetOpponentCharacter( first_is_self ) ),
                   std::forward_as_tuple( replay->GetOpponentCharacter( first_is_self ), description ) );
  }
  this->at( replay->GetOpponentCharacter( first_is_self ) ).SetDown( replay, first_is_self );
}


// -- ByOpponent::RecordOfCharacter -----
Analyze::ByOpponent::RecordOfCharacter::RecordOfCharacter( Character self, Character opponent, const std::wstring& description_for_record_base ) :
self_( self ),
opponent_( opponent ),
description_for_record_base_( description_for_record_base )
{
}

std::optional<int>
Analyze::ByOpponent::RecordOfCharacter::Compare( const ListEntry& other ) const
{
  if ( other.Is<RecordOfCharacter>() ) {
    if ( self_ == other.CastTo<RecordOfCharacter>().self_ ) {
      return opponent_.ToInteger() - other.CastTo<RecordOfCharacter>().opponent_.ToInteger();
    }
    return self_.ToInteger() - other.CastTo<RecordOfCharacter>().self_.ToInteger();
  }
  return std::nullopt;
}


std::wstring
Analyze::ByOpponent::RecordOfCharacter::GetName( void ) const
{
  return self_.ToFullName() + L" vs " + opponent_.ToFullName();
}

std::wstring
Analyze::ByOpponent::RecordOfCharacter::GetDescriptionForRecordBaseDialog( void ) const
{
  return description_for_record_base_;
}


// -- ByOpponent::RecordOfOpponent -----
Analyze::ByOpponent::RecordOfOpponent::RecordOfOpponent( const std::wstring& opponent_name, const std::wstring& self_name ) :
std::unordered_map<std::pair<Character, Character>, RecordOfCharacter, CharacterHash>(),
opponent_name_( opponent_name ),
self_name_( self_name )
{
}

std::wstring
Analyze::ByOpponent::RecordOfOpponent::GetDescriptionForSubDialog( void ) const
{
  return self_name_ + L" vs " + opponent_name_;
}


std::optional<int>
Analyze::ByOpponent::RecordOfOpponent::Compare( const ListEntry& other ) const
{
  if ( other.Is<RecordOfOpponent>() ) {
    return opponent_name_.compare( other.CastTo<RecordOfOpponent>().opponent_name_ );
  }
  if ( other.Is<RecordOfCharacter>() ) {
    return -1;
  }
  return std::nullopt;
}


std::wstring
Analyze::ByOpponent::RecordOfOpponent::GetName( void ) const
{
  return opponent_name_;
}

unsigned int
Analyze::ByOpponent::RecordOfOpponent::GetTotalWin( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.win_; } );
}

unsigned int
Analyze::ByOpponent::RecordOfOpponent::GetTotalLose( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.lose_; } );
}

unsigned int
Analyze::ByOpponent::RecordOfOpponent::GetTotalDraw( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.draw_; } );
}

unsigned int
Analyze::ByOpponent::RecordOfOpponent::GetTotalUnknown( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.unknown_; } );
}


std::vector<const Analyze::ListEntry*>
Analyze::ByOpponent::RecordOfOpponent::GetListEntryArray( void ) const
{
  return this->GetListEntryArrayFromHash( *this );
}


void
Analyze::ByOpponent::RecordOfOpponent::SetDown( ReplayFile* replay, bool first_is_self )
{
  if ( NOT( this->contains( {replay->GetSelfCharacter( first_is_self ), replay->GetOpponentCharacter( first_is_self )} ) ) ) {
    std::wstring description = std::format(
      L"{} ({}) vs {} ({})",
      replay->GetSelfName( first_is_self ),     replay->GetSelfCharacter( first_is_self ).ToFullName(),
      replay->GetOpponentName( first_is_self ), replay->GetOpponentCharacter( first_is_self ).ToFullName() );

    this->emplace( std::piecewise_construct,
                   std::forward_as_tuple( replay->GetSelfCharacter( first_is_self ), replay->GetOpponentCharacter( first_is_self ) ),
                   std::forward_as_tuple( replay->GetSelfCharacter( first_is_self ), replay->GetOpponentCharacter( first_is_self ), description ) );
  }
  this->at( {replay->GetSelfCharacter( first_is_self ), replay->GetOpponentCharacter( first_is_self )} ).SetDown( replay, first_is_self );
}

// -- ByOpponent::Record -----
std::optional<int>
Analyze::ByOpponent::Record::Compare( const ListEntry& other ) const
{
  if ( other.Is<Record>() ) {
    throw GGXXACPR_REPLAY_ORGANIZER_INTERNAL_EXCEPTION;
  }
  if ( other.Is<RecordOfOpponent>() ) {
    return -1;
  }
  return std::nullopt;
}


std::wstring
Analyze::ByOpponent::Record::GetName( void ) const
{
  return L"ALL";
}

unsigned int
Analyze::ByOpponent::Record::GetTotalWin( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.GetTotalWin(); } );
}

unsigned int
Analyze::ByOpponent::Record::GetTotalLose( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.GetTotalLose(); } );
}

unsigned int
Analyze::ByOpponent::Record::GetTotalDraw( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.GetTotalDraw(); } );
}

unsigned int
Analyze::ByOpponent::Record::GetTotalUnknown( void ) const
{
  return get_sum_of( *this, [] ( const auto& tmp ) { return tmp.GetTotalUnknown(); } );
}


std::vector<const Analyze::ListEntry*>
Analyze::ByOpponent::Record::GetListEntryArray( void ) const
{
  return this->GetListEntryArrayFromHash( *this );
}


void
Analyze::ByOpponent::Record::SetDown( ReplayFile* replay, bool first_is_self )
{
  if ( NOT( this->contains( replay->GetOpponentNameSymbol( first_is_self ) ) ) ) {
    this->emplace( std::piecewise_construct,
                   std::forward_as_tuple( replay->GetOpponentNameSymbol( first_is_self ) ),
                   std::forward_as_tuple( replay->GetOpponentName( first_is_self ), replay->GetSelfName( first_is_self ) ) );
  }
  this->at( replay->GetOpponentNameSymbol( first_is_self ) ).SetDown( replay, first_is_self );
}
