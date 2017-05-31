/*=========================================================================

Program: MAF2
Module: vtkMAFVolumeSlicer
Authors: Gianluigi Crimi

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
#include "vtkMAFProjectVolume.h"
#include "vtkObjectFactory.h"
#include "vtkStructuredPoints.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "vtkRectilinearGrid.h"
#include "vtkMAFSmartPointer.h"
#include "vtkProbeFilter.h"
#include "vtkDataSetWriter.h"
#include "vtkMAFRGtoSPImageFilter.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"

vtkStandardNewMacro(vtkMAFProjectVolume);

#define AIR_LIMIT -500

//----------------------------------------------------------------------------
void vtkMAFProjectVolume::PropagateUpdateExtent(vtkDataObject *output)
{
}

//----------------------------------------------------------------------------
vtkMAFProjectVolume::vtkMAFProjectVolume()
{
  ProjectionMode = VTK_PROJECT_FROM_X;
	ProjectSubRange = false;
	
}


//----------------------------------------------------------------------------
int vtkMAFProjectVolume::FillOutputPortInformation(int port, vtkInformation* info)
{
	// now add our info
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkStructuredPoints");
	return 1;
}

//----------------------------------------------------------------------------
int vtkMAFProjectVolume::RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	vtkRectilinearGrid *inputRG = vtkRectilinearGrid::SafeDownCast(GetInput());
	vtkImageData *inputID = vtkImageData::SafeDownCast(GetInput());
	vtkImageData *output = vtkImageData::SafeDownCast(GetOutput());
  int dims[3], outDims[3], extent[6];
  
	if (inputID == NULL && inputRG == NULL)
	{
		vtkErrorMacro("Missing input");
		return 0;
	}

	if (output == NULL)
	{
		vtkErrorMacro("Output error");
		return 0;
	}

	if (inputRG)
		inputRG->GetExtent(extent);
	else
		inputID->GetExtent(extent);

	dims[0] = extent[1] - extent[0] + 1;
	dims[1] = extent[3] - extent[2] + 1;
	dims[2] = extent[5] - extent[4] + 1;

	switch (this->ProjectionMode) {
		case VTK_PROJECT_FROM_X:
			outDims[0] = dims[1];
			outDims[1] = dims[2];
			outDims[2] = 1;
			break;
		case VTK_PROJECT_FROM_Y:
			outDims[0] = dims[0];
			outDims[1] = dims[2];
			outDims[2] = 1;
			break;
		case VTK_PROJECT_FROM_Z:
			outDims[0] = dims[0];
			outDims[1] = dims[1];
			outDims[2] = 1;
	}

	extent[0] = 0;
	extent[1] = outDims[0] - 1;
	extent[2] = 0;
	extent[3] = outDims[1] - 1;
	extent[4] = 0;
	extent[5] = outDims[2] - 1;
  output->SetExtent( extent );

	return 1;
}

//----------------------------------------------------------------------------
int vtkMAFProjectVolume::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	int inputDims[3], projectedDims[3];

	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Initialize some frequently used values.
	vtkRectilinearGrid  *inputRG = vtkRectilinearGrid::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkImageData  *inputID = vtkImageData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkImageData *output = vtkImageData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

	vtkPointData 			*inputPd = inputRG ? inputRG->GetPointData() : inputID->GetPointData();
	vtkDataArray 			*inputScalars = inputPd->GetScalars();
	vtkDataArray 			*projScalars = inputScalars->NewInstance();

	inputRG ? inputRG->GetDimensions(inputDims) : inputID->GetDimensions(inputDims);

	switch (this->ProjectionMode) {
		case VTK_PROJECT_FROM_X:
			projectedDims[0] = inputDims[1];
			projectedDims[1] = inputDims[2];
			projectedDims[2] = 1;
			break;
		case VTK_PROJECT_FROM_Y:
			projectedDims[0] = inputDims[0];
			projectedDims[1] = inputDims[2];
			projectedDims[2] = 1;
			break;
		case VTK_PROJECT_FROM_Z:
			projectedDims[0] = inputDims[0];
			projectedDims[1] = inputDims[1];
			projectedDims[2] = 1;
			break;
	}
	projScalars->SetNumberOfTuples(projectedDims[0] * projectedDims[1]);

	void *inputPointer = inputScalars->GetVoidPointer(0);
	void *outputPointer = projScalars->GetVoidPointer(0);

	switch (inputScalars->GetDataType())
	{
		case VTK_CHAR:
			ProjectScalars(inputDims, (char*)inputPointer, (char*)outputPointer);
			break;
		case VTK_UNSIGNED_CHAR:
			ProjectScalars(inputDims, (unsigned char*)inputPointer, (unsigned char*)outputPointer);
			break;
		case VTK_SHORT:
			ProjectScalars(inputDims, (short*)inputPointer, (short*)outputPointer);
			break;
		case VTK_UNSIGNED_SHORT:
			ProjectScalars(inputDims, (unsigned short*)inputPointer, (unsigned short*)outputPointer);
			break;
		case VTK_FLOAT:
			ProjectScalars(inputDims, (float*)inputPointer, (float*)outputPointer);
			break;
		case VTK_DOUBLE:
			ProjectScalars(inputDims, (double*)inputPointer, (double*)outputPointer);
		default:
			vtkErrorMacro(<< "vtkMAFVolumeSlicer: Scalar type is not supported");
			return 0;
	}

	if (inputRG)
		GenerateOutputFromRG(request, inputRG, projectedDims, projScalars);
	else
		GenerateOutputFromID(request, inputID, projectedDims, projScalars);

	vtkDEL(projScalars);

	return 1;
}

//----------------------------------------------------------------------------
template<typename DataType>
void vtkMAFProjectVolume::ProjectScalars(int * inputDims, DataType * inputScalars, DataType * projScalars)
{
	int x, y, z, idx, newIdx, range[2];
	int sliceSize, jOffset, kOffset;
	float acc, rangeSize;

	//Set Projection Range
	if (ProjectSubRange)
	{
		range[0] = MAX(ProjectionRange[0], 0);
		range[1] = MIN(ProjectionRange[1], inputDims[ProjectionMode - 1]);
		rangeSize = range[1] - range[0];
	}
	else
	{
		range[0] = 0;
		range[1] = inputDims[ProjectionMode - 1];
		rangeSize = inputDims[ProjectionMode - 1];
	}


	sliceSize = inputDims[0] * inputDims[1];
	newIdx = 0;

	switch (this->ProjectionMode)
	{
		case VTK_PROJECT_FROM_X:
			for (z = 0; z < inputDims[2]; z++)
			{
				kOffset = z * sliceSize;
				for (y = 0; y < inputDims[1]; y++)
				{
					jOffset = y * inputDims[0];
					acc = 0;
					for (x = range[0]; x < range[1]; x++)
					{
						idx = x + jOffset + kOffset;
						acc += MAX(AIR_LIMIT, inputScalars[idx]);
					}
					projScalars[newIdx] = acc / rangeSize;
					newIdx++;
				}
			}
			break;
		case VTK_PROJECT_FROM_Y:
			for (z = 0; z < inputDims[2]; z++)
			{
				kOffset = z * sliceSize;
				for (x = 0; x < inputDims[0]; x++)
				{
					acc = 0;
					for (y = range[0]; y < range[1]; y++)
					{
						jOffset = y * inputDims[0];
						idx = x + jOffset + kOffset;
						acc += MAX(AIR_LIMIT, inputScalars[idx]);
					}
					projScalars[newIdx] = acc / rangeSize;
					newIdx++;
				}
			}
			break;
		case VTK_PROJECT_FROM_Z:
			for (y = 0; y < inputDims[1]; y++)
			{
				jOffset = y * inputDims[0];
				for (x = 0; x < inputDims[0]; x++)
				{
					acc = 0;
					for (z = range[0]; z < range[1]; z++)
					{
						kOffset = z * sliceSize;
						idx = x + jOffset + kOffset;
						acc += MAX(AIR_LIMIT, inputScalars[idx]);
					}
					projScalars[newIdx] = acc / rangeSize;
					newIdx++;
				}
			}
			break;
	}
}

//----------------------------------------------------------------------------
void vtkMAFProjectVolume::GenerateOutputFromID(vtkInformation *request, vtkImageData * inputSP, int * projectedDims, vtkDataArray * projScalars)
{
	double inputSpacing[3];
	double outputSpacing[3];
	vtkImageData *output = vtkImageData::SafeDownCast(GetOutput());
	
	inputSP->GetSpacing(inputSpacing);
	switch (this->ProjectionMode) {
		case VTK_PROJECT_FROM_X:
			outputSpacing[0] = inputSpacing[1];
			outputSpacing[1] = inputSpacing[2];
			outputSpacing[2] = 1;
			break;
		case VTK_PROJECT_FROM_Y:
			outputSpacing[0] = inputSpacing[0];
			outputSpacing[1] = inputSpacing[2];
			outputSpacing[2] = 1;
			break;
		case VTK_PROJECT_FROM_Z:
			outputSpacing[0] = inputSpacing[0];
			outputSpacing[1] = inputSpacing[1];
			outputSpacing[2] = 1;
	}

	output->SetScalarType(inputSP->GetScalarType(),request);
	output->SetNumberOfScalarComponents(inputSP->GetNumberOfScalarComponents(),request);
	output->SetDimensions(projectedDims);
	output->SetSpacing(outputSpacing);
	output->GetPointData()->SetScalars(projScalars);
}

//----------------------------------------------------------------------------
void vtkMAFProjectVolume::GenerateOutputFromRG(vtkInformation *request, vtkRectilinearGrid * inputRG, int * projectedDims, vtkDataArray * projScalars)
{
	//Generate temporary rectilinear grid output
	vtkRectilinearGrid *rgOut = vtkRectilinearGrid::New();
	vtkDataArray 			*XCoordinates, *YCoordinates, *ZCoordinates;
	double outputSpacing[3], bounds[6], bestSpacing[3];
	int outputDims[3];


	rgOut->SetDimensions(projectedDims);

	XCoordinates = inputRG->GetXCoordinates()->NewInstance();
	YCoordinates = inputRG->GetYCoordinates()->NewInstance();
	ZCoordinates = inputRG->GetZCoordinates()->NewInstance();

	XCoordinates->SetNumberOfTuples(projectedDims[0]);
	YCoordinates->SetNumberOfTuples(projectedDims[1]);
	ZCoordinates->SetNumberOfTuples(projectedDims[2]);

	ZCoordinates->InsertComponent(0, 0, 0);

	switch (this->ProjectionMode) {
		case VTK_PROJECT_FROM_X:
			XCoordinates->DeepCopy(inputRG->GetYCoordinates());
			YCoordinates->DeepCopy(inputRG->GetZCoordinates());
			break;
		case VTK_PROJECT_FROM_Y:
			XCoordinates->DeepCopy(inputRG->GetXCoordinates());
			YCoordinates->DeepCopy(inputRG->GetZCoordinates());
			break;
		case VTK_PROJECT_FROM_Z:
			XCoordinates->DeepCopy(inputRG->GetXCoordinates());
			YCoordinates->DeepCopy(inputRG->GetYCoordinates());
			break;
	}

	rgOut->SetXCoordinates(XCoordinates);
	rgOut->SetYCoordinates(YCoordinates);
	rgOut->SetZCoordinates(ZCoordinates);
	XCoordinates->Delete();
	YCoordinates->Delete();
	ZCoordinates->Delete();

	rgOut->GetPointData()->SetScalars(projScalars);
	
	
	vtkMAFRGtoSPImageFilter *rgtosoFilter = vtkMAFRGtoSPImageFilter::New();
	rgtosoFilter->SetInputData(rgOut);
	rgtosoFilter->Update();
	
	GetOutput()->DeepCopy(rgtosoFilter->GetOutput());

	vtkDEL(rgtosoFilter);
	vtkDEL(rgOut);
}
