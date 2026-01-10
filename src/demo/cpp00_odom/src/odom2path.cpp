#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "nav_msgs/msg/path.hpp"
#include "nav_msgs/msg/odometry.hpp"

/*  ros2 interface show nav_msgs/msg/Odometry
    # This represents an estimate of a position and velocity in free space.
    # The pose in this message should be specified in the coordinate frame given by header.frame_id
    # The twist in this message should be specified in the coordinate frame given by the child_frame_id

    # Includes the frame id of the pose parent.
    std_msgs/Header header
            builtin_interfaces/Time stamp
                    int32 sec
                    uint32 nanosec
            string frame_id

    # Frame id the pose points to. The twist is in this coordinate frame.
    string child_frame_id

    # Estimated pose that is typically relative to a fixed world frame.
    geometry_msgs/PoseWithCovariance pose
            Pose pose
                    Point position
                            float64 x
                            float64 y
                            float64 z
                    Quaternion orientation
                            float64 x 0
                            float64 y 0
                            float64 z 0
                            float64 w 1
            float64[36] covariance

    # Estimated linear and angular velocity relative to child_frame_id.
    geometry_msgs/TwistWithCovariance twist
            Twist twist
                    Vector3  linear
                            float64 x
                            float64 y
                            float64 z
                    Vector3  angular
                            float64 x
                            float64 y
                            float64 z
            float64[36] covariance
*/

/*  ros2 interface show nav_msgs/msg/Path
    # An array of poses that represents a Path for a robot to follow.

    # Indicates the frame_id of the path.
    std_msgs/Header header
            builtin_interfaces/Time stamp
                    int32 sec
                    uint32 nanosec
            string frame_id

    # Array of poses to follow.
    geometry_msgs/PoseStamped[] poses
            std_msgs/Header header
                    builtin_interfaces/Time stamp
                            int32 sec
                            uint32 nanosec
                    string frame_id
            Pose pose
                    Point position
                            float64 x
                            float64 y
                            float64 z
                    Quaternion orientation
                            float64 x 0
                            float64 y 0
                            float64 z 0
                            float64 w 1
*/

/*
    nav_msgs/msg/Odometry
    geometry_msgs/PoseWithCovariance pose 带协方差矩阵

    nav_msgs/msg/Path
    geometry_msgs/PoseStamped[] poses 位姿数组

    需求:订阅里程计数据并转换为路径数据发布
    发布逻辑: 订阅到数据后直接发布,如果没有订阅到数据则不发布
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建里程计订阅节点(绑定回调函数)
            3-2创建路径数据发布节点
            3-3在回调函数中转换数据并发布路径数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源

    问题：
        1) 你每次回调都新建了 path_msg，所以路径不会累积  
        每次回调：新建一个空 Path
        push_back 1 个点
        publish
        结果是：你发布出去的 Path 永远只含 1 个点，在 RViz 里看起来像“点在跳”，而不是“轨迹在增长”。
        ✅ 正确做法：把 Path 作为成员变量保存起来，每次回调往里面追加。
        ----------------------------------------------上方已经修改----------------------------------------------

        2) 这段代码其实没有“坐标变换”
        你只是把 Odometry 里的 pose 拷贝到 PoseStamped，并没有从 msg->header.frame_id 变到 "odom"。
        你写死 path_msg.header.frame_id = "odom"
        但 msg 的真实坐标系可能是 "odom"、"map"、"base_link" 等
        如果 msg->header.frame_id != "odom"，那你现在等于贴了一个错误的 frame_id 标签，RViz 会显示错位。
        ✅ 建议：要么直接用 msg->header.frame_id，要么用 tf2 真正变换到 "odom"。
*/

using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间

class Odom2Path :public rclcpp::Node{
public:
    Odom2Path(std::string str1):Node(str1,rclcpp::NodeOptions().use_intra_process_comms(true)){
        //创建动态参数
        this->declare_parameter<std::string>("frame_id","odom");//路径相对于的坐标系
        this->declare_parameter<std::string>("sub_topic","odom");//接收话题名称
        this->declare_parameter<std::string>("pub_topic","path");//发布话题名称
        this->declare_parameter<double>("pub_mode_distance",0.5);//发布模式,0-持续发布,>0-距离触发式发布
        this->declare_parameter<int>("path_size",0);//路径最大存储数量,0-不限制

        //获取参数


        RCLCPP_INFO(this->get_logger(),"namesapce:  node: %s 节点创建成功",str1.c_str());
        // 3-1创建里程计订阅节点(绑定回调函数)
        odom_sub_ = this->create_subscription<nav_msgs::msg::Odometry>("odom",10,std::bind(&Odom2Path::callback_this,this,_1));
        // 3-2创建路径数据发布节点
        path_pub_ = this->create_publisher<nav_msgs::msg::Path>("path",10);
        //创建路径对象
        path_msg_ = std::make_shared<nav_msgs::msg::Path>();
    }

private:
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;//里程计订阅节点
    rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr path_pub_;//路径数据发布节点
    std::shared_ptr<nav_msgs::msg::Path> path_msg_;//路径对象指针
    double pub_mode_distance_;//发布模式

    //距离触发式发布相关变量
    // std::vector<geometry_msgs::msg::PoseStamped> poses_;//存储位姿的容器
    geometry_msgs::msg::PoseStamped last_pose_;//容器中最后一个位姿

    //时间戳统一
    rclcpp::Time current_time_;

    // 3-3在回调函数中转换数据并发布路径数据
    void callback_this(const nav_msgs::msg::Odometry::SharedPtr odom_msg){//最好使用SharedPtr 指针类型,兼容性更高;引用类型也可以使用,只是不建议
        //判断容器大小是否超过限制
        if (int(path_msg_->poses.size()) >= this->get_parameter("path_size").as_int() && this->get_parameter("path_size").as_int() >0)
        {
            // path_msg_->poses.clear();//清空之前的路径数据
            //改为清除第一个数据
            path_msg_->poses.erase(path_msg_->poses.begin());
        }
        //更新模式
        pub_mode_distance_ = this->get_parameter("pub_mode_distance").as_double();
        //更新信息
        last_pose_.header.frame_id = odom_msg->header.frame_id;//坐标系
        //统一时间戳(头)
        current_time_ = odom_msg->header.stamp;
    
        if (pub_mode_distance_ < 0)
        {
            RCLCPP_WARN_THROTTLE(this->get_logger(),*this->get_clock(),5000,"距离无效!,请重新设置parameter: pub_mode_distance");//5秒打印一次
            return;   
        }
        else if (pub_mode_distance_ > 0)
        {
            //计算距离
            double distance = current_distance(odom_msg->pose.pose,current_time_);//同时更新path_msg_->poses和last_pose_
            //如果距离不满足返回
            if (distance < pub_mode_distance_){
                return;
            }
            //满足距离要求,更新路径对象并发布
            path_msg_->header.stamp = current_time_;//时间戳
            path_msg_->header.frame_id = odom_msg->header.frame_id;//路径相对于的坐标系
            //清空之前的路径数据
            // path_msg_->poses.clear();
            //给路径对象添加位姿
            // path_msg_->poses = poses_;
            //直接使用容器中的数据
            // path_msg_->poses.push_back(last_pose_);

            //发布路径数据
            path_pub_->publish(*path_msg_);
            return;           
        }
        
        //持续发布模式
        
        last_pose_.header.stamp = odom_msg->header.stamp;//时间戳
        last_pose_.pose = odom_msg->pose.pose;
        //添加进容器
        path_msg_->poses.push_back(last_pose_);
        //给路径对象添加位姿
        path_msg_->header.stamp = current_time_;//时间戳
        path_msg_->header.frame_id = odom_msg->header.frame_id;//路径相对于的坐标系
        //发布路径数据
        path_pub_->publish(*path_msg_);
    }

    double current_distance(const geometry_msgs::msg::Pose &current_pose,const rclcpp::Time current_time){
        //如果容器为空,直接存入并返回0
        if(path_msg_->poses.empty()){
            last_pose_.pose = current_pose;
            last_pose_.header.stamp = current_time;
            path_msg_->poses.push_back(last_pose_);
            return 10e8;//返回一个很大的数字,显示路径
        }
        double dx = current_pose.position.x - last_pose_.pose.position.x;
        double dy = current_pose.position.y - last_pose_.pose.position.y;
        // double dz = current_pose.position.z - last_pose_.pose.position.z;
        double dz = 0.0;
        double distance = sqrt(dx*dx + dy*dy + dz*dz);
        //如果距离满足要求,存入容器并更新最后一个位姿
        if (distance >= pub_mode_distance_){
            last_pose_.pose = current_pose;
            last_pose_.header.stamp = current_time;
            path_msg_->poses.push_back(last_pose_);
        }
        return distance;
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<Odom2Path>("Odom2Path_node_cpp"));//node_name, (namespace可选)

    //释放资源
    rclcpp::shutdown();
    return 0;
}
