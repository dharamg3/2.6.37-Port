/*
 *  linux/arch/arm/kernel/time.c
 *
 *  Copyright (C) 1991, 1992, 1995  Linus Torvalds
 *  Modifications for ARM (C) 1994-2001 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  This file contains the ARM-specific time handling details:
 *  reading the RTC at bootup, etc...
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/smp.h>
#include <linux/timex.h>
#include <linux/errno.h>
#include <linux/profile.h>
#include <linux/sysdev.h>
#include <linux/timer.h>
#include <linux/irq.h>

#include <linux/mc146818rtc.h>

#include <asm/leds.h>
#include <asm/thread_info.h>
#include <asm/stacktrace.h>
#include <asm/mach/time.h>

/*
 * Our system timer.
 */
struct sys_timer *system_timer;

#if defined(CONFIG_RTC_DRV_CMOS) || defined(CONFIG_RTC_DRV_CMOS_MODULE)
/* this needs a better home */
DEFINE_SPINLOCK(rtc_lock);

#ifdef CONFIG_RTC_DRV_CMOS_MODULE
EXPORT_SYMBOL(rtc_lock);
#endif
#endif	/* pc-style 'CMOS' RTC support */

/* change this if you have some constant time drift */
#define USECS_PER_JIFFY	(1000000/HZ)

#ifdef CONFIG_SMP
unsigned long profile_pc(struct pt_regs *regs)
{
	struct stackframe frame;

	if (!in_lock_functions(regs->ARM_pc))
		return regs->ARM_pc;

	frame.fp = regs->ARM_fp;
	frame.sp = regs->ARM_sp;
	frame.lr = regs->ARM_lr;
	frame.pc = regs->ARM_pc;
	do {
		int ret = unwind_frame(&frame);
		if (ret < 0)
			return 0;
	} while (in_lock_functions(frame.pc));

	return frame.pc;
}
EXPORT_SYMBOL(profile_pc);
#endif

#ifdef CONFIG_ARCH_USES_GETTIMEOFFSET
u32 arch_gettimeoffset(void)
{
	if (system_timer->offset != NULL)
		return system_timer->offset() * 1000;

	return 0;
}
#endif /* CONFIG_ARCH_USES_GETTIMEOFFSET */

#ifdef CONFIG_LEDS_TIMER
static inline void do_leds(void)
{
	static unsigned int count = HZ/2;

	if (--count == 0) {
		count = HZ/2;
		leds_event(led_timer);
	}
}
#else
#define	do_leds()
#endif


#ifndef CONFIG_GENERIC_CLOCKEVENTS
/*
 * Kernel system timer support.
 */
void timer_tick(void)
{
	profile_tick(CPU_PROFILING);
	do_leds();
	write_seqlock(&xtime_lock);
	do_timer(1);
	write_sequnlock(&xtime_lock);
#ifndef CONFIG_SMP
	update_process_times(user_mode(get_irq_regs()));
#endif
}
#endif

#if defined(CONFIG_PM) && !defined(CONFIG_GENERIC_CLOCKEVENTS)
static int timer_suspend(struct sys_device *dev, pm_message_t state)
{
	struct sys_timer *timer = container_of(dev, struct sys_timer, dev);

	if (timer->suspend != NULL)
		timer->suspend();

	return 0;
}

static int timer_resume(struct sys_device *dev)
{
	struct sys_timer *timer = container_of(dev, struct sys_timer, dev);

	if (timer->resume != NULL)
		timer->resume();

	return 0;
}
#else
#define timer_suspend NULL
#define timer_resume NULL
#endif

/**
 * save_time_delta - Save the offset between system time and RTC time
 * @delta: pointer to timespec to store delta
 * @rtc: pointer to timespec for current RTC time
 *
 * Return a delta between the system time and the RTC time, such
 * that system time can be restored later with restore_time_delta()
 */
void save_time_delta(struct timespec *delta, struct timespec *rtc)
{
	set_normalized_timespec(delta,
				xtime.tv_sec - rtc->tv_sec,
				xtime.tv_nsec - rtc->tv_nsec);
}
EXPORT_SYMBOL(save_time_delta);

/**
 * restore_time_delta - Restore the current system time
 * @delta: delta returned by save_time_delta()
 * @rtc: pointer to timespec for current RTC time
 */
void restore_time_delta(struct timespec *delta, struct timespec *rtc)
{
	struct timespec ts;

	set_normalized_timespec(&ts,
				delta->tv_sec + rtc->tv_sec,
				delta->tv_nsec + rtc->tv_nsec);

	do_settimeofday(&ts);
}
EXPORT_SYMBOL(restore_time_delta);


static struct sysdev_class timer_sysclass = {
	.name		= "timer",
	.suspend	= timer_suspend,
	.resume		= timer_resume,
};

static int __init timer_init_sysfs(void)
{
	int ret = sysdev_class_register(&timer_sysclass);
	if (ret == 0) {
		system_timer->dev.cls = &timer_sysclass;
		ret = sysdev_register(&system_timer->dev);
	}

	return ret;
}

device_initcall(timer_init_sysfs);

void __init time_init(void)
{
	system_timer->init();
}

