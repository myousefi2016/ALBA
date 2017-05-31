/*=========================================================================

 Program: MAF2
 Module: vtkMAFPolyDataNormals.cxx
 Authors: Josef Kohout (besoft@kiv.zcu.cz)
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABo
 ILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkMAFPolyDataNormals.h"
#include "vtkSetGet.h"
#include "vtkPolydata.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"

vtkStandardNewMacro(vtkMAFPolyDataNormals);

vtkMAFPolyDataNormals::vtkMAFPolyDataNormals()
{
	this->m_LastUpdateTime = 0;
}

/*virtual*/ int vtkMAFPolyDataNormals::RequestData(vtkInformation *request,	vtkInformationVector **inputVector,	vtkInformationVector *outputVector)
{		
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Initialize some frequently used values.
	vtkPolyData  *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

	//strategy: if our settings has not changed, and our input has the same geometry,
	//then reuse already calculated normal vectors, otherwise, perform full calculation
	unsigned long tm = this->GetMTime();	
	
	if (input->GetPoints() != NULL) {
		tm += input->GetPoints()->GetMTime();
	}

	if (input->GetPolys() != NULL) {
		tm += input->GetPolys()->GetMTime();
	}

	if (input->GetStrips() != NULL) {
		tm += input->GetStrips()->GetMTime();
	}

	vtkPolyData* outp = this->GetOutput();
	if (outp == NULL || tm > this->m_LastUpdateTime)
	{
		//perform full update
		Superclass::RequestData(request,inputVector,outputVector);
	}
	else
	{
		//store already calculated normals
		vtkDataArray *np, *nc;
		if (outp->GetPointData() == NULL) {
			np = NULL;
		}
		else 
		{
			if (NULL != (np = outp->GetPointData()->GetNormals()))
				np->Register(this);
		}

		if (outp->GetCellData() == NULL) {
			nc = NULL;
		}
		else 
		{
			if (NULL != (nc = outp->GetCellData()->GetNormals()))
				nc->Register(this);
		}

		//perform update that just passes data to output
		int savePN = this->ComputePointNormals;
		this->ComputePointNormals = 0;

		int saveCN = this->ComputeCellNormals;
		this->ComputeCellNormals = 0;

		Superclass::RequestData(request,inputVector,outputVector);

		this->ComputePointNormals = savePN;
		this->ComputeCellNormals = saveCN;		

		//and restore our normals
		if (np != NULL)
		{
			outp->GetPointData()->SetNormals(np);
			np->UnRegister(this);
		}

		if (nc != NULL)
		{
			outp->GetCellData()->SetNormals(nc);
			nc->UnRegister(this);
		}							
	}
	
	
	this->m_LastUpdateTime = tm;

	return 1;
}
