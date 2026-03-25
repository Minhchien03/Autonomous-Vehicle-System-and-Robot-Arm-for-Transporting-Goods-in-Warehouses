System Architecture & Data Flow:

AI Model Training: Built a custom image dataset using Roboflow for 3 specific types of goods (represented by purple, green, and orange blocks). Trained the YOLOv8n model via Google Colab to achieve high-accuracy object detection, exporting the optimized weights (bestV5.pt) for local deployment.

Real-time Video Streaming: Configured the backend to continuously capture and decode video frames via HTTP stream from the edge device (ESP32-CAM) located on the robotic arm.

Multithreaded TCP/IP Server: Engineered a local Python TCP/IP Socket Server listening on port 8082. Designed a non-blocking architecture using Python's threading module to isolate network I/O operations from the main AI processing loop.

Logic & Hardware Synchronization: Implemented algorithmic logic to filter AI predictions (Confidence > 0.8) and introduced a 6-second event-throttling mechanism to prevent command spamming. The server evaluates the frame, identifies the target color, and pushes the corresponding command (e.g., "orange") to the ESP32 microcontroller over the TCP socket to trigger 4-axis mechanical movements.
