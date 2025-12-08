#include <Arduino.h>
#include "config.h"
#include "Gamepad.h"
#include "BatteryMonitor.h"
#include "Preferences.h"

Gamepad gamepad;
BatteryMonitor batteryMonitor;
Preferences preferences;

unsigned long lastActivityTime = 0;
bool isSleeping = false;

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== ESP32-S3 Gamepad Starting ===");
    
    // 初始化存储
    preferences.begin("gamepad", false);
    
    // 初始化游戏手柄
    if (!gamepad.begin()) {
        Serial.println("Failed to initialize gamepad!");
        while (1) {
            digitalWrite(PIN_LED_CONNECTION, !digitalRead(PIN_LED_CONNECTION));
            delay(100);
        }
    }
    
    // 初始化电池监测
    batteryMonitor.begin(PIN_BATTERY_ADC);
    
    // 加载校准数据
    loadCalibration();
    
    // 设置初始LED状态
    gamepad.leds.setConnectionLED(true);
    
    Serial.println("Gamepad initialized successfully!");
    Serial.print("Device name: ");
    Serial.println(DEVICE_NAME);
    Serial.print("Battery level: ");
    Serial.print(batteryMonitor.getLevel());
    Serial.println("%");
    
    lastActivityTime = millis();
}

void loop() {
    static unsigned long lastUpdate = 0;
    static unsigned long lastBatteryCheck = 0;
    
    unsigned long currentTime = millis();
    
    // 检查电池状态（每分钟一次）
    if (currentTime - lastBatteryCheck > BATTERY_CHECK_INTERVAL) {
        batteryMonitor.update();
        updateBatteryLED();
        lastBatteryCheck = currentTime;
        
        // 低电量警告
        if (batteryMonitor.getLevel() < 20) {
            gamepad.vibration.pulse(255, 100, 3);
        }
    }
    
    // 主更新循环 (100Hz)
    if (currentTime - lastUpdate >= 10) {
        if (!isSleeping) {
            // 读取输入
            gamepad.update();
            
            // 检查活动
            if (gamepad.wasActivity()) {
                lastActivityTime = currentTime;
            }
            
            // 检查休眠超时
            if (currentTime - lastActivityTime > SLEEP_TIMEOUT) {
                enterSleepMode();
            }
        }
        
        lastUpdate = currentTime;
    }
    
    // 处理蓝牙事件
    if (gamepad.isConnected()) {
        gamepad.leds.setConnectionLED(true);
        
        // 发送HID报告
        if (gamepad.needsReport()) {
            gamepad.sendReport();
        }
    } else {
        gamepad.leds.setConnectionLED(false);
        
        // 未连接时闪烁LED
        if (currentTime % 1000 < 500) {
            gamepad.leds.setConnectionLED(true);
        } else {
            gamepad.leds.setConnectionLED(false);
        }
    }
    
    // 处理震动效果
    gamepad.vibration.update();
    
    // 处理LED效果
    gamepad.leds.update();
    
    // 短延迟以降低功耗
    delay(1);
}

void loadCalibration() {
    Serial.println("Loading calibration data...");
    
    if (preferences.isKey("joystick_cal")) {
        size_t len = preferences.getBytesLength("joystick_cal");
        if (len == sizeof(JoystickCalibration)) {
            JoystickCalibration cal;
            preferences.getBytes("joystick_cal", &cal, len);
            gamepad.joysticks.setCalibration(cal);
            Serial.println("Calibration data loaded");
        }
    } else {
        Serial.println("No calibration data found, using defaults");
    }
}

void saveCalibration() {
    JoystickCalibration cal = gamepad.joysticks.getCalibration();
    preferences.putBytes("joystick_cal", &cal, sizeof(cal));
    Serial.println("Calibration data saved");
}

void updateBatteryLED() {
    int level = batteryMonitor.getLevel();
    
    if (batteryMonitor.isCharging()) {
        gamepad.leds.setChargingLED(true);
    } else {
        gamepad.leds.setChargingLED(false);
        
        if (level < 20) {
            // 低电量：快速闪烁
            gamepad.leds.setBatteryLED((millis() % 500) < 250);
        } else if (level < 50) {
            // 中等电量：慢速闪烁
            gamepad.leds.setBatteryLED((millis() % 2000) < 1000);
        } else {
            // 高电量：常亮
            gamepad.leds.setBatteryLED(true);
        }
    }
}

void enterSleepMode() {
    Serial.println("Entering sleep mode...");
    isSleeping = true;
    
    // 关闭所有外设
    gamepad.sleep();
    
    // 配置唤醒源
    esp_sleep_enable_ext0_wakeup((gpio_num_t)WAKEUP_PIN, 0);
    
    // 设置所有GPIO为输入模式以降低功耗
    for (int i = 0; i <= 22; i++) {
        if (i != PIN_BATTERY_ADC) {  // 保持电池监测
            pinMode(i, INPUT);
        }
    }
    
    // 进入深度睡眠
    esp_deep_sleep_start();
}

// 唤醒后的初始化会在setup()中重新执行