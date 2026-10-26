# Top-level entry point for the whole repo.
#
# In Qt Creator, always open THIS file (File > Open File or Project >
# QT_Data_Visualization/CMakeLists.txt). It just forwards to the real app
# in DesignLayout/, so nobody has to pick between the nested CMakeLists files.
#
# The project name matches the app's own project name so the build target
# keeps the same name (DesignLayoutApp).
cmake_minimum_required(VERSION 3.21.1)

project(DesignLayoutApp LANGUAGES CXX)

add_subdirectory(DesignLayout)
