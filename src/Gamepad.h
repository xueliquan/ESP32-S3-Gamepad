#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <Arduino.h>
#include <BleGamepad.h>
#include "config.h"
#include "Button.h"
#include "Joystick.h"
#include "VibrationMotor.h"
#include "LEDController.h"

class Gamepad {
public:
    Gamepad();
    bool begin();
    void update();
    void sendReport();
    void sleep();
    void wake();
    
    // 状态检查
    bool isConnected() const { return bleGamepad.isConnected(); }
    bool wasActivity() const { return activityDetected; }
    bool needsReport() const { return reportNeeded; }
    
    // 组件访问
    Button buttons[BUTTON_COUNT];
    Joystick joysticks;
    VibrationMotor vibration;
    LEDController leds;
    
private:
    BleGamepad bleGamepad;
    bool activityDetected;
    bool reportNeeded;
    unsigned long lastReportTime;
    
    void initializeButtons();
    void initializeJoysticks();
    void initializeVibration();
    void initializeLEDs();
    void readInputs();
    void updateBluetoothReport();
};

#endif // GAMEPAD_H