/* 
插件实现:
继承基类
重写基类函数

注册插件
*/
#include "nav_planner/base_planner.hpp"
#include "rclcpp/rclcpp.hpp"
#include "pluginlib/class_list_macros.hpp"//插件注册宏,必须放在类定义之后
// #include "planner_sample/planner_sample.hpp"
namespace planner_sample
{

class SamplePlanner: public nav_planner::BasePlanner,public rclcpp::Node{
public:
    SamplePlanner():Node("sample_planner_node"){
        RCLCPP_INFO(this->get_logger(),"简单的路径规划器创建了");

    }
    //初始化函数
    void init(double rate){
        RCLCPP_INFO(this->get_logger(),"简单的路径规划器插件初始化..,frequency: %.2f",rate);
        
    }
    //路径生成函数
    void path(){
        RCLCPP_INFO(this->get_logger(),"正在生成路径...");
    }

private:
};

}  // namespace planner_sample

// PLUGINLIB_EXPORT_CLASS(class_type, base_class_type)
PLUGINLIB_EXPORT_CLASS(planner_sample::SamplePlanner, nav_planner::BasePlanner)