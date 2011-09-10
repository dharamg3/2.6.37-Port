/* linux/arch/arm/plat-s3c/include/plat/sdhci.h
 *
 * Copyright 2008 Openmoko, Inc.
 * Copyright 2008 Simtec Electronics
 *	http://armlinux.simtec.co.uk/
 *	Ben Dooks <ben@simtec.co.uk>
 *
 * S3C Platform - SDHCI (HSMMC) platform data definitions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __PLAT_S3C_SDHCI_H
#define __PLAT_S3C_SDHCI_H __FILE__

struct platform_device;
struct mmc_host;
struct mmc_card;
struct mmc_ios;

enum cd_types {
	S3C_SDHCI_CD_INTERNAL,	/* use mmc internal CD line */
	S3C_SDHCI_CD_EXTERNAL,	/* use external callback */
	S3C_SDHCI_CD_GPIO,	/* use external gpio pin for CD line */
	S3C_SDHCI_CD_NONE,	/* no CD line, use polling to detect card */
	S3C_SDHCI_CD_PERMANENT,	/* no CD line, card permanently wired to host */
};

enum clk_types {
	S3C_SDHCI_CLK_DIV_INTERNAL,	/* use mmc internal clock divider */
	S3C_SDHCI_CLK_DIV_EXTERNAL,	/* use external clock divider */
};

#define MAX_BUS_CLK	(3)

/*struct sdhci_s3c {
	struct sdhci_host	*host;
	struct platform_device	*pdev;
	struct resource		*ioarea;
	struct s3c_sdhci_platdata *pdata;
	unsigned int		cur_clk;

	struct clk		*clk_io;	// clock for io bus 
	struct clk		*clk_bus[MAX_BUS_CLK];
};
*/
/**
 * struct s3c_sdhci_platdata() - Platform device data for Samsung SDHCI
 * @max_width: The maximum number of data bits supported.
 * @host_caps: Standard MMC host capabilities bit field.
 * @cfg_gpio: Configure the GPIO for a specific card bit-width
 * @cfg_card: Configure the interface for a specific card and speed. This
 *            is necessary the controllers and/or GPIO blocks require the
 *	      changing of driver-strength and other controls dependant on
 *	      the card and speed of operation.
 *
 * Initialisation data specific to either the machine or the platform
 * for the device driver to use or call-back when configuring gpio or
 * card speed information.
*/
struct s3c_sdhci_platdata {
	unsigned int	max_width;
	unsigned int	host_caps;
	enum cd_types	cd_type;
	enum clk_types	clk_type;

	char		**clocks;	/* set of clock sources */

	int		ext_cd_gpio;
	bool		ext_cd_gpio_invert;
	int	(*ext_cd_init)(void (*notify_func)(struct platform_device *,
						   int state));
	int	(*ext_cd_cleanup)(void (*notify_func)(struct platform_device *,
						      int state));

	void	(*cfg_gpio)(struct platform_device *dev, int width);
	void	(*cfg_card)(struct platform_device *dev,
			    void __iomem *regbase,
			    struct mmc_ios *ios,
			    struct mmc_card *card);
	struct sdhci_host*	sdhci_host;
};

extern void sdhci_s3c_force_presence_change(struct platform_device *pdev);

/**
 * s3c_sdhci0_set_platdata - Set platform data for S3C SDHCI device.
 * @pd: Platform data to register to device.
 *
 * Register the given platform data for use withe S3C SDHCI device.
 * The call will copy the platform data, so the board definitions can
 * make the structure itself __initdata.
 */
extern void s3c_sdhci0_set_platdata(struct s3c_sdhci_platdata *pd);
extern void s3c_sdhci1_set_platdata(struct s3c_sdhci_platdata *pd);
extern void s3c_sdhci2_set_platdata(struct s3c_sdhci_platdata *pd);


/* Default platform data, exported so that per-cpu initialisation can
 * set the correct one when there are more than one cpu type selected.
*/

extern struct s3c_sdhci_platdata s3c_hsmmc0_def_platdata;
extern struct s3c_sdhci_platdata s3c_hsmmc1_def_platdata;
extern struct s3c_sdhci_platdata s3c_hsmmc2_def_platdata;

/* Helper function availablity */

#if defined (CONFIG_S3C6410_SETUP_SDHCI) || defined (CONFIG_S5PC1XX_SETUP_SDHCI) \
	 || defined (CONFIG_S5P6442_SETUP_SDHCI) || defined (CONFIG_S5PC11X_SETUP_SDHCI)
extern char *s5p6442_hsmmc_clksrcs[3];

extern void s5p6442_setup_sdhci0_cfg_gpio(struct platform_device *, int w);
extern void s5p6442_setup_sdhci1_cfg_gpio(struct platform_device *, int w);
extern void s5p6442_setup_sdhci2_cfg_gpio(struct platform_device *, int w);

extern void s5p6442_setup_sdhci0_cfg_card(struct platform_device *dev,
					   void __iomem *r,
					   struct mmc_ios *ios,
					   struct mmc_card *card);

#ifdef CONFIG_S3C_DEV_HSMMC
static inline void s5p6442_default_sdhci0(void)
{
	s3c_hsmmc0_def_platdata.clocks = s5p6442_hsmmc_clksrcs;
	s3c_hsmmc0_def_platdata.cfg_gpio = s5p6442_setup_sdhci0_cfg_gpio;
	s3c_hsmmc0_def_platdata.cfg_card = s5p6442_setup_sdhci0_cfg_card;
}
#else
static inline void s5p6442_default_sdhci0(void) { }
#endif /* CONFIG_S3C_DEV_HSMMC */

#ifdef CONFIG_S3C_DEV_HSMMC1
static inline void s5p6442_default_sdhci1(void)
{
	s3c_hsmmc1_def_platdata.clocks = s5p6442_hsmmc_clksrcs;
	s3c_hsmmc1_def_platdata.cfg_gpio = s5p6442_setup_sdhci1_cfg_gpio;
	s3c_hsmmc1_def_platdata.cfg_card = s5p6442_setup_sdhci0_cfg_card;
}
#else
static inline void s5p6442_default_sdhci1(void) { }
#endif /* CONFIG_S3C_DEV_HSMMC1 */

#ifdef CONFIG_S3C_DEV_HSMMC2
static inline void s5p6442_default_sdhci2(void)
{
	s3c_hsmmc2_def_platdata.clocks = s5p6442_hsmmc_clksrcs;
	s3c_hsmmc2_def_platdata.cfg_gpio = s5p6442_setup_sdhci2_cfg_gpio;
	s3c_hsmmc2_def_platdata.cfg_card = s5p6442_setup_sdhci0_cfg_card;
}
#else
static inline void s5p6442_default_sdhci2(void) { }
#endif /* CONFIG_S3C_DEV_HSMMC2*/

#else
static inline void s5p6442_default_sdhci0(void) { }
static inline void s5p6442_default_sdhci1(void) { }
static inline void s5p6442_default_sdhci2(void) { }
#endif /* CONFIG_S3C6410_SETUP_SDHCI */

#endif /* __PLAT_S3C_SDHCI_H */
