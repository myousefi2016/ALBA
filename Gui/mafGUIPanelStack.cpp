/*=========================================================================

 Program: MAF2
 Module: mafGUIPanelStack
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


#include "mafDecl.h"
#include "mafGUIPanelStack.h"
//----------------------------------------------------------------------------
// mafGUIPanelStack
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUIPanelStack,mafGUIPanel)
    EVT_SIZE(mafGUIPanelStack::OnSize)
END_EVENT_TABLE()
//---------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafGUIPanelStack::mafGUIPanelStack(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
               const wxSize& size, long style, const wxString& name)
:mafGUIPanel(parent,id,pos,size,style,name)         
//----------------------------------------------------------------------------
{
  m_Sizer =  new wxBoxSizer( wxVERTICAL );

  m_CurrentPanel= new mafGUIPanel(this,-1); 
  m_NextPanel = NULL;

  Push(new mafGUIPanel(this,-1));

  this->SetAutoLayout( true );
  this->SetSizer( m_Sizer );
  m_Sizer->Fit(this);
  m_Sizer->SetSizeHints(this);
}
//----------------------------------------------------------------------------
mafGUIPanelStack::~mafGUIPanelStack( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mafGUIPanelStack::Put(mafGUIPanel* p)
//----------------------------------------------------------------------------
{
   Push(p);
   return true;
}
//----------------------------------------------------------------------------
bool mafGUIPanelStack::Remove(mafGUIPanel* p)
//----------------------------------------------------------------------------
{
  assert(p);
  assert(p==m_CurrentPanel);
  Pop();
  return true;
}
//----------------------------------------------------------------------------
void mafGUIPanelStack::Push(mafGUIPanel* p)
//----------------------------------------------------------------------------
{
  if(m_CurrentPanel == p ) return;

  assert(p);
  assert(m_CurrentPanel);
  m_CurrentPanel->Reparent(mafGetFrame());
  m_CurrentPanel->Show(false);
  m_Sizer->Detach(m_CurrentPanel);

  p->m_NextPanel = m_CurrentPanel;
  m_CurrentPanel= p;

  m_CurrentPanel->Show(true);
  m_CurrentPanel->Reparent(this);
  m_Sizer->Add(m_CurrentPanel,1,wxEXPAND);
   
  DoLayout();
}
//----------------------------------------------------------------------------
void mafGUIPanelStack::Pop()
//----------------------------------------------------------------------------
{
  assert(m_CurrentPanel);

  m_CurrentPanel->Show(false);
  m_CurrentPanel->Reparent(mafGetFrame());
  m_Sizer->Detach(m_CurrentPanel);

  m_CurrentPanel= m_CurrentPanel->m_NextPanel;
  assert(m_CurrentPanel);

  m_CurrentPanel->Show(true);
  m_CurrentPanel->Reparent(this);
  m_Sizer->Add(m_CurrentPanel,1,wxEXPAND);

  DoLayout();
}
//----------------------------------------------------------------------------
void mafGUIPanelStack::OnSize(wxSizeEvent& WXUNUSED(event))
//----------------------------------------------------------------------------
{
  DoLayout();
}
//----------------------------------------------------------------------------
void mafGUIPanelStack::DoLayout()
//----------------------------------------------------------------------------
{
  wxSizeEvent e;
	wxPanel::OnSize(e);
}
