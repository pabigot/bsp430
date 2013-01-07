/* Copyright 2012-2013, Peter A. Bigot
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
 * @brief Implementation supporting the command line interface
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#include <bsp430/platform.h>
#include <bsp430/utility/cli.h>
#include <bsp430/utility/console.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static iBSP430cliDiagnosticFunction diagnosticFunction = &iBSP430cliNullDiagnostic;

void vBSP430cliSetDiagnosticFunction (iBSP430cliDiagnosticFunction diagnostic_function)
{
  if (! diagnostic_function) {
    diagnostic_function = &iBSP430cliNullDiagnostic;
  }
  diagnosticFunction = diagnostic_function;
}

#if 0 < BSP430_CLI_CONSOLE_BUFFER_SIZE
static char consoleBuffer_[BSP430_CLI_CONSOLE_BUFFER_SIZE];
static char * cbEnd_;
#endif /* BSP430_CLI_CONSOLE_BUFFER_SIZE */

const char *
xBSP430cliNextToken (const char * * commandp,
                     size_t * remainingp,
                     size_t * lenp)
{
  size_t remaining = *remainingp;
  const char * command = *commandp;
  const char * rv = command;
  const char * lp;

  /* Skip leading space up to end of input */
  while (((rv - command) < remaining)
         && isspace(*rv)) {
    ++rv;
  }
  /* Skip non-space up to end of input */
  lp = rv;
  while (((lp - command) < remaining)
         && ! isspace(*lp)) {
    ++lp;
  }
  /* Updated command string */
  *commandp = lp;
  /* Length of space skipped */
  *remainingp -= (lp - command);
  /* Length of non-space detected */
  if (NULL != lenp) {
    *lenp = lp - rv;
  }
  return rv;
}

const char *
xBSP430cliNextQToken (const char * * commandp,
                      size_t * remainingp,
                      size_t * lenp)
{
  size_t remaining = *remainingp;
  const char * command = *commandp;
  const char * rv = command;
  const char * lp;
  char squote;
  char equote;

  /* Skip leading space up to end of input */
  while (((rv - command) < remaining)
         && isspace(*rv)) {
    ++rv;
  }

  /* If nothing remains, or if the first character of what remains
   * isn't a quote, defer to base implementation. */
  if (((rv - command) >= remaining)
      || (! ((*rv == ((squote = '\'')))
             || (*rv == ((squote = '"')))))) {
    return xBSP430cliNextToken(commandp, remainingp, lenp);
  }
  equote = squote;

  /* Skip the quote and look for the end quote */
  lp = ++rv;
  while (((lp - command) < remaining)
         && (*lp != equote)) {
    ++lp;
  }

  /* If we didn't find the end quote, or the end quote is not the end
   * of the input and is followed by a non-space character, defer to
   * the base implementation */
  if (((lp - command) >= remaining)
      || (((lp + 1 - command) < remaining)
          && ! isspace(lp[1]))) {
    return xBSP430cliNextToken(commandp, remainingp, lenp);
  }
  *commandp = lp+1;
  *remainingp -= (*commandp - command);
  if (NULL != lenp) {
    *lenp = lp - rv;
  }
  return rv;
}

int
iBSP430cliMatchCommand (const sBSP430cliCommand * cmds,
                        const char * command,
                        size_t command_len,
                        const sBSP430cliCommand * * matchp,
                        sBSP430cliMatchCallback * match_callback,
                        const char * * argstrp,
                        size_t * argstr_lenp)
{
  const sBSP430cliCommand * match = NULL;
  unsigned int nmatches;
  size_t len;
  const char * key;

  key = xBSP430cliNextToken(&command, &command_len, &len);
  if (0 != argstrp) {
    *argstrp = command;
  }
  if (0 != argstr_lenp) {
    *argstr_lenp = command_len;
  }
  nmatches = 0;
  while (cmds) {
    if (0 == strncmp(key, cmds->key, len)) {
      ++nmatches;
      if (0 != match_callback) {
        match_callback->callback(match_callback, cmds);
      }
      match = cmds;
    }
    cmds = cmds->next;
  }
  if (0 == len) {
    nmatches = -nmatches;
  } else {
    if (1 < nmatches) {
      match = NULL;
    }
    if (0 != matchp) {
      *matchp = match;
    }
  }
  return nmatches;
}

static int
processSubcommand_ (sBSP430cliCommandLink * chain,
                    const sBSP430cliCommand * command_set,
                    void * param,
                    const char * command,
                    size_t command_len,
                    iBSP430cliHandlerFunction chain_handler,
                    iBSP430cliHandlerFunction handler)
{
  sBSP430cliCommandLink parent_link;
  const sBSP430cliCommand * match;
  int nmatches;
  const char * argstr;
  size_t argstr_len;

  parent_link.link = chain;
  parent_link.command_set = command_set;
  parent_link.cmd = NULL;
  nmatches = iBSP430cliMatchCommand(command_set, command, command_len, &match, 0, &argstr, &argstr_len);
  if (1 != nmatches) {
    if (NULL != handler) {
      return handler(&parent_link, param, command, command_len);
    }
    if (1 < nmatches) {
      return diagnosticFunction(&parent_link, eBSP430_CLI_ERR_MultiMatch, command, command_len);
    }
    return diagnosticFunction(&parent_link, eBSP430_CLI_ERR_Unrecognized, command, command_len);
  }
  parent_link.cmd = match;
  if (NULL != chain_handler) {
    (void)chain_handler(&parent_link, param, command, command_len);
  }
  if (match->child) {
    const char * mut_argstr = argstr;
    size_t ncommand_len = argstr_len;
    size_t len;

    (void)xBSP430cliNextToken(&mut_argstr, &ncommand_len, &len);
    if (0 < len) {
      return processSubcommand_(&parent_link, match->child, param, argstr, argstr_len, chain_handler, handler);
    }
  }
  if (NULL != handler) {
    return handler(&parent_link, param, argstr, argstr_len);
  }
  if (NULL == match->handler) {
    /* Either the user didn't provide a subcommand, or the developer
     * didn't provide the handler. */
    return diagnosticFunction(&parent_link,
                              (match->child ? eBSP430_CLI_ERR_Missing : eBSP430_CLI_ERR_Config),
                              argstr, argstr_len);
  }
  return match->handler(&parent_link, param, argstr, argstr_len);
}

int
iBSP430cliExecuteCommand (const sBSP430cliCommand * cmds,
                          void * param,
                          const char * command)
{
  return processSubcommand_(NULL, cmds, param, command, strlen(command), NULL, NULL);
}

int
iBSP430cliParseCommand (const sBSP430cliCommand * cmds,
                        void * param,
                        const char * command,
                        iBSP430cliHandlerFunction chain_handler,
                        iBSP430cliHandlerFunction handler)
{
  return processSubcommand_(NULL, cmds, param, command, strlen(command), chain_handler, handler);
}

int
iBSP430cliHandlerSimple (sBSP430cliCommandLink * chain,
                         void * param,
                         const char * argstr,
                         size_t argstr_len)
{
  if (0 == chain->cmd->param) {
    return diagnosticFunction(chain, eBSP430_CLI_ERR_Config, argstr, argstr_len);
  }
  return ((iBSP430cliSimpleHandler)chain->cmd->param)(argstr);
}

#define GEN_STORE_EXTRACTED_VALUE(tag_,type_,strtov_,maxvalstr_)        \
  int                                                                   \
  iBSP430cliStoreExtracted##tag_ (const char * * argstrp,               \
                                  size_t * argstr_lenp,                 \
                                  type_ * destp)                        \
  {                                                                     \
    char buffer[sizeof(maxvalstr_)];                                    \
    char * ep;                                                          \
    const char * argstr = *argstrp;                                     \
    size_t argstr_len = *argstr_lenp;                                   \
    const char * vstr;                                                  \
    size_t len;                                                         \
    type_ v;                                                            \
                                                                        \
    vstr = xBSP430cliNextToken(&argstr, &argstr_len, &len);             \
    if (0 == len) {                                                     \
      return -eBSP430_CLI_ERR_Missing;                                  \
    }                                                                   \
    if (sizeof(buffer)-1 < len) {                                       \
      return -eBSP430_CLI_ERR_Invalid;                                  \
    }                                                                   \
    memcpy(buffer, vstr, len);                                          \
    buffer[len] = 0;                                                    \
    v = strtov_(buffer, &ep, 0);                                        \
    if (*ep) {                                                          \
      return -eBSP430_CLI_ERR_Invalid;                                  \
    }                                                                   \
    *destp = v;                                                         \
    *argstrp = argstr;                                                  \
    *argstr_lenp = argstr_len;                                          \
    return 0;                                                           \
  }

/* The text representations with the greatest length are the
 * ones that use octal. */
GEN_STORE_EXTRACTED_VALUE(UI,unsigned int,strtoul,"0177777")
GEN_STORE_EXTRACTED_VALUE(UL,unsigned long int,strtoul,"037777777777")
GEN_STORE_EXTRACTED_VALUE(I,int,strtol,"-0100000")
GEN_STORE_EXTRACTED_VALUE(L,long int,strtol,"-020000000000")
#undef GEN_STORE_EXTRACTED_VALUE

#define GEN_STORE_VALUE_HANDLER(tag_,type_,strtov_,maxvalstr_)          \
  int                                                                   \
  iBSP430cliHandlerStore##tag_ (struct sBSP430cliCommandLink * chain,   \
                                void * param,                           \
                                const char * argstr,                    \
                                size_t argstr_len)                      \
  {                                                                     \
    const sBSP430cliCommand * cmd = chain->cmd;                         \
    int rv;                                                             \
                                                                        \
    if (NULL == cmd->param) {                                           \
      return diagnosticFunction(chain, eBSP430_CLI_ERR_Config, argstr, argstr_len); \
    }                                                                   \
    rv = iBSP430cliStoreExtracted##tag_(&argstr, &argstr_len, (type_*)cmd->param); \
    if (0 != rv) {                                                      \
      return diagnosticFunction(chain, eBSP430_CLI_ERR_Invalid, argstr, argstr_len); \
    }                                                                   \
    return 0;                                                           \
  }

/* The text representations with the greatest length are the
 * ones that use octal. */
GEN_STORE_VALUE_HANDLER(UI,unsigned int,strtoul,"0177777")
GEN_STORE_VALUE_HANDLER(UL,unsigned long int,strtoul,"037777777777")
GEN_STORE_VALUE_HANDLER(I,int,strtol,"-0100000")
GEN_STORE_VALUE_HANDLER(L,long int,strtol,"-020000000000")

#undef GEN_STORE_VALUE_HANDLER

sBSP430cliCommandLink *
xBSP430cliReverseChain (sBSP430cliCommandLink * chain)
{
  sBSP430cliCommandLink * last;

  last = NULL;
  while (chain) {
    sBSP430cliCommandLink * * linkp = &chain->link;
    sBSP430cliCommandLink * next = *linkp;
    *linkp = last;
    last = chain;
    chain = next;
  }
  return last;
}

int
iBSP430cliNullDiagnostic (sBSP430cliCommandLink * chain,
                          enum eBSP430cliErrorType errtype,
                          const char * argstr,
                          size_t argstr_len)
{
  return -(int)errtype;
}


#if BSP430_CONSOLE - 0

static void
display_cmd (sBSP430cliMatchCallback * self,
             const sBSP430cliCommand * cmd)
{
  (void)self;
  cprintf("\t%s\n", cmd->key);
}

void
vBSP430cliConsoleDisplayChain (struct sBSP430cliCommandLink * chain,
                               const char * argstr)
{

  if (chain) {
    struct sBSP430cliCommandLink * cp;
    struct sBSP430cliCommandLink * rchain;
    unsigned int nlinks = 0;
    cp = rchain = xBSP430cliReverseChain(chain);
    do {
      if (cp->cmd) {
        if (0 < nlinks++) {
          cputchar(' ');
        }
        cprintf("%s", cp->cmd->key);
      }
      cp = cp->link;
    } while (cp);
    (void)xBSP430cliReverseChain(rchain);
  }
  if (argstr[0]) {
    cprintf("(%s)", argstr);
  }
}

int
iBSP430cliConsoleDiagnostic (struct sBSP430cliCommandLink * chain,
                             enum eBSP430cliErrorType errtype,
                             const char * argstr,
                             size_t argstr_len)
{
  const sBSP430cliCommand * cmds = chain->command_set;
  switch (errtype) {
    case eBSP430_CLI_ERR_Config:
      cputtext("Command configuration error: ");
      break;
    case eBSP430_CLI_ERR_Missing:
      cputtext("Expected something after: ");
      if (chain->cmd) {
        cmds = chain->cmd->child;
      }
      break;
    case eBSP430_CLI_ERR_Unrecognized:
      cputtext("Unrecognized: ");
      break;
    case eBSP430_CLI_ERR_MultiMatch:
      cputtext("Ambiguous command: ");
      break;
    case eBSP430_CLI_ERR_Invalid:
      cputtext("Invalid value: ");
      break;
    default:
      cprintf("ERROR %u at: ", errtype);
      break;
  }
  vBSP430cliConsoleDisplayChain(chain, argstr);
  if ((0 != cmds)
      && ((eBSP430_CLI_ERR_MultiMatch == errtype)
          || (eBSP430_CLI_ERR_Missing == errtype)
          || (eBSP430_CLI_ERR_Unrecognized == errtype))) {
    sBSP430cliMatchCallback cbs;
    cprintf("\nCandidates:\n");

    /* If the diagnostic is that something's missing or unrecognized,
     * ignore whatever's there so it doesn't inappropriately filter
     * out all the real candidates. */
    if ((eBSP430_CLI_ERR_Missing == errtype)
        || (eBSP430_CLI_ERR_Unrecognized == errtype)) {
      argstr_len = 0;
    }
    cbs.callback = display_cmd;
    (void)iBSP430cliMatchCommand(cmds, argstr, argstr_len, 0, &cbs, 0, 0);
  }
  cputchar('\n');
  return -(int)errtype;
}

static void
consoleDisplayHelp_ (const sBSP430cliCommand * cmd,
                     unsigned int level)
{
  while (cmd) {
    unsigned int indent = level;
    while (indent--) {
      cprintf("  ");
    }
    if (cmd->help) {
      cprintf("%s %s\n", cmd->key, cmd->help);
    } else {
      cprintf("%s # No help available\n", cmd->key);
    }
    if (cmd->child) {
      consoleDisplayHelp_(cmd->child, level+1);
    }
    cmd = cmd->next;
  }
}

void vBSP430cliConsoleDisplayHelp (const sBSP430cliCommand * cmd)
{
  consoleDisplayHelp_(cmd, 0);
}

const char *
xBSP430cliConsoleBuffer_ni (void)
{
#if 0 < BSP430_CLI_CONSOLE_BUFFER_SIZE
  if (NULL == cbEnd_) {
    cbEnd_ = consoleBuffer_;
  }
  *cbEnd_ = 0;
  return consoleBuffer_;
#else /* BSP430_CLI_CONSOLE_BUFFER_SIZE */
  return NULL;
#endif /* BSP430_CLI_CONSOLE_BUFFER_SIZE */
}

#if 0 < BSP430_CLI_CONSOLE_BUFFER_SIZE

#define KEY_BS '\b'
#define KEY_LF '\n'
#define KEY_CR '\r'
#define KEY_BEL '\a'
#define KEY_ESC '\e'
#define KEY_FF '\f'
#define KEY_HT '\t'
#define KEY_KILL_LINE 0x15
#define KEY_KILL_WORD 0x17

void
vBSP430cliConsoleBufferClear_ni (void)
{
  cbEnd_ = NULL;
}

int
iBSP430cliConsoleBufferExtend_ni (const char * text,
                                  size_t len)
{
  const char * in_cb = cbEnd_;

  while ((0 != *text)
         && (0 < len--)
         && ((1+cbEnd_) < (consoleBuffer_ + sizeof(consoleBuffer_)))) {
    *cbEnd_++ = *text++;
  }
  return (cbEnd_ - in_cb);
}

int
iBSP430cliConsoleBufferProcessInput_ni ()
{
  int rv;
  int c;

  rv = 0;
  if (NULL == cbEnd_) {
    cbEnd_ = consoleBuffer_;
  }
  while (0 <= ((c = cgetchar_ni()))) {
    if (KEY_BS == c) {
      if (cbEnd_ == consoleBuffer_) {
        cputchar_ni(KEY_BEL);
      } else {
        --cbEnd_;
        cputtext_ni("\b \b");
      }
#if configBSP430_CLI_COMMAND_COMPLETION - 0
    } else if (KEY_HT == c) {
      rv |= eBSP430cliConsole_DO_COMPLETION;
      break;
#endif /* configBSP430_CLI_COMMAND_COMPLETION */
    } else if (KEY_ESC == c) {
      rv |= eBSP430cliConsole_PROCESS_ESCAPE;
      break;
    } else if (KEY_FF == c) {
      cputchar_ni(c);
      rv |= eBSP430cliConsole_REPAINT;
      break;
    } else if (KEY_CR == c) {
      cputchar_ni('\n');
      rv |= eBSP430cliConsole_READY;
      break;
    } else if (KEY_KILL_LINE == c) {
      cprintf("\e[%uD\e[K", (unsigned int)(cbEnd_ - consoleBuffer_));
      cbEnd_ = consoleBuffer_;
      *cbEnd_ = 0;
    } else if (KEY_KILL_WORD == c) {
      char * kp = cbEnd_;
      while (--kp > consoleBuffer_ && isspace(*kp)) {
      }
      while (--kp > consoleBuffer_ && !isspace(*kp)) {
      }
      ++kp;
      cprintf("\e[%uD\e[K", (unsigned int)(cbEnd_ - kp));
      cbEnd_ = kp;
      *cbEnd_ = 0;
    } else {
      if ((1+cbEnd_) >= (consoleBuffer_ + sizeof(consoleBuffer_))) {
        cputchar_ni(KEY_BEL);
      } else {
        *cbEnd_++ = c;
        cputchar_ni(c);
      }
    }
  }
  return rv;
}

#endif /* BSP430_CLI_CONSOLE_BUFFER_SIZE */

#if configBSP430_CLI_COMMAND_COMPLETION - 0

struct sCallbackParam {
  /** The callback record used on a match.  This must be first in the
   * structure so its address can be cast to obtain access to the
   * remaining parameters. */
  sBSP430cliMatchCallback match_callback;

  /** Data describing the state of the completion process */
  sBSP430cliCompletionData * cdp;
};

void
vBSP430cliCompletionHelperCallback (sBSP430cliCompletionData * cdp,
                                    const char * candidate)
{
  if (0 == cdp->ncandidates) {
    cdp->append_len = strlen(candidate);
  } else {
    const char * p1 = cdp->returned_candidates[0];
    const char * p2 = candidate;
    while ((*p1 == *p2)
           && (0 != *p2)
           && ((p2 - candidate) < cdp->append_len)) {
      ++p1;
      ++p2;
    }
    cdp->append_len = p2 - candidate;
  }
  if (cdp->ncandidates < cdp->max_returned_candidates) {
    cdp->returned_candidates[cdp->ncandidates] = candidate;
  }
  ++cdp->ncandidates;
}

static void
completion_match_callback (struct sBSP430cliMatchCallback * self,
                           const sBSP430cliCommand * cmd)
{
  sBSP430cliCompletionData * cdp = ((struct sCallbackParam *)self)->cdp;
  vBSP430cliCompletionHelperCallback(cdp, cmd->key);
}

static int
storeCompletionParent_ (struct sBSP430cliCommandLink * chain,
                        void * param,
                        const char * command,
                        size_t command_len)
{
  sBSP430cliCompletionData * cdp = (sBSP430cliCompletionData *)param;
  cdp->command = command;
  cdp->command_len = command_len;
  return 0;
}

static int
doCompletion_ (struct sBSP430cliCommandLink * chain,
               void * param,
               const char * argstr,
               size_t argstr_len)
{
  sBSP430cliCompletionData * cdp = (sBSP430cliCompletionData *)param;
  const sBSP430cliCommand * cmd;
  const sBSP430cliCommand * command_set;
  const char * matches[1];
  const char * remstr;
  size_t remstr_len;
  struct sCallbackParam callback_param;
  size_t key_len = 0;
  int rv = 0;

  /* Set this.  The user is free to ignore it. */
  rv |= eBSP430cliConsole_REPAINT_BEL;

  /* Reset the data fields that are return values, or are accessed
   * below and might need a consistency check */
  cdp->append = NULL;
  cdp->append_len = 0;
  cdp->ncandidates = 0;
  if (NULL == cdp->returned_candidates) {
    cdp->max_returned_candidates = 0;
  }

  command_set = chain->command_set;
  cmd = chain->cmd;
  if (NULL != cmd) {
    size_t len = strlen(cmd->key);

    /* A command was uniquely identified, but it either doesn't have
     * sub-commands, or there was no token available to compare
     * against its sub-commands.  If the input ends inside the token
     * that uniquely identified the command, then completion should
     * finish that token and add a separating space for the expected
     * following argument. */
    remstr = cdp->command;
    remstr_len = cdp->command_len;
    (void)xBSP430cliNextToken(&remstr, &remstr_len, &key_len);
    if (0 == remstr_len) {
      cdp->append = cmd->key + key_len;
      cdp->append_len = len - key_len;
      rv |= eBSP430cliConsole_COMPLETE_SPACE;
      /* Add the output fields that would normally be filled in below,
       * if we hadn't already identified the unique result. */
      if (0 < cdp->max_returned_candidates) {
        cdp->returned_candidates[0] = cmd->key;
      }
      cdp->ncandidates = 1;
      return rv;
    }
    /* Otherwise the command is as it is, and we need to look among
     * its children for the next completion. */
    command_set = cmd->child;
  }

  /* Figure out how long a token will be used to resolve the next
   * command.  This is the same calculation performed in
   * iBSP430cliMatchCommand, but we need it too. */
  remstr = argstr;
  remstr_len = argstr_len;
  (void)xBSP430cliNextToken(&remstr, &remstr_len, &key_len);

  /* If no working space for candidates was provided, assign one
   * temporarily: we need at least one in order to calculate the
   * common prefix in the callback. */
  if (NULL == cdp->returned_candidates) {
    cdp->returned_candidates = matches;
    cdp->max_returned_candidates = sizeof(matches)/sizeof(*matches);
  }

#if ((configBSP430_CLI_COMMAND_COMPLETION - 0)                  \
     && (configBSP430_CLI_COMMAND_COMPLETION_HELPER - 0))
  /* If we have a command and it has a completion helper, try that
   * first. */
  if ((NULL != cmd) && (NULL != cmd->completion_helper)) {
    cmd->completion_helper->helper(cmd->completion_helper, argstr, argstr_len, cdp);
  }
#endif /* configBSP430_CLI_COMMAND_COMPLETION_HELPER */

  /* Walk the command set, identifying matching candidates and
   * calculating their common prefix. */
  callback_param.match_callback.callback = completion_match_callback;
  callback_param.cdp = cdp;
  iBSP430cliMatchCommand(command_set, argstr, argstr_len, NULL, &callback_param.match_callback, NULL, NULL);

  if (key_len < cdp->append_len) {
    /* Whatever we were given is sufficient to identify a common
     * prefix, if not a whole command.  Append it. */
    cdp->append = cdp->returned_candidates[0] + key_len;
    cdp->append_len -= key_len;
    if (1 == cdp->ncandidates) {
      rv |= eBSP430cliConsole_COMPLETE_SPACE;
    }
  } else {
    /* There's nothing to append: either the input already has the
     * common prefix, or it's inconsistent with the available
     * commands. */
    cdp->append = NULL;
    cdp->append_len = 0;
  }

  /* If we used a local buffer for the candidates, remove it. */
  if (matches == cdp->returned_candidates) {
    cdp->returned_candidates = NULL;
    cdp->max_returned_candidates = 0;
  }
  return rv;
}

int
iBSP430cliCommandCompletion (sBSP430cliCompletionData * cdp)
{
  return iBSP430cliParseCommand(cdp->command_set, cdp, cdp->command, storeCompletionParent_, doCompletion_);
}

#if ((configBSP430_CLI_COMMAND_COMPLETION - 0)                  \
     && (configBSP430_CLI_COMMAND_COMPLETION_HELPER - 0))

void
vBSP430cliCompletionHelperStrings (const struct sBSP430cliCompletionHelper * self,
                                   const char * argstr,
                                   size_t argstr_len,
                                   struct sBSP430cliCompletionData * cdp)
{
  const sBSP430cliCompletionHelperStrings * chsp = (const sBSP430cliCompletionHelperStrings *)self;
  size_t ni;
  const char * remstr = argstr;
  size_t remstr_len = argstr_len;
  const char * key;
  size_t key_len;

  key = xBSP430cliNextToken(&remstr, &remstr_len, &key_len);
  for (ni = 0; ni < chsp->len; ++ni) {
    if ((NULL != chsp->strings[ni])
        && (0 == strncmp(key, chsp->strings[ni], key_len))) {
      vBSP430cliCompletionHelperCallback(cdp, chsp->strings[ni]);
    }
  }
}

#endif /* configBSP430_CLI_COMMAND_COMPLETION_HELPER */

const char * const *
xBSP430cliHelperStringsExtract (const struct sBSP430cliCompletionHelperStrings * chsp,
                                const char * * argstrp,
                                size_t * argstr_lenp)
{
  int ni;
  const char * remstr = *argstrp;
  size_t remstr_len = *argstr_lenp;
  size_t key_len;
  const char * key = xBSP430cliNextToken(&remstr, &remstr_len, &key_len);
  const char * const * rv = NULL;

  for (ni = 0; ni < chsp->len; ++ni) {
    const char * const * sp = chsp->strings + ni;
    if ((NULL != *sp) && (0 == strncmp(key, *sp, key_len))) {
      if (NULL == rv) {
        rv = sp;
        continue;
      }
      return NULL;
    }
  }
  if (NULL != rv) {
    *argstrp = remstr;
    *argstr_lenp = remstr_len;
  }
  return rv;
}

#if 0 < BSP430_CLI_CONSOLE_BUFFER_SIZE

int
iBSP430cliConsoleBufferCompletion (const sBSP430cliCommand * command_set,
                                   const char * * commandp)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  sBSP430cliCompletionData ccd;
  const char * matches[BSP430_CLI_CONSOLE_BUFFER_MAX_COMPLETIONS];
  int flags;

  memset(&ccd, 0, sizeof(ccd));
  if (NULL == *commandp) {
    BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
    BSP430_CORE_DISABLE_INTERRUPT();
    *commandp = xBSP430cliConsoleBuffer_ni();
    BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  }
  ccd.command = *commandp;
  ccd.command_set = command_set;
  ccd.returned_candidates = matches;
  ccd.max_returned_candidates = sizeof(matches)/sizeof(*matches);
  flags = iBSP430cliCommandCompletion(&ccd);
  if (NULL != ccd.append) {
    size_t app_len = 0;

    BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
    BSP430_CORE_DISABLE_INTERRUPT();
    do {
      if (0 < ccd.append_len) {
        app_len += ccd.append_len;
        iBSP430cliConsoleBufferExtend_ni(ccd.append, ccd.append_len);
      }
      if (flags & eBSP430cliConsole_COMPLETE_SPACE) {
        flags &= ~eBSP430cliConsole_COMPLETE_SPACE;
        iBSP430cliConsoleBufferExtend_ni(" ", 1);
        ++app_len;
      }
      *commandp = xBSP430cliConsoleBuffer_ni();
      if ((0 < app_len) && ! (flags & eBSP430cliConsole_REPAINT)) {
        cputtext_ni(*commandp + strlen(*commandp) - app_len);
      }
    } while (0);
    BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  } else {
    if (0 < ccd.ncandidates) {
      size_t ci = 0;

      cprintf("\nCandidates:");
      while ((ci < ccd.ncandidates)
             && (ci < ccd.max_returned_candidates)) {
        cprintf(" %s", ccd.returned_candidates[ci]);
        ++ci;
      }
      if (ci < ccd.ncandidates) {
        cprintf(" (+ %u more)", ccd.ncandidates - ci);
      }
      cputchar('\n');
      flags |= eBSP430cliConsole_REPAINT;
    }
  }
  return flags;
}

#endif /* BSP430_CLI_CONSOLE_BUFFER_SIZE */

#endif /* configBSP430_CLI_COMMAND_COMPLETION */

#endif /* BSP430_CONSOLE */

