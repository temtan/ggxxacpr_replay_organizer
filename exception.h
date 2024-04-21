// exception.h

#pragma once

#include "tt_exception.h"


namespace GGXXACPRReplayOrganizer {
  // -- Exception --------------------------------------------------------
  class Exception : public TtException {
  public:
    explicit Exception( void );
    virtual std::wstring GetMessage( void ) = 0;
  };

  // -- InternalException ------------------------------------------------
  class InternalException : public Exception {
  public:
    explicit InternalException( const char* file, unsigned int line );

    const char*  GetFile( void ) const;
    unsigned int GetLine( void ) const;

    virtual std::wstring GetMessage( void ) override;
    virtual std::string Dump( void ) const override;

  private:
    const char*  file_;
    unsigned int line_;
  };
#define GGXXACPR_REPLAY_ORGANIZER_INTERNAL_EXCEPTION GGXXACPRReplayOrganizer::InternalException( __FILE__, __LINE__ )

  // -- LanguageException ------------------------------------------------
  class LanguageException : public Exception {
  public:
    explicit LanguageException( const std::wstring& language );

    const std::wstring& GetLanguage( void );

    virtual std::wstring GetMessage( void ) override;
    virtual std::string Dump( void ) const override;

  private:
    const std::wstring language_;
  };

  // -- FileOperationException -------------------------------------------
  class FileOperationException : public Exception {
  public:
    explicit FileOperationException( const std::wstring& source, const std::wstring& destination, const std::wstring& operation );

    const std::wstring& GetSource( void );
    const std::wstring& GetDestination( void );
    const std::wstring& GetOperation( void );

    virtual std::wstring GetMessage( void ) override;
    virtual std::string Dump( void ) const override;

  private:
    std::wstring source_;
    std::wstring destination_;
    std::wstring operation_;
  };
}
