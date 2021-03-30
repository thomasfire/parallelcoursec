cmake_minimum_required(VERSION 3.17)
project(lab_2 C)

set(CMAKE_C_STANDARD 11)
set(IPPROOT /mnt/wartsila/intel/ipp/latest)

find_library(IPP_CORE libippcore.a PATHS ${IPPROOT}/lib/)
find_library(IPP_VM libippvm.a PATHS ${IPPROOT}/lib/)
find_library(IPP_S libipps.a PATHS ${IPPROOT}/lib/)
find_library(IPP_I libippi.a PATHS ${IPPROOT}/lib/)

add_executable(lab_2 main.c)

target_include_directories(lab_2 PRIVATE ${IPPROOT}/include)
target_link_libraries(lab_2 m ${IPP_CORE} ${IPP_VM} ${IPP_I} ${IPP_S})