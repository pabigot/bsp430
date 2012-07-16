/** @file
 *
 * Definitions for a terminal-capable command line processor.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @copyright @link http://www.opensource.org/licenses/BSD-3-Clause BSD 3-Clause @endlink
 */

#include <bsp430/utility/terminal.h>
#include <bsp430/timers/timerA0.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

/** The internal state necessary to manage a terminal task. */
typedef struct xBSP430_TERMINAL_STATE {
	/** Device used to display results and receive commands. */
	xBSP430uartHandle hsuart;

	/** Queue for receiving data.  The reader task pulls characters
	 * from this and aggregates them for handling by the terminal
	 * task.  Null if no reader task is available. */
	xQueueHandle rx_queue;

	/** Queue for transmitting data.  The terminal task adds
	 * characters, and the hsuart device infrastructure pulls them off
	 * and transmits them. */
	xQueueHandle tx_queue;

	/** Queue for signalling the terminal task of requested actions.
	 * All event structures passed on this are private to the terminal
	 * implementation. */
	xQueueHandle event_queue;

	/** Handle for the reader task.  Null if no reader is available
	 * (no command buffer provided). */
	xTaskHandle reader_task;

	/** Handle for the terminal task itself. */
	xTaskHandle terminal_task;

	/** The top level set of commands handled by the terminal. */
	xBSP430CommandGroup * pxCommandGroup;

	/** The prompt to be displayed if the serial interface is present
	 * and accepts input. */
	const char * prompt;

	/** Maximum length (including terminating NUL) for a command
	 * received over the serial interface. */
	unsigned short command_buffer_length;

	/** Buffer used to accumulate a command received over the serial
	 * interface. */
	char * command_buffer;

	/** Maximum length (including terminating NUL) of the shared
	 * display buffer. */
	unsigned short display_buffer_length;

	/** The shared display buffer, or NULL if shared display buffer is
	 * not supported. */
	char * display_buffer;

	/** Semaphore indicating whewther the shared display buffer is
	 * available for use; handle is NULL if shared display buffer is
	 * not supported. */
	xSemaphoreHandle display_buffer_sema;
} xBSP430Terminal;

typedef enum eventCode_e {
	EVT_UNDEFINED,
	
	/** An event providing input characters to the console. */
	EVT_RX_CHARS,

	/** An event indicating that some task wishes an integer event code to
	 * be displayed on the console. */
	EVT_DISPLAY_CODE,

	/** An event indicating that some task wishes text to be displayed on
	 * the console. */
	EVT_DISPLAY_TEXT,

	EVT_EventCode_e
} eventCode_e;

typedef struct event_rx_chars {
	/** Multiple characters can be transferred in a single
	 * event.  The event_flags field holds the number of
	 * characters in the array. */
	char data[8];
} event_rx_chars;

typedef struct event_display {
	/** The task that is requesting the display.  This may be
	 * used to provide the task origin name in the message. */
	xTaskHandle task;
	union {
		/** EVT_DISPLAY_CODE: A code to be displayed */
		int code;
		/** EVT_DISPLAY_TEXT: Text to be displayed */
		const char * text;
	};
	/** Optional semaphore to release when the console has
	 * completed displaying the event. */
	xSemaphoreHandle notify_sema;
} event_display;

typedef struct event {
	/** The EVT_* code identifying the purpose and structure of the
	 * event itself. */
	uint8_t event_type;
	/** Flags, short counts, other event-specific data used primarily
	 * for alignment. */
	uint8_t event_flags;
	union {
		event_rx_chars rx_chars;
		event_display display;
	};
} event;

/* Gross hackery: vuprintf() doesn't permit a data parameter to its
 * putchar function, so this has to live globally.  It's reasonably
 * unlikely that multiple terminal instances will be present; when
 * somebody needs them, then this has to be fixed. */
static xBSP430uartHandle tprintf_uart;

static int
tputchar (int c)
{
	return iBSP430uartPutChar(c, tprintf_uart);
}

static int
__attribute__((__format__(printf, 1, 2)))
tprintf (const char *fmt, ...)
{
	va_list argp;
	int rc;
	
	va_start (argp, fmt);
	rc = vuprintf (tputchar, fmt, argp);
	va_end (argp);
	return rc;
}

void
vBSP430CommandAdd (xBSP430CommandGroup * commands,
				   xBSP430CommandDefn * new_command)
{
	xBSP430CommandDefn ** place = &commands->commands;
	while ((0 != *place) && (0 > strcmp((*place)->tag, new_command->tag))) {
		place = & (*place)->next;
	}
	if (*place) {
		new_command->next = *place;
	}
	*place = new_command;
}

int
vBSP430CommandProcess (xBSP430Terminal * terminal,
					   xBSP430CommandGroup * commands,
					   char * cmd_buffer)
{
	xBSP430CommandDefn * cmd = commands->commands;
	char * argp;

	while (*cmd_buffer && isspace(*cmd_buffer)) {
		++cmd_buffer;
	}
	argp = cmd_buffer;
	while (*argp && !isspace(*argp)) {
		++argp;
	}
	if (*argp) {
		*argp++ = 0;
	}
	while (cmd) {
		if (0 == strcmp(cmd->tag, cmd_buffer)) {
			cmd->function(terminal, cmd, argp);
			return 0;
		}
		cmd = cmd->next;
	}
	if (0 == strcmp("help", cmd_buffer)) {
		if (! terminal) {
			return -2;
		}
		tprintf("\r\n%s commands available:\r\n", commands->group);
		cmd = commands->commands;
		while (cmd) {
			tprintf("\t%s:\t%s\r\n", cmd->tag, cmd->help ?: "<no help available>");
			cmd = cmd->next;
		}
		return 0;
	}
	if (terminal && cmd_buffer[0]) {
		tprintf("\r\nUnrecognized command '%s'; try help\r\n", cmd_buffer);
	}
	return -1;
}

portBASE_TYPE
xBSP430TerminalDisplayText (xBSP430Terminal * terminal,
							const char * pcDisplayText,
							portTickType xTicksToWait,
							xSemaphoreHandle xNotifySemaphore)
{
	struct event evt;

	configASSERT(0 < terminal->display_buffer_length);
	evt.event_type = EVT_DISPLAY_TEXT;
	evt.event_flags = 0;
	evt.display.task = xTaskGetCurrentTaskHandle();
	evt.display.text = pcDisplayText;
	evt.display.notify_sema = xNotifySemaphore;
	if (evt.display.task == terminal->terminal_task) {
		xTicksToWait = 0;
	}
	return pdTRUE == xQueueSendToBack(terminal->event_queue, &evt, xTicksToWait);
}

portBASE_TYPE
xBSP430TerminalDisplayCode (xBSP430Terminal * terminal,
							portBASE_TYPE xCode,
							portTickType xTicksToWait,
							xSemaphoreHandle xNotifySemaphore)
{
	struct event evt;

	configASSERT(0 < terminal->display_buffer_length);
	evt.event_type = EVT_DISPLAY_CODE;
	evt.event_flags = 0;
	evt.display.task = xTaskGetCurrentTaskHandle();
	evt.display.code = xCode;
	evt.display.notify_sema = xNotifySemaphore;
	if (evt.display.task == terminal->terminal_task) {
		xTicksToWait = 0;
	}
	return pdTRUE == xQueueSendToBack(terminal->event_queue, &evt, xTicksToWait);
}

char *
pcBSP430TerminalRequestDisplay (xBSP430Terminal * terminal,
								unsigned short * pusAvailable,
								portTickType xTicksToWait)
{
	if (xTaskGetCurrentTaskHandle() == terminal->terminal_task) {
		xTicksToWait = 0;
	}
	if ((! terminal->display_buffer)
		|| (! xSemaphoreTake(terminal->display_buffer_sema, xTicksToWait))) {
		return 0;
	}
	if (pusAvailable) {
		*pusAvailable = terminal->display_buffer_length;
	}
	return terminal->display_buffer;
}

portBASE_TYPE
xBSP430TerminalReturnDisplay (xBSP430Terminal * terminal,
							  const char * pcDisplayText,
							  portTickType xTicksToWait)
{
	portBASE_TYPE rv;

	configASSERT(0 < terminal->display_buffer_length);
	rv = xBSP430TerminalDisplayText(terminal, pcDisplayText, xTicksToWait, terminal->display_buffer_sema);
	if (! rv) {
		xSemaphoreGive(terminal->display_buffer_sema);
	}
	return rv;
}

static portTASK_FUNCTION(vConsoleReader, pvParameters)
{
	const portTickType MAX_DELAY = portMAX_DELAY;
	xBSP430Terminal * terminal = (xBSP430Terminal *)pvParameters;
	xQueueHandle rx_queue = terminal->rx_queue;
	xQueueHandle event_queue = terminal->event_queue;
	struct event evt;
	
	evt.event_type = EVT_RX_CHARS;
	for (;;) {
		portTickType delay = MAX_DELAY;
		char c;

		if (xQueueReceive(rx_queue, &c, delay)) {
			evt.event_flags = 0;
			do {
				evt.rx_chars.data[evt.event_flags++] = c;
			} while ((evt.event_flags < sizeof(evt.rx_chars.data))
					 && xQueueReceive(rx_queue, &c, 0));
			delay = 0;
			while (! xQueueSendToBack(event_queue, &evt, delay)) {
				delay = MAX_DELAY;
			}
		}
	}
}

#define KEY_BS 0x08
#define KEY_LF 0x0a
#define KEY_CR 0x0d
#define KEY_BELL 0x07
#define KEY_ESC 0x1a
#define KEY_FF 0x0c
#define KEY_KILL_LINE 0x15
#define KEY_KILL_WORD 0x17

static portTASK_FUNCTION( vSerialStuff, pvParameters )
{
	xBSP430Terminal * terminal = (xBSP430Terminal *)pvParameters;
	char* cp = terminal->command_buffer;
	int need_prompt;
	const char* ecp = cp + terminal->command_buffer_length - 1;
	event evt;
	
	if (! terminal) {
		return;
	}
	configASSERT(NULL == tprintf_uart);
	tprintf_uart = terminal->hsuart;
	portENABLE_INTERRUPTS();
	need_prompt = 1;
	for(;;)
	{
		if (need_prompt) {
			if (terminal->prompt) {
				tprintf("\r%s%s", terminal->prompt, terminal->command_buffer);
			} else {
				unsigned long uptime = ulBSP430timerA0Ticks() / portACLK_FREQUENCY_HZ;
				unsigned int seconds;
				unsigned int minutes;

				seconds = uptime % 60;
				uptime /= 60;
				minutes = uptime % 60;
				uptime /= 60;
				tprintf("\r%u:%02u:%02u > %s", (unsigned int)uptime, minutes, seconds, terminal->command_buffer);
			}
			need_prompt = 0;
		}
		if (xQueueReceive(terminal->event_queue, &evt, portMAX_DELAY)) {
			if (EVT_RX_CHARS == evt.event_type) {
				int idx = 0;
				while (idx < evt.event_flags) {
					char c;
					switch ((c = evt.rx_chars.data[idx++])) {
					case KEY_BS:
						if (cp > terminal->command_buffer) {
							*--cp = 0;
							iBSP430uartPutString("\b \b", terminal->hsuart);
						} else {
							iBSP430uartPutChar(KEY_BELL, terminal->hsuart);
						}
						break;
					case KEY_CR: {
						vBSP430CommandProcess(terminal, terminal->pxCommandGroup, terminal->command_buffer);
						need_prompt = 1;
						iBSP430uartPutChar('\n', terminal->hsuart);
						cp = terminal->command_buffer;
						*cp = 0;
						break;
					}
					case KEY_FF:
						iBSP430uartPutChar('\f', terminal->hsuart);
						need_prompt = 1;
						break;
					case KEY_KILL_LINE:
						tprintf("\e[%uD\e[K", (unsigned int)(cp - terminal->command_buffer));
						cp = terminal->command_buffer;
						*cp = 0;
						break;
					case KEY_KILL_WORD: {
						char* kp = cp;
						while (--kp > terminal->command_buffer && isspace(*kp)) {
						}
						while (--kp > terminal->command_buffer && !isspace(*kp)) {
						}
						++kp;
						tprintf("\e[%uD\e[K", (unsigned int)(cp-kp));
						cp = kp;
						*cp = 0;
						break;
					}
					default:
						if (cp >= ecp) {
							iBSP430uartPutChar(KEY_BELL, terminal->hsuart);
						} else {
							*cp++ = c;
							*cp = 0;
							iBSP430uartPutChar(c, terminal->hsuart);
						}
						break;
					}
				}
			} else if ((EVT_DISPLAY_TEXT == evt.event_type)
					   || (EVT_DISPLAY_CODE == evt.event_type)) {
				tprintf("\r\n%s: ", pcTaskGetTaskName(evt.display.task));
				if ((EVT_DISPLAY_TEXT == evt.event_type)
					&& (evt.display.text != NULL)) {
					iBSP430uartPutString(evt.display.text, terminal->hsuart);
				}
				if (EVT_DISPLAY_CODE == evt.event_type) {
					tprintf("code %d", evt.display.code);
				}
				if (evt.display.notify_sema) {
					xSemaphoreGive(evt.display.notify_sema);
				}
				need_prompt = 1;
				iBSP430uartPutChar('\n', terminal->hsuart);
			}
		}
	}
}

xBSP430Terminal *
xBSP430TerminalCreate (const xBSP430TerminalConfiguration * configp)
{
	xBSP430Terminal * terminal;

	terminal = pvPortMalloc(sizeof(*terminal));
	if (! terminal) {
		goto failed;
	}
	memset(terminal, 0, sizeof(*terminal));
	terminal->pxCommandGroup = configp->pxCommandGroup;
	if (0 < configp->usCommandBufferLength) {
		terminal->command_buffer = pvPortMalloc(configp->usCommandBufferLength);
		if (! terminal->command_buffer) {
			goto failed;
		}
		terminal->rx_queue = xQueueCreate(configp->ucReceiveQueueLength, sizeof(char));
		if (! terminal->rx_queue) {
			goto failed;
		}
		terminal->command_buffer_length = configp->usCommandBufferLength;
	}
	terminal->tx_queue = xQueueCreate(configp->ucTransmitQueueLength, sizeof(char));
	if (! terminal->tx_queue) {
		goto failed;
	}
	terminal->event_queue = xQueueCreate(configp->ucEventQueueLength, sizeof(event));
	if (! terminal->event_queue) {
		goto failed;
	}
	if (0 < configp->usDisplayBufferLength) {
		terminal->display_buffer_length = configp->usDisplayBufferLength;
		vSemaphoreCreateBinary(terminal->display_buffer_sema);
		if (! terminal->display_buffer_sema) {
			goto failed;
		}
		terminal->display_buffer = pvPortMalloc(configp->usDisplayBufferLength);
		if (! terminal->display_buffer) {
			goto failed;
		}
	}
	terminal->hsuart = xBSP430uartOpen(configp->uart, configp->control_word, configp->baud, terminal->rx_queue, terminal->tx_queue);
	if (! terminal->hsuart) {
		goto failed;
	}
	if (terminal->rx_queue) {
		xTaskCreate(vConsoleReader, (signed char *) "RX",
					configBSP430_TERMINAL_READER_STACK_ADJUSTMENT + configMINIMAL_STACK_SIZE,
					terminal,
					configp->uxPriority ? configp->uxPriority-1 : 0,
					&terminal->reader_task);
		if (! terminal->reader_task) {
			goto failed;
		}
	}
	xTaskCreate(vSerialStuff, (signed char *) "TRM",
				configBSP430_TERMINAL_STACK_ADJUSTMENT + configMINIMAL_STACK_SIZE,
				terminal, configp->uxPriority, &terminal->terminal_task);
	if (! terminal->terminal_task) {
		goto failed;
	}
	return terminal;
 failed:
#if 0
#if INCLUDE_vTaskDelete - 0
	if (terminal->terminal_task) {
		vTaskDelete(terminal->terminal_task);
	}
	if (terminal->reader_task) {
		vTaskDelete(terminal->reader_task);
	}
#endif /* INCLUDE_vTaskDelete */
	if (terminal->hsuart) {
		iBSP430uartClose(terminal->hsuart);
	}
	if (terminal->display_buffer) {
		vPortFree(terminal->display_buffer);
	}
	if (terminal->display_buffer_sema) {
		vSemaphoreDelete(terminal->display_buffer_sema);
	}
	if (terminal->event_queue) {
		vQueueDelete(terminal->event_queue);
	}
	if (terminal->tx_queue) {
		vQueueDelete(terminal->tx_queue);
	}
	if (terminal->rx_queue) {
		vQueueDelete(terminal->rx_queue);
	}
	if (terminal->command_buffer) {
		vPortFree(terminal->command_buffer);
	}
	vPortFree(terminal);
#endif
	return NULL;
}
