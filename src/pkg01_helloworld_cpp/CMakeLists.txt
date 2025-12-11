cmake_minimum_required(VERSION 3.8) # 设置cmake最低版本
project(pkg01_helloworld_cpp) # 设置包名

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

add_executable(helloworld src/helloworld.cpp) # 添加可执行文件
target_include_directories(helloworld PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(helloworld PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17
ament_target_dependencies( # 链接依赖库
  helloworld # 可执行文件名称
  "rclcpp"
)

install(TARGETS helloworld # 安装可执行文件
  DESTINATION lib/${PROJECT_NAME})

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
