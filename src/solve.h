#ifndef __SOLVE_H
#define __SOLVE_H

#include "detection.h"
#include "move.h"
#include "search.h"

void explore(void);
#ifdef MMSIM_SIMULATION
extern void send_state(void);
#endif

#endif /* __SOLVE_H */
