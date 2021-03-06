/*=========================================================================

 Program: MAF2
 Module: vtkMAFImageFillHolesRemoveIslandsTest
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
#include "vtkMAFImageFillHolesRemoveIslands.h"
#include "vtkMAFImageFillHolesRemoveIslandsTest.h"

#include <cppunit/config/SourcePrefix.h>

#include "vtkRenderWindowInteractor.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkBMPReader.h"
#include "vtkTexture.h"
#include "vtkPlaneSource.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkStructuredPoints.h"
#include "vtkPointData.h"
#include "vtkTimerLog.h"

#include "mafString.h"

//------------------------------------------------------------
void vtkMAFImageFillHolesRemoveIslandsTest::TestFixture()
//------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void vtkMAFImageFillHolesRemoveIslandsTest::BeforeTest()
//----------------------------------------------------------------------------
{
	vtkNEW(m_Renderer);
	vtkNEW(m_RenderWindow);

	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(640, 480);
	m_RenderWindow->SetPosition(100, 0);

	m_Renderer->SetBackground(0.0, 0.0, 0.0);

	m_TestName = "Test";
}
//----------------------------------------------------------------------------
void vtkMAFImageFillHolesRemoveIslandsTest::AfterTest()
//----------------------------------------------------------------------------
{
	vtkDEL(m_Renderer);
	vtkDEL(m_RenderWindow);
}

//------------------------------------------------------------
void vtkMAFImageFillHolesRemoveIslandsTest::RenderData(vtkActor *actor)
//------------------------------------------------------------
{
  vtkRenderWindowInteractor *renderWindowInteractor = vtkRenderWindowInteractor::New();
  renderWindowInteractor->SetRenderWindow(m_RenderWindow);

	m_Renderer->AddActor(actor);
	m_RenderWindow->Render();

	COMPARE_IMAGES(m_TestName);

  renderWindowInteractor->Delete();
  vtkTimerLog::CleanupLog();
}

//------------------------------------------------------------
void vtkMAFImageFillHolesRemoveIslandsTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkMAFImageFillHolesRemoveIslands *to = vtkMAFImageFillHolesRemoveIslands::New();
  to->Delete();
}

//--------------------------------------------
void vtkMAFImageFillHolesRemoveIslandsTest::TestFillHoles()
//--------------------------------------------
{
  m_Algorithm = vtkMAFImageFillHolesRemoveIslands::FILL_HOLES;
	m_TestName = "TestFillHoles";
  TestAlgorithm();
}
//--------------------------------------------
void vtkMAFImageFillHolesRemoveIslandsTest::TestRemoveIslands()
//--------------------------------------------
{
  m_Algorithm = vtkMAFImageFillHolesRemoveIslands::REMOVE_ISLANDS;
	m_TestName = "TestRemoveIslands";
  TestAlgorithm();
}

//--------------------------------------------
void vtkMAFImageFillHolesRemoveIslandsTest::TestAlgorithm()
//--------------------------------------------
{
  mafString filename = MAF_DATA_ROOT;
  filename<<"/Test_ImageFillHolesRemoveIslands/test.bmp";

  //load an image
  vtkBMPReader *r = vtkBMPReader::New();
  r->SetFileName(filename.GetCStr());
  r->Allow8BitBMPOn();
  r->SetDataScalarTypeToUnsignedChar();
  r->Update();

  //create vtkImageData from zero and set the correct parameters (spacing, dimension) ...
  vtkStructuredPoints *originalImage = vtkStructuredPoints::New();
  originalImage->SetSpacing(r->GetOutput()->GetSpacing());
  originalImage->SetDimensions(r->GetOutput()->GetDimensions());
  //originalImage->AllocateScalars();
  originalImage->SetScalarTypeToUnsignedChar();

  // and scalar
  vtkUnsignedCharArray *scalars = vtkUnsignedCharArray::New();
  scalars->SetNumberOfComponents(1);
  scalars->SetName("SCALARS");
  for(int i=0; i<originalImage->GetNumberOfPoints();i++)
  {
    scalars->InsertNextTuple1(r->GetOutput()->GetPointData()->GetScalars()->GetTuple(i)[0]);
  }
  originalImage->GetPointData()->SetScalars(scalars);
  originalImage->Update();

  r->Delete();

  vtkMAFImageFillHolesRemoveIslands *filter = vtkMAFImageFillHolesRemoveIslands::New();
  filter->SetInput(originalImage);
  filter->SetEdgeSize(1);
  filter->SetAlgorithm(m_Algorithm);
  filter->Update();

  vtkStructuredPoints *outputImage = vtkStructuredPoints::New();
  outputImage->DeepCopy(filter->GetOutput());
  outputImage->Update();

  vtkPlaneSource *imagePlane = vtkPlaneSource::New();
  imagePlane->SetOrigin(0.,0.,0.);
  imagePlane->SetPoint1(1.,0.,0.);
  imagePlane->SetPoint2(0.,1.,0.);

  double w,l, range[2];
  outputImage->GetScalarRange(range);
  w = range[1] - range[0];
  l = (range[1] + range[0]) * .5;

  vtkWindowLevelLookupTable *imageLUT = vtkWindowLevelLookupTable::New();
  imageLUT->SetWindow(w);
  imageLUT->SetLevel(l);
  //m_ImageLUT->InverseVideoOn();
  imageLUT->Build();

  vtkTexture *imageTexture = vtkTexture::New();
  imageTexture->RepeatOff();
  imageTexture->InterpolateOn();
  imageTexture->SetQualityTo32Bit();
  imageTexture->SetInput(outputImage);
  imageTexture->SetLookupTable(imageLUT);
  imageTexture->MapColorScalarsThroughLookupTableOn();
  imageTexture->Modified();

  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  mapper->SetInput(imagePlane->GetOutput());
  mapper->ImmediateModeRenderingOff();

  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);
  actor->SetTexture(imageTexture);

  RenderData(actor);

  scalars->Delete();
  actor->Delete();
  mapper->Delete();
  imageTexture->Delete();
  imageLUT->Delete();
  imagePlane->Delete();
  outputImage->Delete();
  originalImage->Delete();
  filter->Delete();
}