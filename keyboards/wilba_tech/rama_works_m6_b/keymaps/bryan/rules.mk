# avr-gcc 16 / avr-libc 2.3 (Arch) cannot build QMK's AVR core cleanly:
# send_string.c uses TCNTn without <avr/io.h>, and LUFA trips new
# -Werror warnings. Send-string is unused here, so drop it, and keep
# warnings as warnings.
SEND_STRING_ENABLE = no
EXTRAFLAGS += -Wno-error
