#include "setup.h"

/** Exception priorities */
#define PRIORITY_FACTOR 16

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

	/* Gyroscope */
	rcc_periph_clock_enable(RCC_SPI2);

	/* Timers */
	rcc_periph_clock_enable(RCC_TIM1);
	rcc_periph_clock_enable(RCC_TIM2);
	rcc_periph_clock_enable(RCC_TIM3);
	rcc_periph_clock_enable(RCC_TIM4);

	/* Alternate functions */
	rcc_periph_clock_enable(RCC_AFIO);

	/* ADC */
	rcc_periph_clock_enable(RCC_ADC1);
	rcc_periph_clock_enable(RCC_ADC2);

	/* Enable clock cycle counter */
	dwt_enable_cycle_counter();
}

/**
 * @brief Exceptions configuration.
 *
 * This function configures Nested Vectored Interrupt Controller for IRQ and
 * System Control Block for system interruptions.
 *
 * Exception priorities:
 *
 * - Systick priority to 1 with SCB.
 * - USART3 with priority 2 with NVIC.
 * - TIM1_UP with priority 0.
 *
 * Interruptions enabled:
 *
 * - TIM1 Update interrupt.
 * - USART3 interrupt.
 *
 * @note The priority levels are assigned on steps of 16 because the processor
 * implements only bits[7:4].
 *
 * @see Programming Manual (PM0056).
 */
static void setup_exceptions(void)
{
	nvic_set_priority(NVIC_TIM1_UP_IRQ, 0);
	nvic_set_priority(NVIC_SYSTICK_IRQ, PRIORITY_FACTOR * 1);
	nvic_set_priority(NVIC_USART3_IRQ, PRIORITY_FACTOR * 2);

	nvic_enable_irq(NVIC_TIM1_UP_IRQ);
	nvic_enable_irq(NVIC_USART3_IRQ);
}

/**
 * @brief Initial GPIO configuration.
 *
 * Set GPIO modes and initial states.
 */
static void setup_gpio(void)
{
	/* TIM2 remap for the quadrature encoder */
	gpio_primary_remap(AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_ON,
			   AFIO_MAPR_TIM2_REMAP_FULL_REMAP);

	/* ADC inputs: sensors and battery */
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG,
		      GPIO0 | GPIO2 | GPIO3 | GPIO4 | GPIO5);

	/* Infrared emitter */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
		      GPIO8 | GPIO9);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
		      GPIO8 | GPIO9);
	gpio_clear(GPIOA, GPIO8 | GPIO9);
	gpio_clear(GPIOB, GPIO8 | GPIO9);

	/* LEDs */
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
		      GPIO4 | GPIO5);
	gpio_clear(GPIOB, GPIO4 | GPIO5);

	/* Blue pill LED */
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
		      GPIO13);
	gpio_set(GPIOC, GPIO13);

	/*Buttons*/
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN,
		      GPIO11 | GPIO12);
}

/**
 * @brief Setup USART for bluetooth communication.
 */
static void setup_usart(void)
{
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART3_TX);
	gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT,
		      GPIO_USART3_RX);

	usart_set_baudrate(USART3, 921600);
	usart_set_databits(USART3, 8);
	usart_set_stopbits(USART3, USART_STOPBITS_1);
	usart_set_parity(USART3, USART_PARITY_NONE);
	usart_set_flow_control(USART3, USART_FLOWCONTROL_NONE);
	usart_set_mode(USART3, USART_MODE_TX_RX);

	USART_CR1(USART3) |= USART_CR1_RXNEIE;

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
 * - Configure channels 1, 2, 3 and 4 as output GPIOs.
 * - Set output compare mode to PWM1 (output is active when the counter is
 *   less than the compare register contents and inactive otherwise.
 * - Reset output compare value (set it to 0).
 * - Enable channels 1, 2, 3 and 4 outputs.
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

	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
		      GPIO_TIM3_CH1 | GPIO_TIM3_CH2);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
		      GPIO_TIM3_CH3 | GPIO_TIM3_CH4);

	timer_set_oc_mode(TIM3, TIM_OC1, TIM_OCM_PWM1);
	timer_set_oc_mode(TIM3, TIM_OC2, TIM_OCM_PWM1);
	timer_set_oc_mode(TIM3, TIM_OC3, TIM_OCM_PWM1);
	timer_set_oc_mode(TIM3, TIM_OC4, TIM_OCM_PWM1);
	timer_set_oc_value(TIM3, TIM_OC1, 0);
	timer_set_oc_value(TIM3, TIM_OC2, 0);
	timer_set_oc_value(TIM3, TIM_OC3, 0);
	timer_set_oc_value(TIM3, TIM_OC4, 0);
	timer_enable_oc_output(TIM3, TIM_OC1);
	timer_enable_oc_output(TIM3, TIM_OC2);
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
 * @brief Setup for ADC 1: Four injected channels on scan mode.
 *
 * - Initialize channel_sequence structure to map physical channels
 *   versus software injected channels. The order to read the sensors is: left
 *   side, right side, left front, right front.
 * - Power off the ADC to be sure that does not run during configuration.
 * - Enable scan mode with single conversion mode triggered by software.
 * - Configure the alignment (right) and the sample time (13.5 cycles of ADC
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
 * (https://bulebule.readthedocs.io/)
 */
static void setup_adc1(void)
{
	int i;

	uint8_t channel_sequence[4] = {ADC_CHANNEL4, ADC_CHANNEL3, ADC_CHANNEL5,
				       ADC_CHANNEL2};

	adc_power_off(ADC1);
	adc_enable_scan_mode(ADC1);
	adc_set_single_conversion_mode(ADC1);
	adc_enable_external_trigger_injected(ADC1, ADC_CR2_JEXTSEL_JSWSTART);
	adc_set_right_aligned(ADC1);
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_13DOT5CYC);
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
 * @brief TIM1 setup.
 *
 * The TIM1 generates an update event interruption that invokes the
 * function tim1_up_isr.
 *
 * - Set TIM1 default values.
 * - Configure the base time (no clock division ratio, no aligned mode,
 *   direction up).
 * - Set clock division, prescaler and period parameters to get an update
 *   event with a frequency of 16 KHz. 16 interruptions by ms, 4 sensors with
 *   4 states.
 *
 *   \f$frequency = \frac{timerclock}{(preescaler + 1)(period + 1)}\f$
 *
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
	timer_set_prescaler(TIM1, (rcc_apb2_frequency / 160000 - 1));
	timer_set_period(TIM1, 10 - 1);
	timer_enable_counter(TIM1);
	timer_enable_irq(TIM1, TIM_DIER_UIE);
}

/**
 * @brief Execute all setup functions.
 */
void setup(void)
{
	setup_clock();
	setup_exceptions();
	setup_gpio();
	setup_usart();
	setup_encoders();
	setup_pwm();
	setup_mpu();
	setup_systick();
	setup_timer1();
	setup_adc1();
}

/**
 * @brief Setup SPI.
 *
 * SPI2 is configured as follows:
 *
 * - Master mode.
 * - Clock baud rate: PCLK1 / speed_div; PCLK1 = 36MHz.
 * - Clock polarity: 0 (idle low; leading edge is a rising edge).
 * - Clock phase: 0 (out changes on the trailing edge and input data
 *   captured on rising edge).
 * - Data frame format: 8-bits.
 * - Frame format: MSB first.
 *
 * NSS is configured to be managed by software.
 */
static void setup_spi(uint8_t speed_div)
{
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO13 | GPIO15);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
		      GPIO12);
	gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO14);

	spi_reset(SPI2);

	spi_init_master(SPI2, speed_div, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
			SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT,
			SPI_CR1_MSBFIRST);

	spi_enable_software_slave_management(SPI2);
	spi_set_nss_high(SPI2);

	spi_enable(SPI2);
}

/**
 * @brief Setup SPI for gyroscope read, less than 20 MHz.
 *
 * The clock baudrate is 36 MHz / 2 = 18 MHz.
 */
void setup_spi_high_speed(void)
{
	setup_spi(SPI_CR1_BAUDRATE_FPCLK_DIV_2);
}

/**
 * @brief Setup SPI for gyroscope Write, less than 1 MHz.
 *
 * The clock baudrate is 36 MHz / 64 = 0.5625 MHz.
 */
void setup_spi_low_speed(void)
{
	setup_spi(SPI_CR1_BAUDRATE_FPCLK_DIV_64);
}
