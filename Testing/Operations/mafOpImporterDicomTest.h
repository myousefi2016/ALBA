/*=========================================================================

 Program: MAF2
 Module: mafOpImporterDicomTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpImporterDicomTest_H
#define CPP_UNIT_mafOpImporterDicomTest_H

#include "mafTest.h"
using namespace std;
 
class mafOpImporterDicomTest : public mafTest
{

public:

  CPPUNIT_TEST_SUITE( mafOpImporterDicomTest );
    CPPUNIT_TEST( TestDynamicAllocation );
    CPPUNIT_TEST( TestAccept );
    CPPUNIT_TEST( TestCreateVolume );
    CPPUNIT_TEST( TestCompareDicomImage );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestAccept();
	void TestCompareDicomImage();
  void TestCreateVolume();

};

#endif
