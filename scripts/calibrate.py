#!/usr/bin/env python3
"""
ESP32-S3 Gamepad 摇杆校准工具
"""

import serial
import json
import time
import sys
import os

class JoystickCalibrator:
    def __init__(self, port='/dev/ttyACM0', baudrate=115200):
        self.serial = serial.Serial(port, baudrate, timeout=1)
        time.sleep(2)  # 等待ESP32重启
        
    def send_command(self, command):
        """发送命令到ESP32"""
        self.serial.write((command + '\n').encode())
        time.sleep(0.1)
        response = self.serial.read_all().decode().strip()
        return response
    
    def get_calibration_data(self):
        """读取当前校准数据"""
        response = self.send_command('GET_CALIBRATION')
        if 'CAL_DATA:' in response:
            data = response.split('CAL_DATA:')[1].strip()
            return json.loads(data)
        return None
    
    def set_calibration_data(self, data):
        """设置校准数据"""
        command = f'SET_CALIBRATION {json.dumps(data)}'
        response = self.send_command(command)
        return 'OK' in response
    
    def save_to_flash(self):
        """保存到Flash"""
        response = self.send_command('SAVE_CALIBRATION')
        return 'SAVED' in response
    
    def start_calibration(self):
        """开始校准流程"""
        print("=== ESP32-S3 Gamepad 摇杆校准工具 ===")
        print("请按照提示操作摇杆...\n")
        
        input("1. 请勿触摸摇杆，按回车开始中心点校准...")
        print("校准中心点...")
        
        # 读取中心点
        centers = {'x1': 0, 'y1': 0, 'x2': 0, 'y2': 0}
        samples = 100
        
        for i in range(samples):
            response = self.send_command('GET_JOYSTICKS')
            if 'JOY:' in response:
                data = response.split('JOY:')[1].strip()
                values = list(map(int, data.split(',')))
                centers['x1'] += values[0]
                centers['y1'] += values[1]
                centers['x2'] += values[2]
                centers['y2'] += values[3]
            
            time.sleep(0.01)
            print(f"\r采样进度: {i+1}/{samples}", end='')
        
        print()
        centers = {k: v // samples for k, v in centers.items()}
        print(f"中心点: {centers}")
        
        input("\n2. 将两个摇杆都推到左上角，按回车...")
        response = self.send_command('GET_JOYSTICKS')
        min_vals = list(map(int, response.split('JOY:')[1].strip().split(',')))
        
        input("\n3. 将两个摇杆都推到右下角，按回车...")
        response = self.send_command('GET_JOYSTICKS')
        max_vals = list(map(int, response.split('JOY:')[1].strip().split(',')))
        
        # 创建校准数据
        calibration = {
            'center': centers,
            'min': {'x1': min_vals[0], 'y1': min_vals[1], 
                   'x2': min_vals[2], 'y2': min_vals[3]},
            'max': {'x1': max_vals[0], 'y1': max_vals[1], 
                   'x2': max_vals[2], 'y2': max_vals[3]},
            'deadzone': 50,
            'timestamp': time.time()
        }
        
        return calibration
    
    def run(self):
        """运行校准程序"""
        try:
            # 测试连接
            print("测试设备连接...")
            self.send_command('AT')  # 简单测试命令
            print("设备连接成功\n")
            
            # 获取当前校准
            current = self.get_calibration_data()
            if current:
                print(f"当前校准数据: {json.dumps(current, indent=2)}\n")
                
                choice = input("是否重新校准? (y/n): ")
                if choice.lower() != 'y':
                    return
            else:
                print("未找到校准数据，开始新校准\n")
            
            # 执行校准
            calibration = self.start_calibration()
            
            print(f"\n校准结果: {json.dumps(calibration, indent=2)}")
            
            # 应用校准
            if self.set_calibration_data(calibration):
                print("校准数据已应用")
                
                # 保存到Flash
                if self.save_to_flash():
                    print("校准数据已保存到Flash")
                else:
                    print("保存到Flash失败")
            else:
                print("应用校准数据失败")
                
        except Exception as e:
            print(f"错误: {e}")
        finally:
            self.serial.close()

if __name__ == '__main__':
    # 自动检测端口
    import glob
    
    ports = []
    if sys.platform.startswith('win'):
        ports = [f'COM{i}' for i in range(1, 10)]
    elif sys.platform.startswith('linux'):
        ports = glob.glob('/dev/ttyACM*') + glob.glob('/dev/ttyUSB*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.usbmodem*') + glob.glob('/dev/tty.usbserial*')
    
    if not ports:
        print("未找到可用串口")
        sys.exit(1)
    
    print("可用串口:")
    for i, port in enumerate(ports):
        print(f"{i+1}. {port}")
    
    choice = input(f"选择端口 (1-{len(ports)}): ")
    try:
        port_index = int(choice) - 1
        if 0 <= port_index < len(ports):
            calibrator = JoystickCalibrator(ports[port_index])
            calibrator.run()
        else:
            print("无效选择")
    except ValueError:
        print("请输入数字")