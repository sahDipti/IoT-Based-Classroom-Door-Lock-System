# IoT-Based-Classroom-Door-Lock-System
🔒 Secure | 📡 IoT-Enabled | 📲 SMS Alerts

This project is an IoT-based smart door lock system designed for classroom security. It allows administrators to remotely lock/unlock doors, monitor access in real time, and receive instant SMS alerts in case of unauthorized access attempts.
🔹 Features

✅ Remote Access – Lock/unlock doors using the Blynk mobile app
✅ Real-Time Monitoring – Logs access data on ThingSpeak
✅ SMS Alerts – Sends instant alerts via Twilio API for unauthorized access
✅ Automated Locking – Based on predefined schedules
✅ LCD Display – Provides local status updates
🔹 Tech Stack

🔹 Hardware: ESP32, Servo Motor, LCD Display
🔹 Software: Arduino IDE, Blynk, ThingSpeak, Twilio API
🔹 Programming Language: C++
🔹 How It Works

1️⃣ Admin Authentication via Blynk app
2️⃣ Correct Password → Unlocks door, logs event on ThingSpeak
3️⃣ Wrong Password (Multiple Attempts) → Triggers SMS alert to admin
4️⃣ Lock Status & Alerts displayed on LCD
🔹 Team Contributions

👨‍💻 Dipti Raj Sah – Circuit design, Twilio SMS alerts, Blynk configuration
👨‍💻 Anubhav Agrawal – ThingSpeak integration, report documentation
👨‍💻 Arjun Khare – System logic implementation, PowerPoint presentation
🔹 Future Enhancements

🚀 AI-based anomaly detection for advanced security
🚀 Integration with biometric authentication
📌 A smart, secure, and efficient solution for classroom security! 🚀🔒
