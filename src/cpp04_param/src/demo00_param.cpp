/*
    在ROS2中,餐食由键/值/和描述符三部分u成,其中键是字符串类型,值可以是bool,
    int64,float64,string,bytearray,bool[],int64[],float64[],string[]中的
    任一类型,描述符默认情况下为空,但是可以设置参数描述,参数数据类型,或其他约束信息

    为了方便操作,但是被承装为了相关类,其中c++客户端对应的类是rclcpp::Parameter,python客户端对应的类是
    rclpy.Parameter 借助于相关api,我们可以实现参数对象创建以及参数属性解析等操作
*/
#include "rclcpp/rclcpp.hpp"


/*
    需求:
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建参数对象
            3-2解析参数对象获取键/获取值/获取描述符
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/

class ParameterNode :public rclcpp::Node{
public:
    ParameterNode(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),(str1+"|param节点创建成功").c_str());
        //创建参数对象
        rclcpp::Parameter param1("sdy_name","水滴鱼");
        rclcpp::Parameter param2("sdy_age",18);
        rclcpp::Parameter param3("sdy_height",1.75);
        rclcpp::Parameter param4("sdy_hobby",std::vector<std::string>{"看书","编程","旅游"});
        //解析参数对象值
        RCLCPP_INFO(this->get_logger(),"sdy_name = %s",param1.as_string().c_str());
        RCLCPP_INFO(this->get_logger(),"sdy_age = %ld",param2.as_int());
        RCLCPP_INFO(this->get_logger(),"sdy_height = %.2f",param3.as_double());
        auto hobby = param4.as_string_array();
        std::string hobby_str;
        for(auto & item : hobby){
            hobby_str += item + " ";
        }
        std::cout<<"sdy_hobby = "<<hobby_str<<std::endl;
        //获取对象键
        RCLCPP_INFO(this->get_logger(),"param1 key = %s",param1.get_name().c_str());
        RCLCPP_INFO(this->get_logger(),"param2 key = %s",param2.get_name().c_str());
        RCLCPP_INFO(this->get_logger(),"param3 key = %s",param3.get_name().c_str());
        RCLCPP_INFO(this->get_logger(),"param4 key = %s",param4.get_name().c_str());
        //整型转换为字符串
        RCLCPP_INFO(this->get_logger(),"param3tostring = %s",param3.value_to_string().c_str());

        // rclcpp::ParameterValue value(,);
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
    rclcpp::spin(std::make_shared<ParameterNode>("Parameter_sdy"));

    //释放资源
    rclcpp::shutdown();
    return 0;
}