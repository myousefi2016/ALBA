/*=========================================================================

 Program: MAF2
 Module: vtkMAFDummyRectilinearGridAlgorithm
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_DUMMYRectilinearGridAlgorithm_H__
#define __CPP_UNIT_DUMMYRectilinearGridAlgorithm_H__

#include "vtkMAFRectilinearGridAlgorithm.h"

class vtkMAFDummyRectilinearGridAlgorithm : public vtkMAFRectilinearGridAlgorithm
{
  vtkTypeMacro(vtkMAFDummyRectilinearGridAlgorithm,vtkMAFRectilinearGridAlgorithm);

protected:
  void operator=(const vtkMAFDummyRectilinearGridAlgorithm&) {};
  vtkMAFDummyRectilinearGridAlgorithm(const vtkMAFDummyRectilinearGridAlgorithm&) {};

};

#endif
