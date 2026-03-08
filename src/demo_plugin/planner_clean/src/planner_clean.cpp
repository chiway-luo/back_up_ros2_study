#include "rclcpp/rclcpp.hpp"
#include "pluginlib/class_list_macros.hpp"//插件注册
#include "nav_planner/base_planner.hpp"//插件基类头文件

namespace planner_clean{
using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间

class PlannerClean :public nav_planner::BasePlanner,public rclcpp::Node{
public:
    PlannerClean():Node("clean_planner_node"){
        RCLCPP_INFO(this->get_logger(),"扫地机器人的路径规划器节点创建成功");
    }
    void init(double rate){
        RCLCPP_INFO(this->get_logger(),"扫地机器人的路径规划器插件初始化..,frequency: %.2f",rate);
    }
    void path(){
        RCLCPP_INFO(this->get_logger(),"扫地机器人正在规划全覆盖路径");
    }

private:

    //回调函数
    void callback_this(){
    }
};


}//namespace planner_clean

PLUGINLIB_EXPORT_CLASS(planner_clean::PlannerClean, nav_planner::BasePlanner)
