/* Copyright (c) 2012, Peter A. Bigot
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
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
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

const char *
xBSP430cliNextToken (const char * command,
                     size_t * remainingp,
                     size_t * lenp)
{
  size_t remaining = *remainingp;
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
  /* Length of space skipped */
  *remainingp -= (rv - command);
  /* Length of non-space detected */
  *lenp = lp - rv;
  return rv;
}

unsigned int
uiBSP430cliMatchCommand (const sBSP430cliCommand * cmds,
                         const char * command,
                         size_t command_len,
                         const sBSP430cliCommand * * matchp,
                         vBSP430cliMatchCallback match_cb,
                         const char * * argstrp,
                         size_t * argstr_lenp)
{
  const sBSP430cliCommand * match = NULL;
  unsigned int nmatches;
  size_t remaining;
  size_t len;
  const char * key;

  remaining = command_len;
  key = xBSP430cliNextToken(command, &remaining, &len);
  if (0 == len) {
    return 0;
  }
  if (0 != argstrp) {
    *argstrp = key + len;
  }
  if (0 != argstr_lenp) {
    *argstr_lenp = remaining - len;
  }
  nmatches = 0;
  while (cmds) {
    if (0 == strncmp(key, cmds->key, len)) {
      ++nmatches;
      if (0 != match_cb) {
        match_cb(cmds);
      }
      match = cmds;
    }
    cmds = cmds->next;
  }
  
  if (1 < nmatches) {
    match = NULL;
  }
  if (0 != matchp) {
    *matchp = match;
  }
  return nmatches;
}

static int
executeSubcommand_ (sBSP430cliCommandLink * chain,
                    const sBSP430cliCommand * cmds,
                    void * param,
                    const char * command,
                    size_t command_len)
{
  sBSP430cliCommandLink parent_link;
  const sBSP430cliCommand * match;
  unsigned int nmatches;
  const char * argstr;
  size_t argstr_len;

  parent_link.link = chain;
  nmatches = uiBSP430cliMatchCommand(cmds, command, command_len, &match, 0, &argstr, &argstr_len);
  if (1 != nmatches) {
    if (1 < nmatches) {
      return diagnosticFunction(chain, eBSP430_CLI_ERR_MultiMatch, command, command_len);
    }
    return diagnosticFunction(chain, eBSP430_CLI_ERR_Missing, command, command_len);
  }
  parent_link.cmd = match;
  if (NULL == match->handler) {
    return diagnosticFunction(&parent_link, eBSP430_CLI_ERR_Config, argstr, argstr_len);
  }
  return match->handler(&parent_link, param, argstr, argstr_len);
}

int
iBSP430cliExecuteCommand (const sBSP430cliCommand * cmds,
                          void * param,
                          const char * command,
                          size_t command_len)
{
  return executeSubcommand_(NULL, cmds, param, command, command_len);
}

int
iBSP430cliHandlerExecuteSubcommand (sBSP430cliCommandLink * chain,
                                    void * param,
                                    const char * command,
                                    size_t command_len)
{
  const sBSP430cliCommand * cmds;

  cmds = chain->cmd->child;
  if (0 == cmds) {
    return diagnosticFunction(chain, eBSP430_CLI_ERR_Config, command, command_len);
  }
  return executeSubcommand_(chain, cmds, param, command, command_len);
}

#define GEN_STORE_VALUE_HANDLER(tag_,type_,strtov_,maxvalstr_)          \
  int                                                                   \
  iBSP430cliHandlerStore##tag_ (struct sBSP430cliCommandLink * chain,   \
                                void * param,                           \
                                const char * argstr,                    \
                                size_t argstr_len)                      \
  {                                                                     \
    const sBSP430cliCommand * cmd = chain->cmd;                         \
    char buffer[sizeof(maxvalstr_)];                                    \
    char * ep;                                                          \
    const char * vstr;                                                  \
    size_t remaining;                                                   \
    size_t len;                                                         \
    type_ v;                                                            \
                                                                        \
    if (NULL == cmd->param) {                                           \
      return diagnosticFunction(chain, eBSP430_CLI_ERR_Config, argstr, argstr_len); \
    }                                                                   \
    remaining = argstr_len;                                             \
    vstr = xBSP430cliNextToken(argstr, &remaining, &len);               \
    if ((0 == len) || (sizeof(buffer)-1 < len)) {                       \
      return diagnosticFunction(chain, eBSP430_CLI_ERR_Unrecognized, argstr, argstr_len); \
    }                                                                   \
    memcpy(buffer, vstr, len);                                          \
    buffer[len] = 0;                                                    \
    v = strtov_(buffer, &ep, 0);                                        \
    if (*ep) {                                                          \
      return diagnosticFunction(chain, eBSP430_CLI_ERR_Unrecognized, argstr, argstr_len); \
    }                                                                   \
    *(type_ *)cmd->param = v;                                           \
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
iBSP430cliNullDiagnostic (struct sBSP430cliCommandLink * chain,
                          enum eBSP430cliErrorType errtype,
                          const char * argstr,
                          size_t argstr_len)
{
  return -(int)errtype;
}


#if BSP430_CONSOLE - 0

static void
display_cmd (const sBSP430cliCommand * cmd)
{
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
      if (0 < nlinks++) {
        cputchar_ni(' ');
      }
      cprintf("%s", cp->cmd->key);
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
  if (eBSP430_CLI_ERR_MultiMatch == errtype) {
    cprintf("Ambiguous command: ");
    vBSP430cliConsoleDisplayChain(chain, argstr);
    cprintf("\nCandidates:\n");
    (void)uiBSP430cliMatchCommand(chain->cmd->child, argstr, argstr_len, 0, display_cmd, 0, 0);
  } else {
    cprintf("ERROR %u at: ", errtype);
    vBSP430cliConsoleDisplayChain(chain, argstr);
    cprintf("\n");
  }
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

#endif /* BSP430_CONSOLE */

