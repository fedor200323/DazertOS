#include "WiFi.h"
#include "BluetoothSerial.h"
#include "WebServer.h"
#include "time.h"
#include "Preferences.h"
#include "BLEDevice.h"
#include "esp_wifi.h"
#include "esp_mac.h"

#define LED_PIN 2

BluetoothSerial SerialBT;
Preferences preferences;
WebServer server(80);
TaskHandle_t blinkTaskHandle = NULL;
bool isSniffing = false;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.printf("BLE Device: %s, Name: %s, RSSI: %d\n", advertisedDevice.getAddress().toString().c_str(), advertisedDevice.getName().c_str(), advertisedDevice.getRSSI());
      SerialBT.printf("BLE Device: %s, Name: %s, RSSI: %d\n", advertisedDevice.getAddress().toString().c_str(), advertisedDevice.getName().c_str(), advertisedDevice.getRSSI());
    }
};

struct MultiStream {
  void print(const String& s)   { Serial.print(s);   SerialBT.print(s);   }
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
    if (isSniffing) { 
      String stopCmd = "";
      if (Serial.available()) stopCmd = Serial.readStringUntil('\n');
      else if (SerialBT.available()) stopCmd = SerialBT.readStringUntil('\n');
      stopCmd.trim();
      if (stopCmd == "sniff_stop") return "sniff_stop";
      delay(100); 
      continue; 
    }
    if (Serial.available()) activeStream = &Serial;
    else if (SerialBT.available()) activeStream = &SerialBT;
    else { delay(10); continue; }
    
    char c = activeStream->read();
    if (c == '\n' || c == '\r') {
      if (line.length() > 0) {
        if(activeStream == &Serial) out.println();
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
  out.println("     DazertOS v2.0.1");
  out.println();
}

void executeBenchmark() {
    out.println("Starting CPU benchmark (10 million floating point operations)...");
    long startTime = millis();
    volatile float a = 1.23;
    volatile float b = 4.56;
    for (int i = 0; i < 10000000; i++) {
        a = a * b / 3.14;
        b = b * a * 2.71;
    }
    long endTime = millis();
    out.printf("Benchmark finished in %ld milliseconds.\n", endTime - startTime);
}

void handleRoot() {
  String html = R"(
<!DOCTYPE html><html><head><meta name=viewport content='width=device-width, initial-scale=1'><title>dazertOS Control Panel</title><style>body{font-family:sans-serif; text-align:center; background:#282c34; color:white;} .btn{display:inline-block; background:#61afef; color:white; padding:15px 30px; text-decoration:none; border-radius:5px; margin:10px;} .btn.off{background:#e06c75;} .info{background:#3c4049; padding:20px; border-radius:10px; display:inline-block; margin-top:20px; text-align:left;}</style></head><body><h1>dazertOS Control Panel</h1><p>Control your ESP32 from the web.</p><div><a href='/led/on' class='btn'>LED ON</a> <a href='/led/off' class='btn off'>LED OFF</a></div><div class='info' id='sysinfo'>Loading system info...</div><script>setInterval(function(){fetch('/sysinfo').then(response => response.text()).then(data => {document.getElementById('sysinfo').innerHTML = data;});}, 2000);</script></body></html>
)";
  server.send(200, "text/html", html);
}

void handleSysInfo() {
  String info = "<b>Chip:</b> " + String(ESP.getChipModel()) + " rev" + String(ESP.getChipRevision()) + "<br>";
  info += "<b>Cores:</b> " + String(ESP.getChipCores()) + "<br>";
  info += "<b>Free Heap:</b> " + String(ESP.getFreeHeap()) + " bytes<br>";
  info += "<b>Wi-Fi Status:</b> ";
  info += (WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
  server.send(200, "text/plain", info);
}

void handleLedOn() { digitalWrite(LED_PIN, HIGH); server.sendHeader("Location", "/"); server.send(302); }
void handleLedOff() { digitalWrite(LED_PIN, LOW); server.sendHeader("Location", "/"); server.send(302); }
void handleNotFound() { server.send(404, "text/plain", "404: Not Found"); }

void startWebServer() {
  if (WiFi.status() != WL_CONNECTED) { out.println("Connect to Wi-Fi first using 'wifi_connect'"); return; }
  server.on("/", HTTP_GET, handleRoot);
  server.on("/sysinfo", HTTP_GET, handleSysInfo);
  server.on("/led/on", HTTP_GET, handleLedOn);
  server.on("/led/off", HTTP_GET, handleLedOff);
  server.onNotFound(handleNotFound);
  server.begin();
  out.println("Web server started!");
  out.print("Open this IP in your browser: http://");
  out.println(WiFi.localIP().toString());
  out.println("Command line is inactive. Press RESET on the board to exit.");
  while (true) { server.handleClient(); delay(1); }
}

void clearScreen() { out.print("\033[2J\033[H"); }

void matrixEffect() {
    clearScreen();
    out.println("Starting Matrix effect... Press any key to exit.");
    delay(1000);
    const int width = 40;
    int positions[width] = {0};
    while(!Serial.available() && !SerialBT.available()) {
        for(int i = 0; i < width; i++) {
            if(positions[i] > 0) {
                out.printf("\033[%d;%dH ", positions[i], i + 1);
            }
            if(positions[i] == 0 && random(0, 100) > 95) {
                positions[i] = 1;
            } else if (positions[i] > 0) {
                positions[i]++;
            }
            if(positions[i] > 20) positions[i] = 0;
            if(positions[i] > 0) {
                out.printf("\033[%d;%dH\033[32m%c\033[0m", positions[i], i + 1, random(33, 126));
            }
        }
        delay(50);
    }
    while(Serial.available()) Serial.read();
    while(SerialBT.available()) SerialBT.read();
    clearScreen();
    printLogo();
}

void printBanner(String text) {
    text.toUpperCase();
    const char* font[5][27] = {
        {"### ", " # ", "### ", "### ", "# # ", "### ", "### ", "# # ", "### ", "  # ", "# # ", "#   ", "# # ", "# # ", "### ", "### ", "### ", "### ", "### ", "### ", "# # ", "# # ", "# # ", "# # ", "# # ", "### "},
        {"# # ", "## ", "#  #", "#  #", "# # ", "#   ", "#   ", "# # ", " # ", "  # ", "# # ", "#   ", "### ", "## #", "# # ", "#  #", "# # ", "#  #", "#   ", " #  ", "# # ", "# # ", "# # ", " # ", " # ", "  # "},
        {"### ", " # ", "### ", "### ", "### ", "### ", "# ##", "### ", " # ", "  # ", "### ", "#   ", "# # ", "# ##", "### ", "### ", "### ", "### ", "### ", " #  ", "# # ", "# # ", "# # ", " # ", " # ", " #  "},
        {"# # ", " # ", "#   ", "#  #", "# # ", "#   ", "#  #", "# # ", " # ", "# # ", "# # ", "#   ", "# # ", "#  #", "# # ", "#   ", "#  #", "# # ", "  # ", " #  ", "# # ", " # ", " # #", " # ", " # ", "#   "},
        {"# # ", "###", "### ", "### ", "# # ", "### ", "### ", "# # ", "### ", "### ", "# # ", "### ", "# # ", "#  #", "# # ", "#   ", "#  #", "# # ", "### ", " #  ", "### ", " # ", "### ", " # ", "### ", "### "}
    };
    out.println();
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < text.length(); j++) {
            if (text[j] >= 'A' && text[j] <= 'Z') {
                out.print(font[i][text[j] - 'A']);
            } else {
                out.print("    ");
            }
        }
        out.println();
    }
    out.println();
}

void neofetch() {
    String logo[] = {
        " ____   ___   ____ ",
        "|  _ \\ / _ \\ / ___|",
        "| | | | | | |\\___ \\ ",
        "| |_| | |_| | ___) |",
        "|____/ \\___/ |____/ "
    };
    String os_name = "dazertOS v2.0.1";
    String chip_model = String(ESP.getChipModel()) + " rev" + String(ESP.getChipRevision());
    String cores = String(ESP.getChipCores()) + " cores";
    String wifi_mac = WiFi.macAddress();
    uint8_t bt_mac_addr[6];
    char bt_mac_str[18];
    esp_read_mac(bt_mac_addr, ESP_MAC_BT);
    sprintf(bt_mac_str, "%02X:%02X:%02X:%02X:%02X:%02X", bt_mac_addr[0], bt_mac_addr[1], bt_mac_addr[2], bt_mac_addr[3], bt_mac_addr[4], bt_mac_addr[5]);

    out.println();
    out.printf("%s   %s\n", logo[0].c_str(), os_name.c_str());
    out.printf("%s   --------------------\n", logo[1].c_str());
    out.printf("%s   Chip: %s\n", logo[2].c_str(), chip_model.c_str());
    out.printf("%s   Cores: %s\n", logo[3].c_str(), cores.c_str());
    out.printf("%s   WiFi MAC: %s\n", logo[4].c_str(), wifi_mac.c_str());
    out.printf("                     BT MAC:   %s\n", bt_mac_str);
    out.println();
}

void bleScan() {
    out.println("Scanning for BLE devices for 5 seconds...");
    BLEDevice::init("");
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->start(5, false);
    out.println("BLE scan finished.");
}

void wifi_sniffer_packet_handler(void* buff, wifi_promiscuous_pkt_type_t type) {
  if (isSniffing) {
    wifi_promiscuous_pkt_t *p = (wifi_promiscuous_pkt_t*)buff;
    wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)p->rx_ctrl;
    if (type == WIFI_PKT_MGMT && (p->payload[0] == 0x80 || p->payload[0] == 0x40)) {
      out.printf(" Beacon/Probe, RSSI: %d, Ch: %d, MAC: %02x:%02x:%02x:%02x:%02x:%02x \n",
        ctrl.rssi, ctrl.channel, p->payload[10], p->payload[11], p->payload[12],
        p->payload[13], p->payload[14], p->payload[15]);
    }
  }
}

void startSniffer(int channel) {
  if (channel < 1 || channel > 13) { out.println("Error: Channel must be between 1 and 13."); return; }
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
  isSniffing = true;
  out.printf("Wi-Fi sniffer started on channel %d. Type 'sniff_stop' to exit.\n", channel);
}

void stopSniffer() {
  if (!isSniffing) { out.println("Sniffer is not running."); return; }
  esp_wifi_set_promiscuous(false);
  isSniffing = false;
  out.println("Wi-Fi sniffer stopped.");
}

void deepSleep(int seconds) {
    if (seconds <= 0) {
        out.println("Error: Please provide a positive number of seconds.");
        return;
    }
    out.printf("Entering deep sleep for %d seconds. See you soon...\n", seconds);
    delay(100);
    esp_deep_sleep(seconds * 1000000);
}

void ledOn() { digitalWrite(LED_PIN, HIGH); out.println("OK. LED is now ON."); }
void ledOff() { digitalWrite(LED_PIN, LOW); out.println("OK. LED is now OFF."); }

void printSystemInfo() {
  out.println("\n---[ System Information ]---");
  out.printf("Chip Model: %s (rev %d)\n", ESP.getChipModel(), ESP.getChipRevision());
  out.printf("Cores: %d\n", ESP.getChipCores());
  out.printf("Flash Size: %d MB\n", ESP.getFlashChipSize() / (1024 * 1024));
  out.printf("MAC Address: %s\n", WiFi.macAddress().c_str());
  out.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
}

void interactiveWiFiConnect() {
  out.println("Starting Wi-Fi scan...");
  WiFi.mode(WIFI_STA); WiFi.disconnect(); delay(100);
  int n = WiFi.scanNetworks();
  if (n == 0) { out.println("No networks found."); return; }
  String ssid_list[n];
  out.printf("%d networks found:\n", n);
  for (int i = 0; i < n; ++i) {
    ssid_list[i] = WiFi.SSID(i);
    out.printf("%d: %s (%d dBm)\n", i + 1, ssid_list[i].c_str(), WiFi.RSSI(i));
  }
  out.print("Enter the number of the network (or 0 to cancel): ");
  String netNumStr = readSerialLine();
  int netNum = netNumStr.toInt();
  if (netNum <= 0 || netNum > n) { out.println("Invalid number. Aborting."); return; }
  String selectedSSID = ssid_list[netNum - 1];
  out.print("Enter password for \"" + selectedSSID + "\": ");
  String password = readSerialLine();
  out.print("Connecting to " + selectedSSID);
  WiFi.begin(selectedSSID.c_str(), password.c_str());
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500); out.print("."); attempts++;
  }
  out.println();
  if (WiFi.status() == WL_CONNECTED) { out.println("WiFi Connected! IP: " + WiFi.localIP().toString()); } 
  else { out.println("Failed to connect."); }
}

void wifiStatus() {
  out.println("\n---[ Wi-Fi Status ]---");
  if (WiFi.status() == WL_CONNECTED) {
    out.println("Status: Connected");
    out.println("SSID: " + WiFi.SSID());
    out.println("IP Address: " + WiFi.localIP().toString());
  } else { out.println("Status: Disconnected"); }
  out.println("----------------------");
}

void getTimeOverNTP() {
  if (WiFi.status() != WL_CONNECTED) { out.println("Connect to Wi-Fi first"); return; }
  configTime(3600, 3600, "pool.ntp.org");
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){ out.println("Failed to obtain time"); return; }
  out.println("\n---[ Current Date & Time ]---");
  char time_buffer[80];
  strftime(time_buffer, sizeof(time_buffer), "%A, %B %d %Y, %H:%M:%S", &timeinfo);
  out.println(time_buffer);
  out.println("-----------------------------");
}

void setVar(String arg) {
    int spaceIndex = arg.indexOf(' ');
    if (spaceIndex == -1) { out.println("Error: Use format 'set_var <key> <value>'"); return; }
    String key = arg.substring(0, spaceIndex);
    String value = arg.substring(spaceIndex + 1);
    preferences.begin("dazert-os", false);
    preferences.putString(key.c_str(), value);
    preferences.end();
    out.printf("Variable '%s' set to '%s'\n", key.c_str(), value.c_str());
}

void getVar(String key) {
    preferences.begin("dazert-os", true);
    String value = preferences.getString(key.c_str(), "(not found)");
    preferences.end();
    out.printf("Value of '%s' is: %s\n", key.c_str(), value.c_str());
}

void generatePassword(int length) {
  if (length <= 0 || length > 128) { out.println("Error: Length must be between 1 and 128."); return; }
  out.print("Generated Password: ");
  const char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()";
  String pass = "";
  for (int i = 0; i < length; i++) { pass += chars[random(0, strlen(chars))]; }
  out.println(pass);
}

void morseCommand(String text) {
  out.println("Transmitting Morse code via LED: " + text);
  for(int i = 0; i < text.length(); i++) { 
    int dot_len = 150;
    switch(toupper(text[i])) {
      case 'H': for(int j=0;j<4;j++){digitalWrite(LED_PIN, HIGH); delay(dot_len); digitalWrite(LED_PIN, LOW); delay(dot_len);} break;
      case 'E': digitalWrite(LED_PIN, HIGH); delay(dot_len); digitalWrite(LED_PIN, LOW); break;
      case 'L': digitalWrite(LED_PIN, HIGH); delay(dot_len); digitalWrite(LED_PIN, LOW); delay(dot_len); digitalWrite(LED_PIN, HIGH); delay(dot_len*3); digitalWrite(LED_PIN, LOW); delay(dot_len); for(int j=0;j<2;j++){digitalWrite(LED_PIN, HIGH); delay(dot_len); digitalWrite(LED_PIN, LOW); delay(dot_len);} break;
      case 'O': for(int j=0;j<3;j++){digitalWrite(LED_PIN, HIGH); delay(dot_len*3); digitalWrite(LED_PIN, LOW); delay(dot_len);} break;
      case ' ': delay(dot_len * 4); break;
    }
    delay(dot_len * 2);
  }
  out.println("\nTransmission complete.");
}

void executeCalc(String expression) {
  double num1, num2;
  char op;
  int items = sscanf(expression.c_str(), "%lf %c %lf", &num1, &op, &num2);
  if (items != 3) { out.println("Error: Invalid format. Use: calc <number> <operator> <number>"); return; }
  double result = 0;
  bool success = true;
  switch (op) {
    case '+': result = num1 + num2; break;
    case '-': result = num1 - num2; break;
    case '*': result = num1 * num2; break;
    case '/': 
      if (num2 == 0) { out.println("Error: Division by zero!"); success = false; }
      else { result = num1 / num2; }
      break;
    default: out.println("Error: Unknown operator. Use +, -, *, /"); success = false; break;
  }
  if (success) {
    out.print("Result: ");
    out.println(String(result));
  }
}

void guessTheNumberGame() {
  out.println("\n---[ Guess the Number Game ]---");
  out.println("I'm thinking of a number (1-100). Type 'exit' to quit.");
  int target = random(1, 101);
  while(true) {
    out.print("Your guess: ");
    String guessStr = readSerialLine();
    if (guessStr == "exit") { out.println("Thanks for playing!"); break; }
    int guess = guessStr.toInt();
    if (guess < target) out.println("Too low!");
    else if (guess > target) out.println("Too high!");
    else { out.println("You got it!"); break; }
  }
}

void ticTacToeGame() {
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
    out.println("\n---[ Tic-Tac-Toe ]---");
    printBoard();
    while (true) {
        out.print("Your move (1-9): ");
        int move = readSerialLine().toInt();
        if (move < 1 || move > 9 || board[move-1] == 'X' || board[move-1] == 'O') {
            out.println("Invalid move. Try again.");
            continue;
        }
        board[move-1] = 'X';
        printBoard();
        char winner = checkWin();
        if (winner != ' ') {
            if (winner == 'X') out.println("You win!");
            else if (winner == 'D') out.println("It's a draw!");
            break;
        }
        out.println("Computer's move...");
        int computer_move;
        do { computer_move = random(0, 9); } 
        while (board[computer_move] == 'X' || board[computer_move] == 'O');
        board[computer_move] = 'O';
        delay(500);
        printBoard();
        winner = checkWin();
        if (winner != ' ') {
            if (winner == 'O') out.println("Computer wins!");
            else if (winner == 'D') out.println("It's a draw!");
            break;
        }
    }
}

void printHelp() {
  out.println();
  printLogo();
  out.println("\n---[ Command List ]---");
  out.println("  help               - Show this list");
  out.println("  neofetch           - Show a cool system summary");
  out.println("  sys_info           - Show detailed system information");
  out.println("  reboot             - Reboot the ESP32");
  out.println("  deepsleep <secs>   - Enter deep sleep mode");
  out.println("  clear              - Clear the screen");
  out.println("\n-- Wi-Fi & Bluetooth --");
  out.println("  wifi_connect, wifi_status, get_time");
  out.println("  ble_scan           - Scan for Bluetooth Low Energy devices");
  out.println("  sniff_start <ch>   - Start Wi-Fi sniffer on a channel");
  out.println("  sniff_stop         - Stop the Wi-Fi sniffer");
  out.println("\n-- Utilities & Fun --");
  out.println("  led_on/off, pass_gen, morse, calc, matrix");
  out.println("  banner <text>      - Print a large text banner");
  out.println("  benchmark          - Run a simple CPU benchmark");
  out.println("  set_var, get_var   - Save and read variables");
  out.println("\n-- Games --");
  out.println("  game_guess, game_tictactoe");
  out.println("------------------------------------");
}

void processCommand(String cmd) {
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
  else if (command == "neofetch") neofetch();
  else if (command == "led_on") ledOn();
  else if (command == "led_off") ledOff();
  else if (command == "reboot") { out.println("Rebooting..."); delay(100); ESP.restart(); }
  else if (command == "deepsleep") deepSleep(arg.toInt());
  else if (command == "wifi_connect") interactiveWiFiConnect();
  else if (command == "wifi_status") wifiStatus();
  else if (command == "ble_scan") bleScan();
  else if (command == "sniff_start") startSniffer(arg.toInt());
  else if (command == "sniff_stop") stopSniffer();
  else if (command == "pass_gen") generatePassword(arg.toInt());
  else if (command == "morse") morseCommand(arg);
  else if (command == "calc") executeCalc(arg);
  else if (command == "game_guess") guessTheNumberGame();
  else if (command == "game_tictactoe") ticTacToeGame();
  else if (command == "get_time") getTimeOverNTP();
  else if (command == "set_var") setVar(arg);
  else if (command == "get_var") getVar(arg);
  else if (command == "benchmark") executeBenchmark();
  else if (command == "web_server") startWebServer();
  else if (command == "clear") clearScreen();
  else if (command == "matrix") matrixEffect();
  else if (command == "banner") printBanner(arg);
  else {
    out.println("Error: Unknown command '" + command + "'");
  }
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("dazertOS"); 
  pinMode(LED_PIN, OUTPUT);
  delay(1000);
  preferences.begin("dazert-os", false);
  preferences.end();
  printLogo();
  out.println("Type 'help' for a list of commands.");
}

void loop() {
  if (isSniffing) {
    String command = "";
    if (Serial.available()) { command = Serial.readStringUntil('\n'); } 
    else if (SerialBT.available()) { command = SerialBT.readStringUntil('\n'); }
    command.trim();
    if (command == "sniff_stop") {
      processCommand("sniff_stop");
      out.print("\n> ");
    }
    return;
  }

  out.print("\n> ");
  String command = readSerialLine();
  processCommand(command);
}
