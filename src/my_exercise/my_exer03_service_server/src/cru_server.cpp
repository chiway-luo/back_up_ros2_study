#include "rclcpp/rclcpp.hpp"
#include "my_exer_interfaces/srv/cru.hpp"
#include "nav_msgs/msg/odometry.hpp"


/*
    需求:编写服务通信的服务端.
        功能点1分析: 提取请求的数据并处理
                0 ------- 机器人终止巡航
                1 ------- 机器人开始巡航
                核心: 怎么向机器人发送速度指令
        功能点1实现: 
                思路: 复用my_exer01_topic_pub实现
                如何复用?
                    1.my_exer01_topic_pub以动态参数的方式提供了对外接口
                    2.创建参数客户端向my_exer01_topic_pub注入参数即可
                优点:
                    1.增强了代码的复用性
                    2.增强了可维护性
        
        功能点2 动态生成响应结果
                通过里程计获取机器人坐标

    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建服务端对象
            3-2创建话题订阅方对象
            3-3处理请求数据并响应结果
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间
std::mutex odom_mutex_; //互斥锁

class CruServer :public rclcpp::Node{
public:
    CruServer(std::string str1,std::string str2):Node(str1,str2){
        //创建参数客户端对象
        param_client_ = std::make_shared<rclcpp::AsyncParametersClient>(this,"/my_car/pub_vel_node_cpp");
        //连接参数服务端
        while (!param_client_->wait_for_service(1s))
        {
            if (!rclcpp::ok())
            {
                RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"客户端连接被中断");
                return;
            }
            RCLCPP_WARN(rclcpp::get_logger("rclcpp"),"连接参数服务器中...");
        }

        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        //创建服务端对象
        server_ = this->create_service<my_exer_interfaces::srv::Cru>(
            "cru_service",
            std::bind(&CruServer::service_cb,this,_1,_2)
        );
        //创建订阅方对象 里程计
        odom_msg_ = nullptr;//防止野指针
        sub_odom_ = this->create_subscription<nav_msgs::msg::Odometry>(
            "/odom",
            10,
            std::bind(&CruServer::sub_swap,this,_1)
        );

    }

private:
    rclcpp::Service<my_exer_interfaces::srv::Cru>::SharedPtr server_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr sub_odom_;
    nav_msgs::msg::Odometry::SharedPtr odom_msg_;
    // geometry_msgs::msg::Twist::SharedPtr vel_msg;
    rclcpp::AsyncParametersClient::SharedPtr param_client_;
    

    //回调函数
    void service_cb(const my_exer_interfaces::srv::Cru::Request::SharedPtr request,
                    const my_exer_interfaces::srv::Cru::Response::SharedPtr response){
        //读取响应数据
        auto request_msg = request->flag;
        RCLCPP_INFO(this->get_logger(),"接收到的消息: %s",(request_msg ? "运行" : "停止"));
        //判断里程计消息是否为空,为空我就拒绝响应
        nav_msgs::msg::Odometry::SharedPtr odom_msg;
        {
            std::lock_guard<std::mutex> lk(odom_mutex_);
            odom_msg = odom_msg_;
        }
        if (odom_msg == nullptr){
            RCLCPP_WARN(this->get_logger(),"里程计消息为空,拒绝响应...");
            response->success = false;
            response->message = "里程计消息为空,拒绝响应";
            return;
        }
        //处理数据 注入参数
        // this->declare_parameter<double>("linear_x", 0.0);
        // this->declare_parameter<double>("angular_z", 0.0);
        if (request_msg)//1 开始巡航
        {
            // vel_msg->linear.x = 0.1;
            // vel_msg->angular.z = 0.5;
            param_client_->set_parameters({
                rclcpp::Parameter("linear_x",0.1),
                rclcpp::Parameter("angular_z",0.5)
            });
        }
        else{//0 终止巡航
            // vel_msg->linear.x = 0.0;
            // vel_msg->angular.z = 0.0;
            param_client_->set_parameters({
                rclcpp::Parameter("linear_x",0.0),
                rclcpp::Parameter("angular_z",0.0)
            });
        }
        //响应结果
        print_odom(odom_msg);
        response->x = odom_msg->pose.pose.position.x;
        response->y = odom_msg->pose.pose.position.y;
        response->success = true;
        response->message = "响应成功";
    }

    //订阅方回调函数
    void sub_swap(const nav_msgs::msg::Odometry::SharedPtr msg){
        odom_msg_ = msg;
    }

    //打印里程计信息函数
    void print_odom(nav_msgs::msg::Odometry::SharedPtr odom_msg){
        RCLCPP_INFO(this->get_logger(),"当前机器人坐标 (%.2f, %.2f)",
        odom_msg->pose.pose.position.x,odom_msg->pose.pose.position.y);
    }

};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<CruServer>("cruserver_node_cpp","my_car"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}