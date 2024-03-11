/*
 * jetclocks.h
 */

#ifndef __JETCLOCKS_H__
#define __JETCLOCKS_H__

#include <linux/ioctl.h>

#define CLK_ENABLE		_IOW('{', 1, char *)
#define CLK_DISABLE		_IOW('{', 2, char *)
#define CLK_IS_ENABLED          _IOR('{', 3, char *)

#endif /* __JETCLOCKS_H__ */
