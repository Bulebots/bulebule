#include "detection.h"

#define SIDE_WALL_DETECTION (CELL_DIMENSION * 0.90)
#define FRONT_WALL_DETECTION (CELL_DIMENSION * 1.5)
#define SIDE_CALIBRATION_READINGS 20
#define SENSORS_SM_TICKS 4
#define LOG_CONVERSION_TABLE_STEP 4
#define LOG_CONVERSION_TABLE_SIZE (ADC_RESOLUTION / LOG_CONVERSION_TABLE_STEP)

static volatile uint16_t sensors_off[NUM_SENSOR], sensors_on[NUM_SENSOR];
static volatile float distance[NUM_SENSOR];
static volatile float calibration_factor[NUM_SENSOR];
const float sensors_calibration_a[NUM_SENSOR] = {
    SENSOR_SIDE_LEFT_A, SENSOR_SIDE_RIGHT_A, SENSOR_FRONT_LEFT_A,
    SENSOR_FRONT_RIGHT_A};
const float sensors_calibration_b[NUM_SENSOR] = {
    SENSOR_SIDE_LEFT_B, SENSOR_SIDE_RIGHT_B, SENSOR_FRONT_LEFT_B,
    SENSOR_FRONT_RIGHT_B};

/**
 * Table to calculate the log of values between `1` and `ADC_RESOLUTION - 1`.
 *
 * @note It uses steps of size `LOG_CONVERSION_TABLE_STEP`.
 * @note To avoid unstable results, it calculates `log(0)` as `1`.
 */
const float log_conversion[LOG_CONVERSION_TABLE_SIZE] = {
    1.,     1.3863, 2.0794, 2.4849, 2.7726, 2.9957, 3.1781, 3.3322, 3.4657,
    3.5835, 3.6889, 3.7842, 3.8712, 3.9512, 4.0254, 4.0943, 4.1589, 4.2195,
    4.2767, 4.3307, 4.382,  4.4308, 4.4773, 4.5218, 4.5643, 4.6052, 4.6444,
    4.6821, 4.7185, 4.7536, 4.7875, 4.8203, 4.852,  4.8828, 4.9127, 4.9416,
    4.9698, 4.9972, 5.0239, 5.0499, 5.0752, 5.0999, 5.124,  5.1475, 5.1705,
    5.193,  5.2149, 5.2364, 5.2575, 5.2781, 5.2983, 5.3181, 5.3375, 5.3566,
    5.3753, 5.3936, 5.4116, 5.4293, 5.4467, 5.4638, 5.4806, 5.4972, 5.5134,
    5.5294, 5.5452, 5.5607, 5.5759, 5.591,  5.6058, 5.6204, 5.6348, 5.649,
    5.663,  5.6768, 5.6904, 5.7038, 5.717,  5.7301, 5.743,  5.7557, 5.7683,
    5.7807, 5.793,  5.8051, 5.8171, 5.8289, 5.8406, 5.8522, 5.8636, 5.8749,
    5.8861, 5.8972, 5.9081, 5.9189, 5.9296, 5.9402, 5.9506, 5.961,  5.9713,
    5.9814, 5.9915, 6.0014, 6.0113, 6.021,  6.0307, 6.0403, 6.0497, 6.0591,
    6.0684, 6.0776, 6.0868, 6.0958, 6.1048, 6.1137, 6.1225, 6.1312, 6.1399,
    6.1485, 6.157,  6.1654, 6.1738, 6.1821, 6.1903, 6.1985, 6.2066, 6.2146,
    6.2226, 6.2305, 6.2383, 6.2461, 6.2538, 6.2615, 6.2691, 6.2766, 6.2841,
    6.2916, 6.2989, 6.3063, 6.3135, 6.3208, 6.3279, 6.3351, 6.3421, 6.3491,
    6.3561, 6.363,  6.3699, 6.3767, 6.3835, 6.3902, 6.3969, 6.4036, 6.4102,
    6.4167, 6.4232, 6.4297, 6.4362, 6.4425, 6.4489, 6.4552, 6.4615, 6.4677,
    6.4739, 6.48,   6.4862, 6.4922, 6.4983, 6.5043, 6.5103, 6.5162, 6.5221,
    6.528,  6.5338, 6.5396, 6.5453, 6.5511, 6.5568, 6.5624, 6.5681, 6.5737,
    6.5793, 6.5848, 6.5903, 6.5958, 6.6012, 6.6067, 6.612,  6.6174, 6.6227,
    6.628,  6.6333, 6.6386, 6.6438, 6.649,  6.6542, 6.6593, 6.6644, 6.6695,
    6.6746, 6.6796, 6.6846, 6.6896, 6.6946, 6.6995, 6.7044, 6.7093, 6.7142,
    6.719,  6.7238, 6.7286, 6.7334, 6.7382, 6.7429, 6.7476, 6.7523, 6.7569,
    6.7616, 6.7662, 6.7708, 6.7754, 6.7799, 6.7845, 6.789,  6.7935, 6.7979,
    6.8024, 6.8068, 6.8112, 6.8156, 6.82,   6.8244, 6.8287, 6.833,  6.8373,
    6.8416, 6.8459, 6.8501, 6.8544, 6.8586, 6.8628, 6.8669, 6.8711, 6.8752,
    6.8794, 6.8835, 6.8876, 6.8916, 6.8957, 6.8997, 6.9037, 6.9078, 6.9117,
    6.9157, 6.9197, 6.9236, 6.9276, 6.9315, 6.9354, 6.9393, 6.9431, 6.947,
    6.9508, 6.9546, 6.9584, 6.9622, 6.966,  6.9698, 6.9735, 6.9773, 6.981,
    6.9847, 6.9884, 6.9921, 6.9958, 6.9994, 7.0031, 7.0067, 7.0103, 7.0139,
    7.0175, 7.0211, 7.0246, 7.0282, 7.0317, 7.0353, 7.0388, 7.0423, 7.0458,
    7.0493, 7.0527, 7.0562, 7.0596, 7.063,  7.0665, 7.0699, 7.0733, 7.0767,
    7.08,   7.0834, 7.0867, 7.0901, 7.0934, 7.0967, 7.1,    7.1033, 7.1066,
    7.1099, 7.1131, 7.1164, 7.1196, 7.1229, 7.1261, 7.1293, 7.1325, 7.1357,
    7.1389, 7.142,  7.1452, 7.1483, 7.1515, 7.1546, 7.1577, 7.1608, 7.1639,
    7.167,  7.1701, 7.1732, 7.1763, 7.1793, 7.1824, 7.1854, 7.1884, 7.1914,
    7.1944, 7.1974, 7.2004, 7.2034, 7.2064, 7.2093, 7.2123, 7.2152, 7.2182,
    7.2211, 7.224,  7.2269, 7.2298, 7.2327, 7.2356, 7.2385, 7.2414, 7.2442,
    7.2471, 7.2499, 7.2528, 7.2556, 7.2584, 7.2612, 7.264,  7.2668, 7.2696,
    7.2724, 7.2752, 7.2779, 7.2807, 7.2834, 7.2862, 7.2889, 7.2917, 7.2944,
    7.2971, 7.2998, 7.3025, 7.3052, 7.3079, 7.3106, 7.3132, 7.3159, 7.3185,
    7.3212, 7.3238, 7.3265, 7.3291, 7.3317, 7.3343, 7.3369, 7.3395, 7.3421,
    7.3447, 7.3473, 7.3499, 7.3524, 7.355,  7.3576, 7.3601, 7.3626, 7.3652,
    7.3677, 7.3702, 7.3727, 7.3753, 7.3778, 7.3803, 7.3827, 7.3852, 7.3877,
    7.3902, 7.3926, 7.3951, 7.3976, 7.4,    7.4025, 7.4049, 7.4073, 7.4097,
    7.4122, 7.4146, 7.417,  7.4194, 7.4218, 7.4242, 7.4265, 7.4289, 7.4313,
    7.4337, 7.436,  7.4384, 7.4407, 7.4431, 7.4454, 7.4478, 7.4501, 7.4524,
    7.4547, 7.457,  7.4593, 7.4616, 7.4639, 7.4662, 7.4685, 7.4708, 7.4731,
    7.4753, 7.4776, 7.4799, 7.4821, 7.4844, 7.4866, 7.4889, 7.4911, 7.4933,
    7.4955, 7.4978, 7.5,    7.5022, 7.5044, 7.5066, 7.5088, 7.511,  7.5132,
    7.5153, 7.5175, 7.5197, 7.5219, 7.524,  7.5262, 7.5283, 7.5305, 7.5326,
    7.5348, 7.5369, 7.539,  7.5412, 7.5433, 7.5454, 7.5475, 7.5496, 7.5517,
    7.5538, 7.5559, 7.558,  7.5601, 7.5622, 7.5642, 7.5663, 7.5684, 7.5704,
    7.5725, 7.5746, 7.5766, 7.5787, 7.5807, 7.5827, 7.5848, 7.5868, 7.5888,
    7.5909, 7.5929, 7.5949, 7.5969, 7.5989, 7.6009, 7.6029, 7.6049, 7.6069,
    7.6089, 7.6109, 7.6128, 7.6148, 7.6168, 7.6187, 7.6207, 7.6227, 7.6246,
    7.6266, 7.6285, 7.6305, 7.6324, 7.6343, 7.6363, 7.6382, 7.6401, 7.642,
    7.644,  7.6459, 7.6478, 7.6497, 7.6516, 7.6535, 7.6554, 7.6573, 7.6592,
    7.6611, 7.6629, 7.6648, 7.6667, 7.6686, 7.6704, 7.6723, 7.6742, 7.676,
    7.6779, 7.6797, 7.6816, 7.6834, 7.6852, 7.6871, 7.6889, 7.6907, 7.6926,
    7.6944, 7.6962, 7.698,  7.6998, 7.7017, 7.7035, 7.7053, 7.7071, 7.7089,
    7.7107, 7.7124, 7.7142, 7.716,  7.7178, 7.7196, 7.7213, 7.7231, 7.7249,
    7.7267, 7.7284, 7.7302, 7.7319, 7.7337, 7.7354, 7.7372, 7.7389, 7.7407,
    7.7424, 7.7441, 7.7459, 7.7476, 7.7493, 7.751,  7.7528, 7.7545, 7.7562,
    7.7579, 7.7596, 7.7613, 7.763,  7.7647, 7.7664, 7.7681, 7.7698, 7.7715,
    7.7732, 7.7749, 7.7765, 7.7782, 7.7799, 7.7816, 7.7832, 7.7849, 7.7866,
    7.7882, 7.7899, 7.7915, 7.7932, 7.7948, 7.7965, 7.7981, 7.7998, 7.8014,
    7.803,  7.8047, 7.8063, 7.8079, 7.8095, 7.8112, 7.8128, 7.8144, 7.816,
    7.8176, 7.8192, 7.8208, 7.8224, 7.824,  7.8256, 7.8272, 7.8288, 7.8304,
    7.832,  7.8336, 7.8352, 7.8368, 7.8383, 7.8399, 7.8415, 7.8431, 7.8446,
    7.8462, 7.8478, 7.8493, 7.8509, 7.8524, 7.854,  7.8555, 7.8571, 7.8586,
    7.8602, 7.8617, 7.8633, 7.8648, 7.8663, 7.8679, 7.8694, 7.8709, 7.8725,
    7.874,  7.8755, 7.877,  7.8785, 7.88,   7.8816, 7.8831, 7.8846, 7.8861,
    7.8876, 7.8891, 7.8906, 7.8921, 7.8936, 7.8951, 7.8966, 7.898,  7.8995,
    7.901,  7.9025, 7.904,  7.9054, 7.9069, 7.9084, 7.9099, 7.9113, 7.9128,
    7.9143, 7.9157, 7.9172, 7.9186, 7.9201, 7.9215, 7.923,  7.9244, 7.9259,
    7.9273, 7.9288, 7.9302, 7.9316, 7.9331, 7.9345, 7.9359, 7.9374, 7.9388,
    7.9402, 7.9417, 7.9431, 7.9445, 7.9459, 7.9473, 7.9487, 7.9501, 7.9516,
    7.953,  7.9544, 7.9558, 7.9572, 7.9586, 7.96,   7.9614, 7.9628, 7.9642,
    7.9655, 7.9669, 7.9683, 7.9697, 7.9711, 7.9725, 7.9738, 7.9752, 7.9766,
    7.978,  7.9793, 7.9807, 7.9821, 7.9834, 7.9848, 7.9862, 7.9875, 7.9889,
    7.9902, 7.9916, 7.9929, 7.9943, 7.9956, 7.997,  7.9983, 7.9997, 8.001,
    8.0024, 8.0037, 8.005,  8.0064, 8.0077, 8.009,  8.0104, 8.0117, 8.013,
    8.0143, 8.0157, 8.017,  8.0183, 8.0196, 8.0209, 8.0222, 8.0236, 8.0249,
    8.0262, 8.0275, 8.0288, 8.0301, 8.0314, 8.0327, 8.034,  8.0353, 8.0366,
    8.0379, 8.0392, 8.0404, 8.0417, 8.043,  8.0443, 8.0456, 8.0469, 8.0481,
    8.0494, 8.0507, 8.052,  8.0533, 8.0545, 8.0558, 8.0571, 8.0583, 8.0596,
    8.0609, 8.0621, 8.0634, 8.0646, 8.0659, 8.0671, 8.0684, 8.0697, 8.0709,
    8.0722, 8.0734, 8.0746, 8.0759, 8.0771, 8.0784, 8.0796, 8.0809, 8.0821,
    8.0833, 8.0846, 8.0858, 8.087,  8.0883, 8.0895, 8.0907, 8.0919, 8.0932,
    8.0944, 8.0956, 8.0968, 8.098,  8.0993, 8.1005, 8.1017, 8.1029, 8.1041,
    8.1053, 8.1065, 8.1077, 8.1089, 8.1101, 8.1113, 8.1125, 8.1137, 8.1149,
    8.1161, 8.1173, 8.1185, 8.1197, 8.1209, 8.1221, 8.1233, 8.1244, 8.1256,
    8.1268, 8.128,  8.1292, 8.1304, 8.1315, 8.1327, 8.1339, 8.1351, 8.1362,
    8.1374, 8.1386, 8.1397, 8.1409, 8.1421, 8.1432, 8.1444, 8.1455, 8.1467,
    8.1479, 8.149,  8.1502, 8.1513, 8.1525, 8.1536, 8.1548, 8.1559, 8.1571,
    8.1582, 8.1594, 8.1605, 8.1617, 8.1628, 8.1639, 8.1651, 8.1662, 8.1674,
    8.1685, 8.1696, 8.1708, 8.1719, 8.173,  8.1741, 8.1753, 8.1764, 8.1775,
    8.1786, 8.1798, 8.1809, 8.182,  8.1831, 8.1842, 8.1854, 8.1865, 8.1876,
    8.1887, 8.1898, 8.1909, 8.192,  8.1931, 8.1942, 8.1953, 8.1964, 8.1975,
    8.1986, 8.1997, 8.2008, 8.2019, 8.203,  8.2041, 8.2052, 8.2063, 8.2074,
    8.2085, 8.2096, 8.2107, 8.2118, 8.2128, 8.2139, 8.215,  8.2161, 8.2172,
    8.2182, 8.2193, 8.2204, 8.2215, 8.2226, 8.2236, 8.2247, 8.2258, 8.2268,
    8.2279, 8.229,  8.23,   8.2311, 8.2322, 8.2332, 8.2343, 8.2354, 8.2364,
    8.2375, 8.2385, 8.2396, 8.2406, 8.2417, 8.2428, 8.2438, 8.2449, 8.2459,
    8.247,  8.248,  8.2491, 8.2501, 8.2511, 8.2522, 8.2532, 8.2543, 8.2553,
    8.2563, 8.2574, 8.2584, 8.2595, 8.2605, 8.2615, 8.2626, 8.2636, 8.2646,
    8.2657, 8.2667, 8.2677, 8.2687, 8.2698, 8.2708, 8.2718, 8.2728, 8.2738,
    8.2749, 8.2759, 8.2769, 8.2779, 8.2789, 8.28,   8.281,  8.282,  8.283,
    8.284,  8.285,  8.286,  8.287,  8.288,  8.289,  8.29,   8.291,  8.292,
    8.293,  8.294,  8.295,  8.296,  8.297,  8.298,  8.299,  8.3,    8.301,
    8.302,  8.303,  8.304,  8.305,  8.306,  8.307,  8.308,  8.3089, 8.3099,
    8.3109, 8.3119, 8.3129, 8.3139, 8.3148, 8.3158, 8.3168};

/**
 * @brief Set an specific emitter ON.
 *
 * @param[in] emitter Emitter type.
 */
static void set_emitter_on(uint8_t emitter)
{
	switch (emitter) {
	case SENSOR_SIDE_LEFT_ID:
		gpio_set(GPIOA, GPIO9);
		break;
	case SENSOR_SIDE_RIGHT_ID:
		gpio_set(GPIOB, GPIO8);
		break;
	case SENSOR_FRONT_LEFT_ID:
		gpio_set(GPIOA, GPIO8);
		break;
	case SENSOR_FRONT_RIGHT_ID:
		gpio_set(GPIOB, GPIO9);
		break;
	default:
		break;
	}
}

/**
 * @brief Set an specific emitter OFF.
 *
 * @param[in] emitter Emitter type.
 */
static void set_emitter_off(uint8_t emitter)
{
	switch (emitter) {
	case SENSOR_SIDE_LEFT_ID:
		gpio_clear(GPIOA, GPIO9);
		break;
	case SENSOR_SIDE_RIGHT_ID:
		gpio_clear(GPIOB, GPIO8);
		break;
	case SENSOR_FRONT_LEFT_ID:
		gpio_clear(GPIOA, GPIO8);
		break;
	case SENSOR_FRONT_RIGHT_ID:
		gpio_clear(GPIOB, GPIO9);
		break;
	default:
		break;
	}
}

/**
 * @brief State machine to manage the sensors activation and deactivation
 * states and readings.
 *
 * In order to get accurate distance values, the phototransistor's output
 * will be read with the infrared emitter sensors powered on and powered
 * off. Besides, to avoid undesired interactions between different emitters and
 * phototranistors, the reads will be done one by one.
 *
 * The battery voltage is also read on the state 1.
 *
 * - State 1 (first because the emitter is OFF on start):
 *         -# Save phototranistors sensors (ADC1) from emitter OFF and
 *            power ON the emitter.
 * - State 2:
 *         -# Start the phototranistors sensors (ADC1) read.
 * - State 3:
 *         -# Save phototranistors sensors (ADC1) from emitter ON and
 *            power OFF the emitter.
 * - State 4:
 *         -# Start the phototranistors sensors (ADC1) read.
 */
static void sm_emitter_adc(void)
{
	static uint8_t emitter_status = 1;
	static uint8_t sensor_index = SENSOR_SIDE_LEFT_ID;

	switch (emitter_status) {
	case 1:
		sensors_off[sensor_index] =
		    adc_read_injected(ADC1, (sensor_index + 1));
		set_emitter_on(sensor_index);
		emitter_status = 2;
		break;
	case 2:
		adc_start_conversion_injected(ADC1);
		emitter_status = 3;
		break;
	case 3:
		sensors_on[sensor_index] =
		    adc_read_injected(ADC1, (sensor_index + 1));
		set_emitter_off(sensor_index);
		emitter_status = 4;
		break;
	case 4:
		adc_start_conversion_injected(ADC1);
		emitter_status = 1;
		if (sensor_index == (NUM_SENSOR - 1))
			sensor_index = 0;
		else
			sensor_index++;
		break;
	default:
		break;
	}
}

/**
 * @brief TIM1 interruption routine.
 *
 * - Manage the update event interruption flag.
 * - Trigger state machine to manage sensors.
 */
void tim1_up_isr(void)
{
	if (timer_get_flag(TIM1, TIM_SR_UIF)) {
		timer_clear_flag(TIM1, TIM_SR_UIF);
		sm_emitter_adc();
	}
}

/**
 * @brief Get sensors values with emitter on and off.
 */
void get_sensors_raw(uint16_t *off, uint16_t *on)
{
	uint8_t i = 0;

	for (i = 0; i < NUM_SENSOR; i++) {
		off[i] = sensors_off[i];
		on[i] = sensors_on[i];
	}
}

/**
 * @brief Apply `log()` to the raw sensor readings.
 *
 * A minimum difference of `1` is enforced to avoid applying `log()` to a zero
 * or negative value.
 *
 * @param[in] on Raw sensor reading with emitter on.
 * @param[in] off Raw sensor reading with emitter off.
 */
static float raw_log(uint16_t on, uint16_t off)
{
	uint16_t diff;

	if (off > on)
		diff = 0;
	else
		diff = on - off;
	diff /= LOG_CONVERSION_TABLE_STEP;
	if (diff == 0)
		diff = 1;
	return log_conversion[diff];
}

/**
 * @brief Calculate and update the distance from each sensor.
 *
 * @note The distances are calculated from the center of the robot.
 */
void update_distance_readings(void)
{
	uint8_t i = 0;

	for (i = 0; i < NUM_SENSOR; i++) {
		distance[i] = (sensors_calibration_a[i] /
				   raw_log(sensors_on[i], sensors_off[i]) -
			       sensors_calibration_b[i]);
		if ((i == SENSOR_SIDE_LEFT_ID) || (i == SENSOR_SIDE_RIGHT_ID))
			distance[i] -= calibration_factor[i];
	}
}

/**
 * @brief Get distance value from front left sensor.
 */
float get_front_left_distance(void)
{
	return distance[SENSOR_FRONT_LEFT_ID];
}

/**
 * @brief Get distance value from front right sensor.
 */
float get_front_right_distance(void)
{
	return distance[SENSOR_FRONT_RIGHT_ID];
}

/**
 * @brief Get distance value from side left sensor.
 */
float get_side_left_distance(void)
{
	return distance[SENSOR_SIDE_LEFT_ID];
}

/**
 * @brief Get distance value from side right sensor.
 */
float get_side_right_distance(void)
{
	return distance[SENSOR_SIDE_RIGHT_ID];
}

/**
 * @brief Calculate and return the side sensors error.
 *
 * Taking into account that the walls are parallel to the robot, this function
 * returns the distance that the robot is moved from the center of the
 * corridor..
 */
float get_side_sensors_error(void)
{
	float left_error;
	float right_error;

	left_error = distance[SENSOR_SIDE_LEFT_ID] - MIDDLE_MAZE_DISTANCE;
	right_error = distance[SENSOR_SIDE_RIGHT_ID] - MIDDLE_MAZE_DISTANCE;

	if ((left_error > 0.) && (right_error < 0.))
		return right_error;
	if ((right_error > 0.) && (left_error < 0.))
		return -left_error;
	if ((left_error > 0.05) && (right_error < 0.05))
		return right_error;
	if ((right_error > 0.05) && (left_error < 0.05))
		return -left_error;
	return 0.;
}

/**
 * @brief Calculate and return the front sensors error.
 *
 * Taking into account that robot is approaching to a perpendicular wall, this
 * function returns the difference between the front sensors distances
 */
float get_front_sensors_error(void)
{
	return distance[SENSOR_FRONT_LEFT_ID] - distance[SENSOR_FRONT_RIGHT_ID];
}

/**
 * @brief Return the front wall distance, in meters.
 */
float get_front_wall_distance(void)
{
	return (distance[SENSOR_FRONT_LEFT_ID] +
		distance[SENSOR_FRONT_RIGHT_ID]) /
	       2.;
}

/**
 * @brief Detect the existance or absence of the left wall.
 */
bool left_wall_detection(void)
{
	return (distance[SENSOR_SIDE_LEFT_ID] < SIDE_WALL_DETECTION) ? true
								     : false;
}

/**
 * @brief Detect the existance or absence of the right wall.
 */
bool right_wall_detection(void)
{
	return (distance[SENSOR_SIDE_RIGHT_ID] < SIDE_WALL_DETECTION) ? true
								      : false;
}

/**
 * @brief Detect the existance or absence of the front wall.
 */
bool front_wall_detection(void)
{
	return ((distance[SENSOR_FRONT_LEFT_ID] < FRONT_WALL_DETECTION) &&
		(distance[SENSOR_FRONT_RIGHT_ID] < FRONT_WALL_DETECTION))
		   ? true
		   : false;
}

/**
 * @brief Return left, front and right walls detection readings.
 */
struct walls_around read_walls(void)
{
	struct walls_around walls_readings;

	walls_readings.left = left_wall_detection();
	walls_readings.front = front_wall_detection();
	walls_readings.right = right_wall_detection();
	return walls_readings;
}

/**
 * @brief Calibration for side sensors.
 */
void side_sensors_calibration(void)
{
	float right_temp = 0;
	float left_temp = 0;
	int i;

	for (i = 0; i < SIDE_CALIBRATION_READINGS; i++) {
		left_temp += distance[SENSOR_SIDE_LEFT_ID];
		right_temp += distance[SENSOR_SIDE_RIGHT_ID];
		sleep_ticks(SENSORS_SM_TICKS);
	}
	calibration_factor[SENSOR_SIDE_LEFT_ID] +=
	    (left_temp / SIDE_CALIBRATION_READINGS) - MIDDLE_MAZE_DISTANCE;
	calibration_factor[SENSOR_SIDE_RIGHT_ID] +=
	    (right_temp / SIDE_CALIBRATION_READINGS) - MIDDLE_MAZE_DISTANCE;
}
