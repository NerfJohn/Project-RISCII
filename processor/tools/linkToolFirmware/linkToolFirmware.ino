/*
 * linkToolFirmware.ino: Implements "J-link" for Project RISCII processor.
 * 
 * Turns Arduino Nano 33 BLE into a "J-Link"/USB-to-JTAG adapter for the
 * Project RISCII MCU JTAG port. Allows for bi-directional PC/MCU communication
 * (with PC acting as master).
 *
 * == JTAG Behavior ==
 * Four pins on the nano are configured to interface with the MCU's JTAG port at
 * a speed configurable by the PC. At the time of this writing, the pins assume
 * a 6-state JTAG state machine, where TCK idles low while TMS idles high.
 *
 * == USB Behavior ==
 * The USB connection is configured to perform "call/response" communication at
 * 9600 baud. Input messages include a command, payload, and "exe cmd" byte
 * (plus some escape chars), while output messages include a result and payload.
 *
 * For input messages, some bytes have special meaning. 0x0A is used to end
 * input messages, while 0xFF is used to escape any byte (ie allows 0x0A to be
 * used in the payload). Input messages shall NOT exceed 300 bytes.
 *
 * For output messages, their size is based on the interpreted size of the input
 * message (where escape symbols are removed and the message is limited to 300
 * bytes). The first byte indicates success (0x01) or failure (0x00).
 *
 * == Input Message Examples ==
 * raw input message   | interpreted input message | possible output message
 * --------------------+---------------------------+------------------------
 * 0x30 0x12 0x0A      | 0x30 0x12 0x0A            | 0x01 0x34 0x0A
 * 0x30 0xFF 0xAB 0x0A | 0x30 0xAB 0x0A            | 0x01 0xCD 0x0A
 * 0x30 0xFF 0x0A 0x0A | 0x30 0x0A 0x0A            | 0x01 0x55 0x0A
 * 0x30 0xFF 0xFF 0x0A | 0x30 0xFF 0x0A            | 0x01 0xEE 0x0A
 * 0x30 <9 bytes> 0x0A | 0x30 <9 bytes> 0x0A       | 0x01 <9 bytes> 0x0A
 * 
 * == Available Commands ==
 * command name      | input message syntax   | output message syntax
 * ------------------+------------------------+----------------------
 * set JTAG period   | 0x30 <new period> 0x0A | <result> <old period> 0x0A
 * shift instruction | 0x31 <shift in> 0x0A   | <result> <shifted out> 0x0A
 * shift data        | 0x32 <shift in> 0x0A   | <result> <shifted out> 0x0A
 */


#include <Serial.h>
#include <stdbool.h>
#include <stdint.h>

// -----------------------------------------------------------------------------

// Definitions mapping MCU pins to JTAG pins/functionality.
#define PIN_TCK 2
#define PIN_TDI 3
#define PIN_TDO 4
#define PIN_TMS 5

// Definitions of notable chars from UART/USB.
#define CHAR_CMD_PERIOD 0x30
#define CHAR_CMD_INSTR  0x31
#define CHAR_CMD_DATA   0x32
#define CHAR_ESC_CHAR   0xFF
#define CHAR_EXE_CMD    0x0A

// Definitions of notables chars to UART/USB.
#define RESP_GOOD (byte)(1)
#define RESP_FAIL (byte)(0)

// Definitions configuring the J-Link program's data model.
#define CFG_BUF_SIZE    300
#define CFG_CMD_IDX     0
#define CFG_DATA_IDX    1
#define CFG_INIT_PERIOD 2
#define CFG_BAUD_RATE   9600

// Definitions for "magic numbers" used in program.
#define BITS_PER_BYTE 8
#define SHIFT_IN_LSB  1
#define MASK_LOW_BYTE 0xFF
#define LOGIC_LOW     0x0
#define LOGIC_HIGH    0x1

// -----------------------------------------------------------------------------

/*
 * Data Model for "J-Link" program.
 *
 * Tracks main pieces of data at the high level: input/output data, meta-data
 * regarding passed in data, and settings for the JTAG connection. Acts like
 * general "state" of the "J-Link".
 */
typedef struct {
  // Data buffer- used to store incoming/outgoing data.
  uint8_t  m_buffer[CFG_BUF_SIZE];
  uint16_t m_bufSize;

  // Meta-data used for parsing UART/USB input.
  bool m_charEscaped;

  // Configured speed (or rather delay) of JTAG.
  uint16_t m_period;
} DataModel_t;

// -----------------------------------------------------------------------------

// Data model instance- global due to setup()/loop() behavior.
DataModel_t s_model;

// -----------------------------------------------------------------------------

/*
 * Bit-bangs one JTAG period. Intended to be called back-to-back.
 *
 * Implements one cycle of JTAG connection (returning just after falling edge of
 * TCK). Note that 1) given period is integer divided to generate TCK and 2)
 * delay in-between back-to-back calls affects TCK's characteristics.
 *
 * @param tdiBit value to send over TDI line during cycle
 * @param tmsBit value to send over TMS line during cycle
 * @param clkPeriod timing use for TCK halves (within function/cycle only!)
 * @return value of TDO line- value is result of last function call
 */
uint8_t jtagCycle(uint8_t tdiBit, uint8_t tmsBit, uint16_t clkPeriod) {
  // Read output- previous falling edge should have triggered update.
  uint8_t retBit = digitalRead(PIN_TDO);

  // Set JTAG outputs (prior to cycling- ensure RX setup times are met).
  digitalWrite(PIN_TDI, (tdiBit) ? HIGH : LOW);
  digitalWrite(PIN_TMS, (tmsBit) ? HIGH : LOW);

  // Cycle TCK.
  delay(clkPeriod / 2);        // TCK low end
  digitalWrite(PIN_TCK, HIGH); // TCK rising edge
  delay(clkPeriod / 2);        // TCK high end
  digitalWrite(PIN_TCK, LOW);  // TCK falling edge

  // Return read TDO value.
  return retBit;
}

// -----------------------------------------------------------------------------

/*
 * Performs one byte of JTAG transfer w/ given last TMS value. Returns TDO byte.
 *
 * "Byte" version of jtagCycle(). Documentation on jtagCycle() still applies,
 * though now TMS line is held LOW through the entire byte (except for the
 * last TMS value, which is user specified). Intended for register I/O.
 *
 * @param dataByte byte of data to shift into/over the JTAG connection
 * @param finalTMS value to use for final TMS value shifted in/over JTAG
 * @param clkPeriod timing use for TCK halves (within function/cycle only!)
 * @return byte returned over TDO line while shifting dataByte over
 */
uint8_t jtagByte(uint8_t dataByte, bool finalTMS, uint16_t clkPeriod) {
  // Returned byte from write/read transfer.
  uint8_t retByte = 0;
  
  // Transfer one byte across JTAG connection (MSbit first).
  for (uint8_t i = 0; i < BITS_PER_BYTE; i++) {
    // Determine input bits for transfer.
    uint8_t tdiBit = (dataByte >> (BITS_PER_BYTE - 1));     // Send MSb
    uint8_t tmsBit = (i == (BITS_PER_BYTE - 1)) & finalTMS; // 0 or finalTMS

    // Conduct transfer.
    uint8_t tdoBit = jtagCycle(tdiBit, tmsBit, clkPeriod);

    // Adjust inputs/outputs.
    retByte = (retByte << SHIFT_IN_LSB) | tdoBit; // "Shifted in" MSb into LSb
    dataByte <<= 1;                               // "Shifted out" MSb enitrely
  }

  // Return compound TDOs.
  return retByte;
}

// -----------------------------------------------------------------------------

/*
 * Attempts to execute buffered data as a command. Returns based on success.
 * 
 * Implements the checking and execution of buffered data as a command. If the
 * buffered data does not implement a command, no action is taken. Data buffer
 * updated with return payload (nix return code) is command is executed.
 *
 * @param model (ptr to) data model of the "J-Link" program
 * return true is command is executed, false otherwise
 */
bool executeCmd(DataModel_t* model) {
  // Take no action if data is not actually a command.
  if (CFG_CMD_IDX >= model->m_bufSize) {return false;}

  /* Line reached- message sent over can be parsed/run as a command */
  
  // Run the defined command.
  switch (model->m_buffer[CFG_CMD_IDX]) {
    case CHAR_CMD_PERIOD: {
      // Save previous period.
      uint16_t prevPrd = model->m_period;

      // Get new period (use last 2 bytes- not counting 'exe cmd' char).
      uint16_t newPrd = 0;
      for (uint16_t i = CFG_DATA_IDX; i < (model->m_bufSize - 1); i++) {
        // Determine new period.
        newPrd = (newPrd << BITS_PER_BYTE) | model->m_buffer[i];

        // Zero-out on the way (makes setting to old period easier).
        model->m_buffer[i] = 0;
      }

      // Set new period.
      model->m_period = newPrd;

      // Save old period to data buffer (just lower bytes- already zeroed).
      model->m_buffer[model->m_bufSize - 3] = (prevPrd >> BITS_PER_BYTE);
      model->m_buffer[model->m_bufSize - 2] = prevPrd & MASK_LOW_BYTE;
      
      break;
    }
    case CHAR_CMD_INSTR: {
      // Get to instruction register (ie instruction shift state).
      jtagCycle(LOGIC_LOW, LOGIC_LOW, model->m_period);
      jtagCycle(LOGIC_LOW, LOGIC_LOW, model->m_period);

      // Shift in instruction bytes (and record output- "swap" bytes).
      for (uint16_t i = CFG_DATA_IDX; i < (model->m_bufSize - 1); i++) {
        model->m_buffer[i] = jtagByte(model->m_buffer[i],
                                      i == (model->m_bufSize - 2),
                                      model->m_period
                                     );
      }

      // Return to JTAG's IDLE state.
      jtagCycle(LOGIC_LOW, LOGIC_HIGH, model->m_period);
      
      break;
    }
    case CHAR_CMD_DATA: {
      // Get to data register (ie data shift state).
      jtagCycle(LOGIC_LOW, LOGIC_LOW, model->m_period);
      jtagCycle(LOGIC_LOW, LOGIC_HIGH, model->m_period);
      jtagCycle(LOGIC_LOW, LOGIC_LOW, model->m_period);

      // Shift in data bytes (and record output- "swap" bytes).
      for (uint16_t i = CFG_DATA_IDX; i < (model->m_bufSize - 1); i++) {
        model->m_buffer[i] = jtagByte(model->m_buffer[i],
                                      i == (model->m_bufSize - 2),
                                      model->m_period
                                     );
      }

      // Data shift breaks out to IDLE by default //

      break;
    }
    default:
      // Invalid command- failure...
      return false;
  }

  // Command executed- sucess!
  return true;
}

// -----------------------------------------------------------------------------

/*
 * Initialize "J-Link" resources (called once upon reset).
 *
 * Initializes "J-Link" resources (namely pins, serial, and data model). Should
 * Only be called once per reset of the device. For posterity, pins are assigned
 * explicit default values (via ouput value or pullup resistors).
 */
void setup() {
  // Setup pin directions.
  pinMode(PIN_TCK, OUTPUT);
  pinMode(PIN_TDI, OUTPUT);
  pinMode(PIN_TMS, OUTPUT);
  pinMode(PIN_TDO, INPUT);

  // Initialize starting output pin values.
  digitalWrite(PIN_TCK, LOW);
  digitalWrite(PIN_TDI, LOW);
  digitalWrite(PIN_TMS, HIGH);

  // (Set inputs as pull-ups).
  digitalWrite(PIN_TDO, HIGH);

  // Setup program data.
  s_model.m_bufSize = 0;
  s_model.m_charEscaped = false;
  s_model.m_period = CFG_INIT_PERIOD;

  // Init serial connection.
  Serial.begin(CFG_BAUD_RATE);
  while(!Serial) {};
}

// -----------------------------------------------------------------------------

/*
 * Main program/loop. Parses and executes commands, returning results/data.
 *
 * Responsible for parsing input (factoring for escaped characters) and sending
 * output data once command is executed (setting return code based on result of
 * command parsing/execution). Function should be called in a loop endlessly.
 */
void loop() {
  // Get next byte over serial.
  while (!Serial.available()) {/* spin */}
  uint8_t newByte = Serial.read();
  
  // Handle byte- data byte to append vs. escape char to note and ignore.
  if ((newByte == CHAR_ESC_CHAR) && !s_model.m_charEscaped) {
    // Escape char- noted and ignored (by data buffer, that is).
    s_model.m_charEscaped = true;
    return;                       // effectively "continue" for loop() function
  }
  else {
    // Data byte- append to data buffer until full (ie "truncate command")
    if (s_model.m_bufSize < CFG_BUF_SIZE) {
      s_model.m_buffer[s_model.m_bufSize] = newByte;
      s_model.m_bufSize++;
    }
  }
  
  /* Line reached- byte is NOT escaping another char */
  
  // Execute command if (unescaped) 'execute' char is seen.
  if ((newByte == CHAR_EXE_CMD) && !s_model.m_charEscaped) {
    // Ensure last byte is "exe cmd" byte (ie consistent end, even on overflow).
    s_model.m_buffer[s_model.m_bufSize] = CHAR_EXE_CMD;
    
    // Attempt to execute command.
    bool goodExe = executeCmd(&s_model);

    // Report status/data from execution.
    Serial.write((goodExe) ? RESP_GOOD : RESP_FAIL);
    for (uint16_t i = CFG_DATA_IDX; i < s_model.m_bufSize; i++) {
      Serial.write(s_model.m_buffer[i]);
    }

    // Reset buffer.
    s_model.m_bufSize = 0;
  }
  
  // Data from escape char (ie escap-ed char) handled- deassert.
  s_model.m_charEscaped = false;
}
