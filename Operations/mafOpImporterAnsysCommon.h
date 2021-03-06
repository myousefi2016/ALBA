/*=========================================================================

Program: MAF2
Module: mafOpImporterAnsysCommon.h
Authors: Nicola Vanella

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpImporterAnsysCommon_H__
#define __mafOpImporterAnsysCommon_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafVMEMesh;
class mafEvent;
class wxBusyInfo;
class mafProgressBarHelper;

struct AnsysComponent
{
  std::string Name;
  std::string ElementsFileName;
  int *Ranges;
  int RangeNum;
};

struct AnsysElement 
{
  int Id; 
  int NodesNumber;
  int Type; 
  int MatId;
  int *Nodes;
};

//----------------------------------------------------------------------------
// lhpOpImporterAnsysFile :
//----------------------------------------------------------------------------
/** 
Importer for Ansys Input files
*/
class MAF_EXPORT mafOpImporterAnsysCommon : public mafOp
{
public:

	mafOpImporterAnsysCommon(const wxString &label = "MeshImporter");
	~mafOpImporterAnsysCommon(); 

	virtual void OnEvent(mafEventBase *maf_event);

	/** Return true for the acceptable vme type. */
	bool Accept(mafVME *node);

	/** Set the input Ansys file */
	void SetFileName(const char *ansysInputFileNameFullPath) {m_AnsysInputFileNameFullPath = ansysInputFileNameFullPath;};
	
  /** Get the input Ansys file */
  wxString GetFileName() {return m_AnsysInputFileNameFullPath.c_str();};

  /** Builds operation's interface. */
  void OpRun();

	/** Import the mesh*/
	int Import();

protected:
 
  virtual mafString GetWildcard() = 0;

	virtual int ParseAnsysFile(mafString fileName) = 0;

  int ReadNBLOCK(FILE *outFile);
  int ReadEBLOCK();
  int ReadMPDATA(FILE *outFile);
  int ReadCMBLOCK();

	int GetLine(FILE *fp, char *buffer);
	int ReplaceInString(char *str, char from, char to);
  int ReadInit(mafString &fileName);

  int WriteElements(int part);

  void AddElement(int Id, int nNodes, int type, int matId, int *nodes);
  bool IsInRange(int elemId, int partId);

  void ReadFinalize();

  wxBusyInfo *m_BusyInfo;

	wxString m_AnsysInputFileNameFullPath;

	int m_ImporterType;

	mafString m_CacheDir;
	mafString m_DataDir;

  long m_OperationProgress;
	/** Nodes file name*/
	wxString m_NodesFileName;

	/** Materials file name*/
	wxString m_MaterialsFileName;

	mafProgressBarHelper *m_ProgressHelper;
  
  FILE * m_FilePointer;
	char *m_Buffer;
	int m_BufferLeft;
  int m_BufferPointer;
  char m_Line[512];
  long m_FileSize;
	long m_BytesReaded;
  
	int m_CurrentMatId;  

  std::vector<AnsysElement> m_Elements;
  std::vector<AnsysComponent> m_Components;
};
#endif
