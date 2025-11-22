.. _bee_f407zet6_board:

Bee F407ZET6
############

Overview
********

The Bee F407ZET6 board is based on the STMicroelectronics STM32F407ZET6 MCU
with ARM Cortex-M4 core. It features:

- STM32F407ZET6 in LQFP144 package
- ARM |reg| Cortex |reg|-M4 Core at 168MHz
- 512 KB Flash memory
- 192 KB SRAM + 64 KB CCMRAM
- Various peripherals including USART, CAN, SPI, I2C
- On-board LEDs

.. image:: img/bee_f407zet6.jpg
   :width: 442px
   :align: center
   :alt: Bee F407ZET6

Hardware
********

Supported Features
==================

The Bee F407ZET6 board configuration supports the following hardware features:

+-----------+------------+-------------------------------------+
| Interface | Controller | Driver/Component                    |
+===========+============+=====================================+
| NVIC      | on-chip    | nested vector interrupt controller  |
+-----------+------------+-------------------------------------+
| SYSTICK   | on-chip    | systick                             |
+-----------+------------+-------------------------------------+
| UART      | on-chip    | serial port-polling;                |
|           |            | serial port-interrupt               |
+-----------+------------+-------------------------------------+
| GPIO      | on-chip    | gpio                                |
+-----------+------------+-------------------------------------+
| PINMUX    | on-chip    | pinmux                              |
+-----------+------------+-------------------------------------+
| CLOCK     | on-chip    | clock_control                       |
+-----------+------------+-------------------------------------+
| FLASH     | on-chip    | flash                               |
+-----------+------------+-------------------------------------+
| RNG       | on-chip    | rng                                 |
+-----------+------------+-------------------------------------+
| WATCHDOG  | on-chip    | independent watchdog                |
+-----------+------------+-------------------------------------+

Other hardware features are not currently supported by the Zephyr kernel.

Connections and IOs
===================

LED
---

- LED0 (RED)   = PC1
- LED1 (BLUE)  = PC2
- LED2 (GREEN) = PC3

System Clock
============

The STM32F407ZET6 MCU is configured to use the 12 MHz external crystal
with the PLL to achieve a system clock of 168 MHz.

Serial Port
===========

USART1 is available on PB7 (RX) and PA9 (TX).

Programming and Debugging
*************************

Applications for the ``bee_f407zet6`` board configuration can be built and
flashed in the usual way (see :ref:`build_an_application` and
:ref:`application_run` for more details).

Flashing
========

The board doesn't currently support an onboard debugger. An external
programmer/debugger is needed to flash the application.

References
**********

- `STM32F407ZET6 Website`_
- `STM32F407ZE Datasheet`_

.. _STM32F407ZET6 Website:
   https://www.st.com/en/microcontrollers-microprocessors/stm32f407ze.html

.. _STM32F407ZE Datasheet:
   https://www.st.com/resource/en/datasheet/stm32f407ze.pdf