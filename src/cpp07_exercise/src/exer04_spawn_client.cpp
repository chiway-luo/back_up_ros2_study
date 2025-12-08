#include "rclcpp/rclcpp.hpp"
#include "base_interfaces_demo/srv/distance.hpp"


/*
    需求:客户端需要提交目标点坐标,并解析响应结果
    流程:
        0.解析动态传入的数据,并作为目标点坐标
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1构造函数创建客户端
            3-2客户端需要连接服务端
            3-3发送请求数据
        4.调用节点对象指针的相关函数
        5.释放资源
*/
using namespace std::chrono_literals;
class MySpawn :public rclcpp::Node{
public:
    MySpawn(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        RCLCPP_INFO(this->get_logger(),"spawn_client客户端创建成功");
        //创建客户端对象
        client_ = this->create_client<base_interfaces_demo::srv::Distance>("/distance_server");
        
    }
    void send_request(double x, double y, double theta){
        //等待客户端连接
        while (!client_->wait_for_service(1s))
        {
            if (!rclcpp::ok())
            {
                RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"客户端被中断");
                return;
            }
            
            RCLCPP_INFO(this->get_logger(),"等待服务端连接...");
        }
        //构建请求数据
        this->request_ = std::make_shared<base_interfaces_demo::srv::Distance::Request>();
        request_->x = x;
        request_->y = y;
        request_->theta = theta;
        //向客户端发送请求
        rclcpp::Rate rate(2);
        rate.sleep();
        
        auto future = client_->async_send_request(request_); 
        //处理响应
        if (rclcpp::spin_until_future_complete(this->get_node_base_interface(),future) == rclcpp::FutureReturnCode::SUCCESS)//成功
        {
            auto response = future.get();
            RCLCPP_INFO(this->get_logger(),"响应成功 num1:%f num2:%f sum:%f",
                request_->x,request_->y,response->distance
            );
        }
        else{
            RCLCPP_ERROR(this->get_logger(),"响应失败");
        }
        return;
    }

private:
    base_interfaces_demo::srv::Distance::Request::SharedPtr request_;
    rclcpp::Client<base_interfaces_demo::srv::Distance>::SharedPtr client_;
    
};

int main(int argc, char * argv[])
{
    //判断传入的参数个数
    if (argc !=5){
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"传入参数错误!需要传入3个参数:目标点x坐标 目标点y坐标 目标点朝向角度");
        return 1;
    }
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    // rclcpp::spin(std::make_shared<MySpawn>("sdy_client"));
    auto client_node = std::make_shared<MySpawn>("sdy_client");
    client_node->send_request(std::atof(argv[1]),std::atof(argv[2]),std::atof(argv[3]));


    //释放资源
    rclcpp::shutdown();
    return 0;
}