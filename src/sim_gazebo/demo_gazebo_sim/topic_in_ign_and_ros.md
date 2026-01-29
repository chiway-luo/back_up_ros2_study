2.ros_gz_bridge支持的消息类型
以下是ROS2与Ignition Gazebo中话题消息类型对应表：

ROS2消息类型	Gazebo Transport 类型
builtin_interfaces/msg/Time	                        gz.msgs.Time
geometry_msgs/msg/Point	                            gz.msgs.Vector3d
geometry_msgs/msg/Pose	                            gz.msgs.Pose
geometry_msgs/msg/PoseArray	                        gz.msgs.Pose_V
geometry_msgs/msg/PoseStamped	                    gz.msgs.Pose
geometry_msgs/msg/PoseWithCovariance	            gz.msgs.PoseWithCovariance
geometry_msgs/msg/Quaternion	                    gz.msgs.Quaternion
geometry_msgs/msg/Transform	                        gz.msgs.Pose
geometry_msgs/msg/TransformStamped	                gz.msgs.Pose
geometry_msgs/msg/Twist	                            gz.msgs.Twist
geometry_msgs/msg/TwistWithCovariance	            gz.msgs.TwistWithCovariance
geometry_msgs/msg/TwistWithCovarianceStamped	    gz.msgs.TwistWithCovariance
geometry_msgs/msg/Vector3	                            gz.msgs.Vector3d
geometry_msgs/msg/Wrench	                            gz.msgs.Wrench
geometry_msgs/msg/WrenchStamped	gz.msgs.Wrench
nav_msgs/msg/Odometry	                            gz.msgs.Odometry
nav_msgs/msg/Odometry	                            gz.msgs.OdometryWithCovariance
rcl_interfaces/msg/ParameterValue	                    gz.msgs.Any
ros_gz_interfaces/msg/Altimeter	                    gz.msgs.Altimeter
ros_gz_interfaces/msg/Contact	                        gz.msgs.Contact
ros_gz_interfaces/msg/Contacts	                    gz.msgs.Contacts
ros_gz_interfaces/msg/Dataframe	                    gz.msgs.Dataframe
ros_gz_interfaces/msg/Entity	                        gz.msgs.Entity
ros_gz_interfaces/msg/Float32Array	gz.msgs.Float_V
ros_gz_interfaces/msg/GuiCamera	gz.msgs.GUICamera
ros_gz_interfaces/msg/JointWrench	gz.msgs.JointWrench
ros_gz_interfaces/msg/Light	gz.msgs.Light
ros_gz_interfaces/msg/SensorNoise	gz.msgs.SensorNoise
ros_gz_interfaces/msg/StringVec	gz.msgs.StringMsg_V
ros_gz_interfaces/msg/TrackVisual	gz.msgs.TrackVisual
ros_gz_interfaces/msg/VideoRecord	gz.msgs.VideoRecord
ros_gz_interfaces/msg/WorldControl	gz.msgs.WorldControl
rosgraph_msgs/msg/Clock*	gz.msgs.Clock*
sensor_msgs/msg/BatteryState	gz.msgs.BatteryState
sensor_msgs/msg/CameraInfo	gz.msgs.CameraInfo
sensor_msgs/msg/FluidPressure	gz.msgs.FluidPressure
sensor_msgs/msg/Image	gz.msgs.Image
sensor_msgs/msg/Imu	gz.msgs.IMU
sensor_msgs/msg/JointState	gz.msgs.Model
sensor_msgs/msg/Joy	gz.msgs.Joy
sensor_msgs/msg/LaserScan	gz.msgs.LaserScan
sensor_msgs/msg/MagneticField	gz.msgs.Magnetometer
sensor_msgs/msg/NavSatFix	gz.msgs.NavSat
sensor_msgs/msg/PointCloud2	gz.msgs.PointCloudPacked
std_msgs/msg/Bool	gz.msgs.Boolean
std_msgs/msg/ColorRGBA	gz.msgs.Color
std_msgs/msg/Empty	gz.msgs.Empty
std_msgs/msg/Float32	gz.msgs.Float
std_msgs/msg/Float64	gz.msgs.Double
std_msgs/msg/Header	gz.msgs.Header
std_msgs/msg/Int32	gz.msgs.Int32
std_msgs/msg/String	gz.msgs.StringMsg
std_msgs/msg/UInt32	gz.msgs.UInt32
tf2_msgs/msg/TFMessage	gz.msgs.Pose_V
trajectory_msgs/msg/JointTrajectory	gz.msgs.JointTrajectory
以及服务消息类型对应表：

ROS2消息类型	Gazebo 请求	Gazebo 响应
ros_gz_interfaces/srv/ControlWorld	gz.msgs.WorldControl	gz.msgs.Boolean