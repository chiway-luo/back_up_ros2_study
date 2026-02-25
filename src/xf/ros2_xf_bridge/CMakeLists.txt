cmake_minimum_required(VERSION 3.8)
project(ros2_xf_bridge)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)

# add_executable(v2t src/v2t.cpp)
# target_include_directories(v2t PUBLIC
#   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#   $<INSTALL_INTERFACE:include>)
# target_compile_features(v2t PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17
# ament_target_dependencies(
#   v2t
#   "rclcpp"
#   "std_msgs"
#   "geometry_msgs"
# )

# install(TARGETS v2t
#   DESTINATION lib/${PROJECT_NAME})

#语音转文字

add_executable(v2t src/v2t.cpp src/speech_recognizer.c src/linuxrec.c)

target_link_libraries(v2t
  ${catkin_LIBRARIES}
  libmsc.so -ldl -lpthread -lm -lrt -lasound
)

target_include_directories(v2t PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
ament_target_dependencies(
  v2t
  "rclcpp"
  "std_msgs"
  "geometry_msgs"
)

install(TARGETS v2t
  DESTINATION lib/${PROJECT_NAME})

install(
  DIRECTORY include/
    DESTINATION include
)


############

# 文字转语音
add_executable(t2v src/t2v.cpp)
target_include_directories(t2v PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
ament_target_dependencies(
  t2v
  "rclcpp"
  "std_msgs"
  "geometry_msgs"
)
target_link_libraries(t2v
  ${catkin_LIBRARIES}
  libmsc.so -ldl -pthread 
)
install(TARGETS t2v
  DESTINATION lib/${PROJECT_NAME})


###########

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
