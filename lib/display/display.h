#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <Arduino.h>

// Pin definitions for display control
enum PINS
{
    RST_PIN = 22,
    CS_PIN = 23,
    A0_PIN = 32,
    WR_PIN = 33,
    RD_PIN = 21,
    D0_PIN = 19,
    D1_PIN = 5,
    D2_PIN = 18,
    D3_PIN = 17,
    D4_PIN = 16,
    D5_PIN = 4,
    D6_PIN = 0,
    D7_PIN = 2
};

// Icon start addresses for the display
enum ICONS
{
    ICON_SIGNAL = 13,       // Signal strength icons (13-17)
    ICON_BELL = 31,         // Bell icon
    ICON_MESSAGE = 46,      // Message icon
    ICON_DATA_RECEIVE = 59, // Data receive icon
    ICON_DATA_SEND = 69,    // Data send icon
    ICON_BATTERY = 92       // Battery icons (92-95)
};

// Seven segment display addresses
enum ICON_SEGMENTS
{
    SEG_A = 96,         // Segment A
    SEG_B = 102,        // Segment B
    SEG_C = 101,        // Segment C
    SEG_D = 100,        // Segment D
    SEG_E = 99,         // Segment E
    SEG_F = 97,         // Segment F
    SEG_G = 98,         // Segment G
    SEG_SEPARATOR = 110 // Separator segment
};

// Command definitions for display control
#define CMD_COLUMN_ADDRESS_LOWER_BASE 0x00
#define CMD_COLUMN_ADDRESS_UPPER_BASE 0x10
#define CMD_REFERENCE_VOLTAGE 0x0E

// Voltage regulator command definitions
#define VOLTAGE_REGULATOR_INTERNAL_RESISTOR 0x20 // Base command for voltage regulator
#define CMD_VOLTAGE_REGULATOR_INTERNAL_RESISTOR_LARGE 0x27
#define POWER_CONTROLLER 0x28
#define CMD_POWER_BOOSTER_ON (POWER_CONTROLLER + 0x4)
#define CMD_POWER_VOLTAGE_REGULATOR_ON (POWER_CONTROLLER + 0x2)
#define CMD_POWER_VOLTAGE_FOLLOWER_CIRCUIT_ON (POWER_CONTROLLER + 0x1)
#define CMD_POWER_CONTROLLER_ALL_ON (CMD_POWER_BOOSTER_ON | CMD_POWER_VOLTAGE_REGULATOR_ON | CMD_POWER_VOLTAGE_FOLLOWER_CIRCUIT_ON)

#define CMD_DISPLAY_START_LINE_SET 0x40 // Command to set the start line for display
#define CMD_ELECTRONIC_VOLUME_REGISTER 0x81

// ADC select commands
#define CMD_ADC_SELECT_NORMAL 0xA0  // Normal SEG output correspondence
#define CMD_ADC_SELECT_REVERSE 0xA1 // Reversed SEG output correspondence

// LCD bias commands
#define CMD_LCD_BIAS_19 0xA2 // Bias ratio 1/9
#define CMD_LCD_BIAS_17 0xA3 // Bias ratio 1/7

// Display control commands
#define CMD_DISPLAY_ALL_POINTS_OFF 0xA4 // Turn off all points
#define CMD_DISPLAY_ALL_POINTS_ON 0xA5  // Turn on all points
#define CMD_DISPLAY_NORMAL 0xA6         // Set display to normal
#define CMD_DISPLAY_REVERSE 0xA7        // Set display to reverse
#define CMD_DISPLAY_OFF 0xAE            // Turn off the display
#define CMD_DISPLAY_ON 0xAF             // Turn on the display

// Command to set the display RAM start line address
#define CMD_ROW_ADDRESS_BASE 0xB0 // Base command for row address

// COM scan direction commands
#define CMD_COM_SCAN_DIRECTION_NORMAL 0xC8   // Normal scan direction (COM0 -> COM63)
#define CMD_COM_SCAN_DIRECTION_REVERSED 0xC9 // Reversed scan direction (COM63 -> COM0)

#define CMD_RESET 0xE2 // Internal reset command

// Display dimensions
#define DISPLAY_WIDTH_PIXEL 132
#define DISPLAY_HEIGHT_PIXEL 64
#define DISPLAY_HEIGHT_ROWS 8
#define BUFFER_SIZE (DISPLAY_WIDTH_PIXEL * DISPLAY_HEIGHT_ROWS)

// Macro definitions for pin control
#define SET_PIN(pin) digitalWrite(pin, HIGH)  // Set pin high
#define CLEAR_PIN(pin) digitalWrite(pin, LOW) // Set pin low

// Functions
void display_data(uint8_t data);
void display_cmd(uint8_t command);
void display_init(void);
void display_Pin_init(void);
void display_update(void);
void display_clear(void);
void Clear_iconbar();
void display_print(int X, int Y, const char *str);
void display_iconbar_signal(uint8_t level);
void display_iconbar_bell(uint8_t state);
void display_iconbar_message(uint8_t state);
void display_iconbar_network(uint8_t state);
void display_iconbar_battery(uint8_t level);
void display_iconbar_clock(uint8_t hour, uint8_t minute, uint8_t separator);

#endif