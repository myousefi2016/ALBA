PROJECT (mafCore_TEST)

IF(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafDLL)
ELSE(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafTests mafCore mafVME mafService)
ENDIF (MAF_BUILD_MAFDLL)

# Set your list of sources here.
SET(PROJECT_SRCS 
  mafCoreTests.h mafCoreTests.cpp
	mafViewVTKTest.h mafViewVTKTest.cpp
	mafViewHTMLTest.h mafViewHTMLTest.cpp
	mafAttributeTest.h mafAttributeTest.cpp
	mafTagItemTest.h mafTagItemTest.cpp
	mafTagArrayTest.h mafTagArrayTest.cpp
	mafVMETest.h mafVMETest.cpp
	mafTimeMapTest.h mafTimeMapTest.cpp
	mafVMEFactoryTest.h mafVMEFactoryTest.cpp
	mmaApplicationLayoutTest.h mmaApplicationLayoutTest.cpp
	mafVMETest.h mafVMETest.cpp
	mafOpTest.h mafOpTest.cpp
	mafEventIOTest.h mafEventIOTest.cpp
	mafUserTest.h mafUserTest.cpp
	mafLODActorTest.h mafLODActorTest.cpp
	mafTimeMapScalarTest.h mafTimeMapScalarTest.cpp
	mafOpSelectTest.h mafOpSelectTest.cpp
	mafVMERootTest.h mafVMERootTest.cpp
	mafOpCutTest.h mafOpCutTest.cpp
	mafOpCopyTest.h mafOpCopyTest.cpp
	mafOpPasteTest.h mafOpPasteTest.cpp
	mafOpEditTest.h mafOpEditTest.cpp
	mafOpContextStackTest.h mafOpContextStackTest.cpp
	mafVMEIteratorTest.h mafVMEIteratorTest.cpp
	mafPipeTest.h mafPipeTest.cpp
	mafMatrixPipeTest.h mafMatrixPipeTest.cpp
	mafPipeBoxTest.h mafPipeBoxTest.cpp
	mafVMEOutputTest.h mafVMEOutputTest.cpp
	mafDataPipeTest.h mafDataPipeTest.cpp
	mafPipeFactoryTest.h mafPipeFactoryTest.cpp
	mmaMaterialTest.h mmaMaterialTest.cpp
	mmaVolumeMaterialTest.h mmaVolumeMaterialTest.cpp
	mafSceneNodeTest.h mafSceneNodeTest.cpp
	mafSceneGraphTest.h mafSceneGraphTest.cpp
	mafVMEOutputNULLTest.h mafVMEOutputNULLTest.cpp
	mafVMEStorageTest.h mafVMEStorageTest.cpp
	mafVMEStorageTest2.h mafVMEStorageTest2.cpp
	mafGUITreeContextualMenuTest.h mafGUITreeContextualMenuTest.cpp
	mafAttributeTraceabilityTest.h mafAttributeTraceabilityTest.cpp
	mafAxesTest.h mafAxesTest.cpp
	mafCoreFactoryTest.h mafCoreFactoryTest.cpp
	mafOpStackTest.h mafOpStackTest.cpp
	mafOpManagerTest.h mafOpManagerTest.cpp
	mafViewManagerTest.h mafViewManagerTest.cpp
	mafVMEManagerTest.h mafVMEManagerTest.cpp
	mafAbsMatrixPipeTest.h mafAbsMatrixPipeTest.cpp
	vtkMAFAssemblyTest.h vtkMAFAssemblyTest.cpp
	mafViewPlotTest.h mafViewPlotTest.cpp
	TimeSetTest.h TimeSetTest.cpp
	VMEPoseTest.h VMEPoseTest.cpp
	mafProgressBarHelperTest.h mafProgressBarHelperTest.cpp
)
#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#----------------------------------------------------------------------------

ADD_EXECUTABLE(mafCoreTests ${PROJECT_SRCS})
ADD_TEST(mafCoreTests  ${EXECUTABLE_OUTPUT_PATH}/mafCoreTests)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafCoreTests PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

