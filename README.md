dazertOS v2.0 - A Command-Line OS for the ESP32
dazertOS is a lightweight, multi-functional, command-line operating system built from scratch, designed specifically to run on the ESP32 microcontroller. It transforms the bare hardware into a tiny, interactive computer that can be controlled from a PC via USB or wirelessly from a smartphone via Bluetooth.

The core philosophy of this project is to explore the vast capabilities of the ESP32 using only its built-in features, without relying on external hardware components or complex libraries.

Please Note: This is a terminal-based OS. It does not have a graphical user interface and is intended to be used exclusively with the Arduino IDE's Serial Monitor or a Bluetooth Terminal application on a smartphone.

What's New in Update 2.0
Version 2.0 is a major update that significantly expands the OS's capabilities, adding a range of professional and fun features:

Web Interface: A built-in web server (web_server) now allows you to control the ESP32's core functions from any web browser on your local network.

Wi-Fi Sniffer: A powerful utility (sniff_start) that puts the Wi-Fi module into promiscuous mode to capture and display metadata from nearby Wi-Fi packets.

BLE Scanner: Added a Bluetooth Low Energy scanner (ble_scan) to discover modern wireless devices like smartwatches and fitness trackers.

Visual Effects: New commands like matrix and banner to give your terminal a classic hacker aesthetic.

System Utilities: Added a simple CPU benchmark and a clear command to manage the console.

Full Feature List
System & Core Commands

help: Displays the logo and the full command list.

neofetch: Shows a stylish, compact summary of the system.

sys_info: Provides detailed system information (chip model, cores, memory, etc.).

reboot: Reboots the device.

deepsleep <secs>: Puts the ESP32 into an ultra-low-power mode for a specified duration.

clear: Clears the terminal screen.

Wi-Fi & Networking

wifi_connect: An interactive wizard to scan and connect to a Wi-Fi network.

wifi_status: Shows the current Wi-Fi connection status.

ble_scan: Scans for Bluetooth Low Energy (BLE) devices.

sniff_start <channel>: Starts the Wi-Fi packet sniffer on a specific channel.

sniff_stop: Stops the Wi-Fi sniffer.

get_time: Fetches the current date and time from an internet (NTP) server.

web_server: Launches a web control panel on the ESP32's IP address.

Utilities & Fun

led_on / led_off: Controls the built-in LED.

pass_gen <length>: Generates a random password.

morse <text>: Blinks a message in Morse code using the LED.

calc <expression>: A simple calculator for expressions like 10 * 5.

banner <text>: Prints your text as a large ASCII art banner.

matrix: Runs the classic "Matrix" falling green code effect.

benchmark: Performs a simple CPU performance test.

set_var <key> <value>: Saves a variable to the ESP32's persistent memory.

get_var <key>: Reads a previously saved variable.

Games

game_guess: The "Guess the Number" game.

game_tictactoe: Play Tic-Tac-Toe against a simple AI.
