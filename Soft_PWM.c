#include "Soft_PWM.h"

static volatile SoftPWM_Channel channels[SOFT_PWM_MAX_CH];
static uint8_t channel_count = 0;

static uint16_t pwm_period = 100;
static volatile uint16_t counter = 0;

static uint32_t APB1_CLK, APB2_CLK;

/* ================= GET CLK ================= */
int power(int base, int exp) {
    int result = 1;
    for(int i = 0; i < exp; i++) {
        result *= base;
    }
    return result;
}
void Get_CLK()
{
	uint32_t sysclk = 0;
	uint32_t HCLK =0;
	uint8_t PLLMUL_value = 0;
	uint8_t AHB_PSC = 0, APB1_PSC = 0, APB2_PSC = 0;
	
	// if use HSI 8MHz and HSI selected as system clock
	if(RCC-> CR & (1<<0) && (RCC-> CFGR & (1<<0)) == 0) sysclk = 8000000;
	else if(RCC-> CR & (1<<16) && (RCC-> CFGR & (1<<0)) == 1) sysclk = HSE_CLK;
	else if(RCC-> CR & (1<<16) && (RCC-> CFGR & (1<<0)) == 2)
	{
		PLLMUL_value = (( RCC-> CFGR >> 18 ) & 0x0F );
		if(PLLMUL_value == 15) PLLMUL_value = 14;
		sysclk = HSE_CLK*( PLLMUL_value + 2);
	}
	
	AHB_PSC = (( RCC-> CFGR >> 4 ) & 0x0F );
	if( AHB_PSC >= 8 && AHB_PSC < 12) HCLK = (sysclk / power(2, AHB_PSC - 7));
	else if(AHB_PSC >= 12 && AHB_PSC < 16) HCLK = (sysclk / power(2, AHB_PSC - 6));
	else HCLK = sysclk;
	
	// cal APB1 clock
	APB1_PSC = (( RCC-> CFGR >> 8 ) & 0x07 );
	if( APB1_PSC >= 4 && APB1_PSC < 8) APB1_CLK = (HCLK / power(2, APB1_PSC - 3));
	else APB1_CLK = HCLK;
	
	// cal APB2 clock
	APB2_PSC = (( RCC-> CFGR >> 8 ) & 0x07 );
	if( APB2_PSC >= 4 && APB2_PSC < 8) APB2_CLK = (HCLK / power(2, APB2_PSC - 3));
	else APB2_CLK = HCLK;
}

/* ================= GPIO OUTPUT ================= */
static uint8_t GPIO_PinToNumber(uint16_t pin)
{
    for (uint8_t i = 0; i < 16; i++)
    {
        if (pin == (1 << i))
            return i;
    }
    return 0xFF; // loi neu khong hop le
}
static void GPIO_Output_Init(GPIO_TypeDef *GPIOx, uint16_t pin)
{
		// enable clock for port x
    if (GPIOx == GPIOA) RCC->APB2ENR |= (1<<2);
    else if (GPIOx == GPIOB) RCC->APB2ENR |= (1<<3);
    else if (GPIOx == GPIOC) RCC->APB2ENR |= (1<<4);
		else if (GPIOx == GPIOD) RCC->APB2ENR |= (1<<5);
    else if (GPIOx == GPIOE) RCC->APB2ENR |= (1<<6);

		// configure pin
		uint8_t pin_ = GPIO_PinToNumber(pin);
    uint8_t pos = (pin_ * 4);

    if (pin_ < 8)
    {
        GPIOx->CRL &= ~(0x0F << pos);
        GPIOx->CRL |=  (0x03 << pos); // Output 50MHz push-pull
    }
    else
    {
        pos = (pin_ - 8) * 4;
        GPIOx->CRH &= ~(0x0F << pos);
        GPIOx->CRH |=  (0x03 << pos);
    }
}

/* ================= TIMER INIT ================= */
void SoftPWM_Init(uint16_t period)
{
    pwm_period = period;

    RCC->APB1ENR |= (1<<0); // TIM2 enable
		
		Get_CLK();

    TIM2->PSC = 9;     // 1MHz (72MHz / 72)
    TIM2->ARR = (APB1_CLK/100000) - 1;     // interrupt moi 10µs

    TIM2->DIER |= (1<<0);   // Update interrupt enable
    TIM2->CR1  |= (1<<0);   // Enable timer

    NVIC-> ISER[0] |= (1<<28); //Enable Global interrupt TIM2
}

/* ================= ADD CHANNEL ================= */
void SoftPWM_AddChannel(GPIO_TypeDef *port, uint16_t pin)
{
    if (channel_count >= SOFT_PWM_MAX_CH) return ;

    GPIO_Output_Init(port, pin);

    channels[channel_count].port = port;
    channels[channel_count].pin  = pin;
    channels[channel_count].duty = 0;
    channels[channel_count].mode = 1;
	
		channel_count ++;
}

/* ================= SET DUTY ================= */
void SoftPWM_SetDuty(uint8_t channel, uint16_t duty)
{
    if (channel >= channel_count) return;

    if (duty > pwm_period) duty = pwm_period;

    channels[channel].duty = duty;
}

/* ================= CHANGE MODE ================= */
void SoftPWM_ChangeMode(uint8_t channel, uint8_t mode)
{
    channels[channel].mode = mode;
}

/* ================= TIMER ISR ================= */
void TIM2_IRQHandler(void)
{
		static uint8_t pin_;
    if (TIM2->SR & 1)
    {
        TIM2->SR &= ~(1<<0); // clear flag interrupt

        counter++;
        if (counter >= pwm_period) counter = 0;

        for (uint8_t i = 0; i < channel_count; i++)
        {
						pin_ = GPIO_PinToNumber(channels[i].pin);
            if (counter < channels[i].duty)
						{
							if(channels[i].mode == 1) channels[i].port->BSRR |=  (1 << pin_);
							else channels[i].port->BSRR |= (1 << (pin_ + 16));
						}
            else
						{
							if(channels[i].mode == 1) channels[i].port->BSRR |= (1 << (pin_ + 16));
							else channels[i].port->BSRR |=  (1 << pin_);
                
						}
        }
    }
}
