// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2018
 *		      Daniel Evans <photonthunder@gmail.com>
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/sama5d3_smc.h>
#include <asm/arch/at91_common.h>
#include <asm/arch/at91_rstc.h>
#include <asm/arch/gpio.h>
#include <asm/arch/clk.h>
#include <debug_uart.h>
#include <spl.h>
#include <asm/arch/atmel_mpddrc.h>
#include <asm/arch/at91_wdt.h>
#include "at91_sckc.h"
#include "pmic.h"

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_NAND_ATMEL
void sama5d3_xplained_nand_hw_init(void)
{
	struct at91_smc *smc = (struct at91_smc *)ATMEL_BASE_SMC;

	at91_periph_clk_enable(ATMEL_ID_SMC);

	/* Configure SMC CS3 for NAND/SmartMedia */
	writel(AT91_SMC_SETUP_NWE(2) | AT91_SMC_SETUP_NCS_WR(1) |
	       AT91_SMC_SETUP_NRD(2) | AT91_SMC_SETUP_NCS_RD(1),
	       &smc->cs[3].setup);
	writel(AT91_SMC_PULSE_NWE(3) | AT91_SMC_PULSE_NCS_WR(5) |
	       AT91_SMC_PULSE_NRD(3) | AT91_SMC_PULSE_NCS_RD(5),
	       &smc->cs[3].pulse);
	writel(AT91_SMC_CYCLE_NWE(8) | AT91_SMC_CYCLE_NRD(8),
	       &smc->cs[3].cycle);
	writel(AT91_SMC_TIMINGS_TCLR(3) | AT91_SMC_TIMINGS_TADL(10) |
	       AT91_SMC_TIMINGS_TAR(3)  | AT91_SMC_TIMINGS_TRR(4)   |
	       AT91_SMC_TIMINGS_TWB(5)  | AT91_SMC_TIMINGS_RBNSEL(3)|
	       AT91_SMC_TIMINGS_NFSEL(1), &smc->cs[3].timings);
	writel(AT91_SMC_MODE_RM_NRD | AT91_SMC_MODE_WM_NWE |
	       AT91_SMC_MODE_EXNW_DISABLE |
#ifdef CONFIG_SYS_NAND_DBW_16
	       AT91_SMC_MODE_DBW_16 |
#else /* CONFIG_SYS_NAND_DBW_8 */
	       AT91_SMC_MODE_DBW_8 |
#endif
	       AT91_SMC_MODE_TDF_CYCLE(3),
	       &smc->cs[3].mode);
}
#endif

#ifdef CONFIG_CMD_USB
static void sama5d3_xplained_usb_hw_init(void)
{
	at91_set_pio_output(AT91_PIO_PORTE, 3, 0);
	at91_set_pio_output(AT91_PIO_PORTE, 4, 0);
}
#endif

#ifdef CONFIG_DEBUG_UART_BOARD_INIT
void board_debug_uart_init(void)
{
	at91_seriald_hw_init();
}
#endif

#ifdef CONFIG_BOARD_EARLY_INIT_F
static void use_crystal_osc_for_slowclk(void)
{
	struct at91_sckc *sckc = (struct at91_sckc *)ATMEL_BASE_SCKCR;
	struct at91_pmc *pmc = (struct at91_pmc *)ATMEL_BASE_PMC;
	u32 tmp;
	
	tmp = readl(&pmc->mckr);
	
	/* check, if we are running on an other clock source than the slow clock.
	 * If we run on slow clock, we can't switch the source of that, otherwise
	 * the cpu would stop working, so exit this function.
	 */
	if (tmp & (1 << AT91_PMC_MCKR_CSS_SLOW)) {
		printf("Can't switch slow clock source to XTAL.\n");
		return;
	}
	
	tmp = readl(&sckc->cr);
	
	/* check if crystal oscillator is on, activate it if not */
	if ((tmp & (1 << AT91_SCKC_CR_OSC32EN)) == 0) {
		tmp |= (1 << AT91_SCKC_CR_OSC32EN);
		writel(tmp, &sckc->cr);
		
		/* wait until the oscillator is stabilized */
		udelay(1000);
	}
	
	/* switch to the crystal ocillator, if not already */
	if ((tmp & (1 << AT91_SCKC_CR_OSCSEL)) == 0) {
		tmp |= (1 << AT91_SCKC_CR_OSCSEL);
		writel(tmp, &sckc->cr);
		
		/* wait at least 5 slowclock cycles (~152 us)*/
		udelay(200);
	}
	
	/* check, if rc-osc is running, disable it */
	if (tmp & (1 << AT91_SCKC_CR_RCEN)) {
		tmp &= ~(1 << AT91_SCKC_CR_RCEN);
		writel(tmp, &sckc->cr);
	}
}

int board_early_init_f(void)
{
	/*at91_set_pio_output(AT91_PIO_PORTB, 14, 0);
	at91_set_pio_output(AT91_PIO_PORTB, 15, 0);
	at91_set_pio_output(AT91_PIO_PORTD, 14, 0);
	at91_set_pio_output(AT91_PIO_PORTD, 15, 0);
	at91_set_pio_output(AT91_PIO_PORTD, 16, 0);
	at91_set_pio_output(AT91_PIO_PORTD, 17, 0);
	at91_set_pio_output(AT91_PIO_PORTD, 18, 0);*/

#ifdef CONFIG_DEBUG_UART
	debug_uart_init();
#endif
	use_crystal_osc_for_slowclk();
	return 0;
}
#endif

int board_init(void)
{
	/* adress of boot parameters */
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;

#ifdef CONFIG_NAND_ATMEL
	sama5d3_xplained_nand_hw_init();
#endif
#ifdef CONFIG_CMD_USB
	sama5d3_xplained_usb_hw_init();
#endif
#ifdef CONFIG_GENERIC_ATMEL_MCI
	at91_mci_hw_init();
#endif
	return 0;
}

int dram_init(void)
{
	gd->ram_size = get_ram_size((void *)CONFIG_SYS_SDRAM_BASE,
				    CONFIG_SYS_SDRAM_SIZE);

	return 0;
}

/* SPL */
#ifdef CONFIG_SPL_BUILD

static void setPortEtoInput(void) {
	int i;
	at91_set_pio_output(AT91_PIO_PORTE, 1, 0);
	for (i=1; i<32; i++) {
		/* Do not touch PE21 & PE22: Reserved for the NAND */
		if (i != 21 && i != 22) {
			at91_set_pio_input(AT91_PIO_PORTE, i, 0);
		}
	}
}

typedef enum {
	Rev2,
	Rev3Plus
} BoardRevision_t;

BoardRevision_t get_board_revision(void)
{
	at91_periph_clk_enable(ATMEL_ID_PIOC);
	/* init PINC20 as input with pull-up enabled */
	at91_set_pio_input(AT91_PIO_PORTC, 20, 1);
	
	/* we have a Rev2 board, if value is high, otherwise
	 * it is a Rev3Plus board
	 */
	
	if (at91_get_pio_value(AT91_PIO_PORTC, 20) == 1) {
		return Rev2;
	} else {
		return Rev3Plus;
	}
}

static void print_board_rev(void)
{
	BoardRevision_t temp;
	
	temp = get_board_revision();
	printf("Board Revision: ");
	
	if (temp == Rev2) {
		printf("Rev2\n");
	} else {
		printf("Rev3Plus\n");
	}
}

void spl_board_init(void)
{
	setPortEtoInput();
	print_board_rev();
#ifdef CONFIG_SD_BOOT
#ifdef CONFIG_GENERIC_ATMEL_MCI
	at91_mci_hw_init();
#endif
#elif CONFIG_NAND_BOOT
	sama5d3_xplained_nand_hw_init();
#endif
}

static void ddr2_conf(struct atmel_mpddrc_config *ddr2)
{
	ddr2->md = (ATMEL_MPDDRC_MD_DBW_32_BITS | ATMEL_MPDDRC_MD_DDR2_SDRAM);

	ddr2->cr = (ATMEL_MPDDRC_CR_NC_COL_10 |
		    ATMEL_MPDDRC_CR_NR_ROW_13 |
		    ATMEL_MPDDRC_CR_CAS_DDR_CAS3 |
		    ATMEL_MPDDRC_CR_ENRDM_ON |
		    ATMEL_MPDDRC_CR_NB_8BANKS |
		    ATMEL_MPDDRC_CR_NDQS_DISABLED |
		    ATMEL_MPDDRC_CR_DECOD_INTERLEAVED |
		    ATMEL_MPDDRC_CR_UNAL_SUPPORTED);
	/*
	 * As the DDR2-SDRAm device requires a refresh time is 7.8125us
	 * when DDR run at 133MHz, so it needs (7.8125us * 133MHz / 10^9) clocks
	 */
	ddr2->rtr = 0x411;

	ddr2->tpr0 = (6 << ATMEL_MPDDRC_TPR0_TRAS_OFFSET |
		      2 << ATMEL_MPDDRC_TPR0_TRCD_OFFSET |
		      2 << ATMEL_MPDDRC_TPR0_TWR_OFFSET |
		      8 << ATMEL_MPDDRC_TPR0_TRC_OFFSET |
		      2 << ATMEL_MPDDRC_TPR0_TRP_OFFSET |
		      2 << ATMEL_MPDDRC_TPR0_TRRD_OFFSET |
		      1 << ATMEL_MPDDRC_TPR0_TWTR_OFFSET |
		      2 << ATMEL_MPDDRC_TPR0_TMRD_OFFSET);

	ddr2->tpr1 = (2 << ATMEL_MPDDRC_TPR1_TXP_OFFSET |
		      200 << ATMEL_MPDDRC_TPR1_TXSRD_OFFSET |
		      18 << ATMEL_MPDDRC_TPR1_TXSNR_OFFSET |
		      17 << ATMEL_MPDDRC_TPR1_TRFC_OFFSET);

	ddr2->tpr2 = (7 << ATMEL_MPDDRC_TPR2_TFAW_OFFSET |
		      2 << ATMEL_MPDDRC_TPR2_TRTP_OFFSET |
		      3 << ATMEL_MPDDRC_TPR2_TRPA_OFFSET |
		      8 << ATMEL_MPDDRC_TPR2_TXARDS_OFFSET |
		      2 << ATMEL_MPDDRC_TPR2_TXARD_OFFSET);
}

void mem_init(void)
{
	struct atmel_mpddr *mpddr = (struct atmel_mpddr *)ATMEL_BASE_MPDDRC;
	struct atmel_mpddrc_config ddr2;
	unsigned int reg;

	ddr2_conf(&ddr2);

	/* Enable MPDDR clock */
	at91_periph_clk_enable(ATMEL_ID_MPDDRC);
	at91_system_clk_enable(AT91_PMC_DDR);
	
	/* Init the special (magic) registers for sama5d3x */
	/* MPDDRC DLL Slave Offset Register: DDR2 configuration */
	reg = ATMEL_MPDDRC_S0OFF_1
			| ATMEL_MPDDRC_S2OFF_1
			| ATMEL_MPDDRC_S3OFF_1;
	writel(reg, &mpddr->dll_sof);
	
	/* MPDDRC DLL Master Offset Register */
	/* write master + clk90 offset */
	reg = ATMEL_MPDDRC_MOFF_7
			| ATMEL_MPDDRC_CLK90OFF_31
			| ATMEL_MPDDRC_SELOFF_ENABLED
			| ATMEL_MPDDRC_KEY;
	writel(reg, &mpddr->dll_mo);
	
	/* MPDDRC I/O Calibration Register */
	reg = ATMEL_MPDDRC_IO_CALIBR_DDR2_RZQ_52
			| ATMEL_MPDDRC_IO_CALIBR_TZQIO_(4); /* (DDRCK * 20*10^-9) + 1 */
	writel(reg, &mpddr->io_calibr);
	
	/* DDRAM2 Controller initialize */
	ddr2_init(ATMEL_BASE_MPDDRC, ATMEL_BASE_DDRCS, &ddr2);
	udelay(2000);
}



void at91_pmc_init(void)
{
	u32 tmp;
	
	tmp = AT91_PMC_PLLAR_29 |
			AT91_PMC_PLLXR_PLLCOUNT(0x3f) |
			AT91_PMC_PLLXR_MUL(43) |
			AT91_PMC_PLLXR_DIV(1);
	at91_plla_init(tmp);
	
	at91_pllicpr_init(AT91_PMC_IPLL_PLLA(0x3));
	
	/* prevents sytem halt after romboot */
	udelay(10);
	
	tmp = AT91_PMC_MCKR_MDIV_4 | AT91_PMC_MCKR_CSS_MAIN;
	at91_mck_init(tmp);
	
	tmp = AT91_PMC_MCKR_MDIV_4 | AT91_PMC_MCKR_CSS_PLLA;
	at91_mck_init(tmp);
	
	udelay(1000);
	
}
#endif
