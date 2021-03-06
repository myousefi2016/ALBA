/*=========================================================================

 Program: MAF2
 Module: mafOpVOIDensityEditorTestTest
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpVOIDensityEditorTestTest_H__
#define __CPP_UNIT_mafOpVOIDensityEditorTestTest_H__

#include "mafTest.h"

/** Test for mafOpVOIDensityEditorTest; Use this suite to trace memory problems */
class mafOpVOIDensityEditorTestTest : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafOpVOIDensityEditorTestTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(DynamicAllocationTest);
  CPPUNIT_TEST(EditVolumeScalarsTest);

  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void DynamicAllocationTest();
  void EditVolumeScalarsTest();

  bool result;
};

#endif
