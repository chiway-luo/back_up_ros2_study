#include "rclcpp/rclcpp.hpp"
#include "base_interfaces_demo/srv/add_ints.hpp"

/*
    需求:创建客户端，组织数据并提交，然后处理相应结果（需要关注业务流程）
    流程:
        前提 需要判断提交的参数是否正确

        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建客户端
            3-2连接服务器 如果连接不到服务器，不能发送请求
            3-3发送请求
        4.创建节点对象指针
        5.调用连接服务的函数，根据连接结果做下一步处理
        连接服务后，调用请求发送函数
        再处理相应结果
        6.释放资源
*/
using base_interfaces_demo::srv::AddInts;
using namespace std::chrono_literals;

class Client_Node :public rclcpp::Node{
public:
    Client_Node(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        //创建客户端 对象
        //使用模板 服务接口 参数 服务话题名称 返回值 客户端对象指针
        client_ = this->create_client<AddInts>("sdy_add_ints");
        
    }
    //连接服务器 如果连接成功返回TRUE
    bool conect_server(){
        //在指定超时时间内连接服务器，如果连接成功，那么返回true
        while (!client_->wait_for_service(2s)) //循环以1s为超时时间连接服务器，连接到服务器才退出循环
        {
            //当前实现存在问题 ctrl+c终止程序时会陷入死循环
            //对ctrl+c 处理
            //按下ctrl+c 后意味着要释放资源，比如关闭context
            //使用(rclcpp::get_logger("rclcpp")终止时不会报错，不依赖context
            if (!rclcpp::ok())
            {
                RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"程序被终止，退出连接服务器");
                return false;
            }
            RCLCPP_WARN(rclcpp::get_logger("rclcpp"),"正在连接服务器...");
        }
        return true;
    }
    //发送请求函数
    inline rclcpp::Client<base_interfaces_demo::srv::AddInts>::FutureAndRequestId  send_request(int num1,int num2){
        //组织请求数据

        //发送
        /*
            inline rclcpp::Client<base_interfaces_demo::srv::AddInts>::FutureAndRequestId 
            rclcpp::Client<base_interfaces_demo::srv::AddInts>::async_send_request(std::shared_ptr<base_interfaces_demo::srv::AddInts_Request> 
            request)
        */
        auto request = std::make_shared<AddInts::Request>();
        request->num1 = num1;
        request->num2 = num2;
        return client_->async_send_request(request);

    }

private:
    rclcpp::Client<AddInts>::SharedPtr client_;
};

int main(int argc, char * argv[])
{
    if (argc != 3)
    {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"参数不正确");
        return 1;
    }
    
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //创建客户端对象
    auto client_node = std::make_shared<Client_Node>("client_node_sdy_cpp");

    //调用客户端对象的连接服务器功能
    bool flag = client_node->conect_server();
    //根据连接结果进一步处理
    if (!flag)
    {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"服务器连接失败，程序退出");
        return 1;
    }
    //发送请求
    //获取服务端返回值
    auto future = client_node->send_request(atoi(argv[1]),atoi(argv[2]));
    //处理响应
    if (rclcpp::spin_until_future_complete(client_node,future) == rclcpp::FutureReturnCode::SUCCESS)//成功
    {
        auto response = future.get();
        RCLCPP_INFO(client_node->get_logger(),"响应成功 num1:%d num2:%d sum:%d",
            atoi(argv[1]),atoi(argv[2]),response->sum
        );
    }
    else{
        RCLCPP_ERROR(client_node->get_logger(),"响应失败");
    }
    

    


    //释放资源
    rclcpp::shutdown();
    return 0;
}