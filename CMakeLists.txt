cmake_minimum_required(VERSION 2.6.4)

PROJECT(SimpleRayCaster)

IF (${USE_32BIT})
   # Right now these flags are for gcc but they can be adapted here if necessary
   ADD_DEFINITIONS("-m32")
   SET(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-m32") 
ENDIF (${USE_32BIT})


FIND_PACKAGE(GLUT)
INCLUDE(FindGLEW.cmake)

INCLUDE_DIRECTORIES(
   ${OPENGL_INCLUDE_DIR}
   ${GLUT_INCLUDE_DIR}
   ${GLEW_INCLUDE_DIR}	
)

SET (RENDER_HEADERS

  RayCaster.h  
  TransferFunction.h 
  
  RenderUtility/ArcBall.h
  RenderUtility/GL.h
  RenderUtility/GLBufferObject.h
  RenderUtility/GLframebufferObject.h
  RenderUtility/GLSLProgram.h
  RenderUtility/GLTexture.h
  RenderUtility/GLTextureBuffer.h
  RenderUtility/StdDefines.h
  RenderUtility/Vectors.h
  RenderUtility/volume3D.h
  
)

SET (RENDER_SRC
  RayCaster.cpp
  TransferFunction.cpp
  Jet.cpp
  main.cpp
  
  RenderUtility/GLSLProgram.cpp
  RenderUtility/GLBufferObject.cpp
  RenderUtility/GLframebufferObject.cpp
  RenderUtility/GLTexture.cpp
  RenderUtility/ArcBall.cpp
  RenderUtility/GL.cpp
  
)

ADD_EXECUTABLE(SimpleRayCaster ${RENDER_HEADERS} ${RENDER_SRC})  
TARGET_LINK_LIBRARIES(SimpleRayCaster
  ${OPENGL_LIBRARIES}
  ${GLUT_LIBRARIES}
  ${GLEW_LIBRARIES}
) 

