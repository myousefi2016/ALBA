/*=========================================================================

 Program: MAF2
 Module: mafPipeVolumeArbSliceTest
 Authors: Eleonora Mambrini
 
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
#include "mafPipeVolumeArbSliceTest.h"
#include "mafPipeVolumeArbSlice.h"

#include "mafSceneNode.h"
#include "mafVMEVolumeGray.h"
#include "mafGUILutPreset.h"
#include "mmaVolumeMaterial.h"

#include "vtkMAFAssembly.h"
#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkStructuredPointsReader.h"
#include "vtkCamera.h"
#include "vtkLookupTable.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"

#include <iostream>
#include <fstream>

enum PIPE_BOX_ACTORS
{
  PIPE_BOX_ACTOR,
  PIPE_BOX_ACTOR_WIRED,
  PIPE_BOX_ACTOR_OUTLINE_CORNER,
  PIPE_BOX_NUMBER_OF_ACTORS,
};

//----------------------------------------------------------------------------
void mafPipeVolumeArbSliceTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipeVolumeArbSliceTest::BeforeTest()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);

	m_Renderer->SetBackground(0.1, 0.1, 0.1);

	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(640, 480);
	m_RenderWindow->SetPosition(200, 0);

	m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);
}
//----------------------------------------------------------------------------
void mafPipeVolumeArbSliceTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}

enum ID_TEST
{
	ID_TEST_PIPEEXECUTION = 0,
	ID_TEST_PIPEEXECUTION_SLICEOPACITY = 10,
	ID_TEST_PIPEEXECUTION_LUTRANGE = 20,
	ID_TEST_PIPEEXECUTION_COLORLOOKUPTABLE = 30,
	ID_TEST_PIPEEXECUTION_TICKS = 40,
	ID_TEST_PIPEEXECUTION_SHOWSLIDER = 50,
	ID_TEST_PIPEEXECUTION_ARBITRARY = 60,
};

//----------------------------------------------------------------------------
void mafPipeVolumeArbSliceTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  ////// Create VME (import vtkData) ////////////////////
  vtkStructuredPointsReader *importer;
  vtkNEW(importer);
  mafString filename1=MAF_DATA_ROOT;
  filename1<<"/Test_PipeVolumeSlice/VolumeSP.vtk";
  importer->SetFileName(filename1.GetCStr());
  importer->Update();

  mafVMEVolumeGray *volumeInput;
  mafNEW(volumeInput);
  volumeInput->SetData((vtkImageData*)importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mmaVolumeMaterial *material;
  mafNEW(material);

  mafVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volumeInput, NULL);

  double zValue[3][3]={{4.0,4.0,0.0},{4.0,4.0,1.0},{4.0,4.0,2.0}};

  for (int direction = mafPipeVolumeArbSlice::SLICE_X ; direction<=mafPipeVolumeArbSlice::SLICE_Z;direction++)
  {
    /////////// Pipe Instance and Creation ///////////
    mafPipeVolumeArbSlice *pipeSlice = new mafPipeVolumeArbSlice;
    pipeSlice->InitializeSliceParameters(direction,zValue[0],true);
    pipeSlice->Create(sceneNode);

    ////////// ACTORS List ///////////////
    vtkPropCollection *actorList = vtkPropCollection::New();
    pipeSlice->GetAssemblyFront()->GetActors(actorList);

    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();
    while(actor)
    {   
      m_Renderer->AddActor(actor);
      m_RenderWindow->Render();

      actor = actorList->GetNextProp();
    }
    double x,y,z,vx,vy,vz;
    switch(direction)
    {
    case mafPipeVolumeArbSlice::SLICE_X:
      {
        //x=-1 ;y=0; z=0; vx=0; vy=0; vz=1;
        x=1 ;y=0; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case mafPipeVolumeArbSlice::SLICE_Y:
      {
        x=0; y=-1; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case mafPipeVolumeArbSlice::SLICE_Z:
      {
        //x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
        x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
      }
      break;
    }

    m_Renderer->GetActiveCamera()->ParallelProjectionOn();
    m_Renderer->GetActiveCamera()->SetFocalPoint(0,0,0);
    m_Renderer->GetActiveCamera()->SetPosition(x*100,y*100,z*100);
    m_Renderer->GetActiveCamera()->SetViewUp(vx,vy,vz);
    m_Renderer->GetActiveCamera()->SetClippingRange(0.1,1000);

    for(int i=0;i<3;i++)
    {
      //
      double normals[3];
      normals[0] = 0.0;
      normals[1] = 0.0;
      normals[2] = 0.0;
      pipeSlice->SetSlice(zValue[i], normals);
			
      m_Renderer->ResetCamera();

      char *strings="Slice";

      m_RenderWindow->Render();
      printf("\n Visualization: %s \n", strings);

			COMPARE_IMAGES("TestPipeExecution", 3 * direction + i);
    }

    m_Renderer->RemoveAllProps();
    vtkDEL(actorList);
		sceneNode->DeletePipe();
  }

  delete sceneNode;

  mafDEL(material);
  mafDEL(volumeInput);
  vtkDEL(importer);
}

//----------------------------------------------------------------------------
void mafPipeVolumeArbSliceTest::TestPipeExecution_SetSliceOpacity()
//----------------------------------------------------------------------------
{
  ////// Create VME (import vtkData) ////////////////////
  vtkStructuredPointsReader *importer;
  vtkNEW(importer);
  mafString filename1=MAF_DATA_ROOT;
  filename1<<"/Test_PipeVolumeSlice/VolumeSP.vtk";
  importer->SetFileName(filename1.GetCStr());
  importer->Update();

  mafVMEVolumeGray *volumeInput;
  mafNEW(volumeInput);
  volumeInput->SetData((vtkImageData*)importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mmaVolumeMaterial *material;
  mafNEW(material);

  mafVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volumeInput, NULL);

  double zValue[3][3]={{4.0,4.0,0.0},{4.0,4.0,1.0},{4.0,4.0,2.0}};

  for (int direction = mafPipeVolumeArbSlice::SLICE_X ; direction<=mafPipeVolumeArbSlice::SLICE_Z;direction++)
  {
    /////////// Pipe Instance and Creation ///////////
    mafPipeVolumeArbSlice *pipeSlice = new mafPipeVolumeArbSlice;
    pipeSlice->InitializeSliceParameters(direction,zValue[0],true);
    pipeSlice->Create(sceneNode);

    double opacity = 0.5;
    //opacity = rand()/RAND_MAX;

    pipeSlice->SetSliceOpacity(opacity);

    CPPUNIT_ASSERT(opacity == pipeSlice->GetSliceOpacity());

    ////////// ACTORS List ///////////////
    vtkPropCollection *actorList = vtkPropCollection::New();
    pipeSlice->GetAssemblyFront()->GetActors(actorList);

    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();
    while(actor)
    {   
      m_Renderer->AddActor(actor);
      m_RenderWindow->Render();

      actor = actorList->GetNextProp();
    }
    double x,y,z,vx,vy,vz;
    switch(direction)
    {
    case mafPipeVolumeArbSlice::SLICE_X:
      {
        //x=-1 ;y=0; z=0; vx=0; vy=0; vz=1;
        x=1 ;y=0; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case mafPipeVolumeArbSlice::SLICE_Y:
      {
        x=0; y=-1; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case mafPipeVolumeArbSlice::SLICE_Z:
      {
        //x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
        x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
      }
      break;
    }

    m_Renderer->GetActiveCamera()->ParallelProjectionOn();
    m_Renderer->GetActiveCamera()->SetFocalPoint(0,0,0);
    m_Renderer->GetActiveCamera()->SetPosition(x*100,y*100,z*100);
    m_Renderer->GetActiveCamera()->SetViewUp(vx,vy,vz);
    m_Renderer->GetActiveCamera()->SetClippingRange(0.1,1000);

    for(int i=0;i<3;i++)
    {
      //
      double normals[3];
      normals[0] = 0.0;
      normals[1] = 0.0;
      normals[2] = 0.0;
      pipeSlice->SetSlice(zValue[i], normals);


      m_Renderer->ResetCamera();

      char *strings="Slice";

      m_RenderWindow->Render();
      printf("\n Visualization: %s \n", strings);

			COMPARE_IMAGES("TestPipeExecution_SetSliceOpacity", ID_TEST_PIPEEXECUTION_SLICEOPACITY + 3 * direction + i);
    }

    m_Renderer->RemoveAllProps();
    vtkDEL(actorList);
		sceneNode->DeletePipe();
  }

  delete sceneNode;

  mafDEL(material);
  mafDEL(volumeInput);
  vtkDEL(importer);
}

//----------------------------------------------------------------------------
void mafPipeVolumeArbSliceTest::TestPipeExecution_SetLutRange()
//----------------------------------------------------------------------------
{
  ////// Create VME (import vtkData) ////////////////////
  vtkStructuredPointsReader *importer;
  vtkNEW(importer);
  mafString filename1=MAF_DATA_ROOT;
  filename1<<"/Test_PipeVolumeSlice/VolumeSP.vtk";
  importer->SetFileName(filename1.GetCStr());
  importer->Update();

  mafVMEVolumeGray *volumeInput;
  mafNEW(volumeInput);
  volumeInput->SetData((vtkImageData*)importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mmaVolumeMaterial *material;
  mafNEW(material);

  mafVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volumeInput, NULL);

  double zValue[3][3]={{4.0,4.0,0.0},{4.0,4.0,1.0},{4.0,4.0,2.0}};

  for (int direction = mafPipeVolumeArbSlice::SLICE_X ; direction<=mafPipeVolumeArbSlice::SLICE_Z;direction++)
  {
    /////////// Pipe Instance and Creation ///////////
    mafPipeVolumeArbSlice *pipeSlice = new mafPipeVolumeArbSlice;
    pipeSlice->InitializeSliceParameters(direction,zValue[0],true);
    pipeSlice->Create(sceneNode);

    double range[2], range_out[2];
    range[0] = 0.2;
    range[1] = 0.5;

    pipeSlice->SetLutRange(range[0], range[1]);
    pipeSlice->GetLutRange(range_out);

    CPPUNIT_ASSERT(range[0] == range_out[0] && range[1] == range_out[1]);

    ////////// ACTORS List ///////////////
    vtkPropCollection *actorList = vtkPropCollection::New();
    pipeSlice->GetAssemblyFront()->GetActors(actorList);

    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();
    while(actor)
    {   
      m_Renderer->AddActor(actor);
      m_RenderWindow->Render();

      actor = actorList->GetNextProp();
    }
    double x,y,z,vx,vy,vz;
    switch(direction)
    {
    case mafPipeVolumeArbSlice::SLICE_X:
      {
        //x=-1 ;y=0; z=0; vx=0; vy=0; vz=1;
        x=1 ;y=0; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case mafPipeVolumeArbSlice::SLICE_Y:
      {
        x=0; y=-1; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case mafPipeVolumeArbSlice::SLICE_Z:
      {
        //x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
        x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
      }
      break;
    }

    m_Renderer->GetActiveCamera()->ParallelProjectionOn();
    m_Renderer->GetActiveCamera()->SetFocalPoint(0,0,0);
    m_Renderer->GetActiveCamera()->SetPosition(x*100,y*100,z*100);
    m_Renderer->GetActiveCamera()->SetViewUp(vx,vy,vz);
    m_Renderer->GetActiveCamera()->SetClippingRange(0.1,1000);

    for(int i=0;i<3;i++)
    {
      //
      double normals[3];
      normals[0] = 0.0;
      normals[1] = 0.0;
      normals[2] = 0.0;
      pipeSlice->SetSlice(zValue[i], normals);

			m_Renderer->ResetCamera();

      char *strings="Slice";

      m_RenderWindow->Render();
      printf("\n Visualization: %s \n", strings);

			COMPARE_IMAGES("TestPipeExecution_SetLutRange", ID_TEST_PIPEEXECUTION_LUTRANGE + 3 * direction + i);
    }

    m_Renderer->RemoveAllProps();
    vtkDEL(actorList);
		sceneNode->DeletePipe();
  }

  delete sceneNode;

  mafDEL(material);
  mafDEL(volumeInput);
  vtkDEL(importer);
}

//----------------------------------------------------------------------------
void mafPipeVolumeArbSliceTest::TestPipeExecution_SetColorLookupTable()
{
  ////// Create VME (import vtkData) ////////////////////
  vtkStructuredPointsReader *importer;
  vtkNEW(importer);
  mafString filename1=MAF_DATA_ROOT;
  filename1<<"/Test_PipeVolumeSlice/VolumeSP.vtk";
  importer->SetFileName(filename1.GetCStr());
  importer->Update();

  mafVMEVolumeGray *volumeInput;
  mafNEW(volumeInput);
  volumeInput->SetData((vtkImageData*)importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mmaVolumeMaterial *material;
  mafNEW(material);

  mafVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volumeInput, NULL);

  double zValue[3][3]={{4.0,4.0,0.0},{4.0,4.0,1.0},{4.0,4.0,2.0}};

  for (int direction = mafPipeVolumeArbSlice::SLICE_X ; direction<=mafPipeVolumeArbSlice::SLICE_Z;direction++)
  {
    /////////// Pipe Instance and Creation ///////////
    mafPipeVolumeArbSlice *pipeSlice = new mafPipeVolumeArbSlice;
    pipeSlice->InitializeSliceParameters(direction,zValue[0],true);
    pipeSlice->Create(sceneNode);

    vtkLookupTable *lut = vtkLookupTable::New();
    lutPreset(6, lut);

    pipeSlice->SetColorLookupTable(lut);

    ////////// ACTORS List ///////////////
    vtkPropCollection *actorList = vtkPropCollection::New();
    pipeSlice->GetAssemblyFront()->GetActors(actorList);

    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();
    while(actor)
    {   
      m_Renderer->AddActor(actor);
      m_RenderWindow->Render();

      actor = actorList->GetNextProp();
    }
    double x,y,z,vx,vy,vz;
    switch(direction)
    {
    case mafPipeVolumeArbSlice::SLICE_X:
      {
        //x=-1 ;y=0; z=0; vx=0; vy=0; vz=1;
        x=1 ;y=0; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case mafPipeVolumeArbSlice::SLICE_Y:
      {
        x=0; y=-1; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case mafPipeVolumeArbSlice::SLICE_Z:
      {
        //x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
        x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
      }
      break;
    }

    m_Renderer->GetActiveCamera()->ParallelProjectionOn();
    m_Renderer->GetActiveCamera()->SetFocalPoint(0,0,0);
    m_Renderer->GetActiveCamera()->SetPosition(x*100,y*100,z*100);
    m_Renderer->GetActiveCamera()->SetViewUp(vx,vy,vz);
    m_Renderer->GetActiveCamera()->SetClippingRange(0.1,1000);

    for(int i=0;i<3;i++)
    {
      //
      double normals[3];
      normals[0] = 0.0;
      normals[1] = 0.0;
      normals[2] = 0.0;
      pipeSlice->SetSlice(zValue[i], normals);


      m_Renderer->ResetCamera();

      char *strings="Slice";

      m_RenderWindow->Render();
      printf("\n Visualization: %s \n", strings);

			COMPARE_IMAGES("TestPipeExecution_SetColorLookupTable", ID_TEST_PIPEEXECUTION_COLORLOOKUPTABLE + 3 * direction + i);
    }

    m_Renderer->RemoveAllProps();
    vtkDEL(actorList);
    vtkDEL(lut);
		sceneNode->DeletePipe();
  }

  delete sceneNode;
  mafDEL(material);
  mafDEL(volumeInput);
  vtkDEL(importer);
}

//----------------------------------------------------------------------------
void mafPipeVolumeArbSliceTest::TestPipeExecution_TicksOnOff()
//----------------------------------------------------------------------------
{
  ////// Create VME (import vtkData) ////////////////////
  vtkStructuredPointsReader *importer;
  vtkNEW(importer);
  mafString filename1=MAF_DATA_ROOT;
  filename1<<"/Test_PipeVolumeSlice/VolumeSP.vtk";
  importer->SetFileName(filename1.GetCStr());
  importer->Update();

  mafVMEVolumeGray *volumeInput;
  mafNEW(volumeInput);
  volumeInput->SetData((vtkImageData*)importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mmaVolumeMaterial *material;
  mafNEW(material);

  mafVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volumeInput, NULL);

  double zValue[3][3]={{4.0,4.0,0.0},{4.0,4.0,1.0},{4.0,4.0,2.0}};

  int direction = mafPipeVolumeArbSlice::SLICE_X;

  for (int showticks = 0; showticks<2; showticks++)
  {
    /////////// Pipe Instance and Creation ///////////
    mafPipeVolumeArbSlice *pipeSlice = new mafPipeVolumeArbSlice;
    pipeSlice->InitializeSliceParameters(direction,zValue[0],true);
    pipeSlice->Create(sceneNode);
    if(showticks)
      pipeSlice->ShowTICKsOn();
    else
      pipeSlice->ShowTICKsOff();
		
    ////////// ACTORS List ///////////////
    vtkPropCollection *actorList = vtkPropCollection::New();
    pipeSlice->GetAssemblyFront()->GetActors(actorList);

    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();
    while(actor)
    {   
      m_Renderer->AddActor(actor);
      m_RenderWindow->Render();

      actor = actorList->GetNextProp();
    }
    double x,y,z,vx,vy,vz;

    x=1 ;y=0; z=0; vx=0; vy=0; vz=1;

    m_Renderer->GetActiveCamera()->ParallelProjectionOn();
    m_Renderer->GetActiveCamera()->SetFocalPoint(0,0,0);
    m_Renderer->GetActiveCamera()->SetPosition(x*100,y*100,z*100);
    m_Renderer->GetActiveCamera()->SetViewUp(vx,vy,vz);
    m_Renderer->GetActiveCamera()->SetClippingRange(0.1,1000);

    for(int i=0;i<3;i++)
    {
      //
      double normals[3];
      normals[0] = i/10;
      normals[1] = 0.0;
      normals[2] = 0.0;
      pipeSlice->SetSlice(zValue[i], normals);


      m_Renderer->ResetCamera();

      char *strings="Slice";

      m_RenderWindow->Render();
      printf("\n Visualization: %s \n", strings);

			COMPARE_IMAGES("TestPipeExecution_TicksOnOff", ID_TEST_PIPEEXECUTION_TICKS + showticks * 3 + i);
    }

    m_Renderer->RemoveAllProps();
    vtkDEL(actorList);
		sceneNode->DeletePipe();
  }  

  delete sceneNode;

  mafDEL(material);
  mafDEL(volumeInput);
  vtkDEL(importer);
}

//----------------------------------------------------------------------------
void mafPipeVolumeArbSliceTest::TestPipeExecution_Arbitrary()
//----------------------------------------------------------------------------
{
	////// Create VME (import vtkData) ////////////////////
	vtkStructuredPointsReader *importer;
	vtkNEW(importer);
	mafString filename1 = MAF_DATA_ROOT;
	filename1 << "/Test_PipeVolumeSlice/VolumeSP.vtk";
	importer->SetFileName(filename1.GetCStr());
	importer->Update();

	mafVMEVolumeGray *volumeInput;
	mafNEW(volumeInput);
	volumeInput->SetData((vtkImageData*)importer->GetOutput(), 0.0);
	volumeInput->GetOutput()->GetVTKData()->Update();
	volumeInput->GetOutput()->Update();
	volumeInput->Update();

	mmaVolumeMaterial *material;
	mafNEW(material);

	mafVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

	//Assembly will be create when instancing mafSceneNode
	mafSceneNode *sceneNode;
	sceneNode = new mafSceneNode(NULL, NULL, volumeInput, NULL);

	double zValue[3] = { 4.0,4.0,0.0 };

	int direction = mafPipeVolumeArbSlice::SLICE_ARB;

	/////////// Pipe Instance and Creation ///////////
	mafPipeVolumeArbSlice *pipeSlice = new mafPipeVolumeArbSlice;
	pipeSlice->InitializeSliceParameters(direction, zValue, true);
	pipeSlice->Create(sceneNode);

	////////// ACTORS List ///////////////
	vtkPropCollection *actorList = vtkPropCollection::New();
	pipeSlice->GetAssemblyFront()->GetActors(actorList);

	actorList->InitTraversal();
	vtkProp *actor = actorList->GetNextProp();
	while (actor)
	{
		m_Renderer->AddActor(actor);
		m_RenderWindow->Render();

		actor = actorList->GetNextProp();
	}
	double x, y, z, vx, vy, vz;

	x = 1; y = 0; z = 0; vx = 0; vy = 0; vz = 1;

	m_Renderer->GetActiveCamera()->ParallelProjectionOn();
	m_Renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
	m_Renderer->GetActiveCamera()->SetPosition(x * 100, y * 100, z * 100);
	m_Renderer->GetActiveCamera()->SetViewUp(vx, vy, vz);
	m_Renderer->GetActiveCamera()->SetClippingRange(0.1, 1000);

	double normal[3] = { 1, -1, 1 };
	pipeSlice->SetSlice(zValue, normal);
	
	m_Renderer->ResetCamera();

	char *strings = "Slice";

	m_RenderWindow->Render();
	printf("\n Visualization: %s \n", strings);

	COMPARE_IMAGES("TestPipeExecution_Arbitrary", ID_TEST_PIPEEXECUTION_TICKS + 1);

	m_Renderer->RemoveAllProps();
	vtkDEL(actorList);
	sceneNode->DeletePipe();


	delete sceneNode;

	mafDEL(material);
	mafDEL(volumeInput);
	vtkDEL(importer);
}