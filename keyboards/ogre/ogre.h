/* Copyright 2019 ctrlshiftba
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include "quantum.h"

#ifdef USE_I2C
#include <stddef.h>
#ifdef __AVR__
  #include <avr/io.h>
  #include <avr/interrupt.h>
#endif
#endif

/* This a shortcut to help you visually see your layout.
 *
 * The first section contains all of the arguments representing the physical
 * layout of the board and position of the keys.
 *
 * The second converts the arguments into a two-dimensional array which
 * represents the switch matrix.
 */
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
) { \
	{ K001,  K000,  K002,  K003,  K004,  K005,  K006 }, \
	{ K021,  K020,  K022,  K023,  K024,  K025,  K026 }, \
	{ K041,  K040,  K042,  K043,  K044,  K045,  K046 }, \
	{ K062,  K061,  K063,  K064,  K065,  K066,  K060 }, \
	{ K081,  K080,  K082,  K083,  K084,  K085,  K086 }, \
	{ K015,  K016,  K014,  K013,  K012,  K011,  K010 }, \
	{ K035,  K036,  K034,  K033,  K032,  K031,  K030 }, \
	{ K055,  K056,  K054,  K053,  K052,  K051,  K050 }, \
	{ K075,  K076,  K074,  K073,  K072,  K071,  K070 }, \
	{ K095,  K096,  K094,  K093,  K092,  K091,  K090 }  \
}


