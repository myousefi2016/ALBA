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

#ifndef __vtkMAFStructuredPointsAlgorithm_h
#define __vtkMAFStructuredPointsAlgorithm_h

#include "mafConfigure.h"
#include "vtkDataSetAlgorithm.h"
/**
class name: vtkMAFStructuredPointsAlgorithm
 vtkMAFStructuredPointsAlgorithm is a filter whose subclasses take as
 input rectilinear grid datasets and generate polygonal data on output.
*/
class MAF_EXPORT vtkMAFStructuredPointsAlgorithm : public vtkDataSetAlgorithm
{
public:
  /** create an instance of the object */
  static vtkMAFStructuredPointsAlgorithm *New();
  /** RTTI Macro */
  vtkTypeMacro(vtkMAFStructuredPointsAlgorithm,vtkDataSetAlgorithm);


protected:
	/** specialize output information type */
	virtual int FillOutputPortInformation(int port, vtkInformation* info);
	/** specialize input information type */
	virtual int FillInputPortInformation(int port, vtkInformation* info);

};

#endif




