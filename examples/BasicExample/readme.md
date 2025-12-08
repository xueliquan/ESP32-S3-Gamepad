# 基础示例

这个示例展示了如何使用游戏手柄库的基本功能。

## 功能
- 按钮输入检测
- 摇杆数据读取
- 震动控制
- LED状态显示

## 代码示例
```cpp
#include <Gamepad.h>

Gamepad gamepad;

void setup() {
    Serial.begin(115200);
    gamepad.begin();
}

void loop() {
    gamepad.update();
    
    // 检查按钮A是否按下
    if (gamepad.buttons[ButtonMap::BUTTON_A].isPressed()) {
        Serial.println("Button A pressed");
        
        // 触发震动
        gamepad.vibration.pulse(255, 255, 500);
    }
    
    // 读取摇杆数据
    JoystickState state = gamepad.joysticks.getState();
    if (abs(state.x1 - 32768) > 1000) {
        Serial.print("Joystick 1 moved: ");
        Serial.print(state.x1);
        Serial.print(", ");
        Serial.println(state.y1);
    }
    
    delay(10);
}
```

更多示例

1. VibrationTest/ - 震动电机测试
2. Calibration/ - 摇杆校准
3. BatteryMonitor/ - 电池监测
4. SleepMode/ - 低功耗模式