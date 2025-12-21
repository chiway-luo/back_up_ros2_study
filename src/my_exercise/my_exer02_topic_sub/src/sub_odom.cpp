#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/odometry.hpp"

/*
ros2 topic info /odom 
Type: nav_msgs/msg/Odometry

    需求:要求订阅/odom话题,解析里程计数据,每移动2m,在终端输出一次机器人位置坐标
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建话题订阅方
            3-2在回调函数中接受消息并解析数据,计算位移是否超出x米(和上一次记录点做比较)
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

using namespace std::chrono_literals; //使用时间命名空间
using std::placeholders::_1; //占位符命名空间

class SubOdom :public rclcpp::Node{
public:
    SubOdom(std::string str1,std::string str2):Node(str1,str2){
        //动态参数
        this->declare_parameter<double>("distance_limited",0.2);

        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        //创建话题订阅方
        sub_odom = this->create_subscription<nav_msgs::msg::Odometry>(
            "/odom",
            1000,
            std::bind(&SubOdom::odom_callback,this,_1)
        );
    }

private:
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr sub_odom;
    nav_msgs::msg::Odometry::SharedPtr last_print_msg; //存储上一次打印的位置信息

    void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg){
        //创建上一次发送数据的变量,里程计包含机器人的坐标信息和速度信息
        static bool is_first = true; //是否是第一次接收消息
        if (is_first)
        {
            last_print_msg = msg;
            is_first = false;
            print_odom(last_print_msg);
            return;
        }
        else{
            //计算距离
            double x = msg->pose.pose.position.x;//当前x坐标
            double y = msg->pose.pose.position.y;//当前y坐标
            double last_x = last_print_msg->pose.pose.position.x;//上一次打印的x坐标
            double last_y = last_print_msg->pose.pose.position.y;//上一次打印的y坐标
            double distance = sqrt(pow((x)-(last_x),2) + pow((y)-(last_y),2));//计算间距
            if (int(distance) >= this->get_parameter("distance_limited").as_double())//判断移动距离是否大于等于2米
            {
                print_odom(msg,distance);
                last_print_msg = msg;
                return;
            }
        }
    }

    void print_odom(const nav_msgs::msg::Odometry::SharedPtr msg){
        RCLCPP_INFO(this->get_logger(),
            "机器人当前位置:(%.2f,%.2f)",
            msg->pose.pose.position.x,
            msg->pose.pose.position.y
        );
    }
    //重载函数,打印距离
    void print_odom(const nav_msgs::msg::Odometry::SharedPtr msg,double distance){
        RCLCPP_INFO(this->get_logger(),
            "机器人当前位置:(%.2f,%.2f),距离上一次移动了%.2f米",
            msg->pose.pose.position.x,
            msg->pose.pose.position.y,
            distance
        );
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<SubOdom>("sub_odom","mycar"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}