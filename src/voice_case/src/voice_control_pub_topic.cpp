#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp" //速度消息
#include "std_msgs/msg/string.hpp" //字符串消息

#include "tf2/LinearMath/Quaternion.h" //欧拉转四元数

#include "geometry_msgs/msg/pose_stamped.hpp" //导航消息  geometry_msgs/msg/PoseStamped /goal_pose


/*
    需求:通过语音控制机器人
            需要调用者下达语音指令,可以控制机器人运动,并且机器人会播报接受指令后即将进入的运动状态
              ps: "前进"
              robot:  status:前进  播报:"小车即将前进"
            自然语音 -> 被语音识别节点捕获 ---生成文本--> 到当前节点 
                                    分支1 提取关键字->生成速度(导航)指令
                                    分支2 生成文本--> 发送给语音合成节点
                                                          语音合成节点-->播报
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建订阅方(提取关键字,并生成指令)
            3-2创建速度消息发布方,结合定时器发布速度指令
            3-3创建导航消息发布方
            3-4创建发布方,订阅到语音指令的文本时,生成消息即将进入的状态,发布给消息语音合成节点
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间

class VoiceControl :public rclcpp::Node{
public:
    VoiceControl(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),"namesapce:  node: %s 节点创建成功",str1.c_str());
        // 3-1创建订阅方(提取关键字,并生成指令)
        sub_str_ = this->create_subscription<std_msgs::msg::String>("/voicewords",10,std::bind(&VoiceControl::sub_str,this,_1));
        // 3-2创建速度消息发布方,结合定时器发布速度指令
        pub_twist_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel",10);
        // 3-2创建定时器,定时发布速度指令
        timer_ = this->create_wall_timer(50ms,std::bind(&VoiceControl::pub_twist,this));
        // 3-3创建导航消息发布方
        pub_pose_ = this->create_publisher<geometry_msgs::msg::PoseStamped>("/goal_pose",10);
        // 3-4创建发布方,订阅到语音指令的文本时,生成消息即将进入的状态,发布给消息语音合成节点
        pub_str_ = this->create_publisher<std_msgs::msg::String>("/ttswords",10);


        //给消息对象分配内存
        twist_msg_ = std::make_shared<geometry_msgs::msg::Twist>();
        pose_msg_ = std::make_shared<geometry_msgs::msg::PoseStamped>();
        str_msg_ = std::make_shared<std_msgs::msg::String>();
        str_ = std::make_shared<std::string>();

        //初始化导航消息
        pose_msg_->header.frame_id = "map";
        pose_msg_->pose.position.z = 0.0;

    }

private:
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_str_; //字符串消息接收方
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_twist_; //速度消息发布方
    rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr pub_pose_; //导航消息发布方
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_str_; //字符串消息发布方-->语音合成模块
    rclcpp::TimerBase::SharedPtr timer_; //定时发布速度指令

    //创建消息对象(减少内存占用,增加性能)
    std::shared_ptr<geometry_msgs::msg::Twist> twist_msg_;
    std::shared_ptr<geometry_msgs::msg::PoseStamped> pose_msg_;
    std::shared_ptr<std_msgs::msg::String> str_msg_;//要发布给语音合成模块的消息
    std::shared_ptr<std::string> str_;//接收语音转文字消息
    bool enable_twist_publish_ = false; //是否持续发布速度

    //停止速度指令发布
    void stop_twist_publish(){
        enable_twist_publish_ = false;
        twist_msg_->linear.x = 0.0;
        twist_msg_->angular.z = 0.0;
        pub_twist_->publish(*twist_msg_);
    }


    //接受语音转文字的回调函数,提取关键字,生成指令,并且发布消息给语音合成模块
    void sub_str(const std_msgs::msg::String::SharedPtr msg){
        //提取字段
        *str_ = msg->data;

        //新指令到来时先停止旧的速度持续发布
        stop_twist_publish();

        //查找关键字
        if (str_->find("导航") != std::string::npos)
        {
            make_pose();
            return;
        }
        else if (str_->find("停止") != std::string::npos)
        {
            str_msg_->data = "小车停止运动";
            pub_str_->publish(*str_msg_);
            return;
        }
        
        else if (str_->find("前进") != std::string::npos)
        {
            //生成速度指令
            twist_msg_->linear.x = 0.5;
            twist_msg_->angular.z = 0.0;
            enable_twist_publish_ = true;
            str_msg_->data = "小车开始前进";
        }
        else if (str_->find("后退") != std::string::npos)
        {
            //生成速度指令
            twist_msg_->linear.x = -0.5;
            twist_msg_->angular.z = 0.0;
            enable_twist_publish_ = true;
            str_msg_->data = "小车开始后退";
        }
        else if (str_->find("左转") != std::string::npos)
        {
            //生成速度指令
            twist_msg_->linear.x = 0.0;
            twist_msg_->angular.z = 0.5;
            enable_twist_publish_ = true;
            str_msg_->data = "小车开始左转";
        }
        else if (str_->find("右转") != std::string::npos)
        {
            //生成速度指令
            twist_msg_->linear.x = 0.0;
            twist_msg_->angular.z = -0.5;
            enable_twist_publish_ = true;
            str_msg_->data = "小车开始右转";
        }
        else
        {
            //发布未找到指令消息
            str_msg_->data = "未找到相关指令,请重新下达";
            pub_str_->publish(*str_msg_);
            return;
        }
        

        //生成被语音合成节点订阅的文本
        pub_str_->publish(*str_msg_);

    }

    //定时发布速度指令的回调函数
    void pub_twist(){
        // pub_twist_->publish(*twist_msg_);
        if (!enable_twist_publish_)
        {
            return;
        }
        pub_twist_->publish(*twist_msg_);
    }

    //处理导航消息的函数 导航消息,要发送的str消息
    void make_pose();

};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<VoiceControl>("voice_control_node_cpp"));//node_name, (namespace可选)

    //释放资源
    rclcpp::shutdown();
    return 0;
}


//处理导航消息的函数 导航消息,要发送的str消息
void VoiceControl::make_pose(){
    //创建欧拉转四元数变化器
    tf2::Quaternion qt;
    //判断导航点 分为 5个点:绿色障碍物上方 海怪左边 小狗下方 王乐家上方 王乐家下方 王乐家左边 金库上方
    if (str_->find("停止") != std::string::npos)
    {
        //获取当前小车位置

        //由于没有获取当前位置信息的接口,所以暂时无法实现停止功能,只能让小车停在当前位置,不发布导航消息

        //生成消息
        str_msg_->data = "小车无法在导航过程中停止";
    }
    else if (str_->find("绿色障碍物") != std::string::npos)
    {
        /* 
            绿色障碍物上方
            pose:
            position:
                x: 8.280085563659668
                y: -4.570252418518066
                z: 0.0
            orientation:
                x: 0.0
                y: 0.0
                z: 0.3595331125624004
                w: 0.9331323276851962
            ---
        */
       pose_msg_->pose.position.x = 8.3;
       pose_msg_->pose.position.y = -4.6;
       qt.setRPY(0.0,0.0,0.0);
       str_msg_->data = "小车即将前往绿色障碍物上方";
       
    }
    else if (str_->find("海怪") != std::string::npos)
    {
        /* 
            海怪左边
            position:
            x: 12.592637062072754
            y: -2.7516164779663086
            z: 0.0
            orientation:
            x: 0.0
            y: 0.0
            z: 0.37637841573196945
            w: 0.9264660210558684
        ---
        */
        pose_msg_->pose.position.x = 12.6;
        pose_msg_->pose.position.y = -2.8;
        qt.setRPY(0.0,0.0,0.0);
        str_msg_->data = "小车即将前往海怪左边";

    }
    else if (str_->find("小狗") != std::string::npos)
    {
        /* 
            小狗下方
            pose:
            position:
                x: 9.792736053466797
                y: 6.139699935913086
                z: 0.0
            orientation:
                x: 0.0
                y: 0.0
                z: 0.9231781132028213
                w: 0.38437243827215145
            ---
        */
        pose_msg_->pose.position.x = 9.8;
        pose_msg_->pose.position.y = 6.1;
        qt.setRPY(0.0,0.0,0.0);
        str_msg_->data = "小车即将前往小狗下方";
    }
    else if (str_->find("王乐家上方") != std::string::npos)
    {
        /* 
            王乐家上方
            position:
                x: 12.666722297668457
                y: 2.192065954208374
                z: 0.0
            orientation:
                x: 0.0
                y: 0.0
                z: -0.38934077766109654
                w: 0.9210937839602722
            ---
        */
        pose_msg_->pose.position.x = 12.7;
        pose_msg_->pose.position.y = 2.2;
        qt.setRPY(0.0,0.0,0.0);
        str_msg_->data = "小车即将前往王乐家上方";

    }
    else if (str_->find("王乐家下方") != std::string::npos)
    {
        /* 
            王乐家下方
            position:
                x: 7.3948869705200195
                y: -3.057488441467285
                z: 0.0
            orientation:
                x: 0.0
                y: 0.0
                z: -0.3848452987206958
                w: 0.9229810919258197
            ---
        */
        pose_msg_->pose.position.x = 7.4;
        pose_msg_->pose.position.y = -3.1;
        qt.setRPY(0.0,0.0,0.0);
        str_msg_->data = "小车即将前往王乐家下方";
    }
    else if (str_->find("王乐家左边") != std::string::npos)
    {
        /* 
            王乐家左边
            pose:
            position:
                x: 8.427873611450195
                y: 1.7048296928405762
                z: 0.0
            orientation:
                x: 0.0
                y: 0.0
                z: 0.3737507829964185
                w: 0.927529165153077
            ---
        */
        pose_msg_->pose.position.x = 8.4;
        pose_msg_->pose.position.y = 1.7;
        qt.setRPY(0.0,0.0,0.0);
        str_msg_->data = "小车即将前往王乐家左边";

    }
    else if (str_->find("金库") != std::string::npos)
    {
        /* 
            金库上方
            pose:
            position:
                x: 15.069781303405762
                y: 4.729888916015625
                z: 0.0
            orientation:
                x: 0.0
                y: 0.0
                z: -0.35877180075821985
                w: 0.9334253023036735
            ---
        */
        pose_msg_->pose.position.x = 15.1;
        pose_msg_->pose.position.y = 4.7;
        qt.setRPY(0.0,0.0,0.0);
        str_msg_->data = "小车即将前往金库上方";

    }
    else
    {
        str_msg_->data = "未找到相关指令,请重新下达";
        pub_str_->publish(*str_msg_);
        return;
    }

    pose_msg_->pose.orientation.x = qt.x();
    pose_msg_->pose.orientation.y = qt.y();
    pose_msg_->pose.orientation.z = qt.z();
    pose_msg_->pose.orientation.w = qt.w();
    pose_msg_->header.stamp = this->get_clock()->now();
    pub_pose_->publish(*pose_msg_);
    pub_str_->publish(*str_msg_);
    return;
}
