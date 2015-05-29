/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFLargeImageSource.cxx,v $ 
  Language: C++ 
  Date: $Date: 2011-05-26 08:51:01 $ 
  Version: $Revision: 1.1.2.2 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#include "vtkMAFLargeImageSource.h"
#include "vtkMAFLargeImageData.h"
#include "vtkMAFLargeDataProvider.h"
#include "vtkExecutive.h"
#include "vtkAlgorithm.h"
#include "vtkObjectFactory.h"

#include "mafMemDbg.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"


//----------------------------------------------------------------------------
vtkMAFLargeImageSource::vtkMAFLargeImageSource()
{
	this->SetNumberOfInputPorts(0);
	this->SetNumberOfOutputPorts(1);
}

//----------------------------------------------------------------------------
// Specify the input data or filter.
void vtkMAFLargeImageSource::SetOutput(vtkMAFLargeImageData *output)
{
	this->GetExecutive()->SetOutputData(0, output);
}

//----------------------------------------------------------------------------
// Specify the input data or filter.
vtkMAFLargeImageData *vtkMAFLargeImageSource::GetOutput()
{
	return (vtkMAFLargeImageData *)this->GetExecutive()->GetOutputData(0);
}


//----------------------------------------------------------------------------
// Convert to Imaging API
int vtkMAFLargeImageSource::RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **vtkNotUsed(inputVector), vtkInformationVector *outputVector)
{
	vtkInformation *outInfo = outputVector->GetInformationObject(0);
	vtkMAFLargeImageData *output = vtkMAFLargeImageData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

	// If we have multiple Outputs, they need to be allocate
	// in a subclass.  We cannot be sure all outputs are images.
	output->SetExtent(this->GetUpdateExtent());
	output->AllocateScalars();

	// call ExecuteData
	this->ExecuteData( output );

	return 1;
}


//----------------------------------------------------------------------------
vtkMAFLargeImageData *vtkMAFLargeImageSource::AllocateOutputData(vtkDataObject *out)
{
	vtkMAFLargeImageData *res = vtkMAFLargeImageData::SafeDownCast(out);
	if (!res)
	{
		vtkWarningMacro("Call to AllocateOutputData with non vtkMAFLargeImageData output");
		return NULL;
	}


	// I would like to eliminate this method which requires extra "information"
	// That is not computed in the graphics pipeline.
	// Until I can eliminate the method, I will reexecute the ExecuteInformation
	// before the execute.
	//this->ExecuteInformation();
		
	res->SetExtent(this->GetUpdateExtent());
	res->AllocateScalars();
	return res;
}

//----------------------------------------------------------------------------
vtkMAFLargeImageData *vtkMAFLargeImageSource::GetOutput(int idx)
{
	return vtkMAFLargeImageData::SafeDownCast( this->GetExecutive()->GetOutputData(idx) );
}

//----------------------------------------------------------------------------
void vtkMAFLargeImageSource::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
}
