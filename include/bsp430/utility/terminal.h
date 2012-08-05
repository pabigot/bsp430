/* Copyright (c) 2012, Peter A. Bigot <bigotp@acm.org>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the software nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** @file
 *
 * @brief Support for a terminal-capable command line processor.
 *
 * In gross concept, this module addresses needs similar to those
 * addressed by <a
 * href="http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_CLI/FreeRTOS_Plus_Command_Line_Interface.shtml">FreeRTOS-Plus-CLI</a>.
 * This version incorporates several differences in approach,
 * including:
 *
 * @li the assumption that command strings are text (char) rather than
 * signed bytes;

 * @li that commands may have arbitrary parameters; and

 * @li that the infrastructure should assist by stripping off parts of
 * the command string that it has processed but should not presume to
 * understand how to parse the remainder of the command line.
 *
 * In addition, this module provides a task that is responsible for
 * executing commands that are submitted to it.  That task also
 * supports an interactive environment through which commands may be
 * entered and their results displayed.  It is expected that
 * applications which use this feature will use that interface as
 * their console, and thus the infrastructure permits asynchronous
 * notification of events.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_TERMINAL_H
#define BSP430_UTILITY_TERMINAL_H

#include <bsp430/periph.h>
#include <bsp430/serial.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

/** @def configBSP430_TERMINAL_STACK_ADJUSTMENT
 *
 * The number of words above #configMINIMAL_STACK_SIZE to be added to
 * the stack of terminal task.
 *
 * Since the terminal task invokes printf(3c) which has a fairly large
 * stack-based buffer and which is often interrupted by a hardware ISR
 * that must also use the stack, you probably don't want this below
 * 96.  You may need to increase it if you do a lot in the command
 * processing.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_TERMINAL_STACK_ADJUSTMENT
#define configBSP430_TERMINAL_STACK_ADJUSTMENT (48 + 40 * sizeof(portSAVED_REGISTER_TYPE) / sizeof(portBASE_TYPE))
#endif /* configBSP430_TERMINAL_STACK_ADJUSTMENT */

/** @def configBSP430_TERMINAL_READER_STACK_ADJUSTMENT
 *
 * The number of words above #configMINIMAL_STACK_SIZE to be added to
 * the stack of the reader assistant task.
 *
 * This task only needs to aggregate incoming characters and hand them
 * off to the terminal task, so you probably don't need to change
 * this.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_TERMINAL_READER_STACK_ADJUSTMENT
#define configBSP430_TERMINAL_READER_STACK_ADJUSTMENT (12 + 4 * sizeof(portSAVED_REGISTER_TYPE) / sizeof(portBASE_TYPE))
#endif /* configBSP430_TERMINAL_READER_STACK_ADJUSTMENT */


/* Forward declarations */
struct BSP430_HAL_COMMAND_DEFN;
struct BSP430_HAL_TERMINAL_STATE;

/** The terminal state structure is private to the implementation. */
typedef struct BSP430_HAL_TERMINAL_STATE * xBSP430TerminalHandle;

/** Prototype for functions that execute commands.
 *
 * @param pxCommandDefn The command definition structure used to
 * locate this command.  This structure may contain instance-specific
 * information, such as a sub-command table.
 *
 * @param pcArgString The remainder of the command string, after
 * having removed the prefix that identified the command to execute.
 * Any remaining leading whitespace is not removed.
 */
typedef void (* xBSP430CommandFunction) (xBSP430TerminalHandle terminal,
    struct BSP430_HAL_COMMAND_DEFN * pxCommandDefn,
    char * pcArgString);

/** Definition of a command that is recognized by this
 * infrastructure.
 *
 * Note that it is expected that this structure may be embedded within
 * a larger structure in a fashion that allows the command processor
 * to access the remainder of the enclosing structure to obtain other
 * information related to processing the command. */
typedef struct BSP430_HAL_COMMAND_DEFN {
  /** An identifier used to recognize this particular command.
   *
   * The value should be a sequence of characters that are not
   * isspace() characters.  Commands are recognized when the first
   * sequence of non-space characters in a command line match this
   * tag.  The match is not to a prefix of either the tag or the
   * command line. */
  const char * tag;

  /** Pointer to the next command available in the list.  The
   * pointer is null for the last command in a set, and should be
   * null at all times when the command structure is not linked into
   * a set. */
  struct BSP430_HAL_COMMAND_DEFN * next;

  /** Optional pointer to a string describing how the command works.
   * Appropriate contents would include a template for any arguments
   * for the command, and a description of what the command does. */
  char * help;

  /** The function used to execute the command. */
  xBSP430CommandFunction function;
} xBSP430CommandDefn;

/** A group of commands used to interpret a given command string.
 *
 * For example, a set of operations related to task management might
 * be aggregated into a "task" group and contain subcommands like
 * "start", "stop", "status". */
typedef struct BSP430_HAL_COMMAND_GROUP {
  /** An identifier for the group.  This is used primarily for
   * diagnostic purposes. */
  char * group;

  /** A pointer to the first command in the group.  This must be
   * null prior to adding the first command to the group.  An order
   * on commands may be imposed by the infrastructure, e.g. to sort
   * them for the purposes of displaying help. */
  xBSP430CommandDefn * commands;
} xBSP430CommandGroup;

/** Add a command to a command set.
 *
 * @param pxCommandGroup A pointer to a structure holding a group of
 * related commands.
 *
 * @param pxCommand A pointer to a definition of a command to be added
 * to the group.  This definition must not be linked into any other
 * command group.
 */
void vBSP430CommandAdd (xBSP430CommandGroup * pxCommandGroup,
                        xBSP430CommandDefn * pxCommand);

/** Structure holding the relevant information required to create a
 * terminal interface. */
typedef struct BSP430_HAL_TERMINAL_CONFIGURATION {
  /** TEMPORARY The device ID for the UART device that manages the
   * connection */
  tBSP430periphHandle uart;

  /** Parameter to #hBSP430serialOpen */
  unsigned int control_word;

  /** Parameter to #hBSP430serialOpen */
  unsigned long baud;

  /** Priority of the terminal task.  Processing of commands
   * received over the serial interface, and dispatch of displayed
   * data, will be done at this priority.  If the priority is
   * nonzero, the supporting reader task (if required) will be one
   * priority level lower. */
  unsigned portBASE_TYPE uxPriority;

  /** The top-level commands supported by this terminal interface. */
  xBSP430CommandGroup * pxCommandGroup;

  /** The number of bytes to be made available to commands as a
   * display buffer for the terminal.  If zero, no display buffer
   * will be allocated.  This length includes a required terminating
   * NUL as the end-of-string marker. */
  unsigned short usDisplayBufferLength;

  /** The number of bytes to be reserved for commands originating
   * over the serial interface.  If zero, the serial input facility
   * will be disabled.  This length includes a required terminating
   * NUL as the end-of-string marker. */
  unsigned short usCommandBufferLength;

  /** The length of the queue filled by the UART receive interrupt.
   * If zero, the terminal does not accept input, which may be
   * appropriate if it is solely a command processing task,
   * optionally with notification capability. */
  uint8_t ucReceiveQueueLength;

  /** The length of the queue drained by the UART transmit
   * interrupt.  Must be nonzero. */
  uint8_t ucTransmitQueueLength;

  /** The length of the queue used to hold events to be processed by
   * the terminal task, including received characters and
   * asynchronous notifications. */
  uint8_t ucEventQueueLength;
} xBSP430TerminalConfiguration;

/** Configure and allocate tasks that manage a terminal.
 *
 * @param pxTerminalConfig The desired terminal configuration.
 *
 * @return A pointer to a terminal, or NULL if the terminal could not
 * be allocated. */
xBSP430TerminalHandle xBSP430TerminalCreate (const xBSP430TerminalConfiguration * pxTerminalConfig);

/** Set the string to be used as a terminal prompt.
 *
 * @param xTerminal Handle for the terminal that should use this
 * string as a prompt.
 *
 * @param pcPrompt Text to be emitted when a prompt is required. */
void vBSP430TerminalSetPrompt (xBSP430TerminalHandle xTerminal,
                               const char * pcPrompt);

/** Evaluate a command in the context of a terminal.
 *
 * @param xTerminal Handle for the terminal to be used for feedback
 * from the command and for diagnostics on invalid commands.  May be a
 * null pointer, in which case feedback must be provided out-of-band.
 *
 * @param pxCommandGroup The set of commands recognized at this level.
 *
 * @param pcCommand The string expressing the command to be executed.
 * The first non-space token of this is used to locate a relevant
 * command in the command group.
 *
 * @return 0 if the command was successfully executed, or an error code.
 */
int vBSP430CommandProcess (xBSP430TerminalHandle xTerminal,
                           xBSP430CommandGroup * pxCommandGroup,
                           char * pcCommand);

/** Display text on the terminal.
 *
 * This command queues an event to be handled by the terminal thread,
 * which will result in the provided text being displayed.
 *
 * @param xTerminal Handle for the terminal on which the text is
 * displayed.
 *
 * @param pcDisplayText The text to be displayed.
 *
 * @param xTicksToWait Duration, in ticks, to wait for space on the
 * terminal event queue.  If this function is invoked by the terminal
 * task itself, the wait will automatically be set to zero to avoid
 * blocking that task.
 *
 * @param xNotifySemaphore A semaphore which will be released by the
 * terminal task once it has completed displaying the requested text.
 * May be a null handle to denote that no notification is required.
 * If the invocation of this function fails, the caller is responsible
 * for releasing the semaphore if necessary.
 *
 * @return pdPASS if the event was queued for the terminal; pdFAIL if
 * it was not queued.
 */
portBASE_TYPE xBSP430TerminalDisplayText (xBSP430TerminalHandle xTerminal,
    const char * pcDisplayText,
    portTickType xTicksToWait,
    xSemaphoreHandle xNotifySemaphore);

/** Display an integral event code on the terminal.
 *
 * This command queues an event to be handled by the terminal thread,
 * which will result in the provided code being displayed.  Since this
 * event does not require a text buffer, it may be more appropriate
 * than xBSP430TerminalDisplayText() for basic notifications.
 *
 * @param xTerminal Handle for the terminal on which the text is
 * displayed.
 *
 * @param xCode The code to be displayed.
 *
 * @param xTicksToWait Duration, in ticks, to wait for space on the
 * terminal event queue.  If this function is invoked by the terminal
 * task itself, the wait will automatically be set to zero to avoid
 * blocking that task.
 *
 * @param xNotifySemaphore A semaphore which will be released by the
 * terminal task once it has completed displaying the requested text.
 * May be a null handle to denote that no notification is required.
 * If the invocation of this function fails, the caller is responsible
 * for releasing the semaphore if necessary.
 *
 * @return pdPASS if the event was queued for the terminal; pdFAIL if
 * it was not queued.
 */
portBASE_TYPE xBSP430TerminalDisplayCode (xBSP430TerminalHandle xTerminal,
    portBASE_TYPE xCode,
    portTickType xTicksToWait,
    xSemaphoreHandle xNotifySemaphore);

/** Request access to the terminal's shared display buffer.
 *
 * To reduce the memory required for buffers, a terminal may maintain
 * a buffer into which commands may write their results.  In normal
 * practice, the command executes in the terminal's thread, obtains
 * access to the shared buffer, stores the results there, then asks
 * the terminal to display it.  In some cases, it may be useful for
 * the buffer to be accessed by another task.
 *
 * @param xTerminal Handle for the terminal providing the buffer, and
 * on which the data will be displayed.
 *
 * @param pusAvailable Optional pointer to a location into which the
 * length of the returned buffer will be written.  If null, the length
 * of the buffer cannot be provided, but some existing FreeRTOS APIs
 * do not provide a way to avoid overwriting buffers so for those APIs
 * it isn't needed.  The length is written only if this function
 * returns a non-null pointer.
 *
 * @param xTicksToWait Duration, in ticks, to wait for another process
 * to return the buffer and make it available.  If this function is
 * invoked by the terminal task itself, the wait will automatically be
 * set to zero to avoid blocking that task.
 *
 * @return A pointer to a buffer, or a null pointer if the buffer
 * could not be provided.  If a non-null pointer is returned, the
 * caller must eventually invoke xBSP430TerminalReturnDisplay() to
 * release the hold on the buffer. */
char * pcBSP430TerminalRequestDisplay (xBSP430TerminalHandle xTerminal,
                                       unsigned short * pusAvailable,
                                       portTickType xTicksToWait);

/** Release access to the terminal's shared display buffer.
 *
 * @param xTerminal Handle for the terminal that provided the buffer,
 * and on which the data will be displayed.
 *
 * @param pcDisplayText The text to be displayed.  This must be a
 * pointer that lies within the buffer returned by the preceding call
 * to pcBSP430TerminalRequestDisplay().
 *
 * @param xTicksToWait Duration, in ticks, to wait for another process
 * to return the buffer and make it available.  If this function is
 * invoked by the terminal task itself, the wait will automatically be
 * set to zero to avoid blocking that task.
 *
 * @return pdTRUE if the request to display was queued for the
 * terminal, pdFAIL if the request was not queued for the terminal.
 * Regardless of return value, the caller is not permitted to access
 * the memory in the buffer after this function returns. */
portBASE_TYPE xBSP430TerminalReturnDisplay (xBSP430TerminalHandle xTerminal,
    const char * pcDisplayText,
    portTickType xTicksToWait);

#endif /* BSP430_UTILITY_TERMINAL_H */
