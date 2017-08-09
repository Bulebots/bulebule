#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>


static void clock_setup(void)
{
	/* Set STM32 to 72 MHz. */
	rcc_clock_setup_in_hse_12mhz_out_72mhz();

	/* Enable GPIOC, and AFIO clocks. */
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_AFIO);
}


static void gpio_setup(void)
{
	/* Set GPIO13 (in GPIO port C) to 'output push-pull'. */
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

	AFIO_MAPR |= AFIO_MAPR_SWJ_CFG_FULL_SWJ_NO_JNTRST;

	/* GPIO initialization */
	gpio_clear(GPIOC, GPIO13);
}


int main(void)
{
	int i;

	clock_setup();
	gpio_setup();

	while (1) {
		gpio_toggle(GPIOC, GPIO13);
		for (i = 0; i < 800000; i++)
			__asm__("nop");
	}

	return 0;
}
