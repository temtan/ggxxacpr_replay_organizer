// exception.cpp

#include "tt_string.h"

#include "string_table.h"
#include "utility.h"

#include "exception.h"

using namespace GGXXACPRReplayOrganizer;


// -- Exception --------------------------------------------------------
Exception::Exception( void ) :
TtException()
{
}

std::wstring
Exception::GetMessage( void )
{
  return std::format( L"Error" );
}


// -- InternalException ------------------------------------------------
InternalException::InternalException( const char* file, unsigned int line ) :
file_( file ),
line_( line )
{
}


const char*
InternalException::GetFile( void ) const
{
  return file_;
}

unsigned int
InternalException::GetLine( void ) const
{
  return line_;
}


std::wstring
InternalException::GetMessage( void )
{
  return Utility::Format( StrT::Message::InternalError.Get(), TtString::CP932ToUTF16( file_ ), line_ );
}


std::string
InternalException::Dump( void ) const
{
  return std::format( "{} : {}({})", typeid( *this ).name(), file_, line_ );
}


// -- LanguageException --------------------------------------------------
LanguageException::LanguageException( const std::wstring& language ) :
language_( language )
{
}

const std::wstring&
LanguageException::GetLanguage( void )
{
  return language_;
}

std::wstring
LanguageException::GetMessage( void )
{
  throw GGXXACPR_REPLAY_ORGANIZER_INTERNAL_EXCEPTION;
}

std::string
LanguageException::Dump( void ) const
{
  return std::format( "{} : {}", typeid( *this ).name(), TtString::UTF16ToCP932( language_ ) );
}


// -- FileOperationException -------------------------------------------
FileOperationException::FileOperationException( const std::wstring& source, const std::wstring& destination, const std::wstring& operation ) :
source_( source ),
destination_( destination ),
operation_( operation )
{
}

const std::wstring&
FileOperationException::GetSource( void )
{
  return source_;
}

const std::wstring&
FileOperationException::GetDestination( void )
{
  return destination_;
}

const std::wstring&
FileOperationException::GetOperation( void )
{
  return operation_;
}

std::wstring
FileOperationException::GetMessage( void )
{
  return Utility::Format( StrT::Message::FileOperationError.Get(), source_, destination_, operation_ );
}

std::string
FileOperationException::Dump( void ) const
{
  return std::format( "{} : {}, {}, {}", typeid( *this ).name(),
                      TtString::UTF16ToCP932( source_ ),
                      TtString::UTF16ToCP932( destination_ ),
                      TtString::UTF16ToCP932( operation_ ) );
}
