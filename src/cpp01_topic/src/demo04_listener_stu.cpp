#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "base_interfaces_demo/msg/student.hpp"

using base_interfaces_demo::msg::Student;

class Stu_Listener :public rclcpp::Node{
public:
    Stu_Listener(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        //创建客户端对象
        sub_ = this->create_subscription<Student>(
            "stu_msgs",10,
            std::bind(&Stu_Listener::callback_this,this,std::placeholders::_1)
        );
    }   

private:
    rclcpp::Subscription<Student>::SharedPtr sub_;
    //回调函数
    void callback_this(const Student::SharedPtr msg){

        RCLCPP_INFO(this->get_logger(),"收到学生信息: name: %s age: %d height: %f"
        ,msg->name.c_str(),msg->age,msg->height);
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<Stu_Listener>("stu_listener_1"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}