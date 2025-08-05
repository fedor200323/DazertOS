DazertOS 🛠️

DazertOS is a powerful, open-source operating system for the ESP32 microcontroller, designed for educational Wi-Fi testing and exploration. It offers a robust set of tools for network analysis, device control, and interactive fun, all wrapped in a multilingual command-line interface (English by default, Russian supported). Whether you're stress-testing Wi-Fi networks, generating passwords, or playing games, DazertOS is your go-to platform for learning and experimentation! 🚀



⚠️ Legal Notice: Use DazertOS responsibly and only on networks and devices you own or have explicit permission to test. Unauthorized use may violate laws (e.g., Russian Criminal Code Articles 272–274).

✨ Features





Wi-Fi Tools 📡





Scan Wi-Fi networks (2.4 GHz) to discover SSIDs, BSSIDs, signal strength, and channels.



Connect to Wi-Fi networks interactively.



Perform powerful deauthentication and disassociation attacks (10,000 packets, customizable client MAC, multiple reason codes) for educational network testing.



Continuous attack mode with auto-stop after 90 seconds to prevent overheating (~30,000 packets).



Multilingual Interface 🌐





Supports English (default) and Russian languages, switchable via set_lang command.



Language settings persist across reboots using non-volatile storage.



Utilities 🧰





Generate secure passwords (up to 128 characters).



Transmit text as Morse code via the onboard LED.



Perform basic arithmetic calculations (calc command).



Store and retrieve variables using Preferences (e.g., set_var user Fedor).



Games 🎮





Number guessing game (game_guess, 1–100).



Tic-Tac-Toe (game_tictactoe) against the ESP32.



System Control ⚙️





Display system info (chip model, cores, flash size, MAC, free memory).



Control the onboard LED (led_on, led_off).



Reboot the ESP32 (reboot).



Get current time via NTP (requires Wi-Fi).



Stability & Safety 🛡️





Memory monitoring during attacks to prevent crashes.



90-second limit for continuous attacks to avoid overheating.



Compatible with ESP32 Dev Module (Arduino IDE, platform 3.3.0).

🛠️ Getting Started

Prerequisites





Hardware: ESP32 Dev Module (e.g., ESP32-WROOM-32).



Software:





Arduino IDE (2.x recommended).



ESP32 platform (version 3.3.0) installed via Boards Manager.



USB cable (≥1A power recommended for stability).



Optional: Bluetooth-enabled device for serial communication.

Installation





Clone the Repository:

git clone https://github.com/your-username/dazertos.git



Open in Arduino IDE:





Open dazertos/minios/minios.ino in Arduino IDE.



Ensure the file is encoded in UTF-8 without BOM (use VS Code or Notepad++ if needed).



Configure Arduino IDE:





Go to Tools > Board > ESP32 Arduino > ESP32 Dev Module.



Select the correct port (Tools > Port, e.g., /dev/ttyUSB0).



Install ESP32 platform 3.3.0 via Tools > Board > Boards Manager.



Clean Cache (if needed):

rm -rf ~/.cache/arduino/
rm -rf ~/.arduino15/packages/esp32/



Compile & Upload:





Click Verify (Ctrl+R) to compile.



Click Upload (Ctrl+U) to flash the ESP32.



If errors occur, check the serial log and ensure proper board/port settings.

Usage





Connect to Serial Monitor:





Open Serial Monitor in Arduino IDE (115200 baud) or connect via Bluetooth (dazertOS device).



Explore Commands:

> help
DazertOS v1.5
---[ Available commands: ]---
help               - Show this list
sys_info           - System information
wifi_scan          - Scan Wi-Fi networks
wifi_deauth <MAC> <channel> - Powerful attack (10000 packets)
set_lang <en|ru>   - Set language (en or ru)
...



Switch Language:

> set_lang ru
Язык установлен на 'ru'
> help
---[ Список команд: ]---
help               - Показать этот список
...



Perform a Wi-Fi Attack (example):

> wifi_scan
Found networks: 3
1: MyWiFi (BSSID: AA:BB:CC:DD:EE:FF, -50 dBm, channel: 6, Secured)
...
> wifi_deauth_interactive
Enter network number for attack (or 0 to cancel): 1
Enter client MAC (or Enter for network-wide attack): 11:22:33:44:55:66
Powerful attack on AA:BB:CC:DD:EE:FF (client: 11:22:33:44:55:66) on channel 6
Cycle 1/20, sent 1000 packets, memory: 150000 bytes
...
Attack completed (sent 10000 packets).



Safety Tips:





Use a stable power source (≥1A USB adapter or 3.3–5V battery).



Monitor memory in Serial Monitor; stop attacks if free memory <10 KB.



Pause for 5–10 minutes between long attacks to prevent overheating.



Attach a small heatsink (10×10 mm) for extended continuous attacks.

⚠️ Stability & Troubleshooting





Overheating: Continuous attacks are limited to 90 seconds (~30,000 packets) to keep the ESP32 temperature below 80°C. Use a heatsink for prolonged use.



Crashes: If the ESP32 hangs, increase delay(3) to delay(5) in attack functions or reduce packets per cycle (500 → 300). Reboot with reboot.



Weak Attack Effect:





Ensure the target network is 2.4 GHz (ESP32 doesn’t support 5 GHz).



Verify BSSID and channel via wifi_scan.



Test on older devices/routers (Wi-Fi 4) for better results.



Use a specific client MAC (e.g., via airodump-ng) for targeted attacks.



Compilation Errors:





Verify UTF-8 encoding without BOM.



Clear Arduino cache (see Installation).



Ensure ESP32 platform 3.3.0 and correct board settings.

📜 License

This project is licensed under the MIT License. See the LICENSE file for details.

🤝 Contributing

Contributions are welcome! Please:





Fork the repository.



Create a feature branch (git checkout -b feature/YourFeature).



Commit changes (git commit -m 'Add YourFeature').



Push to the branch (git push origin feature/YourFeature).



Open a Pull Request.

📢 Disclaimer

DazertOS is intended for educational purposes only. The authors are not responsible for any misuse or illegal activities. Always obtain permission before testing networks or devices you do not own.



⭐ Star this project if you find it useful!
📧 For issues or suggestions, open an issue or contact the maintainers.
