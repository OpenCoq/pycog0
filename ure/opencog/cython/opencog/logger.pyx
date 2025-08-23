from ure cimport ure_logger as c_ure_logger
from opencog.logger cimport cLogger

def ure_logger():
    # Create a simple wrapper for the ure logger
    from opencog.logger import Logger
    logger = Logger()
    return logger
