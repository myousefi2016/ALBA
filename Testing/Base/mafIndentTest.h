/*=========================================================================

 Program: MAF2
 Module: mafIndentTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafIndentTest_H__
#define __CPP_UNIT_mafIndentTest_H__

#include "mafTest.h"

/** Test for mafIndent; Use this suite to trace memory problems */
class mafIndentTest : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafIndentTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestGetTypeName);
  CPPUNIT_TEST(TestGetNextIndent);
  CPPUNIT_TEST(TestOperator_int);
  CPPUNIT_TEST(TestFriend_outputstream);
  
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestGetTypeName();
  void TestGetNextIndent();
  void TestOperator_int();
  void TestFriend_outputstream();
};

#endif
