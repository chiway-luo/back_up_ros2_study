/* 
    需求: 实现话题统计,比如消息之间的平均时间间隔,最大时间间隔,最小时间间隔等
    流程:
        1.实现基本的订阅功能
        2.在此基础上实现拓展统计功能
*/
#include <limits>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
/*
    需求:订阅发布方发布的消息,并在终端输出
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建订阅方
            3-2实现回调函数
            3-3在回调中解析并输出数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源

    - data_type: 1 平均间隔时间
    data: 999.9939528
    - data_type: 3 最大间隔时间
    data: 1000.266703
    - data_type: 2 最小间隔时间
    data: 999.857284
    - data_type: 5 平均频率
    data: 10.0
    - data_type: 4 标准公差
    data: 0.13006921823612486
*/
using namespace std::chrono_literals;
class Listener_sdy :public rclcpp::Node{
public:
    Listener_sdy(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
    }

    void my_listener(){
        //创建options对象
        auto options = rclcpp::SubscriptionOptions();
        //设置话题统计相关参数
        //该功能默认不启动,设置为启动状态
        options.topic_stats_options.state = rclcpp::TopicStatisticsState::Enable;
        //设置统计时间区间
        options.topic_stats_options.publish_period = 10s;
        //设置发布话题名称(options统计功能启用后默认会创建发布方发布统计的消息)
        options.topic_stats_options.publish_topic = "/my_sta";//默认为 std::string publish_topic = "/statistics";
        //创建订阅方
        sub_ = this->create_subscription<std_msgs::msg::String>(
            "chatter",10,
            std::bind(&Listener_sdy::callback_this,this,std::placeholders::_1),
            //const rclcpp::SubscriptionOptionsWithAllocator<AllocatorT> &options = rclcpp::SubscriptionOptionsWithAllocator<AllocatorT>()
            options
        );
        //创建定时器对象
        timer_ = this->create_wall_timer(1s,std::bind(&Listener_sdy::on_timer,this));
        
    }
private:
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
    rclcpp::Time last_time_;//上次收到的消息时间
    rclcpp::Time first_time_;//第一次消息时间
    bool is_first_msg_ = true;//是否是第一条消息
    rclcpp::Duration max_duration_ = rclcpp::Duration(0,0);//最大时间间隔
    rclcpp::Duration min_duration_ = rclcpp::Duration(std::numeric_limits<int32_t>::max(),0);//最小时间间隔，初始化为极大值
    double hz_ = 0.0;//平均频率
    int count_ = 0;//接收消息总数目

    rclcpp::TimerBase::SharedPtr timer_;//定时器对象

    void on_timer(){
        //输出消息
        if (!is_first_msg_)
        {
            RCLCPP_INFO(this->get_logger(),"最大时间间隔: %ld ns, 最小时间间隔: %ld ns, 平均频率: %.2f Hz", max_duration_.nanoseconds(), min_duration_.nanoseconds(), hz_);   
        }
        
    }

    //回调函数
    void callback_this(const std_msgs::msg::String &msg){
        auto current_time = this->now();//统一使用同一个时间点
        if (is_first_msg_) {
            if(count_ <= 10){//稳定后再进行统计
                count_ ++;
                return;
            }
            is_first_msg_ = false;
            first_time_ = current_time;
            last_time_ = first_time_;
            count_ = 0;//重置计数器
            return;
        } else
        {
            //计算时间间隔
            auto duration = current_time - last_time_;
            //更新最大时间间隔
            if (duration > max_duration_) {
                max_duration_ = duration;
            }
            //更新最小时间间隔
            if (duration < min_duration_) {
                min_duration_ = duration;
            }
        }
        //计算平均频率 = 接收消息总数目 / 接收消息总时间
        count_++;
        hz_ = static_cast<double>(count_) / (current_time - first_time_).seconds();
        // RCLCPP_INFO(this->get_logger(),"订阅到的数据:%s",msg.data.c_str());
        last_time_ = current_time;//更新上次收到消息的时间
    }
};

int main(int argc, char const *argv[])
{
    //初始化ros客户端
    rclcpp::init(argc,argv);

    //调用spin函数,并传入节点对象指针
    // rclcpp::spin(std::make_shared<Listener_sdy>("sdy_listen1"));
    auto node = std::make_shared<Listener_sdy>("sdy_listen1");
    node->my_listener();
    rclcpp::spin(node);

    //资源释放
    rclcpp::shutdown();
    return 0;

}
