/*=========================================================================

 Program: MAF2
 Module: mafViewImage
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafViewImage_H__
#define __mafViewImage_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafViewVTK.h"
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafSceneNode;
class vtkActor2D;
class vtkTextMapper;

//----------------------------------------------------------------------------
// mafViewImage :
//----------------------------------------------------------------------------
/** 
mafViewImage is a View that visualize volume as slices and 
initialize the visual pipe according to the camera position that is passed through constructor
\sa mafViewVTK
*/
class MAF_EXPORT mafViewImage: public mafViewVTK
{
public:
  mafViewImage(wxString label = "Image", int camera_position = CAMERA_FRONT, bool show_axes = false, bool show_grid = false, int stereo = 0);
  virtual ~mafViewImage(); 

  mafTypeMacro(mafViewImage, mafViewVTK);

  virtual mafView*  Copy(mafObserver *Listener, bool lightCopyEnabled = false);
  virtual void			OnEvent(mafEventBase *maf_event);

  void Create();

  /** 
  Set the visualization status for the vme (visible, not visible, mutex, ...) \sa mafSceneGraph mafView*/
  virtual int GetNodeStatus(mafVME *vme);

  /** IDs for the view GUI */
  enum VIEW_SLICE_WIDGET_ID
  {
    ID_NONE = Superclass::ID_LAST,
    ID_LAST
  };
	  
  /** Create the background for the slice. */
  void BorderCreate(double col[3]);

  /** Delete the background of the slice. */
  void BorderDelete();

  void VmeShow(mafVME *vme, bool show);

  void VmeDeletePipe(mafVME *vme);

protected:
  virtual mafGUI *CreateGui();

  mafSceneNode    *m_CurrentImage; ///< Current visualized volume

};
#endif
