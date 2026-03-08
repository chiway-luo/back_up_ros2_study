#include "rclcpp/rclcpp.hpp"
#include "pluginlib/class_loader.hpp"//插件类加载器头文件
#include "nav_planner/base_planner.hpp"//插件基类头文件


/*
    需求:加载自定义的插件类,并调用其函数实现
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1 创建类加载器
            3-2 加载插件
            3-3 调用插件的函数
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间

class LoadPlugin :public rclcpp::Node{
public:
    LoadPlugin(std::string str1):Node(str1){
        //通过参数调用 
        /* 
            ros2 run load_planner load_planner --ros-args -p plugin_type:=planner_clean::PlannerClean
            ros2 run load_planner load_planner --ros-args -p plugin_type:=planner_sample::SamplePlanner
        */
       
        this->declare_parameter<std::string>("plugin_type","planner_sample::SamplePlanner");

        RCLCPP_INFO(this->get_logger(),"namesapce:  node: %s 节点创建成功",str1.c_str());
        // 3-1 创建类加载器
            // ClassLoader(
            // std::string package,
            // std::string base_class,
            // std::string attrib_name = std::string("plugin"),
            // std::vector<std::string> plugin_xml_paths = std::vector<std::string>());
        pluginlib::ClassLoader<nav_planner::BasePlanner> planner_loader(
            "nav_planner",//插件基类所在包名
            "nav_planner::BasePlanner"//插件基类的全名,必须包含命名空间
        );
        // 3-2 加载插件
        // std::shared_ptr<nav_planner::BasePlanner> planner = planner_loader.createSharedInstance("planner_sample::SamplePlanner");//插件类的全名
        // std::shared_ptr<nav_planner::BasePlanner> planner = planner_loader.createSharedInstance("planner_clean::PlannerClean");//插件类的全名
        std::shared_ptr<nav_planner::BasePlanner> planner = planner_loader.createSharedInstance(this->get_parameter("plugin_type").as_string());//插件类的全名
        // 3-3 调用插件的函数
        planner->init(10.0);
        planner->path();
    }

private:

    //回调函数
    void callback_this(){
    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<LoadPlugin>("load_planner_node"));//node_name, (namespace可选)

    //释放资源
    rclcpp::shutdown();
    return 0;
}