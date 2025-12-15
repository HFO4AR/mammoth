"""
该模块用于与STM32通过串口通信。
author:icy
"""
import serial
import glob
import time
import threading

class Communicate:
    def __init__(self, port=None, baudrate=115200):
        print("Communicate start")
        self.port = port
        self.baudrate = baudrate
        self.ser = None
        self.receive_data_running = False
        self.receive_data_thread = None
        self.receive_data_val = None

    # 查找并打开串口
    def connect(self):
        if self.port:
            # 如果指定了端口，直接尝试连接
            try:
                self.ser = serial.Serial(self.port, self.baudrate)
                print(f"Connected to {self.port}")
                return True
            except serial.SerialException as e:
                print(f"Failed to connect to {self.port}: {e}")
                return False
        else:
            # 否则遍历所有ttyUSB设备
            print("Scanning for available ttyUSB devices...")
            ports = glob.glob("/dev/ttyUSB*")

            if not ports:
                print("No ttyUSB devices found")
                return False

            print(f"Found {len(ports)} ttyUSB device(s): {ports}")

            for port in ports:
                try:
                    print(f"Trying to connect to {port}...")
                    self.ser = serial.Serial(port, self.baudrate, timeout=1)
                    print(f"Connected to {port}")
                    self.port = port
                    return True
                except serial.SerialException as e:
                    print(f"Failed to connect to {port}: {e}")
            else:
                print("Could not connect to any ttyUSB port")
                return False

    # 检查串口是否打开
    def is_open(self):
        return self.ser is not None and hasattr(self.ser, 'is_open') and self.ser.is_open

    # 设置串口参数
    def set_port(self):
        if not self.is_open():
            return False

        self.ser = serial.Serial(
            port=self.port,                # 串口路径
            baudrate=self.baudrate,        # 波特率
            bytesize=serial.EIGHTBITS,     # 8位数据位
            parity=serial.PARITY_NONE,     # 无校验
            stopbits=serial.STOPBITS_ONE,  # 1位停止位
            timeout=1,                     # 读取超时1秒
            rtscts=False                   # 禁用硬件流控
        )
        return True


    # 关闭串口
    def close(self):
        if self.receive_data_running:
            self.receive_data_running = False
            if self.receive_data_thread and self.receive_data_thread.is_alive():
                self.receive_data_thread.join(timeout=1)
                print("Receive thread stopped")
        if self.is_open():
            self.ser.close()
            print("Serial port closed")


    # 发送字节数据
    def send_data(self, data):
        if not self.is_open():
            return False
        msg_queue = bytearray([])
        msg_queue.append(0xAA)  # 起始字节
        msg_queue.extend(data.encode('utf-8'))
        msg_queue.append(0xBB)  # 结束字节
        self.ser.write(msg_queue)
        print(f"Sent: {data}")
        return True

    # 接收字节数据方法
    def receive_data_main(self):
        while self.receive_data_running:
            if not self.is_open():
                time.sleep(1)
                continue
            try:
                msg_queue = bytearray([])
                while self.receive_data_running:
                    byte = self.ser.read(1)
                    if byte == b'\xAA':     # 起始字节
                        msg_queue.extend(byte)
                        break
                while self.receive_data_running:
                    byte = self.ser.read(1)
                    if byte == b'\xBB':     # 结束字节
                        msg_queue.extend(byte)
                        break
                    msg_queue.extend(byte)
                data = msg_queue[1:-1].decode('utf-8')
                print(f"Received: {data}")
                self.receive_data_running = False
                self.receive_data_thread = None
                self.receive_data_val = data
                return True
            except Exception as e:
                print(f"Error receiving data: {e}")
                self.receive_data_running = False
                self.receive_data_thread = None
                self.receive_data_val = None
                time.sleep(0.1)


    # 接收字节数据（多线程）
    def receive_data(self):
        data = self.receive_data_val
        if not self.receive_data_running:
            self.receive_data_running = True
            self.receive_data_thread = threading.Thread(target=self.receive_data_main)
            self.receive_data_thread.daemon = True
            self.receive_data_thread.start()
            data = self.receive_data_val
            self.receive_data_val = None
        return data




if __name__ == "__main__":
    communicate = Communicate()  #创建一个Communicate对象
    try:
        if communicate.connect():    #连接ttyUSB设备
            communicate.set_port()   #设置串口参数

            while True:
                data = communicate.receive_data()  #接收数据
                if data:
                    print(f"Received_data: {data}")
                communicate.send_data("Hello, STM32!")  #发送数据
                time.sleep(0.5)
    except KeyboardInterrupt:
        communicate.close()
        print("KeyboardInterrupt: Closing serial port")
