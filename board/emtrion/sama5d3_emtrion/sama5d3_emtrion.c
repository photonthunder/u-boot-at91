// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2018 Nelson Irrigation
 *		      Dan Evans <photonthunder@gmail.com>
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/sama5d3_smc.h>
#include <asm/arch/at91_common.h>
#include <asm/arch/at91_rstc.h>
/* #include <asm/arch/at91_pio.h> */
#include <asm/arch/gpio.h>
#include <asm/arch/clk.h>
#include <debug_uart.h>
#include <spl.h>
#include <asm/arch/atmel_mpddrc.h>
#include <asm/arch/at91_wdt.h>

#include <mmc.h>
#include <asm/arch/at91_pmc.h>
#include <asm/arch/at91_sckc.h>
#include <atmel_mci.h>
/* #include <libfdt.h> */
#include <net.h>
#include <netdev.h>
#include <i2c.h>
#include <miiphy.h>
#include <micrel.h>
#include "pmic.h"

DECLARE_GLOBAL_DATA_PTR;

typedef enum {
	R2A,
	R3A
} eRevision_t;

#ifdef CONFIG_NAND_ATMEL
void sama5d3_nand_hw_init(void)
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
#endif /* CONFIG_SYS_NAND_DBW_16 */
	       AT91_SMC_MODE_TDF_CYCLE(3),
	       &smc->cs[3].mode);
}
#endif

#ifdef CONFIG_CMD_USB
static void sama5d3_usb_hw_init(void)
{
	at91_set_pio_output(AT91_PIO_PORTE, 3, 0);
	at91_set_pio_output(AT91_PIO_PORTE, 4, 0);
}
#endif /* CONFIG_CMD_USB */

/* SPI chip select control */
#ifdef CONFIG_ATMEL_SPI
#include <spi.h>
int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return bus == 0 && cs < 1;
}

void spi_cs_activate(struct spi_slave *slave)
{
	if (slave->cs == 0)
	{
		at91_set_pio_output(AT91_PIO_PORTD, 13, 0);
	}
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	if (slave->cs == 0)
	{
		at91_set_pio_output(AT91_PIO_PORTD, 13, 1);
	}
}
#endif /* CONFIG_ATMEL_SPI */

eRevision_t get_board_revision(void)
{
	/* init PINC20 as input with pull-up enabled */
	at91_set_pio_input(AT91_PIO_PORTC, 20, 1);
	
	/* we have a R2A board, if value is high, otherwise
	 * it is a R3A board
	 */
	
	if (at91_get_pio_value(AT91_PIO_PORTC, 20) == 1) {
		return R2A;
	} else {
		return R3A;
	}
}

#ifdef CONFIG_SYS_I2C_SOFT
int get_i2c_sda_pin(void)
{
	if (get_board_revision() == R2A) {
		return AT91_PIN_PC26;
	} else {
		return AT91_PIN_PA30;
	}
}

int get_i2c_scl_pin(void)
{
	if (get_board_revision() == R2A) {
		return AT91_PIN_PC27;
	} else {
		return AT91_PIN_PA31;
	}
}
#endif /* CONFIG_SYS_I2C_SOFT */

void pmic_init(void)
{
	uchar value;
	
	i2c_init(CONFIG_SYS_I2C_SOFT_SPEED, CONFIG_SYS_I2C_SOFT_SLAVE);
	
	puts("Increase core voltage from 1.2V to 1.25V.\n");
	
	value = VSET_1V25;
	if (i2c_write(ACT9845_I2C_ADDR, REG2_VSET_PRIM, 1, &value, 1))
	{
		puts("Error setting core voltage!\n");
	}
}

void use_crystal_osc_for_slowclk(void)
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

#if 0
#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
	at91_pda_detect();
	return 0;
}
#endif /* CONFIG_BOARD_LATE_INIT */
#endif

#ifdef CONFIG_DEBUG_UART_BOARD_INIT
void board_debug_uart_init(void)
{
	at91_seriald_hw_init();
}
#endif /* CONFIG_DEBUG_UART_BOARD_INIT */

#ifdef CONFIG_BOARD_EARLY_INIT_F
int board_early_init_f(void)
{
	at91_periph_clk_enable(ATMEL_ID_PIOA);
	at91_periph_clk_enable(ATMEL_ID_PIOB);
	at91_periph_clk_enable(ATMEL_ID_PIOC);
	at91_periph_clk_enable(ATMEL_ID_PIOD);
	at91_periph_clk_enable(ATMEL_ID_PIOE);
	
#ifdef CONFIG_DEBUG_UART
	debug_uart_init();
#endif /* CONFIG_DEBUG_UART */
	
	/* switch to the crystal oscillator slow clock source */
	use_crystal_osc_for_slowclk();
	

	return 0;
}
#endif /* CONFIG_BOARD_EARLY_INIT_F */


int board_init(void)
{
	/* adress of boot parameters */
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;

	
#ifdef CONFIG_ATMEL_SPI
	at91_spi0_hw_init(1 << 0);
#endif /* CONFIG_ATMEL_SPI */
#ifdef CONFIG_NAND_ATMEL
	sama5d3_nand_hw_init();
#endif /* CONFIG_NAND_ATMEL */
#ifdef CONFIG_CMD_USB
	sama5d3_usb_hw_init();
#endif /* CONFIG_CMD_USB */
#ifdef CONFIG_MACB
	at91_gmac_hw_init();
	at91_macb_hw_init();
#endif /* CONFIG_MACB */
	return 0;
}

int dram_init(void)
{
	gd->ram_size = get_ram_size((void *)CONFIG_SYS_SDRAM_BASE,
				    CONFIG_SYS_SDRAM_SIZE);

	return 0;
}

int board_eth_init(bd_t *bis)
{
#ifdef CONFIG_MACB
	macb_eth_initialize(0, (void *)ATMEL_BASE_GMAC, 0x07);
	macb_eth_initialize(0, (void *)ATMEL_BASE_EMAC, 0x01);
#endif /* CONFIG_MACB */
	return 0;
}

#ifdef CONFIG_GENERIC_ATMEL_MCI
int board_mmc_init(bd_t *bis)
{
	atmel_mci_init((void *)ATMEL_BASE_MCI0);
	
	return 0;
}
#endif /* CONFIG_GENERIC_ATMEL_MCI */

#ifdef CONFIG_OF_BOARD_SETUP
int ft_board_setup(void *blob, bd_t *bd)
{
	int node, i, ret;
	char *tmp, *end;
	unsigned char mac_addr[6];
	
	/* get the mac addr from env */
	tmp = getenv("ethaddr");
	if (!tmp) {
		printf("ethaddr env variable not defined\n");
		return -1;
	}
	for (i = 0; i < 6; i++) {
		mac_addr[i] = tmp ? simple_strtoul(tmp, &end, 16) : 0;
		if (tmp)
		tmp = (*end) ? end+1 : end;
	}
	
	/* find the correct fdt ethernet path and correct it */
	node = fdt_path_offset(blob, "/ahb/apb/ethernet@f0028000");
	if (node < 0) {
		printf("No /ahb/apb/ethernet@f0028000 offset!\n");
		return -1;
	}
	
	ret = fdt_setprop(blob, node, "local-mac-address", &mac_addr, 6);
	if (ret) {
		printf("error setting local-mac-address property\n");
		return -1;
	}
	
	/* get the mac addr from env */
	tmp = getenv("eth1addr");
	if (!tmp) {
		printf("eth1addr env variable not defined\n");
		return -1;
	}
	
	for (i = 0; i < 6; i++) {
		mac_addr[i] = tmp ? simple_strtoul(tmp, &end, 16) : 0;
		if (tmp)
		tmp = (*end) ? end+1 : end;
	}
	
	/* find the correct fdt ethernet path and correct it */
	node = fdt_path_offset(blob, "/ahb/apb/ethernet@f802c000");
	if (node < 0) {
		printf("No /ahb/apb/ethernet@f802c000 offset!\n");
		return -1;
	}
	
	ret = fdt_setprop(blob, node, "local-mac-address", &mac_addr, 6);
	if (ret) {
		printf("error setting local-mac-address property\n");
		return -1;
	}
	
	return 0;
	
}
#endif /* CONFIG_OF_BOARD_SETUP */

void print_board_rev(void)
{
	int temp;
	
	temp = get_board_revision();
	printf("Board Revision: ");
	
	if (temp == R2A) {
		printf("R2\n");
	} else {
		printf("R3\n");
	}
}

/* SPL */
#ifdef CONFIG_SPL_BUILD
void spl_board_init(void)
{
	int i;
	
	/* Set all PIO E to input */
	for (i=0;i<32;i++)
	{
		/* Do not touch PE21 & PE22: Reserved for the NAND */
		if (i != 21 && i != 22)
		at91_set_pio_input(AT91_PIO_PORTE, i, 0);
	}
	
	print_board_rev();
	
#ifdef CONFIG_SYS_I2C_SOFT
	pmic_init();
#endif /* CONFIG_SYS_I2C_SOFT */
	
#ifdef CONFIG_SD_BOOT
	at91_mci_hw_init();
#elif CONFIG_NAND_BOOT
	sama5d3_nand_hw_init();
#endif /* CONFIG_SD_BOOT or CONFIG_NAND_BOOT */
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

	ddr2->tpr2 = (6 << ATMEL_MPDDRC_TPR2_TFAW_OFFSET |
		      2 << ATMEL_MPDDRC_TPR2_TRTP_OFFSET |
		      3 << ATMEL_MPDDRC_TPR2_TRPA_OFFSET |
		      8 << ATMEL_MPDDRC_TPR2_TXARDS_OFFSET |
		      2 << ATMEL_MPDDRC_TPR2_TXARD_OFFSET);
}

void mem_init(void)
{
	struct at91_pmc *pmc = (struct at91_pmc *)ATMEL_BASE_PMC;
	struct atmel_mpddr *mpddr = (struct atmel_mpddr *)ATMEL_BASE_MPDDRC;
	struct atmel_mpddr ddr2;
	unsigned int reg;
	
	ddr2_conf(&ddr2);
	
	/* enable MPDDR clock */
	at91_periph_clk_enable(ATMEL_ID_MPDDRC);
	writel(0x4, &pmc->scer);
	
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
	reg = ATMEL_MPDDRC_RDIV_DDR2_RZQ_50
	| ATMEL_MPDDRC_TZQIO_4; /* (DDRCK * 20*10^-9) + 1 */
	writel(reg, &mpddr->io_calibr);
	
	/* DDRAM2 Controller initialize */
	ddr2_init(ATMEL_BASE_DDRCS, &ddr2);
	
	udelay(2000);
#if 0
	struct atmel_mpddrc_config ddr2;

	ddr2_conf(&ddr2);

	/* Enable MPDDR clock */
	at91_periph_clk_enable(ATMEL_ID_MPDDRC);
	at91_system_clk_enable(AT91_PMC_DDR);

	/* DDRAM2 Controller initialize */
	ddr2_init(ATMEL_BASE_MPDDRC, ATMEL_BASE_DDRCS, &ddr2);
#endif
}

void at91_pmc_init(void)
{
	struct at91_pmc *pmc = (struct at91_pmc *)ATMEL_BASE_PMC;
	u32 tmp;
	
	tmp = AT91_PMC_PLLAR_29 |
	AT91_PMC_PLLXR_PLLCOUNT(0x3f) |
	AT91_PMC_PLLXR_MUL(43) |
	AT91_PMC_PLLXR_DIV(1);
	at91_plla_init(tmp);
	
	/* init PLLA charge pump */
	writel(0x3 << 8, &pmc->pllicpr);
	
	/* prevents sytem halt after romboot */
	udelay(10);
	
	tmp = AT91_PMC_MCKR_MDIV_4 |
	AT91_PMC_MCKR_CSS_MAIN;
	at91_mck_init(tmp);
	
	tmp = AT91_PMC_MCKR_MDIV_4 |
	AT91_PMC_MCKR_CSS_PLLA;
	at91_mck_init(tmp);
	
	udelay(1000);
	
#if 0
 	u32 tmp;

	tmp = AT91_PMC_PLLAR_29 |
	      AT91_PMC_PLLXR_PLLCOUNT(0x3f) |
	      AT91_PMC_PLLXR_MUL(43) |
	      AT91_PMC_PLLXR_DIV(1);
	at91_plla_init(tmp);

	at91_pllicpr_init(AT91_PMC_IPLL_PLLA(0x3));

	tmp = AT91_PMC_MCKR_MDIV_4 |
	      AT91_PMC_MCKR_CSS_PLLA;
	at91_mck_init(tmp);
#endif
}

#define GMAC_PINS	((0x01 << 8) | (0x01 << 11) | (0x01 << 16) | (0x01 << 18))

#define EMAC_PINS	((0x01 << 7) | (0x01 << 8))

void at91_special_pio_output_low(void)
{
	struct at91_pmc *pmc = (struct at91_pmc *) ATMEL_BASE_PMC;
	struct at91_port *piob = (struct at91_port *) ATMEL_BASE_PIOB;
	struct at91_port *pioc = (struct at91_port *) ATMEL_BASE_PIOC;
	
	writel((1 << ATMEL_ID_PIOB), pmc->pcer);
	
	writel(GMAC_PINS, piob->pudr);
	writel(GMAC_PINS, piob->ppddr);
	writel(GMAC_PINS, piob->per);
	writel(GMAC_PINS, piob->oer);
	writel(GMAC_PINS, piob->codr);
	
	writel((1 << ATMEL_ID_PIOC), pmc->pcer);
	
	writel(EMAC_PINS, pioc->pudr);
	writel(EMAC_PINS, pioc->ppddr);
	writel(EMAC_PINS, pioc->per);
	writel(EMAC_PINS, pioc->oer);
	writel(EMAC_PINS, pioc->codr);
}

#endif /* CONFIG_SPL_BUILD */
