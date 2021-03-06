/*=========================================================================

 Program: MAF2
 Module: mafPipeCompoundVolumeIsosurfaceTest
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
#include "mafPipeCompoundVolumeIsosurfaceTest.h"
#include "mafSceneNode.h"
#include "mafGUIDynamicVP.h"

#include "mafVMEVolumeGray.h"


#include <iostream>

class DummyPipeCompoundVolumeIsosurface : public mafPipeCompoundVolumeIsosurface
{
public:
  mafTypeMacro(DummyPipeCompoundVolumeIsosurface,mafPipeCompoundVolumeIsosurface);
  DummyPipeCompoundVolumeIsosurface(){}
  void Create(mafSceneNode *n);
protected:
  // redefinition for avoiding gui components
  mafGUI *CreateGui() {return NULL;}
  void    UpdateGUILayout(bool bForce){}
  void    CreatePageGroups(){}
};

//----------------------------------------------------------------------------
void DummyPipeCompoundVolumeIsosurface::Create(mafSceneNode *n)
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
mafCxxTypeMacro(DummyPipeCompoundVolumeIsosurface);
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void mafPipeCompoundVolumeIsosurfaceTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafPipeCompoundVolumeIsosurfaceTest::TestAllocation()
//----------------------------------------------------------------------------
{
  DummyPipeCompoundVolumeIsosurface *pipe = new DummyPipeCompoundVolumeIsosurface;
  mafDEL(pipe);
}
//----------------------------------------------------------------------------
void mafPipeCompoundVolumeIsosurfaceTest::TestCreateSceneNode()
//----------------------------------------------------------------------------
{
  mafVMEVolumeGray *volume;
  mafNEW(volume);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volume, NULL);

  /////////// Pipe Instance and Creation ///////////
  DummyPipeCompoundVolumeIsosurface *pipe = new DummyPipeCompoundVolumeIsosurface;
  pipe->Create(sceneNode);

  delete sceneNode;
  mafDEL(volume);
}

//----------------------------------------------------------------------------
void mafPipeCompoundVolumeIsosurfaceTest::TestGetCurrentScalarVisualPipe()
//----------------------------------------------------------------------------
{
  mafVMEVolumeGray *volume;
  mafNEW(volume);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volume, NULL);

  /////////// Pipe Instance and Creation ///////////
  DummyPipeCompoundVolumeIsosurface *pipe = new DummyPipeCompoundVolumeIsosurface;
  pipe->Create(sceneNode);

  //smoke test
  CPPUNIT_ASSERT(pipe->GetCurrentScalarVisualPipe() == NULL);

  delete sceneNode;
  mafDEL(volume);

}