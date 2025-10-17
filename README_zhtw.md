- 繁體中文
- [简体中文](README_zhcn.md)
- [English](README.md)
- [日本語](README_jp.md)

# 萌簡AT指令集
[![License](https://img.shields.io/badge/license-LGPL-blue.svg)](LICENSE)

萌簡 AT 指令集是一套基於 AT 指令的簡單指令集，皆在製作一個人人都能自定義，靈活擴展且簡單易用的 AT 指令集。

萌簡 AT 指令集不同於其它 AT 指令集，它不專用於一個設備，通過簡單可自定義擴展的 AT 指令和 SHELL(msh) 指令，可以讓用戶輕鬆地實現各種功能。

## 特性
- 簡單易用：萌簡AT指令集的指令集設計簡單，指令集的指令數量少，指令集的指令語法簡單。
- 靈活擴展：萌簡AT指令集的指令集可以輕鬆擴展，用戶可以根據自己的需求，添加自己的指令。

## 使用方法
1. 下載並安裝Arduino IDE

前往 [Arduino 官網](https://www.arduino.cc/en/software)下載並安裝 Arduino IDE。
2. 安裝萌簡AT指令集庫

前往 Arduino 庫目錄，通常是 "Documents/Arduino/libraries"

通過 `git clone` 命令下載萌簡AT指令集庫：
``` shell
git clone  https://github.com/Moe-Simple/MoeSimpleAT.git
```
3. 打開Arduino IDE，選擇開發板和埠

可以選擇的板子：
  - ESP8266
  - ESP32
  - Air001(AirMCU)
4. 引入庫檔案：
``` Arduino
#include <MoeSimpleAT.h>
```
5. 開始你的開發之旅！

## 基礎使用
基礎示例代碼
``` Arduino
#include <MoeSimpleAT.h>

#define SERIAL_BAUD_RATE 115200 // 設定串口串列傳輸速率

void setup() {
    atSerial->begin(SERIAL_BAUD_RATE); // 開啟串口
    initATCommands(); // 初始化 AT 指令集
}

void loop() {
    handleATCommands(); // 處理 AT 命令
}
```

仅需几行代码即可，當你編譯成功後，通過串口連接設備，當你看到如下信息：
``` shell
MoeSimple AT System by YooTrans.
ready
```
表示 AT 指令集已初始化成功。

### 內建 AT 命令
指令集庫中存有一些內建命令，你可以輸入 `AT+HELP` 或 `AT+?` 获取幫助，以下是內建命令列表：

- AT: 測試 AT 啟動
- AT+RST: 重啓設備
- AT+GMR: 顯示版本資訊
- AT+RESTORE: 還原設備 (通過onRestore(<還原函數>)； 定義還原回檔，如果沒有則只返回OK)
- AT+UART?: 獲取當前串口串列傳輸速率
- AT+UART=xxx: 設置串口串列傳輸速率
- AT+SYSRAM?: 獲取系統記憶體使用情况 (不支持獲取外部PSRAM)
- AT+SHELL: 進入 SHELL 模式，作為互動終端，輸入 exit 退出
- AT+LOG: 進入日誌輸出模式，此時只輸出日誌，不處理 AT 命令，輸入 EXIT 退出

### 內建 SHELL 命令
- echo <字符串>: 輸出字符串
- free [-b|-k|-m] [-t] [-s delay]: 顯示記憶體使用情況，同 Linux free 命令，可支持內部RAM與外部PSRAM，-b: 位元組，-k: 千位元組，-m: 兆位元組，-t: 顯示總計，-s: 刷新間隔(秒)
- reboot: 重啓設備，同 AT+RST
- shutdown: 關閉設備，通過設定 `wakeupConfigured = true;` 設定喚醒相關邏輯。
- exit: 退出 SHELL 模式
- help: 顯示幫助資訊

## 自定義 AT 命令
你可通過程式中調用 `registerATCommand(<指令>, <回調>, <幫助資訊>)` 來註冊自己的 AT 命令。

回調函數的定義如下：
``` Arduino
void myCallback(String args) {
    // 處理指令參數
    // 輸出結果
    atSerial->println("OK"); // 必須輸出 OK 到串口，作為結束和對 AT 設備的兼容。
}
```

## 自定義 SHELL 命令
你可通過程式中調用 `registerShellCommand(<指令>, <回調>, <幫助資訊>)` 來註冊自己的 SHELL 命令。

回調函數的定義如下：
``` Arduino
void myShellCallback(String args) {
    // 處理指令參數
    // 輸出結果
}
```

## 貢獻
歡迎貢獻！請閱讀 [CONTRIBUTING.md](CONTRIBUTING.md) 瞭解如何參與專案開發。

- Fork 專案
- 創建你的特性分支 (git checkout -b feature/AmazingFeature)
- 提交你的更改 (git commit -m 'Add some AmazingFeature')
- 推送到分支 (git push origin feature/AmazingFeature)
- 打開 Pull Request

## 版本更新
查看 [CHANGELOG.md](CHANGELOG.md) 瞭解版本更新歷史。

## 作者
- [MoeCinnamo](https://github.com/MoeCinnamo)

## 致謝
- 感謝參與開發的人員

## 許可證
本项目採用 [LGPLv2.1](LICENSE) 許可證開源。