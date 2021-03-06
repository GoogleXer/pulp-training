/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Pasquale Davide Schiavone (pschiavo@iis.ee.ethz.ch)
 */

#include <rt/rt_api.h>

static unsigned int ninterrupts = 0;

// interupt service routine of the timer
void __attribute__((interrupt)) __test_timer_handler()
{
  ninterrupts++;
  //disable timer and interrupt
  hal_timer_conf(
    hal_timer_fc_addr(0, 1), PLP_TIMER_DIS, PLP_TIMER_RESET_DISABLED,
    PLP_TIMER_IRQ_DISABLED, PLP_TIMER_IEM_DISABLED, PLP_TIMER_CMPCLR_DISABLED,
    PLP_TIMER_ONE_SHOT_DISABLED, PLP_TIMER_REFCLK_ENABLED,
    PLP_TIMER_PRESCALER_DISABLED, 0, PLP_TIMER_MODE_64_DISABLED
  );

  //clear pending interrupt of IRQ ARCHI_FC_EVT_TIMER1
  hal_itc_status_clr (1<<ARCHI_FC_EVT_TIMER1);

}

int main()
{

  int irq = rt_irq_disable();
  unsigned int ticks, ticks_from_now;
  unsigned int us = 100;

  // First compute the number of ticks corresponding to 100 micro-second.
  // The specified time is the minimum we must, so we have to round-up
  // the number of ticks.
  ticks = us / ( 1000000 / ARCHI_REF_CLOCK) + 1;

  //read actual value in timer 0, sub-timer 1
  uint32_t current_time = hal_timer_count_get(hal_timer_fc_addr(0, 1));
  
  uint32_t timer = current_time + ticks;
  
  //set value to perform comparison
  hal_timer_cmp_set(hal_timer_fc_addr(0, 1), timer);
  
  //disable all the interrupts in the INTC
  hal_itc_enable_value_set(0);
  
  //enable the interrupts of the timer in the INTC
  //hal_itc_enable_set(mask);
  /* ADD HERE */
  
  //Set the ISR
  //rt_irq_set_handler (interrupt number, function pointer);
  /* ADD HERE */
  
  printf("Going to sleep.... (ninterrupts %d) \n",ninterrupts );
  
  //enable timer and interrupt generation
  hal_timer_conf(
    hal_timer_fc_addr(0, 1), PLP_TIMER_ACTIVE, PLP_TIMER_RESET_DISABLED,
    PLP_TIMER_IRQ_ENABLED, PLP_TIMER_IEM_DISABLED, PLP_TIMER_CMPCLR_DISABLED,
    PLP_TIMER_ONE_SHOT_DISABLED, PLP_TIMER_REFCLK_ENABLED,
    PLP_TIMER_PRESCALER_DISABLED, 0, PLP_TIMER_MODE_64_DISABLED
  );

  rt_irq_restore(irq);

  hal_itc_wait_for_interrupt();
  
  irq = rt_irq_disable();

  unsigned int wokeup = hal_timer_count_get(hal_timer_fc_addr(0, 1)) * ( 1000000 / ARCHI_REF_CLOCK);

  printf("Woken up.... (ninterrupts %d)\n", ninterrupts);

  rt_irq_restore(irq);

  return 0;
}

