#include "rclcpp/rclcpp.hpp"
#include "turtlesim/srv/spawn.hpp"


/*
    需求:编写客户端实现,发送请求生成一只新的乌龟
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1使用参数服务声明新的乌龟信息
            3-2创建服务客户端对象,发送请求
            3-3连接服务端
            3-4组织并发送数据
        4.创建自定义节点对象 调用函数,处理响应结果
        5.释放资源 
*/
using namespace std::chrono_literals;
class SpawnTurtle :public rclcpp::Node{
public:
    SpawnTurtle(std::string str1,std::string str2):Node(str1,str2){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        // 3-1使用参数服务声明新的乌龟信息
        this->declare_parameter("x",8.0);
        this->declare_parameter("y",9.0);
        this->declare_parameter("theta",3.14);
        this->declare_parameter("turtle_name","turtle2");

        x = this->get_parameter("x").as_double();
        y = this->get_parameter("y").as_double();
        theta = this->get_parameter("theta").as_double();
        turtle_name = this->get_parameter("turtle_name").as_string();

        // 3-2创建服务客户端对象,发送请求
        client_spawn_ = this->create_client<turtlesim::srv::Spawn>("/spawn");
        
    }
    // 3-3连接服务端
    bool connect_server(){
        while(!client_spawn_->wait_for_service(1s)){
            if(!rclcpp::ok()){
                RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"客户端被中断,退出!");
                return false;
            }
            RCLCPP_WARN(this->get_logger(),"连接服务端...");
        }
        return true;
    }
    // 3-4组织并发送数据
    rclcpp::Client<turtlesim::srv::Spawn>::FutureAndRequestId request_spawn(){
        auto req = std::make_shared<turtlesim::srv::Spawn::Request>();
        req->x = this->x;
        req->y = this->y;
        req->theta = this->theta;
        req->name = this->turtle_name;
        /*
            rclcpp::Client<turtlesim::srv::Spawn>::FutureAndRequestId 
            async_send_request(std::shared_ptr<turtlesim::srv::Spawn_Request> request)
        */        
        return client_spawn_->async_send_request(req);
    }
private:
    double x,y,theta;
    std::string turtle_name;
    rclcpp::Client<turtlesim::srv::Spawn>::SharedPtr client_spawn_;
    //回调函数
    void callback_this(){
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //创建自定义节点对象 调用函数,处理响应结果
    auto client_node = std::make_shared<SpawnTurtle>("node_name","namespace");
    auto flag = client_node->connect_server();
    if(!flag){
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"连接服务端失败,退出!");
        return 0;
    }
    //发送请求并处理响应结果
    auto response = client_node->request_spawn();
    /*
        chiway@chiway-ros2-humble:~/study_code/stu_ros2/ws02_tools$ ros2 service call /spawn turtlesim/srv/Spawn "{"x": 2,"y": 2,"name": "turtle2"}"
        requester: making request: turtlesim.srv.Spawn_Request(x=2.0, y=2.0, theta=0.0, name='turtle2')

        response:
        turtlesim.srv.Spawn_Response(name='turtle2')

        chiway@chiway-ros2-humble:~/study_code/stu_ros2/ws02_tools$ ros2 service call /spawn turtlesim/srv/Spawn "{"x": 2,"y": 2,"name": "turtle2"}"
        requester: making request: turtlesim.srv.Spawn_Request(x=2.0, y=2.0, theta=0.0, name='turtle2')

        response:
        turtlesim.srv.Spawn_Response(name='')
        如果生成新乌龟时,名字重复,则响应成功但返回空字符串
    */
    auto future_code = rclcpp::spin_until_future_complete(client_node,response);
    auto res_get = response.get();
    if (future_code == rclcpp::FutureReturnCode::SUCCESS && res_get->name != "")
    {
        RCLCPP_INFO(client_node->get_logger(),"请求成功,新乌龟名字:%s",res_get->name.c_str());
    }
    else if(res_get->name == "")
    {
        RCLCPP_ERROR(client_node->get_logger(),"请求失败,乌龟名字重复!");
    }
    else if (future_code == rclcpp::FutureReturnCode::TIMEOUT)
    {
        RCLCPP_ERROR(client_node->get_logger(),"请求超时");
    }
    else if(future_code == rclcpp::FutureReturnCode::INTERRUPTED)
    {
        RCLCPP_ERROR(client_node->get_logger(),"请求被中断");
    }
    else
    {
        RCLCPP_ERROR(client_node->get_logger(),"未知状态码");
    }
    
    
    //释放资源
    rclcpp::shutdown();
    return 0;
}