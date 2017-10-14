#include <stdio.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/usart.h>

#include "logging.h"
#include "setup.h"

/**
 * @brief Initial clock setup.
 *
 * Use the External High Speed clock (HSE), at 8 MHz, and set the SYSCLK
 * at 72 MHz (the maximum allowed when using the external crystal/resonator).
 * This output frequency is possible thanks to the Phase Locked Loop (PLL)
 * multiplier.
 *
 * The prescalers are set to:
 *
 * - AHB to 1 with output at 72 MHz (max. is 72 MHz).
 * - ADC to 8 with output at 9 MHz (max. is 14 MHz).
 * - APB1 to 2 with output at 36 MHz (max. is 36 MHz).
 * - APB2 to 1 with output at 72 MHz (max. is 72 MHz).
 *
 * Enable required clocks for the GPIOs and timers as well.
 *
 * @see Reference manual (RM0008) "Connectivity line devices: reset and clock
 * control (RCC)" and in particular "Clocks" section.
 */
static void setup_clock(void)
{
	rcc_clock_setup_in_hse_8mhz_out_72mhz();
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);

	/* Bluetooth */
	rcc_periph_clock_enable(RCC_USART3);

	/* Encoders */
	rcc_periph_clock_enable(RCC_TIM2);
	rcc_periph_clock_enable(RCC_TIM4);

	/* PWM */
	rcc_periph_clock_enable(RCC_TIM3);

	/* Alternate functions */
	rcc_periph_clock_enable(RCC_AFIO);

	/* ADC */
	rcc_periph_clock_enable(RCC_ADC1);
	rcc_periph_clock_enable(RCC_ADC2);

	/* TIM1 */
	rcc_periph_clock_enable(RCC_TIM1);
}

/**
 * @brief Initial interruptions configuration.
 *
 * Interruptions enabled:
 *
 * - TIM1 Update interrupt.
 * - ADC1 and ADC2 global interrupt.
 */
static void setup_nvic(void)
{
	nvic_enable_irq(NVIC_TIM1_UP_IRQ);
	nvic_enable_irq(NVIC_ADC1_2_IRQ);
}

/**
 * @brief Initial GPIO configuration.
 *
 * Set GPIO modes and initial states.
 */
static void setup_gpio(void)
{
	/* Battery level*/
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
		      GPIO13);
	gpio_set(GPIOC, GPIO13);

	/* TIM2 remap for the quadrature encoder */
	gpio_primary_remap(AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_ON,
			   AFIO_MAPR_TIM2_REMAP_FULL_REMAP);

	/* Motor driver */
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
		      GPIO12 | GPIO13 | GPIO14 | GPIO15);
	gpio_clear(GPIOB, GPIO12 | GPIO13 | GPIO14 | GPIO15);

	/* ADC inputs: gyroscope, sensors and battery */
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG,
		      GPIO4 | GPIO5 | GPIO6 | GPIO7 | GPIO0 | GPIO2 | GPIO3);

	/* Infrared emitter */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
		      GPIO7);
	gpio_clear(GPIOA, GPIO7);
}

/**
 * @brief Setup USART for bluetooth communication.
 */
static void setup_usart(void)
{
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART3_TX);

	usart_set_baudrate(USART3, 921600);
	usart_set_databits(USART3, 8);
	usart_set_stopbits(USART3, USART_STOPBITS_1);
	usart_set_parity(USART3, USART_PARITY_NONE);
	usart_set_flow_control(USART3, USART_FLOWCONTROL_NONE);
	usart_set_mode(USART3, USART_MODE_TX);

	usart_enable(USART3);
}

/**
 * @brief Setup PWM for the motor drivers.
 *
 * TIM3 is used to generate both PWM signals (left and right motor):
 *
 * - Edge-aligned, up-counting timer.
 * - Prescale to increment timer counter at 24 MHz.
 * - Set PWM frequency to 24 kHz.
 * - Configure channels 3 and 4 as output GPIOs.
 * - Set output compare mode to PWM1 (output is active when the counter is
 *   less than the compare register contents and inactive otherwise.
 * - Reset output compare value (set it to 0).
 * - Enable channels 3 and 4 outputs.
 * - Enable counter for TIM3.
 *
 * @see Reference manual (RM0008) "TIMx functional description" and in
 * particular "PWM mode" section.
 */
static void setup_pwm(void)
{
	timer_set_mode(TIM3, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE,
		       TIM_CR1_DIR_UP);

	timer_set_prescaler(TIM3, 3);
	timer_set_repetition_counter(TIM3, 0);
	timer_enable_preload(TIM3);
	timer_continuous_mode(TIM3);
	timer_set_period(TIM3, DRIVER_PWM_PERIOD);

	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
		      GPIO_TIM3_CH3 | GPIO_TIM3_CH4);

	timer_set_oc_mode(TIM3, TIM_OC3, TIM_OCM_PWM1);
	timer_set_oc_mode(TIM3, TIM_OC4, TIM_OCM_PWM1);
	timer_set_oc_value(TIM3, TIM_OC3, 0);
	timer_set_oc_value(TIM3, TIM_OC4, 0);
	timer_enable_oc_output(TIM3, TIM_OC3);
	timer_enable_oc_output(TIM3, TIM_OC4);

	timer_enable_counter(TIM3);
}

/**
 * @brief Configure timer to read a quadrature encoder.
 *
 * - Set the Auto-Reload Register (TIMx_ARR).
 * - Set the encoder interface mode counting on both TI1 and TI2 edges.
 * - Configure inputs (see note).
 * - Enable counter.
 *
 * @param[in] timer_peripheral Timer register address base to configure.
 *
 * @note It currently always use channels 1 and 2.
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
 * TIM2 for the left motor and TIM4 for the right motor are configured.
 */
static void setup_encoders(void)
{
	configure_timer_as_quadrature_encoder(TIM2);
	configure_timer_as_quadrature_encoder(TIM4);
}

/**
 * @brief Set SysTick interruptions frequency and enable SysTick counter.
 *
 * SYSCLK is at 72 MHz as well as the Advanced High-permormance Bus (AHB)
 * because, by default, the AHB divider is set to 1, so the AHB clock has the
 * same frequency as the SYSCLK.
 *
 * SysTick interruption frequency is set to `SYSTICK_FREQUENCY_HZ`.
 *
 * @see STM32F103x8/STM32F103xB data sheet and in particular the "Clock tree"
 * figure.
 */
static void setup_systick(void)
{
	systick_set_frequency(SYSTICK_FREQUENCY_HZ, SYSCLK_FREQUENCY_HZ);
	systick_counter_enable();
	systick_interrupt_enable();
}

/**
 * @brief Handle the SysTick interruptions.
 */
void sys_tick_handler(void)
{
}

/**
 * @brief Set left motor power.
 *
 * Power is set modulating the PWM signal sent to the motor driver.
 *
 * @param[in] power Power value from 0 to DRIVER_PWM_PERIOD.
 */
static void power_left(uint32_t power)
{
	timer_set_oc_value(TIM3, TIM_OC3, power);
}

/**
 * @brief Set right motor power.
 *
 * Power is set modulating the PWM signal sent to the motor driver.
 *
 * @param[in] power Power value from 0 to DRIVER_PWM_PERIOD.
 */
static void power_right(uint32_t power)
{
	timer_set_oc_value(TIM3, TIM_OC4, power);
}

/**
 * @brief Set driving direction to forward in both motors.
 */
static void drive_forward(void)
{
	gpio_clear(GPIOB, GPIO13 | GPIO15);
	gpio_set(GPIOB, GPIO12 | GPIO14);
}

/**
 * @brief Set driving direction to backward in both motors.
 */
static void drive_backward(void)
{
	gpio_clear(GPIOB, GPIO12 | GPIO14);
	gpio_set(GPIOB, GPIO13 | GPIO15);
}

/**
 * @brief Break both motors.
 *
 * Set driver controlling signals to high to short break the driver outputs.
 * The break will be effective with any PWM input signal in the motor driver.
 */
static void drive_break(void)
{
	gpio_set(GPIOB, GPIO12 | GPIO13 | GPIO14 | GPIO15);
}

/**
 * @brief Read left motor encoder counter.
 */
static uint32_t read_encoder_left(void)
{
	return timer_get_counter(TIM2);
}

/**
 * @brief Read right motor encoder counter.
 */
static uint32_t read_encoder_right(void)
{
	return timer_get_counter(TIM4);
}

/**
 * @brief Setup for ADC 1: Four injected channels on scan mode.
 *
 * - Initialize channel_sequence structure to map physical channels
 *   versus software injected channels. The order to read the sensors is: left
 *   side, right side, left front, right front.
 * - Power off the ADC to be sure that does not run during configuration.
 * - Enable scan mode with single conversion mode triggered by software.
 * - Configure the alignment (right) and the sample time (28.5 cycles of ADC
 *   clock).
 * - Set injected sequence with channel_sequence structure.
 * - Power on the ADC and wait for ADC starting up (at least 3 us).
 * - Calibrate the ADC.
 *
 * @note This ADC reads phototransistor sensors measurements.
 *
 * @see Reference manual (RM0008) "Analog-to-digital converter" and in
 * particular "Scan mode" section.
 *
 * @see Pinout section from project official documentation
 * (https://theseus.readthedocs.io/)
 */
static void setup_adc1(void)
{
	int i;

	uint8_t channel_sequence[4] = {ADC_CHANNEL6, ADC_CHANNEL5, ADC_CHANNEL7,
				       ADC_CHANNEL4};

	adc_power_off(ADC1);
	adc_enable_scan_mode(ADC1);
	adc_set_single_conversion_mode(ADC1);
	adc_enable_external_trigger_injected(ADC1, ADC_CR2_JEXTSEL_JSWSTART);
	adc_set_right_aligned(ADC1);
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_28DOT5CYC);
	adc_set_injected_sequence(
	    ADC1, sizeof(channel_sequence) / sizeof(channel_sequence[0]),
	    channel_sequence);
	adc_power_on(ADC1);
	for (i = 0; i < 800000; i++)
		__asm__("nop");
	adc_reset_calibration(ADC1);
	adc_calibrate(ADC1);
}

/**
 * @brief Setup for ADC 2: Three injected channels on scan mode and an analog
 * watchdog on one channel.
 *
 * - Initialize channel_sequence structure to map physical channels
 *   versus software injected channels.The order of the sequence is: Vout,
 *   Vref, battery.
 * - Power off the ADC to be sure that does not run during configuration.
 * - Enable scan mode with single conversion mode triggered by software.
 * - Configure the alignment (right) and the sample time (28.5 cycles of ADC
 *   clock).
 * - Set injected sequence with channel_sequence structure.
 * - Setup for analog watchdog: define the lowest battery level, select
 *   the channels to be enabled (injected channel 0) and activate ADC2
 *   analog watchdog interruption.
 * - Power on the ADC and wait for ADC starting up (at least 3 us).
 * - Calibrate the ADC.
 *
 * @note This ADC reads gyroscope outputs.
 *
 * @see Reference manual (RM0008) "Analog-to-digital converter" and in
 * particular "Scan mode" section.
 *
 * @see Pinout section from project official documentation
 * (https://theseus.readthedocs.io/)
 */
static void setup_adc2(void)
{
	int i;

	uint8_t channel_sequence[3] = {ADC_CHANNEL3, ADC_CHANNEL2,
				       ADC_CHANNEL0};
	adc_power_off(ADC2);
	adc_enable_scan_mode(ADC2);
	adc_set_single_conversion_mode(ADC2);
	adc_enable_external_trigger_injected(ADC2, ADC_CR2_JEXTSEL_JSWSTART);
	adc_set_right_aligned(ADC2);
	adc_set_sample_time_on_all_channels(ADC2, ADC_SMPR_SMP_28DOT5CYC);
	adc_set_injected_sequence(
	    ADC2, sizeof(channel_sequence) / sizeof(channel_sequence[0]),
	    channel_sequence);
	adc_set_watchdog_low_threshold(ADC2, BATTERY_LOW_LIMIT);
	adc_enable_analog_watchdog_injected(ADC2);
	adc_enable_analog_watchdog_on_selected_channel(ADC2,
						       ADC_CR1_AWDCH_CHANNEL0);
	adc_enable_awd_interrupt(ADC2);
	adc_power_on(ADC2);
	for (i = 0; i < 800000; i++)
		__asm__("nop");
	adc_reset_calibration(ADC2);
	adc_calibrate(ADC2);
}

/**
 * @brief TIM1 setup.
 *
 * The TIM1 generates an update event interruption that invokes the
 * function tim1_up_isr.
 *
 * - Set TIM1 default values.
 * - Configure the base time (no clock division ratio, no aligned mode,
 *   direction up).
 * - Set clock division, prescaler and period parameters to get an update
 *   event with a frequency of 100 KHz / 50 = 2 KHz.
 * - Enable the TIM1.
 * - Enable the interruption of type update event on the TIM1.
 *
 * @note The TIM1 is conected to the APB2 prescaler.
 *
 * @see Reference manual (RM0008) "Advanced-control timers"
 */
static void setup_timer1(void)
{
	rcc_periph_reset_pulse(RST_TIM1);
	timer_set_mode(TIM1, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE,
		       TIM_CR1_DIR_UP);
	timer_set_clock_division(TIM1, 0x00);
	timer_set_prescaler(TIM1, (rcc_apb2_frequency / 100));
	timer_set_period(TIM1, 0x32);
	timer_enable_counter(TIM1);
	timer_enable_irq(TIM1, TIM_DIER_UIE);
}

/**
 * @brief Initial setup and infinite wait.
 */
int main(void)
{
	int j = 0;

	setup_clock();
	setup_nvic();
	setup_gpio();
	setup_usart();
	setup_encoders();
	setup_pwm();
	setup_systick();
	setup_timer1();
	setup_adc1();
	setup_adc2();

	drive_forward();

	int i = 0;

	while (1) {

		if (i < 1000) {
			LOG_INFO("hello world!");
			LOG_INFO("format %c %d!", 'a', 38);
		}
		i++;
	}

	return 0;
}
