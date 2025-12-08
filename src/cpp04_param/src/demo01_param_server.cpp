#include "rclcpp/rclcpp.hpp"


/*  
    需求:创建参数服务端并操作参数 一个普通节点就可以作为参数服务端
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1声明参数 增
            3-2查询参数 查
            3-3修改参数 改
            3-4删除参数 删
        4.创建节点对象指针,调用参数操作函数,并传递给spin函数
        5.释放资源
*/

class ParameterNode :public rclcpp::Node{
public:
    ParameterNode(std::string str1):Node(str1,rclcpp::NodeOptions().allow_undeclared_parameters(true)){
        RCLCPP_INFO(this->get_logger(),(str1+"参数服务端创建成功").c_str());        
    }

    // 3-1声明参数 增
    void declare_param(){
        RCLCPP_INFO(this->get_logger(),"---------增---------");
        this->declare_parameter("sdy_name","水滴鱼");
        this->declare_parameter("sdy_age",18);
        this->declare_parameter("sdy_height",1.75);
        //可以使用set_parameter()函数未声明新增参数 但是必须保证rclcpp::NodeOptions().allow_undeclared_parameters(true)被调用
        //且只能删除set_parameter()函数未声明新增的参数
        this->set_parameter(rclcpp::Parameter("sdy_weight",65.5));

    }
        
    // 3-2查询参数 查
    void get_param(){
        RCLCPP_INFO(this->get_logger(),"---------查---------");
        // this->get_parameter()
        // this->get_parameters()
        //this->has_parameter()
        //获取指定参数
        auto name = this->get_parameter("sdy_name");
        RCLCPP_INFO(this->get_logger(),"name:%s,value:%s,type:%s",name.get_name().c_str(),name.value_to_string().c_str(),name.get_type_name().c_str());

        //获取一些参数
        auto params = this->get_parameters({"sdy_name","sdy_age","sdy_height","sdy_weight"});
        for (auto &&param : params)
        {
            RCLCPP_INFO(this->get_logger(),"name:%s,value:%s",param.get_name().c_str(),param.value_to_string().c_str());
        }

        //判断是否包含指定参数
        RCLCPP_INFO(this->get_logger(),"是否包含sdy_name参数:%s",this->has_parameter("sdy_name")?"是":"否");
        RCLCPP_INFO(this->get_logger(),"是否包含sdy_sdy参数:%s",this->has_parameter("sdy_sdy")?"是":"否");
    }
    // 3-3修改参数 改
    void update_param(){
        RCLCPP_INFO(this->get_logger(),"---------改---------");
        this->set_parameter(rclcpp::Parameter("sdy_name","水滴鱼是我妈"));
        this->set_parameters({rclcpp::Parameter("sdy_age",45),rclcpp::Parameter("sdy_height",159)});
        this->get_param();
    }
    // 3-4删除参数 删 要想删除参数,需要先声明rclcpp::NodeOptions().allow_undeclared_parameters(true)
    //且不能删除声明新增的参数,只能删除使用set_parameter()函数未声明新增的参数
    void del_param(){
        RCLCPP_INFO(this->get_logger(),"---------删---------");
        this->undeclare_parameter("sdy_weight");
        RCLCPP_INFO(this->get_logger(),"sdy_weight参数是否存在:%s",this->has_parameter("sdy_weight")?"是":"否");
    }
private:
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    auto param_node = std::make_shared<ParameterNode>("Parameter_server"); 

    param_node->declare_param();
    param_node->get_param();
    param_node->update_param();
    param_node->del_param();

    rclcpp::spin(param_node);

    //释放资源
    rclcpp::shutdown();
    return 0;
}