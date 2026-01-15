#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"

/*
    需求:发布一张单目相机图片
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建图片发布方对象
            3-2创建定时器对象
            3-3
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间

class SimCam :public rclcpp::Node{
public:
    SimCam(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),"namesapce:  node: %s 节点创建成功",str1.c_str());
        //创建图片发布方对象
        pub_ = this->create_publisher<sensor_msgs::msg::Image>("image",10);
        //创建定时器对象
        timer_ = this->create_wall_timer(50ms,std::bind(&SimCam::callback_this,this));
        //创建图片消息
        image_msg_ = std::make_shared<sensor_msgs::msg::Image>();
        init_image();//初始化图片消息

    }

private:
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr pub_;//图片发布方对象
    rclcpp::TimerBase::SharedPtr timer_;//定时器对象
    std::shared_ptr<sensor_msgs::msg::Image> image_msg_;//图片消息对象
    bool init_flag_ = false;//图片消息初始化标志位
    int arr[2] = {0,0};//用于存储x,y方向位置
    bool flag_x = true; //x方向移动标志位 真为向右-->>移动 假为向左<<--移动

    //回调函数
    void callback_this(){
        if (!init_flag_)
        {
            return;
        }
        
        //分配内存
        // image_msg_->data.resize(image_msg_->height * image_msg_->step);
        //判断x方向是否越界
        if((this->arr[0] >= int(image_msg_->width - 25) && this->flag_x) || (this->arr[0] <= 25 && !this->flag_x)){
            this->flag_x = !this->flag_x; //改变移动方向
        }
        //模拟图像数据 0~255代表灰度值 (这里只是模拟数据，实际应用中应填充真实图像数据)
        //根据成员变量实现移动的矩形碰撞效果(平移)
        //矩形中心点为arr 长度为50x50 像素
        this->arr[1] = image_msg_->height / 2; //y方向位置固定在中间
        this->flag_x? this->arr[0] += 5 : this->arr[0] -= 5; //x方向位置每次移动5个像素

        image_msg_->data.clear();
        image_msg_->data.resize(image_msg_->height * image_msg_->step, 0); //初始化为黑色背景
        //填充矩形区域为白色
        for(int i = this->arr[0] - 25; i < this->arr[0] + 25; i++){//x方向
            for(int j = this->arr[1] - 25; j < this->arr[1] + 25; j++){//y方向
                int index = j * image_msg_->step + i * 3;//计算像素在data数组中的起始位置
                image_msg_->data[index] = 255;     //R
                image_msg_->data[index + 1] = 255; //G
                image_msg_->data[index + 2] = 255; //B
            }
        }
        image_msg_->header.stamp = this->now();//更新时间戳
        //发布图片消息
        pub_->publish(*image_msg_);
    }

    void init_image(){//初始化图片消息
        image_msg_->header.frame_id = "camera_frame";
        image_msg_->height = 480;
        image_msg_->width = 640;
        image_msg_->encoding = "rgb8";//一个像素由三原色组成,每个元素占用8个bit(一个字节)
        image_msg_->is_bigendian = false;//小端字节序(是否是大端字节序)
        image_msg_->step = image_msg_->width * 3; //每一行的字节数 = 宽度 * 每个像素的字节数
        init_flag_ = true;//图片初始化完成
        return;
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<SimCam>("SimCam_node_cpp"));//node_name, (namespace可选)

    //释放资源
    rclcpp::shutdown();
    return 0;
}