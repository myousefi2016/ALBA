/*=========================================================================

 Program: MAF2
 Module: mafSceneNode
 Authors: Silvano Imboden, Gianluigi Crimi
 
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

#include "mafIndent.h"
#include "mafPipe.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mafVME.h"
#include "mafVME.h"
#include "mafMatrixPipe.h"
#include "vtkMAFAssembly.h"
#include "mafVMEIterator.h" 
#include "vtkRenderer.h"
#include "vtkLinearTransform.h"
#include "vtkCommand.h"

//----------------------------------------------------------------------------
mafSceneNode::mafSceneNode(mafSceneGraph *sg,mafSceneNode *parent, mafVME *vme, vtkRenderer *renderFront, vtkRenderer *renderBack, vtkRenderer *alwaysVisibleRender)
{
	m_Sg            = sg;
	m_Parent				= parent;
	m_Vme						= vme;
	m_RenFront			= renderFront;
	m_RenBack				= renderBack;
  m_AlwaysVisibleRenderer = alwaysVisibleRender;
  
  m_Next					= NULL;
  m_Pipe					= NULL;
  m_PipeCreatable = true;
  m_Mutex         = false;
  m_AssemblyFront = NULL;
  m_AssemblyBack  = NULL;
  m_AlwaysVisibleAssembly = NULL;
	m_VisibleChildren = 0;
	
	m_CurrentVisibility = false;

  assert(m_Vme);
  vtkLinearTransform *transform = NULL;

	assert(m_Vme->GetOutput());
  assert(m_Vme->GetOutput()->GetTransform());
  assert(m_Vme->GetOutput()->GetTransform()->GetVTKTransform());
  transform = m_Vme->GetOutput()->GetTransform()->GetVTKTransform();

	//The creation of the Assembly Front is required in tests enven if m_RenFront is NULL
	m_AssemblyFront = vtkMAFAssembly::New();
	m_AssemblyFront->SetVme(m_Vme);
	m_AssemblyFront->SetUserTransform(transform);

  if(m_RenFront != NULL)
	{
		m_AssemblyFront->SetVisibility(m_CurrentVisibility);

	  if (m_Parent) 
			m_Parent->m_AssemblyFront->AddPart(m_AssemblyFront);
	  else
			m_RenFront->AddActor(m_AssemblyFront);
  }

	if(m_RenBack != NULL)
	{
		m_AssemblyBack = vtkMAFAssembly::New();
    m_AssemblyBack->SetVme(m_Vme);
    m_AssemblyBack->SetUserTransform(transform);
		m_AssemblyBack->SetVisibility(m_CurrentVisibility);

    if(m_Parent) 
			m_Parent->m_AssemblyBack->AddPart(m_AssemblyBack);
		else
			m_RenBack->AddActor(m_AssemblyBack);
	}
	
	if(m_AlwaysVisibleRenderer != NULL)
	{
		m_AlwaysVisibleAssembly = vtkMAFAssembly::New();
		m_AlwaysVisibleAssembly->SetVme(m_Vme);
		m_AlwaysVisibleAssembly->SetUserTransform(transform);
		m_AlwaysVisibleAssembly->SetVisibility(m_CurrentVisibility);

		if(m_Parent) 
			m_Parent->m_AlwaysVisibleAssembly->AddPart(m_AlwaysVisibleAssembly);
		else
			m_AlwaysVisibleRenderer->AddActor(m_AlwaysVisibleAssembly);
	}
}
//----------------------------------------------------------------------------
mafSceneNode::~mafSceneNode()
{
	cppDEL(m_Pipe);
	
	if(m_RenFront != NULL) 
	{
    if(m_Parent) 
			m_Parent->m_AssemblyFront->RemovePart(m_AssemblyFront);
		else
			m_RenFront->RemoveActor(m_AssemblyFront);
		
		vtkDEL(m_AssemblyFront);
  }
	
	if(m_RenBack != NULL)
	{
    if(m_Parent) 
			m_Parent->m_AssemblyBack->RemovePart(m_AssemblyBack);
		else
			m_RenBack->RemoveActor(m_AssemblyBack);

    vtkDEL(m_AssemblyBack);  
	}

	if(m_AlwaysVisibleRenderer != NULL)
	{
		if(m_Parent)
			m_Parent->m_AlwaysVisibleAssembly->RemovePart(m_AlwaysVisibleAssembly);
		else
			m_AlwaysVisibleRenderer->RemoveActor(m_AlwaysVisibleAssembly);
		
		vtkDEL(m_AlwaysVisibleAssembly);
	}
}
//----------------------------------------------------------------------------
void mafSceneNode::Select(bool select)   
{
  if(m_Pipe) m_Pipe->Select(select);
}
//----------------------------------------------------------------------------
void mafSceneNode::UpdateProperty(bool fromTag)
{
  if(m_Pipe) m_Pipe->UpdateProperty(fromTag);
}
//-------------------------------------------------------------------------
void mafSceneNode::Print(std::ostream& os, const int tabs)// const
{
  mafIndent indent(tabs);

  // print the scene node
  os << indent << "mafSceneNode" << '\t' << this << std::endl;
  os << indent << "Linked VME" << '\t' << m_Vme << '\t' << m_Vme->GetName() << std::endl;
  os << indent << "Visual Pipe" << '\t' << m_Pipe << '\t' <<  (m_Pipe ? m_Pipe->GetTypeName() : "")  << std::endl;  
  os << indent << "Front Renderer" << '\t' << m_RenFront << std::endl;
  os <<  indent << "Back Renderer" << '\t' << m_RenBack << std::endl;
  os <<  indent << "Always Visible Renderer" << '\t' << m_AlwaysVisibleRenderer << std::endl;
  os << std::endl;
}

//----------------------------------------------------------------------------
mafPipe * mafSceneNode::GetPipe() const
{
	return m_Pipe;
}

//----------------------------------------------------------------------------
void mafSceneNode::SetPipe(mafPipe * pipe)
{
	m_Pipe = pipe;
	UpdateVisibility();
}

//----------------------------------------------------------------------------
void mafSceneNode::DeletePipe()
{
	cppDEL(m_Pipe);
	UpdateVisibility();
}

//----------------------------------------------------------------------------
void mafSceneNode::ChildShow()
{
	m_VisibleChildren++;
	UpdateVisibility();
}

//----------------------------------------------------------------------------
void mafSceneNode::ChildHide()
{
	m_VisibleChildren--;
	UpdateVisibility();
}

//----------------------------------------------------------------------------
void mafSceneNode::UpdateVisibility()
{
	//Nodes are visible if has a pipe or at least a children is visible
	int visible = m_Pipe || m_VisibleChildren > 0;

	//updates visibility only on changes
	if (visible != m_CurrentVisibility)
	{
		if (m_Parent)
		{
			if (visible)
				m_Parent->ChildShow();
			else
				m_Parent->ChildHide();
		}

		if (m_AssemblyFront)
			m_AssemblyFront->SetVisibility(visible);
		if (m_AssemblyBack)
			m_AssemblyBack->SetVisibility(visible);
		if (m_AlwaysVisibleAssembly)
			m_AlwaysVisibleAssembly->SetVisibility(visible);

		m_CurrentVisibility = visible;

		ModifyRootAssembly();
	}
}

//----------------------------------------------------------------------------
void mafSceneNode::ModifyRootAssembly()
{
	mafSceneNode *rootNode = this;

	while (rootNode->m_Parent)
		rootNode = rootNode->m_Parent;

	if (rootNode->m_AssemblyFront)
		rootNode->m_AssemblyFront->Modified();

	if (rootNode->m_AssemblyBack)
		rootNode->m_AssemblyBack->Modified();

	if (rootNode->m_AlwaysVisibleAssembly)
		rootNode->m_AlwaysVisibleAssembly->Modified();

}

