# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/abrown28/Projects/gameengine/build_web/_deps/entt-src")
  file(MAKE_DIRECTORY "/home/abrown28/Projects/gameengine/build_web/_deps/entt-src")
endif()
file(MAKE_DIRECTORY
  "/home/abrown28/Projects/gameengine/build_web/_deps/entt-build"
  "/home/abrown28/Projects/gameengine/build_web/_deps/entt-subbuild/entt-populate-prefix"
  "/home/abrown28/Projects/gameengine/build_web/_deps/entt-subbuild/entt-populate-prefix/tmp"
  "/home/abrown28/Projects/gameengine/build_web/_deps/entt-subbuild/entt-populate-prefix/src/entt-populate-stamp"
  "/home/abrown28/Projects/gameengine/build_web/_deps/entt-subbuild/entt-populate-prefix/src"
  "/home/abrown28/Projects/gameengine/build_web/_deps/entt-subbuild/entt-populate-prefix/src/entt-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/abrown28/Projects/gameengine/build_web/_deps/entt-subbuild/entt-populate-prefix/src/entt-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/abrown28/Projects/gameengine/build_web/_deps/entt-subbuild/entt-populate-prefix/src/entt-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
