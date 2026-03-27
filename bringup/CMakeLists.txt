cmake_minimum_required(VERSION 3.8)
project(omni_robot_bringup)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# -----------------------
# Dependencies
# -----------------------
find_package(ament_cmake REQUIRED)
# We don't 'link' to these, but we 'use' them in our launch/urdf
find_package(urdf REQUIRED)
find_package(xacro REQUIRED)

# -----------------------
# Install directories
# -----------------------

# Install config, description, and launch folders
# Using the 'PATTERN' argument ensures we don't accidentally copy hidden DS_Store or backup files
install(
  DIRECTORY config description launch
  DESTINATION share/${PROJECT_NAME}
)

# -----------------------
# Ament package
# -----------------------
# This macro tells ROS 2 "This is a valid package, index it!"
ament_package()
