/////////////////////////////////////////////////////////////////////////////
// Name:        MainFrame.cxx
// Purpose:     
// Author:      Federico Pinna
// Modified by: 
// Created:     05/02/04 23:19:32
// RCS-ID:      
// Copyright:   (c) 2004 Reitek S.p.A.
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "MainFrame.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "main.h"

////@begin XPM images
////@end XPM images

/*!
 * MainFrame type definition
 */

IMPLEMENT_CLASS( MainFrame, wxFrame )

/*!
 * MainFrame event table definition
 */

BEGIN_EVENT_TABLE( MainFrame, wxFrame )

////@begin MainFrame event table entries
    EVT_MENU( ID_MENU, MainFrame::OnConnect )

    EVT_MENU( ID_MENU1, MainFrame::OnDisconnect )

    EVT_MENU( ID_MENU2, MainFrame::OnQuit )

////@end MainFrame event table entries

END_EVENT_TABLE()

/*!
 * MainFrame constructors
 */

MainFrame::MainFrame( )
{
}

MainFrame::MainFrame( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create( parent, id, caption, pos, size, style );
}

/*!
 * MainFrame creator
 */

bool MainFrame::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin MainFrame member initialisation
    m_RosterTree = NULL;
////@end MainFrame member initialisation

////@begin MainFrame creation
    wxFrame::Create( parent, id, caption, pos, size, style );

    CreateControls();
    Centre();
////@end MainFrame creation
    return TRUE;
}

/*!
 * Control creation for MainFrame
 */

void MainFrame::CreateControls()
{    
////@begin MainFrame content construction

    MainFrame* item1 = this;

    wxStatusBar* item8 = new wxStatusBar( item1, ID_STATUSBAR, wxST_SIZEGRIP|wxNO_BORDER );
    item8->SetFieldsCount(2);
    item1->SetStatusBar(item8);

    wxPanel* item9 = new wxPanel( item1, ID_PANEL, wxDefaultPosition, wxSize(100, 80), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );

    wxGridSizer* item10 = new wxGridSizer(1, 1, 0, 0);
    item9->SetSizer(item10);
    item9->SetAutoLayout(TRUE);

    wxTreeCtrl* item11 = new wxTreeCtrl( item9, ID_TREECTRL, wxDefaultPosition, wxSize(100, 100), wxTR_HAS_BUTTONS |wxTR_HIDE_ROOT|wxTR_SINGLE );
    m_RosterTree = item11;
    item10->Add(item11, 0, wxGROW|wxGROW, 5);

    wxMenuBar* menuBar = new wxMenuBar;

    wxMenu* item3 = new wxMenu;
    item3->Append(ID_MENU, _("Connect"), _T(""), wxITEM_NORMAL);
    item3->Append(ID_MENU1, _("&Disconnect"), _T(""), wxITEM_NORMAL);
    item3->AppendSeparator();
    item3->Append(ID_MENU2, _("E&xit"), _T(""), wxITEM_NORMAL);
    menuBar->Append(item3, _("File"));
    item1->SetMenuBar(menuBar);

////@end MainFrame content construction
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU
 */

void MainFrame::OnConnect( wxCommandEvent& event )
{
    // Insert custom code here
    event.Skip();
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU1
 */

void MainFrame::OnDisconnect( wxCommandEvent& event )
{
    // Insert custom code here
    event.Skip();
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU2
 */

void MainFrame::OnQuit( wxCommandEvent& event )
{
    // Insert custom code here
    event.Skip();
}

/*!
 * Should we show tooltips?
 */

bool MainFrame::ShowToolTips()
{
    return TRUE;
}
