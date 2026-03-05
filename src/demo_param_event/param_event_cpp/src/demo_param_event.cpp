#include "rclcpp/rclcpp.hpp"


/*
    需求:实现监听参数事件的功能,当参数被修改的时候,在终端输出被修改的参数名和值
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1
            3-2
            3-3
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间

class ParamEvent :public rclcpp::Node{
public:
    ParamEvent(std::string str1):Node(str1){
        this->declare_parameter<int>("sty_num",0);

        this->get_parameter("sty_num",sty_num_);

        //为对象添加监听器
        param_event_handler_ = std::make_shared<rclcpp::ParameterEventHandler>(this);
        //绑定事件的回调函数
        // std::function<void (const rclcpp::Parameter &)>
        parameter_cb_ = param_event_handler_->add_parameter_callback(
            "sty_num",
            std::bind(&ParamEvent::callback_this,this,std::placeholders::_1)
            // "",//node_name可选 不填默认为当前节点
        );

        parameter_cb_turtlesim_ = param_event_handler_->add_parameter_callback(
            "background_b",
            std::bind(&ParamEvent::callback_turtlesim,this,std::placeholders::_1),
            "turtlesim"
        );


        RCLCPP_INFO(this->get_logger(),"namesapce:  node: %s 节点创建成功",str1.c_str());

        //创建参数事件的订阅方
        // param_event_sub_ = this->create_subscription<rcl_interfaces::msg::ParameterEvent>(
        //     "/parameter_events",10,
        //     std::bind(&ParamEvent::callback_this,this,std::placeholders::_1)
        // );
    }

private:
    std::shared_ptr<rclcpp::ParameterEventHandler> param_event_handler_;//参数事件处理器对象
    std::shared_ptr<rclcpp::ParameterCallbackHandle> parameter_cb_;//参数事件回调句柄对象
    std::shared_ptr<rclcpp::ParameterCallbackHandle> parameter_cb_turtlesim_;//参数事件回调句柄对象
    // rclcpp::Subscription<rcl_interfaces::msg::ParameterEvent>::SharedPtr param_event_sub_;//参数时间订阅方对象
    int sty_num_;

    //回调函数 参数事件的处理行为
    void callback_this(const rclcpp::Parameter &param){
        //处理修改后的参数
        if(param.get_parameter_value().get<int>() >= 30 || param.get_parameter_value().get<int>() <= 0){
            RCLCPP_WARN(this->get_logger(),"参数值不合法[0,30],请重新设置");
        }
        else{
            this->set_parameter(rclcpp::Parameter("sty_num",param.get_parameter_value().get<int>()));
            RCLCPP_INFO(this->get_logger(),"参数%s被修改为%ld",param.get_name().c_str(),param.get_parameter_value().get<int>());
        }
    }

    void callback_turtlesim(const rclcpp::Parameter &param){
        RCLCPP_INFO(this->get_logger(),"turtlesim包中的参数%s被修改为%ld",param.get_name().c_str(),param.get_parameter_value().get<int>());
    }

};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<ParamEvent>("ParamServer_node_cpp"));//node_name, (namespace可选)

    //释放资源
    rclcpp::shutdown();
    return 0;
}