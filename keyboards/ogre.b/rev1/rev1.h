#pragma once

#include "../redox.h"

#include "quantum.h"


#ifdef USE_I2C
#include <stddef.h>
#ifdef __AVR__
  #include <avr/io.h>
  #include <avr/interrupt.h>
#endif
#endif

#define LAYOUT( \
	K000, K001, K002, K003, K004, K005, K006, \
	K010, K011, K012, K013, K014, K015, K016, \
	K020, K021, K022, K023, K024, K025, K026, \
	K030, K031, K032, K033, K034, K035, K036, \
	K040, K041, K042, K043, K044, K045, K046, \
	K050, K051, K052, K053, K054, K055, K056, \
	K060, K061, K062, K063, K064, K065, K066, \
	K070, K071, K072, K073, K074, K075, K076, \
	K080, K081, K082, K083, K084, K085, K086, \
	K090, K091, K092, K093, K094, K095, K096  \
) \
{ \
	{ K000,  K001,  K002,  K003,  K004,  K005,  K006 }, \
	{ K010,  K011,  K012,  K013,  K014,  K015,  K016 }, \
	{ K020,  K021,  K022,  K023,  K024,  K025,  K026 }, \
	{ K030,  K031,  K032,  K033,  K034,  K035,  K036 }, \
	{ K040,  K041,  K042,  K043,  K044,  K045,  K046 }, \
	{ K050,  K051,  K052,  K053,  K054,  K055,  K056 }, \
	{ K060,  K061,  K062,  K063,  K064,  K065,  K066 }, \
	{ K070,  K071,  K072,  K073,  K074,  K075,  K076 }, \
	{ K080,  K081,  K082,  K083,  K084,  K085,  K086 }, \
	{ K090,  K091,  K092,  K093,  K094,  K095,  K096 }  \
}
