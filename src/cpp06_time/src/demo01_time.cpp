#include "rclcpp/rclcpp.hpp"

/* 
    问题: time和duration有什么区别

    二者只是api使用类似而已,二者有着本质区别
    如果创建time对象,表示一个具体的时刻 表示距离1970年1月1日00:00:00 UTC的时间点过去的纳秒数
    duration表示一个时间段,表示两个time时间点之间的差值

    time: 表示时间点
    duration: 表示持续时间
*/

using namespace std::chrono_literals;

class MyNode :public rclcpp::Node{
public:
    MyNode(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        // this->demo01_rate();
        // this->demo02_time();
        // this->demo03_duration();
        this->demo04_time_duration();
        
    }

private:

    //演示Rate使用
    void demo01_rate(){
        /* 
            Rate
            创建rate类对象 rclcpp::Rate rate(频率)
            rate.sleep() 按照指定频率休眠

        */
        //创建rate对象,设置频率
        rclcpp::Rate rate(1);
        int count = 0;
        while (rclcpp::ok())
        {
            rate.sleep();
            ++count;
            RCLCPP_INFO(this->get_logger(),"当前休眠时间: %ds",count);
        }
    }

    //演示time使用
    void demo02_time(){
        //创建time对象
        rclcpp::Time time1(500000000L); //纳秒
        rclcpp::Time time2(2,500000000L);
        //获取当前时间
        // rclcpp::Time right_now(this->get_clock()->now());
        rclcpp::Time right_now1(this->now());

        //调用Time对象的函数
        RCLCPP_INFO(this->get_logger(),"s = %f ns = %ld",time1.seconds(),time1.nanoseconds());
        RCLCPP_INFO(this->get_logger(),"s = %f ns = %ld",time2.seconds(),time2.nanoseconds());
        RCLCPP_INFO(this->get_logger(),"s = %f ns = %ld",right_now1.seconds(),right_now1.nanoseconds());
    }

    //演示duration使用 持续时间
    void demo03_duration(){
        //创建duration对象
        rclcpp::Duration d1(3,500000000L); //3.5秒
        rclcpp::Duration d2(2,0L); //2秒
        rclcpp::Duration d3(4s); //4秒
        //调用函数
        RCLCPP_INFO(this->get_logger(),"s = %f ns = %ld",d1.seconds(),d1.nanoseconds());
        RCLCPP_INFO(this->get_logger(),"s = %f ns = %ld",d2.seconds(),d2.nanoseconds());
        RCLCPP_INFO(this->get_logger(),"s = %f ns = %ld",d3.seconds(),d3.nanoseconds());
    }

    //演示time和duration之间的运算
    void demo04_time_duration(){
        rclcpp::Time time1(10,0);
        rclcpp::Time time2(30,0);
        rclcpp::Duration d1(8,0);
        rclcpp::Duration d2(17,0);

        //运算
        //比较运算符
        RCLCPP_INFO(this->get_logger(),"t1 > t2 %s",(time1 > time2) ? "是":"否");
        //数学运算
        rclcpp::Duration d3 = time1 - time2;
        rclcpp::Time time3 = time1 + d1;
        rclcpp::Duration d4 = time2 - time1;

        rclcpp::Duration d5 = d1 + d2;
        rclcpp::Duration d6 = d1 - d2;
        RCLCPP_INFO(this->get_logger(),"d6 = d1 - d2 = %f",d6.seconds());

    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<MyNode>("time_node"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}