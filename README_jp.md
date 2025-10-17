- 日本語
- [English](README.md)
- [简体中文](README_zhcn.md)
- [繁體中文](README_zhtw.md)

# 萌え簡 AT 命令セット
[![License](https://img.shields.io/badge/license-LGPL-blue.svg)](LICENSE)

萌え簡 AT 命令セットは AT 命令に基づく簡単な命令セットで、誰でもカスタマイズでき、柔軟に拡張でき、簡単で使いやすいAT命令セットを作成しています。

萌え簡 AT 命令セットは他の AT 命令セットとは異なり、1つのデバイスに専用ではなく、拡張された AT 命令と SHELL(msh) 命令を簡単にカスタマイズすることで、ユーザーが簡単に様々な機能を実現することができます。

## 特徴
- 簡単で使いやすい：萌え簡 AT 命令セットの命令セットの設計は簡単で、命令セットの命令数は少なく、命令セットの命令文法は簡単である。
- 柔軟な拡張：萌え簡 AT 命令セットの命令セットは簡単に拡張でき、ユーザーは自分のニーズに応じて、自分の命令を追加することができる。

## 使い方
1. Arduino IDE のダウンロードとインストール

[Arduino Website](https://www.arduino.cc/en/software) Arduino IDE のダウンロードとインストール
2. 萌え簡AT命令セットライブラリのインストール

Arduinoライブラリディレクトリへ、通常は "Documents/Arduino/libraries"

萌え簡AT命令セットライブラリを `git clone` コマンドでダウンロードするには：
``` shell
git clone https://github.com/Moe-Simple/MoeSimpleAT.git
```
3. Arduino IDE を開き、開発ボードとポートを選択
選択できるボード:

  - ESP8266
  - ESP32
  - Air001(AirMCU)
4. ライブラリファイルをインポートするには：
  ``` Arduino
  #include <MoeSimpleAT.h>
  ```
5. あなたの開発の旅を始めます！

## 基本的な使用方法
ベースサンプルコード
``` Arduino
#include <MoeSimpleAT.h>

#define SERIAL_BAUD_RATE 115200 // シリアルポートボーレートの設定

void setup() {
    atSerial->begin(SERIAL_BAUD_RATE); // オープンシリアルポート
    initATCommands(); // AT 命令セットの初期化
}

void loop() {
    handleATCommands(); // AT コマンドの処理
}
```

コードを数行だけでいいので、コンパイルに成功したら、シリアルポートでデバイスを接続して、次の情報を見ると：
``` shell
MoeSimple AT System by YooTrans.
ready
```
AT 命令セットの初期化が成功したことを示しています。

### 組み込み AT コマンド
命令セットライブラリにはいくつかの組み込み命令が保存されています。あなたは `AT+HELP` または `AT+?` ヘルプを取得します。以下は組み込みコマンドのリストです。

- AT: AT 起動のテスト
- AT+RST: デバイスの再起動
- AT+GMR: バージョン情報の取得
- AT+RESTORE: デバイスをリストアする (onRestore(<リストア関数>)、リストアコールバックを定義し、なければOKのみを返す)
- AT+UART?: 現在のシリアルポートボーレートの取得
- AT+UART=xxx: シリアルポートボーレートの設定
- AT+SYSRAM?: システムメモリ使用量の取得 (外部PSRAMの取得はサポートされていません)
- AT+SHELL: SHELL モードに入り、インタラクティブ端末として exit 終了を入力
- AT+LOG: ログ出力モードに入り、このときログのみ出力し、AT コマンドを処理せず、EXIT 終了を入力する

### 内蔵 SHELL コマンド
- echo <文字列>: 出力文字列
- free [-b|-k|-m] [-t] [-s delay]: メモリの使用状況を表示し、Linux free コマンドと同様に、内部 RAM と外部 PSRAM をサポートし、-b:バイト、-k:KB、-m:MB、-t:合計を表示し、-s:リフレッシュ間隔（秒）
- reboot: デバイスを再起動し、`AT+RST` と
- shutdown: デバイスを閉じて、 `wakeupConfigured=true;` を設定することにより、ウェイクアップ関連論理を設定します。
- exit: SHELL モードを終了する
- help: ヘルプ情報の表示

## カスタム AT コマンド
プログラム内で `registerATCommand(<コマンド>、<コールバック>、<ヘルプ情報>)` を呼び出すことで、自分の AT コマンドを登録することができます。

コールバック関数の定義は以下のようになります：
``` Arduino
void myCallback(String args) {
    // 処理指令パラメータ
    // 出力結果
    atSerial->println("OK"); // 必ず AT デバイスとの互換性のため、シリアルポートに OK を出力する必要があります。
}
```

## カスタム SHELL コマンド
プログラム内で `registerShellCommand(<コマンド>、<コールバック>、<ヘルプ情報>)` を呼び出すことで、自分のSHELLコマンドを登録することができます。

コールバック関数の定義は以下のようになります：
``` Arduino
void myShellCallback(String args) {
    // 処理指令パラメータ
    // 出力結果
}
```

## 貢献
貢献を歓迎します！プロジェクト開発への参加方法については、[CONTRIBUTING.md](CONTRIBUTING.md) を参照してください。

- Fork プロジェクト
- あなたのプロパティ分岐を作成する (git checkout -b feature/AmazingFeature)
- 変更を送信 (git commit -m 'Add some AmazingFeature')
- ブランチへのプッシュ (git push origin feature/AmazingFeature)
- 開ける Pull Request

## バージョンの更新
詳細については [CHANGELOG.md](CHANGELOG.md) 了解バージョン更新履歴。

## 著者
- [MoeCinnamo](https://github.com/MoeCinnamo)

## 謝辞
- 感谢参与開発者

## ライセンス
本プロジェクトは [LGPLv2.1](LICENSE) ライセンスでオープンソースとして公開されています。