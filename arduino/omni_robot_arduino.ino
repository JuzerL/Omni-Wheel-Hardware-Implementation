#include "motors.h"

/*
  ===========================
  CONFIG
  ===========================
*/
#define BAUD_RATE 115200

#define CMD_TIMEOUT 500    // ms (watchdog)
#define FEEDBACK_INTERVAL 50 // ms

/*
  ===========================
  GLOBAL VARIABLES
  ===========================
*/
String input_buffer = "";

int w1 = 0, w2 = 0, w3 = 0, w4 = 0;

unsigned long last_cmd_time = 0;
unsigned long last_feedback_time = 0;


/*
  ===========================
  SETUP
  ===========================
*/
void setup()
{
  Serial.begin(BAUD_RATE);

  init_motors();

  last_cmd_time = millis();
  last_feedback_time = millis();
}


/*
  ===========================
  PARSE COMMAND STRING
  Format: $W1,W2,W3,W4\n
  ===========================
*/
void parse_command(String cmd)
{
  // Remove starting '$'
  cmd.remove(0, 1);

  int values[4];
  int index = 0;

  char *token;
  char buffer[50];
  cmd.toCharArray(buffer, sizeof(buffer));

  token = strtok(buffer, ",");

  while (token != NULL && index < 4)
  {
    values[index++] = atoi(token);
    token = strtok(NULL, ",");
  }

  if (index == 4)
  {
    w1 = values[0];
    w2 = values[1];
    w3 = values[2];
    w4 = values[3];

    write_motors(w1, w2, w3, w4);
    last_cmd_time = millis();
  }
}


/*
  ===========================
  SERIAL READ (NON-BLOCKING)
  ===========================
*/
void read_serial()
{
  while (Serial.available())
  {
    char c = Serial.read();

    // End of message
    if (c == '\n')
    {
      if (input_buffer.length() > 0 && input_buffer[0] == '$')
      {
        parse_command(input_buffer);
      }

      input_buffer = ""; // reset buffer
    }
    else
    {
      input_buffer += c;

      // Prevent overflow
      if (input_buffer.length() > 50)
      {
        input_buffer = "";
      }
    }
  }
}


/*
  ===========================
  WATCHDOG SAFETY
  ===========================
*/
void watchdog_check()
{
  if (millis() - last_cmd_time > CMD_TIMEOUT)
  {
    // Stop robot immediately
    write_motors(0, 0, 0, 0);
  }
}


/*
  ===========================
  FEEDBACK (ENCODER PLACEHOLDER)
  Format: #E1,E2,E3,E4\n
  ===========================
*/
void send_feedback()
{
  if (millis() - last_feedback_time >= FEEDBACK_INTERVAL)
  {
    last_feedback_time = millis();

    // Placeholder encoder values
    int e1 = 0;
    int e2 = 0;
    int e3 = 0;
    int e4 = 0;

    Serial.print("#");
    Serial.print(e1);
    Serial.print(",");
    Serial.print(e2);
    Serial.print(",");
    Serial.print(e3);
    Serial.print(",");
    Serial.print(e4);
    Serial.print("\n");
  }
}


/*
  ===========================
  MAIN LOOP
  ===========================
*/
void loop()
{
  read_serial();     // Handle incoming commands
  watchdog_check();  // Safety stop if no command
  send_feedback();   // Send encoder data
}
