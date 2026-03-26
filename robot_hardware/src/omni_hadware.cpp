#include "omni_robot_hardware/omni_system_hardware.hpp"
#include "pluginlib/class_list_macros.hpp"

namespace omni_robot_hardware
{

hardware_interface::CallbackReturn OmniSystemHardware::on_init(
  const hardware_interface::HardwareInfo & info)
{
  if (SystemInterface::on_init(info) !=
      hardware_interface::CallbackReturn::SUCCESS)
  {
    return hardware_interface::CallbackReturn::ERROR;
  }

  // Match URDF joint names EXACTLY (as in omni_wheel_controller_sample)
  joint_names_ = {
    "front_left",
    "front_right",
    "rear_left",
    "rear_right"
  };

  hw_positions_.assign(NUM_WHEELS, 0.0);
  hw_velocities_.assign(NUM_WHEELS, 0.0);
  hw_commands_.assign(NUM_WHEELS, 0.0);

  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn OmniSystemHardware::on_configure(
  const rclcpp_lifecycle::State &)
{
  // Read parameters from URDF / ros2_control
  serial_device_ = info_.hardware_parameters["serial_device"];
  baud_rate_ = std::stoi(info_.hardware_parameters["baud_rate"]);

  RCLCPP_INFO(rclcpp::get_logger("OmniHW"),
              "Serial device: %s | Baud: %d",
              serial_device_.c_str(), baud_rate_);

  // TODO: open serial port here
  serial_connected_ = true;

  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn OmniSystemHardware::on_activate(
  const rclcpp_lifecycle::State &)
{
  RCLCPP_INFO(rclcpp::get_logger("OmniHW"), "Activating hardware...");

  // Reset states
  std::fill(hw_positions_.begin(), hw_positions_.end(), 0.0);
  std::fill(hw_velocities_.begin(), hw_velocities_.end(), 0.0);

  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn OmniSystemHardware::on_deactivate(
  const rclcpp_lifecycle::State &)
{
  RCLCPP_INFO(rclcpp::get_logger("OmniHW"), "Deactivating hardware...");
  return hardware_interface::CallbackReturn::SUCCESS;
}

std::vector<hardware_interface::StateInterface>
OmniSystemHardware::export_state_interfaces()
{
  std::vector<hardware_interface::StateInterface> interfaces;

  for (size_t i = 0; i < NUM_WHEELS; ++i)
  {
    interfaces.emplace_back(joint_names_[i], "position", &hw_positions_[i]);
    interfaces.emplace_back(joint_names_[i], "velocity", &hw_velocities_[i]);
  }

  return interfaces;
}

std::vector<hardware_interface::CommandInterface>
OmniSystemHardware::export_command_interfaces()
{
  std::vector<hardware_interface::CommandInterface> interfaces;

  for (size_t i = 0; i < NUM_WHEELS; ++i)
  {
    interfaces.emplace_back(joint_names_[i], "velocity", &hw_commands_[i]);
  }

  return interfaces;
}

hardware_interface::return_type OmniSystemHardware::write(
  const rclcpp::Time &,
  const rclcpp::Duration &)
{
  /*
    ============================
    SERIAL TX (Controller → MCU)
    ============================
  */

  const double SCALE = 100.0; // or 1000.0 for more precision

  std::vector<int> cmd_int(NUM_WHEELS);

  // Convert double → int
  for (size_t i = 0; i < NUM_WHEELS; ++i)
  {
    cmd_int[i] = static_cast<int>(hw_commands_[i] * SCALE);
  }

  // Format string: "$W1,W2,W3,W4\n"
  std::string msg = "$";

  for (size_t i = 0; i < NUM_WHEELS; ++i)
  {
    msg += std::to_string(cmd_int[i]);

    if (i < NUM_WHEELS - 1)
      msg += ",";
  }

  msg += "\n";

  /*
    Example:
    "$120,-80,100,95\n"
  */

  // Pseudo send
  /*
    serial.write(msg);
  */

  RCLCPP_INFO(rclcpp::get_logger("OmniHW"),
              "TX: %s", msg.c_str());

  return hardware_interface::return_type::OK;
}

hardware_interface::return_type OmniSystemHardware::read(
  const rclcpp::Time &,
  const rclcpp::Duration & period)
{
  /*
    ============================
    SERIAL RX (MCU → Controller)
    ============================

    Expected incoming string example:
    "#1000,980,1020,995\n"

    Where values = encoder ticks
  */

  std::string incoming;

  // Pseudo read
  /*
    incoming = serial.readLine();
  */

  /*
    Parsing logic:
  */

  if (!incoming.empty() && incoming[0] == '#')
  {
    incoming.erase(0, 1); // remove '#'

    std::stringstream ss(incoming);
    std::string token;
    std::vector<int> ticks;

    while (std::getline(ss, token, ','))
    {
      ticks.push_back(std::stoi(token));
    }

    if (ticks.size() == NUM_WHEELS)
    {
      for (size_t i = 0; i < NUM_WHEELS; ++i)
      {
        // Convert ticks → radians
        double revolutions = ticks[i] / ticks_per_revolution_;
        double position_rad = revolutions * 2.0 * M_PI;

        // Velocity estimation
        double velocity = (position_rad - hw_positions_[i]) / period.seconds();

        hw_positions_[i] = position_rad;
        hw_velocities_[i] = velocity;
      }
    }
  }

  return hardware_interface::return_type::OK;
}

}  // namespace omni_robot_hardware

PLUGINLIB_EXPORT_CLASS(
  omni_robot_hardware::OmniSystemHardware,
  hardware_interface::SystemInterface)
