/*=========================================================================

 Program: MAF2
 Module: mafGizmoROITest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafGizmoROITest_H__
#define __CPP_UNIT_mafGizmoROITest_H__

#include "mafTest.h"
#include "mafGizmoROI.h"
#include "mafVMESurface.h"
#include "mafVMERoot.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

class mafGizmoROITest : public mafTest
{
  public:
  
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafGizmoROITest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestSetListener);
  CPPUNIT_TEST( TestSetInput );
  CPPUNIT_TEST( TestSetGetBounds );
  CPPUNIT_TEST( TestShow );
  CPPUNIT_TEST( TestShowHandles);
  CPPUNIT_TEST( TestShowShadingPlane );
  CPPUNIT_TEST( TestShowROI );
  CPPUNIT_TEST( TestHighlight );
  CPPUNIT_TEST( TestHighlightOff );
  CPPUNIT_TEST( TestSetConstrainRefSys );
  CPPUNIT_TEST( TestEnableMinimumHandleSize );
  CPPUNIT_TEST( TestSetMinimumHandleSize );
  CPPUNIT_TEST( TestReset );
  CPPUNIT_TEST_SUITE_END();
  
  protected:
    
    void TestFixture();

    void TestConstructor();
    void TestSetListener();
    void TestSetInput();
    void TestShow();
    void TestShowHandles();
    void TestShowShadingPlane();
    void TestShowROI();
    void TestSetGetBounds();
    void TestHighlight();
    void TestHighlightOff();
    void TestSetConstrainRefSys();
    void TestEnableMinimumHandleSize();
    void TestSetMinimumHandleSize();
    void TestReset();

    void CreateTestData();

    void RenderGizmo( mafGizmoROI *gizmoROI );

    mafVMESurface *m_GizmoInputSurface;
    mafVMERoot *m_Root;

    // TODO: REFACTOR THIS 
    // move somewhere else... there should be a common class for testing facilities
    void RenderData(vtkDataSet *data);

    void CreateRenderStuff(); 
      
    vtkRenderer *m_Renderer;
    vtkRenderWindow *m_RenderWindow;
    vtkRenderWindowInteractor *m_RenderWindowInteractor;    
    
};

#endif
