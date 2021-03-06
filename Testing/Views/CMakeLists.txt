PROJECT (mafViews_TEST)

IF(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafViews)
ELSE(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafTests GPUAPI mafVME mafVisualPipes mafCommon mafViews mafInteraction mafService)
ENDIF(MAF_BUILD_MAFDLL)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
SET(PROJECT_SRCS 
	mafViewsTests.h mafViewsTests.cpp
  mafViewArbitrarySliceTest.h mafViewArbitrarySliceTest.cpp
	mafViewOrthoSliceTest.h mafViewOrthoSliceTest.cpp
	mafViewRXTest.h mafViewRXTest.cpp
	mafViewRXCTTest.h mafViewRXCTTest.cpp
	mafViewSliceTest.h mafViewSliceTest.cpp
	mafViewGlobalSliceTest.h mafViewGlobalSliceTest.cpp
	mafViewGlobalSliceCompoundTest.h mafViewGlobalSliceCompoundTest.cpp
	mafViewImageTest.h mafViewImageTest.cpp
	mafViewRXCompoundTest.h mafViewRXCompoundTest.cpp
	mafView3DTest.h mafView3DTest.cpp
	mafViewImageCompoundTest.h mafViewImageCompoundTest.cpp
	mafViewCTTest.h mafViewCTTest.cpp
	mafViewSlicerTest.h mafViewSlicerTest.cpp
	mafViewVTKCompoundTest.h mafViewVTKCompoundTest.cpp
	mafViewSliceOnCurveCompoundTest.h mafViewSliceOnCurveCompoundTest.cpp
	mafViewSliceOnCurveTest.h mafViewSliceOnCurveTest.cpp
	mafViewSliceGlobalTest.h mafViewSliceGlobalTest.cpp
	mafViewCompoundWindowingTest.h mafViewCompoundWindowingTest.cpp
	mafViewIsosurfaceCompoundTest.h mafViewIsosurfaceCompoundTest.cpp
)

ADD_EXECUTABLE(mafViewsTests ${PROJECT_SRCS})
ADD_TEST(mafViewsTests  ${EXECUTABLE_OUTPUT_PATH}/mafViewsTests)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafViewsTests PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)