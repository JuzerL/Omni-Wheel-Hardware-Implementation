cmake_minimum_required(VERSION 3.8)
project(omni_robot_bringup)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# -----------------------
# Dependencies
# -----------------------
find_package(ament_cmake REQUIRED)

# -----------------------
# Install directories
# -----------------------

# Install config files
install(
  DIRECTORY config/
  DESTINATION share/${PROJECT_NAME}/config
)

# Install URDF / description files
install(
  DIRECTORY description/
  DESTINATION share/${PROJECT_NAME}/description
)

# Install launch files
install(
  DIRECTORY launch/
  DESTINATION share/${PROJECT_NAME}/launch
)

# -----------------------
# Ament package
# -----------------------
ament_package()
