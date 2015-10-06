cmake_minimum_required(VERSION 2.6.4)

PROJECT(SimpleRayCaster)

IF (${USE_32BIT})
   # Right now these flags are for gcc but they can be adapted here if necessary
   ADD_DEFINITIONS("-m32")
   SET(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-m32")
ENDIF (${USE_32BIT})


Find_package(GLUT)
Find_package(OpenGL)
INCLUDE(FindGLEW.cmake)

INCLUDE_DIRECTORIES(
   ${OPENGL_INCLUDE_DIR}
   ${GLUT_INCLUDE_DIR}
   ${GLEW_INCLUDE_DIR}
   ${OPENGL_INCLUDE_DIR}
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

set (SHADER_SRC
  Raycast_ScreenAlign_Frag.glsl
  ScreenAlignQuad_Vert.glsl
  shadowVolume_Frag.glsl
  shadowVolume_Vert.glsl
)

set (SAVED_TF
  TransparentSytle1
  SolidStyle1
  RenderStatus #renderStatus
)

ADD_EXECUTABLE(SimpleRayCaster ${RENDER_HEADERS} ${RENDER_SRC})
TARGET_LINK_LIBRARIES(SimpleRayCaster
  ${OPENGL_LIBRARIES}
  ${GLUT_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${OPENGL_LIBRARIES}
)

install(TARGETS SimpleRayCaster DESTINATION ${CMAKE_BINARY_DIR}/bin)
install(FILES ${SHADER_SRC} ${SAVED_TF} DESTINATION ${CMAKE_BINARY_DIR}/bin)
