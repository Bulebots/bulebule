#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdlib.h>
#include <string.h>

#include <libopencm3/cm3/sync.h>
#include <libopencm3/stm32/usart.h>

#include "mmlib/calibration.h"
#include "mmlib/common.h"
#include "mmlib/control.h"
#include "mmlib/encoder.h"
#include "mmlib/logging.h"
#include "mmlib/move.h"
#include "mmlib/speed.h"

bool serial_acquire_transfer_lock(void);
void serial_send(char *data, int size);
void execute_command(void);

#endif /* __SERIAL_H */
