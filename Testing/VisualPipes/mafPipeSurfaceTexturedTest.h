/*=========================================================================

 Program: MAF2
 Module: mafPipeSurfaceTexturedTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafPipeSurfaceTexturedTest_H__
#define __CPP_UNIT_mafPipeSurfaceTexturedTest_H__

#include "mafTest.h"

class vtkPolyData;
class vtkRenderWindowInteractor;
class vtkProp;
class vtkPropCollection;

class mafPipeSurfaceTexturedTest : public mafTest
{
  public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

	CPPUNIT_TEST_SUITE( mafPipeSurfaceTexturedTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
	CPPUNIT_TEST( TestPipeTextureExecution );
	CPPUNIT_TEST( TestPipeClassicExecution );
	CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
	void TestPipeTextureExecution();
	void TestPipeClassicExecution();

  vtkRenderWindowInteractor *m_RenderWindowInteractor;

  /** Select The Actor that will be controlled*/
  vtkProp *SelectActorToControl(vtkPropCollection* propList, int index);
};

#endif
