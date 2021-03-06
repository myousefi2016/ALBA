/*=========================================================================

 Program: MAF2
 Module: mafOpMML3ParameterViewTest
 Authors: Alberto Losi
 
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

#include "mafString.h"

#include "mafOpMML3ParameterViewTest.h"
#include "mafOpMML3ParameterView.h"

//----------------------------------------------------------------------------
void mafOpMML3ParameterViewTest::BeforeTest()
//----------------------------------------------------------------------------
{
  vtkNEW(m_RenderWindow);
  vtkNEW(m_Renderer);

  m_Renderer->SetBackground(0, 0, 0);
  //m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(320, 240);
  m_RenderWindow->SetPosition(400,0);
}
//----------------------------------------------------------------------------
void mafOpMML3ParameterViewTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_RenderWindow);
  vtkDEL(m_Renderer);
}

//----------------------------------------------------------------------------
void mafOpMML3ParameterViewTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafOpMML3ParameterView *view = new mafOpMML3ParameterView(m_RenderWindow, m_Renderer);
  CPPUNIT_ASSERT(NULL != view);
  cppDEL(view);
}

//----------------------------------------------------------------------------
void mafOpMML3ParameterViewTest::TestGetValue()
//----------------------------------------------------------------------------
{
  mafOpMML3ParameterView *view = new mafOpMML3ParameterView(m_RenderWindow, m_Renderer);
  float val = view->GetValue(0);
  CPPUNIT_ASSERT(0 == val);
  cppDEL(view);
}

//----------------------------------------------------------------------------
void mafOpMML3ParameterViewTest::TestGetNumberOfDataPoints()
//----------------------------------------------------------------------------
{
  mafOpMML3ParameterView *view = new mafOpMML3ParameterView(m_RenderWindow, m_Renderer);
  CPPUNIT_ASSERT(0 == view->GetNumberOfDataPoints());
  cppDEL(view);
}

//----------------------------------------------------------------------------
void mafOpMML3ParameterViewTest::TestGetMaxY()
//----------------------------------------------------------------------------
{
  mafOpMML3ParameterView *view = new mafOpMML3ParameterView(m_RenderWindow, m_Renderer);
  view->SetRangeY(0,5,10);

  CPPUNIT_ASSERT(10 == view->GetMaxY());
  cppDEL(view);
}

//----------------------------------------------------------------------------
void mafOpMML3ParameterViewTest::TestGetMinY()
//----------------------------------------------------------------------------
{
  mafOpMML3ParameterView *view = new mafOpMML3ParameterView(m_RenderWindow, m_Renderer);
  view->SetRangeY(0,5,10);

  CPPUNIT_ASSERT(0 == view->GetMinY());
  cppDEL(view);
}

//----------------------------------------------------------------------------
void mafOpMML3ParameterViewTest::TestSetLineActorX()
//----------------------------------------------------------------------------
{
  mafOpMML3ParameterView *view = new mafOpMML3ParameterView(m_RenderWindow, m_Renderer);
  view->SetRangeX(10);
  view->SetRangeY(0,5,10);

  view->AddPoint(1,1);
  view->AddPoint(2,2);

  view->SetLineActorX(1);

  // test with render
  m_Renderer->ResetCamera();
  view->Render();

	COMPARE_IMAGES("TestSetLineActorX");
  cppDEL(view);
}

//----------------------------------------------------------------------------
void mafOpMML3ParameterViewTest::TestGetPointsActor()
//----------------------------------------------------------------------------
{
  mafOpMML3ParameterView *view = new mafOpMML3ParameterView(m_RenderWindow, m_Renderer);
  view->SetRangeX(10);
  view->SetRangeY(0,5,10);

  view->AddPoint(1,1);
  view->AddPoint(2,2);

  CPPUNIT_ASSERT(NULL != view->GetPointsActor());

  cppDEL(view);
}

//----------------------------------------------------------------------------
void mafOpMML3ParameterViewTest::TestGetSplineActor()
//----------------------------------------------------------------------------
{
  mafOpMML3ParameterView *view = new mafOpMML3ParameterView(m_RenderWindow, m_Renderer);
  view->SetRangeX(10);
  view->SetRangeY(0,5,10);

  view->AddPoint(1,1);
  view->AddPoint(2,2);

  CPPUNIT_ASSERT(NULL != view->GetSplineActor());

  cppDEL(view);
}

//----------------------------------------------------------------------------
void mafOpMML3ParameterViewTest::TestRemovePoint()
//----------------------------------------------------------------------------
{
  mafOpMML3ParameterView *view = new mafOpMML3ParameterView(m_RenderWindow, m_Renderer);

  view->SetRangeX(10);
  view->SetRangeY(0,5,10);

  view->AddPoint(1,1);
  view->AddPoint(2,2);

  view->RemovePoint(1);
  view->RemovePoint(2);

  CPPUNIT_ASSERT(24 != view->GetValue(0));
  CPPUNIT_ASSERT(0 == view->GetValue(0));
  
  cppDEL(view);
}

//----------------------------------------------------------------------------
void mafOpMML3ParameterViewTest::TestAddPoint()
//----------------------------------------------------------------------------
{
  mafOpMML3ParameterView *view = new mafOpMML3ParameterView(m_RenderWindow, m_Renderer);

  view->SetRangeX(10);
  view->SetRangeY(0,5,10);

  view->AddPoint(1,1);
  view->AddPoint(2,2);
  CPPUNIT_ASSERT(24 == view->GetValue(0));
  cppDEL(view);
}

//----------------------------------------------------------------------------
void mafOpMML3ParameterViewTest::TestRender()
//----------------------------------------------------------------------------
{
  mafOpMML3ParameterView *view = new mafOpMML3ParameterView(m_RenderWindow, m_Renderer);

  view->SetRangeX(10);
  view->SetRangeY(0,5,10);

  view->AddPoint(1,1);
  view->AddPoint(2,2);
  
  m_Renderer->ResetCamera();
  view->Render();

	COMPARE_IMAGES("TestRender");

  cppDEL(view);
}