/*
 * Link Tool Firmware: "J-Link" application for Project RISCII
 * 
 * Application that mimics a "J-Link" by acting as an adapter from a Host PC
 * (using UART/USB) to RISCII's JTAG. Includes configurable JTAG speed and
 * convenient return codes/data.
 * 
 * Hardware is simply an arduino nano (ble) with 4 wires for JTAG connection
 * and USB cable for USB connection.
 */

#include <Serial.h>
#include <stdbool.h>
#include <stdint.h>

// -------------------------------------------------------------------------- //

// Definitions mapping MCU pins to JTAG pins/functionality.
#define PIN_TCK 2
#define PIN_TDI 3
#define PIN_TMS 4
#define PIN_TDO 5

// Definitions for available/possible commands from UART/USB.
#define CMD_SET_PERIOD 0x30
#define CMD_SEND_INSTR 0x31
#define CMD_SEND_DATA  0x32
#define CMD_INVALID    0x00

// Definitions for validating/deciphering UART/USB commands.
#define CMD_SIZE_MIN  3
#define CMD_SIZE_MAX  4
#define CMD_TYPE_IDX  0
#define CMD_DATA_IDX  1

// Definitions for "status" part of sent UART/USB responses.
#define RESP_GOOD (byte)1
#define RESP_FAIL (byte)0

// Definitions for (starting) parameters of application's data model.
#define JTAG_PERIOD_MS 2
#define MSG_BUF_SIZE   4
#define SERIAL_RATE    9600

// Definitions for "magic numbers" in program (generally NOT configurable).
#define MASK_BIT0     0x01
#define LAST_BIT_IDX  7
#define SHIFT_IN_MSB  1
#define SHIFT_BYTE 8
#define NEWLINE_CHAR  '\n'
#define LOGIC_HIGH    0x1
#define LOGIC_LOW     0x0
#define BYTE_NUM_BITS 8

// -------------------------------------------------------------------------- //

// Data model type to hold data pertinent to the application.
typedef struct {
  // Message buffer (and its metadata).
  uint8_t   m_msgBuffer[MSG_BUF_SIZE];    // physical message buffer
  uint8_t   m_bufferIdx;                  // next open index
  uint8_t   m_bufferSize;                 // number of used buffer slots

  // Parsed command tokens.
  uint8_t   m_cmdType;                    // type of command parsed
  uint16_t  m_cmdData;                    // data given with command

  // Mutable JTAG connection params.
  uint16_t  m_jtagPeriod;                 // period of TCK (when active)
} DataModel_t;

// -------------------------------------------------------------------------- //

// Data model of the application- global because of setup()/loop() behavior.
DataModel_t s_prgmData;

// -------------------------------------------------------------------------- //

/*
 * Cycles JTAG connection with given inputs/period. Returns result of TDO bit
 * 
 * Function is intended to be called "back-to-back" to create expected JTAG
 * clocking/signals/etc. Signals are effectively changed on falling edge of
 * TCK (good for TDI/TMS setup timing, though poor for TDO setup timing).
 * 
 * @param tdiBit bit value to use for TDI signal on TCK rising edge
 * @param tmsBit bit value to use for TMS signal on TCK rising edge
 * @param clkPeriod TCK period to emulate- divided into two delays
 * @return TDO bit PRIOR to TCK rising edge
 */
uint8_t jtagCycle(uint8_t tdiBit, uint8_t tmsBit, uint16_t clkPeriod) {
  // Read output immediately- get before TCK rising edge.
  uint8_t retBit = digitalRead(PIN_TDO);

  // Set JTAG outputs (prior to cycling- ensure RX setup times are met).
  digitalWrite(PIN_TDI, (tdiBit) ? HIGH : LOW);
  digitalWrite(PIN_TMS, (tmsBit) ? HIGH : LOW);

  // Cycle TCK.
  delay(clkPeriod / 2);
  digitalWrite(PIN_TCK, HIGH);
  delay(clkPeriod / 2);        // TCK delay + falling edge + application delay
  digitalWrite(PIN_TCK, LOW);  // should ease TDO setup requirements

  // Return read TDO value.
  return retBit;
}

// -------------------------------------------------------------------------- //

/*
 * Performs 1 byte transfer over JTAG. Returns TDO output byte.
 * 
 * Function can be used to transfer multiple bytes (as TDI) over JTAG. An
 * additional argument is available to control final value of TMS (ie if JTAG
 * should be directed to next state after byte transfer).
 * 
 * @param dataByte byte to transfer over JTAG (as TDI)
 * @param toUpdate controls final TMS value sent over JTAG
 * @param clkPeriod TCK period to emulate- divided into two delays
 * @return TDO outputs from JTAG transfer
 */
uint8_t jtagByte(uint8_t dataByte, bool toUpdate, uint16_t clkPeriod) {
  // Transfer one byte across JTAG connection.
  uint8_t retByte = 0; // record TDO output
  for (uint8_t i = 0; i < BYTE_NUM_BITS; i++) {
    // Determine input bits for transfer.
    uint8_t tdiBit = dataByte & MASK_BIT0;
    uint8_t tmsBit = (i == LAST_BIT_IDX) & toUpdate;

    // Conduct transfer.
    uint8_t tdoBit = jtagCycle(tdiBit, tmsBit, clkPeriod);

    // Adjust inputs/outputs.
    retByte = (tdoBit << LAST_BIT_IDX) | (retByte >> SHIFT_IN_MSB);
    dataByte >>= SHIFT_IN_MSB; // shift in MSb == shift out LSb
  }

  // Return compound TDOs.
  return retByte;
}

// -------------------------------------------------------------------------- //

/*
 * Parses model's message into model's command variables. Clears message buffer.
 * 
 * Parsing encompassed little more than moving value around than checking
 * validity of values. However, message buffer values are edited as needed
 * to clear the buffer.
 * 
 * @param model data model of the application
 */
void parseCommand(DataModel_t* model) {
  // Ensure command is correct size.
  if ((model->m_bufferSize < CMD_SIZE_MIN) || 
      (model->m_bufferSize > CMD_SIZE_MAX)) {
    // Invalid-sized command.
    model->m_cmdType = CMD_INVALID;
    return;
  }

  // -- Command properly sized- can be "parsed" -- //

  // Extract command type (may/may not be valid).
  model->m_cmdType = model->m_msgBuffer[CMD_TYPE_IDX];

  // Extract command data/value.
  uint8_t idx = CMD_DATA_IDX; // start at beginning of command's data
  model->m_cmdData = 0;       // start with "reset" data value
  while (model->m_msgBuffer[idx] != NEWLINE_CHAR) {
    model->m_cmdData <<= SHIFT_BYTE;
    model->m_cmdData += model->m_msgBuffer[idx];
    idx++;
  }

  // "Clear" message buffer.
  model->m_bufferIdx = 0;
  model->m_bufferSize = 0;

  // Message parsed into command fields- job well done.
  return;
}

// -------------------------------------------------------------------------- //

/*
 * Initialization rountine. Sets up JTAG/USB connections and starting data.
 */
void setup() {
  // Setup pin directions.
  pinMode(PIN_TCK, OUTPUT);
  pinMode(PIN_TDI, OUTPUT);
  pinMode(PIN_TMS, OUTPUT);
  pinMode(PIN_TDO, INPUT);

  // (Set inputs as pull-ups).
  digitalWrite(PIN_TDO, HIGH);

  // Init serial connection.
  Serial.begin(SERIAL_RATE);
  while(!Serial) {};

  // Setup program data.
  s_prgmData.m_bufferIdx  = 0;
  s_prgmData.m_bufferSize = 0;
  s_prgmData.m_cmdType    = CMD_INVALID;
  s_prgmData.m_cmdData    = 0;
  s_prgmData.m_jtagPeriod = JTAG_PERIOD_MS;
}

// -------------------------------------------------------------------------- //

/*
 * Main loop of application. Attempts to execute/respond to UART commands.
 */
void loop() {
  // Monitor Serial input data.
  if (Serial.available()) {
    // Append input to buffer.
    uint8_t newByte = Serial.read();
    s_prgmData.m_msgBuffer[s_prgmData.m_bufferIdx] = newByte;

    // Adjust buffer records.
    if (s_prgmData.m_bufferIdx < (MSG_BUF_SIZE + 1)) {
      s_prgmData.m_bufferIdx++;
    }
    s_prgmData.m_bufferSize++;

    // Attempt parse/JTAG transaction if message is ended.
    if (newByte == NEWLINE_CHAR) {
      // Parse command- converting buffer into command variables.
      parseCommand(&s_prgmData);

      // Run procedure as appropirate.
      switch(s_prgmData.m_cmdType) {
        case CMD_SET_PERIOD: {
          // Set period, passing back previous period.
          uint16_t oldPeriod = s_prgmData.m_jtagPeriod;
          s_prgmData.m_jtagPeriod = s_prgmData.m_cmdData;
          Serial.write(RESP_GOOD);
          Serial.write((uint8_t)(oldPeriod >> SHIFT_BYTE));
          Serial.write((uint8_t)(oldPeriod));
          break;
        }
        case CMD_SEND_INSTR: {
          // Send instruction byte over JTAG, pass back byte received.
          jtagCycle(LOGIC_LOW, LOGIC_LOW, s_prgmData.m_jtagPeriod);
          jtagCycle(LOGIC_LOW, LOGIC_LOW, s_prgmData.m_jtagPeriod);
          uint8_t retByte = jtagByte((uint8_t)(s_prgmData.m_cmdData),
                                     true,
                                     s_prgmData.m_jtagPeriod
                                    );
          jtagCycle(LOGIC_LOW, LOGIC_HIGH, s_prgmData.m_jtagPeriod);
          Serial.write(RESP_GOOD);
          Serial.write(retByte);
          break;
        }
        case CMD_SEND_DATA: {
          // Send data word over JTAG, pass back word received.
          jtagCycle(LOGIC_LOW, LOGIC_LOW, s_prgmData.m_jtagPeriod);
          jtagCycle(LOGIC_LOW, LOGIC_HIGH, s_prgmData.m_jtagPeriod);
          jtagCycle(LOGIC_LOW, LOGIC_LOW, s_prgmData.m_jtagPeriod);
          uint8_t outByteLo = (uint8_t)(s_prgmData.m_cmdData);
          uint8_t outByteHi = (uint8_t)(s_prgmData.m_cmdData >> SHIFT_BYTE);
          uint8_t retByteLo = jtagByte(outByteLo,
                                       false,
                                       s_prgmData.m_jtagPeriod
                                      );
          uint8_t retByteHi = jtagByte(outByteHi, 
                                       true, 
                                       s_prgmData.m_jtagPeriod
                                      );
          jtagCycle(LOGIC_LOW, LOGIC_HIGH, s_prgmData.m_jtagPeriod);
          Serial.write(RESP_GOOD);
          Serial.write(retByteHi);
          Serial.write(retByteLo);
          break;
        }
        default: {
          // Invalid command- let UART/USB user know this.
          Serial.write(RESP_FAIL);
        }
      }
      Serial.write(NEWLINE_CHAR); // end any message with clear "end" delimiter
    }
  }
}
