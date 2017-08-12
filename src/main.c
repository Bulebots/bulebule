#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>


/**
 * @brief Initial clock setup.
 *
 * Use the External High Speed clock (HSE), at 8 MHz, and set the SYSCLK
 * at 72 MHz (the maximum allowed when using the external crystal/resonator).
 * This output frequency is possible thanks to the Phase Locked Loop (PLL)
 * multiplier.
 *
 * Enable required clocks for the GPIOs and timers as well.
 */
static void setup_clock(void)
{
	rcc_clock_setup_in_hse_8mhz_out_72mhz();
	rcc_periph_clock_enable(RCC_GPIOC);

	/* Encoder timers */
	rcc_periph_clock_enable(RCC_TIM1);
	rcc_periph_clock_enable(RCC_TIM4);
}


/**
 * @brief Initial GPIO configuration.
 *
 * Set GPIO modes and initial states.
 */
static void setup_gpio(void)
{
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
	gpio_clear(GPIOC, GPIO13);
}


/**
 * @brief Configure timer to read a quadrature encoder.
 *
 * - Set the Auto-Reload Register (TIMx_ARR).
 * - Set the encoder interface mode counting on both TI1 and TI2 edges.
 * - Configure inputs.
 * - Enable counter.
 *
 * @param[in] timer_peripheral Timer register address base to configure.
 *
 * @see Reference manual (RM0008) "TIMx functional description" and in
 * particular "Encoder interface mode" section.
 */
static void configure_timer_as_quadrature_encoder(uint32_t timer_peripheral)
{
	timer_set_period(timer_peripheral, 0xFFFF);
	timer_slave_set_mode(timer_peripheral, 0x3);
	timer_ic_set_input(timer_peripheral, TIM_IC1, TIM_IC_IN_TI1);
	timer_ic_set_input(timer_peripheral, TIM_IC2, TIM_IC_IN_TI2);
	timer_enable_counter(timer_peripheral);
}


/**
 * @brief Setup timers for the motor encoders.
 *
 * TIM1 for the left motor and TIM4 for the right motor are configured.
 */
static void setup_encoders(void)
{
	configure_timer_as_quadrature_encoder(TIM1);
	configure_timer_as_quadrature_encoder(TIM4);
}


/**
 * @brief Set SysTick interruptions frequency and enable SysTick counter.
 *
 * SYSCLK is at 72 MHz as well as the Advanced High-permormance Bus (AHB)
 * because, by default, the AHB divider is set to 1, so the AHB clock has the
 * same frequency as the SYSCLK.
 *
 * @see STM32F103x8/STM32F103xB data sheet and in particular the "Clock tree"
 * figure.
 */
static void setup_systick(void)
{
	systick_set_frequency(1, 72000000);
	systick_counter_enable();
	systick_interrupt_enable();
}


/**
 * @brief Handle the SysTick interruptions.
 */
void sys_tick_handler(void)
{
	gpio_toggle(GPIOC, GPIO13);
}


/**
 * @brief Initial setup and infinite wait.
 */
int main(void)
{
	setup_clock();
	setup_gpio();
	setup_encoders();
	setup_systick();

	while(1) {
		int left = timer_get_counter(TIM1);
		int right = timer_get_counter(TIM4);
		for (int i = 0; i < 800000; i++)
			__asm__("nop");
	}

	return 0;
}
