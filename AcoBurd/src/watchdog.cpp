
#include "watchdog.hpp"
#include "Arduino.h"
#include <cytypes.h>
#include <cyfitter.h>
#include <cydevice_trm.h>


void wdt_isr_Start(void)
{
    /* For all we know the interrupt is active. */
    wdt_isr_Disable();

    /* Set the ISR to point to the wdt_isr Interrupt. */
    wdt_isr_SetVector(&wdt_isr_Interrupt);

    /* Set the priority. */
    wdt_isr_SetPriority((uint8)wdt_isr_INTC_PRIOR_NUMBER);

    /* Enable it. */
    wdt_isr_Enable();
}


void wdt_isr_StartEx(cyisraddress address)
{
    /* For all we know the interrupt is active. */
    wdt_isr_Disable();

    /* Set the ISR to point to the wdt_isr Interrupt. */
    wdt_isr_SetVector(address);

    /* Set the priority. */
    wdt_isr_SetPriority((uint8)wdt_isr_INTC_PRIOR_NUMBER);

    /* Enable it. */
    wdt_isr_Enable();
}


void wdt_isr_Stop(void)
{
    /* Disable this interrupt. */
    wdt_isr_Disable();

    /* Set the ISR to point to the passive one. */
    wdt_isr_SetVector(&IntDefaultHandler);
}


CY_ISR(wdt_isr_Interrupt)
{
    #ifdef wdt_isr_INTERRUPT_INTERRUPT_CALLBACK
        wdt_isr_Interrupt_InterruptCallback();
    #endif /* wdt_isr_INTERRUPT_INTERRUPT_CALLBACK */ 

    /*  Place your Interrupt code here. */
    /* `#START wdt_isr_Interrupt` */

    /* `#END` */
}

void wdt_isr_SetVector(cyisraddress address)
{
    CyRamVectors[CYINT_IRQ_BASE + wdt_isr__INTC_NUMBER] = address;
}


cyisraddress wdt_isr_GetVector(void)
{
    return CyRamVectors[CYINT_IRQ_BASE + wdt_isr__INTC_NUMBER];
}


void wdt_isr_SetPriority(uint8 priority)
{
  uint8 interruptState;
    uint32 priorityOffset = ((wdt_isr__INTC_NUMBER % 4u) * 8u) + 6u;
    
  interruptState = CyEnterCriticalSection();
    *wdt_isr_INTC_PRIOR = (*wdt_isr_INTC_PRIOR & (uint32)(~wdt_isr__INTC_PRIOR_MASK)) |
                                    ((uint32)priority << priorityOffset);
  CyExitCriticalSection(interruptState);
}


uint8 wdt_isr_GetPriority(void)
{
    uint32 priority;
  uint32 priorityOffset = ((wdt_isr__INTC_NUMBER % 4u) * 8u) + 6u;

    priority = (*wdt_isr_INTC_PRIOR & wdt_isr__INTC_PRIOR_MASK) >> priorityOffset;

    return (uint8)priority;
}


void wdt_isr_Enable(void)
{
    /* Enable the general interrupt. */
    *wdt_isr_INTC_SET_EN = wdt_isr__INTC_MASK;
}


uint8 wdt_isr_GetState(void)
{
    /* Get the state of the general interrupt. */
    return ((*wdt_isr_INTC_SET_EN & (uint32)wdt_isr__INTC_MASK) != 0u) ? 1u:0u;
}


void wdt_isr_Disable(void)
{
    /* Disable the general interrupt. */
    *wdt_isr_INTC_CLR_EN = wdt_isr__INTC_MASK;
}


void wdt_isr_SetPending(void)
{
    *wdt_isr_INTC_SET_PD = wdt_isr__INTC_MASK;
}


void wdt_isr_ClearPending(void)
{
    *wdt_isr_INTC_CLR_PD = wdt_isr__INTC_MASK;
}

void feedInnerWdt()
{
  CySysWdtClearInterrupt();
  wdt_isr_ClearPending();
}

void innerWdtEnable(bool feed)
{
    /* Start the WDT */
    CySysWdtEnable();
    CySysWdtWriteMatch(0xFFFF);
    if(feed)
    {
      /* Start the WDT */
      wdt_isr_StartEx(feedInnerWdt);
      
      /* Make sure that interrupt is forwarded to the CPU */
      CySysWdtUnmaskInterrupt();
    }
}
