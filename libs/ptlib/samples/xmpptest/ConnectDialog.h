/////////////////////////////////////////////////////////////////////////////
// Name:        ConnectDialog.h
// Purpose:     
// Author:      Federico Pinna
// Modified by: 
// Created:     05/02/04 16:23:19
// RCS-ID:      
// Copyright:   (c) 2004 Reitek S.p.A.
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _CONNECTDIALOG_H_
#define _CONNECTDIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "ConnectDialog.cxx"
#endif

/*!
 * Includes
 */

////@begin includes
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG 10000
#define SYMBOL__CONNECTDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL__CONNECTDIALOG_TITLE _("Connect")
#define SYMBOL__CONNECTDIALOG_IDNAME ID_DIALOG
#define SYMBOL__CONNECTDIALOG_SIZE wxSize(400, 300)
#define SYMBOL__CONNECTDIALOG_POSITION wxDefaultPosition
#define ID_TEXTCTRL 10001
#define ID_TEXTCTRL1 10002
#define ID_CHECKBOX 10003
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * _ConnectDialog class declaration
 */

class _ConnectDialog: public wxDialog
{    
    DECLARE_CLASS( _ConnectDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    _ConnectDialog( );
    _ConnectDialog( wxWindow* parent, wxWindowID id = SYMBOL__CONNECTDIALOG_IDNAME, const wxString& caption = SYMBOL__CONNECTDIALOG_TITLE, const wxPoint& pos = SYMBOL__CONNECTDIALOG_POSITION, const wxSize& size = SYMBOL__CONNECTDIALOG_SIZE, long style = SYMBOL__CONNECTDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL__CONNECTDIALOG_IDNAME, const wxString& caption = SYMBOL__CONNECTDIALOG_TITLE, const wxPoint& pos = SYMBOL__CONNECTDIALOG_POSITION, const wxSize& size = SYMBOL__CONNECTDIALOG_SIZE, long style = SYMBOL__CONNECTDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin _ConnectDialog event handler declarations

////@end _ConnectDialog event handler declarations

////@begin _ConnectDialog member function declarations

////@end _ConnectDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin _ConnectDialog member variables
    wxTextCtrl* m_JID;
    wxTextCtrl* m_Pwd;
    wxCheckBox* m_RememberPwd;
////@end _ConnectDialog member variables
};

#endif
    // _CONNECTDIALOG_H_
