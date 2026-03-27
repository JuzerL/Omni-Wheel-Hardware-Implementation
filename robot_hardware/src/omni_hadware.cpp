#include "omni_hardware.hpp"
#include "pluginlib/class_list_macros.hpp"

// Pro-Way Linux Headers
#include <fcntl.h>   
#include <termios.h> 
#include <unistd.h>  
#include <cstring>

namespace omni_robot_hardware
{

hardware_interface::CallbackReturn OmniSystemHardware::on_init(
  const hardware_interface::HardwareInfo & info)
{
  if (SystemInterface::on_init(info) != hardware_interface::CallbackReturn::SUCCESS)
  {
    return hardware_interface::CallbackReturn::ERROR;
  }

  // Joint names must match your URDF
  joint_names_ = {"front_left", "front_right", "rear_left", "rear_right"};

  hw_positions_.assign(NUM_WHEELS, 0.0);
  hw_velocities_.assign(NUM_WHEELS, 0.0);
  hw_commands_.assign(NUM_WHEELS, 0.0);
  
  serial_port_fd_ = -1; // Initialize file descriptor to -1

  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn OmniSystemHardware::on_configure(
  const rclcpp_lifecycle::State &)
{
  serial_device_ = info_.hardware_parameters["serial_device"];
  
  // OPEN THE SERIAL PORT
  serial_port_fd_ = open(serial_device_.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  if (serial_port_fd_ == -1) {
    RCLCPP_ERROR(rclcpp::get_logger("OmniHW"), "Could not open serial port: %s", serial_device_.c_str());
    return hardware_interface::CallbackReturn::ERROR;
  }

  // CONFIGURE THE SERIAL PORT (The "Pro Way")
  struct termios tty;
  if(tcgetattr(serial_port_fd_, &tty) != 0) {
      RCLCPP_ERROR(rclcpp::get_logger("OmniHW"), "Error from tcgetattr");
      return hardware_interface::CallbackReturn::ERROR;
  }

  cfsetispeed(&tty, B115200);
  cfsetospeed(&tty, B115200);

  tty.c_cflag &= ~PARENB;        // No Parity
  tty.c_cflag &= ~CSTOPB;        // 1 Stop bit
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;            // 8 Bits
  tty.c_cflag &= ~CRTSCTS;       // No flow control
  tty.c_cflag |= CREAD | CLOCAL; // Turn on READ
  tty.c_lflag &= ~ICANON;        // Raw mode
  tty.c_lflag &= ~ECHO;          // Disable echo
  tty.c_lflag &= ~ISIG;          // Disable interpretation of INTR, QUIT, SUSP

  tcsetattr(serial_port_fd_, TCSANOW, &tty);

  RCLCPP_INFO(rclcpp::get_logger("OmniHW"), "Hardware Configured Successfully on %s", serial_device_.c_str());
  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn OmniSystemHardware::on_activate(
  const rclcpp_lifecycle::State &)
{
  std::fill(hw_positions_.begin(), hw_positions_.end(), 0.0);
  std::fill(hw_velocities_.begin(), hw_velocities_.end(), 0.0);
  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn OmniSystemHardware::on_deactivate(
  const rclcpp_lifecycle::State &)
{
  if (serial_port_fd_ != -1) {
    close(serial_port_fd_);
    serial_port_fd_ = -1;
  }
  return hardware_interface::CallbackReturn::SUCCESS;
}

std::vector<hardware_interface::StateInterface> OmniSystemHardware::export_state_interfaces()
{
  std::vector<hardware_interface::StateInterface> interfaces;
  for (size_t i = 0; i < NUM_WHEELS; ++i) {
    interfaces.emplace_back(joint_names_[i], "position", &hw_positions_[i]);
    interfaces.emplace_back(joint_names_[i], "velocity", &hw_velocities_[i]);
  }
  return interfaces;
}

std::vector<hardware_interface::CommandInterface> OmniSystemHardware::export_command_interfaces()
{
  std::vector<hardware_interface::CommandInterface> interfaces;
  for (size_t i = 0; i < NUM_WHEELS; ++i) {
    interfaces.emplace_back(joint_names_[i], "velocity", &hw_commands_[i]);
  }
  return interfaces;
}

hardware_interface::return_type OmniSystemHardware::write(
  const rclcpp::Time &, const rclcpp::Duration &)
{
  const double SCALE = 1000.0; // Increased precision
  std::string msg = "$";

  for (size_t i = 0; i < NUM_WHEELS; ++i) {
    msg += std::to_string(static_cast<int>(hw_commands_[i] * SCALE));
    if (i < NUM_WHEELS - 1) msg += ",";
  }
  msg += "\n";

  // REAL SEND
  if (serial_port_fd_ != -1) {
    ::write(serial_port_fd_, msg.c_str(), msg.size());
  }

  return hardware_interface::return_type::OK;
}

hardware_interface::return_type OmniSystemHardware::read(
  const rclcpp::Time &, const rclcpp::Duration & period)
{
  if (serial_port_fd_ == -1) return hardware_interface::return_type::ERROR;

  char read_buf[256];
  memset(&read_buf, '\0', sizeof(read_buf));
  
  // REAL READ
  int num_bytes = ::read(serial_port_fd_, &read_buf, sizeof(read_buf));

  if (num_bytes > 0) {
    std::string incoming(read_buf);
    if (incoming[0] == '#') {
      incoming.erase(0, 1);
      std::stringstream ss(incoming);
      std::string token;
      std::vector<int> ticks;

      while (std::getline(ss, token, ',')) {
        try { ticks.push_back(std::stoi(token)); } catch (...) { continue; }
      }

      if (ticks.size() == NUM_WHEELS) {
        for (size_t i = 0; i < NUM_WHEELS; ++i) {
          double position_rad = (ticks[i] / ticks_per_revolution_) * 2.0 * M_PI;
          hw_velocities_[i] = (position_rad - hw_positions_[i]) / period.seconds();
          hw_positions_[i] = position_rad;
        }
      }
    }
  }
  return hardware_interface::return_type::OK;
}

} // namespace omni_robot_hardware

PLUGINLIB_EXPORT_CLASS(omni_robot_hardware::OmniSystemHardware, hardware_interface::SystemInterface)
