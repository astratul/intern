# Interrupts and Interrupt Handlers

## Interrupts

Interrupts enable hardware to signal to the processor.The processor receives the interrupt and signals the operating system to enable the operating system to respond to the new data. Hardware devices generate interrupts asynchronously with respect to the processor clock—they can occur at any time. Consequently, the kernel can be interrupted at any time to process interrupts. An interrupt is physically produced by electronic signals originating from hardware devices and directed into input pins on an interrupt controller, a simple chip that multi-plexes multiple interrupt lines into a single line to the processor. Upon receiving an interrupt, the interrupt controller sends a signal to the processor.The processor detects this signal and interrupts its current execution to handle the interrupt.The processor can then notify the operating system that an interrupt has occurred, and the operating system can handle the interrupt appropriately.
Different devices can be associated with different interrupts by means of a unique value associated with each interrupt.This way, interrupts from the keyboard are distinct from interrupts from the hard drive.This enables the operating system to differentiate between interrupts and to know which hardware device caused which interrupt. In turn, the operating system can service each interrupt with its corresponding handler. These interrupt values are often called interrupt request (IRQ) lines. Each IRQ line is assigned a numeric value.

## Interrupt Handlers

The function the kernel runs in response to a specific interrupt is called an interrupt handler or interrupt service routine (ISR). Each device that generates interrupts has an associated interrupt handler.
In Linux, interrupt handlers are normal C functions.They match a specific prototype, which enables the kernel to pass the handler information in a standard way, but otherwise they are ordinary functions.What differentiates interrupt handlers from other kernel functions is that the kernel invokes them in response to interrupts and that they run in a special context (discussed later in this chapter) called interrupt context.This special context is occasionally called atomic context because, as we shall see, code executing in this context is unable to block.

## Top Halves Versus Bottom Halves


## Registering an Interrupt Handler


### Interrupt Handler Flags

### An Interrupt Example

### Freeing an Interrupt Handler

## Writing an Interrupt Handler


### Shared Handlers

### A Real-Life Interrupt Handler

## Interrupt Context


## Implementing Interrupt Handlers


### /proc/interrupts

## Interrupt Control


### Disabling and Enabling Interrupts

### Disabling a Specific Interrupt Line

### Status of the Interrupt System
