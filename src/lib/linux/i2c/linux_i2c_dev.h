#ifndef LIB_LINUX_I2C_DEV_H
#define LIB_LINUX_I2C_DEV_H

#include <stdint.h>

struct i2c_msg {
	uint16_t addr;
	uint16_t flags;
	uint16_t len;
	uint8_t* buf;
};

struct i2c_rdwr_ioctl_data {
	struct i2c_msg* msgs;
	unsigned int nmsgs;
};

#define I2C_M_RD 0x01

#define I2C_SLAVE 0x0703
#define I2C_RDWR  0x0707

#endif /* LIB_LINUX_I2C_DEV_H */
