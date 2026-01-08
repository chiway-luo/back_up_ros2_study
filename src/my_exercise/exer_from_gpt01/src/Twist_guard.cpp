#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"


/*
    需求:机器人上游会发布/cmd_vel_in 话题 但是可能会突然给很大的速度指令,抖动或者停止不及时.需要把速度变得安全平滑,输出到/cmd_vel 话题
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

class TwistGuard :public rclcpp::Node{
public:
    TwistGuard(std::string str1,std::string str2):Node(str1,str2){
        //动态参数
        this->declare_parameter<double>("max_linear",0.5);
        this->declare_parameter<double>("max_angular",1.0);
        this->declare_parameter<double>("acc_linear",0.02);//线速度加速度限制(用于平滑)
        this->declare_parameter<double>("acc_angular",0.1);//角速度加速度限制(用于平滑)
        this->declare_parameter<int>("timeout_ms",300);//超时就强制输出0速度
        this->declare_parameter<int>("publish_hz",20);//发布频率
        this->declare_parameter<double>("deadband_linear",0.02);//线速度死区
        this->declare_parameter<double>("deadband_angular",0.05);//角速度死区
        //获取参数
        this->get_parameter("max_linear",max_linear_);
        this->get_parameter("max_angular",max_angular_);
        this->get_parameter("acc_linear",acc_linear_);
        this->get_parameter("acc_angular",acc_angular_);
        this->get_parameter("timeout_ms",timeout_ms_);
        this->get_parameter("publish_hz",publish_hz_);
        this->get_parameter("deadband_linear",deadband_linear_);
        this->get_parameter("deadband_angular",deadband_angular_);

        //频率换算时间
        publish_ms_ = static_cast<int>(1000.0/publish_hz_);
        std::chrono::duration pub_ms = std::chrono::milliseconds(publish_ms_); 




        RCLCPP_INFO(this->get_logger(),"namesapce: %s node: %s 节点创建成功",str2.c_str(),str1.c_str());
        //创建速度订阅方
        sub_twist_ = this->create_subscription<geometry_msgs::msg::Twist>(
            "/cmd_vel_in",
            10,
            std::bind(&TwistGuard::sub_vel,this,_1)
        );
        //创建速度发布方
        pub_twist_ = this->create_publisher<geometry_msgs::msg::Twist>(
            "/cmd_vel_out",
            10
        );
        //创建定时器对象
        timer_ = this->create_wall_timer(
            pub_ms,
            std::bind(&TwistGuard::timer_callback,this)
        );
        //初始化速度消息指针
        last_twist_msg = std::make_shared<geometry_msgs::msg::Twist>();
    }
    

private:
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr sub_twist_;
    geometry_msgs::msg::Twist::SharedPtr twist_msg;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_twist_;
    rclcpp::TimerBase::SharedPtr timer_;//定时器对象指针
    std::mutex mutex_; //互斥锁
    double max_linear_;//最大线速度
    double max_angular_;//最大角速度
    double acc_linear_;//线速度加速度
    double acc_angular_;//角速度加速度
    int timeout_ms_;//超时时间
    double publish_hz_;//发布频率
    double deadband_linear_;//线速度死区
    double deadband_angular_;//角速度死区
    geometry_msgs::msg::Twist::SharedPtr last_twist_msg;//留存上一次的速度消息,用于计算加速度
    rclcpp::Time last_msg_time_; //上一次消息时间
    bool is_first_msg_ = true; //是否是第一条消息
    int publish_ms_;//发布周期 毫秒



    //订阅速度指令
    void sub_vel(const geometry_msgs::msg::Twist::SharedPtr msg){
        {
            //上锁
            std::lock_guard<std::mutex> lock(mutex_);
            twist_msg = msg;
            last_msg_time_ = this->get_clock()->now();
        }
    }

    //定时器回调函数 正常50ms执行一次
    void timer_callback(){
        //读取速度消息
        geometry_msgs::msg::Twist msg;//速度消息
        {
            std::lock_guard<std::mutex> lock(mutex_); //上锁
            if (twist_msg == nullptr)
            {
                return;
            }
            msg = *twist_msg;            
        }
        //计算时间差
        rclcpp::Time now = this->get_clock()->now();
        double dt = (now - last_msg_time_).seconds();//间隔时间 单位s

        if (dt > (timeout_ms_/1000.0))//超时处理
        {
            give_twist_msg(msg,0.0,0.0);
            pub_twist_->publish(msg);
            return;
        }


        //限幅
        if (msg.linear.x > max_linear_){
            msg.linear.x = max_linear_;
        }
        else if (msg.linear.x < -max_linear_){
            msg.linear.x = -max_linear_;
        }

        if (msg.angular.z > max_angular_){
            msg.angular.z = max_angular_;
        }
        else if (msg.angular.z < -max_angular_){
            msg.angular.z = -max_angular_;
        }
        //加速度限制
        if (is_first_msg_)//当第一次发送或者超时后恢复
        {
            pub_twist_->publish(msg);
            *last_twist_msg = msg;
            is_first_msg_ = false;
            return;
        }
        //线加速度限制
        double diff_linear = (msg.linear.x - last_twist_msg->linear.x)/(this->publish_ms_/1000.0);
        if (diff_linear > acc_linear_){
            msg.linear.x = last_twist_msg->linear.x + acc_linear_;
        }
        else if (diff_linear < -acc_linear_){
            msg.linear.x = last_twist_msg->linear.x - acc_linear_;
        }
        //角加速度限制
        double diff_angular = (msg.angular.z - last_twist_msg->angular.z)/(this->publish_ms_/1000.0);
        if (diff_angular > acc_angular_){
            msg.angular.z = last_twist_msg->angular.z + acc_angular_;
        }
        else if (diff_angular < -acc_angular_){
            msg.angular.z = last_twist_msg->angular.z - acc_angular_;
        }
        //死区处理
        if (std::abs(msg.linear.x) < deadband_linear_){
            msg.linear.x = 0.0;
        }
        if (std::abs(msg.angular.z) < deadband_angular_){
            msg.angular.z = 0.0;
        }

        pub_twist_->publish(msg);
        *last_twist_msg = msg;

    }

    void give_twist_msg(geometry_msgs::msg::Twist &msg,double linear_x,double angular_z){
        msg.linear.x = linear_x;
        msg.angular.z = angular_z;
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<TwistGuard>("twistguard_node_cpp","my_car"));//node_name,namespace

    //释放资源
    rclcpp::shutdown();
    return 0;
}