/** This file is in the public domain.
 *
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/unittest.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/cli.h>
#include <string.h>

void
testNextToken (void)
{
  const char * command = "  one two ";
  const char * mcommand = command;
  size_t last_remaining;
  size_t remaining = strlen(command);
  const char * key;
  size_t len;

  last_remaining = remaining;
  key = xBSP430cliNextToken(&mcommand, &remaining, &len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(len, 3);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(key, command+2);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(last_remaining-5, remaining);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(mcommand, key + len);
  last_remaining = remaining;
  command = mcommand;
  key = xBSP430cliNextToken(&mcommand, &remaining, &len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(len, 3);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(key, command+1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(last_remaining-4, remaining);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(mcommand, key + len);
  last_remaining = remaining;
  command = mcommand;
  key = xBSP430cliNextToken(&mcommand, &remaining, &len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(len, 0);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(key, command+1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(0, remaining);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(mcommand, key + len);
}

void
testNextQToken (void)
{
  const char * command;
  const char * mcommand;
  size_t command_len;
  size_t len;
  const char * tp;

#define SET_INPUT(str_) do {                    \
    mcommand = command = str_;                  \
    command_len = strlen(str_);                 \
  } while (0)                                   \
 
  SET_INPUT("'one two'");
  tp = xBSP430cliNextQToken(&mcommand, &command_len, &len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(tp, command+1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(7,len);

  SET_INPUT("'one two");
  tp = xBSP430cliNextQToken(&mcommand, &command_len, &len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(tp, command);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(4, len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(mcommand, command+4);
  tp = xBSP430cliNextQToken(&mcommand, &command_len, &len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(tp, command+5);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(3, len);

  SET_INPUT("''");
  tp = xBSP430cliNextQToken(&mcommand, &command_len, &len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(tp, command+1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(0,len);

  SET_INPUT("\"\"");
  tp = xBSP430cliNextQToken(&mcommand, &command_len, &len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(tp, command+1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(0,len);

  SET_INPUT("'one'x two");
  tp = xBSP430cliNextQToken(&mcommand, &command_len, &len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(tp, command);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(6, len);

#undef SET_INPUT
}

void
testConsoleBufferExtend (void)
{
  const char * p;
  int rv;
  
  vBSP430cliConsoleBufferClear_ni();
  p = xBSP430cliConsoleBuffer_ni();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(0, strlen(p));
  rv = iBSP430cliConsoleBufferExtend_ni("one", 1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rv, 1);
  p = xBSP430cliConsoleBuffer_ni();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(1, strlen(p));
  BSP430_UNITTEST_ASSERT_EQUAL_ASCIIZ("o", p);
  rv = iBSP430cliConsoleBufferExtend_ni("ne", (size_t)-1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rv, 2);
  p = xBSP430cliConsoleBuffer_ni();
  BSP430_UNITTEST_ASSERT_EQUAL_ASCIIZ("one", p);
}

static int
cmd_dummy (const char * argstr)
{
  return 0;
}
#undef LAST_COMMAND
#define LAST_COMMAND NULL
#undef LAST_SUBCOMMAND
#define LAST_SUBCOMMAND NULL
static const sBSP430cliCommand dcmd_complete_common = {
  .key = "common",
  .next = LAST_SUBCOMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_dummy
};
#undef LAST_SUBCOMMAND
#define LAST_SUBCOMMAND (&dcmd_complete_common)
static const sBSP430cliCommand dcmd_complete_component = {
  .key = "component",
  .next = LAST_SUBCOMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_dummy
};
#undef LAST_SUBCOMMAND
#define LAST_SUBCOMMAND (&dcmd_complete_component)
static const sBSP430cliCommand dcmd_complete = {
  .key = "complete",
  .next = LAST_COMMAND,
  .child = LAST_SUBCOMMAND
};
#undef LAST_COMMAND
#define LAST_COMMAND (&dcmd_complete)

static const sBSP430cliCommand dcmd_other = {
  .key = "other",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_dummy
};
#undef LAST_COMMAND
#define LAST_COMMAND (&dcmd_other)

void

testCommandCompletion (void)
{
  const sBSP430cliCommand * cands[5];
  sBSP430cliCommandCompletionData ccd;
  int flags;

  //vBSP430cliConsoleDisplayHelp(LAST_COMMAND);

  ccd.command_set = LAST_COMMAND;
  ccd.returned_candidates = cands;
  ccd.max_returned_candidates = sizeof(cands)/sizeof(*cands);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(5, ccd.max_returned_candidates);

  ccd.command = "";
  flags = iBSP430cliCommandCompletion(&ccd);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(eBSP430cliConsole_REPAINT_BEL, flags);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, ccd.append);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(2, ccd.ncandidates);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(LAST_COMMAND, ccd.returned_candidates[0]);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(LAST_COMMAND->next, ccd.returned_candidates[1]);
  
  ccd.command = "c"; /* + "omplete " */
  flags = iBSP430cliCommandCompletion(&ccd);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(eBSP430cliConsole_REPAINT_BEL|eBSP430cliConsole_COMPLETE_SPACE, flags);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(1, ccd.ncandidates);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(dcmd_complete.key+1, ccd.append);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(7, ccd.append_len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(&dcmd_complete, ccd.returned_candidates[0]);
  
  ccd.command = "complete"; /* + " " */
  flags = iBSP430cliCommandCompletion(&ccd);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(eBSP430cliConsole_REPAINT_BEL|eBSP430cliConsole_COMPLETE_SPACE, flags);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(1, ccd.ncandidates);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(dcmd_complete.key+8, ccd.append);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(0, ccd.append_len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(&dcmd_complete, ccd.returned_candidates[0]);

  ccd.command = "complete "; /* + "com" */
  flags = iBSP430cliCommandCompletion(&ccd);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(eBSP430cliConsole_REPAINT_BEL, flags);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(2, ccd.ncandidates);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(dcmd_complete.child->key, ccd.append);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(3, ccd.append_len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(dcmd_complete.child, ccd.returned_candidates[0]);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(dcmd_complete.child->next, ccd.returned_candidates[1]);

  ccd.command = "complete com"; /* candidates, no completion */
  flags = iBSP430cliCommandCompletion(&ccd);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(eBSP430cliConsole_REPAINT_BEL, flags);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(2, ccd.ncandidates);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, ccd.append);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(dcmd_complete.child, ccd.returned_candidates[0]);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(dcmd_complete.child->next, ccd.returned_candidates[1]);

  ccd.command = "complete comp"; /* + "onent " */
  flags = iBSP430cliCommandCompletion(&ccd);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(eBSP430cliConsole_REPAINT_BEL|eBSP430cliConsole_COMPLETE_SPACE, flags);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(1, ccd.ncandidates);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(dcmd_complete_component.key + 4, ccd.append);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(5, ccd.append_len);
}


void main (void)
{
  vBSP430platformInitialize_ni();
  vBSP430unittestInitialize();

  testNextToken();
  testNextQToken();
  testConsoleBufferExtend();
  testCommandCompletion();

  vBSP430unittestFinalize();
}

