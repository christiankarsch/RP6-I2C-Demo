#ifndef LIB_LINUX_I2C_H
#define LIB_LINUX_I2C_H

#include <stdint.h>
#include "../../generic/generic_i2c.h"

struct LINUX_I2C_STRUCT {
	void* this;

	char* device;
	int fd;

	GENERIC_I2C* (*getI2C)( void* o );

	int (*close)( void* o );
	int (*open)( void* o );
	int (*read)( void* o , uint8_t addr , uint8_t reg , uint8_t* data );
	int (*readRegister)( void* o , uint8_t addr , uint8_t count , uint8_t* data );
	int (*readRegisters)( void* o , uint8_t addr , uint8_t reg , uint8_t count , uint8_t* data );
	int (*test)( void* o , uint8_t addr );
	int (*write)( void* o , uint8_t addr , uint8_t count , uint8_t* data );
	int (*writeRegister)( void* o , uint8_t addr , uint8_t reg , uint8_t data );
	int (*writeRegisters)( void* o , uint8_t addr , uint8_t reg , uint8_t count , uint8_t* data );
};
typedef struct LINUX_I2C_STRUCT LINUX_I2C;

LINUX_I2C* LINUX_I2C_new( char* device );

GENERIC_I2C* LINUX_I2C_getI2C( void* o );

int LINUX_I2C_close( void* o );
int LINUX_I2C_open( void* o );
int LINUX_I2C_read( void* o , uint8_t addr , uint8_t count , uint8_t* data );
int LINUX_I2C_readRegister( void* o , uint8_t addr , uint8_t reg , uint8_t* data );
int LINUX_I2C_readRegisters( void* o , uint8_t addr , uint8_t reg , uint8_t count , uint8_t* data );
int LINUX_I2C_test( void* o , uint8_t addr );
int LINUX_I2C_write( void* o , uint8_t addr , uint8_t count , uint8_t* data );
int LINUX_I2C_writeRegister( void* o , uint8_t addr , uint8_t reg , uint8_t data );
int LINUX_I2C_writeRegisters( void* o , uint8_t addr , uint8_t reg , uint8_t count , uint8_t* data );

#endif /* LIB_LINUX_I2C_H */
