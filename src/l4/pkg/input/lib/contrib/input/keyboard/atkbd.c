/*
 * AT and PS/2 keyboard driver
 *
 * Copyright (c) 1999-2002 Vojtech Pavlik
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */

/*
 * This driver can handle standard AT keyboards and PS/2 keyboards in
 * Translated and Raw Set 2 and Set 3, as well as AT keyboards on dumb
 * input-only controllers and AT keyboards connected over a one way RS232
 * converter.
 */

#include <linux/delay.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/serio.h>
#include <linux/workqueue.h>
#include <linux/libps2.h>

#define DRIVER_DESC	"AT and PS/2 keyboard driver"

MODULE_AUTHOR("Vojtech Pavlik <vojtech@suse.cz>");
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");

static int atkbd_set = 2;
module_param_named(set, atkbd_set, int, 0);
MODULE_PARM_DESC(set, "Select keyboard code set (2 = default, 3 = PS/2 native)");

#if defined(__i386__) || defined(__x86_64__) || defined(__hppa__)
static int atkbd_reset;
#else
static int atkbd_reset = 1;
#endif
module_param_named(reset, atkbd_reset, bool, 0);
MODULE_PARM_DESC(reset, "Reset keyboard during initialization");

#ifndef L4INPUT
static int atkbd_softrepeat;
#else
static int atkbd_softrepeat = 1;
#endif
module_param_named(softrepeat, atkbd_softrepeat, bool, 0);
MODULE_PARM_DESC(softrepeat, "Use software keyboard repeat");

static int atkbd_softraw = 1;
module_param_named(softraw, atkbd_softraw, bool, 0);
MODULE_PARM_DESC(softraw, "Use software generated rawmode");

static int atkbd_scroll;
module_param_named(scroll, atkbd_scroll, bool, 0);
MODULE_PARM_DESC(scroll, "Enable scroll-wheel on MS Office and similar keyboards");

static int atkbd_extra;
module_param_named(extra, atkbd_extra, bool, 0);
MODULE_PARM_DESC(extra, "Enable extra LEDs and keys on IBM RapidAcces, EzKey and similar keyboards");

__obsolete_setup("atkbd_set=");
__obsolete_setup("atkbd_reset");
__obsolete_setup("atkbd_softrepeat=");

/*
 * Scancode to keycode tables. These are just the default setting, and
 * are loadable via an userland utility.
 */

#if defined(__hppa__)
#include "hpps2atkbd.h"
#else

static unsigned char atkbd_set2_keycode[512] = {

	  0, 67, 65, 63, 61, 59, 60, 88,  0, 68, 66, 64, 62, 15, 41,117,
	  0, 56, 42, 93, 29, 16,  2,  0,  0,  0, 44, 31, 30, 17,  3,  0,
	  0, 46, 45, 32, 18,  5,  4, 95,  0, 57, 47, 33, 20, 19,  6,183,
	  0, 49, 48, 35, 34, 21,  7,184,  0,  0, 50, 36, 22,  8,  9,185,
	  0, 51, 37, 23, 24, 11, 10,  0,  0, 52, 53, 38, 39, 25, 12,  0,
	  0, 89, 40,  0, 26, 13,  0,  0, 58, 54, 28, 27,  0, 43,  0, 85,
	  0, 86, 91, 90, 92,  0, 14, 94,  0, 79,124, 75, 71,121,  0,  0,
	 82, 83, 80, 76, 77, 72,  1, 69, 87, 78, 81, 74, 55, 73, 70, 99,

	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	217,100,255,  0, 97,165,  0,  0,156,  0,  0,  0,  0,  0,  0,125,
	173,114,  0,113,  0,  0,  0,126,128,  0,  0,140,  0,  0,  0,127,
	159,  0,115,  0,164,  0,  0,116,158,  0,150,166,  0,  0,  0,142,
	157,  0,  0,  0,  0,  0,  0,  0,155,  0, 98,  0,  0,163,  0,  0,
	226,  0,  0,  0,  0,  0,  0,  0,  0,255, 96,  0,  0,  0,143,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,107,  0,105,102,  0,  0,112,
	110,111,108,112,106,103,  0,119,  0,118,109,  0, 99,104,119,  0,

	  0,  0,  0, 65, 99,
};

#endif

static unsigned char atkbd_set3_keycode[512] = {

	  0,  0,  0,  0,  0,  0,  0, 59,  1,138,128,129,130, 15, 41, 60,
	131, 29, 42, 86, 58, 16,  2, 61,133, 56, 44, 31, 30, 17,  3, 62,
	134, 46, 45, 32, 18,  5,  4, 63,135, 57, 47, 33, 20, 19,  6, 64,
	136, 49, 48, 35, 34, 21,  7, 65,137,100, 50, 36, 22,  8,  9, 66,
	125, 51, 37, 23, 24, 11, 10, 67,126, 52, 53, 38, 39, 25, 12, 68,
	113,114, 40, 43, 26, 13, 87, 99, 97, 54, 28, 27, 43, 43, 88, 70,
	108,105,119,103,111,107, 14,110,  0, 79,106, 75, 71,109,102,104,
	 82, 83, 80, 76, 77, 72, 69, 98,  0, 96, 81,  0, 78, 73, 55,183,

	184,185,186,187, 74, 94, 92, 93,  0,  0,  0,125,126,127,112,  0,
	  0,139,150,163,165,115,152,150,166,140,160,154,113,114,167,168,
	148,149,147,140
};

static unsigned char atkbd_unxlate_table[128] = {
          0,118, 22, 30, 38, 37, 46, 54, 61, 62, 70, 69, 78, 85,102, 13,
         21, 29, 36, 45, 44, 53, 60, 67, 68, 77, 84, 91, 90, 20, 28, 27,
         35, 43, 52, 51, 59, 66, 75, 76, 82, 14, 18, 93, 26, 34, 33, 42,
         50, 49, 58, 65, 73, 74, 89,124, 17, 41, 88,  5,  6,  4, 12,  3,
         11,  2, 10,  1,  9,119,126,108,117,125,123,107,115,116,121,105,
        114,122,112,113,127, 96, 97,120,  7, 15, 23, 31, 39, 47, 55, 63,
         71, 79, 86, 94,  8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 87,111,
         19, 25, 57, 81, 83, 92, 95, 98, 99,100,101,103,104,106,109,110
};

#define ATKBD_CMD_SETLEDS	0x10ed
#define ATKBD_CMD_GSCANSET	0x11f0
#define ATKBD_CMD_SSCANSET	0x10f0
#define ATKBD_CMD_GETID		0x02f2
#define ATKBD_CMD_SETREP	0x10f3
#define ATKBD_CMD_ENABLE	0x00f4
#define ATKBD_CMD_RESET_DIS	0x00f5
#define ATKBD_CMD_SETALL_MBR	0x00fa
#define ATKBD_CMD_RESET_BAT	0x02ff
#define ATKBD_CMD_RESEND	0x00fe
#define ATKBD_CMD_EX_ENABLE	0x10ea
#define ATKBD_CMD_EX_SETLEDS	0x20eb
#define ATKBD_CMD_OK_GETID	0x02e8


#define ATKBD_RET_ACK		0xfa
#define ATKBD_RET_NAK		0xfe
#define ATKBD_RET_BAT		0xaa
#define ATKBD_RET_EMUL0		0xe0
#define ATKBD_RET_EMUL1		0xe1
#define ATKBD_RET_RELEASE	0xf0
#define ATKBD_RET_HANGUEL	0xf1
#define ATKBD_RET_HANJA		0xf2
#define ATKBD_RET_ERR		0xff

#define ATKBD_KEY_UNKNOWN	  0
#define ATKBD_KEY_NULL		255

#define ATKBD_SCR_1		254
#define ATKBD_SCR_2		253
#define ATKBD_SCR_4		252
#define ATKBD_SCR_8		251
#define ATKBD_SCR_CLICK		250

#define ATKBD_SPECIAL		250

static unsigned char atkbd_scroll_keys[5][2] = {
	{ ATKBD_SCR_1,     0x45 },
	{ ATKBD_SCR_2,     0x29 },
	{ ATKBD_SCR_4,     0x36 },
	{ ATKBD_SCR_8,     0x27 },
	{ ATKBD_SCR_CLICK, 0x60 },
};

/*
 * The atkbd control structure
 */

struct atkbd {

	struct ps2dev	ps2dev;

	/* Written only during init */
	char name[64];
	char phys[40];
	struct input_dev dev;

	unsigned short id;
	unsigned char keycode[512];
	unsigned char set;
	unsigned char translated;
	unsigned char extra;
	unsigned char write;
	unsigned char softrepeat;
	unsigned char softraw;
	unsigned char scroll;
	unsigned char enabled;

	/* Accessed only from interrupt */
	unsigned char emul;
	unsigned char resend;
	unsigned char release;
	unsigned char bat_xl;
	unsigned int last;
	unsigned long time;
};

#ifndef L4INPUT

static ssize_t atkbd_attr_show_helper(struct device *dev, char *buf,
				ssize_t (*handler)(struct atkbd *, char *));
static ssize_t atkbd_attr_set_helper(struct device *dev, const char *buf, size_t count,
				ssize_t (*handler)(struct atkbd *, const char *, size_t));
#define ATKBD_DEFINE_ATTR(_name)						\
static ssize_t atkbd_show_##_name(struct atkbd *, char *);			\
static ssize_t atkbd_set_##_name(struct atkbd *, const char *, size_t);		\
static ssize_t atkbd_do_show_##_name(struct device *d, char *b)			\
{										\
	return atkbd_attr_show_helper(d, b, atkbd_show_##_name);		\
}										\
static ssize_t atkbd_do_set_##_name(struct device *d, const char *b, size_t s)	\
{										\
	return atkbd_attr_set_helper(d, b, s, atkbd_set_##_name);		\
}										\
static struct device_attribute atkbd_attr_##_name = 				\
	__ATTR(_name, S_IWUSR | S_IRUGO, atkbd_do_show_##_name, atkbd_do_set_##_name);

ATKBD_DEFINE_ATTR(extra);
ATKBD_DEFINE_ATTR(scroll);
ATKBD_DEFINE_ATTR(set);
ATKBD_DEFINE_ATTR(softrepeat);
ATKBD_DEFINE_ATTR(softraw);

#endif


static void atkbd_report_key(struct input_dev *dev, struct pt_regs *regs, int code, int value)
{
	input_regs(dev, regs);
	if (value == 3) {
		input_report_key(dev, code, 1);
		input_sync(dev);
		input_report_key(dev, code, 0);
	} else
		input_event(dev, EV_KEY, code, value);
	input_sync(dev);
}

/*
 * atkbd_interrupt(). Here takes place processing of data received from
 * the keyboard into events.
 */

static irqreturn_t atkbd_interrupt(struct serio *serio, unsigned char data,
			unsigned int flags, struct pt_regs *regs)
{
	struct atkbd *atkbd = serio->private;
	unsigned int code = data;
	int scroll = 0, click = -1;
	int value;

#ifdef ATKBD_DEBUG
	printk(KERN_DEBUG "atkbd.c: Received %02x flags %02x\n", data, flags);
#endif

#if !defined(__i386__) && !defined (__x86_64__)
	if ((flags & (SERIO_FRAME | SERIO_PARITY)) && (~flags & SERIO_TIMEOUT) && !atkbd->resend && atkbd->write) {
		printk(KERN_WARNING "atkbd.c: frame/parity error: %02x\n", flags);
		serio_write(serio, ATKBD_CMD_RESEND);
		atkbd->resend = 1;
		goto out;
	}

	if (!flags && data == ATKBD_RET_ACK)
		atkbd->resend = 0;
#endif

	if (unlikely(atkbd->ps2dev.flags & PS2_FLAG_ACK))
		if  (ps2_handle_ack(&atkbd->ps2dev, data))
			goto out;

	if (unlikely(atkbd->ps2dev.flags & PS2_FLAG_CMD))
		if  (ps2_handle_response(&atkbd->ps2dev, data))
			goto out;

	if (!atkbd->enabled)
		goto out;

	input_event(&atkbd->dev, EV_MSC, MSC_RAW, code);

	if (atkbd->translated) {

		if (atkbd->emul ||
		    !(code == ATKBD_RET_EMUL0 || code == ATKBD_RET_EMUL1 ||
		      code == ATKBD_RET_HANGUEL || code == ATKBD_RET_HANJA ||
		      code == ATKBD_RET_ERR ||
	             (code == ATKBD_RET_BAT && !atkbd->bat_xl))) {
			atkbd->release = code >> 7;
			code &= 0x7f;
		}

		if (!atkbd->emul &&
		     (code & 0x7f) == (ATKBD_RET_BAT & 0x7f))
			atkbd->bat_xl = !atkbd->release;
	}

	switch (code) {
		case ATKBD_RET_BAT:
			atkbd->enabled = 0;
			serio_rescan(atkbd->ps2dev.serio);
			goto out;
		case ATKBD_RET_EMUL0:
			atkbd->emul = 1;
			goto out;
		case ATKBD_RET_EMUL1:
			atkbd->emul = 2;
			goto out;
		case ATKBD_RET_RELEASE:
			atkbd->release = 1;
			goto out;
		case ATKBD_RET_HANGUEL:
			atkbd_report_key(&atkbd->dev, regs, KEY_HANGUEL, 3);
			goto out;
		case ATKBD_RET_HANJA:
			atkbd_report_key(&atkbd->dev, regs, KEY_HANJA, 3);
			goto out;
		case ATKBD_RET_ERR:
			printk(KERN_DEBUG "atkbd.c: Keyboard on %s reports too many keys pressed.\n", serio->phys);
			goto out;
	}

	if (atkbd->set != 3)
		code = (code & 0x7f) | ((code & 0x80) << 1);
	if (atkbd->emul) {
		if (--atkbd->emul)
			goto out;
		code |= (atkbd->set != 3) ? 0x80 : 0x100;
	}

	if (atkbd->keycode[code] != ATKBD_KEY_NULL)
		input_event(&atkbd->dev, EV_MSC, MSC_SCAN, code);

	switch (atkbd->keycode[code]) {
		case ATKBD_KEY_NULL:
			break;
		case ATKBD_KEY_UNKNOWN:
			if (data == ATKBD_RET_ACK || data == ATKBD_RET_NAK) {
				printk(KERN_WARNING "atkbd.c: Spurious %s on %s. Some program, "
				       "like XFree86, might be trying access hardware directly.\n",
				       data == ATKBD_RET_ACK ? "ACK" : "NAK", serio->phys);
			} else {
				printk(KERN_WARNING "atkbd.c: Unknown key %s "
				       "(%s set %d, code %#x on %s).\n",
				       atkbd->release ? "released" : "pressed",
				       atkbd->translated ? "translated" : "raw",
				       atkbd->set, code, serio->phys);
				printk(KERN_WARNING "atkbd.c: Use 'setkeycodes %s%02x <keycode>' "
				       "to make it known.\n",
				       code & 0x80 ? "e0" : "", code & 0x7f);
			}
			input_sync(&atkbd->dev);
			break;
		case ATKBD_SCR_1:
			scroll = 1 - atkbd->release * 2;
			break;
		case ATKBD_SCR_2:
			scroll = 2 - atkbd->release * 4;
			break;
		case ATKBD_SCR_4:
			scroll = 4 - atkbd->release * 8;
			break;
		case ATKBD_SCR_8:
			scroll = 8 - atkbd->release * 16;
			break;
		case ATKBD_SCR_CLICK:
			click = !atkbd->release;
			break;
		default:
			value = atkbd->release ? 0 :
				(1 + (!atkbd->softrepeat && test_bit(atkbd->keycode[code], atkbd->dev.key)));

			switch (value) { 	/* Workaround Toshiba laptop multiple keypress */
				case 0:
					atkbd->last = 0;
					break;
				case 1:
					atkbd->last = code;
					atkbd->time = jiffies + (atkbd->dev.rep[REP_DELAY] * HZ + 500) / 1000 / 2;
					break;
				case 2:
					if (!time_after(jiffies, atkbd->time) && atkbd->last == code)
						value = 1;
					break;
			}

			atkbd_report_key(&atkbd->dev, regs, atkbd->keycode[code], value);
	}

	if (scroll || click != -1) {
		input_regs(&atkbd->dev, regs);
		input_report_key(&atkbd->dev, BTN_MIDDLE, click);
		input_report_rel(&atkbd->dev, REL_WHEEL, scroll);
		input_sync(&atkbd->dev);
	}

	atkbd->release = 0;
out:
	return IRQ_HANDLED;
}


/*
 * Event callback from the input module. Events that change the state of
 * the hardware are processed here.
 */

static int atkbd_event(struct input_dev *dev, unsigned int type, unsigned int code, int value)
{
	struct atkbd *atkbd = dev->private;
	const short period[32] =
		{ 33,  37,  42,  46,  50,  54,  58,  63,  67,  75,  83,  92, 100, 109, 116, 125,
		 133, 149, 167, 182, 200, 217, 232, 250, 270, 303, 333, 370, 400, 435, 470, 500 };
	const short delay[4] =
		{ 250, 500, 750, 1000 };
	unsigned char param[2];
	int i, j;

	if (!atkbd->write)
		return -1;

	switch (type) {

		case EV_LED:

			param[0] = (test_bit(LED_SCROLLL, dev->led) ? 1 : 0)
			         | (test_bit(LED_NUML,    dev->led) ? 2 : 0)
			         | (test_bit(LED_CAPSL,   dev->led) ? 4 : 0);
		        ps2_schedule_command(&atkbd->ps2dev, param, ATKBD_CMD_SETLEDS);

			if (atkbd->extra) {
				param[0] = 0;
				param[1] = (test_bit(LED_COMPOSE, dev->led) ? 0x01 : 0)
					 | (test_bit(LED_SLEEP,   dev->led) ? 0x02 : 0)
					 | (test_bit(LED_SUSPEND, dev->led) ? 0x04 : 0)
				         | (test_bit(LED_MISC,    dev->led) ? 0x10 : 0)
				         | (test_bit(LED_MUTE,    dev->led) ? 0x20 : 0);
				ps2_schedule_command(&atkbd->ps2dev, param, ATKBD_CMD_EX_SETLEDS);
			}

			return 0;


		case EV_REP:

			if (atkbd->softrepeat) return 0;

			i = j = 0;
			while (i < 32 && period[i] < dev->rep[REP_PERIOD]) i++;
			while (j < 4 && delay[j] < dev->rep[REP_DELAY]) j++;
			dev->rep[REP_PERIOD] = period[i];
			dev->rep[REP_DELAY] = delay[j];
			param[0] = i | (j << 5);
			ps2_schedule_command(&atkbd->ps2dev, param, ATKBD_CMD_SETREP);

			return 0;
	}

	return -1;
}

/*
 * atkbd_enable() signals that interrupt handler is allowed to
 * generate input events.
 */

static inline void atkbd_enable(struct atkbd *atkbd)
{
	serio_pause_rx(atkbd->ps2dev.serio);
	atkbd->enabled = 1;
	serio_continue_rx(atkbd->ps2dev.serio);
}

/*
 * atkbd_disable() tells input handler that all incoming data except
 * for ACKs and command response should be dropped.
 */

static inline void atkbd_disable(struct atkbd *atkbd)
{
	serio_pause_rx(atkbd->ps2dev.serio);
	atkbd->enabled = 0;
	serio_continue_rx(atkbd->ps2dev.serio);
}

/*
 * atkbd_probe() probes for an AT keyboard on a serio port.
 */

static int atkbd_probe(struct atkbd *atkbd)
{
	struct ps2dev *ps2dev = &atkbd->ps2dev;
	unsigned char param[2];

/*
 * Some systems, where the bit-twiddling when testing the io-lines of the
 * controller may confuse the keyboard need a full reset of the keyboard. On
 * these systems the BIOS also usually doesn't do it for us.
 */

	if (atkbd_reset)
		if (ps2_command(ps2dev, NULL, ATKBD_CMD_RESET_BAT))
			printk(KERN_WARNING "atkbd.c: keyboard reset failed on %s\n", ps2dev->serio->phys);

/*
 * Then we check the keyboard ID. We should get 0xab83 under normal conditions.
 * Some keyboards report different values, but the first byte is always 0xab or
 * 0xac. Some old AT keyboards don't report anything. If a mouse is connected, this
 * should make sure we don't try to set the LEDs on it.
 */

	param[0] = param[1] = 0xa5;	/* initialize with invalid values */
	if (ps2_command(ps2dev, param, ATKBD_CMD_GETID)) {

		/* Some crappy BIOSes (hello DELL!) do not implement CMD_GETID */
		if (param[0]==0xa5 && param[1]==0xa5) {
			if (!ps2dev->serio ||
			    !strstr(ps2dev->serio->name, "Kbd"))
				return -1;
			atkbd->id = 0;
			atkbd->translated = 1;
			return 0;
		}

/*
 * If the get ID command failed, we check if we can at least set the LEDs on
 * the keyboard. This should work on every keyboard out there. It also turns
 * the LEDs off, which we want anyway.
 */
		param[0] = 0;
		if (ps2_command(ps2dev, param, ATKBD_CMD_SETLEDS))
			return -1;
		atkbd->id = 0xabba;
		return 0;
	}

	if (param[0] != 0xab && param[0] != 0xac &&	/* Regular and NCD Sun keyboards */
	    param[0] != 0x2b && param[0] != 0x5d &&	/* Trust keyboard, raw and translated */
	    param[0] != 0x60 && param[0] != 0x47)	/* NMB SGI keyboard, raw and translated */
		return -1;

	atkbd->id = (param[0] << 8) | param[1];

	if (atkbd->id == 0xaca1 && atkbd->translated) {
		printk(KERN_ERR "atkbd.c: NCD terminal keyboards are only supported on non-translating\n");
		printk(KERN_ERR "atkbd.c: controllers. Use i8042.direct=1 to disable translation.\n");
		return -1;
	}

	return 0;
}

/*
 * atkbd_select_set checks if a keyboard has a working Set 3 support, and
 * sets it into that. Unfortunately there are keyboards that can be switched
 * to Set 3, but don't work well in that (BTC Multimedia ...)
 */

static int atkbd_select_set(struct atkbd *atkbd, int target_set, int allow_extra)
{
	struct ps2dev *ps2dev = &atkbd->ps2dev;
	unsigned char param[2];

	atkbd->extra = 0;
/*
 * For known special keyboards we can go ahead and set the correct set.
 * We check for NCD PS/2 Sun, NorthGate OmniKey 101 and
 * IBM RapidAccess / IBM EzButton / Chicony KBP-8993 keyboards.
 */

	if (atkbd->translated)
		return 2;

	if (atkbd->id == 0xaca1) {
		param[0] = 3;
		ps2_command(ps2dev, param, ATKBD_CMD_SSCANSET);
		return 3;
	}

	if (allow_extra) {
		param[0] = 0x71;
		if (!ps2_command(ps2dev, param, ATKBD_CMD_EX_ENABLE)) {
			atkbd->extra = 1;
			return 2;
		}
	}

	if (target_set != 3)
		return 2;

	if (!ps2_command(ps2dev, param, ATKBD_CMD_OK_GETID)) {
		atkbd->id = param[0] << 8 | param[1];
		return 2;
	}

	param[0] = 3;
	if (ps2_command(ps2dev, param, ATKBD_CMD_SSCANSET))
		return 2;

	param[0] = 0;
	if (ps2_command(ps2dev, param, ATKBD_CMD_GSCANSET))
		return 2;

	if (param[0] != 3) {
		param[0] = 2;
		if (ps2_command(ps2dev, param, ATKBD_CMD_SSCANSET))
		return 2;
	}

	ps2_command(ps2dev, param, ATKBD_CMD_SETALL_MBR);

	return 3;
}

static int atkbd_activate(struct atkbd *atkbd)
{
	struct ps2dev *ps2dev = &atkbd->ps2dev;
	unsigned char param[1];

/*
 * Set the LEDs to a defined state.
 */

	param[0] = 0;
	if (ps2_command(ps2dev, param, ATKBD_CMD_SETLEDS))
		return -1;

/*
 * Set autorepeat to fastest possible.
 */

	param[0] = 0;
	if (ps2_command(ps2dev, param, ATKBD_CMD_SETREP))
		return -1;

/*
 * Enable the keyboard to receive keystrokes.
 */

	if (ps2_command(ps2dev, NULL, ATKBD_CMD_ENABLE)) {
		printk(KERN_ERR "atkbd.c: Failed to enable keyboard on %s\n",
			ps2dev->serio->phys);
		return -1;
	}

	return 0;
}

/*
 * atkbd_cleanup() restores the keyboard state so that BIOS is happy after a
 * reboot.
 */

static void atkbd_cleanup(struct serio *serio)
{
	struct atkbd *atkbd = serio->private;
	ps2_command(&atkbd->ps2dev, NULL, ATKBD_CMD_RESET_BAT);
}


/*
 * atkbd_disconnect() closes and frees.
 */

static void atkbd_disconnect(struct serio *serio)
{
	struct atkbd *atkbd = serio->private;

	atkbd_disable(atkbd);

#ifndef L4INPUT
	/* make sure we don't have a command in flight */
	synchronize_kernel();
	flush_scheduled_work();

	device_remove_file(&serio->dev, &atkbd_attr_extra);
	device_remove_file(&serio->dev, &atkbd_attr_scroll);
	device_remove_file(&serio->dev, &atkbd_attr_set);
	device_remove_file(&serio->dev, &atkbd_attr_softrepeat);
	device_remove_file(&serio->dev, &atkbd_attr_softraw);
#endif

	input_unregister_device(&atkbd->dev);
	serio_close(serio);
	kfree(atkbd);
}


/*
 * atkbd_set_device_attrs() initializes keyboard's keycode table
 * according to the selected scancode set
 */

static void atkbd_set_keycode_table(struct atkbd *atkbd)
{
	int i, j;

	memset(atkbd->keycode, 0, sizeof(atkbd->keycode));

	if (atkbd->translated) {
		for (i = 0; i < 128; i++) {
			atkbd->keycode[i] = atkbd_set2_keycode[atkbd_unxlate_table[i]];
			atkbd->keycode[i | 0x80] = atkbd_set2_keycode[atkbd_unxlate_table[i] | 0x80];
			if (atkbd->scroll)
				for (j = 0; i < 5; i++) {
					if (atkbd_unxlate_table[i] == atkbd_scroll_keys[j][1])
						atkbd->keycode[i] = atkbd_scroll_keys[j][0];
					if ((atkbd_unxlate_table[i] | 0x80) == atkbd_scroll_keys[j][1])
						atkbd->keycode[i | 0x80] = atkbd_scroll_keys[j][0];
				}
		}
	} else if (atkbd->set == 3) {
		memcpy(atkbd->keycode, atkbd_set3_keycode, sizeof(atkbd->keycode));
	} else {
		memcpy(atkbd->keycode, atkbd_set2_keycode, sizeof(atkbd->keycode));

		if (atkbd->scroll)
			for (i = 0; i < 5; i++)
				atkbd->keycode[atkbd_scroll_keys[i][1]] = atkbd_scroll_keys[i][0];
	}
}

/*
 * atkbd_set_device_attrs() sets up keyboard's input device structure
 */

static void atkbd_set_device_attrs(struct atkbd *atkbd)
{
	int i;

	memset(&atkbd->dev, 0, sizeof(struct input_dev));

	init_input_dev(&atkbd->dev);

	atkbd->dev.name = atkbd->name;
	atkbd->dev.phys = atkbd->phys;
	atkbd->dev.id.bustype = BUS_I8042;
	atkbd->dev.id.vendor = 0x0001;
	atkbd->dev.id.product = atkbd->translated ? 1 : atkbd->set;
	atkbd->dev.id.version = atkbd->id;
	atkbd->dev.event = atkbd_event;
	atkbd->dev.private = atkbd;
	atkbd->dev.dev = &atkbd->ps2dev.serio->dev;

	atkbd->dev.evbit[0] = BIT(EV_KEY) | BIT(EV_REP) | BIT(EV_MSC);

	if (atkbd->write) {
		atkbd->dev.evbit[0] |= BIT(EV_LED);
		atkbd->dev.ledbit[0] = BIT(LED_NUML) | BIT(LED_CAPSL) | BIT(LED_SCROLLL);
	}

	if (atkbd->extra)
		atkbd->dev.ledbit[0] |= BIT(LED_COMPOSE) | BIT(LED_SUSPEND) |
					BIT(LED_SLEEP) | BIT(LED_MUTE) | BIT(LED_MISC);

	if (!atkbd->softrepeat) {
		atkbd->dev.rep[REP_DELAY] = 250;
		atkbd->dev.rep[REP_PERIOD] = 33;
	}

	atkbd->dev.mscbit[0] = atkbd->softraw ? BIT(MSC_SCAN) : BIT(MSC_RAW) | BIT(MSC_SCAN);

	if (atkbd->scroll) {
		atkbd->dev.evbit[0] |= BIT(EV_REL);
		atkbd->dev.relbit[0] = BIT(REL_WHEEL);
		set_bit(BTN_MIDDLE, atkbd->dev.keybit);
	}

	atkbd->dev.keycode = atkbd->keycode;
	atkbd->dev.keycodesize = sizeof(unsigned char);
	atkbd->dev.keycodemax = ARRAY_SIZE(atkbd_set2_keycode);

	for (i = 0; i < 512; i++)
		if (atkbd->keycode[i] && atkbd->keycode[i] < ATKBD_SPECIAL)
			set_bit(atkbd->keycode[i], atkbd->dev.keybit);
}

/*
 * atkbd_connect() is called when the serio module finds and interface
 * that isn't handled yet by an appropriate device driver. We check if
 * there is an AT keyboard out there and if yes, we register ourselves
 * to the input module.
 */

static void atkbd_connect(struct serio *serio, struct serio_driver *drv)
{
	struct atkbd *atkbd;

	if (!(atkbd = kmalloc(sizeof(struct atkbd), GFP_KERNEL)))
		return;
	memset(atkbd, 0, sizeof(struct atkbd));

	ps2_init(&atkbd->ps2dev, serio);

	switch (serio->type & SERIO_TYPE) {

		case SERIO_8042_XL:
			atkbd->translated = 1;
		case SERIO_8042:
			if (serio->write)
				atkbd->write = 1;
			break;
		case SERIO_RS232:
			if ((serio->type & SERIO_PROTO) == SERIO_PS2SER)
				break;
		default:
			kfree(atkbd);
			return;
	}

	atkbd->softraw = atkbd_softraw;
	atkbd->softrepeat = atkbd_softrepeat;
	atkbd->scroll = atkbd_scroll;

	if (!atkbd->write)
		atkbd->softrepeat = 1;

	if (atkbd->softrepeat)
		atkbd->softraw = 1;

	serio->private = atkbd;

	if (serio_open(serio, drv)) {
		kfree(atkbd);
		return;
	}

	if (atkbd->write) {

		if (atkbd_probe(atkbd)) {
			serio_close(serio);
			serio->private = NULL;
			kfree(atkbd);
			return;
		}

		atkbd->set = atkbd_select_set(atkbd, atkbd_set, atkbd_extra);
		atkbd_activate(atkbd);

	} else {
		atkbd->set = 2;
		atkbd->id = 0xab00;
	}

	if (atkbd->extra)
		sprintf(atkbd->name, "AT Set 2 Extra keyboard");
	else
		sprintf(atkbd->name, "AT %s Set %d keyboard",
			atkbd->translated ? "Translated" : "Raw", atkbd->set);

	snprintf(atkbd->phys, sizeof(atkbd->phys), "%s/input0", serio->phys);

	atkbd_set_keycode_table(atkbd);
	atkbd_set_device_attrs(atkbd);

	input_register_device(&atkbd->dev);

	device_create_file(&serio->dev, &atkbd_attr_extra);
	device_create_file(&serio->dev, &atkbd_attr_scroll);
	device_create_file(&serio->dev, &atkbd_attr_set);
	device_create_file(&serio->dev, &atkbd_attr_softrepeat);
	device_create_file(&serio->dev, &atkbd_attr_softraw);

	atkbd_enable(atkbd);

	printk(KERN_INFO "input: %s on %s\n", atkbd->name, serio->phys);
}

/*
 * atkbd_reconnect() tries to restore keyboard into a sane state and is
 * most likely called on resume.
 */

static int atkbd_reconnect(struct serio *serio)
{
	struct atkbd *atkbd = serio->private;
	struct serio_driver *drv = serio->drv;
	unsigned char param[1];

	if (!atkbd || !drv) {
		printk(KERN_DEBUG "atkbd: reconnect request, but serio is disconnected, ignoring...\n");
		return -1;
	}

	atkbd_disable(atkbd);

	if (atkbd->write) {
		param[0] = (test_bit(LED_SCROLLL, atkbd->dev.led) ? 1 : 0)
		         | (test_bit(LED_NUML,    atkbd->dev.led) ? 2 : 0)
 		         | (test_bit(LED_CAPSL,   atkbd->dev.led) ? 4 : 0);

		if (atkbd_probe(atkbd))
			return -1;
		if (atkbd->set != atkbd_select_set(atkbd, atkbd->set, atkbd->extra))
			return -1;

		atkbd_activate(atkbd);

		if (ps2_command(&atkbd->ps2dev, param, ATKBD_CMD_SETLEDS))
			return -1;
	}

	atkbd_enable(atkbd);

	return 0;
}

static struct serio_driver atkbd_drv = {
	.driver		= {
		.name	= "atkbd",
	},
	.description	= DRIVER_DESC,
	.interrupt	= atkbd_interrupt,
	.connect	= atkbd_connect,
	.reconnect	= atkbd_reconnect,
	.disconnect	= atkbd_disconnect,
	.cleanup	= atkbd_cleanup,
};

#ifndef L4INPUT

static ssize_t atkbd_attr_show_helper(struct device *dev, char *buf,
				ssize_t (*handler)(struct atkbd *, char *))
{
	struct serio *serio = to_serio_port(dev);
	int retval;

	retval = serio_pin_driver(serio);
	if (retval)
		return retval;

	if (serio->drv != &atkbd_drv) {
		retval = -ENODEV;
		goto out;
	}

	retval = handler((struct atkbd *)serio->private, buf);

out:
	serio_unpin_driver(serio);
	return retval;
}

static ssize_t atkbd_attr_set_helper(struct device *dev, const char *buf, size_t count,
				ssize_t (*handler)(struct atkbd *, const char *, size_t))
{
	struct serio *serio = to_serio_port(dev);
	struct atkbd *atkbd;
	int retval;

	retval = serio_pin_driver(serio);
	if (retval)
		return retval;

	if (serio->drv != &atkbd_drv) {
		retval = -ENODEV;
		goto out;
	}

	atkbd = serio->private;
	atkbd_disable(atkbd);
	retval = handler(atkbd, buf, count);
	atkbd_enable(atkbd);

out:
	serio_unpin_driver(serio);
	return retval;
}

static ssize_t atkbd_show_extra(struct atkbd *atkbd, char *buf)
{
	return sprintf(buf, "%d\n", atkbd->extra ? 1 : 0);
}

static ssize_t atkbd_set_extra(struct atkbd *atkbd, const char *buf, size_t count)
{
	unsigned long value;
	char *rest;

	if (!atkbd->write)
		return -EIO;

	value = simple_strtoul(buf, &rest, 10);
	if (*rest || value > 1)
		return -EINVAL;

	if (atkbd->extra != value) {
		/* unregister device as it's properties will change */
		input_unregister_device(&atkbd->dev);
		atkbd->set = atkbd_select_set(atkbd, atkbd->set, value);
		atkbd_activate(atkbd);
		atkbd_set_device_attrs(atkbd);
		input_register_device(&atkbd->dev);
	}
	return count;
}

static ssize_t atkbd_show_scroll(struct atkbd *atkbd, char *buf)
{
	return sprintf(buf, "%d\n", atkbd->scroll ? 1 : 0);
}

static ssize_t atkbd_set_scroll(struct atkbd *atkbd, const char *buf, size_t count)
{
	unsigned long value;
	char *rest;

	value = simple_strtoul(buf, &rest, 10);
	if (*rest || value > 1)
		return -EINVAL;

	if (atkbd->scroll != value) {
		/* unregister device as it's properties will change */
		input_unregister_device(&atkbd->dev);
		atkbd->scroll = value;
		atkbd_set_keycode_table(atkbd);
		atkbd_set_device_attrs(atkbd);
		input_register_device(&atkbd->dev);
	}
	return count;
}

static ssize_t atkbd_show_set(struct atkbd *atkbd, char *buf)
{
	return sprintf(buf, "%d\n", atkbd->set);
}

static ssize_t atkbd_set_set(struct atkbd *atkbd, const char *buf, size_t count)
{
	unsigned long value;
	char *rest;

	if (!atkbd->write)
		return -EIO;

	value = simple_strtoul(buf, &rest, 10);
	if (*rest || (value != 2 && value != 3))
		return -EINVAL;

	if (atkbd->set != value) {
		/* unregister device as it's properties will change */
		input_unregister_device(&atkbd->dev);
		atkbd->set = atkbd_select_set(atkbd, value, atkbd->extra);
		atkbd_activate(atkbd);
		atkbd_set_keycode_table(atkbd);
		atkbd_set_device_attrs(atkbd);
		input_register_device(&atkbd->dev);
	}
	return count;
}

static ssize_t atkbd_show_softrepeat(struct atkbd *atkbd, char *buf)
{
	return sprintf(buf, "%d\n", atkbd->softrepeat ? 1 : 0);
}

static ssize_t atkbd_set_softrepeat(struct atkbd *atkbd, const char *buf, size_t count)
{
	unsigned long value;
	char *rest;

	if (!atkbd->write)
		return -EIO;

	value = simple_strtoul(buf, &rest, 10);
	if (*rest || value > 1)
		return -EINVAL;

	if (atkbd->softrepeat != value) {
		/* unregister device as it's properties will change */
		input_unregister_device(&atkbd->dev);
		atkbd->softrepeat = value;
		if (atkbd->softrepeat)
			atkbd->softraw = 1;
		atkbd_set_device_attrs(atkbd);
		input_register_device(&atkbd->dev);
	}

	return count;
}


static ssize_t atkbd_show_softraw(struct atkbd *atkbd, char *buf)
{
	return sprintf(buf, "%d\n", atkbd->softraw ? 1 : 0);
}

static ssize_t atkbd_set_softraw(struct atkbd *atkbd, const char *buf, size_t count)
{
	unsigned long value;
	char *rest;

	value = simple_strtoul(buf, &rest, 10);
	if (*rest || value > 1)
		return -EINVAL;

	if (atkbd->softraw != value) {
		/* unregister device as it's properties will change */
		input_unregister_device(&atkbd->dev);
		atkbd->softraw = value;
		atkbd_set_device_attrs(atkbd);
		input_register_device(&atkbd->dev);
	}
	return count;
}

#endif

int __init atkbd_init(void)
{
	serio_register_driver(&atkbd_drv);
	return 0;
}

void __exit atkbd_exit(void)
{
	serio_unregister_driver(&atkbd_drv);
}

module_init(atkbd_init);
module_exit(atkbd_exit);
