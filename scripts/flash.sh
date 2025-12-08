#!/bin/bash
# ESP32-S3 Gamepad 烧录脚本

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 脚本目录
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

# 默认配置
PORT="/dev/ttyACM0"
BAUD="921600"
FLASH_MODE="qio"
FLASH_SIZE="4MB"
PARTITION_SCHEME="default"

print_usage() {
    echo "用法: $0 [选项]"
    echo "选项:"
    echo "  -p, --port PORT     串口端口 (默认: /dev/ttyACM0)"
    echo "  -b, --baud BAUD     波特率 (默认: 921600)"
    echo "  -f, --flash FLASH   Flash大小 (默认: 4MB)"
    echo "  -m, --mode MODE     Flash模式 (默认: qio)"
    echo "  -s, --scheme SCHEME 分区方案 (默认: default)"
    echo "  -c, --compile       只编译不烧录"
    echo "  -u, --upload        只烧录不编译"
    echo "  -m, --monitor       启动串口监视器"
    echo "  -h, --help          显示帮助信息"
    echo ""
    echo "示例:"
    echo "  $0 -p /dev/ttyACM0 -b 115200"
    echo "  $0 --compile"
    echo "  $0 --upload"
}

# 解析参数
while [[ $# -gt 0 ]]; do
    case $1 in
        -p|--port)
            PORT="$2"
            shift 2
            ;;
        -b|--baud)
            BAUD="$2"
            shift 2
            ;;
        -f|--flash)
            FLASH_SIZE="$2"
            shift 2
            ;;
        -m|--mode)
            FLASH_MODE="$2"
            shift 2
            ;;
        -s|--scheme)
            PARTITION_SCHEME="$2"
            shift 2
            ;;
        -c|--compile)
            COMPILE_ONLY=true
            shift
            ;;
        -u|--upload)
            UPLOAD_ONLY=true
            shift
            ;;
        -m|--monitor)
            MONITOR_ONLY=true
            shift
            ;;
        -h|--help)
            print_usage
            exit 0
            ;;
        *)
            echo -e "${RED}未知选项: $1${NC}"
            print_usage
            exit 1
            ;;
    esac
done

# 检查PlatformIO
check_platformio() {
    if ! command -v pio &> /dev/null; then
        echo -e "${RED}错误: 未找到PlatformIO，请先安装${NC}"
        echo "安装方法: https://platformio.org/install/cli"
        exit 1
    fi
}

# 检查串口
check_port() {
    if [ ! -e "$PORT" ]; then
        echo -e "${RED}错误: 串口 $PORT 不存在${NC}"
        echo -e "${YELLOW}可用串口:${NC}"
        ls /dev/tty* 2>/dev/null | grep -E "ttyACM|ttyUSB|ttyS" || true
        exit 1
    fi
}

# 编译项目
compile_project() {
    echo -e "${GREEN}正在编译项目...${NC}"
    cd "$PROJECT_DIR"
    
    pio run
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}编译成功！${NC}"
        
        # 显示固件信息
        FIRMWARE="$PROJECT_DIR/.pio/build/esp32-s3-devkitc-1/firmware.bin"
        if [ -f "$FIRMWARE" ]; then
            SIZE=$(stat -f%z "$FIRMWARE" 2>/dev/null || stat -c%s "$FIRMWARE")
            echo -e "${YELLOW}固件大小: $((SIZE / 1024)) KB${NC}"
        fi
    else
        echo -e "${RED}编译失败！${NC}"
        exit 1
    fi
}

# 烧录固件
upload_firmware() {
    echo -e "${GREEN}正在烧录固件...${NC}"
    echo -e "${YELLOW}端口: $PORT${NC}"
    echo -e "${YELLOW}波特率: $BAUD${NC}"
    
    cd "$PROJECT_DIR"
    
    # 检查是否进入下载模式
    echo -e "${YELLOW}请按ESP32-S3的Boot按钮并保持，然后按Reset按钮...${NC}"
    echo -e "${YELLOW}3秒后开始烧录...${NC}"
    sleep 3
    
    pio run --target upload --upload-port "$PORT"
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}烧录成功！${NC}"
    else
        echo -e "${RED}烧录失败！${NC}"
        echo -e "${YELLOW}请检查:${NC}"
        echo "1. 串口连接"
        echo "2. 波特率设置"
        echo "3. ESP32是否进入下载模式"
        exit 1
    fi
}

# 启动串口监视器
start_monitor() {
    echo -e "${GREEN}启动串口监视器...${NC}"
    echo -e "${YELLOW}按 Ctrl+] 退出${NC}"
    
    cd "$PROJECT_DIR"
    pio device monitor --port "$PORT" --baud "$BAUD"
}

# 主流程
main() {
    echo -e "${GREEN}=== ESP32-S3 Gamepad 烧录工具 ===${NC}"
    
    check_platformio
    
    if [ "$MONITOR_ONLY" = true ]; then
        check_port
        start_monitor
        exit 0
    fi
    
    if [ "$UPLOAD_ONLY" != true ]; then
        compile_project
    fi
    
    if [ "$COMPILE_ONLY" != true ]; then
        check_port
        upload_firmware
        
        # 询问是否启动监视器
        read -p "是否启动串口监视器? (y/n): " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            start_monitor
        fi
    fi
    
    echo -e "${GREEN}完成！${NC}"
}

# 异常处理
trap 'echo -e "${RED}脚本被中断${NC}"; exit 1' INT

main