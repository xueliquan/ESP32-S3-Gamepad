#include "Gamepad.h"

Gamepad::Gamepad() 
    : activityDetected(false), 
      reportNeeded(false),
      lastReportTime(0) {
}

bool Gamepad::begin() {
    Serial.println("Initializing gamepad...");
    
    // 初始化蓝牙游戏手柄
    BleGamepadConfiguration config;
    config.setControllerType(CONTROLLER_TYPE_GAMEPAD);
    config.setAxesMin(0);
    config.setAxesMax(65535);
    config.setButtonCount(BUTTON_COUNT);
    config.setAutoReport(false);
    config.setWhichAxes(true, true, true, true, false, false, false, false); // X,Y,Z,RZ
    
    config.setVid(0x045E);  // Microsoft
    config.setPid(0x028E);  // Xbox 360 Controller
    config.setModelNumber(DEVICE_MODEL);
    config.setSoftwareRevision("1.0.0");
    
    bleGamepad.begin(&config);
    
    // 初始化组件
    initializeButtons();
    initializeJoysticks();
    initializeVibration();
    initializeLEDs();
    
    Serial.println("Gamepad initialization complete");
    return true;
}

void Gamepad::initializeButtons() {
    Serial.println("Initializing buttons...");
    
    const uint8_t buttonPins[BUTTON_COUNT] = {
        PIN_BUTTON_START, PIN_BUTTON_SELECT, PIN_BUTTON_HOME,
        PIN_BUTTON_A, PIN_BUTTON_B, PIN_BUTTON_X, PIN_BUTTON_Y,
        PIN_BUTTON_L1, PIN_BUTTON_R1, PIN_BUTTON_L2, PIN_BUTTON_R2,
        PIN_BUTTON_L3, PIN_BUTTON_R3,
        PIN_BUTTON_UP, PIN_BUTTON_DOWN, PIN_BUTTON_LEFT, PIN_BUTTON_RIGHT
    };
    
    for (int i = 0; i < BUTTON_COUNT; i++) {
        buttons[i].begin(buttonPins[i], i + 1);
    }
}

void Gamepad::initializeJoysticks() {
    Serial.println("Initializing joysticks...");
    joysticks.begin();
}

void Gamepad::initializeVibration() {
    Serial.println("Initializing vibration motors...");
    vibration.begin(PIN_VIBRATION_LEFT, PIN_VIBRATION_RIGHT);
}

void Gamepad::initializeLEDs() {
    Serial.println("Initializing LEDs...");
    leds.begin(PIN_LED_CONNECTION, PIN_LED_VIBRATION, 
               PIN_LED_BATTERY, PIN_LED_CHARGING);
}

void Gamepad::update() {
    activityDetected = false;
    reportNeeded = false;
    
    readInputs();
    updateBluetoothReport();
}

void Gamepad::readInputs() {
    // 读取按钮
    for (int i = 0; i < BUTTON_COUNT; i++) {
        buttons[i].update();
        if (buttons[i].wasPressed() || buttons[i].wasReleased()) {
            activityDetected = true;
            reportNeeded = true;
        }
    }
    
    // 读取摇杆
    joysticks.update();
    if (joysticks.wasMoved()) {
        activityDetected = true;
        reportNeeded = true;
    }
}

void Gamepad::updateBluetoothReport() {
    if (!bleGamepad.isConnected()) return;
    
    // 更新按钮状态
    for (int i = 0; i < BUTTON_COUNT; i++) {
        if (buttons[i].wasPressed()) {
            bleGamepad.press(buttons[i].getGamepadButton());
        } else if (buttons[i].wasReleased()) {
            bleGamepad.release(buttons[i].getGamepadButton());
        }
    }
    
    // 更新摇杆状态
    JoystickState state = joysticks.getState();
    bleGamepad.setX(state.x1);
    bleGamepad.setY(state.y1);
    bleGamepad.setZ(state.x2);
    bleGamepad.setRZ(state.y2);
}

void Gamepad::sendReport() {
    if (bleGamepad.isConnected() && reportNeeded) {
        bleGamepad.sendReport();
        reportNeeded = false;
        lastReportTime = millis();
    }
}

void Gamepad::sleep() {
    // 关闭所有组件
    vibration.stop();
    leds.allOff();
    
    // 断开蓝牙连接
    if (bleGamepad.isConnected()) {
        // 注意：这可能不可用，取决于库实现
    }
}

void Gamepad::wake() {
    // 重新初始化
    begin();
}