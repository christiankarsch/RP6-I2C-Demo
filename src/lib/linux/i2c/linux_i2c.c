#include <fcntl.h> /* open(); O_RDWR; */
#include <stdlib.h> /* malloc(); EXIT_FAILURE; EXIT_SUCCESS; */
#include <string.h> /* strcpy(); strlen(); */
#include <unistd.h> /* close(); read(); write(); */
#include <sys/ioctl.h> /* ioctl(); */

#include "linux_i2c_dev.h"
#include "linux_i2c.h"

LINUX_I2C* LINUX_I2C_new( char* device ) {
	LINUX_I2C* t;

	if ( device == NULL ) {
		return NULL;
	}

	t = (LINUX_I2C*) malloc( sizeof(LINUX_I2C) );
	if ( t == NULL ) {
		return NULL;
	}

	t->this = t;
	t->device = NULL;
	t->fd = 0;

	t->getI2C = LINUX_I2C_getI2C;

	t->close = LINUX_I2C_close;
	t->open = LINUX_I2C_open;
	t->read = LINUX_I2C_read;
	t->readRegister = LINUX_I2C_readRegister;
	t->readRegisters = LINUX_I2C_readRegisters;
	t->test = LINUX_I2C_test;
	t->write = LINUX_I2C_write;
	t->writeRegister = LINUX_I2C_writeRegister;
	t->writeRegisters = LINUX_I2C_writeRegisters;

	t->device = (char*) malloc( strlen( device ) );
	if ( t->device == NULL ) {
		free( t );
		return NULL;
	}

	strcpy( t->device , device );

	return t;
}

GENERIC_I2C* LINUX_I2C_getI2C( void* o ) {
	LINUX_I2C* t = ( (LINUX_I2C*) o )->this;

	GENERIC_I2C* g;

	g = (GENERIC_I2C*) malloc( sizeof(GENERIC_I2C) );
	if ( g == NULL ) {
		return NULL;
	}

	g->this = t;

	g->read = t->read;
	g->readRegister = t->readRegister;
	g->readRegisters = t->readRegisters;
	g->test = t->test;
	g->write = t->write;
	g->writeRegister = t->writeRegister;
	g->writeRegisters = t->writeRegisters;

	return g;
}

int LINUX_I2C_close( void* o ) {
	LINUX_I2C* t = ( (LINUX_I2C*) o )->this;

	if ( t->fd == 0 ) {
		return EXIT_FAILURE;
	}

	close( t->fd );
	t->fd = 0;

	return EXIT_SUCCESS;
}

int LINUX_I2C_open( void* o ) {
	LINUX_I2C* t = ( (LINUX_I2C*) o )->this;

	if ( t->fd != 0 ) {
		return EXIT_FAILURE;
	}

	t->fd = open( t->device , O_RDWR );

	if ( t->fd == 0 ) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/**
 * one message
 * master read n bytes
 */
int LINUX_I2C_read( void* o , uint8_t addr , uint8_t count , uint8_t* data ) {
	LINUX_I2C* t = ( (LINUX_I2C*) o )->this;

	if ( t->fd == 0 ) {
		return EXIT_FAILURE;
	}

	struct i2c_msg msg = { addr , I2C_M_RD , count , data };
	struct i2c_rdwr_ioctl_data rdwr = { &msg , 1 };

	if ( ioctl( t->fd , I2C_RDWR , &rdwr ) < 0 ) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/**
 * two messages
 * master write one byte
 * master read one byte
 */
int LINUX_I2C_readRegister( void* o , uint8_t addr , uint8_t reg , uint8_t* data ) {
	LINUX_I2C* t = ( (LINUX_I2C*) o )->this;

	return t->readRegisters( 0 , addr , reg , 1 , data );
}

/**
 * two messages
 * master write one byte
 * master read n byte
 */
int LINUX_I2C_readRegisters( void* o , uint8_t addr , uint8_t reg , uint8_t count , uint8_t* data ) {
	LINUX_I2C* t = ( (LINUX_I2C*) o )->this;

	if ( t->fd == 0 ) {
		return EXIT_FAILURE;
	}

	struct i2c_msg msg[ 2 ] = { { addr , 0 , 1 , &reg } , { addr , I2C_M_RD , count , data } };
	struct i2c_rdwr_ioctl_data rdwr = { msg , 2 };

	if ( ioctl( t->fd , I2C_RDWR , &rdwr ) < 0 ) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int LINUX_I2C_test( void* o , uint8_t addr ) {
	LINUX_I2C* t = ( (LINUX_I2C*) o )->this;

	if ( t->fd == 0 ) {
		return EXIT_FAILURE;
	}

	if ( ioctl( t->fd , I2C_SLAVE , addr ) < 0 ) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/**
 * one message
 * master write n bytes
 */
int LINUX_I2C_write( void* o , uint8_t addr , uint8_t count , uint8_t* data ) {
	LINUX_I2C* t = ( (LINUX_I2C*) o )->this;

	if ( t->fd == 0 ) {
		return EXIT_FAILURE;
	}

	struct i2c_msg msg = { addr , 0 , count , data };
	struct i2c_rdwr_ioctl_data rdwr = { &msg , 1 };

	if ( ioctl( t->fd , I2C_RDWR , &rdwr ) < 0 ) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/**
 * one message
 * master write 2 bytes
 */
int LINUX_I2C_writeRegister( void* o , uint8_t addr , uint8_t reg , uint8_t data ) {
	LINUX_I2C* t = ( (LINUX_I2C*) o )->this;

	return t->writeRegisters( o , addr , reg , 1 , &data );

}

/**
 * one message
 * master write 1 + n bytes
 */
int LINUX_I2C_writeRegisters( void* o , uint8_t addr , uint8_t reg , uint8_t count , uint8_t* data ) {
	LINUX_I2C* t = ( (LINUX_I2C*) o )->this;

	int i;
	uint8_t buf[ count + 1 ];
	buf[ 0 ] = reg;
	for ( i = 0; i < count ; i++ ) {
		buf[ i + 1 ] = data[ i ];
	}
	return t->write( o , addr , count + 1 , buf );
}

