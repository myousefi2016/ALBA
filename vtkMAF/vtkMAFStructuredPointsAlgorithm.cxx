/*=========================================================================

 Program: MAF2
 Module: vtkMAFStructuredPointsAlgorithm
 Authors: Gianluigi Crimi
 
 Copyright (c) IOR-LTM
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkMAFStructuredPointsAlgorithm.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkDataObject.h"

vtkStandardNewMacro(vtkMAFStructuredPointsAlgorithm);

//----------------------------------------------------------------------------
int vtkMAFStructuredPointsAlgorithm::FillOutputPortInformation(int port, vtkInformation* info)
{
	// now add our info
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkImageData"); 
	return 1;
}

//----------------------------------------------------------------------------
int vtkMAFStructuredPointsAlgorithm::FillInputPortInformation(int port, vtkInformation* info)
{
	info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
	return 1;
}
