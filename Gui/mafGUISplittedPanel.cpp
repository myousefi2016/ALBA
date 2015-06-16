/*=========================================================================

 Program: MAF2
 Module: mafGUISplittedPanel
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


#include "mafGUISplittedPanel.h" 
//----------------------------------------------------------------------------
// costants :
//----------------------------------------------------------------------------
#define PAN_BOTTOM         200

//----------------------------------------------------------------------------
// mafGUISplittedPanel
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUISplittedPanel,mafGUIPanel)
    EVT_SIZE(mafGUISplittedPanel::OnSize)
    EVT_SASH_DRAGGED(PAN_BOTTOM, mafGUISplittedPanel::OnSashDrag)
END_EVENT_TABLE()
//----------------------------------------------------------------------------
mafGUISplittedPanel::mafGUISplittedPanel(wxWindow* parent, wxWindowID id, int size) 
:mafGUIPanel(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL | wxCLIP_CHILDREN) 
//----------------------------------------------------------------------------
{
  m_Bottom = new wxSashLayoutWindow(this, PAN_BOTTOM, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN);
  m_Bottom->SetDefaultSize(wxSize(1000, size));
  m_Bottom->SetOrientation(wxLAYOUT_HORIZONTAL);
  m_Bottom->SetAlignment(wxLAYOUT_BOTTOM);
  m_Bottom->SetSashVisible(wxSASH_TOP, true);
  m_Bottom->SetExtraBorderSize(0);

  m_BottomPanel    = NULL;
  m_TopPanel = NULL;
}
//----------------------------------------------------------------------------
mafGUISplittedPanel::~mafGUISplittedPanel( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUISplittedPanel::OnSize(wxSizeEvent& WXUNUSED(event))
//----------------------------------------------------------------------------
{
  DoLayout();
}
//----------------------------------------------------------------------------
void mafGUISplittedPanel::OnSashDrag(wxSashEvent& event)
//----------------------------------------------------------------------------
{
  if(event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE) 
    return;
  if(event.GetId()==PAN_BOTTOM) 
    m_Bottom->SetDefaultSize(wxSize(1000,event.GetDragRect().height));
  DoLayout();
}
//----------------------------------------------------------------------------
void mafGUISplittedPanel::DoLayout()
//----------------------------------------------------------------------------
{
  wxLayoutAlgorithm layout;
  layout.LayoutWindow(this,m_TopPanel);
}
//----------------------------------------------------------------------------
void mafGUISplittedPanel::PutOnTop(wxWindow *w)
//----------------------------------------------------------------------------
{
  if (m_TopPanel) m_BottomPanel->Show(false);
  w->Reparent(this);
  m_TopPanel = w;
  w->Show(true);
}
//----------------------------------------------------------------------------
void mafGUISplittedPanel::PutOnBottom(wxWindow *w)
//----------------------------------------------------------------------------
{
  if (m_BottomPanel) m_BottomPanel->Show(false);
  w->Reparent(m_Bottom);
  m_BottomPanel = w;
  w->Show(true);
}
