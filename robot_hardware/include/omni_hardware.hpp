#pragma once

#include <fcntl.h>   // File control
#include <termios.h> // POSIX terminal control
#include <unistd.h>  // UNIX standard functions

#include <vector>
#include <string>

#include "hardware_interface/system_interface.hpp"
#include "rclcpp/rclcpp.hpp"

namespace omni_robot_hardware
{

class OmniSystemHardware : public hardware_interface::SystemInterface
{
public:
  RCLCPP_SHARED_PTR_DEFINITIONS(OmniSystemHardware)

  // Lifecycle
  hardware_interface::CallbackReturn on_init(
    const hardware_interface::HardwareInfo & info) override;

  hardware_interface::CallbackReturn on_configure(
    const rclcpp_lifecycle::State & previous_state) override;

  hardware_interface::CallbackReturn on_activate(
    const rclcpp_lifecycle::State & previous_state) override;

  hardware_interface::CallbackReturn on_deactivate(
    const rclcpp_lifecycle::State & previous_state) override;

  // Interfaces
  std::vector<hardware_interface::StateInterface> export_state_interfaces() override;
  std::vector<hardware_interface::CommandInterface> export_command_interfaces() override;

  // IO
  hardware_interface::return_type read(
    const rclcpp::Time & time,
    const rclcpp::Duration & period) override;

  hardware_interface::return_type write(
    const rclcpp::Time & time,
    const rclcpp::Duration & period) override;

private:
  static constexpr size_t NUM_WHEELS = 4;

  std::vector<std::string> joint_names_;

  // States
  std::vector<double> hw_positions_;
  std::vector<double> hw_velocities_;

  // Commands
  std::vector<double> hw_commands_;

  // Serial parameters
  std::string serial_device_;
  int baud_rate_;

  int serial_port_fd_; // The "File Descriptor" for the USB port

  // Placeholder for serial connection
  bool serial_connected_ = false;

  // Encoder conversion
  double ticks_per_revolution_ = 2048.0;
  double wheel_radius_ = 0.05; // meters
};

}  // namespace omni_robot_hardware
