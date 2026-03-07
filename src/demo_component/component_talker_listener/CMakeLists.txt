cmake_minimum_required(VERSION 3.8)
project(component_talker_listener)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rclcpp_components REQUIRED)

# add_executable(talker src/talker.cpp)
add_library(talker SHARED src/talker.cpp) #设置为共享库
add_library(listener SHARED src/listener.cpp) #设置为共享库

target_include_directories(talker PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(talker PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17

ament_target_dependencies(
  talker
  "rclcpp"
  "std_msgs"
  "rclcpp_components"
)

ament_target_dependencies(
  listener
  "rclcpp"
  "std_msgs"
  "rclcpp_components"
)

# 注册组件
rclcpp_components_register_nodes(talker "my_componet::TalkerComponent")
rclcpp_components_register_nodes(listener "my_componet::ListenerComponent")

install(TARGETS talker listener
  ARCHIVE DESTINATION lib # 静态库安装路径
  LIBRARY DESTINATION lib # 共享库安装路径
  RUNTIME DESTINATION bin # 可执行文件安装路径
)

install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
