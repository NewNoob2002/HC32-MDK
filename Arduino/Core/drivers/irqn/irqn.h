#pragma once
#include <hc32_ll.h>
#include "core_debug.h"

#define IRQN_AA_FIRST_IRQN 0                                                 // IRQ0 is the first auto-assignable IRQn
#define IRQN_AA_AVAILABLE_COUNT 128                                          // IRQ0 - IRQ127 are available for auto-assignment (all normal IRQn)
#define IRQN_AA_LAST_IRQN (IRQN_AA_FIRST_IRQN + IRQN_AA_AVAILABLE_COUNT - 1) // last IRQn available for auto-assignment

#ifdef __cplusplus
extern "C"
{
#endif

    int32_t _irqn_aa_get(IRQn_Type &irqn);
    int32_t _irqn_aa_resign(IRQn_Type &irqn);

    /**
     * @brief get auto-assigned IRQn
     * @param irqn assigned IRQn
     * @param name name of the IRQn (for debug purposes)
     * @return Ok or Error
     */
    int32_t irqn_aa_get(IRQn_Type &irqn, const char *name);

    /**
     * @brief resign auto-assigned IRQn
     * @param irqn IRQn to resign
     * @param name name of the IRQn (for debug purposes)
     * @return Ok or Error
     */
    int32_t irqn_aa_resign(IRQn_Type &irqn, const char *name);

#ifdef __cplusplus
}
#endif
