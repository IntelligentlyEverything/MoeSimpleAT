/**
 * at_commands.cpp - AT Commands
 */

#include <string>
#include "MoeSimpleAT.h"

#if defined(ESP32)
    #include <vector>
    #include <esp_heap_caps.h>
    #include <esp_sleep.h>
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
#endif

// ----------------------------
// global variable
// ----------------------------

HardwareSerial* atSerial = &Serial;
String inputBuffer;
bool inLogMode = false;
bool inShellMode = false;

// User defined instruction list
std::vector<CustomATCommand> customATCommands;
std::vector<CustomShellCommand> customShellCommands;

// Static variable to hold the restore callback
static std::function<void()> restoreCallback = nullptr;

// Static variable to hold the shutdown callback
static std::function<void()> shutdownCallback = nullptr;

static bool shellFirstPromptDone = false;

bool wakeupConfigured = false;

// ----------------------------
// User callable function implementation
// ----------------------------

/**
 * @brief Initialize AT command system and print startup message.
 */
void initATCommands() {
    if (atSerial) {
        atSerial->println();
        atSerial->println();
        atSerial->println(AT_WELCOME);
        atSerial->println("ready");
    }
}

/**
 * @brief Register a custom AT command.
 * 
 * Example: registerATCommand("LED", ledHandler, "Control LED");
 *          Then user can type 'LED ON' in shell mode.
 * 
 * @param cmd     Command name (case-insensitive)
 * @param handler Function to call when command is received
 * @param help    Description shown in help menu
 */
void registerATCommand(const String& cmd, const ATCommandHandler& handler, const String& help) {
    customATCommands.push_back({ "AT+" + cmd, handler, help });
}

/**
 * @brief Register a custom shell command.
 * 
 * Example: registerShellCommand("LED", ledHandler, "Control LED");
 *          Then user can type 'LED ON' in shell mode.
 * 
 * @param cmd     Command name (case-insensitive)
 * @param handler Function to call when command is received
 * @param help    Description shown in help menu
 */
void registerShellCommand(const String& cmd, const ShellCommandHandler& handler, const String& help) {
    customShellCommands.push_back({ cmd, handler, help });
}

/**
 * @brief Get the help message for all AT commands.
 * 
 * @return String containing the help message
 */
String getATHelp() {
    String help = "Built-in Commands:\r\n";
    help += "  AT           - Test\r\n";
    help += "  AT+RST       - Reset system\r\n";
    help += "  AT+GMR       - Show version info\r\n";
    help += "  AT+RESTORE   - Clear user settings\r\n";
    help += "  AT+UART?     - Show UART baud\r\n";
    help += "  AT+UART=9600 - Set UART baud\r\n";
    help += "  AT+SYSRAM?   - Show system RAM usage\r\n";
    help += "  AT+SHELL     - Enter shell mode\r\n";
    help += "  AT+LOG       - Enter log mode\r\n";
    help += "  AT+HELP      - Show this help\r\n";
    help += "Custom Commands:\r\n";
    if (customATCommands.empty()) {
        help += "  No custom commands registered.\r\n";
    }
    for (const auto& c : customATCommands) {
        help += "  " + c.command + "  - " + c.help + "\n";
    }
    return help;
}

/**
 * @brief Get the help message for all shell commands.   
 * 
 * @return String containing the help message
 */
String getShellHelp() {
    String help = "Built-in Shell Commands:\r\n";
    help += "  echo <text>                      - Print text\r\n";
    help += "  free [-b|-k|-m] [-t] [-s delay]  - Show memory usage\r\n";
    help += "  ping [args]                      - Network ping (if supported)\r\n";
    help += "  ifconfig                         - Show network config (if supported)\r\n";
    help += "  top                              - Show system tasks (if supported)\r\n";
    help += "  kill [pid]                       - Kill task by PID (if supported)\r\n";
    help += "  reboot                           - Restart system\r\n";
    help += "  shutdown                         - Shutdown system\r\n";
    help += "  exit                             - Exit shell mode\r\n";
    help += "  help                             - Show this message\r\n";
    help += "Custom Commands:\r\n";
    if (customShellCommands.empty()) {
        help += "  No custom shell commands registered.\r\n";
    }

    for (const auto& c : customShellCommands) {
        help += "  " + c.command + "  - " + c.help + "\r\n";
    }
    return help;
}

void log(const String& msg) {
    if (inLogMode) {
        atSerial->println(msg);
    }
}

// ----------------------------
// Internal Tool Functions
// ----------------------------

String trim(const String& str) {
    String s = str;
    s.trim();
    return s;
}

// ----------------------------
// AT Command Handler
// ----------------------------

void onRestore(std::function<void()> callback) {
    restoreCallback = callback;
}

void onShutdown(std::function<void()> callback) {
    shutdownCallback = callback;
}

long currentBaudRate = SERIAL_BAUD_RATE;

void processATCommand(const String& fullCmd) {
    String cmd = trim(fullCmd);
    if (cmd.length() == 0) return;

    // Log mode only respond to EXIT
    if (inLogMode) {
        if (cmd.equalsIgnoreCase("EXIT")) {
            inLogMode = false;
            atSerial->println("OK");
        }
        return;
    }

    // Convert to uppercase
    cmd.toUpperCase();

    // Built-in command matching
    if (cmd == "AT") {
        atSerial->println("OK");
    }
    else if (cmd == "AT+RST") {
        atSerial->println("OK");
        delay(100);
        #ifdef MOE_RTOS_PLATFORM_AIR001
            void(* resetFunc) (void) = 0;
            resetFunc();
        #else
            ESP.restart();
        #endif
    }
    else if (cmd == "AT+GMR") {
        atSerial->print(SYSTEM_NAME);
        atSerial->print(" ");
        atSerial->println(SYSTEM_VERSION);
        atSerial->print("User Program Name: ");
        atSerial->println(USER_PROGRAM_NAME);
        atSerial->print("User Program Version: ");
        atSerial->println(USER_PROGRAM_VERSION);
        atSerial->print("Compiled: ");
        atSerial->println(COMPILED_DATETIME);
        atSerial->println(" ");
        atSerial->println("OK");
    }
    else if (cmd == "AT+RESTORE") {
        atSerial->println("OK");
        
        atSerial->flush(); // Ensure serial port output is complete

        // If the user has registered for a recovery callback, execute
        if (restoreCallback) {
            restoreCallback();
        }
    }
    else if (cmd.startsWith("AT+UART")) {
        if (cmd == "AT+UART?") {
            atSerial->print("+UART:");
            atSerial->println(currentBaudRate);
            atSerial->println();
            atSerial->println("OK");
        }
        else if (cmd.startsWith("AT+UART=")) {
            long baud = cmd.substring(8).toInt();
            if (baud <= 0) {
                atSerial->println("ERROR");
            }
            if (baud > 0) {
                atSerial->end();
                atSerial->begin(baud);
                currentBaudRate = baud;
            }
            atSerial->println("OK");
        }
    }
    else if (cmd == "AT+LOG") {
        inLogMode = true;
        atSerial->println("Entering log mode. Type 'EXIT' to return.");
    }
    else if (cmd == "AT+SYSRAM?") {
        size_t total = 0;
        size_t free = 0;
        size_t used = 0;

        // ========================
        // Platform-specific memory query
        // ========================

        #if defined(ESP32)
            // ESP32: has full APIs
            total = ESP.getHeapSize();
            free = ESP.getFreeHeap();
            used = total - free;

        #elif defined(ESP8266)
            // ESP8266: Only getFreeHeap() is available
            free = ESP.getFreeHeap();

            // Total RAM
            total = TOTAL_DRAM_SIZE; // 52KB

            // Avoid underflow
            used = (free < total) ? (total - free) : 0;

            // Clamp used to total
            if (used > total) used = total;

        #elif defined(MOE_RTOS_PLATFORM_AIR001)
            // Air001 / STM32: Use linker symbols
            extern uint32_t _heap_start;
            extern uint32_t _heap_end;
            char* heap_brk = (char*)sbrk(0);
            char* heap_start = (char*)&_heap_start;
            char* heap_end = (char*)&_heap_end;

            total = heap_end - heap_start;

            if ((char*)heap_brk >= heap_start && (char*)heap_brk <= heap_end) {
                used = (char*)heap_brk - heap_start;
            } else {
                used = 0;
            }
        #endif

        // Output in standard format
        atSerial->print("+SYSRAM:");
        atSerial->print(total);
        atSerial->print(",");
        atSerial->print(used);
        atSerial->println();
        atSerial->println();
        atSerial->println("OK");
    }
    else if (cmd == "AT+SHELL") {
        inShellMode = true;
        atSerial->println("Entering shell mode. Type 'exit' to return.");
        atSerial->println();
        atSerial->print(SYSTEM_NAME);
        atSerial->print(" ");
        atSerial->print(SYSTEM_VERSION);
        atSerial->println(" built-in shell (msh)");
        atSerial->println("Enter 'help' for a list of built-in commands.");
        shellFirstPromptDone = false;
    }
    else if (cmd == "AT+HELP" || cmd == "AT+?") {
        atSerial->print(getATHelp());
        atSerial->println("OK");
    }
    else {
        // Attempt to match user-defined commands
        bool matched = false;
        for (auto& c : customATCommands) {
            if (cmd.startsWith(c.command)) {
                String args = "";
                if (cmd.length() > c.command.length()) {
                    args = cmd.substring(c.command.length());
                }
                c.handler(args);
                matched = true;
                break;
            }
        }
        if (!matched) {
            atSerial->println("error");
        }
    }
}

// ----------------------------
// Free Command Handler
// ----------------------------
static void handleFreeCommand(const String& args) {
    // Parse arguments
    bool showTotal = false;
    int delaySec = -1;  // -1 = no loop
    long unit = 1024;   // default: KB

    String arg = args;
    arg.trim();

    int index = 0;
    while (index < arg.length()) {
        int space = arg.indexOf(' ', index);
        String token = (space == -1) ? arg.substring(index) : arg.substring(index, space);
        token.trim();
        if (token == "-b") unit = 1;
        else if (token == "-k") unit = 1024;
        else if (token == "-m") unit = 1024 * 1024;
        else if (token == "-t") showTotal = true;
        else if (token == "-s" && space != -1) {
            int nextSpace = arg.indexOf(' ', space + 1);
            String delayStr = (nextSpace == -1) ? arg.substring(space + 1) : arg.substring(space + 1, nextSpace);
            delaySec = delayStr.toInt();
            if (delaySec < 1) delaySec = 1;
            index = nextSpace == -1 ? arg.length() : nextSpace + 1;
            continue;
        }
        index = space == -1 ? arg.length() : space + 1;
    }

    // Helper: format number (right-aligned, fixed width)
    auto formatNum = [](long value, int width = 12) -> String {
        String s = String(value);
        while (s.length() < width) s = " " + s;
        return s;
    };

    // Helper: format string (right-aligned, fixed width)
    auto formatStr = [](const String& str, int width = 12) -> String {
        String s = str;
        while (s.length() < width) s = " " + s;
        return s;
    };

    // Helper: print memory info
    auto printMem = [unit, showTotal, formatNum, formatStr](HardwareSerial* ser) {
        ser->print("         ");
        ser->print(formatStr("total", 12));
        ser->print(formatStr("used", 12));
        ser->print(formatStr("free", 12));
        ser->println();

        #if defined(ESP32)
            // Internal RAM
            size_t total_iram = ESP.getHeapSize();
            size_t free_iram = ESP.getFreeHeap();
            size_t used_iram = total_iram - free_iram;
            
            long total_iram_u = total_iram / unit;
            long used_iram_u = used_iram / unit;
            long free_iram_u = free_iram / unit;

            ser->print("Ram:     ");
            ser->print(formatNum(total_iram_u));
            ser->print(formatNum(used_iram_u));
            ser->print(formatNum(free_iram_u));

            ser->println();

            // Detect and print PSRAM(s)
            std::vector<size_t> psram_sizes;
            std::vector<size_t> psram_free;

            // Try to detect PSRAM via heap_caps
            #ifdef CONFIG_SPIRAM
                // Use heap_caps to get PSRAM size and free
                size_t psram_total = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
                if (psram_total > 0) {
                    size_t psram_free_size = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
                    psram_sizes.push_back(psram_total);
                    psram_free.push_back(psram_free_size);
                }
            #endif

            // Optional: support multiple PSRAM chips (if your hardware supports it)
            // This is a placeholder; actual multi-PSRAM detection depends on hardware/driver
            // For now, assume at most one PSRAM block via SPIRAM

            for (int i = 0; i < psram_sizes.size(); i++) {
                size_t total_p = psram_sizes[i];
                size_t free_p = psram_free[i];
                size_t used_p = total_p - free_p;

                long total_p_u = total_p / unit;
                long used_p_u = used_p / unit;
                long free_p_u = free_p / unit;

                ser->print("PSRam("); ser->print(i); ser->print("):");
                ser->print(formatNum(total_p_u));
                ser->print(formatNum(used_p_u));
                ser->print(formatNum(free_p_u));
                ser->println();
            }

            // Print Total if requested
            if (showTotal) {
                size_t total_mem = total_iram;
                size_t used_mem = used_iram;
                size_t free_mem = free_iram;

                for (size_t sz : psram_sizes) total_mem += sz;
                // Note: used from PSRAM is already (total - free), so just sum free and subtract
                size_t total_free_psram = 0;
                for (size_t f : psram_free) total_free_psram += f;
                free_mem += total_free_psram;
                used_mem = total_mem - free_mem;

                long total_mem_u = total_mem / unit;
                long used_mem_u = used_mem / unit;
                long free_mem_u = free_mem / unit;

                ser->print("Total:   ");
                ser->print(formatNum(total_mem_u));
                ser->print(formatNum(used_mem_u));
                ser->print(formatNum(free_mem_u));
                ser->println();
            }
        #elif defined(ESP8266)
            // IRAM heap
            size_t total_iram_heap = TOTAL_IRAM_SIZE;

            ser->print("IRam:    ");
            ser->print(formatNum(total_iram_heap));
            ser->print(formatStr("N/A"));
            ser->print(formatStr("N/A"));
            ser->println();

            // DRAM heap
            size_t free_dram_heap = ESP.getFreeHeap();
            size_t total_dram_heap = TOTAL_DRAM_SIZE;
            size_t used_dram_heap = (free_dram_heap < total_dram_heap) ? total_dram_heap - free_dram_heap : 0;
            if (used_dram_heap > total_dram_heap) used_dram_heap = total_dram_heap;

            long total_dram_heap_u = total_dram_heap / unit;
            long used_dram_heap_u = used_dram_heap / unit;
            long free_dram_heap_u = free_dram_heap / unit;

            ser->print("DRam:    ");
            ser->print(formatNum(total_dram_heap_u));
            ser->print(formatNum(used_dram_heap_u));
            ser->print(formatNum(free_dram_heap_u));
            ser->println();

            // Print Total if requested
            if (showTotal) {
                size_t total_mem = total_iram_heap + total_dram_heap;
                size_t used_mem = used_dram_heap;
                size_t free_mem = free_dram_heap;

                long total_mem_u = total_mem / unit;
                long used_mem_u = used_mem / unit;
                long free_mem_u = free_mem / unit;

                ser->print("Total:   ");
                ser->print(formatNum(total_mem_u));
                ser->print(formatNum(used_mem_u));
                ser->print(formatNum(free_mem_u));
                ser->println();
            }
        #elif defined(MOE_RTOS_PLATFORM_AIR001)
            extern uint32_t _heap_start;
            extern uint32_t _heap_end;
            char* heap_brk = (char*)sbrk(0);
            char* heap_start = (char*)&_heap_start;
            char* heap_end = (char*)&_heap_end;
            size_t total = heap_end - heap_start;
            size_t used = ((char*)heap_brk >= heap_start && (char*)heap_brk <= heap_end)
                       ? (char*)heap_brk - heap_start : 0;
            size_t free = (used < total) ? total - used : 0;

            long total_u = total / unit;
            long used_u = used / unit;
            long free_u = free / unit;

            ser->print("Ram:     ");
            ser->print(formatRight(total_u));
            ser->print(formatRight(used_u));
            ser->print(formatRight(free_u));
            ser->println();
        #endif // platform
    };
    // Execute print (loop if needed)
    do {
        printMem(atSerial);
        if (delaySec > 0) {
            delay(delaySec * 1000);
            atSerial->println(); // separator
        }
    } while (delaySec > 0);
}
// ----------------------------
// Shutdown Command Handler
// ----------------------------
static void handleShutdownCommand() {
    atSerial->println();
    // Check if the startup source is configured
    if (!wakeupConfigured) {
        atSerial->println("The startup related logic is not enabled, so it may not be able to start up.");
    }

    // Prompt to shut down
    atSerial->println("The system is going down for shutdown NOW!");

    // If a shutdown callback is registered, execute
    if (shutdownCallback) {
        shutdownCallback();
    }

    #if defined(ESP32)
        esp_deep_sleep_start();
    #elif defined(ESP8266)
        ESP.deepSleep(0);
    #elif defined(MOE_RTOS_PLATFORM_AIR001)
        atSerial->println("Air001 currently does not support shutdown commands.");
    #endif
}

// ----------------------------
// Shell Mode Handler
// ----------------------------
void handleShellMode() {
    static String line = "";
    static bool expectLF = false;  // Status: Waiting for \n (as just received \r)

    if (!shellFirstPromptDone && inShellMode) {
        atSerial->println();
        atSerial->print("msh> ");
        shellFirstPromptDone = true;
    }

    while (atSerial->available()) {
        char c = atSerial->read();

        // handle line breaks
        if (c == '\r') {
            // Received CR, waiting for LF (entering CRLF mode)
            expectLF = true;
            continue;  // Not processed, waiting for the next character
        }
        else if (c == '\n') {
            if (expectLF) {
                // It is the LF part of CRLF, processed normally
                expectLF = false;
            }
            // If! expectLF， Explanation: It is a separate LF and can be processed normally (compatible)

            // execute command
            if (line.length() > 0) {
                atSerial->println();  // Line break, end input display

                String cmdLine = trim(line);
                String args = trim(line);
                if (cmdLine == "help" || cmdLine == "HELP" || cmdLine == "?") {
                    atSerial->println();
                    atSerial->print(getShellHelp());
                }
                else if (cmdLine =="exit" || cmdLine =="EXIT") {
                    atSerial->println("OK");
                    inShellMode = false;
                    line = "";
                    return;
                }
                else if (cmdLine == "reboot") {
                    atSerial->print(__DATE__);
                    atSerial->print(" ");
                    atSerial->print(__TIME__);
                    atSerial->println();
                    atSerial->println("The system is going down for reboot NOW!");
                    delay(100);
                    #ifdef MOE_RTOS_PLATFORM_AIR001
                        void(* resetFunc) (void) = 0;
                        resetFunc();
                    #else
                        ESP.restart();
                    #endif
                }
                else if (cmdLine == "shutdown") {
                    handleShutdownCommand();
                }
                else if (cmdLine.startsWith("echo ")) {
                    String output = cmdLine.substring(5);
                    output.trim();
                    atSerial->println(output);
                }
                else if (cmdLine == "echo") {
                    atSerial->println();
                }
                else if (cmdLine.startsWith("free ")) {
                    handleFreeCommand(args);
                }
                else if (cmdLine == "free") {
                    handleFreeCommand(args);
                }
                else {
                    // Check built-in extended commands (ping, ifconfig, top, kill)
                    static const char* builtinCmds[] = { "ping", "ifconfig", "top", "kill" };

                    bool isBuiltin = false;
                    for (const auto& name : builtinCmds) {
                        std::string nameStr = std::string(name) + " ";
                        if (cmdLine.startsWith(nameStr.c_str())) {
                            isBuiltin = true;
                            bool found = false;
                            for (auto& c : customShellCommands) {
                                if (c.command == name) {
                                    c.handler(args);
                                    found = true;
                                    break;
                                }
                            }
                            if (!found) {
                                atSerial->println("msh: applet not found");
                            }
                            break;
                        }
                        if (cmdLine == name) {
                            isBuiltin = true;
                            bool found = false;
                            for (auto& c : customShellCommands) {
                                if (c.command == name) {
                                    c.handler("");
                                    found = true;
                                    break;
                                }
                            }
                            if (!found) {
                                atSerial->println("msh: applet not found");
                            }
                            break;
                        }
                    }

                    // If a known builtin command, execute it and return
                    if (isBuiltin) {
                        // Output a new prompt
                        atSerial->print("msh> ");
                        line = "";
                        return;
                    }

                    // If not a known builtin, check all custom commands
                    int spaceIndex = cmdLine.indexOf(' ');
                    String cmdName = (spaceIndex == -1) ? cmdLine : cmdLine.substring(0, spaceIndex);
                    String args = (spaceIndex == -1) ? "" : cmdLine.substring(spaceIndex + 1);

                    cmdName.trim();
                    args.trim();

                    bool matched = false;
                    for (auto& c : customShellCommands) {
                        if (cmdName.equalsIgnoreCase(c.command)) {
                            c.handler(args);
                            matched = true;
                            break;
                        }
                    }

                    if (!matched) {
                        atSerial->println("msh: not found");
                    }
                }
            }

            // Output a new prompt
            atSerial->print("msh> ");
            line = "";
        }
        else {
            // Non line breaking characters: Reset expectLF (to prevent timeout waiting)
            if (expectLF) {
                // Received non \n characters, indicating that \r is not part of CRLF and should be considered invalid
                expectLF = false;
            }

            if (c == 8 || c == 127) { // Backspace/Delete
                if (line.length() > 0) {
                    line.remove(line.length() - 1);
                    atSerial->print("\b \b");
                }
            }
            else if (c >= 32 && c < 127) { // Printable
                line += c;
                atSerial->print(c);
            }
        }
    }
}

// ----------------------------
// Main Loop Handler
// ----------------------------

void handleATCommands() {
    if (inShellMode) {
        handleShellMode();
        return;
    }

    if (inLogMode) {
        static char prevChar = 0;
        while (atSerial->available()) {
            char c = atSerial->read();

            if (c == '\n' && prevChar == '\r') {
                inputBuffer = trim(inputBuffer);
                if (inputBuffer.equalsIgnoreCase("EXIT")) {
                    inLogMode = false;
                    atSerial->println("OK");
                }
                inputBuffer = "";
                prevChar = 0;
            }
            else if (c == '\r') {
                prevChar = c;
            }
            else if (c == '\n') {
                inputBuffer = trim(inputBuffer);
                if (inputBuffer.equalsIgnoreCase("EXIT")) {
                    inLogMode = false;
                    atSerial->println("OK");
                }
                inputBuffer = "";
                prevChar = 0;
            }
            else {
                if (prevChar == '\r') {
                    inputBuffer += '\r';
                }
                inputBuffer += c;
                prevChar = 0;
            }
        }
        return;
    }

    static char prevChar = 0;
    while (atSerial->available()) {
        char c = atSerial->read();

        if (c == '\n' && prevChar == '\r') {
            inputBuffer = trim(inputBuffer);
            if (!inputBuffer.isEmpty()) {
                processATCommand(inputBuffer);
            }
            inputBuffer = "";
            prevChar = 0;
        }
        else if (c == '\r') {
            prevChar = c;
        }
        else if (c == '\n') {
            inputBuffer = trim(inputBuffer);
            if (!inputBuffer.isEmpty()) {
                processATCommand(inputBuffer);
            }
            inputBuffer = "";
            prevChar = 0;
        }
        else {
            if (prevChar == '\r') {
                inputBuffer += '\r';
            }
            inputBuffer += c;
            prevChar = 0;
        }
    }
}