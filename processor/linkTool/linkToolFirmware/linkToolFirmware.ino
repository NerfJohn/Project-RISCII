/*
 * linkToolFirmware.ino: Implements "J-link" for Project RISCII processor.
 * 
 * JTAG Behavior:
 *  -> TCK idles at LOGIC_LOW, TMS idles at LOGIC_HIGH.
 *  -> Assumes Project RISCII's custom JTAG state machine (6-state JTAG).
 *  -> Rate of TCK is configurable by UART/USB.
 *  
 * UART/USB Behavior:
 *  -> Takes in commands to set period or send instruction/data sequences.
 *  -> Sends return message of same size to report success and read data.
 *    (0x00 = Failure, 0x01 = Good)
 *  -> Uses escape char (0xFF) to properly detect end-of-command char (0x0A).
 *    (Byte equal to 0xFF or 0x0A should prefix 0xFF before it)
 *    (Escape chars only for input- output sized to input to avoid escape chars)
 * 
 * Available Commands:
 *  -> Set Period: 0x30 <new period bytes> 0x0A
 *    (Returns: <result> <old period bytes> 0x0A)
 *    (Period saved as uint16_t- last 2 period bytes sets value)
 *  -> Send Instruction Sequence: 0x31 <instruction sequence> 0x0A
 *    (Returns: <result> <shifted out value> 0x0A)
 *    (Instruction is 8-bit register- according to Project RISCII JTAG)
 *  -> Send Data Sequence: 0x31 <data sequence> 0x0A
 *    (Returns: <result> <shifted out data> 0x0A)
 *    (Data is 16-bit register- according to Project RISCII JTAG)
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

// -------------------------------------------------------------------------- //

// Data-type to hold J-Link program's main data while running.
typedef struct {
  // Data buffer- used to store incoming/outgoing data.
  uint8_t  m_buffer[CFG_BUF_SIZE];
  uint16_t m_bufSize;

  // Meta-data used for parsing UART/USB input.
  bool m_charEscaped;

  // Configured speed (or rather delay) of JTAG.
  uint16_t m_period;
} DataModel_t;

// -------------------------------------------------------------------------- //

// Data model instance- global due to setup()/loop() behavior.
DataModel_t s_model;

// -------------------------------------------------------------------------- //

// Bit-bangs one JTAG period. Intended to be called back-to-back.
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

// -------------------------------------------------------------------------- //

// Performs one byte of JTAG transfer w/ given last TMS value. Returns TDO byte.
uint8_t jtagByte(uint8_t dataByte, bool toUpdate, uint16_t clkPeriod) {
  // Returned byte from write/read transfer.
  uint8_t retByte = 0;
  
  // Transfer one byte across JTAG connection (MSbit first).
  for (uint8_t i = 0; i < BITS_PER_BYTE; i++) {
    // Determine input bits for transfer.
    uint8_t tdiBit = (dataByte >> (BITS_PER_BYTE - 1));     // Get MSB to send
    uint8_t tmsBit = (i == (BITS_PER_BYTE - 1)) & toUpdate; // 0 = stay in state

    // Conduct transfer.
    uint8_t tdoBit = jtagCycle(tdiBit, tmsBit, clkPeriod);

    // Adjust inputs/outputs.
    retByte = (retByte << SHIFT_IN_LSB) | tdoBit; // "Shifted in" MSB into LSB
    dataByte <<= 1;                               // "Shifted out" MSB enitrely
  }

  // Return compound TDOs.
  return retByte;
}

// -------------------------------------------------------------------------- //

// Attempts to execute command (and leave data in buffer). Returns if executed.
bool executeCmd(DataModel_t* model) {
  // Ensure command actually contains command byte.
  if (CFG_CMD_IDX >= model->m_bufSize) {
    // Unable to execute- failure.
    return false;
  }

  /* Line reached- message sent over can be parsed/run as a command */
  
  // Run the defined command.
  switch (model->m_buffer[CFG_CMD_IDX]) {
    case CHAR_CMD_PERIOD: {
      // Save previous period.
      uint16_t prevPrd = model->m_period;

      // Set new period (use last 2 bytes- not counting 'execute' char).
      uint16_t newPrd = 0;
      for (uint16_t i = CFG_DATA_IDX; i < (model->m_bufSize - 1); i++) {
        // Determine new period.
        newPrd = (newPrd << BITS_PER_BYTE) | model->m_buffer[i];

        // Zero-out on the way (makes setting to old period easier).
        model->m_buffer[i] = 0;
      }

      // Set new period.
      model->m_period = newPrd;

      // Save old in model (for reporting).
      model->m_buffer[model->m_bufSize - 3] = (prevPrd >> BITS_PER_BYTE);
      model->m_buffer[model->m_bufSize - 2] = prevPrd & MASK_LOW_BYTE;
      
      break;
    }
    case CHAR_CMD_INSTR: {
      // Get to instruction register (ie instruction shift state).
      jtagCycle(LOGIC_LOW, LOGIC_LOW, model->m_period);
      jtagCycle(LOGIC_LOW, LOGIC_LOW, model->m_period);

      // Shift in instruction bytes (and record output).
      for (uint16_t i = CFG_DATA_IDX; i < (model->m_bufSize - 1); i++) {
        model->m_buffer[i] = jtagByte(model->m_buffer[i],
                                      i == (model->m_bufSize - 2),
                                      model->m_period
                                     );
      }

      // Return to JTAG's IDLE state.
      jtagCycle(LOGIC_LOW, LOGIC_HIGH, model->m_period);

      /* Recorded same length output return data already adjusted */
      
      break;
    }
    case CHAR_CMD_DATA: {
      // Get to data register (ie data shift state).
      jtagCycle(LOGIC_LOW, LOGIC_LOW, model->m_period);
      jtagCycle(LOGIC_LOW, LOGIC_HIGH, model->m_period);
      jtagCycle(LOGIC_LOW, LOGIC_LOW, model->m_period);

      // Shift in data bytes (and record output).
      for (uint16_t i = CFG_DATA_IDX; i < (model->m_bufSize - 1); i++) {
        model->m_buffer[i] = jtagByte(model->m_buffer[i],
                                      i == (model->m_bufSize - 2),
                                      model->m_period
                                     );
      }

      // Return to JTAG's IDLE state.
      jtagCycle(LOGIC_LOW, LOGIC_HIGH, model->m_period);

      /* Recorded same length output return data already adjusted */
      
      break;
    }
    default:
      // Invalid command- failure.
      return false;
  }

  // Command executed- sucess!
  return true;
}

// -------------------------------------------------------------------------- //

// Initializes serial, JTAG pins, and program's data (called once on reset).
void setup() {
  // Setup pin directions.
  pinMode(PIN_TCK, OUTPUT);
  pinMode(PIN_TDI, OUTPUT);
  pinMode(PIN_TMS, OUTPUT);
  pinMode(PIN_TDO, INPUT);

  // Initialize starting pin values.
  digitalWrite(PIN_TCK, LOW);
  digitalWrite(PIN_TDI, LOW);
  digitalWrite(PIN_TMS, HIGH);

  // (Set inputs as pull-ups).
  digitalWrite(PIN_TDO, HIGH);

  // Init serial connection.
  Serial.begin(CFG_BAUD_RATE);
  while(!Serial) {};

  // Setup program data.
  s_model.m_bufSize = 0;
  s_model.m_charEscaped = false;
  s_model.m_period = CFG_INIT_PERIOD;
}

// -------------------------------------------------------------------------- //

// Main program. Parses in command, executes it, and returns result/data.
void loop() {
  // Monitor Serial input data.
  if (Serial.available()) { 
    // Grab newest char.
    uint8_t newByte = Serial.read();

    // Append (or escape) as necessary.
    if ((newByte == CHAR_ESC_CHAR) && !s_model.m_charEscaped) {
      // Escape char- record presence.
      s_model.m_charEscaped = true;
      return;                       // like "continue" in context of loop()
    }
    else {
      // Otherwise, append to buffer (if able).
      if (s_model.m_bufSize < CFG_BUF_SIZE) {
        s_model.m_buffer[s_model.m_bufSize] = newByte;
        s_model.m_bufSize++;
      }
    }

    /* Line reached- char in question is NOT used as escape char */

    // Execute command if (unescaped) 'execute' char is seen.
    if ((newByte == CHAR_EXE_CMD) && !s_model.m_charEscaped) {  
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
}
