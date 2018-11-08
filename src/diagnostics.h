#ifndef __DIAGNOSTICS_H
#define __DIAGNOSTICS_H

#include "detection.h"
#include "formatting.h"

#define NUM_LOG_ELEMENTS 2000

void set_log_matrix(void);
float get_log_matrix_element(void);
void set_enable_diagnostics(bool value);
bool get_enable_diagnostics(void);
int get_log_set_index(void);
void reset_log_matrix(void);


#endif /* __DIAGNOSTICS_H */
