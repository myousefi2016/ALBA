/*=========================================================================

 Program: MAF2
 Module: mafPipePolylineTest
 Authors: Matteo Giacomoni
 
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
#include "mafPipePolylineTest.h"

#include "mafSceneNode.h"
#include "mafVMERoot.h"
#include "mafPipePolyline.h"
#include "mafVMEPolyline.h"
#include "mmaMaterial.h"

#include "vtkMAFAssembly.h"
#include "vtkMAFSmartPointer.h"
#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"
#include "vtkPolyData.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"
#include "vtkCamera.h"

#include <iostream>

enum TESTS_PIPE_SURFACE
{
  TEST_COLOR,
  TEST_POLYLINE,
  TEST_SCALAR,
  TEST_POLYLINE_SPLINE_MODE,
  TEST_TUBE,
  TEST_TUBE_SPLINE_MODE,
  TEST_TUBE_CHANGE_VALUES,
  TEST_TUBE_CAPPING,
  TEST_GLYPH,
  TEST_GLYPH_CHANGE_VALUES,
  TEST_GLYPH_UNCONNECTED,
  TEST_GLYPH_SPLINE_MODE,
  NUMBER_OF_TEST,
};

//----------------------------------------------------------------------------
void mafPipePolylineTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipePolylineTest::BeforeTest()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);

	m_Renderer->SetBackground(0.1, 0.1, 0.1);

	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(640, 480);
	m_RenderWindow->SetPosition(400, 0);

	m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);
}
//----------------------------------------------------------------------------
void mafPipePolylineTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipePolylineTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
	////// Create VME (import vtkData) ////////////////////
	vtkMAFSmartPointer<vtkDataSetReader> importer;
	mafString filename = MAF_DATA_ROOT;
	filename << "/Test_PipePolyline/polyline.vtk";
	importer->SetFileName(filename);
	importer->Update();
	mafSmartPointer<mafVMEPolyline> polyline;
	polyline->SetData((vtkPolyData*)importer->GetOutput(), 0.0);
	polyline->GetOutput()->Update();
	polyline->GetMaterial();
	polyline->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
	polyline->Update();

	//Assembly will be create when instancing mafSceneNode
	mafSceneNode *sceneNode;
	sceneNode = new mafSceneNode(NULL, NULL, polyline, NULL);

	/////////// Pipe Instance and Creation ///////////
	mafPipePolyline *pipePolyline = new mafPipePolyline;
	pipePolyline->Create(sceneNode);

	////////// ACTORS List ///////////////
	vtkPropCollection *actorList = vtkPropCollection::New();

	for (int i = 0; i < NUMBER_OF_TEST; i++)
	{
		switch (i)
		{
		case TEST_COLOR:
		{
			double col[3] = { 1.0,0.0,0.0 };
			pipePolyline->SetColor(col);
		}
		break;
		case TEST_POLYLINE:
		{
			pipePolyline->SetRepresentation(mafPipePolyline::LINES);
			pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_POLYLINE_REPRESENTATION));
		}
		break;
		case TEST_POLYLINE_SPLINE_MODE:
		{
			pipePolyline->SetSplineMode(TRUE);
			pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_SPLINE));
		}
		break;
		case TEST_TUBE:
		{
			pipePolyline->SetSplineMode(FALSE);
			pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_SPLINE));
			pipePolyline->SetRepresentation(mafPipePolyline::TUBES);
			pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_POLYLINE_REPRESENTATION));
		}
		break;
		case TEST_TUBE_SPLINE_MODE:
		{
			pipePolyline->SetSplineMode(TRUE);
			pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_SPLINE));
		}
		break;
		case TEST_TUBE_CHANGE_VALUES:
		{
			pipePolyline->SetRadius(2.0);
			pipePolyline->SetTubeResolution(5.0);
			pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_TUBE_RADIUS));
			pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_TUBE_RESOLUTION));
		}
		break;
		case TEST_TUBE_CAPPING:
		{
			pipePolyline->SetTubeCapping(TRUE);
			pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_TUBE_CAPPING));
		}
		break;
		case TEST_GLYPH:
		{
			pipePolyline->SetRepresentation(mafPipePolyline::SPHERES);
			pipePolyline->SetShowSphere(true);
			pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_POLYLINE_REPRESENTATION));
		}
		break;
		case TEST_GLYPH_CHANGE_VALUES:
		{
			pipePolyline->SetRadius(2.0);
			pipePolyline->SetGlyphResolution(5.0);
			pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_SPHERE_RADIUS));
			pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_SPHERE_RESOLUTION));
		}
		break;
		case TEST_GLYPH_SPLINE_MODE:
		{
			pipePolyline->SetSplineMode(TRUE);
			pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_SPLINE));
			pipePolyline->SetRadius(1.0);
			pipePolyline->SetGlyphResolution(10.0);
			pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_SPHERE_RADIUS));
			pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_SPHERE_RESOLUTION));
		} 
		break;

		default:
			break;
		}

		pipePolyline->GetAssemblyFront()->GetActors(actorList);
		actorList->InitTraversal();
		vtkProp *actor = actorList->GetNextProp();

		while (actor)
		{
			m_Renderer->AddActor(actor);
			m_RenderWindow->Render();
			actor = actorList->GetNextProp();
		}

		COMPARE_IMAGES("TestPipeExecution", i);
	}

	vtkDEL(actorList);
	delete sceneNode;
}