/*=========================================================================

 Program: MAF2
 Module: mafPipeCompoundVolumeVRTest
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

#include <cppunit/config/SourcePrefix.h>
#include "mafPipeCompoundVolumeFixedScalars.h"
#include "mafPipeCompoundVolumeVRTest.h"
#include "mafSceneNode.h"
#include "mafGUIDynamicVP.h"

#include "mafVMEVolumeGray.h"


#include <iostream>

class DummyPipeCompoundVolumeVR : public mafPipeCompoundVolumeVR
{
public:
  mafTypeMacro(DummyPipeCompoundVolumeVR,mafPipeCompoundVolumeVR);
  DummyPipeCompoundVolumeVR(){}
  void Create(mafSceneNode *n);
protected:
  // redefinition for avoiding gui components
  mafGUI *CreateGui() {return NULL;}
  void    UpdateGUILayout(bool bForce){}
  void    CreatePageGroups(){}
};

//----------------------------------------------------------------------------
void DummyPipeCompoundVolumeVR::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  //remove wxCursor busy;

  mafPipe::Create(n);
  m_SceneNode = n; 

  //creates description of groups
  CreatePageGroups();

  //creates initial pages for each group
  CreatePages();

  //AddActor();
}

//----------------------------------------------------------------------------
mafCxxTypeMacro(DummyPipeCompoundVolumeVR);
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void mafPipeCompoundVolumeVRTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafPipeCompoundVolumeVRTest::TestAllocation()
//----------------------------------------------------------------------------
{
  DummyPipeCompoundVolumeVR *pipe = new DummyPipeCompoundVolumeVR;
  mafDEL(pipe);
}
//----------------------------------------------------------------------------
void mafPipeCompoundVolumeVRTest::TestCreateSceneNode()
//----------------------------------------------------------------------------
{
  mafVMEVolumeGray *volume;
  mafNEW(volume);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volume, NULL);

  /////////// Pipe Instance and Creation ///////////
  DummyPipeCompoundVolumeVR *pipe = new DummyPipeCompoundVolumeVR;
  pipe->Create(sceneNode);

  delete sceneNode;
  mafDEL(volume);
}

//----------------------------------------------------------------------------
void mafPipeCompoundVolumeVRTest::TestGetCurrentScalarVisualPipe()
//----------------------------------------------------------------------------
{
  mafVMEVolumeGray *volume;
  mafNEW(volume);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volume, NULL);

  /////////// Pipe Instance and Creation ///////////
  DummyPipeCompoundVolumeVR *pipe = new DummyPipeCompoundVolumeVR;
  pipe->Create(sceneNode);

  //smoke test
  CPPUNIT_ASSERT(pipe->GetCurrentScalarVisualPipe() == NULL);

  delete sceneNode;
  mafDEL(volume);

}