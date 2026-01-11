#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"

/*
    需求:发布雷达消息,实现圆圈障碍物逐渐缩小至laser坐标系原点处(仿真案例)
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建雷达消息发布方
            3-2创建定时器对象
            3-3
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

/*
    std_msgs/Header header # 头部中的时间戳是扫描的采集时间
            builtin_interfaces/Time stamp
                    int32 sec
                    uint32 nanosec
            string frame_id
                                # 扫描中第一条射线的时间
                                #
                                # 在坐标系frame_id中,角度围绕
                                # 正Z轴测量(如果Z向上则为逆时针)
                                # 零角度沿x轴正方向

    float32 angle_min            # 扫描起始角度 [rad]
    float32 angle_max            # 扫描结束角度 [rad]
    float32 angle_increment      # 测量间的角距离 [rad]

    float32 time_increment       # 测量间的时间 [秒] - 如果您的扫描仪
                                # 在移动,这将用于插值3d点的位置
    float32 scan_time            # 扫描间的时间 [秒]

    float32 range_min            # 最小范围值 [m]
    float32 range_max            # 最大范围值 [m]

    float32[] ranges             # 范围数据 [m]
                                # (注意:小于range_min或大于range_max的值应被丢弃)
    float32[] intensities        # 强度数据 [设备特定单位]。如果您的
                                # 设备不提供强度,请保持数组为空

*/

using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间

class SimLaser :public rclcpp::Node{
public:
    SimLaser(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),"namesapce:  node: %s 节点创建成功",str1.c_str());
        //3-1创建雷达消息发布方
        pub_laser_ = this->create_publisher<sensor_msgs::msg::LaserScan>("scan",10);
        //3-2创建定时器对象
        timer_ = this->create_wall_timer(50ms,std::bind(&SimLaser::timer_callback,this));
        //3-3创建雷达消息对象
        sensor_msg_ = std::make_shared<sensor_msgs::msg::LaserScan>();
        init_laser_scan("laser", 360.0, 720, 0.12, 10);//坐标系id,视角,扫描点数,最小范围,最大范围

    }

private:
    rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr pub_laser_;//雷达消息发布方指针
    rclcpp::TimerBase::SharedPtr timer_;//定时器对象指针
    std::shared_ptr<sensor_msgs::msg::LaserScan> sensor_msg_;//雷达消息对象
    bool msg_init_flag_ = false;//雷达消息初始化标志位

    //定时器函数
    void timer_callback(){
        if (!msg_init_flag_)
        {
            return;
        }
        //组织雷达消息
        // sensor_msg_->ranges.clear();//清空之前的数据
        sensor_msg_->header.stamp = this->get_clock()->now();//时间戳
        static double distance = 9.0;//初始距离
        if(distance <= 0.12){
            distance = 9.0;//重置距离
        }
        //模拟圆圈障碍物逐渐缩小至laser坐标系原点处
        //填充距离数据
        std::fill(sensor_msg_->ranges.begin(), sensor_msg_->ranges.end(), distance);
        distance -= 0.01;//距离递减
        pub_laser_->publish(*sensor_msg_);//发布雷达消息        
        
    }

    //初始化雷达消息数据
    //坐标系id,视角,扫描点数,最小范围,最大范围,发布频率(一秒20圈)
    void init_laser_scan(std::string frame_id="laser", float pov=360.0,
                         int num_points=720, float range_min=0.12, float range_max=3.5,float rate=20.0){
        sensor_msg_->header.frame_id = frame_id;//坐标系id
        //计算起始角度
        double half_pov = pov / 2.0;
        sensor_msg_->angle_min = half_pov * M_PI / 180.0 * -1;//起始角度
        sensor_msg_->angle_max = half_pov * M_PI / 180.0;//结束角度
        sensor_msg_->angle_increment = sensor_msg_->angle_max / (num_points / 2);//角度增量
        sensor_msg_->time_increment = 1/rate/num_points;//时间增量 同一圈扫描里相邻两束之间的时间间隔
        sensor_msg_->scan_time = 1/rate;//扫描时间 一圈扫描所需的时间
        sensor_msg_->range_min = range_min;//最小范围
        sensor_msg_->range_max = range_max;//最大范围
        // int num_points = static_cast<int>((sensor_msg_->angle_max - sensor_msg_->angle_min) / sensor_msg_->angle_increment);//扫描点数
        sensor_msg_->ranges.resize(num_points,0);//调整范围数组大小
        msg_init_flag_ = true;//雷达消息初始化完成
        return;
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<SimLaser>("SimLaser_node_cpp"));//node_name, (namespace可选)

    //释放资源
    rclcpp::shutdown();
    return 0;
}
