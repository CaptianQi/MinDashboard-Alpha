# 🚀 MinDashboard-Alpha

一个基于 **Qt 6** 的轻量级工业数据监控仪表盘（上位机 Demo 项目）。  
支持 **TCP / UDP / 串口** 通讯、实时仪表盘、实时曲线、日志查看、参数设置和数据记录。

> 当前为 **Alpha 版本**，适合作为：  
> - 工业设备 / 仪表 / 上位机

---

## 📌 功能概览

- 🔌 **多种通讯方式**
  - TCP 客户端
  - UDP 通讯
  - 串口通讯（可配置串口号、波特率、停止位、校验位等）

- 📊 **实时可视化**
  - 自绘仪表盘（ArcGaugeWidget，基于 QPainter）
  - 实时面积图（RealtimeAreaChartWidget）

- 📜 **日志系统**
  - QML TableView 日志列表
  - 不同级别颜色区分（ERROR / WARN / INFO / DEBUG）


- ⚙ **配置管理**
  - 使用 QSettings 保存配置信息到 `settingsconfig.ini`
  - 通讯模式和参数（TCP / UDP / 串口）
  - 数据保存路径
  - 数据记录格式（CSV / JSON）

- 💾 **数据记录**
  - 后台 DataRecorder 模块
  - 支持按指定格式写入文件（如 CSV / JSON）
  - 与 DataModel 对接，可按需启动 / 停止记录

- 🧱 **清晰分层架构**
  - DataManager 作为后端中枢
  - DataSignals 后端数据消息信号中心，前端监听接收数据
  - Gateway 作为前端 → 后端的统一入口
  - Provider 抽象（NetworkProvider / SerialProvider）
  - FrameParser 负责解析数据帧
  - DataModel 负责数据分发

---

## 🗂 目录结构（核心部分）

> 主要目录

```text
.
├── CMakeLists.txt
├── main.cpp
├── mainwindow.cpp / .h / .ui      # 主窗口，导航和页面容器
├── resource.qrc                   # 资源文件（QML / 配置等）
└── src/
    ├── core/
    │   ├── man/
    │   │   ├── datamanager.h/.cpp   # 后端中枢，控制 Provider / Model / Recorder
    │   │   └── gateway.h/.cpp       # 统一 API 入口（send / dispatch）
    │   ├── provider/
    │   │   ├── dataprovider.h/.cpp  # 抽象基类：start/stop/setParams
    │   │   ├── networkprovider.h/.cpp # TCP / UDP 实现
    │   │   └── serialprovider.h/.cpp  # 串口实现
    │   ├── parser/
    │   │   └── framparser.h/.cpp      # 帧解析逻辑
    │   ├── model/
    |   |   ├── datarecorder.h/.cpp    # 数据记录模块
    |   |   ├── datasignals.h/.cpp     # 消息广播中心 
    │       └── datamodel.h/.cpp       # 后端数据 → 消息中心 → 前端UI
    │
    ├── ui/
    │   ├── uicom/
    │   │   ├── gaugewidget.h/.cpp         # 自绘仪表盘
    │   │   └── realtimeareachartwidget.h/.cpp # 实时面积图
    │   ├── pages/
    │   │   ├── dashboardpage.h/.cpp       # 仪表盘页面（QWidget 容器）
    │   │   ├── logpage.h/.cpp             # 日志页面（嵌入 QML）
    │   │   └── settingpage.h/.cpp         # 设置页面（嵌入 QML）
    │   └── qml/
    │       ├── SettingsPage.qml           # 通讯 & 记录参数配置界面
    │       ├── LogPage.qml                # 日志界面（TableView 美化）
    │       └── AppDialog.qml              # 通用弹窗（待集成）
    │
    └── core/log/ …                        # 日志相关
```
## 🧩 架构设计
> 整个数据流大致如下：
```text
[ TCP / UDP / Serial Provider ]
                │ dataReceived(QByteArray)
                ▼
          [ FrameParser ]
                │ frameParsed(...)
                ▼
          [ DataModel ]
           /          \
   frameUpdated()      ─────────► [ DataRecorder：写文件 ]
        │
        └────────────► [ Dashboard / LogPage / QML UI ]
```
> 前端（MainWindow / 各 Page）通过 Gateway 调后端：
```text
[ 前端 UI 页面 ]
      │
      ├─► Gateway::send(API::PROVIDER::SET, params)
      ├─► Gateway::send(API::PROVIDER::START, …)
      └─► Gateway::send(API::RECORDER::SET, …)

                      ▼

         [ DataManager 统一处理 ]

                      ▼

         [ DataSignals 广播消息 ]

                      ▼
             [ 前端 UI 页面 ]
```
## 💻 运行环境
> Qt 版本：推荐 Qt 6.5+
> 工具链：MinGW / MSVC（任选其一）
> 构建系统：CMake
> 操作系统：Windows / Linux（开发主要在 Windows）
