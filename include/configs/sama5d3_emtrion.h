/*
 * Configuration settings for the emtrion sbc-sama5d36 board
 *
 * Copyright (C) 2014 emtrion GmbH
 *		      Anthony Harivel <anthony.harivel@emtrion.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <asm/hardware.h>

//#define CONFIG_SYS_TEXT_BASE		0x26f00000

/* ARM asynchronous clock */
#define CONFIG_SYS_AT91_SLOW_CLOCK      32768
#define CONFIG_SYS_AT91_MAIN_CLOCK      12000000 /* from 12 MHz crystal */

#define CONFIG_ARCH_CPU_INIT

#ifndef CONFIG_SPL_BUILD
#define CONFIG_SKIP_LOWLEVEL_INIT
#endif

//#define CONFIG_BOARD_EARLY_INIT_F
//#define CONFIG_DISPLAY_CPUINFO

//#define CONFIG_CMD_BOOTZ
//#define CONFIG_OF_LIBFDT		/* Device Tree support */
//#define CONFIG_OF_BOARD_SETUP
//#define CONFIG_SYS_GENERIC_BOARD

/* general purpose I/O */
//#ifndef CONFIG_DM_GPIO
//#define CONFIG_AT91_GPIO
//#endif
//#define CONFIG_CMD_GPIO

/* serial console */
//#define CONFIG_ATMEL_USART
#define CONFIG_USART_BASE		ATMEL_BASE_DBGU
#define CONFIG_USART_ID			ATMEL_ID_DBGU


/*
 * This needs to be defined for the OHCI code to work but it is defined as
 * ATMEL_ID_UHPHS in the CPU specific header files.
 */
#define ATMEL_ID_UHP			ATMEL_ID_UHPHS

/*
 * Specify the clock enable bit in the PMC_SCER register.
 */
#define ATMEL_PMC_UHP			AT91SAM926x_PMC_UHP

//#define CONFIG_BOOTDELAY		3

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME

/* No NOR flash */
// #define CONFIG_SYS_NO_FLASH // Does not exist

/*
 * Command line configuration.
 */
#include <config_cmd_default.h>
#undef CONFIG_CMD_FPGA
#undef CONFIG_CMD_IMI
#undef CONFIG_CMD_LOADS
//#define CONFIG_CMD_PING
//#define CONFIG_CMD_DHCP
//#define CONFIG_CMD_I2C
//#define CONFIG_CMD_SF

/* I2C */

#define CONFIG_ATMEL_LEGACY /* needed for gpio header */
#define CONFIG_SPL_I2C_SUPPORT
#define CONFIG_SYS_I2C_SOFT
#define CONFIG_SYS_I2C

#ifndef __ASSEMBLY__
int get_i2c_sda_pin(void);
int get_i2c_scl_pin(void);
#endif
#define CONFIG_SOFT_I2C_GPIO_SDA	get_i2c_sda_pin() // doesn't exist
#define CONFIG_SOFT_I2C_GPIO_SCL	get_i2c_scl_pin() // doesn't exist
#define CONFIG_SYS_I2C_SOFT_SPEED	50000
#define CONFIG_SYS_I2C_SOFT_SLAVE	0
#define I2C_DELAY              udelay(5)       /* 1/4 I2C clock duration */
#define CONFIG_SYS_NUM_I2C_ADAPTERS 1

/* SDRAM */
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_SYS_SDRAM_BASE           ATMEL_BASE_DDRCS
#define CONFIG_SYS_SDRAM_SIZE		0x10000000

#ifdef CONFIG_SPL_BUILD
#define CONFIG_SYS_INIT_SP_ADDR		0x310000
#else
#define CONFIG_SYS_INIT_SP_ADDR \
#error "CONFIG_SPL_BUILD not defined"
(CONFIG_SYS_SDRAM_BASE + 4 * 1024 - GENERATED_GBL_DATA_SIZE)
#endif

#ifdef CONFIG_CMD_SF
//#define CONFIG_ATMEL_SPI
//#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_MACRONIX
#define CONFIG_SPI_FLASH_SPANSION
#define CONFIG_SPI_FLASH_STMICRO
#define CONFIG_SF_DEFAULT_SPEED		30000000
#define CONFIG_SF_DEFAULT_MODE		SPI_MODE_3
#else
#error "CONFIG_CMD_SF not defined"
#endif

/* NAND flash */
//#define CONFIG_CMD_NAND

#ifdef CONFIG_CMD_NAND
#define CONFIG_NAND_ATMEL
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		ATMEL_BASE_CS3
/* our ALE is AD21 */
#define CONFIG_SYS_NAND_MASK_ALE	(1 << 21)
/* our CLE is AD22 */
#define CONFIG_SYS_NAND_MASK_CLE	(1 << 22)
#define CONFIG_SYS_NAND_ONFI_DETECTION

/* PMECC & PMERRLOC */
#define CONFIG_ATMEL_NAND_HWECC
#define CONFIG_ATMEL_NAND_HW_PMECC
#define CONFIG_PMECC_CAP		4
#define CONFIG_PMECC_SECTOR_SIZE	512
//#define CONFIG_CMD_NAND_TRIMFFS
#define CONFIG_CMD_MTDPARTS

#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
//#define MTDIDS_DEFAULT     "nand0=atmel_nand"
//#define MTDPARTS_DEFAULT   "mtdparts=atmel_nand:256k@0(bootstrap),512k@256k(uboot),256k@768k(env),511m@1m(rootfs)"
#define CONFIG_RBTREE
#define CONFIG_LZO
//#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#endif

/* Ethernet Hardware */
//#define CONFIG_MACB
#define CONFIG_RMII
#define CONFIG_NET_MULTI
#define CONFIG_NET_RETRY_COUNT		20
#define CONFIG_RGMII
//#define CONFIG_CMD_MII
#define CONFIG_PHYLIB

/* MMC */
//#define CONFIG_CMD_MMC

#ifdef CONFIG_CMD_MMC
#define CONFIG_MMC
#define CONFIG_GENERIC_MMC
//#define CONFIG_GENERIC_ATMEL_MCI
#endif

/* additional stuff */
#define CONFIG_CMD_CACHE
/*#define CONFIG_CMD_MD5SUM*/

/* USB */
//#define CONFIG_CMD_USB

#ifdef CONFIG_CMD_USB
#define CONFIG_USB_ATMEL
#define CONFIG_USB_ATMEL_CLK_SEL_UPLL
#define CONFIG_USB_OHCI_NEW
#define CONFIG_SYS_USB_OHCI_CPU_INIT
#define CONFIG_SYS_USB_OHCI_REGS_BASE		ATMEL_BASE_OHCI
#define CONFIG_SYS_USB_OHCI_SLOT_NAME		"emtrion SBC-SAMA5D36"
#define CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS	2
#define CONFIG_DOS_PARTITION
//#define CONFIG_USB_STORAGE
#endif

#if defined(CONFIG_CMD_USB) || defined(CONFIG_CMD_MMC)
//#define CONFIG_CMD_FAT
//#define CONFIG_FAT_WRITE
//#define CONFIG_CMD_EXT4
//#define CONFIG_CMD_EXT4_WRITE
#endif

#define CONFIG_SYS_LOAD_ADDR			0x22000000 /* load address */

#if CONFIG_NAND_BOOT
/* bootstrap + u-boot + env in nandflash */
//#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_OVERWRITE
#define CONFIG_ENV_OFFSET		0xc0000
#define CONFIG_ENV_SIZE			0x20000
#define CONFIG_EXTRA_ENV_SETTINGS \
"console=ttyS0,115200 earlyprintk\0" \
"loadaddr=0x22000000\0" \
"fdt_addr=0x21000000\0" \
"bootdir=/boot\0" \
"bootfile=zImage\0" \
"ip-method=dhcp\0" \
"bootdelay=3\0" \
"configure-ip=if test -n \"${ip-method}\"; then if test \"${ip-method}\" = dhcp; then setenv ip dhcp && setenv autoload no && dhcp ; elif test \"${ip-method}\" = static; then if test -n \"${ipaddr}\" && test -n \"${serverip}\" && test -n \"${netmask}\"; then setenv ip ${ipaddr}:${serverip}:${gatewayip}:${netmask}:${hostname}:eth0:off; else echo You have to set ipaddr, netmask and serverip when using ip-method static. ;  false; fi; else echo ip-method has to be either dhcp or static. ; false ; fi; else echo ip-method has to be either dhcp or static. ; false ; fi\0" \
"test-nfsroot=if test -n \"${nfsroot}\"; then true ; else echo Please set nfsroot variable. ; false ; fi\0" \
"net_boot=run configure-ip && if test -n \"${tftproot}\"; then tftp ${tftproot}/boot/uboot_script; else run test-nfsroot && nfs ${nfsroot}/boot/uboot_script; fi && env import -t ${loadaddr} ${filesize} && if test -n \"${uboot_script_net_boot}\"; then run uboot_script_net_boot; else echo Bootscript does not define uboot_script_net_boot, aborting. ; fi\0" \
"flash_boot=setenv result 0; while test \"0\" -eq ${result}; do mtdparts default && ubi part rootfs && ubifsmount ubi0:rootfs && ubifsload ${loadaddr} /boot/uboot_script ; && setenv result 1; done; env import -t ${loadaddr} ${filesize} && if test -n \"${uboot_script_flash_boot}\" ; then run uboot_script_flash_boot ; else echo Bootscript does not define uboot_script_flash_boot, aborting. ; fi\0" \
"update_uboot=if test -n \"${serverip}\"; then run configure-ip && tftp ${image.bootstrap} && mtdparts default && nand erase.part bootstrap && nand write ${loadaddr} bootstrap ${filesize} && tftp ${image.uboot} && nand erase.part uboot && nand write ${loadaddr} uboot ${filesize} && echo Update U-Boot successful; else echo Please set serverip variable. ; fi\0" \
"update_kernel=if test -n \"${serverip}\"; then run configure-ip && if test -n \"${tftproot}\"; then tftp ${tftproot}/boot/uboot_script; else run test-nfsroot && nfs ${nfsroot}/boot/uboot_script; fi && env import -t ${loadaddr} ${filesize} && if test -n \"${uboot_script_update_kernel}\"; then run uboot_script_update_kernel; else echo Bootscript does not define uboot_script_update kernel, aborting ; fi ; else echo Please set serverip variable. ; fi\0" \
"update_rootfs=if test -n \"${serverip}\"; then run configure-ip && if test -n \"${tftproot}\"; then tftp ${tftproot}/boot/uboot_script; else run test-nfsroot && nfs ${nfsroot}/boot/uboot_script; fi && env import -t ${loadaddr} ${filesize} && if test -n \"${uboot_script_update_rootfs}\" ; then run uboot_script_update_rootfs; else echo Bootscript does not define update_rootfs, aborting ; fi ; else echo Please set serverip variable. ; fi\0" \
"restore_sys=if test -n \"${serverip}\"; then run configure-ip && if test -n \"${tftproot}\"; then tftp ${tftproot}/boot/uboot_script; else run test-nfsroot && nfs ${nfsroot}/boot/uboot_script; fi && env import -t ${loadaddr} ${filesize} && if test -n \"${uboot_script_restore_sys}\"; then run uboot_script_restore_sys; else echo Bootscript does not define uboot_script_restore_sys, aborting. ; fi ; else echo Please set serverip variable. ; fi\0"


#elif CONFIG_SD_BOOT
/* bootstrap + u-boot + env in sd card */
#define CONFIG_ENV_IS_IN_MMC
#define CONFIG_ENV_OFFSET	0x2000
#define CONFIG_ENV_SIZE		0x1000
#define CONFIG_BOOTCOMMAND	"fatload mmc 0:1 0x21000000 sbc-sama5d36.dtb; " \
"fatload mmc 0:1 0x22000000 zImage; " \
"bootz 0x22000000 - 0x21000000"
#define CONFIG_SYS_MMC_ENV_DEV	0
#else
#define CONFIG_ENV_IS_NOWHERE
#endif

//#ifdef CONFIG_SD_BOOT
//#define CONFIG_BOOTARGS							\
//"console=ttyS0,115200 earlyprintk "				\
//"root=/dev/mmcblk0p2 rw rootwait"
//#else
//#define CONFIG_BOOTARGS							\
//"console=ttyS0,115200 earlyprintk "				\
//"rootfstype=ubifs ubi.mtd=3 root=ubi0:rootfs rw rootwait"
//#endif

#define CONFIG_BAUDRATE			115200

#define CONFIG_SYS_PROMPT		"U-Boot> "
#define CONFIG_SYS_CBSIZE		1024
#define CONFIG_SYS_MAXARGS		16
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_LONGHELP
#define CONFIG_CMDLINE_EDITING
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_HUSH_PARSER

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(4 * 1024 * 1024)

/* SPL */
//#define CONFIG_SPL
//#define CONFIG_SPL_FRAMEWORK
#define CONFIG_SPL_TEXT_BASE		0x300000
#define CONFIG_SPL_MAX_SIZE		0x10000
#define CONFIG_SPL_BSS_START_ADDR	0x20000000
#define CONFIG_SPL_BSS_MAX_SIZE		0x80000
#define CONFIG_SYS_SPL_MALLOC_START	0x20080000
#define CONFIG_SYS_SPL_MALLOC_SIZE	0x80000

//#define CONFIG_SPL_LIBCOMMON_SUPPORT
//#define CONFIG_SPL_LIBGENERIC_SUPPORT
//#define CONFIG_SPL_GPIO_SUPPORT
//#define CONFIG_SPL_SERIAL_SUPPORT

#define CONFIG_SPL_BOARD_INIT
#define CONFIG_SYS_MONITOR_LEN		(512 << 10)

#ifdef CONFIG_SD_BOOT
#define CONFIG_SPL_LDSCRIPT		arch/arm/cpu/at91-common/u-boot-spl.lds
#define CONFIG_SPL_MMC_SUPPORT
#define CONFIG_SYS_U_BOOT_MAX_SIZE_SECTORS	0x400
#define CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR 0x200
#define CONFIG_SYS_MMC_SD_FAT_BOOT_PARTITION	1
#define CONFIG_SPL_FAT_LOAD_PAYLOAD_NAME	"u-boot.img"
#define CONFIG_SPL_FAT_SUPPORT
#define CONFIG_SPL_LIBDISK_SUPPORT

#elif CONFIG_NAND_BOOT
//#define CONFIG_SPL_NAND_SUPPORT
#define CONFIG_SPL_NAND_DRIVERS
#define CONFIG_SPL_NAND_BASE
#define CONFIG_SYS_NAND_U_BOOT_OFFS	0x40000
#define CONFIG_SYS_NAND_5_ADDR_CYCLE
#define CONFIG_SYS_NAND_PAGE_SIZE	0x800
#define CONFIG_SYS_NAND_PAGE_COUNT	64
#define CONFIG_SYS_NAND_OOBSIZE		64
#define CONFIG_SYS_NAND_BLOCK_SIZE	0x20000
#define CONFIG_SYS_NAND_BAD_BLOCK_POS	0x0
#define CONFIG_SYS_NAND_USE_FLASH_BBT

#endif

#endif

