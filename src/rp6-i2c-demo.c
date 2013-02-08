#include <stdio.h>
#include <stdlib.h>

#include "lib/generic/generic_i2c.h"
#include "lib/linux/i2c/linux_i2c.h"
#include "lib/rp6/rp6.h"

#include "config.h"

LINUX_I2C* linux_i2c;
GENERIC_I2C* generic_i2c;
RP6* rp6;
RP6_registers regs;

void changeDirection( RP6_DIRECTION dir ) {
	if ( rp6->changeDirection( rp6 , dir ) ) {
		printf( "ERROR: RP6 - changeDirection\n" );
		exit( EXIT_FAILURE );
	}
}

void move( uint8_t desired_speed , RP6_DIRECTION dir , uint16_t distance ) {
	if ( rp6->move( rp6 , desired_speed , dir , distance ) ) {
		printf( "ERROR: RP6 - move\n" );
		exit( EXIT_FAILURE );
	}
}

void moveAtSpeed( uint8_t desired_speed_left , uint8_t desired_speed_right ) {
	if ( rp6->moveAtSpeed( rp6 , desired_speed_left , desired_speed_right ) ) {
		printf( "ERROR: RP6 - moveAtSpeed\n" );
		exit( EXIT_FAILURE );
	}
}

void readRegisters( void ) {
	if ( rp6->readRegisters( rp6 , &regs ) ) {
		printf( "ERROR: RP6 - readRegisters\n" );
		exit( EXIT_FAILURE );
	}
}

void rotate( uint8_t desired_speed , RP6_DIRECTION dir , uint16_t angle ) {
	if ( rp6->rotate( rp6 , desired_speed , dir , angle ) ) {
		printf( "ERROR: RP6 - rotate\n" );
		exit( EXIT_FAILURE );
	}
}

void setACSPower( RP6_ACS_POWER acs ) {
	if ( rp6->setACSPower( rp6 , acs ) ) {
		printf( "ERROR: RP6 - setACSPower\n" );
		exit( EXIT_FAILURE );
	}
}

void stop( void ) {
	if ( rp6->stop( rp6 ) ) {
		printf( "ERROR: RP6 - stop\n" );
		exit( EXIT_FAILURE );
	}
}

int init( void ) {
	printf( "DEBUG: create LINUX_I2C\n" );
	linux_i2c = LINUX_I2C_new( I2C_DEVICE );
	if ( linux_i2c == NULL ) {
		printf( "ERROR: create LINUX_I2C\n" );
		return EXIT_FAILURE;
	}

	printf( "DEBUG: open I2C device\n" );
	if ( linux_i2c->open( linux_i2c ) ) {
		printf( "ERROR: open I2C device\n" );
		return EXIT_FAILURE;
	}

	printf( "DEBUG: create GENERIC_I2C\n" );
	generic_i2c = linux_i2c->getI2C( linux_i2c );
	if ( generic_i2c == NULL ) {
		printf( "ERROR: create GENERIC_I2C\n" );
		linux_i2c->close( linux_i2c );
		return EXIT_FAILURE;
	}

	printf( "DEBUG: create RP6\n" );
	rp6 = RP6_new( generic_i2c );
	if ( rp6 == NULL ) {
		printf( "ERROR: create RP6\n" );
		linux_i2c->close( linux_i2c );
		return EXIT_FAILURE;
	}

	printf( "DEBUG: set ACSPower\n" );
	setACSPower( RP6_HIGH );
	
	return EXIT_SUCCESS;
}

enum STATE_ENUM {
	S_init, forward, bumperLeft, bumperLeft2, bumperRight, bumperRight2, obstacleLeft, obstacleRight
};
typedef enum STATE_ENUM STATE;

int main( void ) {
	if ( !init() ) {
		return EXIT_FAILURE;
	}

	STATE state = S_init;
	STATE last_state;

	stop();

	while ( 1 ) {
		last_state = state;
		readRegisters();

		switch ( state ) {
		default:
		case S_init:
			if ( regs.STATUS1.bumperLeft ) {
				state = bumperLeft;
			} else if ( regs.STATUS1.bumperRight ) {
				state = bumperRight;
			} else if ( regs.STATUS1.obstacleLeft ) {
				state = obstacleLeft;
			} else if ( regs.STATUS1.obstacleRight ) {
				state = obstacleRight;
			} else {
				state = forward;
			}
			break;
		case forward:
			if ( regs.STATUS1.bumperLeft ) {
				state = bumperLeft;
			} else if ( regs.STATUS1.bumperRight ) {
				state = bumperRight;
			} else if ( regs.STATUS1.obstacleLeft ) {
				state = obstacleLeft;
			} else if ( regs.STATUS1.obstacleRight ) {
				state = obstacleRight;
			}
			break;
		case bumperLeft:
			if ( regs.STATUS3.movementComplete ) {
				state = bumperLeft2;
			}
			break;
		case bumperLeft2:
			if ( regs.STATUS3.movementComplete ) {
				state = forward;
			}
			break;
		case bumperRight:
			if ( regs.STATUS3.movementComplete ) {
				state = bumperRight2;
			}
			break;
		case bumperRight2:
			if ( regs.STATUS3.movementComplete ) {
				state = forward;
			}
			break;
		case obstacleLeft:
			if ( regs.STATUS1.bumperLeft ) {
				state = bumperLeft;
			} else if ( regs.STATUS1.bumperRight ) {
				state = bumperRight;
			} else if ( !regs.STATUS1.obstacleLeft ) {
				state = forward;
			}
			break;
		case obstacleRight:
			if ( regs.STATUS1.bumperLeft ) {
				state = bumperLeft;
			} else if ( regs.STATUS1.bumperRight ) {
				state = bumperRight;
			} else if ( !regs.STATUS1.obstacleRight ) {
				state = forward;
			}
			break;
		}

		if ( last_state != state ) {
			switch ( state ) {
			case forward:
				stop();
				changeDirection( RP6_FORWARD );
				moveAtSpeed( 80 , 80 );
				printf( "Change State: forward\n" );
				break;
			case bumperLeft:
				stop();
				move( 20 , RP6_BACKWARD , 600 );
				printf( "Change State: bumperLeft\n" );
				break;
			case bumperLeft2:
				rotate( 20 , RP6_RIGHT , 45 );
				printf( "Change State: bumperLeft2\n" );
				break;
			case bumperRight:
				stop();
				move( 20 , RP6_BACKWARD , 600 );
				printf( "Change State: bumperRight\n" );
				break;
			case bumperRight2:
				rotate( 20 , RP6_LEFT , 45 );
				printf( "Change State: bumperRight2\n" );
				break;
			case obstacleLeft:
				stop();
				changeDirection( RP6_RIGHT );
				moveAtSpeed( 20 , 20 );
				printf( "Change State: obstacleLeft\n" );
				break;
			case obstacleRight:
				stop();
				changeDirection( RP6_LEFT );
				moveAtSpeed( 20 , 20 );
				printf( "Change State: obstacleRight\n" );
				break;
			default:
				break;
			}
		}
	}
	linux_i2c->close( linux_i2c );
	return EXIT_SUCCESS;

}
