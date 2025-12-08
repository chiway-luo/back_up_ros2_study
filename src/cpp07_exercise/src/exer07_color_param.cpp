#include "rclcpp/rclcpp.hpp"


/*
    需求:
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建参数客户端
            3-2连接参数服务端
            3-3更新修改参数
        4.创建节点对象指针,并调用相关函数
        5.释放资源
*/
using namespace std::chrono_literals;

class ParamNode :public rclcpp::Node{
public:
    ParamNode(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        //创建参数客户端
        param_client_ = std::make_shared<rclcpp::SyncParametersClient>(this,"/turtlesim");
    }
    //连接参数服务端
    bool conect_server(){
        while(!param_client_->wait_for_service(1s)){
            RCLCPP_WARN(this->get_logger(),"等待参数服务端连接...");
            if(!rclcpp::ok()){
                RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"程序被终止");
                return false;
            }
        }
        return true;
    }
    //更新参数
    void update_param(){
        //背景色要递进修改
        //background_r [0,255] 5 10 15 ...
        //background_g [0,255] 10 20 30 ...
        //background_b [0,255] 15 30 45 ...

        //1.获取参数
        auto param = param_client_->get_parameters({"background_r","background_g","background_b"});
        int r = param[0].as_int();
        int g = param[1].as_int();
        int b = param[2].as_int();

        //2.获取最接近当前颜色的相位差值
        //在 [0, 2π) 上搜索一个最匹配当前颜色的初始相位 phase_
        int N = 720;//采样点数
        const double TWO_PI = 2.0 * M_PI;
        double min_c = 999.0;//最小误差 初始化一个最大值
        for(int i=0;i<N;++i){
            double phi = TWO_PI * (double)i / (double)N;//0-2pi
            int r0 = static_cast<int>((std::sin(phi)               * 0.5 + 0.5) * 255);
            int g0 = static_cast<int>((std::sin(phi + 2.0*M_PI/3)  * 0.5 + 0.5) * 255);
            int b0 = static_cast<int>((std::sin(phi + 4.0*M_PI/3)  * 0.5 + 0.5) * 255);

            double temp_min = std::pow(r - r0,2)+std::pow(g - g0,2)+std::pow(b - b0,2);
            if (min_c > temp_min){
                min_c = temp_min;
                phase = phi;
            }
        }
        //2.编写循环修改参数(通过休眠控制修改频率)
        rclcpp::Rate rate(30.0); //30Hz
        while(rclcpp::ok()){
            
            phase += 0.02; // 控制变色速度，越大变得越快
            // 保持在 [0, 2π) 里面
            if (phase >= TWO_PI) {
                phase -= TWO_PI;
            }

            r = static_cast<int>((std::sin(phase)               * 0.5 + 0.5) * 255);
            g = static_cast<int>((std::sin(phase + TWO_PI/3)  * 0.5 + 0.5) * 255);
            b = static_cast<int>((std::sin(phase + 2.0*TWO_PI/3)  * 0.5 + 0.5) * 255);
        
            //设置参数
            param_client_->set_parameters({
                rclcpp::Parameter("background_r",r),
                rclcpp::Parameter("background_g",g),
                rclcpp::Parameter("background_b",b)
            });
            rate.sleep();
        }

    }

private:
    rclcpp::SyncParametersClient::SharedPtr param_client_; //参数客户端指针
    double phase = 0.0; //相位变量

    //回调函数
    void callback_this(){
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    auto node = std::make_shared<ParamNode>("param_color_turtle");
    auto futrue = node->conect_server();
    if(!futrue){
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"连接参数服务端失败");
        return 1;
    }
    node->update_param();


    //释放资源
    rclcpp::shutdown();
    return 0;
}