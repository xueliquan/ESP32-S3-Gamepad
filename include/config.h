#ifndef CONFIG_H
#define CONFIG_H

// 设备配置
#define DEVICE_NAME         "ESP32 Gamepad"
#define DEVICE_MANUFACTURER "ESP32"
#define DEVICE_MODEL        "Gamepad v1.0"
#define SERIAL_NUMBER       "ESP32-GAMEPAD-001"

// 蓝牙配置
#define BLE_APPEARANCE  0x03C4  // 游戏手柄
#define BLE_INTERVAL_MIN 0x0006  // 7.5ms
#define BLE_INTERVAL_MAX 0x000C  // 15ms

// 按钮配置
#define BUTTON_COUNT 17
#define DEBOUNCE_DELAY 20  // ms

// 摇杆配置
#define JOYSTICK_DEADZONE 50
#define JOYSTICK_MIN 0
#define JOYSTICK_MAX 4095
#define JOYSTICK_CENTER 2048
#define ADC_SAMPLES 5      // 采样次数
#define ADC_FILTER_ALPHA 0.3  // 滤波器系数

// 震动配置
#define VIBRATION_MAX 255
#define VIBRATION_MIN 0
#define VIBRATION_DEFAULT_DURATION 1000  // ms

// LED配置
#define LED_BRIGHTNESS 128  // 0-255
#define LED_BLINK_INTERVAL 500  // ms

// 电池配置
#define BATTERY_PIN 5
#define BATTERY_MIN_VOLTAGE 3.3
#define BATTERY_MAX_VOLTAGE 4.2
#define BATTERY_CHECK_INTERVAL 60000  // ms

// 省电配置
#define SLEEP_TIMEOUT 300000  // 5分钟无操作进入休眠
#define WAKEUP_PIN 0          // 任意按钮唤醒

// 引脚映射
enum Pins {
    // 按钮引脚 (0-15 + 16)
    PIN_BUTTON_START = 0,
    PIN_BUTTON_SELECT = 1,
    PIN_BUTTON_HOME = 2,
    PIN_BUTTON_A = 3,
    PIN_BUTTON_B = 4,
    PIN_BUTTON_X = 5,
    PIN_BUTTON_Y = 6,
    PIN_BUTTON_L1 = 7,
    PIN_BUTTON_R1 = 8,
    PIN_BUTTON_L2 = 9,
    PIN_BUTTON_R2 = 10,
    PIN_BUTTON_L3 = 11,
    PIN_BUTTON_R3 = 12,
    PIN_BUTTON_UP = 13,
    PIN_BUTTON_DOWN = 14,
    PIN_BUTTON_LEFT = 15,
    PIN_BUTTON_RIGHT = 16,
    
    // 摇杆引脚
    PIN_JOYSTICK1_X = 1,
    PIN_JOYSTICK1_Y = 2,
    PIN_JOYSTICK2_X = 3,
    PIN_JOYSTICK2_Y = 4,
    
    // 震动电机
    PIN_VIBRATION_LEFT = 17,
    PIN_VIBRATION_RIGHT = 18,
    
    // LED
    PIN_LED_CONNECTION = 19,
    PIN_LED_VIBRATION = 20,
    PIN_LED_BATTERY = 21,
    PIN_LED_CHARGING = 22,
    
    // 电池监测
    PIN_BATTERY_ADC = 5,
    
    // I2C (备用)
    PIN_I2C_SDA = 8,
    PIN_I2C_SCL = 9,
    
    // SPI (备用)
    PIN_SPI_MISO = 37,
    PIN_SPI_MOSI = 35,
    PIN_SPI_SCK = 36,
    PIN_SPI_CS = 34
};

// 按钮映射 (对应标准游戏手柄)
enum ButtonMap {
    BUTTON_A = 1,
    BUTTON_B = 2,
    BUTTON_X = 3,
    BUTTON_Y = 4,
    BUTTON_L1 = 5,
    BUTTON_R1 = 6,
    BUTTON_L2 = 7,
    BUTTON_R2 = 8,
    BUTTON_SELECT = 9,
    BUTTON_START = 10,
    BUTTON_L3 = 11,
    BUTTON_R3 = 12,
    BUTTON_UP = 13,
    BUTTON_DOWN = 14,
    BUTTON_LEFT = 15,
    BUTTON_RIGHT = 16,
    BUTTON_HOME = 17
};

#endif // CONFIG_H