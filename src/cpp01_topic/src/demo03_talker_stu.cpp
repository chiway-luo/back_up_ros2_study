#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "base_interfaces_demo/msg/student.hpp"  //包含自定义消息头文件

/*
    需求:以固定频率发布学生信息
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建发布者对象
            3-2实现回调函数
            3-3在回调中解析并输出数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/
using base_interfaces_demo::msg::Student;
using namespace std::chrono_literals;
class Stu_Talker :public rclcpp::Node{
public:
    Stu_Talker(std::string str1):Node(str1),count_age(0){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        //3-1创建发布者对象
        pub_ = this->create_publisher<Student>("stu_msgs",10);
        //创建定时器
        timer_ = this->create_wall_timer(500ms,std::bind(&Stu_Talker::callback_this,this));

    }

private:
    rclcpp::Publisher<Student>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    //计数器
    int count_age = 0;
    //回调函数
    void callback_this(){
        auto stu = Student();
        stu.name = "水滴鱼";
        stu.age = 49+count_age;
        stu.height = 1.59;
        pub_->publish(stu);
        RCLCPP_INFO(this->get_logger(),"发布的消息是: name: %s age: %d height: %f",stu.name.c_str(),stu.age,stu.height);
        count_age++;
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<Stu_Talker>("stu_talker_1"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}