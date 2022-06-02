# XVC Teensy 4.0

[tom01h/xvc-pico (github.com)](https://github.com/tom01h/xvc-pico) の Raspberry Pi Pico の部分を Teensy 4.0 に移植したものです。 (さらにもとにしたのは [kholia/xvc-pico: (github.com)](https://github.com/kholia/xvc-pico) )

使い方とdaemonの部分は上を参照ください。

わざわざ入手しにくくて値段の高い (ピコ比) Teensy 4.0 を使ったのは High Speed USB を使いたかったからで。(でもちょっとしか速くなっていない💦)

### 違い

#### ピン配置

| ピン番号 | JTAGピン |
| ---- | ------ |
| 2    | TMS    |
| 3    | TDI    |
| 4    | TDO    |
| 5    | TCK    |

3.3Vデバイスしか使えないのもそのままです

#### コンパイルと書き込み

このディレクトリ `xvc-teensy/` に並べて [hathach/tinyusb: (github.com)](https://github.com/hathach/tinyusb) をクローンする。サブモジュールもアップデートする。

ファームウェアディレクトリ `xvc-teensy/firmware/` に移って、`make BOARD=teensy_40` 実行。

[Teensy Loader Application (pjrc.com)](https://www.pjrc.com/teensy/loader.html) を使って`_build/teensy_40/firmware.hex` を書き込む。

#### データ転送機能

移植はまだです
