# CMakeのバージョンを設定
cmake_minimum_required(VERSION 3.16.3)
# プロジェクト名と使用する言語を設定
project(mmc CXX)

option(STATIC_LIBS "build as a static library" OFF)
if (STATIC_LIBS)
  add_library(mmc STATIC mmc.cc)
else()
  add_library(mmc SHARED mmc.cc)
endif()