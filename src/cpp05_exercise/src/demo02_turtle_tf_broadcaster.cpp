#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/transform_broadcaster.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "turtlesim/msg/pose.hpp"
#include "tf2/LinearMath/Quaternion.hpp"


/*
    需求:发布乌龟1相对于base_link 的坐标变换关系
        发布乌龟2相对于base_link 的坐标变换关系
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建
            3-2实现回调函数
            3-3在回调中解析并输出数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/
using namespace std::chrono_literals;
class TfBroadcaster :public rclcpp::Node{
public:
    TfBroadcaster(std::string str1,std::string str2):Node(str1,str2){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        //创建乌龟位姿订阅节点
        sub_turtle1_ = this->create_subscription<turtlesim::msg::Pose>(
            "/turtle1/pose",
            10,
            [this](const turtlesim::msg::Pose::SharedPtr msg){
                this->msg_turtle1_ = msg;
                flag1_ = true;
            }
        );
        sub_turtle2_ = this->create_subscription<turtlesim::msg::Pose>(
            "/turtle2/pose",
            10,
            [this](const turtlesim::msg::Pose::SharedPtr msg){
                this->msg_turtle2_ = msg;
                flag2_ = true;
            }
        );
        //创建定时器
        timer_ = this->create_wall_timer(
            0.5ms,
            std::bind(&TfBroadcaster::callback_this,this)
        );
        //创建动态坐标系广播器
        tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
    }

private:
    rclcpp::TimerBase::SharedPtr timer_;//定时器对象
    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;//动态坐标系广播器对象
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_turtle1_;//乌龟1位姿订阅对象
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_turtle2_;//乌龟2位姿订阅对象
    turtlesim::msg::Pose::SharedPtr msg_turtle1_;//乌龟1位姿消息指针
    turtlesim::msg::Pose::SharedPtr msg_turtle2_;//乌龟2位姿消息指针
    bool flag1_ = false;//标志位1
    bool flag2_ = false;//标志位2
    //回调函数
    void callback_this(){
        if (!flag1_ || !flag2_)
        {
            RCLCPP_WARN(this->get_logger(),"等待坐标消息");
            return;
        }
        //创建坐标系变换消息
        geometry_msgs::msg::TransformStamped tf_turtle2_2base;
        geometry_msgs::msg::TransformStamped tf_turtle1_2base;

        //turtle1->base_link
        tf_turtle1_2base.header.stamp = this->get_clock()->now();
        tf_turtle1_2base.header.frame_id = "base_link";
        tf_turtle1_2base.child_frame_id = "turtle1";
        tf_turtle1_2base.transform.translation.x = msg_turtle1_->x;
        tf_turtle1_2base.transform.translation.y = msg_turtle1_->y;
        tf_turtle1_2base.transform.translation.z = 0.0;
        //四元数转换 欧拉 2 quernion
        tf2::Quaternion tfs;

        tfs.setRPY(0,0,msg_turtle1_->theta);
        tf_turtle1_2base.transform.rotation.x = tfs.x();
        tf_turtle1_2base.transform.rotation.y = tfs.y();
        tf_turtle1_2base.transform.rotation.z = tfs.z();
        tf_turtle1_2base.transform.rotation.w = tfs.w();
        //turtle2->base_link
        tf_turtle2_2base.header.stamp = this->get_clock()->now();
        tf_turtle2_2base.header.frame_id = "base_link";
        tf_turtle2_2base.child_frame_id = "turtle2";
        tf_turtle2_2base.transform.translation.x = msg_turtle2_->x;
        tf_turtle2_2base.transform.translation.y = msg_turtle2_->y;
        tf_turtle2_2base.transform.translation.z = 0.0;
        //四元数转换 欧拉 2 quernion
        tfs.setRPY(0,0,msg_turtle2_->theta);
        tf_turtle2_2base.transform.rotation.x = tfs.x();
        tf_turtle2_2base.transform.rotation.y = tfs.y();
        tf_turtle2_2base.transform.rotation.z = tfs.z();
        tf_turtle2_2base.transform.rotation.w = tfs.w();

        //广播坐标系变换消息
        std::vector<geometry_msgs::msg::TransformStamped> tfs_vec;
        tfs_vec.push_back(tf_turtle2_2base);
        tfs_vec.push_back(tf_turtle1_2base);
        tf_broadcaster_->sendTransform(tfs_vec);
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<TfBroadcaster>("node_name","namespace"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}