#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/point_stamped.hpp"


/*
    需求:发布一个坐标点消息,相对于laser坐标系,做圆周运动
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建发布方对象
            3-2创建定时器对象 指定时间间隔和回调函数
            3-3回调函数中组织并发布消息
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/
using namespace std::chrono_literals;
class TfPoint :public rclcpp::Node{
public:
    TfPoint(std::string str1,std::string str2):Node(str1,str2),t(0){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        //创建发布方对象
        pub_point_ = this->create_publisher<geometry_msgs::msg::PointStamped>("point",10);
        //创建定时器对象 时间间隔 回调函数
        timer_ = this->create_wall_timer(
            0.5s,
            std::bind(&TfPoint::callback_this,this)
        );
        //回调函数中实现组织并发布消息
    }

private:
    rclcpp::Publisher<geometry_msgs::msg::PointStamped>::SharedPtr pub_point_;
    rclcpp::TimerBase::SharedPtr timer_;
    size_t t;
    //回调函数
    void callback_this(){
        //组织消息
        geometry_msgs::msg::PointStamped point_msg;
        point_msg.header.stamp = this->now();
        point_msg.header.frame_id = "laser";
        t += 1;
        /*
            x(t)=xc​+Rcos(ωt+φ0​)
            y(t)=yc​+Rsin(ωt+φ0​)​

            半径：𝑅
            角速度：ω（单位 rad/s）
            t=0 时的初始相位：𝜑
        */
        double R = 1;
        double x_c = 2;
        double y_c = 0;
        double omega = 0.5; //角速度
        double phi_0 = M_PI;//初始相位


        point_msg.point.x = x_c + R * cos((omega * t) + phi_0);
        point_msg.point.y= y_c + R * sin((omega * t) + phi_0);
        point_msg.point.z = -0.1;

        //发布消息
        pub_point_->publish(point_msg);
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<TfPoint>("node_name","active_point"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}