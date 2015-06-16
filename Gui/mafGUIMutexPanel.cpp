/*=========================================================================

 Program: MAF2
 Module: mafGUIMutexPanel
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafGUIMutexPanel.h"
#include <wx/list.h>

//----------------------------------------------------------------------------
// mafGUIMutexPanel
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUIMutexPanel,mafGUIPanel)
  //EVT_BUTTON (ID_EPANEL, wxEPanel::OnButton)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mafGUIMutexPanel::mafGUIMutexPanel(wxWindow* parent,wxWindowID id)
:mafGUIPanel(parent,id)
//----------------------------------------------------------------------------
{
  m_Sizer =  new wxBoxSizer( wxVERTICAL );
  this->SetAutoLayout( true );
  this->SetSizer( m_Sizer );
  //m_Sizer->Fit(this);
  //m_Sizer->SetSizeHints(this);
}
//----------------------------------------------------------------------------
mafGUIMutexPanel::~mafGUIMutexPanel( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mafGUIMutexPanel::Put(wxWindow *win)
//----------------------------------------------------------------------------
{
  if(win == GetCurrentClient())
    return false; //SIL. 22-4-2005: -- if passing the current content, nothing happen

  RemoveCurrentClient();
  if(win == NULL)
  return false; //SIL. 22-4-2005: -- if passing NULL, the old content is removed 
  
  //win->FitGui(); // solo in mafGUI
  win->Reparent(this);
  m_Sizer->Add(win,1,wxEXPAND);
  win->Show(true);
  win->Update();
  
  //m_Sizer->Fit(this);
  //m_Sizer->SetSizeHints(this);
  this->Layout();

  return true;
}
//----------------------------------------------------------------------------
bool mafGUIMutexPanel::RemoveCurrentClient()
//----------------------------------------------------------------------------
{
  wxWindow *current_gui = GetCurrentClient();
  if(current_gui == NULL) 
    return false;  //nothing to remove

  m_Sizer->Detach(current_gui);
  current_gui->Show(false);
  current_gui->Reparent(mafGetFrame());
  this->Layout();
  return true;
}
//----------------------------------------------------------------------------
wxWindow *mafGUIMutexPanel::GetCurrentClient()
//----------------------------------------------------------------------------
{
  int num_children = this->GetChildren().GetCount();
  assert(num_children <= 1);

  if (num_children == 1)
  {
    wxWindowList::Node *node = this->GetChildren().GetFirst();
    return (wxWindow *)node->GetData();
  }
  return NULL;
}
