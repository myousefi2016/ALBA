/*=========================================================================

 Program: MAF2
 Module: mafDeviceButtonsPadMouseDialogTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafDeviceButtonsPadMouseDialogTest_H__
#define __CPP_UNIT_mafDeviceButtonsPadMouseDialogTest_H__

#include "mafTest.h"

class mafDeviceButtonsPadMouseDialogTest : public mafTest
{
 public:

 CPPUNIT_TEST_SUITE( mafDeviceButtonsPadMouseDialogTest );
 CPPUNIT_TEST( TestFixture );
 CPPUNIT_TEST( TestConstructorDestructor );
 CPPUNIT_TEST( TestSetView );
 CPPUNIT_TEST_SUITE_END();

 protected:

   void TestFixture();
   void TestConstructorDestructor();
   void TestSetView();

};

#endif
