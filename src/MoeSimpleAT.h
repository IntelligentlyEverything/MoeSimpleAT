/**
 * at_commands.h - Header file for MoeSimple AT Command System
 * 
 * This module provides a simple AT command parser and handler framework.
 * Users can register custom AT commands and enter log mode for debugging.
 */

#ifndef MOE_SIMPLE_AT_H
#define MOE_SIMPLE_AT_H

#include <Arduino.h>
#include <vector>
#include <functional>

// ----------------------------
// User configurable items
// ----------------------------

#ifndef SERIAL_BAUD_RATE
  #define SERIAL_BAUD_RATE 115200
#endif

#ifndef TOTAL_IRAM_SIZE
  #define TOTAL_IRAM_SIZE 32768
#endif

#ifndef TOTAL_DRAM_SIZE
  #define TOTAL_DRAM_SIZE 50000
#endif

// [Required] User program name (format: "MyProgram")
#ifndef USER_PROGRAM_NAME
  #define USER_PROGRAM_NAME "MoeSimple AT Program"
#endif

// [Required] User program version (format: "v1.0" or "1.2.3")
#ifndef USER_PROGRAM_VERSION
  #define USER_PROGRAM_VERSION "dev"
#endif

// [Required] Startup message to display on boot
#ifndef AT_WELCOME
  #define AT_WELCOME "MoeSimple AT System by YooTrans."
#endif

// [Required] System name
#ifndef SYSTEM_NAME
  #define SYSTEM_NAME "MoeSimple AT System"
#endif

// [Required] System version
#ifndef SYSTEM_VERSION
  #define SYSTEM_VERSION "dev"
#endif

// [Required] Compile date and time (format: "YYYY-MM-DD HH:MM:SS")
#ifndef COMPILED_DATETIME
  #define COMPILED_DATETIME "1900-01-01 00:00:00"
#endif

// ----------------------------
// Forward declarations
// ----------------------------
class HardwareSerial;

// ----------------------------
// Type Definitions
// ----------------------------

/**
 * @brief Function type for handling custom AT commands.
 * 
 * @param args The argument string after the command (e.g., "=115200" for AT+UART=115200)
 */
using ATCommandHandler = std::function<void(const String& args)>;

/**
 * @brief Function type for handling custom Shell commands.
 * 
 * @param args The argument string after the command (e.g., "=115200" for AT+UART=115200)
 */
using ShellCommandHandler = std::function<void(const String& args)>;

/**
 * @brief Structure representing a custom AT command.
 */
struct CustomATCommand {
    String command;           // Full command string (e.g., "AT+MYCMD")
    ATCommandHandler handler; // Callback function
    String help;              // Help text description
};

/**
 * @brief Structure representing a custom shell command.
 */
struct CustomShellCommand {
    String command;           // Full command string (e.g., "MYCMD")
    ShellCommandHandler handler; // Callback function
    String help;              // Help text description
};

// ----------------------------
// External Global Variables
// ----------------------------

// Pointer to the serial interface used for AT commands
extern HardwareSerial* atSerial;

// Input buffer for accumulating serial data
extern String inputBuffer;

// Flag indicating whether the system is in log output mode
extern bool inLogMode;

// Flag indicating whether the system is in shell mode
extern bool inShellMode;

// Vector of user-registered custom AT commands
extern std::vector<CustomATCommand> customATCommands;

// Vector of user-registered custom shell commands
extern std::vector<CustomShellCommand> customShellCommands;

// ----------------------------
// User Callable Functions
// ----------------------------

/**
 * @brief Initialize AT command system and print startup message.
 * 
 * Must be called after serial is initialized.
 */
void initATCommands();

/**
 * @brief Register a custom AT command.
 * 
 * The command will be prefixed with "AT+" automatically.
 * Example: registerATCommand("MYCMD", myHandler, "My custom command");
 *          -> Can be triggered via "AT+MYCMD"
 * 
 * @param cmd     Command name (without "AT+")
 * @param handler Function to call when command is received
 * @param help    Description shown in help menu
 */
void registerATCommand(const String& cmd, const ATCommandHandler& handler, const String& help);

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
void registerShellCommand(const String& cmd, const ShellCommandHandler& handler, const String& help);

/**
 * @brief Get help string for all registered commands.
 * 
 * @return Formatted help string listing all commands
 */
String getATHelp();

/**
 * @brief Get help string for all registered shell commands.
 * 
 * @return Formatted help string listing all shell commands
 */
String getShellHelp();

/**
 * @brief Output a message in log mode (if enabled).
 * 
 * This is typically used by the application to send debug/info messages
 * when log mode is active.
 * 
 * @param msg Message to output
 */
void log(const String& msg);

// ----------------------------
// Internal Tool Functions (optional to expose)
// ----------------------------

/**
 * @brief Trim whitespace from both ends of a string.
 * 
 * @param str Input string
 * @return Trimmed copy of the string
 */
String trim(const String& str);

// ----------------------------
// AT Command Processing Functions
// ----------------------------

/**
 * @brief Process a complete AT command line.
 * 
 * This function parses and executes the given command.
 * It handles both built-in and user-registered commands.
 * 
 * @param fullCmd The full command line (without newline)
 */
void processATCommand(const String& fullCmd);

/**
 * @brief Main loop handler for AT command processing.
 * 
 * Call this function repeatedly in the main loop to handle incoming serial data.
 * It reads characters from atSerial and buffers them until a newline is received.
 */
void handleATCommands();

/**
 * @brief Register a callback function to be called when AT+RESTORE is received.
 * 
 * This allows the user to define custom behavior for factory reset,
 * such as clearing EEPROM, resetting settings, etc.
 * 
 * @param callback Function to call when AT+RESTORE is executed
 */
void onRestore(std::function<void()> callback);

/**
 * @brief Register a callback function to be called when shutdown is received.
 * 
 * @param callback Function to call when shutdown is executed
 */
void onShutdown(std::function<void()> callback);

#endif // AT_COMMANDS_H