/* Names: Tyler Andrews, Brennan Campbell, Tyler Tetens
 * Project: Experiment 5
 * Class: Cpe5160
 * Professor: Mr. Younger
*/
#ifndef _BLINKING_LIGHTS_H
#define _BLINKING_LIGHTS_H

#include "main.h"
#include "PORT.h"

// The type used for all the possible states of the system
typedef enum {
	IDLE_STATE,
	STATE_1_L_to_R,
	STATE_2_L_to_R,
	STATE_3_L_to_R,
	STATE_4_L_to_R,
	STATE_1_R_to_L,
	STATE_2_R_to_L,
	STATE_3_R_to_L,
	STATE_4_R_to_L,
	TIMER_INCREMENT_MODE
} states_t;

/* Desc: Function sets the state of the LED's (Active Low);
 * Warning: Takes light_config and shifts right 4 times and sets the lower bits to one
 *          to not disrupt the state of the buttons
 */
void set_lights(uint8_t light_config);

#endif
