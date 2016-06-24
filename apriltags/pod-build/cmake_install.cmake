# Install script for directory: /home/barton/Documents/aptags/apriltags

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/barton/Documents/aptags/apriltags/build")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/barton/Documents/aptags/apriltags/pod-build/lib/libapriltags.a")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/AprilTags" TYPE FILE FILES
    "/home/barton/Documents/aptags/apriltags/AprilTags/Tag25h7.h"
    "/home/barton/Documents/aptags/apriltags/AprilTags/Tag16h5.h"
    "/home/barton/Documents/aptags/apriltags/AprilTags/TagDetector.h"
    "/home/barton/Documents/aptags/apriltags/AprilTags/Gaussian.h"
    "/home/barton/Documents/aptags/apriltags/AprilTags/Quad.h"
    "/home/barton/Documents/aptags/apriltags/AprilTags/Tag25h9.h"
    "/home/barton/Documents/aptags/apriltags/AprilTags/GLineSegment2D.h"
    "/home/barton/Documents/aptags/apriltags/AprilTags/TagFamily.h"
    "/home/barton/Documents/aptags/apriltags/AprilTags/Segment.h"
    "/home/barton/Documents/aptags/apriltags/AprilTags/MathUtil.h"
    "/home/barton/Documents/aptags/apriltags/AprilTags/Homography33.h"
    "/home/barton/Documents/aptags/apriltags/AprilTags/Tag36h11_other.h"
    "/home/barton/Documents/aptags/apriltags/AprilTags/GLine2D.h"
    "/home/barton/Documents/aptags/apriltags/AprilTags/Tag36h9.h"
    "/home/barton/Documents/aptags/apriltags/AprilTags/Tag36h11.h"
    "/home/barton/Documents/aptags/apriltags/AprilTags/FloatImage.h"
    "/home/barton/Documents/aptags/apriltags/AprilTags/Gridder.h"
    "/home/barton/Documents/aptags/apriltags/AprilTags/Edge.h"
    "/home/barton/Documents/aptags/apriltags/AprilTags/pch.h"
    "/home/barton/Documents/aptags/apriltags/AprilTags/Tag16h5_other.h"
    "/home/barton/Documents/aptags/apriltags/AprilTags/TagDetection.h"
    "/home/barton/Documents/aptags/apriltags/AprilTags/XYWeight.h"
    "/home/barton/Documents/aptags/apriltags/AprilTags/GrayModel.h"
    "/home/barton/Documents/aptags/apriltags/AprilTags/UnionFindSimple.h"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/barton/Documents/aptags/apriltags/pod-build/lib/pkgconfig/apriltags.pc")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/barton/Documents/aptags/apriltags/pod-build/example/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/barton/Documents/aptags/apriltags/pod-build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
