/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFLargeImageSource.h,v $ 
  Language: C++ 
  Date: $Date: 2011-05-26 08:51:01 $ 
  Version: $Revision: 1.1.2.2 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#ifndef __vtkMAFLargeImageSource_h
#define __vtkMAFLargeImageSource_h

#include "mafConfigure.h"
#include "vtkMAFIdType64.h"
#include "vtkDataSetAlgorithm.h"

class vtkMAFLargeImageData;

class MAF_EXPORT vtkMAFLargeImageSource : public vtkDataSetAlgorithm
{
public:
  static vtkMAFLargeImageSource* New();

  vtkTypeMacro(vtkMAFLargeImageSource,vtkDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get the output of this source.
  void SetOutput(vtkMAFLargeImageData *output);
  vtkMAFLargeImageData *GetOutput();
  vtkMAFLargeImageData *GetOutput(int idx);
  
protected:
  vtkMAFLargeImageSource();
  ~vtkMAFLargeImageSource() {};

  int RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **vtkNotUsed(inputVector), vtkInformationVector *outputVector);

	virtual void ExecuteData(vtkDataObject *data)=0;
		
	virtual int RequestInformation(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outInfoVec)=0;


  // a helper method that sets the extent and allocates the output 
  // passed into it and returns it as an image data
  virtual vtkMAFLargeImageData *AllocateOutputData(vtkDataObject *out);

private:
  vtkMAFLargeImageSource(const vtkMAFLargeImageSource&);  // Not implemented.
  void operator=(const vtkMAFLargeImageSource&);  // Not implemented.
};


#endif


