/*=========================================================================

 Program: MAF2
 Module: mafOpSelect
 Authors: Silvano Imboden, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafOpSelect_H__
#define __mafOpSelect_H__
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafOp.h"
#include "mafVMEIterator.h"
#include "mafSmartPointer.h" //for mafAutoPointer
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;

template class MAF_EXPORT mafAutoPointer<mafVME>;

/**
    class name: mafOpSelect
    Operation for the selection of a vme.
*/
class MAF_EXPORT mafOpSelect: public mafOp
{
public:
		mafTypeMacro(mafOpSelect, mafOp);

    /** Constructor. */
    mafOpSelect(wxString label=_("Select"));
    /** Destructor. */
   ~mafOpSelect(); 
    /** check if node can be input of the operation. */
    bool Accept(mafVME* vme);
    /** set input node to the operation. */
    void SetInput(mafVME* vme);
    /** retrieve the input node. */
    mafVME* GetInput(){return m_OldNodeSelected;};
    /** selection of another node. */
    void SetNewSel(mafVME* vme);
    /** retrieve new selected node. */
    mafVME* GetNewSel(){return m_NewNodeSelected;};
    /** execute the operation.  */
    void OpDo();
    /** undo the operation. */
    void OpUndo();
    /** return a instance of current object. */
    mafOp* Copy();
		/** runs operation. */
		void OpRun();
protected:
    mafAutoPointer<mafVME> m_OldNodeSelected;
    mafAutoPointer<mafVME> m_NewNodeSelected;
};
/**
    class name: mafOpEdit
    Interface operation for cut, copy, and paste operation.
*/
class MAF_EXPORT mafOpEdit: public mafOp
{
public:
    /** Constructor. */
    mafOpEdit(wxString label="");
    /** Destructor. */
    ~mafOpEdit(); 
    /** set input node to the operation. */
    void     SetInput(mafVME* vme) {m_Selection = vme;};
    /** check if the clipboard is empty.*/
    bool     ClipboardIsEmpty();
    /** clear the clipboard. */
		void     ClipboardClear();
    /** store clipboard for backup  */
		void     ClipboardBackup();
    /** restore clipboard from backup */
		void     ClipboardRestore();
    /** return the mafVME that is in the clipboard */
    mafVME* GetClipboard();
    /** set the clipboard */
    void SetClipboard(mafVME *node);
		/** set the parent of the selection */
		void SetSelectionParent(mafVME *parent);
		/** runs operation. */
		void OpRun();
protected:

	void RemoveBackLinksForTheSubTree(mafVME *vme);
	void RestoreBackLinksForTheSubTree(mafVME *vme);
	bool CanRestoreBackLinksForTheSubTree(mafVME *vme, mafVME *root);
  
	mafAutoPointer<mafVME> m_Backup;
  mafAutoPointer<mafVME> m_Selection;
};
/**
    class name: mafOpCut
    Operation which perform cut on a node input. (copying it into a clipboard)
*/
class MAF_EXPORT mafOpCut: public mafOpEdit
{
public:
  /** Constructor. */
  mafOpCut(wxString label=_("Cut"));
  /** Destructor. */
  ~mafOpCut();
  /** check if node can be input of the operation. */
  bool Accept(mafVME* vme);
  /** execute the operation.  */
  void OpDo();

	/** undo the operation. */
  void OpUndo();
  /** return a instance of current object. */
  mafOp* Copy(); 
  /** Load VTK data for the specified VME (Added by Losi on 03.06.2010) */
  void LoadVTKData(mafVME *vme);

protected: 
	/** Load all children in the tree (Added by Di Cosmo on 24.05.2012) */
	void LoadChild(mafVME *vme);
	mafAutoPointer<mafVME> m_SelectionParentBackup;
	bool m_Cutted;
};
/**
    class name: mafOpCopy
    Operation which perform copy of a node.
*/
class MAF_EXPORT mafOpCopy: public mafOpEdit
{
public:
    /** Constructor. */
    mafOpCopy(wxString label=_("Copy"));
    /** Destructor. */
    ~mafOpCopy();
    /** check if node can be input of the operation. */
    bool Accept(mafVME* vme);
    /** execute the operation.  */
    void OpDo();
		/** undo the operation. */
    void OpUndo();
    /** return a instance of current object. */
    mafOp* Copy();
};
/**
    class name: mafOpPaste
    Operation which perform paste of a node previously copied or cut.
*/
class MAF_EXPORT mafOpPaste: public mafOpEdit
{
public:
    /** Constructor. */
    mafOpPaste(wxString label=_("Paste"));
    /** check if node can be input of the operation. */
    bool Accept(mafVME* vme);
    /** execute the operation.  */    
    void OpDo();
    /** undo the operation. */
    void OpUndo();
    /** return a instance of current object. */
    mafOp* Copy(); 
protected:
    mafAutoPointer<mafVME> m_PastedVme;
		bool m_Pasted;
};

#endif
