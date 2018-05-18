#ifndef __SOLVE_H
#define __SOLVE_H

#include "detection.h"
#include "move.h"
#include "search.h"

void explore(void);
#ifdef MMSIM_SIMULATION
void send_state(void);
#endif
void set_run_sequence(void);
void run(void);

#endif /* __SOLVE_H */
