/*
 * Copyright (C) 2017 Frank Erdrich
 * Copyright (C) 2017 emtrion GmbH.
 *
 * Slow Clock Controller (SCKC) - System peripherals regsters.
 * Based on SamA5D36 datasheet.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef AT91_SCKC_H
#define AT91_SCKC_H

#ifndef __ASSEMBLY__

typedef struct at91_sckc {
	u32	cr;	/* Slow Clock Control Register */
} at91_sckc_t;

#endif /* __ASSEMBLY__ */

#define AT91_SCKC_CR_RCEN		0
#define AT91_SCKC_CR_OSC32EN	1
#define AT91_SCKC_CR_OSC32BYP	2
#define AT91_SCKC_CR_OSCSEL		3

#endif /* AT91_SCKC_H */
