# MegaMol
# Copyright (c) 2021, MegaMol Dev Team
# All rights reserved.
#

megamol_plugin(datatools
  BUILD_DEFAULT ON
  DEPENDS_PLUGINS
    mmstd
    cluster
    geometry_calls
    mesh)

if (datatools_PLUGIN_ENABLED)
  find_package(ZeroMQ CONFIG REQUIRED)
  find_package(cppzmq CONFIG REQUIRED)
  find_package(imgui CONFIG REQUIRED)
  find_package(Eigen3 CONFIG REQUIRED)
  find_package(nanoflann CONFIG REQUIRED)
  find_package(simultaneous_sort CONFIG REQUIRED)

  find_path(MMPLD_IO_INCLUDE_DIRS "file_header.h")

  target_link_libraries(datatools
    PRIVATE
      libzmq
      cppzmq
      imgui::imgui
      Eigen3::Eigen
      nanoflann::nanoflann
      simultaneous_sort)

  # Additional sources
  file(GLOB_RECURSE extra_source_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "3rd/min_sphere_of_spheres/*.cpp")
  target_sources(datatools PRIVATE ${extra_source_files})
  target_include_directories(datatools PUBLIC "3rd/min_sphere_of_spheres" PRIVATE ${MMPLD_IO_INCLUDE_DIRS})
  if (MPI_C_FOUND)
    target_link_libraries(datatools PRIVATE MPI::MPI_C)
  endif ()
endif ()
