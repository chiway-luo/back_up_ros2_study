#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/transform_listener.h"
#include "tf2_ros/buffer.h"
// #include "geometry_msgs/msg/point_stamped.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"


/*
    需求:goal 和 robot_0/base_footprint 两个 frame,根据 TF 做追踪目标(goal)并发 cmd_vel”的小控制器
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建缓冲区对象
            3-2创建坐标变换监听方
            3-3创建速度指令发布方
            3-4创建定时器,周期性的解析坐标变换,生成并发布速度指令
        4.调用spin函数,并传入节点对象指针
        5.释放资源

    ros2 run teleop_twist_keyboard teleop_twist_keyboard --ros-args --remap __ns:=/robot_1

    优化:
        读取雷达数据判断左右是否有障碍物,调整角度避让
    问题:
        当前方有障碍物时,机器人会直接撞击过去,需要改进避障算法,但是我不改,我比较懒
*/

using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间

class TfListener :public rclcpp::Node{
public:
    TfListener(std::string str1):Node(str1){
        //声明动态参数
        this->declare_parameter<std::string>("goal_frame","goal");//目标坐标系
        this->declare_parameter<std::string>("follow_frame","robot_0/base_link");//跟随坐标系
        this->declare_parameter<double>("obstacle_threshold",0.5);//最小避障距离阈值
        //获取参数
        this->get_parameter("goal_frame",goal_frame);
        this->get_parameter("follow_frame",follow_frame);
        this->get_parameter("obstacle_threshold",obstacle_threshold);


        RCLCPP_INFO(this->get_logger(),"namesapce:  node: %s 节点创建成功",str1.c_str());
        //创建缓冲区对象
        tf2_buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
        //创建监听器对象
        tf2_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf2_buffer_);
        //创建定时器对象 20hz
        timer_ = this->create_wall_timer(50ms,std::bind(&TfListener::sub_goal,this));
        //创建坐标点对象消息指针
        goal_tf_ = std::make_shared<geometry_msgs::msg::TransformStamped>();
        //创建速度消息发布对象
        pub_ = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel",10);

        //接收激光雷达数据
        sub_laser_ = this->create_subscription<sensor_msgs::msg::LaserScan>("base_scan",10,std::bind(&TfListener::obstacle_detect,this,_1));
        //创建激光雷达数据指针
        laser_msg_ = std::make_shared<sensor_msgs::msg::LaserScan>();

        
        

    }

private:
    tf2_ros::Buffer::SharedPtr tf2_buffer_; //缓冲区对象指针
    std::shared_ptr<tf2_ros::TransformListener> tf2_listener_; //监听器对象指针
    rclcpp::TimerBase::SharedPtr timer_; //定时器对象指针
    std::shared_ptr<geometry_msgs::msg::TransformStamped> goal_tf_; //坐标系点对象指针
    std::mutex mutex_; //互斥锁
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_; //速度消息发布器
    //优化增加变量
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr sub_laser_; //激光雷达数据订阅器
    std::shared_ptr<sensor_msgs::msg::LaserScan> laser_msg_; //激光雷达数据指针

    //动态参数
    std::string goal_frame;//目标坐标系
    std::string follow_frame;//跟随坐标系
    double obstacle_threshold; //避障距离阈值


    
    //回调函数
    void sub_goal(){
        
        // {
        //     std::lock_guard<std::mutex> lock(mutex_); //加锁
        //     *goal_tf_ = tf2_buffer_->lookupTransform("robot_0/base_link","goal",rclcpp::Time(0)); //接收变换关系
        // }
        try {
            // 关键：拿“goal 在 base_footprint 坐标系下”的位置
            *goal_tf_ = tf2_buffer_->lookupTransform(
            follow_frame,   // target
            goal_frame,                    // source
            tf2::TimePointZero         // 超时时间 只接受最新的转换
            );
        } catch (const tf2::TransformException &ex) {
            RCLCPP_WARN_THROTTLE(this->get_logger(), *this->get_clock(), 2000,
                                "TF not ready: %s", ex.what());
            return;
        }
        //计算距离
        double x = goal_tf_->transform.translation.x;
        double y = goal_tf_->transform.translation.y;

        double distance = sqrt(x*x + y*y);
        double goal_angle = atan2(y,x);

        //构建速度信息
        geometry_msgs::msg::Twist twist_msg;
        
        twist_msg.linear.x = 0.5 * distance;
        twist_msg.angular.z = 4.0 * goal_angle;

        if(distance <= 0.1){
            twist_msg.linear.x = 0.0;
            twist_msg.angular.z = 0.0;
        }
        else{
            //检测障碍物并调整角速度
            double obstacle_angular = caculate_obstacle();
            twist_msg.angular.z += obstacle_angular;
            RCLCPP_INFO(this->get_logger(),"obstacle_angular: %.2f",obstacle_angular);//测试信息,可以注释
        }

        //发布速度消息
        pub_->publish(twist_msg);

        return;
    }

    void obstacle_detect(const sensor_msgs::msg::LaserScan::SharedPtr msg){
        {
            std::lock_guard<std::mutex> lock(mutex_); //加锁
            *laser_msg_ = *msg; //接收激光雷达数据
        }
    }

    double caculate_obstacle(){
        if (laser_msg_ == NULL)//没有用,需要修改
        {
            return 0.0;
        }
        
        /* 
        ---
        header:
        stamp:
            sec: 64
            nanosec: 100000000
        frame_id: robot_0/laser
        angle_min: -3.1415927410125732
        angle_max: 3.1415927410125732
        angle_increment: 0.008738783188164234
        time_increment: 0.0
        scan_time: 0.0
        range_min: 0.0
        range_max: 15.0
        ranges:
        */
        //分配检测区域为左右
        const static double range_r = -1.57;
        const static double range_l = 1.57;
        double min_distance = obstacle_threshold; //最小距离阈值
        sensor_msgs::msg::LaserScan laser_msg;
        {
            std::lock_guard<std::mutex> lock(mutex_); //加锁
            laser_msg = *laser_msg_; //接收激光雷达数据
        }
        //计算索引值
        // int i_max = static_cast<int>((laser_msg.angle_max - laser_msg.angle_min) / laser_msg.angle_increment) - 10; //最大索引值, 防止越界减去10
        int i_max = static_cast<int>(laser_msg.ranges.size() - 1); //最大索引值
        int i0 = static_cast<int>(i_max / 2); //x轴方向索引值
        int i_l_max = static_cast<int>((range_l - laser_msg.angle_min) / laser_msg.angle_increment); //左侧结束索引值
        int i_r_min = static_cast<int>((range_r - laser_msg.angle_min) / laser_msg.angle_increment); //右侧结束索引值

        //左侧区域检测
        int count_l = 0;//障碍物计数器
        float min_front_l = 15.0; //左侧最小距离初始化
        for(int i = i0; i <= i_l_max; i += 1){
            if(laser_msg.ranges[i] < min_distance && laser_msg.ranges[i] > 1e-3 && std::isfinite(laser_msg.ranges[i])){
                count_l++;
                min_front_l = std::min(min_front_l, laser_msg.ranges[i]);
            }
        }
        //右侧区域检测
        int count_r = 0;//障碍物计数器
        float min_front_r = 15.0; //右侧最小距离初始化
        for(int i = i0; i >= i_r_min; i -= 1){
            if(laser_msg.ranges[i] < min_distance && laser_msg.ranges[i] > 1e-3 && std::isfinite(laser_msg.ranges[i])){
                count_r++;
                min_front_r = std::min(min_front_r, laser_msg.ranges[i]);
            }
        }

        //速度判定
        double vel = 0.0;
        int point_threshold = 5; //障碍物点阈值
        if (count_l - count_r >= point_threshold)
        {
            //左侧障碍物多，向右转
            vel = -10;
        }
        else if (count_r - count_l > point_threshold)
        {
            //右侧障碍物多，向左转
            vel = 10;
        }
        else if (count_r >= point_threshold && count_l >= point_threshold)
        {
            //两侧都有障碍物，向障碍物远的方向转
            if (min_front_l > min_front_r)
            {
                vel = 10; //左转
            }
            else
            {
                vel = -10; //右转
            }
        }
        return vel * std::min(min_front_l, min_front_r) * 100; //根据距离调整角速度
        
    }


};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<TfListener>("TfListener_node_cpp"));//node_name, (namespace可选)

    //释放资源
    rclcpp::shutdown();
    return 0;
}