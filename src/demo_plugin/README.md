Demo Plugin 演示插件
===
## 插件组成
- 共同基类
```
被所有插件继承,
声明插件实现的规范
```
- 插件实现
```
继承基类,重写父类函数
```
- 插件使用
```
加载插件,并执行相关函数
```

## 代码架构
- 规范实现
```
共同基类对应一个功能包(头文件)
每种插件都对应单独的功能包
插件使用也由单独的功能包实现
```
- 其他实现方式
```
封装一个包
基类和插件使用封装进一个包,插件实现封装进一个包
```

## 创建指令
### 创建基类功能包
```ros2 pkg create nav_planner --build-type ament_cmake --dependcies rclcpp pluginlib```
### 创建插件实现
```ros2 pkg create planner_sample --build-type ament_cmake --dependcies rclcpp pluginlib nav_planner --library-name planner_sample```
### 创建插件使用
```ros2 pkg create load_planner --build-type ament_cmake --dependcies rclcpp pluginlib nav_planner planner_sample --node-name load_planner```
