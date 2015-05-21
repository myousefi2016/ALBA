/*=========================================================================

 Program: MAF2
 Module: vtkMAFDataPipe
 Authors: Marco Petrone
 
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


#include "mafDecl.h"
#include "vtkMAFDataPipe.h"
#include "mafEventBase.h"

#include "mafDataPipeInterpolatorVTK.h"
#include "mafVME.h"

#include "vtkDataSet.h"
#include "vtkObjectFactory.h"
#include "vtkErrorCode.h"
#include "vtkAlgorithm.h"
#include "vtkExecutive.h"
//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkMAFDataPipe)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vtkMAFDataPipe::vtkMAFDataPipe()
//------------------------------------------------------------------------------
{
  m_DataPipe = NULL;
}

//------------------------------------------------------------------------------
vtkMAFDataPipe::~vtkMAFDataPipe()
//------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void vtkMAFDataPipe::SetDataPipe(mafDataPipe *dpipe)
//----------------------------------------------------------------------------
{
  m_DataPipe=dpipe;
}

//----------------------------------------------------------------------------
void vtkMAFDataPipe::SetNthInput(int num, vtkDataSet *input)
//----------------------------------------------------------------------------
{
  Superclass::SetNthInput(num,input);
}

//----------------------------------------------------------------------------
// Get the MTime. Take in consideration also modifications to the MAF data pipe
unsigned long vtkMAFDataPipe::GetMTime()
//------------------------------------------------------------------------------
{
  unsigned long mtime = this->Superclass::GetMTime();

  if (m_DataPipe)
  {
    unsigned long dpipeMTime = m_DataPipe->GetMTime();
    if (dpipeMTime > mtime)
    {
      mtime = dpipeMTime;
    }
  }

  return mtime;
}

//------------------------------------------------------------------------------
unsigned long vtkMAFDataPipe::GetInformationTime()
//------------------------------------------------------------------------------
{
  return InformationTime.GetMTime();
}

//------------------------------------------------------------------------------
vtkDataSet *vtkMAFDataPipe::GetOutput(int idx)
//------------------------------------------------------------------------------
{
  if (this->NumberOfOutputs < idx+1)
  {
    UpdateInformation(); // force creating the outputs
  }
  return Superclass::GetOutput(idx);
}

//------------------------------------------------------------------------------
vtkDataSet *vtkMAFDataPipe::GetOutput()
//------------------------------------------------------------------------------
{
  if (this->NumberOfOutputs == 0)
  {
    UpdateInformation(); // force creating the outputs
  }
  return Superclass::GetOutput();
}

//------------------------------------------------------------------------------
void vtkMAFDataPipe::UpdateInformation()
//------------------------------------------------------------------------------
{
  // forward event to MAF data pipe
  if (m_DataPipe)
    m_DataPipe->OnEvent(&mafEventBase(this,VME_OUTPUT_DATA_PREUPDATE));

  this->Superclass::UpdateInformation();
}

//------------------------------------------------------------------------------
void vtkMAFDataPipe::ExecuteInformation()
//------------------------------------------------------------------------------
{
  this->SetErrorCode( vtkErrorCode::NoError );
  
  // check if output array is still empty
  if (this->Outputs==NULL||this->Outputs[0]==NULL)
  {
    // create a new object of the same type of those in the array
    if (GetNumberOfInputs()>0)
    {
      for (int i=0;i<GetNumberOfInputs();i++)
      {
        
        vtkDataSet *data=(vtkDataSet *)GetInputs()[i];
        if (data)
        {
          data->UpdateInformation();
          vtkDataSet *new_data=data->NewInstance();
          new_data->CopyInformation(data);
          
					this->GetExecutive()->SetOutputData(i,new_data);
          new_data->Delete();
        }
      }
    }
  } 
  
  if (GetNumberOfInputs()>0&&GetInput()) // work around to skip vtkDataSet bug with zero inputs
    Superclass::ExecuteInformation(); 
}

//------------------------------------------------------------------------------
void vtkMAFDataPipe::Execute()
//------------------------------------------------------------------------------
{
  if (GetInput())
  {
    if(m_DataPipe->IsA("mafDataPipeCustom"))
      m_DataPipe->OnEvent(&mafEventBase(this,VME_OUTPUT_DATA_UPDATE));
    for (int i=0;i<GetNumberOfInputs();i++)
    {
      if (GetNumberOfOutputs()>i)
      {
        vtkDataSet *input=(vtkDataSet *)GetInputs()[i];
        input->Update();
        this->Outputs[i]->ShallowCopy(input);
      }
      else
      {
        vtkErrorMacro("DEBUG: NULL output pointer!");
      }
    }
    // forward event to MAF data pipe
    if(!m_DataPipe->IsA("mafDataPipeCustom"))
      m_DataPipe->OnEvent(&mafEventBase(this,VME_OUTPUT_DATA_UPDATE));
  }
}
