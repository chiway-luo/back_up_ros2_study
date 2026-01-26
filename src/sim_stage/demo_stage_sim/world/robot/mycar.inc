include "robot/car_base.inc"
include "robot/camera.inc"
include "robot/laser.inc"


# 组装机器人 激光雷达 + 摄像头
# define my_car car_base(
#     my_camera(pose [0.15 0 0 0])
#     my_laser()
# )

# 组装机器人 激光雷达 * 2 + 摄像头
define my_car car_base(
    my_camera(pose [0.15 0 0 0])
    my_laser( pose [0.1 0 0 0] name "my_laser_front" )
    my_laser( pose [-0.1 0 0 0] name "my_laser_back" )
)