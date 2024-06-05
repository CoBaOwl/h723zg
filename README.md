# STM32H723ZG CMSIS RCC config max frequency

## Description
This code help you to understand how RCC in STM32H7 work. It is not so hard to understand if you know how RCC work on F1 or F4 mcu. 

Function rcc_init can switch between 500 and 550 MHz. When **CPU_FREQ_BOOST** enabled **ITCM-RAM** and **DTCM-RAM** in disabled.

This flag is need when CPU run on 550MHz.
```C
SYSCFG->UR18 |= SYSCFG_UR18_CPU_FREQ_BOOST;
```

This section needed if freq up to 400Mhz
```C
PWR->D3CR &= ~PWR_D3CR_VOS;
while(!(PWR->D3CR & PWR_D3CR_VOSRDY));
```

And this section needed if freq up to 520MHz withiout **CPU_FREQ_BOOST**
```C
PWR->CR3 &= ~PWR_CR3_SCUEN;
while(!(PWR->CSR1 & PWR_CSR1_ACTVOSRDY));
```
## Requirements
This repo build with **PlatformIO**
