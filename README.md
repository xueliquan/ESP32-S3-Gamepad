# ESP32-S3 蓝牙游戏手柄

基于ESP32-S3-WROOM-1-N8R2的17键双摇杆无线蓝牙游戏手柄，支持震动反馈和LED指示。

## 特性
- 17个数字按钮
- 2个模拟摇杆（双轴）
- 2个震动电机（PWM控制）
- 4个可编程LED指示灯
- 蓝牙HID游戏手柄协议
- 可充电锂电池
- 低功耗模式

## 硬件规格
- MCU: ESP32-S3-WROOM-1-N8R2 (Xtensa® dual-core 32-bit LX7, 240MHz)
- 内存: 8MB Flash, 512KB SRAM
- 无线: Wi-Fi 802.11 b/g/n, Bluetooth 5.0
- 电源: 3.7V锂电池，USB Type-C充电
- 接口: 23个GPIO，8个ADC通道，8个PWM通道

## 快速开始

### 硬件连接
参考 `docs/pinout.md` 和 `hardware/schematic/`

### 软件安装
1. 安装 [VS Code](https://code.visualstudio.com/)
2. 安装 [PlatformIO IDE](https://platformio.org/)
3. 克隆本项目：
```bash
git clone https://github.com/yourusername/ESP32-S3-Gamepad.git
cd ESP32-S3-Gamepad
```

### 编译和烧录

1. 使用VS Code打开项目
2. 点击PlatformIO工具栏的编译按钮
3. 连接ESP32-S3开发板
4. 点击上传按钮

### 校准摇杆

```bash
python scripts/calibrate.py
```

## 使用说明

1. 上电后等待蓝牙广播
2. 在设备上搜索并连接 "ESP32 Gamepad"
3. LED指示灯显示连接状态：
   · LED1: 蓝牙连接状态
   · LED2: 震动状态
   · LED3: 低电量警告
   · LED4: 充电状态

## 配置

修改 include/config.h 可以调整：

· 按钮映射
· 摇杆死区
· 震动强度
· LED效果

## 项目结构

```
### 项目结构说明
```
ESP32-S3-Gamepad/
├── README.md
├── LICENSE
├── .gitignore
├── platformio.ini
├── .vscode/
│   └── extensions.json
├── src/
│   ├── main.cpp
│   ├── Gamepad.cpp
│   ├── Gamepad.h
│   ├── Button.cpp
│   ├── Button.h
│   ├── Joystick.cpp
│   ├── Joystick.h
│   ├── VibrationMotor.cpp
│   ├── VibrationMotor.h
│   ├── LEDController.cpp
│   ├── LEDController.h
│   ├── BatteryMonitor.cpp
│   └── BatteryMonitor.h
├── include/
│   └── config.h
├── lib/
│   └── README.md
├── docs/
│   ├── hardware.md
│   └── pinout.md
├── hardware/
│   ├── schematic/
│   │   └── gamepad_schematic.pdf
│   ├── pcb/
│   │   └── gamepad_pcb.kicad
│   └── bom/
│       └── bill_of_materials.csv
├── examples/
│   ├── BasicExample/
│   ├── VibrationTest/
│   └── Calibration/
├── scripts/
│   ├── calibrate.py
│   └── flash.sh
└── data/
    └── calibration.json
	
## 依赖库

· [ESP32-BLE-Gamepad](https://github.com/lemmingDev/ESP32-BLE-Gamepad/)

## 许可证

MIT License - 详见 LICENSE 文件

## 贡献

欢迎提交Issue和Pull Request！