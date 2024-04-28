# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/tommaso/esp/v5.2.1/esp-idf/components/bootloader/subproject"
  "/home/tommaso/git_repos/esp32_f1/esp32_f1_car/build/bootloader"
  "/home/tommaso/git_repos/esp32_f1/esp32_f1_car/build/bootloader-prefix"
  "/home/tommaso/git_repos/esp32_f1/esp32_f1_car/build/bootloader-prefix/tmp"
  "/home/tommaso/git_repos/esp32_f1/esp32_f1_car/build/bootloader-prefix/src/bootloader-stamp"
  "/home/tommaso/git_repos/esp32_f1/esp32_f1_car/build/bootloader-prefix/src"
  "/home/tommaso/git_repos/esp32_f1/esp32_f1_car/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/tommaso/git_repos/esp32_f1/esp32_f1_car/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/tommaso/git_repos/esp32_f1/esp32_f1_car/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
