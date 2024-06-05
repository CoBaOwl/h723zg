#include "stm32h7xx.h"

void rcc_init(uint8_t boost = 0) {
    //----------------PLL1 Configuration----------------------------
    //Multiplier for PLL1
    //                  40 for 500 MHz
    //                  44 for 550 MHz
    uint8_t multi = 40;
    while(!(PWR->D3CR & PWR_D3CR_VOSRDY));
    PWR->CR3 &= ~PWR_CR3_SCUEN;
    while(!(PWR->CSR1 & PWR_CSR1_ACTVOSRDY));
    PWR->D3CR &= ~PWR_D3CR_VOS;
    while(!(PWR->D3CR & PWR_D3CR_VOSRDY));
    //BOOST for 550Mhz
    if (boost) {
        multi = 44;
        SYSCFG->UR18 |= SYSCFG_UR18_CPU_FREQ_BOOST;
    }
    SCB->CPACR |= ((1 << 23) | (1 << 20));
    RCC->CR |= RCC_CR_HSEON;
    while(!(RCC->CR & RCC_CR_HSERDY));
    RCC->PLLCKSELR &= ~RCC_PLLCKSELR_DIVM1;
    RCC->PLLCKSELR |= RCC_PLLCKSELR_PLLSRC_HSE | (2 << RCC_PLLCKSELR_DIVM1_Pos);
    RCC->PLL1DIVR = ((multi - 1) << RCC_PLL1DIVR_N1_Pos) | (1 << RCC_PLL1DIVR_Q1_Pos) | (1 << RCC_PLL1DIVR_R1_Pos);
    RCC->PLLCFGR |= RCC_PLLCFGR_PLL1RGE_3 | RCC_PLLCFGR_DIVQ1EN | RCC_PLLCFGR_DIVP1EN | RCC_PLLCFGR_DIVR1EN;
    RCC->CR |= RCC_CR_PLL1ON;
    while(!(RCC->CR & RCC_CR_PLL1RDY));
    //--------------------------------------------------------------
    //----------------Peripheral clock configuration----------------
    FLASH->ACR &= ~(FLASH_ACR_LATENCY);
    FLASH->ACR |= FLASH_ACR_LATENCY_3WS;
    RCC->D1CFGR = RCC_D1CFGR_D1PPRE_DIV2 | RCC_D1CFGR_HPRE_DIV2;
    RCC->D2CFGR = RCC_D2CFGR_D2PPRE2_DIV2 | RCC_D2CFGR_D2PPRE1_DIV2;
    RCC->D3CFGR = RCC_D3CFGR_D3PPRE_DIV2;
    RCC->CFGR |= RCC_CFGR_SW_PLL1;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL1);
    //--------------------------------------------------------------
}

void spi_init(void) {
}

int main(void) {
    rcc_init();
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOGEN;
    GPIOG->MODER &= ~(GPIO_MODER_MODE7_1);

    RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;

    TIM15->DIER |= TIM_DIER_UIE;
    TIM15->PSC = 22500 - 1;
    TIM15->ARR = 10000;
    NVIC_EnableIRQ(TIM15_IRQn);
    TIM15->CR1 |= TIM_CR1_CEN;

    while(1) {
        __NOP();
    }
}

extern "C" {
    void TIM15_IRQHandler(void) {
        if(TIM15->SR & TIM_SR_UIF) {
            TIM15->SR = ~TIM_SR_UIF;
            GPIOG->ODR ^= GPIO_ODR_OD7;
        }
    }
}