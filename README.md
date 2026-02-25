第八阶段 语音模块案例
===

## Install & Dependence
- 讯飞语音SDK

## Dataset Preparation
| Dataset | Download |
| ---     | ---   |
| dataset-A | [download]() |
| dataset-B | [download]() |
| dataset-C | [download]() |

## 安装依赖
```
sudo apt install libasound2-dev
sudo apt install sox
```
## use
- 语音转文本
```
ros2 run ros2_xf_bridge v2t --ros-args -p appid:=<self_appid> 66a19765
```

- 语音合成
```
ros2 run ros2_xf_bridge t2v --ros-args -p appid:=<self_appid> 66a19765
```
```
ros2 topic pub -1 /ttswords std_msgs/msg/String "data: 我是水滴鱼"
```