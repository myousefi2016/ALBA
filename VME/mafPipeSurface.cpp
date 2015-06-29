/*=========================================================================

 Program: MAF2
 Module: mafPipeSurface
 Authors: Silvano Imboden - Paolo Quadrani
 
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

#include "mafPipeSurface.h"
#include "mafSceneNode.h"
#include "mafGUI.h"
#include "mafGUIMaterialButton.h"
#include "mafAxes.h"
#include "mmaMaterial.h"
#include "mafEventSource.h"
#include "mafVMESurface.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"

#include "vtkPointData.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "mafLODActor.h"
#include "vtkRenderer.h"
#include "vtkGlyph3D.h"
#include "vtkPolyDataNormals.h"
#include "vtkActor.h"
#include "vtkLookupTable.h"
#include "vtkLineSource.h"
#include "vtkCellCenters.h"
#include "vtkCellData.h"
#include "vtkArrowSource.h"
#include "vtkFeatureEdges.h"
#include "vtkMAFPolyDataNormals.h"

#include <vector>



//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeSurface);
//----------------------------------------------------------------------------
//#include "mafMemDbg.h"

//----------------------------------------------------------------------------
mafPipeSurface::mafPipeSurface()
:mafPipe()
//----------------------------------------------------------------------------
{
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_OutlineActor    = NULL;
  m_MaterialButton  = NULL;
  m_SurfaceMaterial = NULL;
  m_Gui             = NULL;

	m_Normal							= NULL;
	m_NormalGlyph					= NULL;
	m_NormalMapper				= NULL;
	m_NormalActor					= NULL;
	m_CenterPointsFilter	= NULL;
	m_NormalArrow					= NULL;

  m_EdgesActor          = NULL;
  m_EdgesMapper         = NULL;
  m_ExtractEdges        = NULL;

  m_ScalarVisibility = 0;
	m_NormalVisibility = 0;
	m_EdgeVisibility	 = 0;
  m_RenderingDisplayListFlag = 0;

  m_UseVTKProperty  = 1;
  m_UseLookupTable  = 0;

  m_EnableActorLOD  = 0;

  m_SelectedScalarsArray = 0;     // (added by Losi 2011/04/08 to allow scalars array selection)
  m_SelectedDataAttribute = 0;  // (added by Losi 2011/04/08 to allow scalars array selection)
}
//----------------------------------------------------------------------------
void mafPipeSurface::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);
  
  m_Selected = false;
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_OutlineActor    = NULL;
  m_Axes            = NULL;

  m_Vme->Update();
  assert(m_Vme->GetOutput()->IsMAFType(mafVMEOutputSurface));
  mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
  assert(surface_output);
  surface_output->Update();
  vtkPolyData *data = vtkPolyData::SafeDownCast(surface_output->GetVTKData());
  assert(data);

  m_Vme->GetEventSource()->AddObserver(this);

  vtkDataSetAttributes *dataAttribute = NULL;
  if(m_SelectedDataAttribute == 0) // Point data
  {
    dataAttribute = data->GetPointData();
  }
  else if(m_SelectedDataAttribute == 1) // Cell data
  {
    dataAttribute =  data->GetCellData();
  }

  vtkDataArray *scalars = dataAttribute->GetScalars();

  double sr[2] = {0,1};
  if(scalars != NULL)
  {
    //m_ScalarVisibility = 1;
    scalars->GetRange(sr);
  }

  m_SurfaceMaterial = surface_output->GetMaterial();
  assert(m_SurfaceMaterial);  // all vme that use PipeSurface must have the material correctly set

  vtkNEW(m_Mapper);

	//BES: 11.9.2012 - VTK rendering core is stupid to calculate normal vectors all the time unless they are specified in the input data
	//to speed up rendering in time-variant situations (in static, display lists make this problem negligible), we calculate normal vectors here
	if (data->GetPointData() == NULL || data->GetPointData()->GetNormals() != NULL) 
		m_Mapper->SetInputData(data);
	else
	{
		vtkMAFSmartPointer< vtkMAFPolyDataNormals > normals;
		normals->SetInputData(data);
		normals->SetComputePointNormals(1);
		normals->SetComputeCellNormals(0);
		m_Mapper->SetInputConnection(normals->GetOutputPort());	
	}			
  
  //m_RenderingDisplayListFlag = m_Vme->IsAnimated() ? 0 : 1;
  m_RenderingDisplayListFlag = m_Vme->IsAnimated() ? 1 : 0;
  m_Mapper->SetImmediateModeRendering(m_RenderingDisplayListFlag);
  
  // TODO: REFACTOR THIS 
  // workaround code to show RefSys colors by default... RefSys could have a different pipe
  // inheriting from this one...
  if (m_Vme->IsA("mafVMERefSys"))
  {
    m_ScalarVisibility = 1;
  }
  // END TODO: REFACTOR THIS 

  m_Mapper->SetScalarVisibility(m_ScalarVisibility);

	m_Mapper->SetLookupTable(m_SurfaceMaterial->m_ColorLut);
  m_Mapper->SetScalarRange(sr);

  vtkNEW(m_Actor);

	m_Actor->SetMapper(m_Mapper);
  //m_Actor->SetEnableHighThreshold(m_EnableActorLOD);
  if (m_SurfaceMaterial->m_MaterialType == mmaMaterial::USE_LOOKUPTABLE)
  {
    m_UseVTKProperty = 0;
    m_UseLookupTable = 1;
  }
  if (m_SurfaceMaterial->m_MaterialType == mmaMaterial::USE_VTK_PROPERTY)
  {
    m_UseVTKProperty = 1;
    m_UseLookupTable = 0;
    m_Actor->SetProperty(m_SurfaceMaterial->m_Prop);
  }

  if (m_Gui)
  {
    m_Gui->Update();
  }

  m_AssemblyFront->AddPart(m_Actor);

  // selection highlight
  vtkMAFSmartPointer<vtkOutlineCornerFilter> corner;
	corner->SetInputData(data);

  vtkMAFSmartPointer<vtkPolyDataMapper> corner_mapper;
	corner_mapper->SetInputConnection(corner->GetOutputPort());

  vtkMAFSmartPointer<vtkProperty> corner_props;
	corner_props->SetColor(1,1,1);
	corner_props->SetAmbient(1);
	corner_props->SetRepresentationToWireframe();
	corner_props->SetInterpolationToFlat();

	vtkNEW(m_OutlineActor);
	m_OutlineActor->SetMapper(corner_mapper);
	m_OutlineActor->VisibilityOff();
	m_OutlineActor->PickableOff();
	m_OutlineActor->SetProperty(corner_props);

  m_AssemblyFront->AddPart(m_OutlineActor);

  if(m_RenFront)
	  m_Axes = new mafAxes(m_RenFront, m_Vme);

	if(m_Vme->IsA("mafVMERefSys"))
		m_Axes->SetVisibility(false);
  	
/*	
	if(data->GetCellData()->GetNormals())
	{
		CreateNormalsPipe();
	}

	CreateEdgesPipe();
*/
}
//----------------------------------------------------------------------------
void mafPipeSurface::CreateEdgesPipe()
//----------------------------------------------------------------------------
{
	mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
	surface_output->Update();
	vtkPolyData *data = vtkPolyData::SafeDownCast(surface_output->GetVTKData());

	vtkNEW(m_ExtractEdges);
	m_ExtractEdges->SetInputData(data);
	m_ExtractEdges->SetBoundaryEdges(1);
	m_ExtractEdges->SetFeatureEdges(0);
	m_ExtractEdges->SetNonManifoldEdges(0);
	m_ExtractEdges->SetManifoldEdges(0);
	m_ExtractEdges->Update();

	vtkNEW(m_EdgesMapper);
	m_EdgesMapper->SetInputConnection(m_ExtractEdges->GetOutputPort());
	m_EdgesMapper->ScalarVisibilityOff();
	m_EdgesMapper->Update();

	vtkNEW(m_EdgesActor);
	m_EdgesActor->SetMapper(m_EdgesMapper);
	m_EdgesActor->PickableOff();
	m_EdgesActor->GetProperty()->SetColor(1-m_Actor->GetProperty()->GetColor()[0],1-m_Actor->GetProperty()->GetColor()[1],1-m_Actor->GetProperty()->GetColor()[2]);
	m_EdgesActor->SetVisibility(m_EdgeVisibility);
	m_EdgesActor->Modified();

	m_AssemblyFront->AddPart(m_EdgesActor);
}
//----------------------------------------------------------------------------
void mafPipeSurface::CreateNormalsPipe()
//----------------------------------------------------------------------------
{
	mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
	surface_output->Update();
	vtkPolyData *data = vtkPolyData::SafeDownCast(surface_output->GetVTKData());

	vtkNEW(m_CenterPointsFilter);
	m_CenterPointsFilter->SetInputData(data);
	m_CenterPointsFilter->Update();

	vtkPolyData *centers = m_CenterPointsFilter->GetOutput();
	centers->GetPointData()->SetNormals(data->GetCellData()->GetNormals());

	double bounds[6];
	data->GetBounds(bounds);
	double maxBounds = (bounds[1]-bounds[0] < bounds[3]-bounds[2])?bounds[1]-bounds[0]:bounds[3]-bounds[2];
	maxBounds = (maxBounds<bounds[5]-bounds[4])?maxBounds:bounds[5]-bounds[4];

	vtkNEW(m_NormalArrow);
	m_NormalArrow->SetTipLength(0.0);
	m_NormalArrow->SetTipRadius(0.0);
	m_NormalArrow->SetShaftRadius(0.005*maxBounds);
	m_NormalArrow->SetTipResolution(16);
	m_NormalArrow->SetShaftResolution(16);
	m_NormalArrow->Update();

	vtkNEW(m_NormalGlyph);
	m_NormalGlyph->SetInputData(centers);
	m_NormalGlyph->SetSourceConnection(m_NormalArrow->GetOutputPort());
	m_NormalGlyph->SetVectorModeToUseNormal();
	m_NormalGlyph->Update();

	vtkNEW(m_NormalMapper);
	m_NormalMapper->SetInputConnection(m_NormalGlyph->GetOutputPort());
	m_NormalMapper->Update();

	vtkNEW(m_NormalActor);
	m_NormalActor->SetMapper(m_NormalMapper);
	m_NormalActor->SetVisibility(m_NormalVisibility);
	m_NormalActor->PickableOff();
	m_NormalActor->Modified();

	m_AssemblyFront->AddPart(m_NormalActor);
}
//----------------------------------------------------------------------------
mafPipeSurface::~mafPipeSurface()
//----------------------------------------------------------------------------
{
  m_Vme->GetEventSource()->RemoveObserver(this);

	if(m_Actor)
		m_AssemblyFront->RemovePart(m_Actor);
	if(m_OutlineActor)
		m_AssemblyFront->RemovePart(m_OutlineActor);
	if(m_NormalActor)
		m_AssemblyFront->RemovePart(m_NormalActor);
	if(m_EdgesActor)
		m_AssemblyFront->RemovePart(m_EdgesActor);

  cppDEL(m_Axes);
  cppDEL(m_MaterialButton);

  vtkDEL(m_Mapper);
  vtkDEL(m_Actor);
  vtkDEL(m_OutlineActor);
  vtkDEL(m_NormalActor);
  vtkDEL(m_NormalMapper);
  vtkDEL(m_CenterPointsFilter);
  vtkDEL(m_NormalGlyph);
  vtkDEL(m_NormalArrow);
  vtkDEL(m_Normal);
  vtkDEL(m_EdgesActor);
  vtkDEL(m_EdgesMapper);
  vtkDEL(m_ExtractEdges);
}
//----------------------------------------------------------------------------
void mafPipeSurface::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_Actor->GetVisibility()) 
	{
		m_OutlineActor->SetVisibility(sel);
		if(!m_Vme->IsA("mafVMERefSys"))
			m_Axes->SetVisibility(sel);
	}
}
//----------------------------------------------------------------------------
/*void mafPipeSurface::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
}*/
//----------------------------------------------------------------------------
mafGUI *mafPipeSurface::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);
  m_Gui->Bool(ID_RENDERING_DISPLAY_LIST,"displaylist",&m_RenderingDisplayListFlag,0,"turn on/off \nrendering displaylist calculation");
	//m_Gui->Bool(ID_SCALAR_VISIBILITY,"scalar vis.", &m_ScalarVisibility,0,"turn on/off the scalar visibility");
	m_Gui->Bool(ID_NORMAL_VISIBILITY,"norm. vis.",&m_NormalVisibility);
	m_Gui->Bool(ID_EDGE_VISIBILITY,"edge vis.",&m_EdgeVisibility);
  m_Gui->Divider();
  m_Gui->Bool(ID_USE_VTK_PROPERTY,"property",&m_UseVTKProperty);
  m_MaterialButton = new mafGUIMaterialButton(m_Vme,this);
  m_Gui->AddGui(m_MaterialButton->GetGui());
  m_MaterialButton->Enable(m_UseVTKProperty != 0);
  m_Gui->Divider();
  m_Gui->Bool(ID_USE_LOOKUP_TABLE,"lut",&m_UseLookupTable);
  double sr[2];
  m_Mapper->GetScalarRange(sr);
  m_SurfaceMaterial->m_ColorLut->SetTableRange(sr);
	m_Gui->Lut(ID_LUT,"lut",m_SurfaceMaterial->m_ColorLut);
  //m_Gui->Bool(ID_LUT, "new lut", &m_UseLookupTable);
  m_Gui->Bool(ID_SCALAR_VISIBILITY,"scalar vis.", &m_ScalarVisibility,0,"turn on/off the scalar visibility");
	m_Gui->Enable(ID_LUT,m_UseLookupTable != 0);
  //m_Gui->Divider(2);
  //m_Gui->Bool(ID_ENABLE_LOD,"LOD",&m_EnableActorLOD);

  //// (added by Losi 2011/04/08 to allow scalars array selection)
  // Get the surface's scalars array
  // Scalars data type selection (point data or cell data)
  wxString scalarsDataTypes[2];
  scalarsDataTypes[0] = "points";
  scalarsDataTypes[1] = "cells";
  m_Gui->Combo(ID_SCALARS_DATA_TYPE_SELECTION,"scalars ty.",&m_SelectedDataAttribute,2,scalarsDataTypes,"Determine the visible scalars data type (points data or cell data)");
  m_Gui->Enable(ID_SCALARS_DATA_TYPE_SELECTION, m_ScalarVisibility != 0);

  vtkDataSetAttributes *dataAttribute = GetSelectedDataAttribute();

  // Scalars array selection
  // if(data->GetPointData()->GetNumberOfArrays()>0)
  // {
  wxArrayString scalarsArrayNames;
  for(int i = 0; i < dataAttribute->GetNumberOfArrays(); i++)
  {
    scalarsArrayNames.Add(dataAttribute->GetArrayName(i));
  }
  wxCArrayString scalarsCArrayNames = wxCArrayString(scalarsArrayNames); // Use this class because wxArrayString::GetStringsArray() is a deprecated method
  m_ScalarsArraySelection = m_Gui->Combo(ID_SCALARS_ARRAY_SELECTION,"scalars ar.",&m_SelectedScalarsArray,scalarsCArrayNames.GetCount(),scalarsCArrayNames.GetStrings(),"Determine the visible scalars array");
  m_Gui->Enable(ID_SCALARS_ARRAY_SELECTION, m_ScalarVisibility != 0 && dataAttribute->GetNumberOfArrays() > 0);
  // }
  ////

  m_Gui->Label("");

  if (m_SurfaceMaterial == NULL)
  {
    mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
    m_SurfaceMaterial = surface_output->GetMaterial();
  }
  m_Gui->Enable(ID_USE_LOOKUP_TABLE, m_SurfaceMaterial->m_ColorLut != NULL);
  m_Gui->Divider();
	return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeSurface::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
      case ID_SCALAR_VISIBILITY:
      {
        vtkDataSetAttributes *dataAttribute = GetSelectedDataAttribute();
				if (m_Gui != NULL)
				{ 
					m_Gui->Enable(ID_LUT,m_ScalarVisibility != 0);
          m_Gui->Enable(ID_SCALARS_DATA_TYPE_SELECTION, m_ScalarVisibility != 0);
          m_Gui->Enable(ID_SCALARS_ARRAY_SELECTION, m_ScalarVisibility != 0 && dataAttribute->GetNumberOfArrays() > 0);
				  m_Gui->Update();
        }
        UpdateScalarsArrayVisualization(dataAttribute);
				mafEventMacro(mafEvent(this,CAMERA_UPDATE)); //BES 12.9.2012
      }
    	break;
      case ID_LUT:
      {
        m_SurfaceMaterial->UpdateFromLut();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
      case ID_ENABLE_LOD:
      {
        //m_Actor->SetEnableHighThreshold(m_EnableActorLOD);
        m_OutlineActor->SetEnableHighThreshold(m_EnableActorLOD);
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
      case ID_USE_VTK_PROPERTY:
      {
        if (m_UseVTKProperty != 0)
        {
          m_Actor->SetProperty(m_SurfaceMaterial->m_Prop);
        }
        else
        {
          m_Actor->SetProperty(NULL);
        }

        if (m_MaterialButton != NULL)
        {
          m_MaterialButton->Enable(m_UseVTKProperty != 0);
        }
        mafEventMacro(mafEvent(this,CAMERA_UPDATE))
      };
      break;
      case ID_RENDERING_DISPLAY_LIST:
      {
        m_Mapper->SetImmediateModeRendering(m_RenderingDisplayListFlag);
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
			case ID_NORMAL_VISIBILITY:
      {
        if(m_NormalActor)
        {
          m_NormalActor->SetVisibility(m_NormalVisibility);
        }
        else if(m_Vme->GetOutput()->GetVTKData()->GetCellData()->GetNormals())
        {
          CreateNormalsPipe();
        }
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
			break;
			case ID_EDGE_VISIBILITY:
      {
        if(m_EdgesActor)
        {
          m_EdgesActor->GetProperty()->SetColor(1-m_Actor->GetProperty()->GetColor()[0],1-m_Actor->GetProperty()->GetColor()[1],1-m_Actor->GetProperty()->GetColor()[2]);
          m_EdgesActor->SetVisibility(m_EdgeVisibility);
        }
        else
        {
          CreateEdgesPipe();
        }
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
      case ID_USE_LOOKUP_TABLE:
      {
        m_Gui->Enable(ID_LUT,m_UseLookupTable != 0);
      }
      break;
      //// (added by Losi 2011/04/08 to allow scalars array selection)
      case ID_SCALARS_DATA_TYPE_SELECTION:
      {
        vtkDataSetAttributes *dataAttribute = GetSelectedDataAttribute(); 

        // Update the scalars array selection combo m_ScalarsArraySelection
        // Clear the combo list
        m_ScalarsArraySelection->Clear();

        // Get the scalars arrays names and append them to the combo list
        m_SelectedScalarsArray = 0;
        m_Gui->Enable(ID_SCALARS_ARRAY_SELECTION, m_ScalarVisibility != 0 && dataAttribute->GetNumberOfArrays() > 0);
        for(int sa = 0; sa < dataAttribute->GetNumberOfArrays(); sa++)
        {
          m_ScalarsArraySelection->Append(_T(dataAttribute->GetArrayName(sa)));
        }
        m_ScalarsArraySelection->SetSelection(0);
        m_Gui->Update();

        UpdateScalarsArrayVisualization(dataAttribute);
				mafEventMacro(mafEvent(this,CAMERA_UPDATE)); //BES 12.9.2012
      }
      case ID_SCALARS_ARRAY_SELECTION:
      {
        vtkDataSetAttributes *dataAttribute = GetSelectedDataAttribute(); 
        UpdateScalarsArrayVisualization(dataAttribute);
				mafEventMacro(mafEvent(this,CAMERA_UPDATE)); //BES 12.9.2012
      }
      break;
      ////
      default:
        mafEventMacro(*e);
      break;
    }
  }
  else if (maf_event->GetSender() == m_Vme)
  {
    if(maf_event->GetId() == VME_TIME_SET)
    {
      vtkDataSetAttributes *dataAttribute = GetSelectedDataAttribute(); 
      UpdateScalarsArrayVisualization(dataAttribute);
			//the caller will cause CAMERA_UPDATE, so not necessary (and even undesirable) to do so here
    }
  }
}
//----------------------------------------------------------------------------
void mafPipeSurface::SetEnableActorLOD(bool value)
//----------------------------------------------------------------------------
{
  m_EnableActorLOD = (int) value;
  if(m_Gui)
    m_Gui->Update();
}
//----------------------------------------------------------------------------
void mafPipeSurface::SetActorPicking(int enable)
//----------------------------------------------------------------------------
{
	m_Actor->SetPickable(enable);
  m_Actor->Modified();
	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafPipeSurface::UpdateScalarsArrayVisualization(vtkDataSetAttributes *dataAttribute) // (added by Losi 2011/04/08 to allow scalars array selection)
//----------------------------------------------------------------------------
{
  // Update the scalars array visualization
  m_Mapper->SetScalarVisibility(m_ScalarVisibility != 0 && dataAttribute->GetNumberOfArrays() > 0);
  if(m_ScalarVisibility != 0 && dataAttribute->GetNumberOfArrays() > 0)
  {
    dataAttribute->SetActiveScalars(dataAttribute->GetArrayName(m_SelectedScalarsArray));
    dataAttribute->Update();
    double range[2];
    dataAttribute->GetArray(dataAttribute->GetArrayName(m_SelectedScalarsArray))->GetRange(range);
    m_Mapper->SetScalarRange(range);
  }

  //mafEventMacro(mafEvent(this,CAMERA_UPDATE)); //BES 12.9.2012 - do not call it here since this method is called on VME_TIME_SET  and, thus, this would cause repetitive rendering
}
//----------------------------------------------------------------------------
vtkDataSetAttributes *mafPipeSurface::GetSelectedDataAttribute() // (added by Losi 2011/04/08 to allow scalars array selection)
//----------------------------------------------------------------------------
{
  // Return the selected data attribute (point or cell data)
  m_Vme->Update();
  // assert(m_Vme->GetOutput()->IsMAFType(mafVMEOutputSurface)); Already verified in the Create() method
  mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
  // assert(surface_output); Already verified in the Create() method
  surface_output->Update();
  vtkPolyData *data = vtkPolyData::SafeDownCast(surface_output->GetVTKData());

  vtkDataSetAttributes *dataAttribute = NULL;
  if(m_SelectedDataAttribute == 0) // Point data
  {
    dataAttribute = data->GetPointData();
  }
  else if(m_SelectedDataAttribute == 1) // Cell data
  {
    dataAttribute =  data->GetCellData();
  }
  return dataAttribute;
}

