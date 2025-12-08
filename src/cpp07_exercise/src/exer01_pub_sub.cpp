#include "rclcpp/rclcpp.hpp"
#include "turtlesim/msg/pose.hpp"
#include "geometry_msgs/msg/twist.hpp"

#include "turtlesim/srv/teleport_absolute.hpp"


/*
    需求:实现一个节点,订阅乌龟1位姿信息,并传递到第二个窗口实现两窗口镜像显示
    订阅话题: /turtle1/pose
    ---
    x: 5.544444561004639
    y: 5.544444561004639
    theta: 3.135999917984009
    linear_velocity: 0.0
    angular_velocity: 0.0
    ---
    消息类型: turtlesim/msg/Pose

    发布话题: /sdy_namespace/turtle1/cmd_vel
    消息类型: geometry_msgs/msg/Twist

    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建
            3-2实现回调函数
            3-3在回调中解析并输出数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源

    bug描述 乌龟1后退时,乌龟2前进,与位姿发布有关,当乌龟速度为负数时,位姿中的速度依然是正数
    修复: 修改源代码
*/

//设置占位符
using std::placeholders::_1;
using namespace std::chrono_literals;
class Exer01PubSub :public rclcpp::Node{
public:
    Exer01PubSub(std::string str1):Node(str1,"sdy_namespace"){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        //创建服务客户端对象实现乌龟掉头
        //"ros2 service call sdy_namespace/turtle1/teleport_absolute\turtlesim/srv/TeleportAbsolute \"{x: 5.4, y: 5.4, theta: 3.14}\"
        teleport_client_ = this->create_client<turtlesim::srv::TeleportAbsolute>(
            "/sdy_namespace/turtle1/teleport_absolute"
        );
        //等待服务可用
        int count = 0;
        while (!teleport_client_->wait_for_service(1s))
        {
            if (!rclcpp::ok()) {
                RCLCPP_ERROR(this->get_logger(), "等待服务过程中节点被关闭");
                return;
            }
            ++count;
            RCLCPP_INFO(this->get_logger(),"等待服务端连接...%ds",count);
        }

        // 2. 订阅窗口1的 /turtle1/pose（只用来“抄”一次初始姿态）
        pose_sub_once_ = this->create_subscription<turtlesim::msg::Pose>(
            "/turtle1/pose",             // 窗口1的乌龟话题（绝对名称）
            10,
            std::bind(&Exer01PubSub::pose_cb, this, _1));

        RCLCPP_INFO(this->get_logger(), "已订阅 /turtle1/pose，等待第一帧位姿以同步到窗口2");

        // //创建请求对象
        // auto tele_request = std::make_shared<turtlesim::srv::TeleportAbsolute::Request>();
        // tele_request->theta = 3.14;
        // tele_request->x = 5.4;
        // tele_request->y = 5.4;


        // auto future = teleport_client_->async_send_request(tele_request);
        // //处理响应结果
        // if (rclcpp::spin_until_future_complete(this->get_node_base_interface(), future)
        //     != rclcpp::FutureReturnCode::SUCCESS)
        // {
        //     RCLCPP_ERROR(this->get_logger(), "调用 teleport_absolute 失败");
        // } else {
        //     RCLCPP_INFO(this->get_logger(), "乌龟初始姿态设置完成");
        // }
        
    }

private:
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;
    rclcpp::Client<turtlesim::srv::TeleportAbsolute>::SharedPtr teleport_client_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr pose_sub_once_;

    void pose_cb(const turtlesim::msg::Pose::SharedPtr pose)
    {
        // 已经同步过一次就直接返回（保险）
        if (synced_) {
            return;
        }

        if (!teleport_client_->service_is_ready()) {
            RCLCPP_WARN(this->get_logger(), "teleport 服务暂不可用，稍后再试");
            return;
        }

        // 3. 拿到窗口1的 x,y,theta，打包给窗口2
        auto req = std::make_shared<turtlesim::srv::TeleportAbsolute::Request>();
        req->x = pose->x;
        req->y = pose->y;
        req->theta = 3.14 - pose->theta;  // 镜像翻转

        teleport_client_->async_send_request(req);

        RCLCPP_INFO(this->get_logger(),
            "已将窗口1的姿态同步给窗口2: x=%.2f, y=%.2f, theta=%.2f",
            pose->x, pose->y, req->theta);

        // 标记为已同步，并且只订阅这一次
        synced_ = true;
        pose_sub_once_.reset();   // 释放订阅器，后面就不会再进这个回调了

        //创建第二个窗体的pose发布对象
        pub_ = this->create_publisher<geometry_msgs::msg::Twist>(
            "/sdy_namespace/turtle1/cmd_vel",1
        );
        //创建第一个窗体的pose订阅者对象
        sub_ = this->create_subscription<turtlesim::msg::Pose>(
            "/turtle1/pose",10,std::bind(&Exer01PubSub::sub_callback_this,this,_1)
        );
    }

    bool synced_ = false;


    //回调函数
    void sub_callback_this(const turtlesim::msg::Pose::ConstSharedPtr pose){
        //创建消息类型变量
        geometry_msgs::msg::Twist twist;
        //解析数据
        twist.linear.x = pose->linear_velocity;
        twist.angular.z = -pose->angular_velocity;
        // //发布数据
        pub_->publish(twist);
    }

};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<Exer01PubSub>("sdy_change_pose"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}