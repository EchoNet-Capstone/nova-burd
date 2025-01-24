/*
This is code from the arduino for watchdog timers
that is being moved into the scope,
this is done to break up the header file into 
a header and source file so it is 
compatable with platform.io
*/
#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <cytypes.h>
#include <CyLib.h>

#define wdt_isr__INTC_CLR_EN_REG CYREG_CM0P_ICER
#define wdt_isr__INTC_CLR_PD_REG CYREG_CM0P_ICPR
#define wdt_isr__INTC_MASK 0x40u
#define wdt_isr__INTC_NUMBER 6u
#define wdt_isr__INTC_PRIOR_MASK 0xC00000u
#define wdt_isr__INTC_PRIOR_NUM 3u
#define wdt_isr__INTC_PRIOR_REG CYREG_CM0P_IPR1
#define wdt_isr__INTC_SET_EN_REG CYREG_CM0P_ISER
#define wdt_isr__INTC_SET_PD_REG CYREG_CM0P_ISPR

/* Interrupt Controller Constants */
/* Address of the INTC.VECT[x] register that contains the Address of the wdt_isr ISR. */
#define wdt_isr_INTC_VECTOR            ((reg32 *) wdt_isr__INTC_VECT)

/* Address of the wdt_isr ISR priority. */
#define wdt_isr_INTC_PRIOR             ((reg32 *) wdt_isr__INTC_PRIOR_REG)

/* Priority of the wdt_isr interrupt. */
#define wdt_isr_INTC_PRIOR_NUMBER      wdt_isr__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable wdt_isr interrupt. */
#define wdt_isr_INTC_SET_EN            ((reg32 *) wdt_isr__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the wdt_isr interrupt. */
#define wdt_isr_INTC_CLR_EN            ((reg32 *) wdt_isr__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the wdt_isr interrupt state to pending. */
#define wdt_isr_INTC_SET_PD            ((reg32 *) wdt_isr__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the wdt_isr interrupt. */
#define wdt_isr_INTC_CLR_PD            ((reg32 *) wdt_isr__INTC_CLR_PD_REG)

#if !defined(wdt_isr__REMOVED) /* Check for removal by optimization */

extern cyisraddress CyRamVectors[CYINT_IRQ_BASE + CY_NUM_INTERRUPTS];

/* Declared in startup, used to set unused interrupts to. */
CY_ISR_PROTO(IntDefaultHandler);

/* Interrupt Controller API. */
void wdt_isr_Start(void);
void wdt_isr_StartEx(cyisraddress address);
void wdt_isr_Stop(void);

CY_ISR_PROTO(wdt_isr_Interrupt);

void wdt_isr_SetVector(cyisraddress address);
cyisraddress wdt_isr_GetVector(void);

void wdt_isr_SetPriority(uint8 priority);
uint8 wdt_isr_GetPriority(void);

void wdt_isr_Enable(void);
uint8 wdt_isr_GetState(void);
void wdt_isr_Disable(void);

void wdt_isr_SetPending(void);
void wdt_isr_ClearPending(void);

void feedInnerWdt();
void innerWdtEnable(bool feed);

#endif
#endif