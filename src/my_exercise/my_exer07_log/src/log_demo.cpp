#include "rclcpp/rclcpp.hpp"


/*
    需求:周期性的输出不同级别的日志
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建定时器
            3-2回调函数输出日志
            3-3
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间

class LogDemo :public rclcpp::Node{
public:
    LogDemo(std::string str1,std::string str2):Node(str1,str2){
        RCLCPP_INFO(this->get_logger(),"namesapce: %s node: %s 节点创建成功",str2.c_str(),str1.c_str());
        //创建定时器
        timer_ = this->create_wall_timer(
            1s,
            std::bind(&LogDemo::callback_this,this)
        );
    }

private:
    rclcpp::TimerBase::SharedPtr timer_;
    //回调函数
    void callback_this(){//时间间隔 1 3 5 7 11  对应 info warn error debug fatal
        this->get_logger().set_level(rclcpp::Logger::Level::Debug); //代码中设置日志级别为Debug
        static rclcpp::Time time = this->now();
        rclcpp::Duration d = 0s;
        rclcpp::Duration d_1 = 1s;
        rclcpp::Duration d_3 = 3s;
        rclcpp::Duration d_5 = 5s;
        rclcpp::Duration d_7 = 7s;
        rclcpp::Duration d_11 = 11s;
        if(rclcpp::ok())
        {
            d = this->now() - time;
            if (d - d_11 >= 0s)
            {
                RCLCPP_FATAL(this->get_logger(),"这是致命日志");
            }
            else if (d - 7s >= 0s)
            {
                RCLCPP_DEBUG(this->get_logger(),"这是调试日志");
            }
            else if (d - 5s >= 0s)
            {
                RCLCPP_ERROR(this->get_logger(),"这是错误日志");
            }
            else if (d - 3s >= 0s)
            {
                RCLCPP_WARN(this->get_logger(),"这是警告日志");
            }
            else if (d - 1s >= 0s)
            {
                RCLCPP_INFO(this->get_logger(),"这是信息日志");
            }
        }
        if(d >= 12s)
        {
            time = this->now();
        }

        
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<LogDemo>("logdemo_node_cpp","my_car"));//node_name,namespace

    //释放资源
    rclcpp::shutdown();
    return 0;
}