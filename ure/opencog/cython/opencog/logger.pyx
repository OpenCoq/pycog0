from ure cimport ure_logger as c_ure_logger
from opencog.logger cimport wrap_clogger, cLogger

def ure_logger():
    return wrap_clogger(&c_ure_logger())
