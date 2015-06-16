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

#ifndef __mafPipeSurface_H__
#define __mafPipeSurface_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkTexture;
class vtkPolyDataMapper;
class vtkPolyData;
class mafLODActor;
class mafAxes;
class mafGUIMaterialButton;
class mmaMaterial;
class vtkActor;
class vtkGlyph3D;
class vtkPolyDataNormals;
class vtkLineSource;
class vtkCellCenters;
class vtkArrowSource;
class vtkFeatureEdges;
class vtkDataSetAttributes;

//----------------------------------------------------------------------------
// mafPipeSurface :
//----------------------------------------------------------------------------
/** Visual pipe used to render VTK polydata and allowing to manage scalar visibility,
lookup table and some polygonal features like edges and normals.

Shared documentation:
https://docs.google.com/a/scsitaly.com/document/d/1j_AtB2aB3cwph6Tpi4QzjQYysOxPMbXBCgE3ZyLiS3M/edit?hl=en#

*/
class MAF_EXPORT mafPipeSurface : public mafPipe
{
public:
  mafTypeMacro(mafPipeSurface,mafPipe);

               mafPipeSurface();
  virtual     ~mafPipeSurface();

  /** process events coming from Gui */
  virtual void OnEvent(mafEventBase *maf_event);

  /** Create the VTK rendering pipeline*/
  virtual void Create(mafSceneNode *n);
  
  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
  virtual void Select(bool select); 

  /** Let to enable/disable the Level Of Detail behavior.*/
  void SetEnableActorLOD(bool value);

  /** IDs for the GUI */
  enum PIPE_SURFACE_WIDGET_ID
  {
    ID_SCALAR_VISIBILITY = Superclass::ID_LAST,
    ID_RENDERING_DISPLAY_LIST,
    ID_USE_VTK_PROPERTY,
    ID_USE_LOOKUP_TABLE,
    ID_LUT,
    ID_ENABLE_LOD,
		ID_NORMAL_VISIBILITY,
		ID_EDGE_VISIBILITY,
    ID_SCALARS_DATA_TYPE_SELECTION, // (added by Losi 2011/04/08 to allow scalars array selection)
    ID_SCALARS_ARRAY_SELECTION,     // (added by Losi 2011/04/08 to allow scalars array selection)
    ID_LAST
  };

  /** Set the actor picking*/
  void SetActorPicking(int enable = true);

  /** Get assembly front/back */
  virtual vtkMAFAssembly *GetAssemblyFront(){return m_AssemblyFront;};
  virtual vtkMAFAssembly *GetAssemblyBack(){return m_AssemblyBack;};

  void SetEdgeVisibilityOn(){m_EdgeVisibility = true;};
  void SetEdgeVisibilityOff(){m_EdgeVisibility = false;};

  void SetNormalVisibilityOn(){m_NormalVisibility = true;};
  void SetNormalVisibilityOff(){m_NormalVisibility = false;};

  void SetScalarVisibilityOn(){m_ScalarVisibility = true;};
  void SetScalarVisibilityOff(){m_ScalarVisibility = false;};

  void SetUseVtkPropertyOn(){m_UseVTKProperty = true;};
  void SetUseVtkPropertyOff(){m_UseVTKProperty = false;};

protected:
  vtkPolyDataMapper	      *m_Mapper;
	vtkLineSource						*m_Arrow;
	vtkPolyDataNormals			*m_Normal;
	vtkGlyph3D							*m_NormalGlyph;
	vtkPolyDataMapper				*m_NormalMapper;
	vtkActor								*m_NormalActor;
	vtkCellCenters					*m_CenterPointsFilter;
	vtkArrowSource					*m_NormalArrow;
  vtkActor                *m_Actor; ///< Actor representing the polygonal surface

  mafLODActor             *m_OutlineActor;
  mafAxes                 *m_Axes;
	vtkFeatureEdges					*m_ExtractEdges;
	vtkPolyDataMapper				*m_EdgesMapper;
	vtkActor								*m_EdgesActor;

  int m_UseVTKProperty; ///< Flag to switch On/Off the VTK property usage to color the surface
  int m_UseLookupTable; ///< Flag to switch On/Off the lookup table usage to color the surface
  int m_EnableActorLOD; ///< Flag to switch On/Off the usage of the Level Of Detail

  int m_ScalarVisibility;         ///< Flag to switch On/Off the scalar visibility
	int m_NormalVisibility;         ///< Flag to switch On/Off the visibility of normals on the surface
	int m_EdgeVisibility;           ///< Flag to switch On/Off the visibility of edge feature on the surface
  int m_RenderingDisplayListFlag; ///< Flag to switch On/Off the 
  mmaMaterial *m_SurfaceMaterial;
  mafGUIMaterialButton *m_MaterialButton;

  int m_SelectedScalarsArray;           ///< Contains the index of the visivle scalars array of the input surface (added by Losi 2011/04/08 to allow scalars array selection)
  int m_SelectedDataAttribute;          ///< Determine the visible scalars data type (0 for point data, 1 for cell data) (added by Losi 2011/04/08 to allow scalars array selection)
  wxComboBox *m_ScalarsArraySelection ; ///< GUI element for selecting the active scalars array (added by Losi 2011/04/08 to allow scalars array selection)

  //void UpdateProperty(bool fromTag = false);

  /** Create the pipeline to show the edges information on surface.*/
	void CreateEdgesPipe();

  /** Create the pipeline to show the normals of the polydata as arrows centered into the center of the cells.*/
	void CreateNormalsPipe();

  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
  virtual mafGUI  *CreateGui();

  /** Update the scalars array visualization (added by Losi 2011/04/08 to allow scalars array selection) */
  void UpdateScalarsArrayVisualization(vtkDataSetAttributes *dataAttribute);

  /** Return the selected data attribute (point or cell data) (added by Losi 2011/04/08 to allow scalars array selection) */
  vtkDataSetAttributes *GetSelectedDataAttribute();
};  
#endif // __mafPipeSurface_H__
