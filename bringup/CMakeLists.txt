<?xml version="1.0"?>
<package format="3">
  <name>omni_robot_bringup</name>
  <version>0.0.1</version>
  <description>Bringup package for 4-wheel omni robot</description>

  <maintainer email="you@example.com">Your Name</maintainer>
  <license>Apache-2.0</license>

  <!-- Build tool -->
  <buildtool_depend>ament_cmake</buildtool_depend>

  <!-- Runtime dependencies -->
  <exec_depend>rclcpp</exec_depend>

  <!-- Local packages -->
  <exec_depend>controller_package</exec_depend>
  <exec_depend>robot_hardware</exec_depend>

  <!-- ROS 2 control stack -->
  <exec_depend>ros2_control</exec_depend>
  <exec_depend>controller_manager</exec_depend>

  <!-- Robot description -->
  <exec_depend>xacro</exec_depend>
  <exec_depend>robot_state_publisher</exec_depend>

  <!-- Launch system -->
  <exec_depend>launch</exec_depend>
  <exec_depend>launch_ros</exec_depend>

  <export>
    <build_type>ament_cmake</build_type>
  </export>

</package>
