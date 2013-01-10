/** This file is in the public domain.
 *
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/serial.h>
#include <stdio.h>

#define APP_TX_BUFFER_SIZE 128

typedef struct sConsoleTxBuffer {
  sBSP430halISRVoidChainNode cb_node;
  char buffer[APP_TX_BUFFER_SIZE];
  volatile unsigned char head;
  volatile unsigned char tail;
} sConsoleTxBuffer;

static int
console_tx_isr_ni (const struct sBSP430halISRVoidChainNode * cb,
                   void * context)
{
  sConsoleTxBuffer * bufp = (sConsoleTxBuffer *)cb;
  sBSP430halSERIAL * hal = (sBSP430halSERIAL *) context;
  unsigned char tail = bufp->tail;
  int rv = 0;

  /* If there's data available here, store it and mark that we have
   * done so. */
  if (bufp->head != tail) {
    hal->tx_byte = bufp->buffer[tail];
    rv |= BSP430_HAL_ISR_CALLBACK_BREAK_CHAIN;
    tail = (tail + 1) % (sizeof(bufp->buffer) / sizeof(*bufp->buffer));
  }
  if (bufp->head == tail) {
    rv |= BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT | BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
  }
  bufp->tail = tail;
  return rv;
}

static sConsoleTxBuffer tx_buffer_ = {
  .cb_node = { .callback = console_tx_isr_ni },
};

void main ()
{
  hBSP430halSERIAL console = NULL;
  hBSP430halSERIAL uart = NULL;
  unsigned long prep_utt = 0;
  unsigned long emit_utt = 0;
  unsigned long done_utt = 0;

  vBSP430platformInitialize_ni();
  iBSP430consoleInitialize();

  console = hBSP430console();
  cprintf("\ntxcb " __DATE__ " " __TIME__ "\n");

  cprintf("\nConsole %p is on %s: %s\n", console,
          xBSP430serialName(BSP430_CONSOLE_SERIAL_PERIPH_HANDLE),
          xBSP430platformPeripheralHelp(BSP430_CONSOLE_SERIAL_PERIPH_HANDLE, 0));

  uart = hBSP430serialLookup(UART_PERIPH_HANDLE);
  if (NULL == uart) {
    cprintf("Failed to resolve secondary\n");
    return;
  }
  cprintf("\nSecondary %p is on %s: %s\n", uart,
          xBSP430serialName(UART_PERIPH_HANDLE),
          xBSP430platformPeripheralHelp(UART_PERIPH_HANDLE, 0));

  tx_buffer_.head = tx_buffer_.tail = 0;
  BSP430_HAL_ISR_CALLBACK_LINK_NI(sBSP430halISRVoidChainNode, uart->tx_cbchain_ni, tx_buffer_.cb_node, next_ni);
  uart = hBSP430serialOpenUART(uart, 0, 0, 9600);
  if (NULL == uart) {
    cprintf("Secondary open failed\n");
  }

  /* Need to enable interrupts so timer overflow events are properly
   * acknowledged */
  BSP430_CORE_ENABLE_INTERRUPT();
  while (1) {
    unsigned long next_prep_utt;
    char * obp;
    char * ob_end;

    next_prep_utt = ulBSP430uptime();
    obp = tx_buffer_.buffer;
    ob_end = obp + sizeof(tx_buffer_.buffer);
    obp += snprintf(obp, ob_end - obp, "prep %lu emit %lu\r\n", emit_utt - prep_utt, done_utt - emit_utt);
    ob_end = obp;
    BSP430_CORE_DISABLE_INTERRUPT();
    emit_utt = ulBSP430uptime_ni();
    prep_utt = next_prep_utt;
    tx_buffer_.tail = 0;
    tx_buffer_.head = obp - tx_buffer_.buffer;
    vBSP430serialWakeupTransmit_ni(uart);
    BSP430_CORE_LPM_ENTER_NI(LPM0_bits);
    /* Expect tail == head otherwise should not have awoken */
    done_utt = ulBSP430uptime();
  }
}
