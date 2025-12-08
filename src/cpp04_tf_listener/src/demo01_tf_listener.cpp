#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/transform_listener.hpp"
#include "tf2_ros/buffer.hpp"


/*
    需求:先发布laser到base_link的坐标变换,再发布camera到base_link的坐标系相对关系
        求解laser到camera的坐标系相对关系
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建缓存对象(保存通过监听器订阅到的多个坐标变换数据)
            3-2创建监听器,绑定缓存对象,将所有广播器广播的数据写入缓存对象,监听器会自动订阅/tf和/tf_static话题
            3-3编写定时器对象,循环实现转换
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/
using namespace std::chrono_literals;
class TfListener :public rclcpp::Node{
public:
    TfListener(std::string str1,std::string str2):Node(str1,str2){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        //创建缓存对象
        tf_buffer_ = std::make_unique<tf2_ros::Buffer>(this->get_clock());
        //创建监听器对象,绑定缓存对象
        tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_,this);
        //编写定时器,绑定回调函数
        timer_ = this->create_wall_timer(
            1s,
            std::bind(&TfListener::callback_this,this)
        );
    }

private:
    std::unique_ptr<tf2_ros::Buffer> tf_buffer_; //缓存对象
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_; //监听器对象
    std::shared_ptr<rclcpp::TimerBase> timer_;//定时器对象
    //回调函数
    void callback_this(){
        // bool c_t = tf_buffer_->canTransform("camera","laser",tf2::TimePointZero); 除了try-catch外的另一种判断方式
        //实现坐标系转换
        try
        {
            // geometry_msgs::msg::TransformStamped 返回值 新的坐标帧
            // lookupTransform(const std::string & target_frame, 新的坐标帧的父级坐标系
            // const std::string & source_frame,  新的坐标帧的子级坐标系
            // const TimePoint & time) const override; 转换的时间点 tf2::TimePointZero表示最新的时间点
            auto ts = tf_buffer_->lookupTransform("camera","laser",tf2::TimePointZero);
            RCLCPP_INFO(this->get_logger(),"===============转换完成的坐标帧信息==================");
            RCLCPP_INFO(this->get_logger(),"父级坐标系: %s 子级坐标系: %s 偏移量(%.2f,%.2f,%.2f)",
                ts.header.frame_id.c_str(),//camera
                ts.child_frame_id.c_str(),//laser
                ts.transform.translation.x,
                ts.transform.translation.y,
                ts.transform.translation.z
            );
        }
        catch(const tf2::LookupException& e)
        {
            //当转换失败时会抛出异常 正常现象
            RCLCPP_ERROR(this->get_logger(), "异常提示: %s", e.what());
        }
        
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<TfListener>("node_name","namespace"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}