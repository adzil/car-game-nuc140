#ifndef __FSM_H__
#define __FSM_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "cmsis_os.h"
#include "GPIO.h"
#include "LCD.h"

#define BLOCK_WIDTH 16
#define BLOCK_HEIGHT 18
#define BLOCK_OFFSET_TOP 10
#define BLOCK_OFFSET_LEFT 0
#define BLOCK_LANE 3
#define BLOCK_RAND 4
#define BLOCK_END_WIDTH 3
#define BLOCK_SEPARATOR_WIDTH 2
#define BLOCK_LANE_BIT (1<<BLOCK_LANE)-1
#define SCREEN_X 128
#define SCREEN_Y 64
#define CAR_WIDTH 16
#define CAR_HEIGHT 8

typedef struct {
    union {
        uint8_t u8button;
        struct {
            uint8_t up : 1;
            uint8_t down : 1;
        } button;
    };
} fsm_input_t;

typedef enum {
    FSM_IDLE,
    FSM_GAME_MENU,
    FSM_GAME_PLAY,
    FSM_GAME_OVER
} fsm_state_t;

typedef struct {
    uint16_t score;
    uint8_t ncount;
    uint8_t rndcount;
    uint8_t carpos;
    uint16_t obstaclepos[BLOCK_LANE];
} fsm_output_t;

typedef struct {
    fsm_input_t input;
    fsm_input_t lastInput;
    fsm_state_t state;
    fsm_state_t lastState;
    fsm_output_t output;
    fsm_output_t lastOutput;
} fsm_t;

void FSM_init(fsm_t *fsm);
void FSM_isPressed(fsm_input_t *ret, fsm_t *fsm);
void FSM_input(fsm_t *fsm);
void FSM_process(fsm_t *fsm);
void FSM_output(fsm_t *fsm);

#endif /* __FSM_H__ */
