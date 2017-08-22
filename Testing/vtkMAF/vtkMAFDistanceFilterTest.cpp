/*=========================================================================

 Program: MAF2
 Module: vtkMAFDistanceFilterTest.cpp
 Authors: Nicola Vanella
 
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

#include "vtkMAFDistanceFilterTest.h"
#include "vtkMAFDistanceFilter.h"
#include "vtkSphereSource.h"
#include "vtkPolyDataNormals.h"
#include "vtkMAFSmartPointer.h"
#include "vtkDataSetReader.h"

//-----------------------------------------------------------
void vtkMAFDistanceFilterTest::TestDynamicAllocation()
{
	vtkMAFDistanceFilter *filter;
	vtkNEW(filter);
	vtkDEL(filter);
}

//----------------------------------------------------------------------------
void vtkMAFDistanceFilterTest::TestInput()
{
	vtkMAFSmartPointer<vtkMAFDistanceFilter> filter;

	CPPUNIT_ASSERT(filter->GetTotalNumberOfInputConnections() == 0);

	vtkMAFSmartPointer<vtkSphereSource> sphere;
	sphere->Update();

	vtkMAFSmartPointer<vtkPolyDataNormals> normals;
	normals->SetInputConnection(sphere->GetOutputPort());

	filter->SetInputConnection(normals->GetOutputPort());
	filter->Update();

	CPPUNIT_ASSERT(filter->GetTotalNumberOfInputConnections() == 1);
}

//----------------------------------------------------------------------------
void vtkMAFDistanceFilterTest::TestGetSet()
{
	vtkMAFSmartPointer<vtkMAFDistanceFilter> filter;

	// Distance Mode
	filter->SetDistanceMode(VTK_SCALAR);	
	CPPUNIT_ASSERT(filter->GetDistanceMode() == VTK_SCALAR);

	filter->SetDistanceModeToVector();
	CPPUNIT_ASSERT(filter->GetDistanceMode() == VTK_VECTOR);
	CPPUNIT_ASSERT(filter->GetDistanceModeAsString() == "Vector");

	filter->SetDistanceModeToScalar();
	CPPUNIT_ASSERT(filter->GetDistanceMode() == VTK_SCALAR);
	CPPUNIT_ASSERT(filter->GetDistanceModeAsString() == "Scalar");

	// Filter Mode
	filter->SetFilterMode(VTK_DISTANCE_MODE);
	CPPUNIT_ASSERT(filter->GetFilterMode() == VTK_DISTANCE_MODE);

	filter->SetFilterModeToDensity();
	CPPUNIT_ASSERT(filter->GetFilterMode() == VTK_DENSITY_MODE);
	CPPUNIT_ASSERT(filter->GetFilterModeAsString() == "Density");

	filter->SetFilterModeToDistance();
	CPPUNIT_ASSERT(filter->GetFilterMode() == VTK_DISTANCE_MODE);
	CPPUNIT_ASSERT(filter->GetFilterModeAsString() == "Distance");
}

//----------------------------------------------------------------------------
void vtkMAFDistanceFilterTest::TestFilter_Scalar_Density()
{
	////// import vtkData ////////////////////
	vtkDataSetReader *importer;
	vtkNEW(importer);
	mafString filename = MAF_DATA_ROOT;
	filename << "/VTK_Volumes/volume.vtk";
	importer->SetFileName(filename);
	importer->Update();
	
	vtkMAFSmartPointer<vtkSphereSource> sphere;
 	sphere->SetCenter(60, 60, 50);
	sphere->Update();

	vtkMAFSmartPointer<vtkPolyDataNormals> normals;
	normals->SetInputConnection(sphere->GetOutputPort());

	// Create Filter
	vtkMAFSmartPointer<vtkMAFDistanceFilter> filter;

	filter->SetDistanceModeToScalar();
	filter->SetFilterModeToDensity();

	filter->SetSource(importer->GetOutput());
	filter->SetInputConnection(normals->GetOutputPort());
	filter->Update();

	//	
	CPPUNIT_ASSERT(filter->GetOutput()->GetNumberOfPoints() == 66);
	CPPUNIT_ASSERT(filter->GetOutput()->GetNumberOfCells() == 96);
	CPPUNIT_ASSERT(filter->GetOutput()->GetPointData()->GetNumberOfTuples() == 66);
	
	vtkDataArray *vectors = filter->GetOutput()->GetPointData()->GetVectors();
	vtkDataArray *scalars = filter->GetOutput()->GetPointData()->GetScalars();
	
  CPPUNIT_ASSERT(vectors == NULL && scalars != NULL);

	CPPUNIT_ASSERT(scalars->GetTuple1(10) == 19877.480468750000);
	CPPUNIT_ASSERT(scalars->GetTuple1(15) == 18057.615234375000);
	CPPUNIT_ASSERT(scalars->GetTuple1(20) == 18629.535156250000);
	CPPUNIT_ASSERT(scalars->GetTuple1(31) == 20268.123046875000);
	CPPUNIT_ASSERT(scalars->GetTuple1(44) == 19265.169921875000);
	CPPUNIT_ASSERT(scalars->GetTuple1(55) == 17749.121093750000);

	//
	vtkDEL(importer);
}

//----------------------------------------------------------------------------
void vtkMAFDistanceFilterTest::TestFilter_Vector_Distance()
{
	////// import vtkData ////////////////////
	vtkDataSetReader *importer;
	vtkNEW(importer);
	mafString filename = MAF_DATA_ROOT;
	filename << "/VTK_Volumes/volume.vtk";
	importer->SetFileName(filename);
	importer->Update();
	
	vtkMAFSmartPointer<vtkSphereSource> sphere;
	sphere->SetCenter(60, 60, 50);
	sphere->Update();

	vtkMAFSmartPointer<vtkPolyDataNormals> normals;
	normals->SetInputConnection(sphere->GetOutputPort());

	// Create Filter
	vtkMAFSmartPointer<vtkMAFDistanceFilter> filter;

	filter->SetDistanceModeToVector();
	filter->SetFilterModeToDistance();

	filter->SetSource(importer->GetOutput());
	filter->SetInputConnection(normals->GetOutputPort());
	filter->Update();

	//	
	vtkDataArray *vectors = filter->GetOutput()->GetPointData()->GetVectors();
	vtkDataArray *scalars = filter->GetOutput()->GetPointData()->GetScalars();

	CPPUNIT_ASSERT(vectors != NULL && scalars == NULL);

	CPPUNIT_ASSERT(vectors->GetTuple3(10)[0] == -9.7669057846069336);
	CPPUNIT_ASSERT(vectors->GetTuple3(10)[1] == -4.0455884933471680);
	CPPUNIT_ASSERT(vectors->GetTuple3(10)[2] == -3.0398604869842529);

	CPPUNIT_ASSERT(vectors->GetTuple3(33)[0] == 5.9230184555053711);
	CPPUNIT_ASSERT(vectors->GetTuple3(33)[1] == 6.2009935379028320);
	CPPUNIT_ASSERT(vectors->GetTuple3(33)[2] == -6.8895235061645508);

	CPPUNIT_ASSERT(vectors->GetTuple3(55)[0] == 4.0455870628356934);
	CPPUNIT_ASSERT(vectors->GetTuple3(55)[1] == -9.7669029235839844);
	CPPUNIT_ASSERT(vectors->GetTuple3(55)[2] == 3.0398747920989990);
	
	//
	vtkDEL(importer);
}
