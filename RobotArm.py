import cv2
import numpy as np
from ultralytics import YOLO
import socket
import time
import threading

# Thông số camera
CAMERA_WIDTH = 640    # Độ rộng khung hình camera
CAMERA_HEIGHT = 480   # Độ cao khung hình camera

# Địa chỉ IP của ESP32 và ESP32-CAM
ESP32_IP = "192.168.1.150"
ESP32_CAM_URL = "http://192.168.1.100:81/stream"

# Đường dẫn đến mô hình đã train
model_path = "D:\\FPL\\Research project\\Self-driving\\bestV5.pt"

# Load mô hình YOLOv8
YOLO_robotArm = YOLO(model_path)

# Thiết lập kết nối TCP với ESP32
HOST = '192.168.1.2'
PORT = 8082
client_socket = None

def tcp_server():
    """Thiết lập kết nối TCP với ESP32."""
    global client_socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((HOST, PORT))
    server_socket.listen(1)
    print("Server is running on 8082 port")
    client_socket, addr = server_socket.accept()
    print(f"Connection from {addr}")

def send_command_to_esp32(command):
    """Gửi lệnh đến RobotArm."""
    def _send_command(): 
        global client_socket
        if client_socket and client_socket.fileno() != -1:
            try:
                print(f"Gửi lệnh: {command}")
                client_socket.send((command + '\n').encode())
                response = client_socket.recv(1024).decode().strip()
                print(f"Phản hồi từ RobotArm: {response}")
            except Exception as e:
                print(f"Lỗi khi gửi lệnh: {e}")
    
    # Chạy gửi lệnh trong luồng riêng
    threading.Thread(target=_send_command).start()

def detect_blocks(frame):
    """Phát hiện khối màu trong toàn bộ frame và trả về màu sắc."""
    results = YOLO_robotArm(frame)
    detected_color = None  # Chỉ lưu màu sắc đầu tiên được phát hiện
    for result in results: 
        for box in result.boxes: 
            x1, y1, x2, y2 = map(int, box.xyxy[0])
            conf = box.conf[0]
            cls = int(box.cls[0])
            if conf > 0.8: 
                color = YOLO_robotArm.names[cls].split('-')[0].lower()
                if color == "puple": 
                    color = "purple"
                detected_color = color
                # Vẽ bounding box và nhãn
                cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
                cv2.putText(frame, f"{color}", (x1, y1 - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
                break  # Chỉ lấy màu đầu tiên được phát hiện
        if detected_color:  # Thoát vòng lặp nếu đã phát hiện màu
            break
    return detected_color, frame    

def main():
    """Hàm chính điều phối chương trình."""
    tcp_server()  # Thiết lập kết nối với ESP32
    cap = cv2.VideoCapture(ESP32_CAM_URL)
    if not cap.isOpened():
        print("Can't open camera from ESP32_cam")
        return

    previous_color = None
    last_command_time = 0  # Thời điểm gửi lệnh cuối cùng
    
    try:
        while True:
            success, frame = cap.read()
            if not success:
                print("Failed to read frame from ESP32-CAM")
                break
            
            detected_color, frame_with_boxes = detect_blocks(frame)
            cv2.imshow("Camera Stream", frame_with_boxes)
            
            # Kiểm tra thời gian để gửi lệnh (mỗi 10 giây)
            current_time = time.time()
            if current_time - last_command_time >= 6:  # Đã qua 10 giây
                if detected_color and detected_color != previous_color:
                    print(f"Phát hiện khối màu: {detected_color}")
                    send_command_to_esp32(detected_color)  # Chỉ gửi màu sắc (purple, green, orange)
                    last_command_time = current_time  # Cập nhật thời điểm gửi lệnh
                
                previous_color = detected_color

            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
    finally:
        cap.release()
        cv2.destroyAllWindows()
        if client_socket:
            client_socket.close()

if __name__ == "__main__":
    main()  