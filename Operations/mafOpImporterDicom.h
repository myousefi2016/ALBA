/*=========================================================================

Program: MAF2
Module: mafOpImporterDicom
Authors: Matteo Giacomoni, Roberto Mucci , Stefano Perticoni, Gianluigi Crimi

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpImporterDicom_H__
#define __mafOpImporterDicom_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"
#include "vtkImageData.h"
#include <map>
#include <vector>
#include "mafGUIWizard.h"
#include "vtkMatrix4x4.h"


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafDicomSlice;
class mafInteractorDICOMImporter;
class mafGUIWizardPageNew;
class mafString;
class mafTagArray;
class mafVME;
class mafVMEImage;
class mafVMEVolumeGray;
class mafGUICheckListBox;
class vtkDirectory;
class vtkLookupTable;
class vtkPlaneSource;
class vtkPolyDataMapper;
class vtkTexture;
class vtkActor;
class vtkActor2D;
class vtkPolyData;
class vtkTextMapper;
class mafProgressBarHelper;
class DcmDataset;

class mafDicomSlice;
class mafDicomSeries;
class mafGUIDicomSettings;
class mafDicomStudyList;
class mafDicomStudy;
namespace gdcm {
	class DataSet;
	class Tag;
	class Image;
	template<uint16_t Group, uint16_t Element,int TVR, int TVM> class Attribute;
}



//----------------------------------------------------------------------------
// mafOpImporterDicomOffis :
//----------------------------------------------------------------------------
/** 
Perform DICOM importer.
From a DICOM dataset return a VME Volume or a VME Image.
*/
class MAF_EXPORT mafOpImporterDicom : public mafOp
{
public:
	/** constructor */
	mafOpImporterDicom(wxString label = "Importer DICOM", bool justOnce = false);
	/** RTTI macro */
	mafTypeMacro(mafOpImporterDicom, mafOp);

	/** Copy. */
	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafVME*node);

	/** Builds operation's interface calling CreateGui() method. */
	virtual void OpRun();

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	virtual void OpStop	(int result);

	/** Execute the operation. */
	virtual  void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo(){};
			
	/** Create the vtkTexture for slice_num Dicom slice: this will be written to m_SliceTexture*/
	void GenerateSliceTexture(int imageID);

	/** Crops the image data  input depending on calculated crop extent*/
	void Crop(vtkImageData *slice);

	/** Calculate crop Extent depending on interactor data */
	void CalculateCropExtent();
			
	/** Build a volume from the list of Dicom files. */
	int BuildVMEVolumeGrayOutput();
		
	/** Build images starting from the list of Dicom files. */
	int BuildVMEImagesOutput();

	/** Create the pipeline to read the images. */
	virtual void CreateSliceVTKPipeline();

	/** Open dir containing Dicom images. */
	bool OpenDir(const char *dirPath);

	/** Get slice Image data*/
	vtkImageData *GetSliceInCurrentSeries(int id);

	/** Import dicom tags into vme tags. */
	void ImportDicomTags();

	/** method allows to handle events from other objects*/
	virtual void OnEvent(mafEventBase *maf_event);
	
	/*Set JustOnce Import Mode*/
	void SetJustOnceImport(bool justOnce) { m_JustOnceImport = justOnce; };
protected:

	enum DICOM_IMPORTER_GUI_ID
	{
		ID_FIRST = mafGUIWizard::ID_LAST,
		ID_STUDY_SELECT,
		ID_SERIES_SELECT,
		ID_CANCEL,
		ID_SCAN_TIME,
		ID_SCAN_SLICE,
		ID_VME_TYPE,
		ID_SHOW_TEXT,
		ID_UPDATE_NAME
	};

	/** OnEvent helper functions */
	void OnChangeSlice();
	void OnRangeModified();
	void OnWizardChangePage( mafEvent * e );
	void SelectSeries(mafDicomSeries * selectedSeries);
	void OnStudySelect();
	
	/** Create load page and his GUI for the wizard. */
	void CreateLoadPage();

	/** Create crop page and   his GUI for the wizard. */
	void CreateCropPage();
		
	/** Reset the slider that allow to scan the slices. */
	void CreateSliders();

	/** Load Dicom in folder */
	bool LoadDicomFromDir(const char *dicomDirABSPath);

	/** Read the list of Dicom files recognized. */
	mafDicomSlice *ReadDicomFile(mafString fileName);

	/** Get the value of a tag inside a dicom dataset. */
	template <uint16_t A, uint16_t B> double GetAttributeValue(gdcm::DataSet &dcmDataSet);

	/** Return the slice number from the heightId and sliceId*/
	int GetSliceIDInSeries(int heightId, int timeId);

	/** Show the slice slice_num. */
	void SetPlaneDims();

	/** Fill Study listbox. */
	void FillStudyListBox();
		
	/** Fill Series listbox. */
	void FillSeriesListBox();
		
	/** Perform gui update in several wizard pages */
	void GuiUpdate();

	/** Update camera. */
	void CameraUpdate();

	/** reset camera. */
	void CameraReset();

	/** On page changing. */
	void OnWizardPageChanging(){};

	/** On wizard start. */
	virtual int RunWizard();
	
	/** Sets the VME name*/
	void SetVMEName();

	mafVME *FindVolumeInTree(mafVME *node);
		
	vtkLookupTable		*m_SliceLookupTable;
	vtkPlaneSource		*m_SlicePlane;
	vtkPolyDataMapper	*m_SliceMapper;
	vtkTexture				*m_SliceTexture;
	vtkActor					*m_SliceActor;
	vtkImageData			*m_CurrentSliceID;
	
	// text stuff
	vtkActor2D    *m_TextActor;
	vtkTextMapper	*m_TextMapper;

	mafInteractorDICOMImporter *m_DicomInteractor;

	mafGUIWizard			*m_Wizard;
	mafGUIWizardPageNew	*m_LoadPage;
	mafGUIWizardPageNew	*m_CropPage;
	
	mafGUI	*m_LoadGuiLeft;
	mafGUI	*m_LoadGuiUnderLeft;
	mafGUI	*m_CropGuiLeft;
	mafGUI	*m_CropGuiCenter;
	mafGUI	*m_LoadGuiCenter;
	mafGUI  *m_LoadGuiUnderCenter;

	int       m_OutputType;
	int			m_DescrInName;
	int			m_PatientNameInName;
	int			m_SizeInName;
	mafString m_VMEName;

	mafDicomStudyList *m_StudyList;
	mafDicomSeries	*m_SelectedSeries; ///< Selected series slices list
	
	/**Get dicom settings*/
	mafGUIDicomSettings* GetSetting();

	wxListBox	*m_StudyListbox;
	wxListBox *m_SeriesListbox;
	int m_SelectedStudy;
	
	int           m_ZCropBounds[2];
	int						m_CurrentSlice;
	wxSlider		 *m_SliceScannerLoadPage;
	wxSlider		 *m_SliceScannerCropPage;
	
	int						m_CurrentTime;
	
	mafTagArray	*m_TagArray;

	double	m_SliceBounds[6];
	bool m_CropEnabled;
	int m_CropExtent[6];

	bool m_ConstantRotation;
	
	int m_ShowOrientationPosition;
	int m_CurrentImageID;

	bool m_JustOnceImport;

	double m_SliceRange[2]; ///< contains the scalar range og the full dicom
	double m_SliceSubRange[2]; ///< contains the scalar range og the full dicom

	/** destructor */
	~mafOpImporterDicom();
};


//----------------------------------------------------------------------------
// mafDicomStudyList:
//----------------------------------------------------------------------------

class mafDicomStudyList
{
public:
	~mafDicomStudyList();

	/** Add a slice, if necessary creates a new study/series*/
	void AddSlice(mafDicomSlice *slice);

	/** Get the num-th study*/
	mafDicomStudy *GetStudy(int num);

	/** Returns the number of studies*/
	int GetStudiesNum() { return m_Studies.size(); }

	/** Returns the total number of series*/
	int GetSeriesTotalNum();
protected:
	std::vector<mafDicomStudy *> m_Studies;
};

//----------------------------------------------------------------------------
// mafDicomStudy:
//----------------------------------------------------------------------------

class mafDicomStudy
{
public:

	mafDicomStudy(mafString studyID) { m_StudyID = studyID; } 

	~mafDicomStudy();

	/** Add a slice, if necessary creates a new series */
	void AddSlice(mafDicomSlice *slice);
	
	/** Returns StudyID */
	mafString GetStudyID() const { return m_StudyID; }

	/** Returns the number of series */
	int GetSeriesNum() { return m_Series.size(); }

	/** Gets the id-th series */
	mafDicomSeries *GetSeries(int id) { return m_Series[id]; }

protected:
	mafString m_StudyID;
	std::vector<mafDicomSeries *> m_Series;
};

//----------------------------------------------------------------------------
// mafDicomSeries:
//----------------------------------------------------------------------------

class mafDicomSeries
{
public:

	mafDicomSeries(mafString seriesID) { m_SeriesID = seriesID;  m_IsRotated = false; m_CardiacImagesNum = 0; }

	~mafDicomSeries();

	/** Add a slice */
	void AddSlice(mafDicomSlice *slice);

	/** Gets the id-th slice */
	mafDicomSlice *GetSlice(int id) { return m_Slices[id]; }

	/** Returns the number of series */
	int GetSlicesNum() { return m_Slices.size(); }

	/** Returns IsRotated */
	bool IsRotated() const { return m_IsRotated; }
	
	/** Returns Slices vector */
	std::vector<mafDicomSlice *> GetSlices() const { return m_Slices; }

	/** Returns SerieID */
	mafString GetSerieID() const { return m_SeriesID; }

	/** Return the slices dims */
	const int *GetDimensions() { return m_Dimensions; }

	/** Returns FramesNum */
	int GetCardiacImagesNum() const { return m_CardiacImagesNum; }

	/** Sort slices internally */
	void SortSlices();
		
protected:

	/** Check if Dicom dataset contains rotations */
	bool IsRotated(const double dcmImageOrientationPatient[6]);

	std::vector<mafDicomSlice *> m_Slices;
	int m_Dimensions[3];
	mafString m_SeriesID;
	bool m_IsRotated;
	int m_CardiacImagesNum;
};


//----------------------------------------------------------------------------
// mafDicomSlice:
//----------------------------------------------------------------------------
/**
class name: mafDicomSlice
Holds information on a single dicom slice
*/
class MAF_EXPORT mafDicomSlice
{
public:

	/** Constructor */
	mafDicomSlice(mafString sliceABSFilename, double dcmImageOrientationPatient[6], double dcmImagePositionPatient[3], mafString description, mafString date,
								mafString patientName, mafString patientBirthdate, int dcmCardiacNumberOfImages = -1, double dcmTtriggerTime = -1.0)
	{
		m_PatientBirthdate = patientBirthdate;
		m_PatientName = patientName;
		m_Description = description;
		m_Date = date;
		m_SliceABSFileName = sliceABSFilename;
		m_NumberOfCardiacImages = dcmCardiacNumberOfImages;
		m_TriggerTime = dcmTtriggerTime;
		SetDcmImageOrientationPatient(dcmImageOrientationPatient);
		SetImagePositionPatient(dcmImagePositionPatient);
	};

	/** destructor */
	~mafDicomSlice() {};

	/** Return patient birthday */
	mafString GetPatientBirthday() { return m_PatientBirthdate; };

	/** Return patient name */
	mafString GetPatientName() { return m_PatientName; };

	/** Return the filename of the corresponding Dicom slice. */
	const char *GetSliceABSFileName() const { return m_SliceABSFileName.GetCStr(); };

	/** Set the filename of the corresponding Dicom slice. */
	void SetSliceABSFileName(char *fileName) { m_SliceABSFileName = fileName; };
		
	/** Return the number of cardiac time frames */
	int GetNumberOfCardiacImages() const { return m_NumberOfCardiacImages; };

	/** Set the number of cardiac time frames */
	void SetNumberOfCardiacImages(int number) { m_NumberOfCardiacImages = number; };

	/** Return the trigger time of the Dicom slice */
	double GetTriggerTime() const { return m_TriggerTime; };

	/** Set the trigger time of the Dicom slice*/
	void SetTriggerTime(double time) { m_TriggerTime = time; };

	/* Gets the Dicom spacing by reading tags */
	void GetDicomSpacing(gdcm::DataSet &dcmDataSet, double * dcmPixelSpacing);

	/** Retrieve image data */
	vtkImageData* GetNewVTKImageData();
	
	/** Set the DcmImageOrientationPatient tag for the slice */
	void SetDcmImageOrientationPatient(double dcmImageOrientationPatient[6])
	{
		m_ImageOrientationPatient[0] = dcmImageOrientationPatient[0];
		m_ImageOrientationPatient[1] = dcmImageOrientationPatient[1];
		m_ImageOrientationPatient[2] = dcmImageOrientationPatient[2];
		m_ImageOrientationPatient[3] = dcmImageOrientationPatient[3];
		m_ImageOrientationPatient[4] = dcmImageOrientationPatient[4];
		m_ImageOrientationPatient[5] = dcmImageOrientationPatient[5];
		ComputeUnrotatedOrigin();
	};

	/** Get the DcmImageOrientationPatient tag for the slice */
	void GetDcmImageOrientationPatient(double dcmImageOrientationPatient[6])
	{
		dcmImageOrientationPatient[0] = m_ImageOrientationPatient[0];
		dcmImageOrientationPatient[1] = m_ImageOrientationPatient[1];
		dcmImageOrientationPatient[2] = m_ImageOrientationPatient[2];
		dcmImageOrientationPatient[3] = m_ImageOrientationPatient[3];
		dcmImageOrientationPatient[4] = m_ImageOrientationPatient[4];
		dcmImageOrientationPatient[5] = m_ImageOrientationPatient[5];
	};

	/** Get the DcmImageOrientationPatient tag for the slice */
	const double *GetDcmImageOrientationPatient() { return m_ImageOrientationPatient; }

	/** Return the position unrotated. 
	The origin is set to the output to this value,
	when the rotation matrix is applied the origin will be placed on the "right origin",
	unrotated origin is used also to sort slices depending on unrotated Z value */
	double *GetUnrotatedOrigin() { return m_UnrotatedOrigin; }
	
	/** return the description */
	mafString GetDescription() { return m_Description; };

	/** return the date */
	mafString GetDate() { return m_Date; };

	/** Get the Modality */
	mafString GetModality() { return m_Modality; };

	/** Set the DCM modality */
	void SetModality(mafString dcmModality) { m_Modality = dcmModality; };
	
	/** Returns PhotometricInterpretation */
	mafString GetPhotometricInterpretation() const { return m_PhotometricInterpretation; }

	/** Sets PhotometricInterpretation */
	void SetPhotometricInterpretation(mafString photometricInterpretation) { m_PhotometricInterpretation = photometricInterpretation; }

	/** Returns SeriesID */
	mafString GetSeriesID() const { return m_SeriesID; }

	/** Sets SeriesID */
	void SetSeriesID(mafString seriesID) { m_SeriesID = seriesID; }
	
	/** Returns StudyID */
	mafString GetStudyID() const { return m_StudyID; }

	/** Sets StudyID */
	void SetStudyID(mafString studyID) { m_StudyID = studyID; }
	
	/** Returns ImagePositionPatient */
	void GetImagePositionPatient(double imagePositionPatient[3]) 
	{
		imagePositionPatient[0] = m_ImagePositionPatient[0];
		imagePositionPatient[1] = m_ImagePositionPatient[1];
		imagePositionPatient[2] = m_ImagePositionPatient[2];

	}

	/** Sets ImagePositionPatient */
	void SetImagePositionPatient(double imagePositionPatient[3]) {
		m_ImagePositionPatient[0] = imagePositionPatient[0];
		m_ImagePositionPatient[1] = imagePositionPatient[1];
		m_ImagePositionPatient[2] = imagePositionPatient[2];
		ComputeUnrotatedOrigin();
	}
	
	/** Returns SliceSize */
	int *GetSliceSize() { return m_SliceSize; }

	/** Sets SliceSize */
	void SetSliceSize(int *sliceSize) { m_SliceSize[0] = sliceSize[0]; m_SliceSize[1] = sliceSize[1]; }

protected:

	/** Compute VTK Scalar Type from Dicom image*/
	int ComputeVTKScalarType(int scalarType);

	/** Compute the unrotated origin */
	void ComputeUnrotatedOrigin();

	double m_UnrotatedOrigin[3];
	double m_ImagePositionPatient[3];
	double m_ImageOrientationPatient[6];
	mafString m_SliceABSFileName;
	mafString m_Description;
	mafString m_Date;
	mafString m_PatientName;
	mafString m_PatientBirthdate;
	mafString m_Modality;
	mafString m_PhotometricInterpretation;
	mafString m_SeriesID;
	mafString m_StudyID;

	double m_TriggerTime;
	int m_NumberOfCardiacImages;
	int m_SliceSize[2];
};

#endif
