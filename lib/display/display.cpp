#include "display.h"
#include "font/font_5x8.h"

uint8_t buffer[BUFFER_SIZE];

typedef struct
{
    uint8_t on[7];
    uint8_t off[7];
    uint8_t on_size;
    uint8_t off_size;
} Segment;

/**
 * Sends a byte of data to the display.
 * @param data The data byte to be sent to the display.
 */
void display_data(uint8_t data)
{
    SET_PIN(A0_PIN);
    CLEAR_PIN(CS_PIN);
    CLEAR_PIN(WR_PIN);
    digitalWrite(D0_PIN, (data >> 0) & 0x01);
    digitalWrite(D1_PIN, (data >> 1) & 0x01);
    digitalWrite(D2_PIN, (data >> 2) & 0x01);
    digitalWrite(D3_PIN, (data >> 3) & 0x01);
    digitalWrite(D4_PIN, (data >> 4) & 0x01);
    digitalWrite(D5_PIN, (data >> 5) & 0x01);
    digitalWrite(D6_PIN, (data >> 6) & 0x01);
    digitalWrite(D7_PIN, (data >> 7) & 0x01);
    SET_PIN(WR_PIN);
    SET_PIN(CS_PIN);
}

/**
 * Sends a command byte to the display.
 * @param command The command byte to be sent to the display.
 */
void display_cmd(uint8_t command)
{
    CLEAR_PIN(CS_PIN);
    CLEAR_PIN(A0_PIN);
    SET_PIN(RD_PIN);
    CLEAR_PIN(WR_PIN);
    digitalWrite(D0_PIN, (command >> 0) & 0x01);
    digitalWrite(D1_PIN, (command >> 1) & 0x01);
    digitalWrite(D2_PIN, (command >> 2) & 0x01);
    digitalWrite(D3_PIN, (command >> 3) & 0x01);
    digitalWrite(D4_PIN, (command >> 4) & 0x01);
    digitalWrite(D5_PIN, (command >> 5) & 0x01);
    digitalWrite(D6_PIN, (command >> 6) & 0x01);
    digitalWrite(D7_PIN, (command >> 7) & 0x01);
    SET_PIN(WR_PIN);
    SET_PIN(A0_PIN);
    SET_PIN(CS_PIN);
}

/**
 * Initializes the display by sending a series of configuration commands.
 */
void display_init(void)
{
    SET_PIN(WR_PIN);
    SET_PIN(A0_PIN);
    SET_PIN(CS_PIN);
    SET_PIN(RD_PIN);
    CLEAR_PIN(RST_PIN);
    delay(1);
    SET_PIN(RST_PIN);
    delay(1);
    display_cmd(CMD_RESET);
    display_cmd(CMD_COM_SCAN_DIRECTION_NORMAL);
    display_cmd(CMD_LCD_BIAS_19);
    display_cmd(CMD_POWER_CONTROLLER_ALL_ON);
    display_cmd(CMD_DISPLAY_NORMAL);
    display_cmd(CMD_VOLTAGE_REGULATOR_INTERNAL_RESISTOR_LARGE);
    display_cmd(CMD_ELECTRONIC_VOLUME_REGISTER);
    display_cmd(CMD_REFERENCE_VOLTAGE);
    display_cmd(CMD_DISPLAY_ON);
    display_cmd(CMD_DISPLAY_ALL_POINTS_OFF);
    delay(1);
}

/**
 * Initializes the control and data pins for the display.
 */
void display_Pin_init()
{
    // control pins
    pinMode(RST_PIN, OUTPUT);
    pinMode(CS_PIN, OUTPUT);
    pinMode(A0_PIN, OUTPUT);
    pinMode(WR_PIN, OUTPUT);
    pinMode(RD_PIN, OUTPUT);

    // data pins
    pinMode(D0_PIN, OUTPUT);
    pinMode(D1_PIN, OUTPUT);
    pinMode(D2_PIN, OUTPUT);
    pinMode(D3_PIN, OUTPUT);
    pinMode(D4_PIN, OUTPUT);
    pinMode(D5_PIN, OUTPUT);
    pinMode(D6_PIN, OUTPUT);
    pinMode(D7_PIN, OUTPUT);
}

/**
 * Updates the display with the current pixel data stored in the buffer.
 */
void display_update()
{
    for (int row = 0; row < DISPLAY_HEIGHT_ROWS; row++)
    {
        display_cmd(CMD_ROW_ADDRESS_BASE + row);
        display_cmd(CMD_COLUMN_ADDRESS_LOWER_BASE);
        display_cmd(CMD_COLUMN_ADDRESS_UPPER_BASE);
        for (int col = 0; col < DISPLAY_WIDTH_PIXEL; col++)
        {
            int index = row * DISPLAY_WIDTH_PIXEL + col;
            display_data(buffer[index]);
        }
    }
}

/**
 * Clears the display buffer, setting all pixels to off.
 */
void display_clear()
{
    memset(buffer, 0, sizeof(buffer));
}

/**
 * Clears the icon bar by turning off all pixels in the last row.
 */
void Clear_iconbar()
{
    uint8_t x = 0;

    for (x = 0; x < DISPLAY_WIDTH_PIXEL; x++)
    {
        display_cmd(CMD_ROW_ADDRESS_BASE | 0x08); // 0xD8 -> last row address
        display_cmd(CMD_COLUMN_ADDRESS_LOWER_BASE | (x & 0x0F));
        display_cmd(CMD_COLUMN_ADDRESS_UPPER_BASE | (x >> 4));
        display_data(0x00);
    }
}

/**
 * Updates the icon bar with the specified icon state.
 * @param icon The icon to update.
 * @param state The state of the icon (on/off).
 */
static inline void _iconbar(uint8_t icon, uint8_t state)
{
    display_cmd(CMD_ROW_ADDRESS_BASE | 0x08); // 0xD8 -> last row address
    display_cmd(CMD_COLUMN_ADDRESS_LOWER_BASE | (icon & 0x0F));
    display_cmd(CMD_COLUMN_ADDRESS_UPPER_BASE | (icon >> 4));
    display_data(state);
}

/**
 * Sets the pixel at the specified coordinates to the given status.
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 * @param pixel_status The status to set the pixel (on/off).
 */
static inline void _display_pixel(int x, int y, bool pixel_status)
{
    int byte_index = 0;
    uint8_t mask = 0;

    if (x >= 0 && x < DISPLAY_WIDTH_PIXEL && y >= 0 && y < DISPLAY_HEIGHT_PIXEL)
    {
        byte_index = (y / 8) * DISPLAY_WIDTH_PIXEL + x;
        mask = 1 << (y % 8);

        if (pixel_status)
            buffer[byte_index] |= mask;
        else
            buffer[byte_index] &= ~mask;
    }
}

/**
 * Prints a string of characters at the specified coordinates on the display.
 * @param X The x-coordinate to start printing (0-132).
 * @param Y The y-coordinate to start printing (0-64).
 * @param str The string to print.
 */
void display_print(int X, int Y, const char *str)
{
    uint8_t column_data = 0;
    int row = 0;

    while (*str)
    {
        if (*str >= 32 && *str <= 127) // ascii characters
        {
            for (int col = 0; col < FONT_5x8_WIDTH; col++)
            {
                column_data = (*(const uint8_t *)(&(font_5x8[*str - 32][col])));
                for (row = 0; row < FONT_5x8_HEIGHT; row++)
                    _display_pixel(X, Y + row, !!(column_data & (1 << row)));

                X++;
            }
            if (*str != ' ')
                X++;
        }
        str++;
    }
}

/**
 * Updates the signal strength icons in the icon bar.
 * @param level The signal strength level (0-5).
 */
void display_iconbar_signal(uint8_t level)
{
    if (level > 5)
        level = 5;

    for (uint8_t i = 0; i < 5; i++)
        _iconbar(i + ICON_SIGNAL, (i < level) ? 1 : 0);
}

/**
 * Toggles the bell icon in the icon bar.
 * @param state The state of the bell icon (0: off, 1: on).
 */
void display_iconbar_bell(uint8_t state)
{
    _iconbar(ICON_BELL, state);
}

/**
 * Toggles the message icon in the icon bar.
 * @param state The state of the message icon (0: off, 1: on).
 */
void display_iconbar_message(uint8_t state)
{
    _iconbar(ICON_MESSAGE, state);
}

/**
 * Updates the network status icons in the icon bar.
 * @param state The state of the network icons (0: off, 1: data receive, 2: data send, 3: send and receive data).
 */
void display_iconbar_network(uint8_t state)
{
    if (state > 3)
        state = 3;

    _iconbar(ICON_DATA_RECEIVE, (state == 1 | state == 3) ? 1 : 0);
    _iconbar(ICON_DATA_SEND, (state == 2 | state == 3) ? 1 : 0);
}

/**
 * Updates the battery status icons in the icon bar.
 * @param level The battery level (0: empty, 1: 10-30%, 2: 30-70%, 3: 70-100%).
 */
void display_iconbar_battery(uint8_t level)
{
    if (level > 3)
        level = 3;

    _iconbar(ICON_BATTERY, (level >= 0) ? 1 : 0);
    _iconbar(ICON_BATTERY + 1, (level >= 1) ? 1 : 0);
    _iconbar(ICON_BATTERY + 3, (level >= 2) ? 1 : 0);
    _iconbar(ICON_BATTERY + 2, (level >= 3) ? 1 : 0);
}

static inline void display_segments(Segment segment, uint8_t segment_number)
{
    uint8_t offset[4] = {0, 7, 15, 22}; // segment digit offset
    if (segment_number > 4)
        segment_number = 4;
    if (segment_number < 1)
        segment_number = 1;

    for (uint8_t i = 0; i < segment.on_size; i++)
        _iconbar(segment.on[i] + offset[segment_number - 1], 1);

    for (uint8_t i = 0; i < segment.off_size; i++)
        _iconbar(segment.off[i] + offset[segment_number - 1], 0);
}

/**
 * Displays the clock in the icon bar using two segments and a separator.
 * @param segment_1 The first segment value (0-99).
 * @param segment_2 The second segment value (0-99).
 * @param separator The state of the separator (0: off, 1: on).
 */
void display_iconbar_clock(uint8_t segment_1, uint8_t segment_2, uint8_t separator)
{
    Segment segments[10] = {
        {{SEG_A, SEG_F, SEG_E, SEG_D, SEG_C, SEG_B}, {SEG_G}, 6, 1},
        {{SEG_C, SEG_B}, {SEG_A, SEG_F, SEG_G, SEG_E, SEG_D}, 2, 5},
        {{SEG_A, SEG_G, SEG_E, SEG_D, SEG_B}, {SEG_F, SEG_C}, 5, 2},
        {{SEG_A, SEG_G, SEG_D, SEG_C, SEG_B}, {SEG_F, SEG_E}, 5, 2},
        {{SEG_F, SEG_G, SEG_C, SEG_B}, {SEG_A, SEG_E, SEG_D}, 4, 3},
        {{SEG_A, SEG_F, SEG_G, SEG_D, SEG_C}, {SEG_E, SEG_B}, 5, 2},
        {{SEG_A, SEG_F, SEG_G, SEG_E, SEG_D, SEG_C}, {SEG_B}, 6, 1},
        {{SEG_A, SEG_C, SEG_B}, {SEG_F, SEG_G, SEG_E, SEG_D}, 3, 4},
        {{SEG_A, SEG_F, SEG_G, SEG_E, SEG_D, SEG_C, SEG_B}, {}, 7, 0},
        {{SEG_A, SEG_F, SEG_G, SEG_D, SEG_C, SEG_B}, {SEG_E}, 6, 1}};

    uint8_t segment_1_tens = segment_1 / 10;
    uint8_t segment_1_ones = segment_1 % 10;
    uint8_t segment_2_tens = segment_2 / 10;
    uint8_t segment_2_ones = segment_2 % 10;

    display_segments(segments[segment_1_tens], 1);
    display_segments(segments[segment_1_ones], 2);
    display_segments(segments[segment_2_tens], 3);
    display_segments(segments[segment_2_ones], 4);
    _iconbar(SEG_SEPARATOR, separator);
}