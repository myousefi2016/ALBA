/*=========================================================================

Program: MAF2
Module: vtkMAFRGtoSPImageFilter
Authors: Gianluigi Crimi

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
#include "vtkMAFRGtoSPImageFilter.h"
#include "vtkObjectFactory.h"
#include "vtkStructuredPoints.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "vtkRectilinearGrid.h"
#include "vtkMAFSmartPointer.h"
#include "vtkProbeFilter.h"
#include "vtkDataSetWriter.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"

vtkStandardNewMacro(vtkMAFRGtoSPImageFilter);

#define EPSILON 1e-3


//----------------------------------------------------------------------------
int vtkMAFRGtoSPImageFilter::FillOutputPortInformation(int port, vtkInformation* info)
{
	// now add our info
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkStructuredPoints");
	return 1;
}

//--------------------------------------------------------------------------------------
vtkMAFRGtoSPImageFilter::vtkMAFRGtoSPImageFilter()
{
}

//--------------------------------------------------------------------------------------
int vtkMAFRGtoSPImageFilter::RequestInformation(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Initialize some frequently used values.
	vtkRectilinearGrid  *input = vtkRectilinearGrid::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkStructuredPoints *output = vtkStructuredPoints::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
	
	int dims[3], outDims[3], extent[6];
	double bestSpacing[3], outputSpacing[3],bounds[6];
  
	if (input == NULL)
	{
		vtkErrorMacro("Missing input");
		return 0;
	}

	if (output == NULL)
	{
		vtkErrorMacro("Output error");
		return 0;
	}
	
	input->GetDimensions(dims);
	if (dims[2] != 1)
	{
		vtkErrorMacro("Wrong input type");
		return 0;
	}
		
	input->GetExtent(extent);
	input->GetBounds(bounds);
	GetBestSpacing(input, bestSpacing);

	outDims[0] = ((bounds[1] - bounds[0]) / bestSpacing[0]) + 1;
	outDims[1] = ((bounds[3] - bounds[2]) / bestSpacing[1]) + 1;
	outDims[2] = 1;

	outputSpacing[0] = (bounds[1] - bounds[0]) / (double)(outDims[0]-1);
	outputSpacing[1] = (bounds[3] - bounds[2]) / (double)(outDims[1]-1);
	outputSpacing[2] = 1;

	//output->SetDimensions(outDims);
	output->SetSpacing(outputSpacing);
	output->SetOrigin(bounds[0], bounds[2], bounds[4]);

	
	OutputExtent[0] = 0;
	OutputExtent[1] = outDims[0]-1;
	OutputExtent[2] = 0;
	OutputExtent[3] = outDims[1]-1;
	OutputExtent[4] = 0;
	OutputExtent[5] = outDims[2]-1;

	return 1;
}

//=========================================================================
int vtkMAFRGtoSPImageFilter::RequestData(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	int outDims[3];
	vtkRectilinearGrid *input = vtkRectilinearGrid::SafeDownCast(GetInput());
	vtkImageData *output = vtkImageData::SafeDownCast(GetOutput());

	vtkPointData 			*inputPd = input->GetPointData();
	vtkDataArray 			*inputScalars = inputPd->GetScalars();
	vtkDataArray 			*outScalars = inputPd->GetScalars()->NewInstance();

	output->SetExtent(OutputExtent);

	output->GetDimensions(outDims);
	outScalars->SetNumberOfTuples(outDims[0] * outDims[1]);

	void *inputPointer = inputScalars->GetVoidPointer(0);
	void *outputPointer = outScalars->GetVoidPointer(0);

	switch (inputScalars->GetDataType())
	{
		case VTK_CHAR:
			FillSP(input, output, (char*) inputPointer, (char*) outputPointer);
			break;
		case VTK_UNSIGNED_CHAR:
			FillSP(input, output, (unsigned char*)inputPointer, (unsigned char*)outputPointer);
			break;
		case VTK_SHORT:
			FillSP(input, output, (short*)inputPointer, (short*)outputPointer);
			break;
		case VTK_UNSIGNED_SHORT:
			FillSP(input, output, (unsigned short*)inputPointer, (unsigned short*)outputPointer);
			break;
		case VTK_FLOAT:
			FillSP(input, output, (float*)inputPointer, (float*)outputPointer);
			break;
		case VTK_DOUBLE:  //NOTE: GPU is not allowed
			FillSP(input, output, (double*)inputPointer, (double*)outputPointer);
		default:
			vtkErrorMacro(<< "vtkMAFVolumeSlicer: Scalar type is not supported");
			return 0;
	}
	output->GetPointData()->SetScalars(outScalars);
	vtkDEL(outScalars);

	return 1;
}

//----------------------------------------------------------------------------
template<typename DataType>
void vtkMAFRGtoSPImageFilter::FillSP(vtkRectilinearGrid * input, vtkImageData * output, DataType *inputScalars, DataType *outScalars)
{
	int outDims[3], inDims[3], x, y, xRG, yRG, yOffset, yRGoffsetA, yRGoffsetB, newIdx;
	double spacing[3], bounds[6], yRatioA, yRatioB, xRatioA, xRatioB, ySize, xSize, yPos, xPos, yCoordA, yCoordB, xCoordA, xCoordB, acc1, acc2, acc;
	vtkDataArray 			*xCoordinates, *yCoordinates;

	xCoordinates = input->GetXCoordinates();
	yCoordinates = input->GetYCoordinates();
	double *xCoordPointer = (double *)xCoordinates->GetVoidPointer(0);
	double *yCoordPointer = (double *)yCoordinates->GetVoidPointer(0);

	input->GetBounds(bounds);
	output->GetDimensions(outDims);
	input->GetDimensions(inDims);
	output->GetSpacing(spacing);

	if (inDims[0] == outDims[0] && inDims[1] == outDims[1] && inDims[2] == outDims[2])
	{
		memcpy(outScalars, inputScalars, sizeof(DataType)*inDims[0] * inDims[1] * inDims[2]);
	}
	else
	{
		newIdx = 0;

		yRG = 0;
		yCoordA = yCoordPointer[yRG];
		yCoordB = yCoordPointer[yRG + 1];
		yRGoffsetA = 0;
		yRGoffsetB = inDims[0];
		ySize = yCoordB - yCoordA;
		for (y = 0; y < outDims[1]; y++)
		{
			yPos = bounds[2] + y*spacing[1];

			while (yPos > yCoordB)
			{
				yRG++;
				yCoordA = yCoordB;
				yCoordB = yCoordPointer[yRG + 1];

				yRGoffsetA = yRG * inDims[0];
				yRGoffsetB = (yRG + 1) * inDims[0];
				ySize = yCoordB - yCoordA;
			}

			yOffset = y * outDims[0];

			//Avoid problems on slicing volumes with doubled slices
			yRatioB = (ySize != 0) ? (yPos - yCoordA) / ySize : 1;
			yRatioA = 1.0 - yRatioB;


			xRG = 0;
			xCoordA = xCoordPointer[xRG];
			xCoordB = xCoordPointer[xRG + 1];
			xSize = xCoordB - xCoordA;
			for (x = 0; x < outDims[0]; x++)
			{
				xPos = bounds[0] + x*spacing[0];

				while (xPos > xCoordB)
				{
					xRG++;
					xCoordA = xCoordB;
					xCoordB = xCoordPointer[xRG + 1];
					xSize = xCoordB - xCoordA;
				}

				//Avoid problems on slicing volumes with duplicated slices
				xRatioB = (xSize != 0) ? (xPos - xCoordA) / xSize : 1;
				xRatioA = 1.0 - xRatioB;

				acc1 = inputScalars[xRG + yRGoffsetA] * xRatioA;
				acc1 += inputScalars[xRG + 1 + yRGoffsetA] * xRatioB;

				acc2 = inputScalars[xRG + yRGoffsetB] * xRatioA;
				acc2 += inputScalars[xRG + 1 + yRGoffsetB] * xRatioB;

				acc = acc1*yRatioA + acc2*yRatioB;

				outScalars[newIdx] = acc;
				newIdx++;
			}
		}
	}
}

//=========================================================================
void vtkMAFRGtoSPImageFilter::GetBestSpacing(vtkRectilinearGrid* rGrid, double * bestSpacing)
{
	bestSpacing[0] = VTK_DOUBLE_MAX;
	bestSpacing[1] = VTK_DOUBLE_MAX;
	bestSpacing[2] = VTK_DOUBLE_MAX;

	for (int xi = 1; xi < rGrid->GetXCoordinates()->GetNumberOfTuples(); xi++)
	{
		double spcx = rGrid->GetXCoordinates()->GetTuple1(xi) - rGrid->GetXCoordinates()->GetTuple1(xi - 1);
		if (spcx > EPSILON && bestSpacing[0] > spcx && spcx != 0.0)
			bestSpacing[0] = spcx;
	}

	for (int yi = 1; yi < rGrid->GetYCoordinates()->GetNumberOfTuples(); yi++)
	{
		double spcy = rGrid->GetYCoordinates()->GetTuple1(yi) - rGrid->GetYCoordinates()->GetTuple1(yi - 1);
		if (spcy> EPSILON && bestSpacing[1] > spcy && spcy != 0.0)
			bestSpacing[1] = spcy;
	}

	bestSpacing[2] = 1;
}

