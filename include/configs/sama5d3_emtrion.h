/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Configuration settings for the SAMA5D3 Xplained board.
 *
 * Copyright (C) 2014 Atmel Corporation
 *		      Bo Shen <voice.shen@atmel.com>
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/* ARM asynchronous clock */
#define CONFIG_SYS_AT91_SLOW_CLOCK      32768
#define CONFIG_SYS_AT91_MAIN_CLOCK      12000000 /* from 12 MHz crystal */

#define CONFIG_ARCH_CPU_INIT

#ifndef CONFIG_SPL_BUILD
#define CONFIG_SKIP_LOWLEVEL_INIT
#endif

/* general purpose I/O */
#ifndef CONFIG_DM_GPIO
#define CONFIG_AT91_GPIO
#endif


/*
 * BOOTP options
 */
#define CONFIG_BOOTP_BOOTFILESIZE

/*
 * Command line configuration.
 */

#ifdef CONFIG_SD_BOOT

#ifdef CONFIG_ENV_IS_IN_MMC
/* Use raw reserved sectors to save environment */
#define CONFIG_ENV_OFFSET		0x2000
#define CONFIG_ENV_SIZE			0x1000
#define CONFIG_SYS_MMC_ENV_DEV		0
#else
/* u-boot env in sd/mmc card */
#define CONFIG_ENV_SIZE		0x4000
#endif

#define CONFIG_BOOTCOMMAND	"fatload mmc 0:1 0x21000000 sama5d3_emtrion.dtb; " \
"fatload mmc 0:1 0x22000000 zImage; " \
"bootz 0x22000000 - 0x21000000"

#else

#ifdef CONFIG_NAND_BOOT
/* u-boot env in nand flash */
#define CONFIG_ENV_OFFSET		0x140000
#define CONFIG_ENV_OFFSET_REDUND	0x100000
#define CONFIG_ENV_SIZE			0x20000
#define CONFIG_BOOTCOMMAND		"nand read 0x21000000 0x180000 0x80000;"	\
"nand read 0x22000000 0x200000 0x600000;"	\
"bootz 0x22000000 - 0x21000000"
#elif CONFIG_SPI_BOOT
#error "Serial Flash not setup"
#endif

#endif

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(4 * 1024 * 1024)


#define STUFF_TO_INCLUDE 0

#if STUFF_TO_INCLUDE
/* I2C */

#define CONFIG_ATMEL_LEGACY /* needed for gpio header */
#define CONFIG_SPL_I2C_SUPPORT
#define CONFIG_SYS_I2C_SOFT
#define CONFIG_SYS_I2C

#ifndef __ASSEMBLY__
int get_i2c_sda_pin(void);
int get_i2c_scl_pin(void);
#endif
#define CONFIG_SOFT_I2C_GPIO_SDA	get_i2c_sda_pin()
#define CONFIG_SOFT_I2C_GPIO_SCL	get_i2c_scl_pin()
#define CONFIG_SYS_I2C_SOFT_SPEED	50000
#define CONFIG_SYS_I2C_SOFT_SLAVE	0
#define I2C_DELAY              udelay(5)       /* 1/4 I2C clock duration */
#define CONFIG_SYS_NUM_I2C_ADAPTERS 1
#endif

/*
 * This needs to be defined for the OHCI code to work but it is defined as
 * ATMEL_ID_UHPHS in the CPU specific header files.
 */
#define ATMEL_ID_UHP			32

/*
 * Specify the clock enable bit in the PMC_SCER register.
 */
#define ATMEL_PMC_UHP			(1 <<  6)

/* SDRAM */
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_SYS_SDRAM_BASE           0x20000000
#define CONFIG_SYS_SDRAM_SIZE		0x10000000

#ifdef CONFIG_SPL_BUILD
#define CONFIG_SYS_INIT_SP_ADDR		0x318000
#else
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_SDRAM_BASE + 16 * 1024 - GENERATED_GBL_DATA_SIZE)
#endif

/* NAND flash */
#ifdef CONFIG_CMD_NAND
#define CONFIG_NAND_ATMEL
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		0x60000000
/* our ALE is AD21 */
#define CONFIG_SYS_NAND_MASK_ALE	(1 << 21)
/* our CLE is AD22 */
#define CONFIG_SYS_NAND_MASK_CLE	(1 << 22)
#define CONFIG_SYS_NAND_ONFI_DETECTION
#if STUFF_TO_INCLUDE
#define MTDIDS_DEFAULT     "nand0=atmel_nand"
#define MTDPARTS_DEFAULT   "mtdparts=atmel_nand:256k@0(bootstrap),512k@256k(uboot),256k@768k(env),511m@1m(rootfs)"
#endif
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#endif
/* PMECC & PMERRLOC */
#define CONFIG_ATMEL_NAND_HWECC
#define CONFIG_ATMEL_NAND_HW_PMECC
#define CONFIG_PMECC_CAP		4
#define CONFIG_PMECC_SECTOR_SIZE	512

/* USB */

#ifdef CONFIG_CMD_USB
#define CONFIG_USB_ATMEL
#define CONFIG_USB_ATMEL_CLK_SEL_UPLL
#define CONFIG_USB_OHCI_NEW
#define CONFIG_SYS_USB_OHCI_CPU_INIT
#define CONFIG_SYS_USB_OHCI_REGS_BASE		0x00600000
#define CONFIG_SYS_USB_OHCI_SLOT_NAME		"SAMA5D3 Emtrion"
#define CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS	2
#endif

#define CONFIG_SYS_LOAD_ADDR			0x22000000 /* load address */

/* SPL */
#define CONFIG_SPL_TEXT_BASE		0x300000
#define CONFIG_SPL_MAX_SIZE		0x18000
#define CONFIG_SPL_BSS_START_ADDR	0x20000000
#define CONFIG_SPL_BSS_MAX_SIZE		0x80000
#define CONFIG_SYS_SPL_MALLOC_START	0x20080000
#define CONFIG_SYS_SPL_MALLOC_SIZE	0x80000

#define CONFIG_SYS_MONITOR_LEN		(512 << 10)

#ifdef CONFIG_SD_BOOT
#define CONFIG_SYS_MMCSD_FS_BOOT_PARTITION	1
#define CONFIG_SPL_FS_LOAD_PAYLOAD_NAME	"u-boot.img"

#elif CONFIG_NAND_BOOT
#define CONFIG_SPL_NAND_DRIVERS
#define CONFIG_SPL_NAND_BASE
#define CONFIG_SYS_NAND_U_BOOT_OFFS	0x40000
#define CONFIG_SYS_NAND_5_ADDR_CYCLE
#define CONFIG_SYS_NAND_PAGE_SIZE	0x800
#define CONFIG_SYS_NAND_PAGE_COUNT	64
#define CONFIG_SYS_NAND_OOBSIZE		64
#define CONFIG_SYS_NAND_BLOCK_SIZE	0x20000
#define CONFIG_SYS_NAND_BAD_BLOCK_POS	0x0
#define CONFIG_SPL_GENERATE_ATMEL_PMECC_HEADER
#endif
#endif
