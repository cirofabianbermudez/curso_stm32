# STM32



 ## ARM

Cuando decimos ARM nos podemos estar refiriendo a dos cosas, a una arquitectura de computadora o a un núcleo de un procesador.



## Generalidades

Arquitectura RISC (Reduced Instruction Set Computer) con ISA (Instruction Set Architecture) Load/Store



## Registros

* General purpose registers
  * Low registers
  * High registers
* Stack pointer
* Link Register
* Program counter
* Special registers
  * Program status register
  * Exception mask register
  * Control register
* FPU registers (Floating Point registers)





* Blue Pill
* Discovery
* Nucleo
* Eval





* F0, F1, F2, F3, F4
* F0 vs L0
* Cortex M0



## Funciones

```c
HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
HAL_Delay(i);
HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

```



## NUCLEo STM32G431RB

* Part Number: STM32G431RBT6
* STM32CUBEMX
  * Sirve para realizar las configuraciones del microcontrolador utilizando una GUI



## Maneras para programas

* CMSIS
* HAL (Hardware Abstraction Layer)
* HAL

## Links

* https://developer.arm.com/Processors/Cortex-M4
* https://www.st.com/en/evaluation-tools/nucleo-g431rb.html
* https://www.st.com/en/microcontrollers-microprocessors/stm32g431rb.html

* https://www.st.com/en/microcontrollers-microprocessors.html
* https://www.st.com/content/st_com/en/support/learning/stm32-education/stm32-online-training/stm32g4-online-training.html

* https://www.youtube.com/watch?v=rfBeq-Fu0hc