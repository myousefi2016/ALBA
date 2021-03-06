/*=========================================================================

 Program: MAF2
 Module: mafPipeMeshSliceTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafPipeMeshSliceTest_H__
#define __CPP_UNIT_mafPipeMeshSliceTest_H__

#include "mafTest.h"

class vtkPolyData;
class vtkRenderWindowInteractor;
class vtkProp;
class vtkPropCollection;

class mafPipeMeshSliceTest : public mafTest
{
  public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

	CPPUNIT_TEST_SUITE( mafPipeMeshSliceTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
	CPPUNIT_TEST( TestPipeExecution );
  CPPUNIT_TEST( TestPipeExecution_Wireframe );
  CPPUNIT_TEST( TestPipeExecution_WiredActorVisibility );
  CPPUNIT_TEST( TestPipeExecution_FlipNormal );
  CPPUNIT_TEST( TestPipeExecution_UseVTKProperty );
  CPPUNIT_TEST( TestPipeExecution_Thickness_PickActor );
	CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
	void TestPipeExecution();
  void TestPipeExecution_Wireframe();
  void TestPipeExecution_WiredActorVisibility();
  void TestPipeExecution_FlipNormal();
  void TestPipeExecution_UseVTKProperty();
  void TestPipeExecution_Thickness_PickActor();

  vtkRenderWindowInteractor *m_RenderWindowInteractor;

  /**Compare Mapper scalarange with controlled values */
  void ProceduralControl(double controlRangeMapper[2] , vtkProp *propToControl);

  /** Select The Actor that will be controlled*/
  vtkProp *SelectActorToControl(vtkPropCollection* propList, int index);
};

#endif
