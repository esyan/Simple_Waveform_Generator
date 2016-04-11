
/** @file TIMER_DRV.c
 *  @brief TIMER Driver for the STM32F072RB.
 *
 *	@details The current Timer driver only supports the basic timers of the
 *	STM32F072RB. The timers which are supported are TIMER 6 and TIMER 7.
 *
 *  @author Dennis Law
 *  @date April 2016
 */
 
#include "TIMER_DRV.h"

/** Pointers to callback functions */
void (*TIMER6_callbackFunction)(void);
void (*TIMER7_callbackFunction)(void);

/** @brief Sets the auto reload register of a basic timer.
 *	@param tim Base pointer to the timer to be configured. The value
 *	for this argument can either be TIM6 or TIM7.
 *	@param val The value to be written.
 *	@returns 0 if successful and -1 if otherwise.
 */
int TIMER_setCount(TIM_TypeDef *tim, int16_t val)
{
	if ((tim != TIM6) && (tim != TIM7))
		return -1;

	tim->ARR = val;
	return 0;
}

/** @brief Sets the prescaler of a basic timer.
 *	@param tim Base pointer to the timer to be configured. The value
 *	for this argument can either be TIM6 or TIM7.
 *	@param val The value to be written.
 *	@returns 0 if successful and -1 if otherwise.
 */
int TIMER_setPrescaler(TIM_TypeDef *tim, int16_t val)
{
	if ((tim != TIM6) && (tim != TIM7))
		return -1;

	tim->PSC = val;
	return 0;
}

/** @brief Disable the counting of a basic timer.
 *	@param tim Base pointer to the timer to be configured. The value
 *	for this argument can either be TIM6 or TIM7.
 *	@returns 0 if successful and -1 if otherwise.
 */
int TIMER_disable(TIM_TypeDef *tim)
{
	if ((tim != TIM6) && (tim != TIM7))
		return -1;

	tim->CR1 &= ~(TIM_CR1_CEN);
	return 0;
}

/** @brief Enables the counting of a basic timer.
 *	@param tim Base pointer to the timer to be configured. The value
 *	for this argument can either be TIM6 or TIM7.
 *	@returns 0 if successful and -1 if otherwise.
 */
int TIMER_enable(TIM_TypeDef *tim)
{
	if ((tim != TIM6) && (tim != TIM7))
		return -1;

	tim->CR1 |= TIM_CR1_CEN;
	return 0;
}

/** @brief Enables the clock for a basic timer peripheral.
 *	@param tim Base pointer to the timer to be configured. The value
 *	for this argument can either be TIM6 or TIM7.
 *	@returns 0 if sucessful and -1 if otherwise.
 */
int TIMER_enableClock(TIM_TypeDef *tim)
{
	if (tim == TIM6)
		RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	else if (tim == TIM7)
		RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
	else
		return -1;

	return 0;
}

/** @brief Initializes a basic timer
 *	@param tim Base pointer of the timer to initializr. The value for this
 *	argument can either be TIM6 or TIM7.
 *	@conf Configuration parameters for setting up the timer.
 *	@callback Interrupt callback function.
 *	@returns 0 if successful and -1 if otherwise.
 */
int TIMER_init(TIM_TypeDef *tim, struct TIMER_config conf,
				void (*callback)(void))
{
	if ((tim != TIM6) & (tim != TIM7))
		return -1;

	if(!callback)
		return -1;

	TIMER_enableClock(tim);

	tim->CR1 |= TIM_CR1_ARPE; /* ARR register is buffered */

	TIMER_setCount(tim, conf.count);
	TIMER_setPrescaler(tim, conf.prescale);

	tim->SR &= ~(TIM_SR_UIF); /* Clear interrupt flag */
	tim->DIER |= TIM_DIER_UIE; /* Enable interrupt */

	if (tim == TIM6) {
		NVIC_EnableIRQ(TIM6_DAC_IRQn);
		TIMER6_callbackFunction = callback;
	} else {
		NVIC_EnableIRQ(TIM7_IRQn);
		TIMER7_callbackFunction = callback;
	}

	return 0;
}

/** @brief IRQ Handler for Timer 6
 *	@details The interrupt flag for Timer 6 will be cleared before calling
 *	the callback function.
 */
void TIM6_DAC_IRQHandler(void)
{
	TIM6->SR &= ~(TIM_SR_UIF);

	if (TIMER6_callbackFunction)
		TIMER6_callbackFunction();
}

/** @brief IRQ Handler for Timer 7
 *	@details The interrupt flag for Timer 7 will be cleared before calling
 *	the callback function.
 */
void TIM7_IRQHandler(void)
{
	TIM7->SR &= ~(TIM_SR_UIF);

	if (TIMER7_callbackFunction)
		TIMER7_callbackFunction();
}
