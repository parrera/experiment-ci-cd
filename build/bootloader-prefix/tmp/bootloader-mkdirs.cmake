# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/igor/esp/esp-idf/components/bootloader/subproject"
  "/home/igor/esp/experiment-ci-cd/build/bootloader"
  "/home/igor/esp/experiment-ci-cd/build/bootloader-prefix"
  "/home/igor/esp/experiment-ci-cd/build/bootloader-prefix/tmp"
  "/home/igor/esp/experiment-ci-cd/build/bootloader-prefix/src/bootloader-stamp"
  "/home/igor/esp/experiment-ci-cd/build/bootloader-prefix/src"
  "/home/igor/esp/experiment-ci-cd/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/igor/esp/experiment-ci-cd/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/igor/esp/experiment-ci-cd/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
