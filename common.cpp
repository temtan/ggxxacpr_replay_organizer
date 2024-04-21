// common.cpp

#include <stdio.h>

#include "tt_enum.h"
#include "tt_ini_file.h"
#include "tt_time.h"

#include "common.h"

using namespace GGXXACPRReplayOrganizer;


// -- IniFileOperation ---------------------------------------------------
namespace Tag {
#define DEFINE_PARAMETER_NAME_STRING( name ) static const std::string name( #name )
  DEFINE_PARAMETER_NAME_STRING( GGXXACPRReplayOrganizer );

  DEFINE_PARAMETER_NAME_STRING( Placement );
  DEFINE_PARAMETER_NAME_STRING( ErrorLog );
  DEFINE_PARAMETER_NAME_STRING( Time );
  DEFINE_PARAMETER_NAME_STRING( Dump );
  DEFINE_PARAMETER_NAME_STRING( Language );
  DEFINE_PARAMETER_NAME_STRING( ReplayFolderPath );
  DEFINE_PARAMETER_NAME_STRING( DateTemplate );
  DEFINE_PARAMETER_NAME_STRING( SelfName );
  DEFINE_PARAMETER_NAME_STRING( OpponentLowerLimit );
  DEFINE_PARAMETER_NAME_STRING( OpponentLowerLimitNumber );
  DEFINE_PARAMETER_NAME_STRING( AnalyzeDialogPlacement );
}

void
IniFileOperation::SavePlacement( const WINDOWPLACEMENT& placement )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::GGXXACPRReplayOrganizer];
  section.SetString( Tag::Placement, TtUtility::Serialize( placement ) );
}

bool
IniFileOperation::LoadPlacement( WINDOWPLACEMENT& placement )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::GGXXACPRReplayOrganizer];
  auto keys = section.GetKeys();
  if ( std::find( keys.begin(), keys.end(), Tag::Placement ) == keys.end() ) {
    return false;
  }
  return TtUtility::Deserialize( section.GetString( Tag::Placement ), placement );
}


void
IniFileOperation::SaveLanguage( const std::string& language )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::GGXXACPRReplayOrganizer];
  section.SetString( Tag::Language, language );
}

std::string
IniFileOperation::LoadLanguage( void )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::GGXXACPRReplayOrganizer];
  return section.GetString( Tag::Language, "" );
}


void
IniFileOperation::SaveReplayFolderPath( const std::wstring& path )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::GGXXACPRReplayOrganizer];
  section.SetStringUTF16( Tag::ReplayFolderPath, path );
}

std::wstring
IniFileOperation::LoadReplayFolderPath( void )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::GGXXACPRReplayOrganizer];
  return section.GetStringUTF16( Tag::ReplayFolderPath, L"" );
}


void
IniFileOperation::SaveDateTtemplate( const std::wstring& date_template )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::GGXXACPRReplayOrganizer];
  section.SetStringUTF16( Tag::DateTemplate, date_template );
}

std::wstring
IniFileOperation::LoadDateTemplate( void )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::GGXXACPRReplayOrganizer];
  return section.GetStringUTF16( Tag::DateTemplate, L"" );
}


void
IniFileOperation::SaveSelfName( const std::wstring& self_name )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::GGXXACPRReplayOrganizer];
  section.SetStringUTF16( Tag::SelfName, self_name );
}

std::wstring
IniFileOperation::LoadSelfName( void )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::GGXXACPRReplayOrganizer];
  return section.GetStringUTF16( Tag::SelfName, L"" );
}


void
IniFileOperation::SaveOpponentLowerLimit( bool flag )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::GGXXACPRReplayOrganizer];
  section.SetBoolean( Tag::OpponentLowerLimit, flag );
}

bool
IniFileOperation::LoadOpponentLowerLimit( void )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::GGXXACPRReplayOrganizer];
  return section.GetBoolean( Tag::OpponentLowerLimit, true );
}


void
IniFileOperation::SaveOpponentLowerLimitNumber( int number )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::GGXXACPRReplayOrganizer];
  section.SetInteger( Tag::OpponentLowerLimitNumber, number );
}

int
IniFileOperation::LoadOpponentLowerLimitNumber( void )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::GGXXACPRReplayOrganizer];
  return section.GetInteger( Tag::OpponentLowerLimitNumber, 10 );
}


void
IniFileOperation::SaveErrorLogDump( TtException& e )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::ErrorLog];
  section.SetString( Tag::Time, TtTime::GetNow().GetDateTimeString() );
  section.SetString( Tag::Dump, e.Dump() );
}


void
IniFileOperation::SaveAnalyzeDialogPlacement( const WINDOWPLACEMENT& placement )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::GGXXACPRReplayOrganizer];
  section.SetString( Tag::AnalyzeDialogPlacement, TtUtility::Serialize( placement ) );
}

bool
IniFileOperation::LoadAnalyzeDialogPlacement( WINDOWPLACEMENT& placement )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::GGXXACPRReplayOrganizer];
  auto keys = section.GetKeys();
  if ( std::find( keys.begin(), keys.end(), Tag::AnalyzeDialogPlacement ) == keys.end() ) {
    return false;
  }
  return TtUtility::Deserialize( section.GetString( Tag::AnalyzeDialogPlacement ), placement );
}


// -- Image --------------------------------------------------------------
#include "resource.h"

std::vector<TtIcon>
Image::ICONS;

void
Image::Initialize( void )
{
  auto add = [] ( DWORD id ) {
    TtIcon icon = TtIcon::CreateFromResourceID( id );
    Image::ICONS.push_back( icon );
  };

  add( RSID_ICON_MAIN );
  add( RSID_ICON_MAIN_SMALL );
}
