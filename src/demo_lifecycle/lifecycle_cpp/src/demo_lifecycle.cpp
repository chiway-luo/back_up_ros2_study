/*
    需求: 编写一个ros2的lifecycle Node,并执行

    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.创建一个生命周期节点类
        4.执行生命周期节点对象
        5.释放资源

    ros2 run lifecycle_cpp demo_lifecycle_cpp

    ros2 lifecycle nodes 查看当前系统中所有的生命周期节点

    ros2 lifecycle list /node_name  查看节点的生命周期状态 可以进入的状态
    改变生命周期指令
    ros2 lifecycle set /node_name configure  配置节点
    ros2 lifecycle set /node_name activate   激活节点
    ros2 lifecycle set /node_name deactivate 停用节点
    ros2 lifecycle set /node_name cleanup    清空节点

    ros2 lifecycle set /node_name shutdown   销毁节点
*/
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"

//创建一个生命周期节点类
class CycleNode: public rclcpp_lifecycle::LifecycleNode{
public:
    CycleNode():LifecycleNode("lifecycle_node_cpp"){
        RCLCPP_INFO(this->get_logger(),"生命周期节点被创建 node_name: %s","lifecycle_node_cpp");
    }
    //重写转换过程相关函数
    // virtual CallbackReturn
    // on_configure(const State & previous_state);
    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_configure(const rclcpp_lifecycle::State & previous_state){
        (void)previous_state;
        RCLCPP_INFO(this->get_logger(),"生命周期节点被配置");
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
    }
    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_activate(const rclcpp_lifecycle::State & previous_state){
        (void)previous_state;
        RCLCPP_INFO(this->get_logger(),"配置-->>激活---------------");
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
    }
    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_deactivate(const rclcpp_lifecycle::State & previous_state){
        (void)previous_state;
        RCLCPP_INFO(this->get_logger(),"激活-->>停用(待激活)---------------");
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
    }
    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_cleanup(const rclcpp_lifecycle::State & previous_state){
        (void)previous_state;
        RCLCPP_INFO(this->get_logger(),"停用(待激活)-->>清空(待配置)---------------");
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
    }
    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_shutdown(const rclcpp_lifecycle::State & previous_state){
        (void)previous_state;
        RCLCPP_INFO(this->get_logger(),"生命周期节点被销毁---------------");
        // rclcpp::shutdown();
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
    }

private:

};

int main(int argc, char **argv){
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //创建一个生命周期节点对象
    auto my_node = std::make_shared<CycleNode>();

    //执行生命周期节点对象
    rclcpp::spin(my_node->get_node_base_interface());

    //释放资源
    rclcpp::shutdown();

    return 0;
}
