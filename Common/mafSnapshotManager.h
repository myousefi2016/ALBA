/*=========================================================================

 Program: MAF2
 Module:  mafSnapshotManager
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafSnapshotManager_H__
#define __mafSnapshotManager_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafObserver.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

class mafVMEGroup;
class mafGUIImageViewer;
class mafDeviceButtonsPadMouse;

/**
  Class Name: mafSnapshotManager.
*/
class MAF_EXPORT mafSnapshotManager : mafObserver
{
public:

  /** Default constructor */
	mafSnapshotManager();

  /** Default destructor */
  ~mafSnapshotManager();

	/** Event Management */
	virtual void OnEvent(mafEventBase *maf_event);
	
	/* Create snapshot from selected view*/
	void CreateSnapshot(mafVME *node, mafView *selectedView);

	/* Find or Create Snapshots group*/
	void FindOrCreateSnapshotGroup(mafVME *root);

	/* Show a Preview of Snapshots*/
	void ShowSnapshotPreview();

	/* Get Number of snapshots*/
	int GetNSnapshots();

	bool HasSnapshots(mafVME *root);

	/* Return the snapshots group*/
	mafVMEGroup* GetSnapshotGroup() { return m_SnapshotsGroup; };
	
	/* Set the snapshots group*/
	void SetSnapshotGroup(mafVMEGroup* group) { m_SnapshotsGroup = group; };

	/* Set Mouse for imageViewer interaction*/
	void SetMouse(mafDeviceButtonsPadMouse *mouse);

protected:

	void SelectImage();

	wxString			 			m_GroupName;
	mafVMEGroup					*m_SnapshotsGroup;
	mafGUIImageViewer		*m_ImageViewer;

private:
  friend class mafLogicWithManagers;
};
#endif
