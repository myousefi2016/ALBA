/*=========================================================================

 Program: MAF2
 Module: vtkMAFRectilinearGridAlgorithm
 Authors: Gianluigi Crimi
 
 Copyright (c) IOR-LTM
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkMAFRectilinearGridAlgorithm.h"
#include "vtkInformation.h"
#include "vtkDataObject.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkMAFRectilinearGridAlgorithm);

//----------------------------------------------------------------------------
int vtkMAFRectilinearGridAlgorithm::FillOutputPortInformation(int port, vtkInformation* info)
{
	// now add our info
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkRectilinearGrid"); 
	return 1;
}

//----------------------------------------------------------------------------
int vtkMAFRectilinearGridAlgorithm::FillInputPortInformation(int port, vtkInformation* info)
{
	info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkRectilinearGrid");
	return 1;
}
