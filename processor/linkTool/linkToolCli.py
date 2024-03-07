'''
LinkToolCli.py

CLI tool to interface with Link firmware (and thus JTAG circuit).

Author: John Eslinger
'''

import os.path as fs
import re
import serial
import signal
import sys

# Return codes for the program
RETURN_GOOD = 0
RETURN_BAD  = 1

# Cmd line argument information/constants.
ARG_HELP_STRING = "Usage: python linkToolCli.py [-h] <COM port name>"
ARG_NO_ARGS_NUM = 1
ARG_REGEX_FLAG  = r'-.*'
ARG_REGEX_HELP  = r'-h'
ARG_REGEX_PORT  = r'COM[0-9]+'
ARG_REQ_ARG_NUM = 1
ARG_IDX_PORT    = 0

# Constant arguments related to serial port.
SERIAL_RATE     = 9600
SERIAL_TIMEOUT  = 1
SERIAL_TEST_CMD = b'\x0A'
SERIAL_TEST_RSP = b'\x00\x0A'

# CLI information/constants.
CLI_PREFIX      = "> "
CLI_SPACE       = " "
CLI_TAB         = "\t"
CLI_NEWLINE     = "\n"
CLI_RETURN      = "\r"
CLI_CMD_IDX     = 0
CLI_VAL_IDX     = 1
CLI_REGEX_QUIT  = "quit"
CLI_REGEX_VAL   = "0x[0-9a-fA-F]+"

# Custom command constants.
CMD_SET_PERIOD  = "period"
CMD_SET_ADDR    = "addr"
CMD_READ        = "read"
CMD_WRITE       = "write"
CMD_INSTR_ADDR  = b'\x01'
CMD_INSTR_READ  = b'\x02'
CMD_INSTR_WRITE = b'\x03'

'''
Handle arguments passed to script. Handles flags and returns required arguments.

Handles parsing of arguments from user. Flags are processed while arguments are
checked for validity prior to passing back to caller for further use.

@return input COM port
'''
def handleArgs():
    # No-arg call should be informative (albeit still be an error).
    if len(sys.argv) == ARG_NO_ARGS_NUM: # argv[0] = "linkToolCli.py"
        print(ARG_HELP_STRING)
        sys.exit(RETURN_BAD)
    
    # Parse arguments/options (except for argv[0] = "linkToolCli.py").
    reqArgs = []
    for i in range(1,len(sys.argv)): # argv[1] and onwards...
        # Get argument.
        arg = sys.argv[i]
        
        # Parse flags (as applicable).
        if re.fullmatch(ARG_REGEX_FLAG, arg):
            # Help Flag- print information and (successfully) exit.
            if re.fullmatch(ARG_REGEX_HELP, arg):
                print(ARG_HELP_STRING)
                sys.exit(RETURN_GOOD)
            
            # Bad flag- inform user and exit.
            print("Bad flag: \"%s\"- see \"python genScanState.py -h\""%arg)
            sys.exit(RETURN_BAD)
        
        # Parse non-flags as required args.
        reqArgs.append(arg)
    
    # Check expected number of required args is accurate.
    if len(reqArgs) != ARG_REQ_ARG_NUM:
        print("Expected %d args, %d given"%(ARG_REQ_ARG_NUM, len(reqArgs)))
        sys.exit(RETURN_BAD)
    
    # Check each argument is of expected type (and exists).
    portName = reqArgs[ARG_IDX_PORT]
    if not re.fullmatch(ARG_REGEX_PORT, portName):
        print("\"%s\" is not a valid COM port"%(portName))
        sys.exit(RETURN_BAD)

    # Arguments parsed- return arguments for further use.
    return portName

'''
Initialize serial port/connection, confirming connection is with link firmware.

Function not only ensures serial connection, but affirms link firmware is
connected by sending some benign test data (and checking response).

@param portName name of serial port to connect to
@return initialized serial object, connected to link firmware
'''
def initSerial(portName):
    # (Try to) connect to serial port/link tool.
    linkSerial = None
    try:
        linkSerial = serial.Serial(port=portName, baudrate=SERIAL_RATE, timeout=SERIAL_TIMEOUT)
    except:
        print("Failed to connect to serial port \"%s\""%(portName))
        sys.exit(RETURN_BAD)
    
    # For posterity: affirm link is connected with sample command.
    linkSerial.write(SERIAL_TEST_CMD)
    response = linkSerial.readline()
    if response != SERIAL_TEST_RSP:
        print("Failed to confirm connection to link firmware \"%s\""%(portName))
        sys.exit(RETURN_BAD)
    
    # Return solid serial connection.
    return linkSerial

'''
Runs main CLI of script. Gets user input and executes approapriate procedure.

@param linkSerial configured serial port to link firmware
'''
def runCLI(linkSerial):
    # Local command to do link firmware transaction.
    def sendMsg(msg):
        # Send message.
        linkSerial.write(msg)
        
        # Keep receiving until newline is detected.
        retMsg = b''
        while b'\n' not in retMsg:
            retMsg += linkSerial.readline()
        
        # Return return message.
        return retMsg

    # CLI generally runs forever- broken from within.
    while True:
        # Display readiness for input.
        print(CLI_PREFIX, end="")
        sys.stdout.flush()
    
        # Get new input (as list of pertinent tokens).
        rawInput = sys.stdin.readline()
        rawInput = rawInput.replace(CLI_RETURN, '').replace(CLI_NEWLINE, '').replace(CLI_SPACE, CLI_TAB)
        tkns = [x for x in rawInput.split(CLI_TAB) if x]
        
        # Skip empty lines.
        if len(tkns) == 0:
            continue
        
        # Break out for built-in keyword.
        if re.fullmatch(CLI_REGEX_QUIT, tkns[CLI_CMD_IDX]):
            return
            
        # Custom commands- ensure value exists/is given.
        if len(tkns) <= CLI_VAL_IDX or not re.fullmatch(CLI_REGEX_VAL, tkns[CLI_VAL_IDX]):
            print("Command must come with value (ie %s)"%(CLI_REGEX_VAL))
            continue
        
        # Run "custom" procedures.
        if re.fullmatch(CMD_SET_PERIOD, tkns[CLI_CMD_IDX]):
            # Parse command to send.
            msg = b'\x30' + bytearray.fromhex(tkns[CLI_VAL_IDX][2:][:4].zfill(4)) + b'\n'
            
            # Conduct transaction.
            retMsg = sendMsg(msg)
            
            # Report.
            if retMsg[0]:
                print("Success! Previous period = %d ms"%(int.from_bytes(retMsg[1:3], "big")))
            else:
                print("Failure! Link reported failure")
        if re.fullmatch(CMD_SET_ADDR, tkns[CLI_CMD_IDX]):
            # Parse commands to send.
            msg1 = b'\x32' + bytearray.fromhex(tkns[CLI_VAL_IDX][2:][:4].zfill(4)) + b'\n'
            msg2 = b'\x31' + CMD_INSTR_ADDR + b'\n'
            
            # Conduct transaction.
            isGood = sendMsg(msg1)[0] and sendMsg(msg2)[0]
            
            # Report.
            if not isGood:
                print("Failure! Link failed to send command(s)")
        if re.fullmatch(CMD_READ, tkns[CLI_CMD_IDX]):
            # Parse commands to send.
            msg1 = b'\x31' + CMD_INSTR_READ + b'\n'
            msg2 = b'\x32' + b'\x00\x00' + b'\n'
            
            # Conduct transaction.
            resp1 = sendMsg(msg1)
            resp2 = sendMsg(msg2)
            isGood = resp1[0] and resp2[0]
            
            # Report.
            if isGood:
                print("Success! Read data = %s"%(hex(int.from_bytes(resp2[1:3], "big"))))
            else:
                print("Failure! Link failed to send command(s)")
        if re.fullmatch(CMD_WRITE, tkns[CLI_CMD_IDX]):
            # Parse commands to send.
            msg1 = b'\x32' + bytearray.fromhex(tkns[CLI_VAL_IDX][2:][:4].zfill(4)) + b'\n'
            msg2 = b'\x31' + CMD_INSTR_WRITE + b'\n'
            
            # Conduct transaction.
            isGood = sendMsg(msg1)[0] and sendMsg(msg2)[0]
            
            # Report.
            if not isGood:
                print("Failure! Link failed to send command(s)")

'''
Main application- connect to link firmware via serial and control it with CLI commands.
'''
if __name__ == "__main__":
    # Handle arguments.
    portName = handleArgs()
    
    # Connect to link firmware (through serial port).
    linkSerial = initSerial(portName)
    
    # Run CLI for user.
    print("=== Project RISCII Link CLI ===")
    runCLI(linkSerial)
    
    # Gracefully exit.
    print("Closing serial port...")
    linkSerial.close()
    sys.exit(RETURN_GOOD)
