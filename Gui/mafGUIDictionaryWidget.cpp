/*=========================================================================

 Program: MAF2
 Module: mafGUIDictionaryWidget
 Authors: Daniele Giunchi
 
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

#include "mafGUIDictionaryWidget.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/laywin.h>
#include <wx/listctrl.h>
#include <wx/dialog.h>

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafTagArray.h"

#include "mafGUIListCtrl.h"
#include "mafGUIDictionaryWidget.h"

#include "mafVME.h"
#include "mafVMERoot.h"
#include "mafVMELandmarkCloud.h"

#include "mafXMLStorage.h"
#include "mafStorageElement.h"



enum DICTIONARY_WIDGET_ID
{
	LOAD_DICT = MINID,
};
//----------------------------------------------------------------------------
mafGUIDictionaryWidget::mafGUIDictionaryWidget(wxWindow *parent, int id)
//----------------------------------------------------------------------------
{  
  m_Vme   = NULL;
  m_File = "";

  m_List = new mafGUIListCtrl(parent,id,false,true);
  m_List->Show(false);
  m_List->SetListener(this);
  m_List->SetSize(wxSize(100,100));

  if(m_File != "") 
		LoadDictionary(m_File);
}
//----------------------------------------------------------------------------
mafGUIDictionaryWidget::~mafGUIDictionaryWidget()
//----------------------------------------------------------------------------
{
	m_Items.clear();
	m_Selections.clear();
  cppDEL(m_List);
}
//----------------------------------------------------------------------------
void mafGUIDictionaryWidget::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if(mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
	  switch(e->GetId())
	  {
			case ITEM_SELECTED:
	      mafEventMacro(*e);
			break;
	  }
	}
}
//----------------------------------------------------------------------------
void mafGUIDictionaryWidget::LoadDictionary()
//----------------------------------------------------------------------------
{
  wxString wild_dict	= "Dictionary file (*.dic)|*.dic|All files (*.*)|*.*";
  wxString m_dict		  = mafGetApplicationDirectory().c_str();
  m_dict = m_dict + "\\Config\\Dictionary\\";
	wxString file       = mafGetOpenFile(m_dict,wild_dict,"Choose Dictionary File",m_List).c_str(); 
	if(file != "") LoadDictionary(file);
}
//----------------------------------------------------------------------------
void mafGUIDictionaryWidget::LoadDictionary(wxString file)
//----------------------------------------------------------------------------
{
  m_File = file;
  
  // XML storage to restore
  mafXMLStorage restore;
  restore.SetURL(m_File);
  restore.SetFileType("DIC");
  restore.SetVersion("2.0");

  // create a new object to restore into
  mafStorableDictionary *storeDict = new mafStorableDictionary;
  restore.SetDocument(storeDict);
  restore.Restore();
  
	m_Items.clear();
	m_Selections.clear();

  m_List->Reset();
  //m_List->SetColumnLabel(0, "names already in use are displayed with the red icon");

  for(int i=0; i<storeDict->m_StrVector.size(); i++)
  {
		wxString item = storeDict->m_StrVector[i];
		m_Items.push_back(item);
		m_Selections.push_back(false);
    m_List->AddItem(item);
  }
  
  storeDict->Delete();
  ValidateAllItem();
}
//----------------------------------------------------------------------------
void mafGUIDictionaryWidget::InitDictionary(std::vector<wxString> *strVect)
//----------------------------------------------------------------------------
{
	m_Items.clear();
	m_Selections.clear();
	m_List->Reset();

	if(strVect!=NULL)
	{ 
		for (int i = 0; i < strVect->size(); i++)
		{
			wxString item = (*strVect)[i];
			m_Items.push_back(item);
			m_Selections.push_back(false);
			m_List->AddItem(item);
		}
	}

	ValidateAllItem();
}

//----------------------------------------------------------------------------
void mafGUIDictionaryWidget::SetCloud(mafVME *vme)
//----------------------------------------------------------------------------
{
  m_Vme = vme;
	ValidateAllItem();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void mafGUIDictionaryWidget::AddItem(wxString item)
//----------------------------------------------------------------------------
{
	m_Items.push_back(item);
	m_Selections.push_back(false);
	m_List->AddItem(item);
	
	ValidateAllItem();
}
//----------------------------------------------------------------------------
int mafGUIDictionaryWidget::RemoveItem(mafString itemName)
//----------------------------------------------------------------------------
{
	for (int i = 0; i < m_Items.size(); i++)
	{
		if (m_Items[i] == itemName)
		{
			return RemoveItem(i);
		}
	}

	return -1;
}
//----------------------------------------------------------------------------
int mafGUIDictionaryWidget::RemoveItem(long itemId)
//----------------------------------------------------------------------------
{
	bool res = m_List->DeleteItem(itemId);

	if (res)
	{
		m_Items.erase(m_Items.begin() + itemId);
		m_Selections.erase(m_Selections.begin() + itemId);
		
		return itemId;
	}

	return -1;
}

//----------------------------------------------------------------------------
int mafGUIDictionaryWidget::GetItemIndex(mafString itemName)
//----------------------------------------------------------------------------
{
	for (int i = 0; i < m_Items.size(); i++)
	{
		if (m_Items[i] == itemName)
		{
			return i;
		}
	}

	return -1;
}

mafString mafGUIDictionaryWidget::GetItemByIndex(int index)
{
	if (index >= 0 < m_Items.size())
		return m_Items[index];
	else
		return "";
}

//----------------------------------------------------------------------------
int mafGUIDictionaryWidget::UpdateItem(mafString oldItemName, mafString newItemName)
//----------------------------------------------------------------------------
{
	for (int i = 0; i < m_Items.size(); i++)
	{
		if (m_Items[i] == oldItemName)
		{
			m_Items[i] = newItemName;
			m_List->SetItemLabel(i, newItemName.GetCStr());
			return i;
		}
	}

	return -1;
}
//----------------------------------------------------------------------------
int mafGUIDictionaryWidget::SelectItem(mafString itemName,bool select)
//----------------------------------------------------------------------------
{
	int retVal = -1;
	if(itemName!="")
	{
		for (int i = 0; i < m_Items.size(); i++)
		{
			m_List->DeselectItem(i);

			if (m_Items[i] == itemName)
			{
				m_List->SelectItem(i);
				m_Selections[i] = select;
				retVal=i;
			}
		}

		ValidateAllItem();
	}

	return retVal;
}
//----------------------------------------------------------------------------
int mafGUIDictionaryWidget::DeselectItem(mafString itemName)
//----------------------------------------------------------------------------
{
	return SelectItem(itemName, false);
}
//----------------------------------------------------------------------------
void mafGUIDictionaryWidget::ValidateAllItem(bool valid)
//----------------------------------------------------------------------------
{

	for (int i = 0; i < m_Items.size(); i++)
	{
		if (!m_Vme)
		{
			m_List->SetItemIcon(i, ITEM_GRAY);
		}
		else
		{
			
			mafVMELandmarkCloud* lc = (mafVMELandmarkCloud*)m_Vme;
			ITEM_ICONS icon = ITEM_GRAY;
			if (lc->FindInTreeByName(m_Items[i]))
			{
				if (m_Selections[i])
					icon = ITEM_BLUE;
				else
					icon = ITEM_RED;

			}
			m_List->SetItemIcon(i, icon);
		}
	}
}

// mafStorableDictionary ///////////////////////////////////////////////////////

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafStorableDictionary);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafStorableDictionary::mafStorableDictionary()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
mafStorableDictionary::~mafStorableDictionary()
//------------------------------------------------------------------------------
{
  m_StrVector.resize(0);
}
//------------------------------------------------------------------------------
// example of de-serialization code
int mafStorableDictionary::InternalRestore(mafStorageElement *element)
//------------------------------------------------------------------------------
{
  m_StrVector.resize(element->GetChildren()[0]->GetChildren().size());
  
  if(element->RestoreVectorN("Dictionary",m_StrVector,element->GetChildren()[0]->GetChildren().size(),"DItem"))
    return MAF_ERROR;
  return MAF_OK;
}
