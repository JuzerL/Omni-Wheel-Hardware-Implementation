add_library(robot_hardware SHARED
  src/omni_hardware.cpp
)

target_include_directories(robot_hardware PUBLIC
  include
)

ament_target_dependencies(robot_hardware
  rclcpp
  hardware_interface
  pluginlib
)

pluginlib_export_plugin_description_file(hardware_interface plugin.xml)

install(TARGETS robot_hardware
  DESTINATION lib
)

install(DIRECTORY include/
  DESTINATION include
)

install(FILES plugin.xml
  DESTINATION share/${PROJECT_NAME}
)
