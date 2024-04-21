// utf16_window.h

#pragma once

#include "tt_window.h"
#include "tt_combo_box.h"
#include "tt_list_view.h"
#include "tt_tab.h"
#include "tt_form.h"
#include "tt_dialog.h"
#include "tt_string.h"
#include "tt_exception.h"

#include "utility.h"


// -- UTF16Window --------------------------------------------------------
template <class BASE>
class UTF16Window : public BASE {
public:
  virtual LPCWSTR GetClassNameUTF16( void ) {
    static std::wstring class_name = TtString::CP932ToUTF16( this->GetClassName() );
    return class_name.c_str();
  }

  virtual void CreatePrimitive( BASE::CreateParameter& parameter ) override {
    this->parent_ = parameter.parent_;
    this->instance_handle_ = parameter.instance_handle_;
    this->id_ = parameter.id_;

    DWORD style = ( this->GetStyle() |
                    ((parameter.parent_ != nullptr && parameter.is_child_) ? WS_CHILD : 0) |
                    (parameter.group_start_ ? WS_GROUP : 0) );

    this->handle_ = CreateWindowExW(
      this->GetExtendedStyle(),
      this->GetClassNameUTF16(),
      nullptr,
      style,
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
      parameter.parent_ ? parameter.parent_->GetHandle() : nullptr,
      reinterpret_cast<HMENU>( static_cast<int64_t>( parameter.id_ ) ),
      this->instance_handle_,
      parameter.lp_param_ );
    if ( this->handle_ == nullptr ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::CreateWindowEx ) );
    }

    this->SetFont( TtFont::DEFAULT );
    BASE::WINDOW_TABLE.Register( *this );

    if ( NOT( this->CreatedInternal() ) ) {
      ::DestroyWindow( this->handle_ );
      this->handle_ = nullptr;
    }
  }

  void SetTextUTF16( const std::wstring& str ) {
    if ( ::SetWindowTextW( this->handle_, str.c_str() ) == 0 ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SetWindowTextW ) );
    }
  }

  int GetTextLengthUTF16( void ) {
    return ::GetWindowTextLengthW( this->handle_ );
  }

  std::wstring GetTextUTF16( void ) {
    int length = this->GetTextLengthUTF16();
    if ( length == 0 ) {
      return std::wstring();
    }
    auto buf = std::make_unique<wchar_t[]>( length + 1 );
    if ( ::GetWindowTextW( this->handle_, buf.get(), length + 1 ) == 0 ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::GetWindowText ) );
    }
    return buf.get();
  }

  int SendMessageUTF16( UINT msg, WPARAM w_param, LPARAM l_param ) {
    return static_cast<int>( ::SendMessageW( this->handle_, msg, w_param, l_param ) );
  }
};


// -- UTF16ComboBox ------------------------------------------------------
class UTF16ComboBox : public UTF16Window<TtComboBox> {
public:
  std::wstring GetTextAtUTF16( unsigned int index ) {
    auto buf = std::make_unique<wchar_t[]>( this->GetTextLengthAt( index ) + 1 );
    this->SendMessageUTF16( CB_GETLBTEXT, index, reinterpret_cast<LPARAM>( buf.get() ) );
    return buf.get();
  }

  int PushUTF16( const std::wstring& str ) {
    return this->SendMessageUTF16( CB_ADDSTRING, 0, reinterpret_cast<LPARAM>( str.c_str() ) );
  }

  std::wstring GetSelectedStringUTF16( void ) {
    return this->GetTextAtUTF16( this->GetSelectedIndex() );
  }
};

using LanguageComboBox = TtWindowWithStyle<UTF16ComboBox, CBS_DROPDOWNLIST>;
using DateTemplateComboBox = TtWindowWithStyle<UTF16ComboBox, CBS_DROPDOWN>;
using AnalyzeComboBox = TtWindowWithStyle<UTF16ComboBox, WS_VSCROLL | CBS_DROPDOWNLIST>;


// -- UTF16ListView ------------------------------------------------------
class UTF16ListView : public UTF16Window<TtWindowWithStyle<TtListViewReport, LVS_SINGLESEL | LVS_SHOWSELALWAYS>> {
public:
  // -- Column -----------------------------------------------------------
  class Column : public TtListViewColumn {
  public:
    explicit Column( const TtListViewColumn& other ) : TtListViewColumn( other ) {};

    std::wstring GetTextUTF16( void ) const {
      LVCOLUMNW tmp = {LVCF_TEXT | LVCF_SUBITEM};
      tmp.iSubItem = this->GetIndex();
      const size_t buffer_size = 1024 * 8;
      wchar_t buf[buffer_size];
      tmp.pszText = const_cast<LPWSTR>( buf );
      tmp.cchTextMax = buffer_size;
      if ( NOT( ::SendMessageW( this->GetTreeView()->GetHandle(), LVM_GETCOLUMNW, this->GetIndex(), reinterpret_cast<LPARAM>( &tmp ) ) ) ) {
        throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( ListView_GetColumnW ) );
      }
      return tmp.pszText;
    }
    void SetTextUTF16( const std::wstring& text ) {
      LVCOLUMNW tmp = {LVCF_TEXT | LVCF_SUBITEM};
      tmp.iSubItem = this->GetIndex();
      tmp.pszText = const_cast<LPWSTR>( text.c_str() );
      tmp.cchTextMax = static_cast<int>( text.size() );
      if ( NOT( ::SendMessageW( this->GetTreeView()->GetHandle(), LVM_SETCOLUMNW, this->GetIndex(), reinterpret_cast<LPARAM>( &tmp ) ) ) ) {
        throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( ListView_SetColumnW ) );
      }
    }
  };

  // -- Item -------------------------------------------------------------
  class Item : public TtListViewItem {
  public:
    explicit Item( const TtListViewItem& other ) : TtListViewItem( other ) {};

    std::wstring GetTextUTF16( void ) const {
      return this->GetSubItemTextUTF16( 0 );
    }
    void SetTextUTF16( const std::wstring& text ) {
      this->SetSubItemTextUTF16( 0, text );
    }

    std::wstring GetSubItemTextUTF16( unsigned int index ) const {
      const size_t buffer_size = 1024 * 8;
      wchar_t buf[buffer_size];
      LVITEMW item;
      item.iSubItem = index;
      item.cchTextMax = buffer_size;
      item.pszText = buf;
      ::SendMessage( this->GetTreeView()->GetHandle(), LVM_GETITEMTEXTW, this->GetIndex(), reinterpret_cast<LPARAM>( &item ) );
      return buf;
    }
    void SetSubItemTextUTF16( unsigned int index, const std::wstring& text ) {
      LVITEMW item;
      item.iSubItem = index;
      item.pszText = const_cast<LPWSTR>( text.c_str() );
      ::SendMessage( this->GetTreeView()->GetHandle(), LVM_SETITEMTEXTW, this->GetIndex(), reinterpret_cast<LPARAM>( &item ) );
    }
  };
};

using OpponentList = UTF16ListView;


// -- UTF16Tab -----------------------------------------------------------
/* 使わなくなったので封印

template <>
TtPanel&
TtTab::Insert<TtPanel, TtPanal*>( int index, const std::string&, TtPanel* panel )
{
  this->RegisterPanel( index, panel );
  return *panel;
}

class UTF16Tab : public UTF16Window<TtTab> {
public:
  template <class TYPE = TtPanel, class... Args>
  TYPE& AppendUTF16( const std::wstring& title, Args... args ) {
    int index = this->GetCount();
    this->InsertNewTabUTF16( index, title );
    TYPE* panel = new TYPE( args... );
    // this->RegisterPanel( index, panel );
    // 本来は上記を呼ぶが private なので裏技を使い呼び出す
    this->Insert<TtPanel, TtPanel*>( index, std::string(), static_cast<TtPanel*>( panel ) );
    return *panel;
  }

private:
  void InsertNewTabUTF16( int index, const std::wstring& title ) {
    TCITEMW item;
    item.mask = TCIF_TEXT;
    item.pszText = const_cast<wchar_t*>( title.c_str() );
    int ret = this->SendMessageUTF16( TCM_INSERTITEMW, (WPARAM)(int)index, (LPARAM)(const TC_ITEMW *)(&item) );
    if( ret == -1 ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TabCtrl_InsertItem ) );
    }
  }
};
 */

// -- UTF16Form ----------------------------------------------------------
class UTF16Form : public UTF16Window<TtForm> {
public:
  inline static const wchar_t* CLASS_NAME_UTF16 = L"UTF16TTFORM";

  static void Initialize( HINSTANCE h_instance ) {
    static bool initialized = false;
    if ( NOT( initialized ) ) {
      initialized = true;
      WNDCLASSW window_class;
      window_class.style         = CS_HREDRAW | CS_VREDRAW;
      window_class.lpfnWndProc   = UTF16Form::WindowProcedureForTTL;
      window_class.cbClsExtra    = 0;
      window_class.cbWndExtra    = 0;
      window_class.hInstance     = h_instance;
      window_class.hIcon         = nullptr;
      window_class.hCursor       = ::LoadCursor( nullptr, IDC_ARROW );
      window_class.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
      window_class.lpszMenuName  = nullptr;
      window_class.lpszClassName = UTF16Form::CLASS_NAME_UTF16;
      UTF16Form::RegisterClassUTF16( &window_class );
    }
  }

  static LRESULT CALLBACK WindowProcedureForTTL( HWND handle, UINT msg, WPARAM w_param, LPARAM l_param ) {
    TtWindow::WMResult ret = TtWindow::FindAndCallWindowProcedure( handle, msg, w_param, l_param );
    return ( ret.result_type == TtWindow::WMResult::Done ) ? ret.result : ::DefWindowProcW( handle, msg, w_param, l_param );
  }

  static void RegisterClassUTF16( WNDCLASSW* window_class ) {
    if ( ::RegisterClassW( window_class ) == 0 ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::RegisterClassW ) );
    }
  }

public:
  explicit UTF16Form( void ) {
    UTF16Form::Initialize( ::GetModuleHandle( nullptr ) );
  }

  virtual LPCWSTR GetClassNameUTF16( void ) override {
    return CLASS_NAME_UTF16;
  }
};


// -- UTF16Dialog --------------------------------------------------------
class UTF16Dialog : public UTF16Window<TtDialog> {
public:
  virtual void CreatePrimitive( CreateParameter& parameter ) override {
    this->TtDialog::CreatePrimitive( parameter );
  }

  virtual int ShowDialog( TtWindow& parent ) override {
    parent_ = &parent;
    instance_handle_ = parent.GetInstanceHandle();
    id_ = 0;

    DLGTEMPLATE& tmp = *(static_cast<DLGTEMPLATE*>( template_object_ ) );
    tmp.style = this->GetStyle();
    tmp.dwExtendedStyle = this->GetExtendedStyle();
    tmp.cdit = 0;
    tmp.x = 100;
    tmp.y = 100;
    tmp.cx = 100;
    tmp.cy = 100;
    INT_PTR ret = ::DialogBoxIndirectParamW(
      instance_handle_, &tmp, parent.GetHandle(),
      TtDialog::WindowProcedureForTTLDialog, reinterpret_cast<LPARAM>( this ) );
    if ( ret == -1 ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::DialogBoxIndirectParam ) );
    }
    if ( handle_ != nullptr ) {
      TtWindow::WINDOW_TABLE.Unregister( *this );
      handle_ = nullptr;
    }
    return static_cast<int>( ret );
  }
};


// -- UTF16FolderBrowserDialog ----------------------------------------------
class UTF16FolderBrowserDialog {
private:
  static int CALLBACK Callback( HWND handle, UINT msg, LPARAM, LPARAM lpData ) {
    if ( msg == BFFM_INITIALIZED ) {
      TtExtraordinarilyWindow box( handle );
      UTF16FolderBrowserDialog* dialog = reinterpret_cast<UTF16FolderBrowserDialog*>( lpData );
      ::SendMessageW( handle, BFFM_SETSELECTIONW, 1, reinterpret_cast<LPARAM>( dialog->selected_path_.c_str() ) );
      box.SetCenterRelativeTo( *dialog->parent_ );
    }
    return 0;
  }

public:
  bool ShowDialog( TtWindow& parent ) {
    parent_ = &parent;

    BROWSEINFOW bi = {0};
    bi.hwndOwner = parent.GetHandle();
    bi.pidlRoot = nullptr;
    bi.pszDisplayName = nullptr;
    bi.lpszTitle = description_.c_str();
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_NEWDIALOGSTYLE;
    bi.lpfn = UTF16FolderBrowserDialog::Callback;
    bi.lParam = reinterpret_cast<LPARAM>( this );

    auto item_id_list = ::SHBrowseForFolderW( &bi );
    if ( item_id_list == nullptr ) {
      return false;
    }

    wchar_t tmp[MAX_PATH + 16];
    if ( ::SHGetPathFromIDListW( item_id_list, tmp ) == FALSE ) {
      throw TT_WIN_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( ::SHGetPathFromIDListW ) );
    }
    selected_path_ = tmp;
    ::CoTaskMemFree( item_id_list );

    return true;
  }

public:
  TtWindow*    parent_;
  std::wstring selected_path_;
  std::wstring description_;
};
