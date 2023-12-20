/*
 * Copyright (c) 2019 Nuclei Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __CORE_FEATURE_TIMER_H__
#define __CORE_FEATURE_TIMER_H__
/*!
 * @file     core_feature_timer.h
 * @brief    Timer feature API header file for Nuclei N100 Core
 */
/*
 * System Timer Feature Configuration Macro:
 * 1. __TIMER_PRESENT: Must, Define whether Private System Timer is present or not.
 *   * 0: Not present
 *   * 1: Present
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "core_feature_base.h"

#if defined(__TIMER_PRESENT) && (__TIMER_PRESENT == 1)
/* ##################################    SysTimer function  ############################################ */
/**
 * \defgroup NMSIS_Core_SysTimer SysTimer Functions
 * \brief    Functions that configure the Core System Timer.
 * @{
 */

/**
 * \brief  Set system timer load value
 * \details
 * This function set the system timer load value in MTIMER register.
 * \param [in]  value   value to set system timer MTIMER register.
 * \remarks
 * - Load value is 64bits wide.
 * - \ref SysTimer_GetLoadValue
 */
__STATIC_FORCEINLINE void SysTimer_SetLoadValue(uint64_t value)
{
#if __RISCV_XLEN == 32
    uint8_t *addr;
    addr = (uint8_t *)(&(SysTimer->MTIMER));
    __SW(addr, 0);      // prevent carry
    __SW(addr + 4, (uint32_t)(value >> 32));
    __SW(addr, (uint32_t)(value));
#else
    SysTimer->MTIMER = value;
#endif
}

/**
 * \brief  Get system timer load value
 * \details
 * This function get the system timer current value in MTIMER register.
 * \return  current value(64bit) of system timer MTIMER register.
 * \remarks
 * - Load value is 64bits wide.
 * - \ref SysTimer_SetLoadValue
 */
__STATIC_FORCEINLINE uint64_t SysTimer_GetLoadValue(void)
{
#if __RISCV_XLEN == 32
    volatile uint32_t high0, low, high;
    uint64_t full;
    uint8_t *addr;

    addr = (uint8_t *)(&(SysTimer->MTIMER));

    high0 = __LW(addr + 4);
    low = __LW(addr);
    high = __LW(addr + 4);
    if (high0 != high) {
        low = __LW(addr);
    }
    full = (((uint64_t)high) << 32) | low;
    return full;
#else
    return SysTimer->MTIMER;
#endif
}

/**
 * \brief  Set system timer compare value by hartid
 * \details
 * This function set the system Timer compare value in MTIMERCMP register.
 * \param [in]  value   compare value to set system timer MTIMERCMP register.
 * \param [in]  hartid  hart ID, one hart is required to have a known hart ID of 0, other harts ID can be in 1~1023.
 * \remarks
 * - Compare value is 64bits wide.
 * - If compare value is larger than current value timer interrupt generate.
 * - Modify the load value or compare value less to clear the interrupt.
 * - In S-mode, hartid can't be get by using __get_hart_id function, so this api suits S-mode particularly.
 * - \ref SysTimer_GetHartCompareValue
 */
__STATIC_FORCEINLINE void SysTimer_SetHartCompareValue(uint64_t value, unsigned long hartid)
{
    if (hartid == 0) {
#if __RISCV_XLEN == 32
        uint8_t *addr;
        addr = (uint8_t *)(&(SysTimer->MTIMERCMP));
        __SW(addr, -1U);      // prevent load > timecmp
        __SW(addr + 4, (uint32_t)(value >> 32));
        __SW(addr, (uint32_t)(value));
#else
        SysTimer->MTIMERCMP = value;
#endif
    } else {
        uint8_t *addr = (uint8_t *)(SysTimer_CLINT_MTIMECMP_BASE(hartid));
#if __RISCV_XLEN == 32
        __SW(addr, -1U);      // prevent load > timecmp
        __SW(addr + 4, (uint32_t)(value >> 32));
        __SW(addr, (uint32_t)value);
#else
        __SD(addr, value);
#endif
    }
}

/**
 * \brief  Set system timer compare value in machine mode
 * \details
 * This function set the system Timer compare value in MTIMERCMP register.
 * \param [in]  value   compare value to set system timer MTIMERCMP register.
 * \remarks
 * - Compare value is 64bits wide.
 * - If compare value is larger than current value timer interrupt generate.
 * - Modify the load value or compare value less to clear the interrupt.
 * - __get_hart_id function can only be accessed in machine mode, or else exception will occur.
 * - \ref SysTimer_GetCompareValue
 */
__STATIC_FORCEINLINE void SysTimer_SetCompareValue(uint64_t value)
{
    unsigned long hartid = SysTimer_GetHartID();
    SysTimer_SetHartCompareValue(value, hartid);
}

/**
 * \brief  Get system timer compare value by hartid
 * \details
 * This function get the system timer compare value in MTIMERCMP register.
 * \param [in]  hartid  hart ID, one hart is required to have a known hart ID of 0, other harts ID can be in 1~1023.
 * \return  compare value of system timer MTIMERCMP register.
 * \remarks
 * - Compare value is 64bits wide.
 * - In S-mode, hartid can't be get by using __get_hart_id function, so this api suits S-mode particularly.
 * - \ref SysTimer_SetHartCompareValue
 */
__STATIC_FORCEINLINE uint64_t SysTimer_GetHartCompareValue(unsigned long hartid)
{
    if (hartid == 0) {
        return SysTimer->MTIMERCMP;
    } else {
        uint64_t full;
        uint8_t *addr = (uint8_t *)(SysTimer_CLINT_MTIMECMP_BASE(hartid));
#if __RISCV_XLEN == 32
        // MTIMECMP didn't increase
        uint32_t high, low;

        high = __LW(addr + 4);
        low = __LW(addr);
        full = (((uint64_t)high) << 32) | low;
#else
        full = __LD(addr);
#endif
        return full;
    }
}

/**
 * \brief  Get system timer compare value in machine mode
 * \details
 * This function get the system timer compare value in MTIMERCMP register.
 * \return  compare value of system timer MTIMERCMP register.
 * \remarks
 * - Compare value is 64bits wide.
 * - \ref SysTimer_SetCompareValue
 */
__STATIC_FORCEINLINE uint64_t SysTimer_GetCompareValue(void)
{
    unsigned long hartid = SysTimer_GetHartID();
    return SysTimer_GetHartCompareValue(hartid);
}

/**
 * \brief  Enable system timer counter running
 * \details
 * Enable system timer counter running by clear
 * TIMESTOP bit in MTIMECTL register.
 */
__STATIC_FORCEINLINE void SysTimer_Start(void)
{
    SysTimer->MTIMECTL &= ~(SysTimer_MTIMECTL_TIMESTOP_Msk);
}

/**
 * \brief  Stop system timer counter running
 * \details
 * Stop system timer counter running by set
 * TIMESTOP bit in MTIMECTL register.
 */
__STATIC_FORCEINLINE void SysTimer_Stop(void)
{
    SysTimer->MTIMECTL |= SysTimer_MTIMECTL_TIMESTOP_Msk;
}

/**
 * \brief  Set system timer control value
 * \details
 * This function set the system timer MTIMECTL register value.
 * \param [in]  mctl    value to set MTIMECTL register
 * \remarks
 * - Bit TIMESTOP is used to start and stop timer.
 *   Clear TIMESTOP bit to 0 to start timer, otherwise to stop timer.
 * - Bit CMPCLREN is used to enable auto MTIMER clear to zero when MTIMER >= MTIMERCMP.
 *   Clear CMPCLREN bit to 0 to stop auto clear MTIMER feature, otherwise to enable it.
 * - Bit CLKSRC is used to select timer clock source.
 *   Clear CLKSRC bit to 0 to use *mtime_toggle_a*, otherwise use *core_clk_aon*
 * - \ref SysTimer_GetControlValue
 */
__STATIC_FORCEINLINE void SysTimer_SetControlValue(uint32_t mctl)
{
    SysTimer->MTIMECTL = (mctl & SysTimer_MTIMECTL_Msk);
}

/**
 * \brief  Get system timer control value
 * \details
 * This function get the system timer MTIMECTL register value.
 * \return  MTIMECTL register value
 * \remarks
 * - \ref SysTimer_SetControlValue
 */
__STATIC_FORCEINLINE uint32_t SysTimer_GetControlValue(void)
{
    return (SysTimer->MTIMECTL & SysTimer_MTIMECTL_Msk);
}

/**
 * \brief  Trigger or set software interrupt via system timer by hartid
 * \details
 * This function set the system timer MSIP bit in MSIP register.
 * \param [in]  hartid  hart ID, one hart is required to have a known hart ID of 0, other harts ID can be in 1~1023.
 * \remarks
 * - Set system timer MSIP bit and generate a SW interrupt.
 * - In S-mode, hartid can't be get by using __get_hart_id function, so this api suits S-mode particularly.
 * - \ref SysTimer_ClearHartSWIRQ
 * - \ref SysTimer_GetHartMsipValue
 */
__STATIC_FORCEINLINE void SysTimer_SetHartSWIRQ(unsigned long hartid)
{
    if (hartid == 0) {
        SysTimer->MSIP |= SysTimer_MSIP_MSIP_Msk;
    } else {
        uint8_t *addr = (uint8_t *)(SysTimer_CLINT_MSIP_BASE(hartid));
        __SW(addr, SysTimer_MSIP_MSIP_Msk);
    }
}

/**
 * \brief  Trigger or set software interrupt via system timer in machine mode
 * \details
 * This function set the system timer MSIP bit in MSIP register.
 * \remarks
 * - Set system timer MSIP bit and generate a SW interrupt.
 * - \ref SysTimer_ClearSWIRQ
 * - \ref SysTimer_GetMsipValue
 */
__STATIC_FORCEINLINE void SysTimer_SetSWIRQ(void)
{
    unsigned long hartid = SysTimer_GetHartID();
    SysTimer_SetHartSWIRQ(hartid);
}

/**
 * \brief  Clear system timer software interrupt pending request by hartid
 * \details
 * This function clear the system timer MSIP bit in MSIP register.
 * \param [in]  hartid  hart ID, one hart is required to have a known hart ID of 0, other harts ID can be in 1~1023.
 * \remarks
 * - Clear system timer MSIP bit in MSIP register to clear the software interrupt pending.
 * - In S-mode, hartid can't be get by using __get_hart_id function, so this api suits S-mode particularly.
 * - \ref SysTimer_SetHartSWIRQ
 * - \ref SysTimer_GetHartMsipValue
 */
__STATIC_FORCEINLINE void SysTimer_ClearHartSWIRQ(unsigned long hartid)
{
    if (hartid == 0) {
        SysTimer->MSIP &= ~SysTimer_MSIP_MSIP_Msk;
    } else {
        uint8_t *addr = (uint8_t *)(SysTimer_CLINT_MSIP_BASE(hartid));
        __SW(addr, 0);
    }
}

/**
 * \brief  Clear system timer software interrupt pending request in machine mode
 * \details
 * This function clear the system timer MSIP bit in MSIP register.
 * \remarks
 * - Clear system timer MSIP bit in MSIP register to clear the software interrupt pending.
 * - \ref SysTimer_SetSWIRQ
 * - \ref SysTimer_GetMsipValue
 */
__STATIC_FORCEINLINE void SysTimer_ClearSWIRQ(void)
{
    unsigned long hartid = SysTimer_GetHartID();
    SysTimer_ClearHartSWIRQ(hartid);
}

/**
 * \brief  Get system timer MSIP register value by hartid
 * \details
 * This function get the system timer MSIP register value.
 * \param [in]  hartid  hart ID, one hart is required to have a known hart ID of 0, other harts ID can be in 1~1023.
 * \return    Value of Timer MSIP register.
 * \remarks
 * - Bit0 is SW interrupt flag.
 *   Bit0 is 1 then SW interrupt set. Bit0 is 0 then SW interrupt clear.
 * - In S-mode, hartid can't be get by using __get_hart_id function, so this api suits S-mode particularly.
 * - \ref SysTimer_SetHartSWIRQ
 * - \ref SysTimer_ClearHartSWIRQ
 * - \ref SysTimer_SetHartMsipValue
 */
__STATIC_FORCEINLINE uint32_t SysTimer_GetHartMsipValue(unsigned long hartid)
{
    if (hartid == 0) {
        return (uint32_t)(SysTimer->MSIP & SysTimer_MSIP_Msk);
    } else {
        uint8_t *addr = (uint8_t *)(SysTimer_CLINT_MSIP_BASE(hartid));
        return __LW(addr);
    }
}

/**
 * \brief  Get system timer MSIP register value in machine mode
 * \details
 * This function get the system timer MSIP register value.
 * \return    Value of Timer MSIP register.
 * \remarks
 * - Bit0 is SW interrupt flag.
 *   Bit0 is 1 then SW interrupt set. Bit0 is 0 then SW interrupt clear.
 * - \ref SysTimer_SetSWIRQ
 * - \ref SysTimer_ClearSWIRQ
 * - \ref SysTimer_SetMsipValue
 */
__STATIC_FORCEINLINE uint32_t SysTimer_GetMsipValue(void)
{
    unsigned long hartid = SysTimer_GetHartID();
    return SysTimer_GetHartMsipValue(hartid);
}

/**
 * \brief  Set system timer MSIP register value by hartid
 * \details
 * This function set the system timer MSIP register value.
 * \param [in]  msip   value to set MSIP register
 * \param [in]  hartid  hart ID, one hart is required to have a known hart ID of 0, other harts ID can be in 1~1023.
 * \remarks
 * - In S-mode, hartid can't be get using __get_hart_id function, so this api suits S-mode particularly.
 * - \ref SysTimer_GetHartMsipValue
 */
__STATIC_FORCEINLINE void SysTimer_SetHartMsipValue(uint32_t msip, unsigned long hartid)
{
    if (hartid == 0) {
        SysTimer->MSIP = (msip & SysTimer_MSIP_Msk);
    } else {
        uint8_t *addr = (uint8_t *)(SysTimer_CLINT_MSIP_BASE(hartid));
        __SW(addr, msip);
    }
}

/**
 * \brief  Set system timer MSIP register value in machine mode
 * \details
 * This function set the system timer MSIP register value.
 * \param [in]  msip   value to set MSIP register
 * - \ref SysTimer_GetMsipValue
 */
__STATIC_FORCEINLINE void SysTimer_SetMsipValue(uint32_t msip)
{
    unsigned long hartid = SysTimer_GetHartID();
    SysTimer_SetHartMsipValue(msip, hartid);
}

/**
 * \brief  Do software reset request
 * \details
 * This function will do software reset request through MTIMER
 * - Software need to write \ref SysTimer_MSFRST_KEY to generate software reset request
 * - The software request flag can be cleared by reset operation to clear
 * \remarks
 * - The software reset is sent to SoC, SoC need to generate reset signal and send back to Core
 * - This function will not return, it will do while(1) to wait the Core reset happened
 */
__STATIC_FORCEINLINE void SysTimer_SoftwareReset(void)
{
    SysTimer->MSFTRST = SysTimer_MSFRST_KEY;
    // will reset cpu, never return
    while (1);
}

/**
 * \brief  send ipi to target hart using Systimer Clint
 * \details
 * This function send ipi using clint timer.
 * \param [in]  hart  target hart
 */
__STATIC_FORCEINLINE void SysTimer_SendIPI(unsigned long hartid)
{
    uint8_t *addr = (uint8_t *)(SysTimer_CLINT_MSIP_BASE(hartid));
    __SW(addr, 1);
}

/**
 * \brief  clear ipi to target hart using Systimer Clint
 * \details
 * This function clear ipi using Systimer clint timer.
 * \param [in]  hart  target hart
 */
__STATIC_FORCEINLINE void SysTimer_ClearIPI(unsigned long hartid)
{
    uint8_t *addr = (uint8_t *)(SysTimer_CLINT_MSIP_BASE(hartid));
    __SW(addr, 0);
}

#if defined (__Vendor_SysTickConfig) && (__Vendor_SysTickConfig == 0U) && defined(__ECLIC_PRESENT) && (__ECLIC_PRESENT == 1)
/**
 * \brief   System Tick Configuration
 * \details Initializes the System Timer and its non-vector interrupt, and starts the System Tick Timer.
 *
 *  In our default implementation, the timer counter will be set to zero, and it will start a timer compare non-vector interrupt
 *  when it matchs the ticks user set, during the timer interrupt user should reload the system tick using \ref SysTick_Reload function
 *  or similar function written by user, so it can produce period timer interrupt.
 * \param [in]  ticks  Number of ticks between two interrupts.
 * \return          0  Function succeeded.
 * \return          1  Function failed.
 * \remarks
 * - For \ref __NUCLEI_N_REV >= 0x0104, the CMPCLREN bit in MTIMECTL is introduced,
 *   but we assume that the CMPCLREN bit is set to 0, so MTIMER register will not be
 *   auto cleared to 0 when MTIMER >= MTIMERCMP.
 * - When the variable \ref __Vendor_SysTickConfig is set to 1, then the
 *   function \ref SysTick_Config is not included.
 * - In this case, the file <b><Device>.h</b> must contain a vendor-specific implementation
 *   of this function.
 * - If user need this function to start a period timer interrupt, then in timer interrupt handler
 *   routine code, user should call \ref SysTick_Reload with ticks to reload the timer.
 * - This function only available when __TIMER_PRESENT == 1 and __ECLIC_PRESENT == 1 and __Vendor_SysTickConfig == 0
 * \sa
 * - \ref SysTimer_SetCompareValue; SysTimer_SetLoadValue
 */
__STATIC_INLINE uint32_t SysTick_Config(uint64_t ticks)
{
    uint64_t loadticks = SysTimer_GetLoadValue();
    SysTimer_SetCompareValue(ticks + loadticks);
    ECLIC_SetShvIRQ(SysTimer_IRQn, ECLIC_NON_VECTOR_INTERRUPT);
    ECLIC_SetLevelIRQ(SysTimer_IRQn, 0);
    ECLIC_EnableIRQ(SysTimer_IRQn);
    return (0UL);
}

/**
 * \brief   System Tick Configuration By hartid
 * \details Initializes the System Timer and its non-vector interrupt, and starts the System Tick Timer.
 *
 *  In our default implementation, the timer counter will be set to zero, and it will start a timer compare non-vector interrupt
 *  when it matchs the ticks user set, during the timer interrupt user should reload the system tick using \ref SysTick_Reload function
 *  or similar function written by user, so it can produce period timer interrupt.
 * \param [in]  ticks  Number of ticks between two interrupts.
 * \param [in]  hartid  hart ID, one hart is required to have a known hart ID of 0, other harts ID can be in 1~1023.
 * \return          0  Function succeeded.
 * \return          1  Function failed.
 * \remarks
 * - For \ref __NUCLEI_N_REV >= 0x0104, the CMPCLREN bit in MTIMECTL is introduced,
 *   but we assume that the CMPCLREN bit is set to 0, so MTIMER register will not be
 *   auto cleared to 0 when MTIMER >= MTIMERCMP.
 * - When the variable \ref __Vendor_SysTickConfig is set to 1, then the
 *   function \ref SysTick_Config is not included.
 * - In this case, the file <b><Device>.h</b> must contain a vendor-specific implementation
 *   of this function.
 * - If user need this function to start a period timer interrupt, then in timer interrupt handler
 *   routine code, user should call \ref SysTick_Reload with ticks to reload the timer.
 * - This function only available when __TIMER_PRESENT == 1 and __ECLIC_PRESENT == 1 and __Vendor_SysTickConfig == 0
 * - In S-mode, hartid can't be get by using __get_hart_id function, so this api suits S-mode particularly.
 * \sa
 * - \ref SysTimer_SetCompareValue; SysTimer_SetLoadValue
 */
__STATIC_INLINE uint32_t SysTick_HartConfig(uint64_t ticks, unsigned long hartid)
{
    uint64_t loadticks = SysTimer_GetLoadValue();
    SysTimer_SetHartCompareValue(ticks + loadticks, hartid);
    ECLIC_SetShvIRQ(SysTimer_IRQn, ECLIC_NON_VECTOR_INTERRUPT);
    ECLIC_SetLevelIRQ(SysTimer_IRQn, 0);
    ECLIC_EnableIRQ(SysTimer_IRQn);
    return (0UL);
}

/**
 * \brief   System Tick Reload
 * \details Reload the System Timer Tick when the MTIMECMP reached TIME value
 *
 * \param [in]  ticks  Number of ticks between two interrupts.
 * \return          0  Function succeeded.
 * \return          1  Function failed.
 * \remarks
 * - For \ref __NUCLEI_N_REV >= 0x0104, the CMPCLREN bit in MTIMECTL is introduced,
 *   but for this \ref SysTick_Config function, we assume this CMPCLREN bit is set to 0,
 *   so in interrupt handler function, user still need to set the MTIMERCMP or MTIMER to reload
 *   the system tick, if vendor want to use this timer's auto clear feature, they can define
 *   \ref __Vendor_SysTickConfig to 1, and implement \ref SysTick_Config and \ref SysTick_Reload functions.
 * - When the variable \ref __Vendor_SysTickConfig is set to 1, then the
 *   function \ref SysTick_Reload is not included.
 * - In this case, the file <b><Device>.h</b> must contain a vendor-specific implementation
 *   of this function.
 * - This function only available when __TIMER_PRESENT == 1 and __ECLIC_PRESENT == 1 and __Vendor_SysTickConfig == 0
 * - Since the MTIMERCMP value might overflow, if overflowed, MTIMER will be set to 0, and MTIMERCMP set to ticks
 * \sa
 * - \ref SysTimer_SetCompareValue
 * - \ref SysTimer_SetLoadValue
 */
__STATIC_FORCEINLINE uint32_t SysTick_Reload(uint64_t ticks)
{
    uint64_t cur_ticks = SysTimer_GetLoadValue();
    uint64_t reload_ticks = ticks + cur_ticks;

    if (__USUALLY(reload_ticks > cur_ticks)) {
        SysTimer_SetCompareValue(reload_ticks);
    } else {
        /* When added the ticks value, then the MTIMERCMP < TIMER,
         * which means the MTIMERCMP is overflowed,
         * so we need to reset the counter to zero */
        SysTimer_SetLoadValue(0);
        SysTimer_SetCompareValue(ticks);
    }

    return (0UL);
}

/**
 * \brief   System Tick Reload
 * \details Reload the System Timer Tick when the MTIMECMP reached TIME value
 *
 * \param [in]  ticks  Number of ticks between two interrupts.
 * \param [in]  hartid  hart ID, one hart is required to have a known hart ID of 0, other harts ID can be in 1~1023.
 * \return          0  Function succeeded.
 * \return          1  Function failed.
 * \remarks
 * - For \ref __NUCLEI_N_REV >= 0x0104, the CMPCLREN bit in MTIMECTL is introduced,
 *   but for this \ref SysTick_Config function, we assume this CMPCLREN bit is set to 0,
 *   so in interrupt handler function, user still need to set the MTIMERCMP or MTIMER to reload
 *   the system tick, if vendor want to use this timer's auto clear feature, they can define
 *   \ref __Vendor_SysTickConfig to 1, and implement \ref SysTick_Config and \ref SysTick_Reload functions.
 * - When the variable \ref __Vendor_SysTickConfig is set to 1, then the
 *   function \ref SysTick_Reload is not included.
 * - In this case, the file <b><Device>.h</b> must contain a vendor-specific implementation
 *   of this function.
 * - This function only available when __TIMER_PRESENT == 1 and __ECLIC_PRESENT == 1 and __Vendor_SysTickConfig == 0
 * - Since the MTIMERCMP value might overflow, if overflowed, MTIMER will be set to 0, and MTIMERCMP set to ticks
 * - In S-mode, hartid can't be get by using __get_hart_id function, so this api suits S-mode particularly.
 * \sa
 * - \ref SysTimer_SetCompareValue
 * - \ref SysTimer_SetLoadValue
 */
__STATIC_FORCEINLINE uint32_t SysTick_HartReload(uint64_t ticks, unsigned long hartid)
{
    uint64_t cur_ticks = SysTimer_GetLoadValue();
    uint64_t reload_ticks = ticks + cur_ticks;

    if (__USUALLY(reload_ticks > cur_ticks)) {
        SysTimer_SetHartCompareValue(reload_ticks, hartid);
    } else {
        /* When added the ticks value, then the MTIMERCMP < TIMER,
         * which means the MTIMERCMP is overflowed,
         * so we need to reset the counter to zero */
        SysTimer_SetLoadValue(0);
        SysTimer_SetHartCompareValue(ticks, hartid);
    }

    return (0UL);
}

#endif /* defined(__Vendor_SysTickConfig) && (__Vendor_SysTickConfig == 0U) */
/** @} */ /* End of Doxygen Group NMSIS_Core_SysTimer */

#endif /* defined(__TIMER_PRESENT) && (__TIMER_PRESENT == 1) */

#ifdef __cplusplus
}
#endif
#endif /* __CORE_FEATURE_TIMER_H__ */
