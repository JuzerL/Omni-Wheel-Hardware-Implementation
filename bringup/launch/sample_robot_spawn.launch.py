import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import ExecuteProcess
from launch_ros.actions import Node
import xacro

def generate_launch_description():

```
# ===== Paths =====
description_pkg = get_package_share_directory('your_description_package')  
controller_pkg = get_package_share_directory('omni_wheel_controller')

xacro_file = os.path.join(
    description_pkg,
    'robots',
    'sample_robot.urdf.xacro'
)

controller_yaml = os.path.join(
    controller_pkg,
    'config',
    'omniwheel_controller.yaml'
)

# ===== Process Xacro =====
doc = xacro.process_file(xacro_file, mappings={'use_sim': 'false'})
robot_desc = doc.toxml()

# ===== Robot State Publisher =====
robot_state_publisher = Node(
    package='robot_state_publisher',
    executable='robot_state_publisher',
    output='screen',
    parameters=[{'robot_description': robot_desc}]
)

# ===== ros2_control node (VERY IMPORTANT) =====
ros2_control_node = Node(
    package='controller_manager',
    executable='ros2_control_node',
    parameters=[
        {'robot_description': robot_desc},
        controller_yaml
    ],
    output='screen'
)

# ===== Load Controllers (Fixed) =====
joint_state_broadcaster_spawner = Node(
    package="controller_manager",
    executable="spawner",
    arguments=["joint_state_broadcaster", "--controller-manager", "/controller_manager"],
)

omni_controller_spawner = Node(
    package="controller_manager",
    executable="spawner",
    arguments=["omni_wheel_controller", "--controller-manager", "/controller_manager"],
)

# ===== Optional: cmd_vel test node =====
'''velocity_pub = Node(
    package='velocity_pub',
    executable='velocity_pub',
    name='velocity_pub',
    remappings=[
        ('/cmd_vel_stamped', '/omni_wheel_controller/cmd_vel'),
    ],
    output='screen'
)'''

# ===== Return LaunchDescription =====
return LaunchDescription([
    robot_state_publisher,
    ros2_control_node,
    joint_state_broadcaster_spawner, # Using the spawner now
    omni_controller_spawner,         # Using the spawner now
    velocity_pub,
])
```
