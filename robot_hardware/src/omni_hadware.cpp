#include "robot_hardware/omni_hardware.hpp"

#include <pluginlib/class_list_macros.hpp>

namespace robot_hardware
{

hardware_interface::CallbackReturn OmniHardware::on_init(
  const hardware_interface::HardwareInfo & info)
{
  if (hardware_interface::SystemInterface::on_init(info) !=
      hardware_interface::CallbackReturn::SUCCESS)
  {
    return hardware_interface::CallbackReturn::ERROR;
  }

  size_t n = info.joints.size();

  hw_commands_.resize(n, 0.0);
  hw_states_.resize(n, 0.0);

  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn OmniHardware::on_configure(
  const rclcpp_lifecycle::State &)
{
  RCLCPP_INFO(rclcpp::get_logger("OmniHardware"), "Configured!");
  return hardware_interface::CallbackReturn::SUCCESS;
}

std::vector<hardware_interface::StateInterface>
OmniHardware::export_state_interfaces()
{
  std::vector<hardware_interface::StateInterface> states;

  for (size_t i = 0; i < hw_states_.size(); i++)
  {
    states.emplace_back(
      info_.joints[i].name, hardware_interface::HW_IF_VELOCITY, &hw_states_[i]);
  }

  return states;
}

std::vector<hardware_interface::CommandInterface>
OmniHardware::export_command_interfaces()
{
  std::vector<hardware_interface::CommandInterface> commands;

  for (size_t i = 0; i < hw_commands_.size(); i++)
  {
    commands.emplace_back(
      info_.joints[i].name, hardware_interface::HW_IF_VELOCITY, &hw_commands_[i]);
  }

  return commands;
}

hardware_interface::return_type OmniHardware::read(
  const rclcpp::Time &, const rclcpp::Duration &)
{
  // TODO: read encoder data from Arduino
  // For now: simulate feedback
  for (size_t i = 0; i < hw_states_.size(); i++)
  {
    hw_states_[i] = hw_commands_[i];
  }

  return hardware_interface::return_type::OK;
}

hardware_interface::return_type OmniHardware::write(
  const rclcpp::Time &, const rclcpp::Duration &)
{
  // TODO: send commands to Arduino

  RCLCPP_INFO(rclcpp::get_logger("OmniHardware"),
              "Cmd: %.2f %.2f %.2f %.2f",
              hw_commands_[0], hw_commands_[1],
              hw_commands_[2], hw_commands_[3]);

  return hardware_interface::return_type::OK;
}

}  // namespace robot_hardware

PLUGINLIB_EXPORT_CLASS(
  robot_hardware::OmniHardware,
  hardware_interface::SystemInterface)
