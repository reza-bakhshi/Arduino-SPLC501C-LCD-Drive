
#include <Arduino.h>
#include "display.h"

int a = 0;

const char *intToString(int value)
{
  static char buffer[20];
  sprintf(buffer, "%d    ", value);
  return buffer;
}

void setup()
{
  display_Pin_init();
  display_init();
  Clear_iconbar();
  display_clear();
  display_update();

  for (int i = 0; i < 6; i++)
  {
    display_iconbar_signal(i);
    delay(400);
  }

  display_iconbar_bell(1);
  delay(400);

  display_iconbar_message(1);
  delay(400);

  for (int i = 0; i < 4; i++)
  {
    display_iconbar_network(i);
    delay(400);
  }

  for (int i = 0; i < 4; i++)
  {
    display_iconbar_battery(i);
    delay(400);
  }

  for (int i = 0; i < 15; i++)
  {
    display_iconbar_clock(i, 0, i % 2);
    delay(200);
  }

  for (int i = 0; i < 10; i++)
  {
    display_iconbar_clock(15, i * 11, i % 2);
    delay(200);
  }

  display_print(DISPLAY_WIDTH_PIXEL / 4, 1, "REZA-BAKHSHI");
  display_update();
  delay(200);

  display_print(0, 20, "1234567890 abcdefghij");
  display_update();
  delay(200);

  display_print(0, 30, "!@#$%^&*()_+=-[]{}<>?");
  display_update();
  delay(200);
}

void loop()
{
  if (a > 100)
    a = 0;

  const char *str = intToString(a++);
  display_print(DISPLAY_WIDTH_PIXEL / 2, 50, str);
  display_update();
  delay(200);
}
