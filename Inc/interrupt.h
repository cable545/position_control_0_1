#ifndef INTERRUPT_H
#define INTERRUPT_H

/*ARM Cortex M0 implementation for interrupt priority shift*/
#define ARM_INTERRUPT_LEVEL_BITS          2

#define EnableInterrupts __asm(" CPSIE i");
#define DisableInterrupts __asm(" CPSID i");

void enable_irq (int irq);
void disable_irq (int irq);
void set_irq_priority (int irq, int prio);

#endif
