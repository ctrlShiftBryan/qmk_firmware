#pragma once

// Double-tap window for ports 7 and 8. Measured double taps on this pad
// were 310-375 ms apart, well past QMK's 200 ms default. Single taps on
// keys 1 and 3 wait this long before switching.
#define TAPPING_TERM 450
