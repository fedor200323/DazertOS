#include "WiFi.h"
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define LED_PIN 2

BluetoothSerial SerialBT;

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
  out.println("     DazertOS v1.1");
  out.println();
}

void ledOn() { out.println("OK. LED is now ON."); digitalWrite(LED_PIN, HIGH); }
void ledOff() { out.println("OK. LED is now OFF."); digitalWrite(LED_PIN, LOW); }

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
  out.println("Scan complete.");

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
    out.print("Signal Strength (RSSI): " + String(WiFi.RSSI()) + " dBm\n");
  } else { out.println("Status: Disconnected"); }
  out.println("----------------------");
}

void generatePassword(int length) {
  if (length <= 0 || length > 128) { out.println("Error: Length must be between 1 and 128."); return; }
  out.print("Generated Password: ");
  const char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()";
  String pass = "";
  for (int i = 0; i < length; i++) {
    pass += chars[random(0, strlen(chars))];
  }
  out.println(pass);
}

void blinkMorse(char c) {
  int dot_len = 150;
  switch(toupper(c)) {
    case 'H': for(int i=0;i<4;i++){digitalWrite(LED_PIN, HIGH); delay(dot_len); digitalWrite(LED_PIN, LOW); delay(dot_len);} break;
    case 'E': digitalWrite(LED_PIN, HIGH); delay(dot_len); digitalWrite(LED_PIN, LOW); break;
    case 'L': digitalWrite(LED_PIN, HIGH); delay(dot_len); digitalWrite(LED_PIN, LOW); delay(dot_len); digitalWrite(LED_PIN, HIGH); delay(dot_len*3); digitalWrite(LED_PIN, LOW); delay(dot_len); for(int i=0;i<2;i++){digitalWrite(LED_PIN, HIGH); delay(dot_len); digitalWrite(LED_PIN, LOW); delay(dot_len);} break;
    case 'O': for(int i=0;i<3;i++){digitalWrite(LED_PIN, HIGH); delay(dot_len*3); digitalWrite(LED_PIN, LOW); delay(dot_len);} break;
    case ' ': delay(dot_len * 4); break;
  }
  delay(dot_len * 2);
}

void morseCommand(String text) {
  out.println("Transmitting Morse code via LED: " + text);
  for(int i = 0; i < text.length(); i++) {
    blinkMorse(text[i]);
  }
  out.println("\nTransmission complete.");
}

void executeCalc(String expression) {
  double num1, num2;
  char op;
  int items = sscanf(expression.c_str(), "%lf %c %lf", &num1, &op, &num2);

  if (items != 3) {
    out.println("Error: Invalid format. Use: calc <number> <operator> <number>");
    return;
  }
  
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
  out.println("  sys_info           - Show system information");
  out.println("  reboot             - Reboot the ESP32");
  out.println("  led_on / led_off   - Control the LED");
  out.println("\n-- Wi-Fi --");
  out.println("  wifi_scan          - Scan for networks");
  out.println("  wifi_connect       - Connect to a network");
  out.println("  wifi_status        - Show connection status");
  out.println("\n-- Utilities --");
  out.println("  pass_gen [len]     - Generate a random password");
  out.println("  morse [text]       - Blink a message");
  out.println("  calc <expression>  - Simple calculator (e.g., calc 10 * 5)");
  out.println("\n-- Games --");
  out.println("  game_guess         - Play Guess the Number");
  out.println("  game_tictactoe     - Play Tic-Tac-Toe");
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
  else if (command == "led_on") ledOn();
  else if (command == "led_off") ledOff();
  else if (command == "reboot") { out.println("Rebooting..."); delay(100); ESP.restart(); }
  else if (command == "wifi_scan") WiFi.scanNetworks(true);
  else if (command == "wifi_connect") interactiveWiFiConnect();
  else if (command == "wifi_status") wifiStatus();
  else if (command == "pass_gen") generatePassword(arg.toInt());
  else if (command == "morse") morseCommand(arg);
  else if (command == "calc") executeCalc(arg);
  else if (command == "game_guess") guessTheNumberGame();
  else if (command == "game_tictactoe") ticTacToeGame();
  else {
    out.println("Error: Unknown command '" + command + "'");
  }
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("dazertOS"); 
  
  pinMode(LED_PIN, OUTPUT);
  delay(1000);
  
  printLogo();
  out.println("Type 'help' for a list of commands.");
}

void loop() {
  out.print("\n> ");
  String command = readSerialLine();
  processCommand(command);
}