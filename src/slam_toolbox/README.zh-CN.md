## Slam Toolbox（中文译文）

> 说明：此文件为 [README.md](README.md) 的中文翻译，尽量保持与原文结构一致；技术名词、参数名、Topic/Service 名称与代码块均保留原样。若与上游文档有出入，请以英文原文为准。

| DockerHub  | [![Build Status](https://img.shields.io/docker/cloud/build/stevemacenski/slam-toolbox.svg?label=build)](https://hub.docker.com/r/stevemacenski/slam-toolbox) | [![Build Status](https://img.shields.io/docker/pulls/stevemacenski/slam-toolbox.svg?maxAge=2592000)](https://hub.docker.com/r/stevemacenski/slam-toolbox) |
|-----|----|----|
| **Build Farm** | [![Build Status](http://build.ros2.org/job/Ddev__slam_toolbox__ubuntu_bionic_amd64/badge/icon)](http://build.ros2.org/job/Ddev__slam_toolbox__ubuntu_bionic_amd64/) | N/A |

我们收到了用户反馈，并且已有机器人在以下环境中使用 SLAM Toolbox 运行：
- 零售
- 仓储
- 图书馆
- 科研

它也是目前 ROS 2 生态中受支持的 SLAM 库之一。关于如何在 [ROS2 Navigation](https://navigation.ros.org/tutorials/docs/navigation2_with_slam.html) 中使用它，请参考相应教程。

### 引用此工作

你可以在 [这里](https://joss.theoj.org/papers/10.21105/joss.02783) 找到该工作，并点击下方图片查看。

> Macenski, S., Jambrecic I., "SLAM Toolbox: SLAM for the dynamic world", Journal of Open Source Software, 6(61), 2783, 2021.

> Macenski, S., "On Use of SLAM Toolbox, A fresh(er) look at mapping and localization for the dynamic world", ROSCon 2019.

[![IMAGE ALT TEXT](https://user-images.githubusercontent.com/14944147/74176653-f69beb80-4bec-11ea-906a-a233541a6064.png)](https://vimeo.com/378682207)

# 介绍

Slam Toolbox 是一套用于 2D SLAM 的工具与能力集合，由 [Steve Macenski](https://www.linkedin.com/in/steve-macenski-41a985101) 在 [Simbe Robotics](https://www.simberobotics.com/) 任职期间开发，在 Samsung Research 任职期间维护，并且很大程度上是在其业余时间持续维护。

这个项目包含了几乎所有其他可用 SLAM 库（无论开源还是商业）所具备的能力，甚至更多。主要包括：
- 典型的“开箱即用”2D SLAM（启动、建图、保存 pgm 文件），并内置一些实用工具（例如保存地图）
- 可在任意时间继续优化、重建、或在已保存（序列化）的位姿图上继续建图
- 终身建图（life-long mapping）：加载已保存的位姿图后继续建图，同时从新加入的扫描中移除多余信息
- 基于位姿图的优化式定位模式；也可在没有先验地图的情况下运行定位模式，作为“激光里程计（lidar odometry）”模式，并在局部进行回环闭合
- 同步与异步两种建图模式
- 运动学地图融合（并计划加入一种“弹性图”操控的融合技术）
- 基于插件的优化求解器，并提供新的、经过优化的 Google Ceres 插件
- 用于交互操作的 RVIZ 插件
- RVIZ 中的图操作工具：建图时可手动操作节点与连接
- 地图序列化与无损数据存储
- ……更多，但上述是重点

如果要在真实生产机器人上运行，我建议使用 snap：`slam-toolbox`。它包含一些优化，使其速度大约快 10 倍。若你需要其他面向开发者的工具（例如 rviz 插件等，不需要部署到机器人本体），则需要 deb/source 安装。

该包经过基准测试：在约 30,000 平方英尺的建筑内可达到 5 倍以上实时；在约 60,000 平方英尺内可达到 3 倍实时。已知最大同步模式（即无论是否滞后都处理所有扫描）用于约 200,000 平方英尺建筑；在异步模式下可处理更大的空间。

下方视频采集于加州奥克兰的 [Circuit Launch](https://www.circuitlaunch.com/)。感谢 [Silicon Valley Robotics](https://svrobo.org/) 与 Circuit Launch 作为该工作的测试平台。

![map_image](/images/circuit_launch.gif?raw=true "Map Image")

# 支持与贡献

若你对使用或配置有疑问，请将问题发布到 [ROS Answers](answers.ros.org)，社区会尽力回复。代码层面的具体问题或功能需求，请通过 GitHub issues 提交。

如果你希望以实质性的方式贡献该项目，请为你的新功能/补丁提交一个公开的 GitHub issue。若因某些原因该功能开发较为敏感，请通过 `package.xml` 中列出的邮箱地址联系维护者。

所有贡献请在 GitHub issue 与 PR 模板中完整填写必要上下文。所有 PR 必须通过 CI，并在已发布的 ROS 发行版中保持 ABI 兼容。维护者随后会跟进。

# 03/23/2021 关于序列化文件的说明

截至 03/23/2021，序列化文件的内容发生了变化。对该日期之后的新用户而言，本节不影响你。

如果你在此日期之前已经存在序列化文件（例如不是 `pgm` 地图，而是 `.posegraph` 形式序列化的 slam 会话），则为了保持当前特性，你可能需要采取一些操作。遗憾的是，为了修复一个影响任何 360° 或非轴向安装 LiDAR 系统的重大 bug，必须做一次 ABI 破坏性变更。

该问题在 [这篇 Discourse 帖子](https://discourse.ros.org/t/request-for-input-potential-existing-slam-toolbox-serialized-file-invalidation/19520) 中有说明：用于优化器保存扫描数据的坐标系不正确，导致在使用保守损失函数时，360° 与非轴向对齐机器人会出现地图爆炸或翻转。该变更永久修复该问题，但也改变了该数据保存与序列化时的参考坐标系。

如果你的系统不是 360° 雷达，且其坐标系与机器人 base 坐标系对齐，你大概率不会注意到问题，可以忽略该说明。对其他出现问题的用户，有以下选项：
- 使用 `<distro>-devel-unfixed` 分支而非 `<distro>-devel`。该分支包含该发行版发布时的未修复版本，并会与主分支并行维护，便于继续使用你当前可工作的方案
- 使用离线工具将你的序列化文件转换到新的参考坐标系
- 使用原始数据重新运行 SLAM 会话，生成包含正确内容的新序列化文件

更多讨论可见 tickets #198 与 #281。对此带来的不便深表歉意，但它解决了影响大量用户的重大问题。我也尽力确保每位用户都有可行的迁移路径。

# 终身建图（LifeLong Mapping）

<!--  Continuing mapping Gif here-->

终身建图指的是：能够对一个空间进行完整或部分建图，并随着时间推移在持续与空间交互的过程中不断改进与更新地图。我们的方案实现了这一点，并且也考虑到可在云端运行，以及多机器人在共享空间中建图（云端分布式建图）等应用。

虽然 Slam Toolbox 也可以作为“点按式（point-and-shoot）建图”：对空间建图并以传统方式保存为 `.pgm` 文件，但它也允许你将位姿图与元数据以无损方式保存下来，之后可用同一或不同机器人加载并继续建图。

我们的终身建图包含几个关键步骤：
- 通过 Serialization/Deserialization 存储与重新加载地图信息
- 通过 KD-Tree 搜索匹配，在重新初始化时定位机器人处于地图中的位置
- 基于位姿图优化的 SLAM，并抽象出 2D 扫描匹配

这使用户能够创建并更新已有地图，然后将数据序列化用于后续建图会话——这是大多数 SLAM 实现（尤其几乎所有平面 2D SLAM）所缺乏的能力。其他也能做到这一点的库包括 RTab-Map 与 Cartoprapher，但它们也各自有一些“怪癖”，使得它们（在我看来）不太适合用于生产级机器人应用。

本库不仅提供保存数据的机制，也能保存位姿图与相关元数据供你使用。它已被用于地图融合技术（将 2 个或更多序列化对象合并成 1 个全局一致对象）以及连续建图技术（随时间更新同一个序列化地图对象并持续优化）。

相较于 RTab-Map 或 Cartoprapher，其主要优势在于底层（虽然被大量修改过）的 `open_karto` 库更加成熟。Karto 的扫描匹配器在 2D 激光扫描匹配方面以效果优秀著称，许多公司都在使用经过修改的 Karto 版本。

Slam Toolbox 支持主要启动方式：
- 从预定义 dock 启动（假设靠近起始区域）
- 从任意节点启动：选择一个 node ID 作为起始附近节点
- 从任意区域启动：在 map 坐标系中指定当前位姿作为起点，类似 AMCL

在 RVIZ 界面中（见下方章节），你可以通过 ROS services 图形化或编程方式在地图中重新定位或继续建图。

截至撰写时：这里有一个我称为“真正终身（true lifelong）”的**高度**实验性实现，它支持随时间移除节点，也支持添加节点，从而让建图真正具备“终身”能力：通过移除多余或过时信息来约束计算规模。

如果你希望持续精炼地图，建议在云端运行非 full 的 LifeLong 模式以承担更高算力开销。不过一个非常现实且迫切需要的应用，是进行多会话建图：只更新地图的一小部分，或者一次只建图半个区域，最终生成完整（随后静态）的地图用于 AMCL 或 Slam Toolbox 定位模式——这一点它能很好处理。

短期计划是：在 LifeLong 模式中加入节点衰减（decay）以约束计算，并通过改进实验性节点让其可在边缘端运行。在节点衰减实现之前，建议使用 continued mapping（lifelong）先生成完整地图，然后切换到位姿图形变（pose-graph deformation）定位模式，且**你不应看到明显的性能影响**。

# 定位（Localization）

<!-- map refined local area localization Gif here-->

定位模式包含 3 个部分：
- 将已存在的序列化地图加载进节点
- 在位姿图中维护近期扫描的滚动缓冲区（rolling buffer）
- 扫描在缓冲区中过期后会被移除，不会影响底层地图

基于栅格图像（image map）文件的定位方法已存在多年，效果通常还不错。但在学术界并没有太多工作将这些算法精炼到让我满意的程度。另一方面，SLAM 是一个成熟且被充分基准测试的领域。本工作的灵感来自这样一个想法：“能否让定位重新变成 SLAM？”——让我们在定位中也能利用 SLAM 的优点，同时避免计算无界增长。

要启用该模式，在配置文件中设置 `mode: localization`，以便 Ceres 插件正确设置，允许更快地对位姿图中的节点与约束进行 *添加与移除*。这不是严格必须的，但属于性能优化。

定位模式会自动加载你的位姿图，拿到第一帧扫描并与局部区域匹配以进一步精炼位姿估计，然后开始定位。

为了尽量减少从 AMCL 迁移到该模式所需的改动，我们也提供了对 AMCL 用于重定位的 `/initialpose` topic 的订阅，并且它也连接到 RVIZ 的 `2D Pose Estimation` 工具。这样你可以进入我们的定位模式，同时仍然使用你熟悉的 AMCL API，便于集成。

总结来说，我将这种方法称为 `elastic pose-graph localization`：我们在已有地图位姿图中进行定位，并维护一个近期扫描的滚动窗口。这样我们可以用扫描匹配器在已有地图中定位，但在长期层面不更新底层地图，以防出错。

它可以被视作 AMCL 的一种替代方案，结果是你不再需要任何 `.pgm` 地图。若你希望在移动过程中修改底层图，上述 lifelong mapping/continuous slam 模式会更适合。

这种定位方法并不适合所有应用：它需要针对你的机器人进行不少调参，并且需要高质量里程计。如果不确定，你始终可以使用生态中其他 2D 地图定位器，例如 AMCL。对大多数新手或希望开箱体验更好的用户，我会推荐 AMCL。

## 工具

### 基于插件的优化器

我为 ScanSolver 抽象类创建了一个 pluginlib 接口，因此你可以在运行时切换优化器，以便测试不同优化器。

我也为一些可能感兴趣的求解器生成了插件。我喜欢用它们做基准测试，并确保所有测试都跑在同一套代码上。目前支持 Ceres、G2O、SPA 和 GTSAM。

GTSAM/G2O/SPA 目前标记为“非支持（unsupported）”，虽然代码都在。它们并不优于我在下方描述的 Ceres 设置，因此我停止默认编译它们以节省构建时间；但如果你想用，它们依然可用。欢迎提交 PR 实现更多优化器插件。

### 地图融合（Map Merging）——使用序列化原始数据与位姿图的示例

#### 运动学（Kinematic）

该方式使用 RVIZ 与插件加载任意数量的 posegraph，它们会以 `map_N` 的形式显示在 RVIZ 中，并提供一组交互式 marker 让你移动它们。

当你把各个子图按期望的相对关系摆放好后，可以将这些子图合并为全局 `map`，并用你喜欢的 map server 实现下载/使用。

这更像是一个演示：一旦你拥有原始数据，你还能做哪些事情。但除非你习惯手工拼接地图，否则我不认为大多数人会经常用它。

更多信息见下方 RVIZ 插件章节。

### RVIZ 插件

提供了一个 rviz 插件，用于手动回环闭合与在线/离线建图。默认交互模式关闭（允许移动节点）因为这会让 rviz 负担很大。

当你需要移动节点时，勾选 interactive 选项，移动你想移动的内容，然后保存更改以触发一次手动回环闭合。若你操作失误可清除（Clear）。完成后，再退出交互模式。

此外还有工具帮助你控制在线与离线数据：你可以随时停止处理新扫描，或停止把新扫描加入队列。

当你希望机器人静止以让系统“追上进度”时，这很有用（**该选项仅在同步模式下有意义。异步模式下机器人永远不会落后。**），或者你想在手动回环/手动“辅助”时停止处理新扫描。

如果队列里积压太多，你也可以清空它。

另外也暴露了序列化与反序列化服务按钮：加载旧位姿图以继续更新与精炼，或继续建图，然后保存回文件。

“Start By Dock” 复选框会尝试与第一个节点进行扫描匹配（假设你从 dock 开始），以给出一个起始里程计估计。

另一个选项是通过 GUI 输入位置或调用底层 service 从指定位姿开始。你也可以在机器人刚暂停或两次运行间移动不多时，使用当前里程计位置估计。

最后（也是最有用的），你可以使用 RVIZ 的 **2D Pose Estimation** 工具，在 **localization mode** 下告诉它“从哪里开始”，体验与 AMCL 类似。

此外，RVIZ 插件允许你把序列化地图文件作为子图（submaps）添加到 RVIZ 中。它们会以可交互 marker 显示，你可以平移/旋转使其对齐，然后点击 Generate Map 生成复合地图。

此时复合地图会在 `/map` topic 上广播，你可以用 `map_saver` 保存。

建议尽量在 dock 附近继续建图；如果不行，可考虑从位姿启动或地图融合技术。

该 RVIZ 插件主要作为调试工具，但如果你经常在 rviz 中建图，我会建议把它一直开着。所有 RVIZ 按钮都通过 services 实现，主控应用也可以调用这些 services 来控制。

界面如下图。

![rviz_plugin](/images/rviz_plugin.png?raw=true "Rviz Plugin")

### 图操作（Graph Manipulation）

启用 `Interactive Mode` 后，图节点会从普通 marker 变为可交互 marker，你可以操作它们。

当你移动节点后，点击 Save Changes 会将更新后的位置发送给位姿图，并触发一次优化运行，使位姿图根据新的节点位置更新。

当机器人被推挤、打滑、撞墙，或里程计漂移时，这对手动修正很有帮助。

当地图足够大时，RVIZ 中交互 marker 数量可能过多，RVIZ 会开始卡顿。我只推荐将此功能作为测试/调试工具，而不是生产用途。

不过如果你真的能让它在 10,000 个交互 marker 下也能工作，我会立刻合并那个 PR。否则建议只在小地图上使用，或仅在有限时间内快速调整后，取消勾选返回静态模式。

## 指标（Metrics）

如果你像我一样有点“怪”，想知道我如何为 Ceres 优化器得出这些设置，请看下图。

![ceres_solver_comparison](https://user-images.githubusercontent.com/14944147/41576505-a6802d76-733c-11e8-8eca-334da2c8bd50.png)

该数据集展示了在一个大型数据集上“求解时间 vs 位姿图节点数量”的关系。该数据集不是开源的，但足以说明我推荐的设置表现良好。我认为在一般应用中很难超过它，或找到明显更好的求解器类型（底部那条超低曲线，对，就是它）。基准机器为低功耗 7 代 i7。

它可以在合理的 CPU 与内存消耗下绘制 _非常_ 大的空间。我的默认设置在位姿图元素数量上呈 $O(N)$ 增长。

根据大量测试，我推荐在 Ceres 中使用 `SPARSE_NORMAL_CHOLESKY` 求解器与 `SCHUR_JACOBI` 预条件器。

在信赖域策略上，使用 `LM` 与 dogleg 子空间策略效果相当，但 `LM` 支持更好，因此没必要纠结。

如果你的里程计很好（也就是说离群值概率很低），你可以不使用 loss function。

如果你有非典型应用或预计会有轮滑，我可能建议使用 `HuberLoss`，这是一个很通用的损失函数，适合作为起点。所有这些选项及更多都可以通过 ROS 参数服务器配置。

# API

以下为对外暴露的 services/topics。RVIZ 插件中提供了它们的使用示例。

## 订阅的 Topics

| /scan  | `sensor_msgs/LaserScan` | 输入的激光扫描数据 | 
|-----|----|----|
| **tf** | N/A | 从配置的 odom_frame 到 base_frame 的有效 TF 变换 |

## 发布的 Topics

| Topic  | Type | Description | 
|-----|----|----|
| map  | `nav_msgs/OccupancyGrid` | 位姿图的栅格地图表示，以 `map_update_interval` 频率发布 | 
| pose | `geometry_msgs/PoseWithCovarianceStamped` | base_frame 在配置的 map_frame 中的位姿，以及由扫描匹配计算的协方差 |

## 暴露的 Services

| Topic  | Type | Description | 
|-----|----|----|
| `/slam_toolbox/clear_changes`  | `slam_toolbox/Clear` | 清除所有待处理的手动位姿图改动 | 
| `/slam_toolbox/deserialize_map`  | `slam_toolbox/DeserializePoseGraph` | 从磁盘加载已保存的序列化位姿图文件 | 
| `/slam_toolbox/dynamic_map`  | `nav_msgs/OccupancyGrid` | 请求当前位姿图状态对应的栅格地图 | 
| `/slam_toolbox/manual_loop_closure`  | `slam_toolbox/LoopClosure` | 请求处理待处理的手动位姿图改动 | 
| `/slam_toolbox/pause_new_measurements`  | `slam_toolbox/Pause` | 暂停 toolbox 对新进入激光扫描的处理 | 
| `/slam_toolbox/save_map`  | `slam_toolbox/SaveMap` | 保存位姿图生成的地图图像文件，用于显示或 AMCL 定位。它是 `map_server/map_saver` 的简单封装，但很实用。 | 
| `/slam_toolbox/serialize_map`  | `slam_toolbox/SerializePoseGraph` | 保存地图位姿图与相关数据，用于继续建图、slam_toolbox 定位、离线操作等 | 
| `/slam_toolbox/toggle_interactive_mode`  | `slam_toolbox/ToggleInteractive` | 切换交互模式，发布可交互 marker（节点及其位置），供应用更新 |

# 配置（Configuration）

以下设置与选项可供使用。默认配置位于 `config` 目录。

## Solver Params

`solver_plugin` - karto 的 scan solver 所用的非线性求解器类型。可选：`solver_plugins::CeresSolver`, `solver_plugins::SpaSolver`, `solver_plugins::G2oSolver`。默认：`solver_plugins::CeresSolver`。

`ceres_linear_solver` - Ceres 使用的线性求解器。可选：`SPARSE_NORMAL_CHOLESKY`, `SPARSE_SCHUR`, `ITERATIVE_SCHUR`, `CGNR`。默认：`SPARSE_NORMAL_CHOLESKY`。

`ceres_preconditioner` - 与该求解器配套的预条件器。可选：`JACOBI`, `IDENTITY`（无）, `SCHUR_JACOBI`。默认：`JACOBI`。

`ceres_trust_strategy` - 信赖域策略。线搜索策略未暴露，因为在此用例表现较差。可选：`LEVENBERG_MARQUARDT`, `DOGLEG`。默认：`LEVENBERG_MARQUARDT`。

`ceres_dogleg_type` - 当 trust strategy 为 `DOGLEG` 时使用的 dogleg 策略。可选：`TRADITIONAL_DOGLEG`, `SUBSPACE_DOGLEG`。默认：`TRADITIONAL_DOGLEG`

`ceres_loss_function` - 用于拒绝离群测量的损失函数类型。不使用等价于平方损失。可选：`None`, `HuberLoss`, `CauchyLoss`。默认：`None`。

`mode` - "mapping" 或 "localization" 模式，用于 Ceres 问题构建的性能优化

## Toolbox Params

`odom_frame` - Odometry frame

`map_frame` - Map frame

`base_frame` - Base frame

`scan_topic` - scan topic，*绝对*路径，例如 `/scan` 而不是 `scan`

`restamp_tf` - 是否用当前时间重置 TF 消息时间戳，或使用 scan 消息的时间戳。默认 False。

`scan_queue_size` - 在丢弃旧消息前可排队的 scan 消息数量。异步模式下应始终设为 1

`use_map_saver` - 实例化 map saver service 并自订阅 map topic

`map_file_name` - 启动时要加载的位姿图文件名（若可用）

`map_start_pose` - 启动位姿图建图/定位的起始位姿（若可用）

`map_start_at_dock` - 在 dock（第一个节点）处开始加载位姿图（若可用）。若 pose 与 dock 同时设置，将优先使用 pose

`debug_logging` - 将 logger 调整为 debug

`throttle_scans` - 同步模式下对扫描的节流数量

`transform_publish_period` - map 到 odom 的 transform 发布周期。0 表示不发布 transforms

`map_update_interval` - 更新 2D occupancy map 的间隔，供其他应用/可视化使用

`enable_interactive_mode` - 是否允许启用交互模式。交互模式会保留一个将激光扫描与其 ID 关联的缓存，用于可视化，因此进程内存会增加。该选项在 localization 与 lifelong 模式下会手动禁用，因为它们会随时间增加内存使用。对 mapping 或 continued mapping 模式有效。

`position_covariance_scale` - 发布 scan match 位姿时对位置协方差的缩放倍率。可用于调节下游定位滤波器中该位姿测量的影响力。协方差表示测量不确定性，因此增大协方差会使位姿对下游滤波器影响变小。默认：1.0

`yaw_covariance_scale` - 发布 scan match 位姿时对 yaw 协方差的缩放倍率。见 position_covariance_scale 的说明。默认：1.0

`resolution` - 生成的 2D occupancy map 分辨率

`min_laser_range` - 用于 2D occupancy map 光栅化的最小激光距离

`max_laser_range` - 用于 2D occupancy map 光栅化的最大激光距离

`minimum_time_interval` - 同步模式下处理两次扫描的最小时间间隔

`transform_timeout` - 查找 TF 变换的超时时间

`tf_buffer_duration` - 存储 TF 消息用于查询的持续时间。若在同步模式下离线以多倍速度运行，请设高一些。

`stack_size_to_use` - 重置栈大小的字节数，用于启用序列化/反序列化。一个相对宽松的默认值为 40000000，但更小也可以。

`minimum_travel_distance` - 处理新扫描前的最小移动距离

## Matcher Params

`use_scan_matching` - 是否使用 scan matching 来精炼里程计位姿（呃，你为什么不呢？）

`use_scan_barycenter` - 使用重心（barycenter）还是扫描位姿

`minimum_travel_heading` - 触发更新所需的最小航向变化

`scan_buffer_size` - 缓冲到链中的扫描数量；也用于定位模式环形缓冲区中扫描数量

`scan_buffer_maximum_scan_distance` - 扫描距离位姿超过该阈值后将从 buffer 移除

`link_match_minimum_response_fine` - link matching 算法在精细分辨率下通过的最小响应阈值

`link_scan_maximum_distance` - 链接扫描有效的最大距离

`loop_search_maximum_distance` - 用于回环闭合候选扫描的最大距离阈值

`do_loop_closing` - 是否进行回环闭合（如果你不确定，答案是 "true"）

`loop_match_minimum_chain_size` - 回环闭合搜索的最小扫描链长度

`loop_match_maximum_variance_coarse` - 粗搜索通过以进入精炼的最大方差阈值

`loop_match_minimum_response_coarse` - 粗搜索通过以进入精炼的最小响应阈值

`loop_match_minimum_response_fine` - 精细搜索通过的最小响应阈值

`correlation_search_space_dimension` - 扫描相关（scan correlation）搜索网格大小

`correlation_search_space_resolution` - 扫描相关搜索网格分辨率

`correlation_search_space_smear_deviation` - 多模态涂抹（smearing）量，用于平滑响应

`loop_search_space_dimension` - 回环闭合算法搜索网格大小

`loop_search_space_resolution` - 回环闭合搜索网格分辨率

`loop_search_space_smear_deviation` - 多模态涂抹量，用于平滑响应

`distance_variance_penalty` - 当匹配扫描偏离里程计位姿时施加的距离惩罚

`angle_variance_penalty` - 当匹配扫描偏离里程计位姿时施加的角度惩罚

`fine_search_angle_offset` - 精细扫描匹配测试的角度范围

`coarse_search_angle_offset` - 粗扫描匹配测试的角度范围

`coarse_angle_resolution` - 在 Offset 范围内进行扫描匹配的角度分辨率

`minimum_angle_penalty` - 角度的最小惩罚，防止规模发散

`minimum_distance_penalty` - 距离的最小惩罚，防止规模发散

`use_response_expansion` - 若未找到可用匹配，是否自动扩大搜索网格

`min_pass_through` - 一格中必须有多少束光穿过才认为该格可能被占用或空闲。用于防止零散光束干扰地图。

`occupancy_threshold` - 将格子标记为占用所需的最小比例：击中该格的光束数 / 穿过该格的光束数

# 安装（Install）

ROSDep 会处理主要依赖。

```
rosdep install -q -y -r --from-paths src --ignore-src
```

或通过 apt 安装。

```
apt install ros-eloquent-slam-toolbox
```

然后按你习惯的方式运行 colcon build。

你可以通过以下命令运行：`ros2 launch slam_toolbox online_sync_launch.py`

# 其他（Etc）

## NanoFlann!

为了在 continued mapping 与 localization 中更快地执行某些操作，我大量使用了 NanoFlann（致敬！）。

## 关于 snaps 的简要说明

Snap 是完全隔离的容器化软件包，可通过 Canonical 的生态在大量 Linux 发行版上运行。它们类似 Docker 容器，但不共享内核或系统库；所有内容都封装在内部，类似一个基于 Ubuntu Core 的“独立分区操作系统”。

我们以这种方式打包 slam toolbox，能够获得数倍的执行加速，原因在该项目里相当细节且微妙；但一般而言，你不应该指望 snap 一定带来加速。

由于 Snap 完全隔离，且不像 Docker 那样有覆盖（override）标志，因此只有少数固定目录允许 snap 与宿主系统双方读写，其中包括 SNAP_COMMON（通常在 `/var/snap/[snap name]/common`）。

因此，如果你要序列化/反序列化地图，你需要让地图文件能在该目录中访问。

你可以选择把所有序列化地图都存放在那里；需要时把地图移动进去；序列化后再从那里取出；或者使用我最喜欢的方式：使用 `ln` 创建符号链接，把该目录链接到你平时存放地图与序列化输出的目录。

`ln` 示例：

```
#           Source                           Linked
sudo ln -s /home/steve/maps/serialized_map/ /var/snap/slam-toolbox/common
```

然后你只需要在指定要用的地图时，将文件名设为 `slam-toolbox/map_name`，它就能在 snap、docker 或裸机环境下通用。

`-s` 表示符号链接，因此并不是 `/var/snap/slam-toolbox/common/*` 直接包含地图，而是 `/var/snap/slam-toolbox/common/serialized_map/*` 包含地图。

默认情况下，在裸机环境中地图会保存在 `.ros`。

## 更多动图（Gifs）

![map_image](/images/mapping_steves_apartment.gif?raw=true "Map Image")

如果 iRobot 的人能通过关联该地图告诉我 Roomba 的序列号，我请他们吃午饭，可能还会尝试招聘他们。
