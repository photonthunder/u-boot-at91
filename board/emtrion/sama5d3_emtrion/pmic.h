/*
 * Copyright (C) 2015 emtrion GmbH
 *                    Frank Erdrich <frank.erdrich@emtrion.de>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */


#ifndef PMIC_H
#define PMIC_H

#define ACT9845_I2C_ADDR	0x5B

/* ACT9845 register addresses */
#define REG2_VSET_PRIM		0x30
#define REG2_VSET_SEC		0x31
#define REG2_MODE		0x32

#define REG4_VSET		0x50
#define REG4_MODE		0x51

#define REG5_VSET		0x54
#define REG5_MODE		0x55

/* Bit settings */
#define MODE_NFLTMSK		(1 << 1)
#define MODE_LOWIQ		(1 << 5)
#define MODE_DISCHARGE		(1 << 6)
#define MODE_OUTPUT		(1 << 7)

/* Delay settings */
#define DELAY_0MS		(0 << 2)
#define DELAY_2MS		(1 << 2)
#define DELAY_4MS		(2 << 2)
#define DELAY_8MS		(3 << 2)
#define DELAY_16MS		(4 << 2)
#define DELAY_32MS		(5 << 2)
#define DELAY_64MS		(6 << 2)
#define DELAY_128MS		(7 << 2)

/* Voltage Set values (VSET) */
#define VSET_1V15		0x16
#define VSET_1V175		0x17
#define VSET_1V2		0x18
#define VSET_1V25		0x19
#define VSET_1V3		0x1A

#define VSET_1V75		0x23
#define VSET_1V8		0x24
#define VSET_1V85		0x25

#define VSET_2V4		0x30
#define VSET_2V5		0x31
#define VSET_2V6		0x32

#define VSET_3V2		0x38
#define VSET_3V3		0x39
#define VSET_3V4		0x3A

/* Default mode register settings. */
#define REG4_MODE_DEFAULT	0xCC    	/* REG on, Discharge on, Delay 3 */
#define REG4_VSET_DEFAULT	VSET_2V5
#define REG5_MODE_DEFAULT	0xC0    	/* REG on, Discharge on */
#define REG5_VSET_DEFAULT	VSET_3V3

#endif			// PMIC_H
