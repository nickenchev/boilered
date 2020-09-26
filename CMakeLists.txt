cmake_minimum_required(VERSION 3.5)

project(boilered LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(boilered
  src/main.cpp
  src/mainwindow.cpp
  src/samplepart.cpp
  src/mainwindow.h
  src/mainwindow.ui
  src/renderview.h
  src/samplepart.h
)

add_subdirectory(boiler/ libboiler)

target_include_directories(boilered PUBLIC boiler)
target_link_libraries(boilered PRIVATE Qt5::Widgets boiler)
