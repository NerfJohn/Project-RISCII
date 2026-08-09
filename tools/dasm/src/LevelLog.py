################################################################################
# LevelLog.py: serial logger with 'levels' for verobsity control.
################################################################################

from enum import Enum

################################################################################
# Levels of logging.
class LevelLogLevel(Enum):
    SILENT  = 0
    ERROR   = 1
    WARNING = 2
    INFO    = 3
    DEBUG   = 4

################################################################################
# Serial logger with verbosity control.
class LevelLog():
    # Variables.
    m_lvl = LevelLogLevel.SILENT # current level setting

    ############################################################################
    # Init w/ optional level set.
    def __init__(self, lvl: LevelLogLevel=LevelLogLevel.SILENT):
        self.m_lvl = lvl

    ############################################################################
    # Strinify LevelLog object.
    def __str__(self):
        return f"(lvl={self.m_lvl.name})"

    ############################################################################
    # Set level.
    def setLevel(self, lvl: LevelLogLevel):
        self.m_lvl = lvl

    ############################################################################
    # Print (if allowed by level).
    def print(self, lvl: LevelLogLevel, msg: str):
        # Skip if 1) silenced 2) set level too low or 3) 'print silent'
        if (self.m_lvl == LevelLogLevel.SILENT): return
        if (self.m_lvl.value < lvl.value):       return
        if (lvl == LevelLogLevel.SILENT):        return

        # Print.
        if   (lvl == LevelLogLevel.ERROR):   print(f"[ERR] {msg}")
        elif (lvl == LevelLogLevel.WARNING): print(f"[WRN] {msg}")
        elif (lvl == LevelLogLevel.INFO):    print(f"[INF] {msg}")
        elif (lvl == LevelLogLevel.DEBUG):   print(f"[DBG] {msg}")
