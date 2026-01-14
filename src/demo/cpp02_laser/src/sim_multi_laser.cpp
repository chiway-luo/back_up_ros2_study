#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"

/*
    需求:模仿多线激光雷达发布的点云数据,圆圈包围向内收缩
    流程:
        1.包含头文件
        2.初始化ros2客户端
        3.自定义节点类
            3-1创建点云数据发布方
            3-2创建定时器对象
            3-3创建点云数据对象
        4.调用spin函数,并传入节点对象指针
        5.释放资源
*/
/*
    本消息包含一个 N 维点的集合，可能包含诸如法线、强度等附加信息。
    点数据以二进制块存储，其布局由 "fields" 数组描述。

    点云数据可组织为 2D（类似图像）或 1D（无序）。
    以 2D 组织的点云可能由深度相机（如立体摄像头或飞行时间传感器）产生。

    传感器数据采集时间和坐标系 ID（针对 3D 点）。
    std_msgs/Header header
            builtin_interfaces/Time stamp
                    int32 sec
                    uint32 nanosec
            string frame_id

    点云的二维结构。如果点云无序，则 height 为 1，width 为点云长度。
    uint32 height
    uint32 width

    描述通道及其在二进制数据块中的布局。
    PointField[] fields
            uint8 INT8    = 1
            uint8 UINT8   = 2
            uint8 INT16   = 3
            uint8 UINT16  = 4
            uint8 INT32   = 5
            uint8 UINT32  = 6
            uint8 FLOAT32 = 7 占用4字节
            uint8 FLOAT64 = 8 占用8字节
            string name      # 字段名
            uint32 offset    # 偏移量（字节）
            uint8  datatype  # 数据类型标识
            uint32 count     # 元素个数

    bool    is_bigendian # 数据是否为大端序？
    uint32  point_step   # 每个点的字节长度
    uint32  row_step     # 每行的字节长度
    uint8[] data         # 实际点数据，大小为 (row_step * height)

    bool is_dense        # 若无无效点则为 true
*/

using namespace std::chrono_literals; //使用时间命名空间
using namespace std::placeholders; //占位符命名空间

class MultiLaser :public rclcpp::Node{
public:
    MultiLaser(std::string str1):Node(str1){
        RCLCPP_INFO(this->get_logger(),"namesapce:  node: %s 节点创建成功",str1.c_str());
        //3-1创建点云数据发布方
        pub_laser_ = this->create_publisher<sensor_msgs::msg::PointCloud2>("point_cloud",10);
        //3-2创建定时器对象
        timer_ = this->create_wall_timer(50ms,std::bind(&MultiLaser::callback_this,this));
        //3-3创建点云数据对象
        laser_msg_ = std::make_shared<sensor_msgs::msg::PointCloud2>();
    }

private:
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_laser_;//点云数据发布方
    rclcpp::TimerBase::SharedPtr timer_;//定时器对象
    std::shared_ptr<sensor_msgs::msg::PointCloud2> laser_msg_;//点云数据对象
    bool is_laser_init_flag = false;//点云数据初始化标志位

    //回调函数
    void callback_this(){
        if(!is_laser_init_flag){
            init_laser_msg();
            return;
        }
        //生成点云数据
        static float distance = 10.0;
        //清空数据
        std::fill(laser_msg_->data.begin(),laser_msg_->data.end(),0);
        //填充数据
        for(size_t i = 0;i < laser_msg_->height;++i){//32线
            for(size_t j = 0;j < laser_msg_->width;++j){//1800点
                //计算点在data中的起始位置
                size_t index = i * laser_msg_->row_step + j * laser_msg_->point_step;

                float angle_h = (float)j / (float)laser_msg_->width * 2.0 * M_PI - M_PI;//水平角度-pi~PI
                float angle_v = ((float)i - (float)laser_msg_->height / 2) * M_PI / (float)laser_msg_->height;//垂直角度-16~16线  -90~90度
                    
                //圆球实现
                float x = distance * cos(angle_v) * sin(angle_h);
                float y = distance * cos(angle_v) * cos(angle_h); 
                float z = distance * sin(angle_v);
                
                //圆柱实现 X=Rcosθ、Y=Rsinθ
                // float x = distance * cos(angle_h);
                // float y = distance * sin(angle_h);
                // float z = ((float)i - (float)laser_msg_->height / 2) * 0.1; //每线间隔0.1米

                //平面实现 长4 高不考虑
                // float x = distance;
                // float y = ((float)j - (float)laser_msg_->width / 2) * 0.01; //每点间隔0.01米  
                // float z = ((float)i - (float)laser_msg_->height / 2) * 0.02; //每线间隔0.1米

                // if(y > 2 || y < -2){
                //     continue;
                // }


                float intensity = 100.0;

                //拷贝数据到data中
                std::memcpy(&laser_msg_->data[index + 0],&x,sizeof(float));
                std::memcpy(&laser_msg_->data[index + 4],&y,sizeof(float));
                std::memcpy(&laser_msg_->data[index + 8],&z,sizeof(float));
                std::memcpy(&laser_msg_->data[index + 12],&intensity,sizeof(float));
            }
        }

        //更新header
        laser_msg_->header.stamp = this->get_clock()->now();

        distance -= 0.1;
        if(distance < 1.0){
            distance = 10.0;
        }

        
        //发布点云数据
        pub_laser_->publish(*laser_msg_);
    }

    //初始化点云数据
    void init_laser_msg(){
        laser_msg_->header.frame_id = "laser";
        laser_msg_->height = 32; //激光雷达一圈1800个点 1800列
        laser_msg_->width = 1800;  //激光雷达32线 32行
        laser_msg_->is_dense = true; //点云数据不允许有无效点
        laser_msg_->point_step = 16; //每个点4字节 * 4个字段 每个点的步长值
        laser_msg_->row_step = laser_msg_->point_step * laser_msg_->width;//每行字节数
        laser_msg_->is_bigendian = false;//小端序
        /* 
            在 RViz2 里，默认的 XYZ Position Transformer 通常只认 FLOAT32 的 x/y/z（你现在用的是 FLOAT64），
            因此它找不到可用的 position transformer，就报这个错。
        */
        //定义点云数据字段
        sensor_msgs::msg::PointField field_x;
        field_x.name = "x";
        field_x.offset = 0;
        field_x.datatype = sensor_msgs::msg::PointField::FLOAT32;//7 4
        field_x.count = 1;
        sensor_msgs::msg::PointField field_y;
        field_y.name = "y";
        field_y.offset = laser_msg_->point_step / 4;
        field_y.datatype = sensor_msgs::msg::PointField::FLOAT32;//7 4
        field_y.count = 1;
        sensor_msgs::msg::PointField field_z;
        field_z.name = "z";
        field_z.offset = laser_msg_->point_step / 2;
        field_z.datatype = sensor_msgs::msg::PointField::FLOAT32;//7 4
        field_z.count = 1;
        sensor_msgs::msg::PointField field_intensity;
        field_intensity.name = "intensity";
        field_intensity.offset = laser_msg_->point_step / 4 * 3;
        field_intensity.datatype = sensor_msgs::msg::PointField::FLOAT32;//7 4
        field_intensity.count = 1;

        laser_msg_->fields.push_back(field_x);
        laser_msg_->fields.push_back(field_y);
        laser_msg_->fields.push_back(field_z);
        laser_msg_->fields.push_back(field_intensity);

        laser_msg_->data.resize(laser_msg_->row_step * laser_msg_->height);


        is_laser_init_flag = true;
        return;

    }
};

int main(int argc, char * argv[])
{
    //初始化ros2客户端
    rclcpp::init(argc,argv);

    //调用spin函数,使用自定义类对象指针
    rclcpp::spin(std::make_shared<MultiLaser>("MultiLaser_node_cpp"));//node_name, (namespace可选)

    //释放资源
    rclcpp::shutdown();
    return 0;
}