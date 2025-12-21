#include "rclcpp/rclcpp.hpp"
#include "my_exer_interfaces/srv/cru.hpp"

/*
    需求:需要提交一个整形数据到服务端,服务端根据服务端的响应数据返回机器人坐标(x,y)
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.判断提交的数据是否是整形数据(不合法退出) 如果没有提交则使用默认值1
        4.自定义节点类
        5.创建类对象
        6.连接服务端
        7.发送请求
        8.处理响应
        9.释放资源
*/

using namespace std::chrono_literals; //使用时间命名空间
using std::placeholders::_1; //占位符命名空间

class CruClient :public rclcpp::Node{
public:
    CruClient(std::string str1,std::string str2):Node(str1,str2){
        RCLCPP_INFO(this->get_logger(),"namesapce: %s node: %s 节点创建成功",str2.c_str(),str1.c_str());
        //创建客户端对象
        client_cru_ = this->create_client<my_exer_interfaces::srv::Cru>(
            "cru_service"
        );
    }

    //连接函数
    bool connect_server(){
        //等待服务端连接
        while (!client_cru_->wait_for_service(1s))
        {
            if (!rclcpp::ok())
            {
                RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"客户端节点被中断,退出");
                return false;
            }
            RCLCPP_WARN(rclcpp::get_logger("rclcpp"),"等待服务端连接...");
        }
        return true;
    }

    //发送请求函数
    rclcpp::Client<my_exer_interfaces::srv::Cru>::FutureAndRequestId send_request(int request_num){
        //创建请求对象
        auto request = std::make_shared<my_exer_interfaces::srv::Cru::Request>();
        //填写请求数据
        request->flag = request_num;
        //异步发送请求
        return client_cru_->async_send_request(request);
    }

    //打印响应结果
    void response_callback(rclcpp::Client<my_exer_interfaces::srv::Cru>::FutureAndRequestId &future,int request_num){
        auto result = future.get();
        if (result->success == false)
        {
            RCLCPP_WARN(this->get_logger(),"服务端消息: %s",result->message.c_str());
            return;
        }
        RCLCPP_INFO(this->get_logger(),"发送的 %s 接收到的机器人坐标 (%.2f, %.2f)",
        (request_num ? "启动" : "停止"),result->x,result->y);
    }

private:
    rclcpp::Client<my_exer_interfaces::srv::Cru>::SharedPtr client_cru_;
};

int main(int argc, char * argv[])
{
    //判断参数合法性
    if (argc != 2)
    {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"参数不合法,请传入一个整数0或者1");
        return 2;
    }
    
    if (argc == 2)
    {
        //判断是否是1 或者 0
        if (std::stoi(argv[1]) == 1 || std::stoi(argv[1]) == 0)
        {
            //合法
        }
        else{
            RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"参数不合法,请输入0或者1");
            return 3;
        }
    }
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    auto node = std::make_shared<CruClient>("cru_client_node_cpp","my_car");
    //连接服务端
    if (!node->connect_server())
    {
        return 4;
    }
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"),"服务端连接成功");
    //发送请求
    auto res = node->send_request(std::stoi(argv[1]));
    //处理响应
    auto future = rclcpp::spin_until_future_complete(node,res);
    if (future == rclcpp::FutureReturnCode::SUCCESS)
    {
        RCLCPP_INFO(node->get_logger(),"服务端响应成功");
        node->response_callback(res,std::stoi(argv[1]));
    }
    else{
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"请求失败");
    }
    //释放资源
    rclcpp::shutdown();
    return 0;
}