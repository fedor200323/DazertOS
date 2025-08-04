# DazertOS
Hi! It a miniOS - DazertOS. This work on ESP32. 
nglish Description
dazertOS - A Mini Command-Line OS for the ESP32
Welcome to dazertOS! This project is a feature-rich, command-line operating system built from scratch for the powerful ESP32 microcontroller. It transforms your ESP32 into a tiny, interactive computer that you can control from your PC's serial monitor or wirelessly from your smartphone via Bluetooth.

This project was developed as a fun, educational exercise to explore the capabilities of the ESP32, from hardware control to networking and interactive games, all without any external library dependencies.

 ____   ___   ____ 
|  _ \ / _ \ / ___|
| | | | | | |\___ \ 
| |_| | |_| | ___) |
|____/ \___/ |____/ 
     DazertOS v1.1
Key Features
Dual Interface: Control your ESP32 via USB Serial or wirelessly using any standard Bluetooth Serial Terminal app on your phone.

System Utilities: Get detailed system information (sys_info), and reboot the device (reboot).

Hardware Control: Instantly toggle the onboard LED (led_on / led_off).

Wi-Fi Management: Scan for networks (wifi_scan), connect interactively (wifi_connect), and check your connection status (wifi_status).

Useful Tools:

morse <text>: Transmit text in Morse code using the LED.

pass_gen <length>: Generate a random, strong password.

calc <expression>: A simple calculator for basic arithmetic (e.g., calc 15 * 4).

Games:

game_tictactoe: Play Tic-Tac-Toe against a simple AI.

game_guess: Play the classic "Guess the Number" game.

Getting Started
You Will Need
An ESP32 Development Board.

Arduino IDE with the ESP32 Boards package installed.

(Optional) A smartphone with a Bluetooth Terminal application (e.g., "Serial Bluetooth Terminal" on Android).

Setup
Upload the dazertOS.ino sketch to your ESP32 using the Arduino IDE.

To control via USB: Open the Serial Monitor at 115200 baud.

To control via Bluetooth:

Pair your phone with the new Bluetooth device named "dazertOS".

Open your Bluetooth Terminal app and connect to the "dazertOS" device.

Type help and press Enter to see the full list of commands. Enjoy!

Русское описание
dazertOS - Миниатюрная командная ОС для ESP32
Добро пожаловать в dazertOS! Этот проект — многофункциональная операционная система с интерфейсом командной строки, созданная с нуля для мощного микроконтроллера ESP32. Она превращает вашу плату в крошечный интерактивный компьютер, которым можно управлять как через USB с вашего ПК, так и без проводов со смартфона по Bluetooth.

Проект был разработан в качестве увлекательного образовательного эксперимента для изучения возможностей ESP32: от управления оборудованием до работы с сетью и интерактивных игр. Все это — без каких-либо зависимостей от внешних библиотек.

 ____   ___   ____ 
|  _ \ / _ \ / ___|
| | | | | | |\___ \ 
| |_| | |_| | ___) |
|____/ \___/ |____/ 
     DazertOS v1.1
Ключевые возможности
Двойной интерфейс: Управляйте вашим ESP32 через USB Serial или без проводов с помощью любого стандартного приложения "Bluetooth Terminal" на телефоне.

Системные утилиты: Получайте подробную информацию о системе (sys_info) и перезагружайте устройство (reboot).

Управление оборудованием: Мгновенно включайте и выключайте встроенный светодиод (led_on / led_off).

Менеджер Wi-Fi: Сканируйте сети (wifi_scan), подключайтесь к ним в интерактивном режиме (wifi_connect) и проверяйте статус соединения (wifi_status).

Полезные инструменты:

morse <текст>: Передавайте сообщение азбукой Морзе с помощью светодиода.

pass_gen <длина>: Генерируйте случайный и надежный пароль.

calc <выражение>: Простой калькулятор для базовых операций (например, calc 15 * 4).

Игры:

game_tictactoe: Сыграйте в "Крестики-нолики" против простого ИИ.

game_guess: Классическая игра "Угадай число".

Как начать работу
Вам понадобится
Плата разработки на базе ESP32.

Программа Arduino IDE с установленным пакетом для плат ESP32.

(Опционально) Смартфон с приложением "Bluetooth Terminal" (например, "Serial Bluetooth Terminal" на Android).

Запуск
Загрузите скетч dazertOS.ino на вашу плату ESP32 через Arduino IDE.

Для управления по USB: Откройте "Монитор последовательного порта" на скорости 115200.

Для управления по Bluetooth:

Выполните сопряжение вашего телефона с новым Bluetooth-устройством "dazertOS".

Откройте приложение "Bluetooth Terminal" и подключитесь к устройству "dazertOS".

Введите команду help и нажмите Enter, чтобы увидеть полный список команд. Наслаждайтесь!
