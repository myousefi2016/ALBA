/*=========================================================================

Program: MAF2
Module: mafOpImporterAnsysCommon.cpp
Authors: Nicola Vanella

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

#include "mafOpImporterAnsysCommon.h"

#include "mafDecl.h"
#include "mafGUI.h"
#include "mafSmartPointer.h"
#include "mafTagItem.h"
#include "mafTagArray.h"
#include "mafVME.h"
#include "mafVMEMeshAnsysTextImporter.h"
#include "mafProgressBarHelper.h"

#include "vtkMAFSmartPointer.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include "wx/filename.h"
#include "wx/stdpaths.h"
#include "wx/busyinfo.h"

//buffer size 1024*1024 
#define READ_BUFFER_SIZE 1048576 
#define min(a,b)  (((a) < (b)) ? (a) : (b))

//----------------------------------------------------------------------------
mafOpImporterAnsysCommon::mafOpImporterAnsysCommon(const wxString &label) :
mafOp(label)
{ 
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_ImporterType = 0;
  m_Output=NULL;

  m_NodesFileName = "";
  m_MaterialsFileName = "";

	wxStandardPaths std_paths;
	wxString userPath=std_paths.GetUserDataDir();

	m_DataDir = userPath + "\\Data";
  m_CacheDir = m_DataDir + "\\AnsysReaderCache";
  m_AnsysInputFileNameFullPath		= "";
  
  m_BusyInfo = NULL;
}

//----------------------------------------------------------------------------
mafOpImporterAnsysCommon::~mafOpImporterAnsysCommon()
{
	for (int e = 0; e<m_Elements.size(); e++)
	{
		delete[] m_Elements[e].Nodes;
	}
	m_Elements.clear();

	for (int c = 0; c<m_Components.size(); c++)
  {
    if(m_Components[c].Ranges)
      delete[] m_Components[c].Ranges;
  }
  m_Components.clear();
}
//----------------------------------------------------------------------------
bool mafOpImporterAnsysCommon::Accept(mafVME *node)
{
  return true;
}

//----------------------------------------------------------------------------
void mafOpImporterAnsysCommon::OpRun()   
{  
  mafString wildcard = GetWildcard();

  int result = OP_RUN_CANCEL;
  m_AnsysInputFileNameFullPath = "";

  wxString f;
  f = mafGetOpenFile("", wildcard).c_str(); 
  if(!f.IsEmpty() && wxFileExists(f))
  {
    m_AnsysInputFileNameFullPath = f;
    Import();
    result = OP_RUN_OK;
  }
  mafEventMacro(mafEvent(this,result));  
}

//----------------------------------------------------------------------------
void mafOpImporterAnsysCommon::OnEvent(mafEventBase *maf_event)
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case wxOK:
      {
        this->Import();
        this->OpStop(OP_RUN_OK);
      }
      break;
    case wxCANCEL:
      {
        this->OpStop(OP_RUN_CANCEL);
      }
      break;
    default:
      mafEventMacro(*e);
      break;
    }	
  }
}
//----------------------------------------------------------------------------
int mafOpImporterAnsysCommon::Import()
{
  m_NodesFileName = m_CacheDir + "\\nodes.lis" ;
  m_MaterialsFileName = m_CacheDir + "\\materials.lis" ;

  // Create tmp path
  mkdir(m_DataDir);
	
	if(!wxDirExists(m_DataDir))
	{
		mafLogMessage("Cloud not create \"Data\" Directory");
		return MAF_ERROR;
	}

	mkdir(m_CacheDir);
	if(!wxDirExists(m_DataDir))
	{
		mafLogMessage("Cloud not create Read Cache Directory");
		return MAF_ERROR;
	}
  
  // Parsing Ansys File
  if(ParseAnsysFile(m_AnsysInputFileNameFullPath.c_str()) == MAF_ERROR)
  {
    return MAF_ERROR;
  }

  if (GetTestMode() == false)
  {
    m_BusyInfo = new wxBusyInfo("Please wait importing VME Mesh AnsysText...");
  }

  wxString name, path, ext;
  wxFileName::SplitPath(m_AnsysInputFileNameFullPath, &path, &name, &ext);

  int returnValue = MAF_OK;  
  
  if(m_Components.size()==0)
  {
    // Create dafault component
    AnsysComponent comp;
    comp.Name=name;
    comp.RangeNum=1;
    comp.Ranges=NULL;

    m_Components.push_back(comp);
  }

  for (int c=0; c<m_Components.size(); c++)
  {
    mafVMEMeshAnsysTextImporter *reader = new mafVMEMeshAnsysTextImporter;
    reader->SetNodesFileName(m_NodesFileName.c_str());
    reader->SetMaterialsFileName(m_MaterialsFileName.c_str());
    WriteElements(c);
    reader->SetElementsFileName(m_Components[c].ElementsFileName.c_str());

	  returnValue = reader->Read();

    if (returnValue == MAF_ERROR)
    {
      if (!m_TestMode)
        mafMessage(_("Error parsing input files! See log window for details..."),_("Error"));
		  else
			  printf("Error parsing input files!");
    } 
    else if (returnValue == MAF_OK)
    {
			mafVMEMesh *importedVmeMesh;
      mafNEW(importedVmeMesh);

      importedVmeMesh->SetName(m_Components[c].Name.c_str());
	    importedVmeMesh->SetDataByDetaching(reader->GetOutput()->GetUnstructuredGridOutput()->GetVTKData(),0);

      mafTagItem tag_Nature;
      tag_Nature.SetName("VME_NATURE");
      tag_Nature.SetValue("NATURAL");
      importedVmeMesh->GetTagArray()->SetTag(tag_Nature);

      importedVmeMesh->ReparentTo(m_Input);
      importedVmeMesh->Update();

			mafDEL(importedVmeMesh);
    }
    delete reader;
  }
  
  if (GetTestMode() == false)
  {
    cppDEL(m_BusyInfo);
  }
 
  return returnValue;
}

//----------------------------------------------------------------------------
int mafOpImporterAnsysCommon::ReadNBLOCK(FILE *outFile)
{
  char blockName[254];
  int maxId = 0, numElements = 0, nodeId, nodeSolidModelEntityId, nodeLine;
  double nodeX, nodeY, nodeZ;

  // NBLOCK,6,SOLID,   2596567,    25   or
  // NBLOCK,6,SOLID
  int nReaded = sscanf(m_Line, "%s %d, %d", blockName, &maxId, &numElements);

  GetLine(m_FilePointer, m_Line); // (3i8,6e20.13) Line ignored

  if(nReaded == 1)
  {
    // CDB from Hypermesh
    while (GetLine(m_FilePointer, m_Line) != 0 && strncmp (m_Line,"N,R5",4) != 0 && strncmp (m_Line,"-1",2) != 0)
    {
      nodeId = nodeSolidModelEntityId = nodeLine = 0;
      nodeX = nodeY = nodeZ = 0;

      //15239 0 0 112.48882247215 -174.4868225037 -378.3886770441 0.0 0.0 0.0
      int nReaded = sscanf(m_Line, "%d %d %d %lf %lf %lf", &nodeId, &nodeSolidModelEntityId, &nodeLine, &nodeX, &nodeY, &nodeZ);
			if (nReaded < 2)
				break;

      fprintf(outFile,"%d\t%.13E\t%.13E\t%.13E\n", nodeId, nodeX, nodeY, nodeZ);
    }
  }
  else
  {
    // CDB from Ansys
    for (int i = 0; i < numElements; i++)
    {
      if(GetLine(m_FilePointer, m_Line) != 0)
      {
        nodeId = nodeSolidModelEntityId = nodeLine = 0;
        nodeX = nodeY = nodeZ = 0;

        sscanf(m_Line, "%d %d %d %lf %lf %lf", &nodeId, &nodeSolidModelEntityId, &nodeLine, &nodeX, &nodeY, &nodeZ);
        fprintf(outFile,"%d\t%.13E\t%.13E\t%.13E\n", nodeId, nodeX, nodeY, nodeZ);
      }
    } 
  }

  return MAF_OK;
}
//----------------------------------------------------------------------------
int mafOpImporterAnsysCommon::ReadEBLOCK()
{
  char blockName[254];
  int maxId = 0, numElements = 0;
  int idMaterial,idTypeElement,idConstants,nNodes, elementNumber, nodes[16], unused;

  // EBLOCK,19,SOLID,   3436163,    234932   / EBLOCK,19,SOLID,   3436163     or 
  // EBLOCK,19,SOLID,
  int nReaded = sscanf(m_Line, "%s %d, %d", blockName, &maxId, &numElements);

  GetLine(m_FilePointer, m_Line); //(19i8) Line ignored
  
  if(nReaded < 3)
  {
    // CDB from Hypermesh
    while (GetLine(m_FilePointer, m_Line) != 0 )
    {
      int readedElem=sscanf(m_Line, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", &idMaterial,&idTypeElement,&idConstants,&unused,&unused,&unused,&unused,&unused,&nNodes,&unused,&elementNumber,nodes+0,nodes+1,nodes+2,nodes+3,nodes+4,nodes+5,nodes+6,nodes+7);

      if (readedElem < 2)
        break;

      if(nNodes > 8)
      {
        GetLine(m_FilePointer, m_Line);
        sscanf(m_Line, "%d %d %d %d %d %d %d %d", nodes+8,nodes+9,nodes+10,nodes+11,nodes+12,nodes+13,nodes+14,nodes+15);
      }

      AddElement(elementNumber, nNodes, idTypeElement, idMaterial, nodes);
    }
  }
  else
  {
    // CDB from Ansys
    for (int index = 0; index < numElements; index++)
    {
      if(GetLine(m_FilePointer, m_Line) != 0)
      {
        sscanf(m_Line, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", &idMaterial,&idTypeElement,&idConstants,&unused,&unused,&unused,&unused,&unused,&nNodes,&unused,&elementNumber,nodes+0,nodes+1,nodes+2,nodes+3,nodes+4,nodes+5,nodes+6,nodes+7);

        if(nNodes > 8)
        {
          GetLine(m_FilePointer, m_Line);
          sscanf(m_Line, "%d %d %d %d %d %d %d %d", nodes+8,nodes+9,nodes+10,nodes+11,nodes+12,nodes+13,nodes+14,nodes+15);
        }

        AddElement(elementNumber, nNodes, idTypeElement, idMaterial, nodes);
      }
    } 
  }  

  return MAF_OK;
}
//----------------------------------------------------------------------------
int mafOpImporterAnsysCommon::ReadMPDATA(FILE *outFile)
{
  char matName[254], unusedStr[254], matValue[254];
  int matId;

  //MPDATA,EX  ,2,1,          1000.0  or MP,EX  ,2,1,          1000.0
  //MPDATA,R5.0, 1,EX  ,       1, 1,  26630.9000 
  int commaNum = ReplaceInString(m_Line, ',', ' ');

  if(commaNum <= 4)
  {
    //MPDATA EX   2 1           1000.0  or //MP EX   2 1           1000.0   
    sscanf(m_Line, "%s %s %d %s", unusedStr, matName, &matId, matValue);
  }
  else
  {
    //MPDATA R5.0  1 EX          1  1   26630.9000    
    sscanf(m_Line, "%s %s %s %s %d %s %s", unusedStr, unusedStr, unusedStr, matName, &matId, unusedStr, matValue);
  }

  if(matId != m_CurrentMatId)
  {  
    if(m_CurrentMatId != -1)
      fprintf(outFile,"\n");

    fprintf(outFile,"MATERIAL NUMBER =      %d EVALUATED AT TEMPERATURE OF   0.0  \n", matId);
    m_CurrentMatId = matId;
  }

  fprintf(outFile,"%s = %s\n", matName, matValue);

  return MAF_OK; 
}
//----------------------------------------------------------------------------
int mafOpImporterAnsysCommon::ReadCMBLOCK()
{
  char compHeader[254], unusedStr[254];
  int compNum;

  // CMBLOCK,NAME,ELEM,       2  ! comment OR
  // CMBLOCK,NAME,NODE,       100  ! comment
  sscanf(m_Line, "%s %d %s", compHeader, &compNum, unusedStr);

  std::string line = compHeader;
  std::string name = line.substr(8).c_str();
  std::size_t pos = name.find(",");

  std::string compName = name.substr(0,pos).c_str();
  std::string compType = name.substr(pos+1, 5).c_str();

  if(compType== "ELEM,")
  {
    wxString fname, fpath, fext;
    wxFileName::SplitPath(m_AnsysInputFileNameFullPath, &fpath, &fname, &fext);

    // Create Component
    AnsysComponent comp;
    comp.Name = fname + "-" + compName.c_str();

    GetLine(m_FilePointer, m_Line); // (8i10) Line ignored

    // Create range array
    int *idList = new int[compNum];

    int nReaded=0, totReaded=0;
    int value;
    char *linePointer;

    GetLine(m_FilePointer, m_Line);
    {
      linePointer=m_Line;

      for (int i=0; i<compNum; i++)
      {
        nReaded = sscanf(linePointer, "%d", &value);
        if(value<0) value*=-1;
        idList[i] = value;
        linePointer+=10;
      }
    }

    comp.Ranges=idList;
    comp.RangeNum=compNum;

    m_Components.push_back(comp);
  }

  return MAF_OK; 
}

//----------------------------------------------------------------------------
int mafOpImporterAnsysCommon::WriteElements(int comp)
{
  mafString elementsfileName = m_CacheDir + "\\elements_part"<<comp<<".lis";
  m_Components[comp].ElementsFileName = elementsfileName;

  FILE* elementsFile = fopen(elementsfileName, "w");
  if (!elementsFile)
  {
    mafLogMessage("Cannot Open: %s",elementsfileName);
    return MAF_ERROR;
  }

  int elemId, matId = 0, elemType = 0, idConstants = 0, nNodes, nodes[16];

  for (int e=0; e<m_Elements.size(); e++)
  {
    AnsysElement myElem = m_Elements[e];

    if(m_Components[comp].RangeNum==1 || IsInRange(myElem.Id, comp))
    {
      elemId = myElem.Id;
      matId = myElem.MatId;
      elemType = myElem.Type;
      nNodes = myElem.NodesNumber;

      fprintf(elementsFile,"%d\t%d\t%d\t%d\t%d\t%d", elemId,matId,elemType,idConstants,0,1);

      for (int j=0;j<nNodes;j++)
      {
        fprintf(elementsFile,"\t%d",myElem.Nodes[j]);
      }

      fprintf(elementsFile,"\n");
    }
  }
  
  fclose(elementsFile);
  return MAF_OK;
}

//----------------------------------------------------------------------------
void mafOpImporterAnsysCommon::AddElement(int Id, int nNodes, int type, int matId, int *nodes)
{
  AnsysElement elem;

  elem.Id=Id;
  elem.NodesNumber=nNodes;
  elem.MatId=matId;
  elem.Type=type;

  int *newNodes=new int[nNodes];

  memcpy(newNodes,nodes,nNodes*sizeof(int));
  elem.Nodes=newNodes;  

  m_Elements.push_back(elem);
}
//----------------------------------------------------------------------------
bool mafOpImporterAnsysCommon::IsInRange(int elemId, int partId)
{
  AnsysComponent comp = m_Components[partId];

  for (int i=0; i<comp.RangeNum/2; i++)
  {
    if(elemId>=comp.Ranges[(i*2)] && elemId<=comp.Ranges[(i*2)+1]) 
      return true;
  }

  return false;
}

//----------------------------------------------------------------------------
int mafOpImporterAnsysCommon::GetLine(FILE *fp, char *lineBuffer)
{
  char readValue;
  int readedChars=0;

  do 
  {
		if (m_BufferLeft == 0)
		{
			m_BufferLeft = fread(m_Buffer,sizeof(char),READ_BUFFER_SIZE,m_FilePointer);
			m_BufferPointer = 0;
			//Breaks if EOF is reached
			if(m_BufferLeft==0)
				break;
		}

		lineBuffer[readedChars]=readValue=m_Buffer[m_BufferPointer];
		readedChars++;
		m_BufferPointer++;
		m_BufferLeft--;
  } while (readValue != '\n');

  lineBuffer[readedChars]=0;

	//Windows translate CR/LF into CR char we need to count a char more for each newline
	m_BytesReaded+=readedChars+1; 
  m_ProgressHelper->UpdateProgressBar(((double)m_BytesReaded) * 100 / m_FileSize);

  return readedChars;
}
//----------------------------------------------------------------------------
int mafOpImporterAnsysCommon::ReplaceInString(char *str, char from, char to)
{
  int count=0;

  for (int i=0; str[i]!= '\0'; i++)
  {
    if (str[i] == from)
    {
      str[i]=to;
      count++;
    }
  }

  return count;
}
//----------------------------------------------------------------------------
int mafOpImporterAnsysCommon::ReadInit(mafString &fileName)
{
	m_FilePointer = fopen(fileName.GetCStr(), "r");

	if (m_FilePointer == NULL)
	{
		if(GetTestMode()==false)
			mafMessage(_("Error parsing input files! File not found."),_("Error"));
		else 
			printf("Error parsing input files! File not found.\n");
		return MAF_ERROR;
	}

	// Calculate file size
	fseek(m_FilePointer, 0L, SEEK_END);
	m_FileSize = ftell(m_FilePointer);
	fseek(m_FilePointer, 0L, SEEK_SET);

	m_Buffer=new char[READ_BUFFER_SIZE];
	m_BytesReaded = m_BufferLeft = m_BufferPointer = 0;
}
//----------------------------------------------------------------------------
void mafOpImporterAnsysCommon::ReadFinalize()
{
	delete [] m_Buffer;
	fclose(m_FilePointer);
}
