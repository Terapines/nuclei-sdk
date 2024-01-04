/*
 * Copyright (c) 2009-2018 Arm Limited. All rights reserved.
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
/******************************************************************************
 * @file     system_evalsoc.c
 * @brief    NMSIS Nuclei Core Device Peripheral Access Layer Source File for
 *           Nuclei Eval SoC which support Nuclei N/NX class cores
 * @version  V1.00
 * @date     22. Nov 2019
 ******************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include "nuclei_sdk_hal.h"

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
/* ToDo: add here your necessary defines for device initialization
         following is an example for different system frequencies */
#ifndef SYSTEM_CLOCK
#define SYSTEM_CLOCK    (80000000UL)
#endif

/**
 * \defgroup  NMSIS_Core_SystemConfig       System Device Configuration
 * \brief Functions for system and clock setup available in system_<device>.c.
 * \details
 * Nuclei provides a template file **system_Device.c** that must be adapted by
 * the silicon vendor to match their actual device. As a <b>minimum requirement</b>,
 * this file must provide:
 *  -  A device-specific system configuration function, \ref SystemInit.
 *  -  Global c library \ref _premain_init and \ref _postmain_fini functions called right before calling main function.
 *     -  A global variable that contains the system frequency, \ref SystemCoreClock.
 *     -  A global interrupt configuration initialization, \ref Interrupt_Init.
 *     -  A global exception and trap configuration initialization, \ref Trap_Init and \ref Exception_Init.
 *  -  Vendor customized interrupt, exception handling code, see \ref NMSIS_Core_IntExcNMI_Handling
 *
 * The file configures the device and, typically, initializes the oscillator (PLL) that is part
 * of the microcontroller device. This file might export other functions or variables that provide
 * a more flexible configuration of the microcontroller system.
 *
 * And this file also provided common interrupt, exception exception handling framework template,
 * Silicon vendor can customize these template code as they want.
 *
 * \note Please pay special attention to the static variable \c SystemCoreClock. This variable might be
 * used throughout the whole system initialization and runtime to calculate frequency/time related values.
 * Thus one must assure that the variable always reflects the actual system clock speed.
 *
 * \attention
 * Be aware that a value stored to \c SystemCoreClock during low level initialization (i.e. \c SystemInit()) might get
 * overwritten by C libray startup code and/or .bss section initialization.
 * Thus its highly recommended to call \ref SystemCoreClockUpdate at the beginning of the user \c main() routine.
 *
 * @{
 */
/*----------------------------------------------------------------------------
  System Core Clock Variable
 *----------------------------------------------------------------------------*/
/* ToDo: initialize SystemCoreClock with the system core clock frequency value
         achieved after system intitialization.
         This means system core clock frequency after call to SystemInit() */
/**
 * \brief      Variable to hold the system core clock value
 * \details
 * Holds the system core clock, which is the system clock frequency supplied to the SysTick
 * timer and the processor core clock. This variable can be used by debuggers to query the
 * frequency of the debug timer or to configure the trace clock speed.
 *
 * \attention
 * Compilers must be configured to avoid removing this variable in case the application
 * program is not using it. Debugging systems require the variable to be physically
 * present in memory so that it can be examined to configure the debugger.
 */
volatile uint32_t SystemCoreClock = SYSTEM_CLOCK;  /* System Clock Frequency (Core Clock) */

/*----------------------------------------------------------------------------
  Clock functions
 *----------------------------------------------------------------------------*/

/**
 * \brief      Function to update the variable \ref SystemCoreClock
 * \details
 * Updates the variable \ref SystemCoreClock and must be called whenever the core clock is changed
 * during program execution. The function evaluates the clock register settings and calculates
 * the current core clock.
 */
void SystemCoreClockUpdate(void)             /* Get Core Clock Frequency */
{
    /* ToDo: add code to calculate the system frequency based upon the current
     *    register settings.
     * Note: This function can be used to retrieve the system core clock frequeny
     *    after user changed register settings.
     */
    SystemCoreClock = SYSTEM_CLOCK;
}

/**
 * \brief      Function to Initialize the system.
 * \details
 * Initializes the microcontroller system. Typically, this function configures the
 * oscillator (PLL) that is part of the microcontroller device. For systems
 * with a variable clock speed, it updates the variable \ref SystemCoreClock.
 * SystemInit is called from the file <b>startup<i>_device</i></b>.
 */
void SystemInit(void)
{
    /* ToDo: add code to initialize the system
     * Warn: do not use global variables because this function is called before
     * reaching pre-main. RW section maybe overwritten afterwards.
     */
    SystemCoreClock = SYSTEM_CLOCK;
}

/**
 * \defgroup  NMSIS_Core_IntExcNMI_Handling   Interrupt and Exception Handling
 * \brief Functions for interrupt, exception and nmi handle available in system_<device>.c.
 * \details
 * Nuclei provide a template for interrupt, exception handling. Silicon Vendor could adapat according
 * to their requirement. Silicon vendor could implement interface for different exception code and
 * replace current implementation.
 *
 * @{
 */
// TODO If you don't want any exception handler print any thing, uncomment below macro
// Define this will reduce code size and less debug message when exception happened
//#define DISABLE_EXCEPTION_DEBUG

extern __WEAK void irqc_mtip_handler(void);
extern __WEAK void irqc_msip_handler(void);
extern __WEAK void irqc_uart0_handler(void);
extern void default_intexc_handler(void);

typedef void (*__fp)(void);
// TODO irqc vector table
// Please fill in this irq vector table with your real interrupt function name
// MUST marked as __USED to avoid unused variable elimination
static const __fp vector_base[32] __USED __attribute__((section (".mintvec"))) = {
    irqc_msip_handler,          /* irq 0 ,  internal irq 0  */
    irqc_mtip_handler,          /* irq 1 ,  internal irq 1  */
    irqc_uart0_handler,         /* irq 2 ,  ext_irq 0       */
    default_intexc_handler,     /* irq 3 ,  ext_irq 1       */
    default_intexc_handler,     /* irq 4 ,  ext_irq 2       */
    default_intexc_handler,     /* irq 5 ,  ext_irq 3       */
    default_intexc_handler,     /* irq 6 ,  ext_irq 4       */
    default_intexc_handler,     /* irq 7 ,  ext_irq 5       */
    default_intexc_handler,     /* irq 8 ,  ext_irq 6       */
    default_intexc_handler,     /* irq 9 ,  ext_irq 7       */
    default_intexc_handler,     /* irq 10,  ext_irq 8       */
    default_intexc_handler,     /* irq 11,  ext_irq 9       */
    default_intexc_handler,     /* irq 12,  ext_irq 10      */
    default_intexc_handler,     /* irq 13,  ext_irq 11      */
    default_intexc_handler,     /* irq 14,  ext_irq 12      */
    default_intexc_handler,     /* irq 15,  ext_irq 13      */
    default_intexc_handler,     /* irq 16,  ext_irq 14      */
    default_intexc_handler,     /* irq 17,  ext_irq 15      */
    default_intexc_handler,     /* irq 18,  ext_irq 16      */
    default_intexc_handler,     /* irq 19,  ext_irq 17      */
    default_intexc_handler,     /* irq 20,  ext_irq 18      */
    default_intexc_handler,     /* irq 21,  ext_irq 19      */
    default_intexc_handler,     /* irq 22,  ext_irq 20      */
    default_intexc_handler,     /* irq 23,  ext_irq 21      */
    default_intexc_handler,     /* irq 24,  ext_irq 22      */
    default_intexc_handler,     /* irq 25,  ext_irq 23      */
    default_intexc_handler,     /* irq 26,  ext_irq 24      */
    default_intexc_handler,     /* irq 27,  ext_irq 25      */
    default_intexc_handler,     /* irq 28,  ext_irq 26      */
    default_intexc_handler,     /* irq 29,  ext_irq 27      */
    default_intexc_handler,     /* irq 30,  ext_irq 28      */
    default_intexc_handler      /* irq 31,  ext_irq 29      */
};

/* TODO: Using default exception handling code provided by NMSIS Device Template.
 * If you want to disable it and use your own one, you can implement Exception_Init and core_exception_handler function
 */
#if defined(__Vendor_EXCEPTION) && (__Vendor_EXCEPTION == 0)

/** \brief Max exception handler number */
#define MAX_SYSTEM_EXCEPTION_NUM        12
/**
 * \brief      Store the exception handlers for each exception ID
 * \note
 * - This SystemExceptionHandlers are used to store all the handlers for all
 * the exception codes Nuclei N100 core provided.
 * - Exception code 0 - 11, totally 12 exceptions are mapped to SystemExceptionHandlers[0:11]
 */
static unsigned long SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM];

/**
 * \brief      Exception Handler Function Typedef
 * \note
 * This typedef is only used internal in this system_<Device>.c file.
 * It is used to do type conversion for registered exception handler before calling it.
 */
typedef void (*EXC_HANDLER)(unsigned long cause, unsigned long sp);

/**
 * \brief      System Default Exception Handler
 * \details
 * This function provides a default exception handler for all exception ids.
 * By default, It will just print some information for debug, Vendor can customize it according to its requirements.
 * \param [in]  mcause    code indicating the reason that caused the trap in machine mode
 * \param [in]  sp        stack pointer
 */
static void system_default_exception_handler(unsigned long mcause, unsigned long sp)
{
#ifndef DISABLE_EXCEPTION_DEBUG
    /* TODO: Uncomment this if you have implement printf function */
    printf("MCAUSE : 0x%lx\r\n", mcause);
    printf("MEPC   : 0x%lx\r\n", __RV_CSR_READ(CSR_MEPC));
    printf("HARTID : %u\r\n", (unsigned int)__get_hart_id());
    Exception_DumpFrame(sp, PRV_M);
#if defined(SIMULATION_MODE)
    // directly exit if in SIMULATION
    extern void simulation_exit(int status);
    simulation_exit(1);
#endif
#endif
    while (1) {
        __WFI();
    }
}

/**
 * \brief      Initialize all the default core exception handlers
 * \details
 * The core exception handler for each exception id will be initialized to \ref system_default_exception_handler.
 * \note
 * Called in \ref _premain_init function, used to initialize default exception handlers for all exception IDs
 */
void Exception_Init(void)
{
    for (int i = 0; i < MAX_SYSTEM_EXCEPTION_NUM; i++) {
        SystemExceptionHandlers[i] = (unsigned long)system_default_exception_handler;
    }
}

/**
 * \brief      Dump Exception Frame
 * \details
 * This function provided feature to dump exception frame stored in stack.
 * \param [in]  sp    stackpoint
 * \param [in]  mode  privileged mode
 */
void Exception_DumpFrame(unsigned long sp, uint8_t mode)
{
#ifndef DISABLE_EXCEPTION_DEBUG
    EXC_Frame_Type *exc_frame = (EXC_Frame_Type *)sp;

#ifndef __riscv_32e
    printf("ra: 0x%lx, tp: 0x%lx, t0: 0x%lx, t1: 0x%lx, t2: 0x%lx, t3: 0x%lx, t4: 0x%lx, t5: 0x%lx, t6: 0x%lx\n" \
           "a0: 0x%lx, a1: 0x%lx, a2: 0x%lx, a3: 0x%lx, a4: 0x%lx, a5: 0x%lx, a6: 0x%lx, a7: 0x%lx\n" \
           "cause: 0x%lx, epc: 0x%lx\n", exc_frame->ra, exc_frame->tp, exc_frame->t0, \
           exc_frame->t1, exc_frame->t2, exc_frame->t3, exc_frame->t4, exc_frame->t5, exc_frame->t6, \
           exc_frame->a0, exc_frame->a1, exc_frame->a2, exc_frame->a3, exc_frame->a4, exc_frame->a5, \
           exc_frame->a6, exc_frame->a7, exc_frame->cause, exc_frame->epc);
#else
    printf("ra: 0x%lx, tp: 0x%lx, t0: 0x%lx, t1: 0x%lx, t2: 0x%lx\n" \
           "a0: 0x%lx, a1: 0x%lx, a2: 0x%lx, a3: 0x%lx, a4: 0x%lx, a5: 0x%lx\n" \
           "cause: 0x%lx, epc: 0x%lx\n", exc_frame->ra, exc_frame->tp, exc_frame->t0, \
           exc_frame->t1, exc_frame->t2, exc_frame->a0, exc_frame->a1, exc_frame->a2, exc_frame->a3, \
           exc_frame->a4, exc_frame->a5, exc_frame->cause, exc_frame->epc);
#endif
#endif
}

/**
 * \brief       Register an exception handler for exception code EXCn
 * \details
 * - For EXCn < \ref MAX_SYSTEM_EXCEPTION_NUM, it will be registered into SystemExceptionHandlers[EXCn].
 * \param [in]  EXCn    See \ref EXCn_Type
 * \param [in]  exc_handler     The exception handler for this exception code EXCn
 */
void Exception_Register_EXC(uint32_t EXCn, unsigned long exc_handler)
{
    if (EXCn < MAX_SYSTEM_EXCEPTION_NUM) {
        SystemExceptionHandlers[EXCn] = exc_handler;
    }
}

/**
 * \brief       Get current exception handler for exception code EXCn
 * \details
 * - For EXCn < \ref MAX_SYSTEM_EXCEPTION_NUM, it will return SystemExceptionHandlers[EXCn].
 * \param [in]  EXCn    See \ref EXCn_Type
 * \return  Current exception handler for exception code EXCn, if not found, return 0.
 */
unsigned long Exception_Get_EXC(uint32_t EXCn)
{
    if (EXCn < MAX_SYSTEM_EXCEPTION_NUM) {
        return SystemExceptionHandlers[EXCn];
    }
}

/**
 * \brief      Common Exception handler entry
 * \details
 * This function provided a command entry for exception. Silicon Vendor could modify
 * this template implementation according to requirement.
 * \param [in]  mcause    code indicating the reason that caused the trap in machine mode
 * \param [in]  sp        stack pointer
 * \remarks
 * - RISCV provided common entry for all types of exception. This is proposed code template
 *   for exception entry function, Silicon Vendor could modify the implementation.
 * - For the core_exception_handler template, we provided exception register function \ref Exception_Register_EXC
 *   which can help developer to register your exception handler for specific exception number.
 */
uint32_t core_exception_handler(unsigned long mcause, unsigned long sp)
{
    uint32_t EXCn = (uint32_t)(mcause & 0X00000fff);
    EXC_HANDLER exc_handler;

    if (EXCn < MAX_SYSTEM_EXCEPTION_NUM) {
        exc_handler = (EXC_HANDLER)SystemExceptionHandlers[EXCn];
    } else {
        exc_handler = (EXC_HANDLER)system_default_exception_handler;
    }
    if (exc_handler != NULL) {
        exc_handler(mcause, sp);
    }
    return 0;
}
#else /* Use Vendor implemented exception handling code __Vendor_EXCEPTION == 1 */

// TODO implement core_exception_handler and Exception_Init if __Vendor_EXCEPTION == 1
__WEAK uint32_t core_exception_handler(unsigned long mcause, unsigned long sp)
{
    while (1) {
        __WFI();
    }
}

__WEAK void Exception_Init(void)
{
}

#endif

/** @} */ /* End of Doxygen Group NMSIS_Core_ExceptionAndNMI */

/** Banner Print for Nuclei SDK */
void SystemBannerPrint(void)
{
// TODO to reduce code size and less message output.
// you can set NUCLEI_BANNER to 0 in nuclei_sdk_hal.h
// but it will show no messsage when bringup
#if defined(NUCLEI_BANNER) && (NUCLEI_BANNER == 1)
    printf("N100 Nuclei SDK Build Time: %s, %s\r\n", __DATE__, __TIME__);
#ifdef DOWNLOAD_MODE_STRING
    printf("Download Mode: %s\r\n", DOWNLOAD_MODE_STRING);
#endif
    printf("CPU Frequency %u Hz\r\n", (unsigned int)SystemCoreClock);
    printf("CPU HartID: %u\r\n", (unsigned int)__get_hart_id());
#endif
}

/**
 * \brief initialize interrupt config
 * \details
 * interrupt needs be initialized after boot up
 */
void Interrupt_Init(void)
{
}

/**
 * \brief  Initialize a specific IRQ and register the handler
 * \details
 * This function set vector mode, trigger mode and polarity, interrupt level and priority,
 * assign handler for specific IRQn.
 * \param [in]  IRQn        interrupt handler address
 * \param [in]  handler     interrupt handler, if NULL, handler will not be installed
 * \return       -1 means invalid input parameter. 0 means successful.
 * \remarks
 * - This function use to configure specific irqc interrupt and register its interrupt handler and enable its interrupt.
 * - If the vector table is placed in read-only section(FLASHXIP mode), handler could not be installed
 */
int32_t IRQC_Register_IRQ(IRQn_Type IRQn, void* handler)
{
    if (IRQn >= SOC_INT_MAX) {
    }
    if (handler != NULL) {
        IRQC_SetVector(IRQn, (rv_csr_t)handler);
    }
    IRQC_EnableIRQ(IRQn);
    return 0;
}

/**
 * \brief do the init for trap
 * \details
 */
static void Trap_Init(void)
{
}

/**
 * \brief early init function before main
 * \details
 * This function is executed right before main function.
 * For RISC-V gnu toolchain, _init function might not be called
 * by __libc_init_array function, so we defined a new function
 * to do initialization.
 */
void _premain_init(void)
{
    // TODO Code below used to do premain init, you can place pre main init code here
    // TODO implement get_cpu_freq function to get real cpu clock freq in HZ or directly give the real cpu HZ
    // Or directly give the correct freqency, no need to use this function
    SystemCoreClock = get_cpu_freq();
    uart_init(SOC_DEBUG_UART, 115200);
    /* Display banner after UART initialized */
    SystemBannerPrint();
    /* Initialize exception default handlers */
    Exception_Init();
    /* Interrupt initialization, mainly MTH and NLBIT */
    Interrupt_Init();
    Trap_Init();
}

/**
 * \brief finish function after main
 * \param [in]  status     status code return from main
 * \details
 * This function is executed right after main function.
 * For RISC-V gnu toolchain, _fini function might not be called
 * by __libc_fini_array function, so we defined a new function
 * to do initialization
 */
void _postmain_fini(int status)
{
    /* TODO: Add your own finishing code here, called after main */
    extern void simulation_exit(int status);
    simulation_exit(status);
}

/** @} */ /* End of Doxygen Group NMSIS_Core_SystemConfig */
