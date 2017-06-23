PROJECT (Gui_TEST)

# mafGUI e' indipendente da Core, ma i test no -- Silvano 04/04/05
IF(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafDLL)
ELSE(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafTests mafCore mafVME mafCommon mafInteraction mafGui mafService)
ENDIF (MAF_BUILD_MAFDLL)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
SET(PROJECT_SRCS 
	mafGuiTests.h 					mafGuiTests.cpp
	mafAttachCameraTest.h 			mafAttachCameraTest.cpp
	mafGUISettingsAdvancedTest.h 	mafGUISettingsAdvancedTest.cpp
	mafGUIDicomSettingsTest.h 		mafGUIDicomSettingsTest.cpp
	mafGUIWizardTest.h 				mafGUIWizardTest.cpp
	mafGUIWizardPageTest.h 			mafGUIWizardPageTest.cpp
	mafGUILandmarkTest.h 			mafGUILandmarkTest.cpp
	mafGUIWizardPageNewTest.h 		mafGUIWizardPageNewTest.cpp
)

ADD_EXECUTABLE(mafGuiTests ${PROJECT_SRCS})
ADD_TEST(mafGuiTests  ${EXECUTABLE_OUTPUT_PATH}/mafGuiTests)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGuiTests PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)