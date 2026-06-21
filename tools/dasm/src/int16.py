################################################################################
# int16.py: common ops/values for 16-bit word pulled from binary image.
# by John Eslinger
################################################################################

import copy

################################################################################
# Basic 16-bit word based on raw binary image bytes.
class Int16:
    # Core value.
    value = 0

    # Initialize with raw bytes.
    def __init__(self, rawBytes: bytes):
        # Convert.
        self.value = int.from_bytes(rawBytes, byteorder='big', signed=False)

        # Scope.
        self.value = 0xFFFF & self.value
    
    # Copy ctor.
    def copy(self):
        return copy.copy(self)
    
    # Get as integer.
    def as_int(self):
        return self.value
    
    # Get as hex string.
    def as_hex(self):
        return f"0x{self.value:04X}"
