# Set a variable to determine which Linux distribution is being used.
# Debian packages things differently to Fedora (which is standard).
#
# This module sets the variable:
#
#  LSB_ID_SHORT – the short name of the Linux distribution or unknown.

set(LSB_ID_SHORT "unknown")

find_program(LSB_RELEASE lsb_release)
if(LSB_RELEASE)
  execute_process(COMMAND ${LSB_RELEASE} -is
    OUTPUT_VARIABLE LSB_ID_SHORT
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
endif()
