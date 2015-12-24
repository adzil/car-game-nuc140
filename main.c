#include <inttypes.h>
#include <stdlib.h>
#include "NUC100Series.h"
#include "cmsis_os.h"
#include "SYS.h"
#include "LCD.h"
#include "GPIO.h"
#include "fsm.h"

#define PLL_CLOCK           50000000


fsm_t fsm;


void SYS_Init(void) {
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    
    /* Enable Internal RC 22.1184MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

    /* Enable external XTAL 12MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);
    
}

int main() {
    /* Init System, peripheral clock and multi-function I/O */
    SYS_UnlockReg();
    SYS_Init();
    SYS_LockReg();
    
    /* Turn on the lamp */
    DrvGPIO_Open(E_GPD, 14, E_IO_OUTPUT);
    DrvGPIO_ClrBit(E_GPD, 14);
    /* Set up buzzer */
    DrvGPIO_Open(E_GPB, 11, E_IO_OUTPUT);
    DrvGPIO_SetBit(E_GPB, 11);
    
    init_LCD();
    clear_LCD();
    FSM_init(&fsm);
    
    /* Main loop */
    while (1) {
        FSM_input(&fsm);
        FSM_process(&fsm);
        FSM_output(&fsm);
        
        osDelay(5);
        
        /*
        osDelay(500);
        printS_5x7(60, 10, "A");
        osDelay(500);
        printS_5x7(60, 10, "B");*/
        /*
        osDelay(500);
        DrvGPIO_ClrBit(E_GPB, 11);
        osDelay(500);
        DrvGPIO_SetBit(E_GPB, 11);*/
    }
}
