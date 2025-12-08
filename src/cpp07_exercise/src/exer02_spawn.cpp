#include "rclcpp/rclcpp.hpp"
#include "turtlesim/srv/spawn.hpp"
#include "turtlesim/srv/kill.hpp"
#include <chrono>
#include "turtlesim/msg/pose.hpp"
#include <future>   // 为了 std::future_status


/*
    需求:
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建
            3-2实现回调函数
            3-3在回调中解析并输出数据
        4.调用spin函数,并传入节点对象指针
        5.释放资源


    ros2 service type /spawn 
        turtlesim/srv/Spawn

    ros2 interface show turtlesim/srv/Spawn
        float32 x
        float32 y
        float32 theta
        string name # Optional.  A unique name will be created and returned if this is empty
        ---
        string name

    ros2 service type /kill 
turtlesim/srv/Kill

ros
2 interface show turtlesim/srv/Kill 
string name
---


*/
using namespace std::chrono_literals;
class Spawn_Node :public rclcpp::Node{
public:
    Spawn_Node(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),(str1+"节点创建成功").c_str());
        //创建客户端对象
        client_ = this->create_client<turtlesim::srv::Spawn>("/spawn");
        //等待服务端启动
        while (!client_->wait_for_service(1s))
        {
            RCLCPP_INFO(this->get_logger(),"等待服务端启动...");
        }
        pose1_ready_ = false;
        pose2_ready_ = false;


    }

    //主逻辑实现
    void main_class(){ 
        rclcpp::sleep_for(500ms);
        std::flush(std::cout);
        this->menu();
        int choice = 0;
        std::cin>>choice;
        if (choice == 1)
        {
            this->spawn_turtle();
        }
        else if (choice == 2)
        {
            this->delete_turtle();
        }
        else if (choice == 3)
        {
            this->caculates_done_ = false;
            this->calculate_distance();
        }
        else if (choice == 0)
        {
            RCLCPP_INFO(this->get_logger(),"退出程序");
            rclcpp::shutdown();
            return;
        }
        else{
            RCLCPP_ERROR(this->get_logger(),"输入有误,请重新输入");
        }
    }

private:
    rclcpp::Client<turtlesim::srv::Spawn>::SharedPtr client_;
    void menu(){
        std::cout<<"**********菜单**********"<<std::endl;
        std::cout<<"*  1.生成乌龟         *"<<std::endl;
        std::cout<<"*  2.删除乌龟         *"<<std::endl;
        std::cout<<"*  3.计算距离         *"<<std::endl;
        std::cout<<"*  0.退出             *"<<std::endl;
        std::cout<<"请输入选项:";
        return;
    }
    void spawn_turtle(){
        std::cout<<"请输入生成乌龟的名字: ";
        std::string name;
        std::cin>>name;
        std::cout<<"请输入生成乌龟的x坐标: ";
        float x;
        std::cin>>x;
        std::cout<<"请输入生成乌龟的y坐标: ";
        float y;
        std::cin>>y;
        std::cout<<"请输入生成乌龟的朝向角度(-3.14 ~ 3.14): ";
        float theta;
        std::cin>>theta;
        //创建数据类型对象
        auto msg = std::make_shared<turtlesim::srv::Spawn::Request>();
        msg->x = x;
        msg->y = y;
        msg->theta = theta;
        msg->name = name;
        //发送数据
        auto future = client_->async_send_request(msg);
        //等待结果        
        // 因为后台有 spin 线程在跑，这里只需要等待 future 就行
        if (future.wait_for(3s) == std::future_status::ready) {
            auto response = future.get();
            RCLCPP_INFO(this->get_logger(),
                        "乌龟生成成功,名字叫: %s",
                        response->name.c_str());
        } else {
            RCLCPP_ERROR(this->get_logger(),
                        "调用 /spawn 服务超时或失败");
        }
        return;
        
    }

    //删除乌龟
    void delete_turtle(){
        auto topic_name = this->get_topic_names_and_types();
        for (auto &&names : topic_name)
        {
            std::cout<<names.first<<std::endl;
        }
        std::cout<<"请输入要删除乌龟的名字: ";
        std::string name;
        std::cin>>name;
        
        //创建客户端对象
        auto client_kill_ = this->create_client<turtlesim::srv::Kill>("/kill");
        //等待服务端启动
        while (!client_kill_->wait_for_service(1s))
        {
            RCLCPP_INFO(this->get_logger(),"等待服务端启动...");
        }
        //创建数据类型对象
        auto kill_msg = std::make_shared<turtlesim::srv::Kill::Request>();
        kill_msg->name = name;
        //发送数据
        auto kill_future = client_kill_->async_send_request(kill_msg);
        //等待结果
        if (kill_future.wait_for(3s) == std::future_status::ready)
        {
            RCLCPP_INFO(this->get_logger(),"乌龟删除成功");
        }
        else{
            RCLCPP_ERROR(this->get_logger(),"删除乌龟失败");
        }
        return;
    }

    //计算两个乌龟之间的距离
    void calculate_distance(){
        std::cout<<"输入第一只乌龟的名字: ";
        std::string name1;
        std::cin>>name1;
        std::cout<<"输入第二只乌龟的名字: ";
        std::string name2;
        std::cin>>name2;

        //创建接收方端对象
        sub_1_ = this->create_subscription<turtlesim::msg::Pose>(
            "/"+name1+"/pose",10,
            std::bind(&Spawn_Node::pose1_callback,this,std::placeholders::_1)
        );

        sub_2_ = this->create_subscription<turtlesim::msg::Pose>(
            "/"+name2+"/pose",10,
            std::bind(&Spawn_Node::pose2_callback,this,std::placeholders::_1)
        );
        if (caculates_done_)
        {
            return;
        }
        
    }

    void pose1_callback(const turtlesim::msg::Pose::SharedPtr msg){
        if (pose1_ready_)
        {
            sub_1_.reset();
            return;
        }
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"),"乌龟1坐标:(%.2f,%.2f)",msg->x,msg->y);
        pose1_ = *msg;
        pose1_ready_ = true;
        try_calculate_distance();
    }

    void pose2_callback(const turtlesim::msg::Pose::SharedPtr msg){
        if (pose2_ready_)
        {
            sub_2_.reset();
            return;
        }
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"),"乌龟2坐标:(%.2f,%.2f)",msg->x,msg->y);
        pose2_ = *msg;
        pose2_ready_ = true;
        try_calculate_distance();
    }

    void try_calculate_distance(){
        if (!pose1_ready_ || !pose2_ready_)
        {
            return;
        }
        //数据处理
        double distance = std::sqrt(std::pow(pose1_.x - pose2_.x,2) + std::pow(pose1_.y - pose2_.y,2));
        RCLCPP_INFO(this->get_logger(),"两只乌龟之间的距离为: %f",distance);
        this->caculates_done_ = true;

        // 如果只想算一次，可以注销订阅
        if (sub_1_) sub_1_.reset();
        if (sub_2_) sub_2_.reset();
        return;
    }

    turtlesim::msg::Pose pose1_;
    turtlesim::msg::Pose pose2_;
    bool pose1_ready_;
    bool pose2_ready_;
    bool caculates_done_;

    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_1_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_2_;
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    auto node = std::make_shared<Spawn_Node>("spawn_node");

    // 开一个线程专门 spin
    std::thread spin_thread([&]() {
        rclcpp::spin(node);
    });

    while (rclcpp::ok())
    {
        node->main_class();
    }

    //释放资源
    rclcpp::shutdown();
    return 0;
}