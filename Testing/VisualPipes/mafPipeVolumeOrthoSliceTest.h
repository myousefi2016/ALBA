/*=========================================================================

 Program: MAF2
 Module: mafPipeVolumeOrthoSliceTest
 Authors: Eleonora Mambrini, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafPipeVolumeOrthoSliceTest_H__
#define __CPP_UNIT_mafPipeVolumeOrthoSliceTest_H__

#include "mafTest.h"

class vtkPolyData;
class vtkRenderWindowInteractor;
class vtkProp;
class vtkPropCollection;

class mafPipeVolumeOrthoSliceTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafPipeVolumeOrthoSliceTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestPipeExecution );
  CPPUNIT_TEST( TestPipeExecution_SetSliceOpacity );
  CPPUNIT_TEST( TestPipeExecution_SetLutRange );
  CPPUNIT_TEST( TestPipeExecution_SetColorLookupTable );
	CPPUNIT_TEST( TestPipeExecution_SetInterpolation );
	CPPUNIT_TEST( TestPipeExecution_TicksOnOff );


  CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
  void TestPipeExecution();
  void TestPipeExecution_SetSliceOpacity();
  void TestPipeExecution_SetLutRange();
  void TestPipeExecution_SetColorLookupTable();
  void TestPipeExecution_TicksOnOff();
	void TestPipeExecution_SetInterpolation();

  vtkRenderWindowInteractor *m_RenderWindowInteractor;
};

#endif
