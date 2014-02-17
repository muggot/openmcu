/////////////////////////////////////////////////////////////////////////////
// Name:        ConnectDialog.cxx
// Purpose:     
// Author:      Federico Pinna
// Modified by: 
// Created:     05/02/04 16:23:19
// RCS-ID:      
// Copyright:   (c) 2004 Reitek S.p.A.
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "ConnectDialog.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "main.h"

////@begin includes
////@end includes

#include "ConnectDialog.h"

////@begin XPM images
////@end XPM images

/*!
 * _ConnectDialog type definition
 */

IMPLEMENT_CLASS( _ConnectDialog, wxDialog )

/*!
 * _ConnectDialog event table definition
 */

BEGIN_EVENT_TABLE( _ConnectDialog, wxDialog )

////@begin _ConnectDialog event table entries
////@end _ConnectDialog event table entries

END_EVENT_TABLE()

/*!
 * _ConnectDialog constructors
 */

_ConnectDialog::_ConnectDialog( )
{
}

_ConnectDialog::_ConnectDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * ConnectDialog creator
 */

bool _ConnectDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin _ConnectDialog member initialisation
    m_JID = NULL;
    m_Pwd = NULL;
    m_RememberPwd = NULL;
////@end _ConnectDialog member initialisation

////@begin _ConnectDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end _ConnectDialog creation
    return TRUE;
}

/*!
 * Control creation for ConnectDialog
 */

void _ConnectDialog::CreateControls()
{    
////@begin _ConnectDialog content construction

    _ConnectDialog* item1 = this;

    wxBoxSizer* item2 = new wxBoxSizer(wxVERTICAL);
    item1->SetSizer(item2);
    item1->SetAutoLayout(TRUE);

    wxFlexGridSizer* item3 = new wxFlexGridSizer(2, 2, 0, 0);
    item2->Add(item3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* item4 = new wxStaticText( item1, wxID_STATIC, _("User ID"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add(item4, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* item5 = new wxTextCtrl( item1, ID_TEXTCTRL, _T(""), wxDefaultPosition, wxSize(150, -1), 0 );
    m_JID = item5;
    if (ShowToolTips())
        item5->SetToolTip(_("Enter your jabber user id"));
    item3->Add(item5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* item6 = new wxStaticText( item1, wxID_STATIC, _("Password"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add(item6, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* item7 = new wxTextCtrl( item1, ID_TEXTCTRL1, _T(""), wxDefaultPosition, wxSize(150, -1), wxTE_PASSWORD );
    m_Pwd = item7;
    if (ShowToolTips())
        item7->SetToolTip(_("Enter your password"));
    item3->Add(item7, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* item8 = new wxStaticText( item1, wxID_STATIC, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add(item8, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxCheckBox* item9 = new wxCheckBox( item1, ID_CHECKBOX, _("Remember password"), wxDefaultPosition, wxDefaultSize, 0 );
    m_RememberPwd = item9;
    item9->SetValue(FALSE);
    item9->SetHelpText(_("Check to save the password"));
    if (ShowToolTips())
        item9->SetToolTip(_("Check to save the password"));
    item3->Add(item9, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* item10 = new wxBoxSizer(wxHORIZONTAL);
    item2->Add(item10, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* item11 = new wxButton( item1, wxID_OK, _("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->SetDefault();
    item10->Add(item11, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* item12 = new wxButton( item1, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add(item12, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end _ConnectDialog content construction
}

/*!
 * Should we show tooltips?
 */

bool _ConnectDialog::ShowToolTips()
{
    return TRUE;
}
