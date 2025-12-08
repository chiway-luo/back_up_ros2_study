#include "rclcpp/rclcpp.hpp"
#include "base_interfaces_demo/srv/distance.hpp"
#include "turtlesim/msg/pose.hpp"

/*
    需求:解析客户端提交的目标点坐标,获取原生乌龟的坐标,计算二者距离,响应回客户端
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1.创建订阅方(原生乌龟的位姿\turtle1\pose )
            3-2.创建服务端(使用自定义消息类型)
            3-3在回调中解析请求数据,计算距离,并响应回客户端
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/
using namespace std::chrono_literals;
class MySpawn :public rclcpp::Node{
public:
    MySpawn(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        RCLCPP_INFO(this->get_logger(),"spawn_server服务端创建成功");
        
        // 3-1.创建订阅方(原生乌龟的位姿\turtle1\pose )
        sub_ = this->create_subscription<turtlesim::msg::Pose>(
            "/turtle1/pose",
            10,
            std::bind(&MySpawn::callback_sub,this,std::placeholders::_1)
        );

        //创建服务端(使用自定义消息类型)
        service_ = this->create_service<base_interfaces_demo::srv::Distance>(
            "/distance_server",
            std::bind(&MySpawn::callback_this,this,std::placeholders::_1,std::placeholders::_2)
        );
    }

private:
    rclcpp::Service<base_interfaces_demo::srv::Distance>::SharedPtr service_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_;
    base_interfaces_demo::srv::Distance::Request::SharedPtr temp_pose_;
    bool has_pose_ = false;
    bool pose_print = true;
    //回调函数
    void callback_sub(const turtlesim::msg::Pose::SharedPtr msg){
        //订阅原乌龟节点的坐标信息
        if(pose_print){
            RCLCPP_INFO(this->get_logger(),"乌龟当前坐标:(%.2f,%.2f)",msg->x,msg->y);
            pose_print = false;
        }

        temp_pose_ = std::make_shared<base_interfaces_demo::srv::Distance::Request>();
        temp_pose_->x = msg->x;
        temp_pose_->y = msg->y;
        temp_pose_->theta = msg->theta;
        has_pose_ = true;
    }
    void callback_this(const base_interfaces_demo::srv::Distance::Request::SharedPtr request,
                       const base_interfaces_demo::srv::Distance::Response::SharedPtr response){
        //等待订阅方接收数据
        rclcpp::Rate rate(1s);
        rate.sleep(); //等待一秒钟让订阅方接收到数据
        while(rclcpp::ok()){
            if(has_pose_){
                break;
            }
            RCLCPP_INFO(this->get_logger(),"等待订阅方发送乌龟位姿数据..");
            rate.sleep();
        }
        
        RCLCPP_INFO(this->get_logger(),"收到客户端请求,正在计算距离...");
        
        //计算距离
        float distance = sqrt(std::pow(temp_pose_->x - request->x,2) +
                              std::pow(temp_pose_->y - request->y,2));
        //响应回客户端
        response->distance = distance;
        RCLCPP_INFO(this->get_logger(),"目标点坐标(%.2f,%.2f),乌龟当前坐标(%.2f,%.2f),计算结果:距离=%.2f",
                     request->x,request->y,
                     temp_pose_->x,temp_pose_->y,
                     distance);
        RCLCPP_INFO(this->get_logger(),"距离计算完成,已响应回客户端");
        pose_print = true;
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<MySpawn>("sdy_service"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}