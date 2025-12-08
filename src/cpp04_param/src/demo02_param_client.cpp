#include "rclcpp/rclcpp.hpp"
#include <vector>


/*
    需求:创建参数客户端,查询或修改服务端参数
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建参数客户端对象
            3-2连接服务端
            3-3参数的查询
            3-4参数的修改
            
        4.创建节点对象指针,调用参数操作函数,并传递给spin函数
        4.如果操作完成结束就不需要spin函数
        5.释放资源
*/
using namespace std::chrono_literals;
class ParameterNode :public rclcpp::Node{
public:
    ParameterNode(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),(str1+"参数客户端创建成功").c_str());
        //创建参数客户端对象
        //当前对象节点 参数服务端节点名称
        parameter_client_ = std::make_shared<rclcpp::SyncParametersClient>(this,"Parameter_server");
        /*
            问题:服务通信不是通过服务话题关联吗,为什么参数客户端是通过参数服务端的节点关联
            1.参数服务端启动后,底层封装了多个服务通信的服务端 ros2 service list /服务端节点名称/xxxx
            2.参数客户端启动后,底层封装了多个服务通信的客户端 ros2 service list  
            3.参数客户端通过参数服务端节点名称找到参数服务端下的多个服务端
            4.参数客户端通过调用不同的服务客户端,实现对参数服务端不同的参数操作

        */
    }

    //连接服务端 
    bool connect_server(){
        int count = 0;
        while (rclcpp::ok() && parameter_client_->wait_for_service(1s) == false)
        {
            count++;
            RCLCPP_WARN(rclcpp::get_logger("rclcpp"),"正在连接服务端...%ds",count);
        }
        if (!rclcpp::ok())
        {
            return false;
        }
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"),"服务端连接成功!!!耗时:%ds",count);
        return true;
        
    }
    //查询参数
    void get_param(){
        RCLCPP_INFO(this->get_logger(),"--------------参数查询操作--------------");
        //列出参数列表
        std::cout << "请输入要查询的参数名称:";
        std::string param_name;
        std::cin >> param_name;
        //获取单个参数
        // auto temp_param = parameter_client_->get_parameter<std::string>("sdy_name");
        //获取多个参数
        auto temp_param = parameter_client_->get_parameters({param_name});
        //判断是否有这个参数  temp_param.at(0).get_type() == rclcpp::ParameterType::PARAMETER_NOT_SET
        if (parameter_client_->has_parameter(param_name) == false)
        {
            RCLCPP_ERROR(this->get_logger(),"服务端没有这个参数");
            return;
        }
        
        RCLCPP_INFO(this->get_logger(),"参数名称:%s,参数值:%s,参数类型:%s",
            temp_param.at(0).get_name().c_str(),
            temp_param.at(0).value_to_string().c_str(),
            temp_param.at(0).get_type_name().c_str()
        );
    }

    //修改参数
    /*
        可以在客户端设置服务端不存在的参数,但是需要提前在服务端设置rclcpp::NodeOptions().allow_undeclared_parameters(true)
    */
    void set_param(){
        RCLCPP_INFO(this->get_logger(),"--------------参数修改操作--------------");
        std::string param_name;
        std::string param_value;
        rclcpp::Parameter new_param;
        std::cout << "请输入需要修改的参数名称:";
        std::cin >> param_name;
        if (parameter_client_->has_parameter(param_name))
        {
            //当前值
            auto old_param = parameter_client_->get_parameters({param_name});
            RCLCPP_INFO(this->get_logger(),"当前参数值:%s,参数类型:%s",
                old_param.at(0).value_to_string().c_str(),
                old_param.at(0).get_type_name().c_str()
            );

            std::cout << "请输入需要修改的参数值:";
            std::cin >> param_value;
            if (old_param.at(0).get_type() == rclcpp::ParameterType::PARAMETER_STRING)
            {
                new_param = rclcpp::Parameter(param_name,param_value);
            }
            else if (old_param.at(0).get_type() == rclcpp::ParameterType::PARAMETER_INTEGER)
            {
                new_param = rclcpp::Parameter(param_name,std::stol(param_value));
            }
            else if (old_param.at(0).get_type() == rclcpp::ParameterType::PARAMETER_DOUBLE)
            {
                new_param = rclcpp::Parameter(param_name,std::stod(param_value));
            }
            else
            {
                RCLCPP_ERROR(this->get_logger(),"暂不支持该类型参数修改");
                return;
            }
        }
        else
        {
            RCLCPP_ERROR(this->get_logger(),"服务端没有这个参数");
            return;
        }
        //用参数客户端发送给Parameter服务端
        std::vector<rclcpp::Parameter> new_param_vector;
        new_param_vector.push_back(new_param);
        auto set_result = parameter_client_->set_parameters(new_param_vector);
        if (set_result[0].successful)
        {
            RCLCPP_INFO(this->get_logger(),"参数修改成功");
        }
        else
        {
            RCLCPP_ERROR(this->get_logger(),"由于未知原因,参数修改失败");
        }
        //调试代码
        // auto results = parameter_client_->set_parameters(new_param_vector);

        // if (results.empty()) {
        //     RCLCPP_ERROR(this->get_logger(), "服务端没有返回结果");
        //     return;
        // }

        // // 调试输出所有 result
        // for (size_t i = 0; i < results.size(); ++i) {
        //     RCLCPP_INFO(this->get_logger(),
        //                 "result[%zu]: successful=%d, reason=\"%s\"",
        //                 i,
        //                 results[i].successful,
        //                 results[i].reason.c_str());
        // }

        // if (results[0].successful) {
        //     RCLCPP_INFO(this->get_logger(),
        //                 "参数修改成功: %s -> %s",
        //                 param_name.c_str(),
        //                 new_param.value_to_string().c_str());
        // } else {
        //     RCLCPP_ERROR(this->get_logger(),
        //                 "参数修改失败: %s, 原因: %s",
        //                 param_name.c_str(),
        //                 results[0].reason.c_str());
        // }
        
    }

    //列出参数列表
    void list_param(){
        
        auto param_list = parameter_client_->list_parameters({},10);
        if (param_list.names.empty())
        {
            RCLCPP_INFO(this->get_logger(),"远程节点当前无任何参数");
            return;
        }
        RCLCPP_INFO(this->get_logger(),"=======节点列表=======");
        for (auto &&i : param_list.names)
        {
            RCLCPP_INFO(this->get_logger(),"参数名:%s",i.c_str());
        }
        return; 
    }

    //主逻辑
    void run();

private:
    rclcpp::SyncParametersClient::SharedPtr parameter_client_;
};


int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    auto client_node = std::make_shared<ParameterNode>("Parameter_client");
    auto flag = client_node->connect_server();
    if (!flag)
    {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"连接服务端失败,退出程序");
        rclcpp::shutdown();
        return 0;
    }
    
    client_node->run();

    //释放资源
    rclcpp::shutdown();
    return 0;
}

//主逻辑
void ParameterNode::run(){
    while (rclcpp::ok())
    {
        std::cout << "\n====== 参数客户端菜单 ======\n";
        std::cout << "1. 查询参数\n";
        std::cout << "2. 修改参数\n";
        std::cout << "3. 参数列表\n";
        std::cout << "0. 退出\n";
        std::cout << "请输入操作编号: ";

        int cmd;
        std::cin >> cmd;

        if (!std::cin) {
            // 防止输入非法内容导致死循环
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "输入非法，请重新输入\n";
            continue;
        }

        switch (cmd)
        {
        case 1:
            get_param();
            break;
        case 2:
            set_param();
            break;
        case 3:
            list_param();
            break;
        case 0:
            RCLCPP_INFO(this->get_logger(),"退出菜单");
            return;
        default:
            std::cout << "无效编号，请重新输入\n";
            break;
        }
    }
}