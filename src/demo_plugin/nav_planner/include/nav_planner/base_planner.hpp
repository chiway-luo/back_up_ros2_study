/* 
    路径规划器插件的共同基类,在该类中编写所需要的函数
        1.初始化操作
        2.生成路径
*/
#ifndef NAV_PLANNER__BASE_PLANNER_HPP_ //包名称_头文件名称_ 防止重复包含
#define NAV_PLANNER__BASE_PLANNER_HPP_

namespace nav_planner{

class BasePlanner 
{
protected:
    //插件基类必须提供无参构造
    BasePlanner(){}
public:
    virtual void init(double rate) = 0; //纯虚函数,由子类实现
    virtual void path() = 0; //纯虚函数,由子类实现
    
private:
    

};



}//namespace nav_planner



#endif  // NAV_PLANNER__BASE_PLANNER_HPP_