- 简体中文
- [English](README.md)

# 萌简 AT 指令集
[![License](https://img.shields.io/badge/license-LGPL-blue.svg)](LICENSE)

萌简 AT 指令集是一套基于 AT 指令的简单指令集，皆在制作一个人人都能自定义，灵活扩展且简单易用的 AT 指令集。

萌简 AT 指令集不同于其它 AT 指令集，它不专用于一个设备，通过简单可自定义扩展的 AT 指令和 SHELL(msh) 指令，可以让用户轻松地实现各种功能。

## 特性
- 简单易用：萌简 AT 指令集的指令集设计简单，指令集的指令数量少，指令集的指令语法简单。
- 灵活扩展：萌简 AT 指令集的指令集可以轻松扩展，用户可以根据自己的需求，添加自己的指令。

## 使用方法
1. 下载并安装 Arduino IDE
前往 [Arduino 官网](https://www.arduino.cc/en/software)下载并安装 Arduino IDE。
2. 安装萌简 AT 指令集库
前往 Arduino 库目录，通常是 "Documents/Arduino/libraries"
通过git clone命令下载萌简 AT 指令集库：
``` shell
git clone https://github.com/Moe-Simple/MoeSimpleAT.git
```
3. 打开 Arduino IDE，选择开发板和端口
可以选择的板子：
  - ESP8266
  - ESP32
  - Air001(AirMCU)
4. 引入库文件：
  ``` Arduino
  #include <MoeSimpleAT.h>
  ```
5. 开始你的开发之旅！
   
## 基础使用
基础示例代码
``` Arduino
#include <MoeSimpleAT.h>

#define SERIAL_BAUD_RATE 115200 // 设置串口波特率

void setup() {
    atSerial->begin(SERIAL_BAUD_RATE); // 开启串口
    initATCommands(); // 初始化 AT 指令集
}

void loop() {
    handleATCommands(); // 处理 AT 命令
}
```

仅需几行代码即可，当你编译成功后，通过串口连接设备，当你看到如下信息：
``` shell
MoeSimple AT System by YooTrans.
ready
```
说明 AT 指令集已经初始化成功。

### 内建 AT 命令
指令集库中存有一些内建命令，你可以输入 `AT+HELP` 或 `AT+?` 获取帮助，以下是内建命令列表：

- AT: 测试 AT 启动
- AT+RST: 重启设备
- AT+GMR: 获取版本信息
- AT+RESTORE: 还原设备(通过onRestore(<还原函数>);定义还原回调，如果没有则只返回OK)
- AT+UART?: 获取当前串口波特率
- AT+UART=xxx: 设置串口波特率
- AT+SYSRAM?: 获取系统内存使用情况(不支持获取外部 PSRAM)
- AT+SHELL: 进入 SHELL 模式，作为交互终端，输入 exit 退出
- AT+LOG: 进入日志输出模式，此时只输出日志，不处理 AT 命令，输入 EXIT 退出

### 内建 SHELL 命令
- echo <字符串>: 输出字符串
- free [-b|-k|-m] [-t] [-s delay]: 显示内存使用情况，同 Linux free 命令，可支持内部 RAM 与外部 PSRAM，-b: 字节，-k: 千字节，-m: 兆字节，-t: 显示总计，-s: 刷新间隔(秒)
- reboot: 重启设备，同 AT+RST
- exit: 退出 SHELL 模式
- help: 显示帮助信息

## 自定义 AT 命令
你可以通过程序中调用 `registerATCommand(<指令>, <回调>, <帮助信息>)` 来注册自己的 AT 命令。

回调函数的定义如下：
``` Arduino
void myCallback(String args) {
    // 处理指令参数
    // 输出结果
    atSerial->println("OK"); // 必须输出 OK 到串口，作为结束和对 AT 设备的兼容。
}
```

## 自定义 SHELL 命令
你可以通过程序中调用 `registerShellCommand(<指令>, <回调>, <帮助信息>)` 来注册自己的 AT 命令。

回调函数的定义如下：
``` Arduino
void myShellCallback(String args) {
    // 处理指令参数
    // 输出结果
}
```

## 贡献
欢迎贡献！请阅读 [CONTRIBUTING.md](CONTRIBUTING.md) 了解如何参与项目开发。

- Fork 项目
- 创建你的特性分支 (git checkout -b feature/AmazingFeature)
- 提交你的更改 (git commit -m 'Add some AmazingFeature')
- 推送到分支 (git push origin feature/AmazingFeature)
- 打开 Pull Request

## 版本更新
查看 [CHANGELOG.md](CHANGELOG.md) 了解版本更新历史。

## 作者
- [MoeCinnamo](https://github.com/MoeCinnamo)

## 致谢
- 感谢参与开发的人员

## 许可证
本项目基于 [LGPLv2.1](LICENSE) 许可证开源。