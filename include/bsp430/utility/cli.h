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
 * @brief Basic support for command line processing
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_CLI_H
#define BSP430_UTILITY_CLI_H

#include <bsp430/core.h>

/** Get the next token in the command string.
 *
 * @param command pointer into an immutable buffer containing a
 * sequence of whitespace-separated tokens.
 *
 * @param remainingp pointer to the length of the command.  On input
 * this is the number of valid characters beginning at @a command.  On
 * return the value is updated to reflect the number of valid
 * characters beginning at the pointer that is returned.
 *
 * @param lenp pointer to the length of the returned token.  On return
 * the value is set to the number of valid non-space characters
 * beginning at the returned pointer.
 *
 * @return A pointer to the first non-space character in @a command.
 * If there are no such characters, the pointer is just past the end
 * of @a command and @a *remainingp is updated to have a value of
 * zero. */
const char * xBSP430cliNextToken (const char * command,
                                  size_t * remainingp,
                                  size_t * lenp);

/* Forward declarations */
struct sBSP430cliCommand;
struct sBSP430cliCommandLink;

/** Link in a command chain.
 *
 * When a command is executed from a string representation, each
 * initial token that can be uniquely identified as a command is
 * extracted, and the corresponding handler invoked on the remainder
 * of the command string.  When the handler is
 * iBSP430cliHandlerExecuteSubcommand(), the interpreter creates a
 * link structure on the stack referencing the command definition and
 * recurses.  These link structures create a chain that can be used by
 * the final operation to extract information from higher layers of
 * the command.
 */
typedef struct sBSP430cliCommandLink {
  /** The command definition to be applied */
  const struct sBSP430cliCommand * cmd;

  /** A link to the parent command definition for layered commands. */
  struct sBSP430cliCommandLink * link;
} sBSP430cliCommandLink;

/** Type for a function that implements a command.
 *
 * @param cmd a pointer to the command structure that is being executed
 *
 * @param argstr any additional arguments remaining unprocessed after
 * reaching this command
 *
 * @param argstr_len the length of @a argstr in characters.
 *
 * @return a context-specific value. */
typedef int (* iBSP430cliHandlerFunction) (struct sBSP430cliCommandLink * chain,
                                          void * param,
                                          const char * argstr,
                                          size_t argstr_len);

/** The definition of a command */
typedef struct sBSP430cliCommand {
  /** The token used to match this command.
   *
   * No key may be a prefix of the key of any sibling command.  An
   * incomplete command may thus be executed if it is a prefix of
   * exactly one key in the sibling commands acceptable at that point
   * in the parsed command string. */
  const char * key;

  /** A description of the command.
   *
   * The expected format of this string is an abstract representation
   * of whatever would follow it; e.g. for a "set" command something
   * like "[variable] [value]" would be appropriate.
   * 
   * To save space where diagnostics and interactive help are
   * unnecessary, this may be a null pointer. */
  const char * help;

  /** Subordinate command structures.
   *
   * This is an entrypoint to subordinate commands, e.g. further
   * keywords that must be matched against argstr.  This field is
   * normally processed by iBSP430cliHandlerExecuteSubCommand(). */
  const struct sBSP430cliCommand * const child;

  /** The next command at this level.
   *
   * When looking for a match, if @a key does not match processing
   * continues with the next command in this chain. */
  const struct sBSP430cliCommand * const next;

  /** The handler that implements this command.  Under normal
   * circumstances this must not be null. */
  iBSP430cliHandlerFunction const handler;

  /** A parameter providing additional information required for the
   * command.  The value is generally used by
   * sBSP430cliCommand::handler in this or a parent command structure.
   * For example, iBSP430cliHandlerStoreUI() extracts the address of
   * the variable into which the value should be stored from this
   * field. */
  void * const param;
} sBSP430cliCommand;

/** Type of a function that does something with a command structure.
 *
 * For example, this is used in uiBSP430cliMatchCommand() to inform
 * the user application of potential matches for the purpose of
 * diagnostics or command completion.
 *
 * @param cmd the command definition of interest */
typedef void (* vBSP430cliMatchCallback) (const sBSP430cliCommand * cmd);


/** A utility function to extract the next command in the parsed string.
 *
 * @param cmds the first in a sequence of sibling commands that may
 * appear at the beginning of the remainder of the command string.
 *
 * @param command the unprocessed remainder of the command string
 *
 * @param command_len the number of characters in @a command
 *
 * @param matchp an optional pointer to where the uniquely identified
 * next command should be stored.  If @matchp is null, no action is
 * taken; otherwise if a unique command could be identified the
 * pointer to that command is stored in @c *matchp; otherwise a null
 * pointer is stored in @a *matchp.
 *
 * @param match_cb an optional callback that is invoked on each
 * matching command.  If null, no action is taken; otherwise when the
 * key of a command in @a cmds has the first token of @a command as a
 * prefix @a match_cb() is invoked on that command.  The primary use
 * of this parameter is to determine the potential matching commands
 * for the purposes of diagnostics or command completion.
 *
 * @param argstrp if null or if no unique match is identified no
 * action is taken, otherwise the pointer to the remainder of @a
 * command after the initial token has been removed is stored in @a
 * *argstrp.
 *
 * @param argstr_lenp if null or if no unique match is identified no
 * action is taken, otherwise the length of the string (that would be)
 * stored in @a *argstrp is stored in @a *argstr_lenp.
 *
 * @return the number of commands in @a cmds for which the first token
 * of @a command was a prefix.
 */
unsigned int
uiBSP430cliMatchCommand (const sBSP430cliCommand * cmds,
                         const char * command,
                         size_t command_len,
                         const sBSP430cliCommand * * matchp,
                         vBSP430cliMatchCallback match_cb,
                         const char * * argstrp,
                         size_t * argstr_lenp);

/** Entrypoint to command parsing.
 *
 * @param cmds the first in a sequence of sibling commands that may
 * appear at the beginning of the parsed command string.
 *
 * @param param A user-provided parameter passed to the handler of the
 * recognized command.
 *
 * @param command A text representation of a command, consisting of
 * whitespace-separated tokens.  The first token is extracted and used
 * to identify a unique command within the siblings of @a cmds.  If a
 * unique command is identified, its sBSP430cliCommand::handler
 * function is invoked, providing a #sBSP430cliCommandLink reference
 * identifying the command itself, the provided @a param argument, and
 * the remainder of the @a command string after having stripped off
 * the initial token.
 *
 * @param command_len the length of @a command in characters.
 *
 * @return a negative error code if a unique command is not
 * identifiable from the first token in @a command, otherwise the
 * value returned by the sBSP430cliCommand::handler.
 */
int iBSP430cliExecuteCommand (const sBSP430cliCommand * cmds,
                              void * param,
                              const char * command,
                              size_t command_len);

/** Handler to execute subcommands.
 *
 * See iBSP430cliHandlerFunction() and iBSP430cliExecuteCommand().
 *
 * @param chain Pointer to the end of the command chain.  @link
 * sBSP430cliCommand::param @a chain->cmd->child @endlink is expected
 * to be non-null, and represents the first in a sequence of sibling
 * commands that may appear next in the parsed command string.
 *
 * @param param Caller-provided value, passed to the handler of the
 * selected sub-command.
 *
 * @param argstr text representation of the remainder of the command
 * string.  The first token is extracted and used to identify the
 * sub-command to execute.  If the token fails to identify a unique
 * subcommand, the diagnostic routine is invoked and the parsing
 * fails.
 *
 * @param argstr_len length of the @a argstr text.
 *
 * @return a negative error code if a unique subcommand is not
 * recognized, otherwise the return value after invoking the handler
 * of the subcommand on the remainder of the command string. */
int iBSP430cliHandlerExecuteSubcommand (sBSP430cliCommandLink * chain,
                                        void * param,
                                        const char * command,
                                        size_t command_len);

/** Handler to store a signed 16-bit integer expressed in text.
 *
 * See iBSP430cliHandlerFunction().
 *
 * @param chain Pointer to the end of the command chain.  @link
 * sBSP430cliCommand::param @a chain->cmd->param @endlink is expected
 * to be a <c>int *</c> value.
 *
 * @param param unused
 *
 * @param argstr text representation of a signed 16-bit integer,
 * normally decimal but optionally in hexadecimal (with leading @c 0x)
 * or octal (with leading @c 0).
 * 
 * @param argstr_len length of the @a argstr text */
int iBSP430cliHandlerStoreI (struct sBSP430cliCommandLink * chain,
                             void * param,
                             const char * argstr,
                             size_t argstr_len);

/** Handler to store an unsigned 16-bit integer expressed in text.
 *
 * See iBSP430cliHandlerFunction().
 *
 * @param chain Pointer to the end of the command chain.  @link
 * sBSP430cliCommand::param @a chain->cmd->param @endlink is expected to be a
 * <c>unsigned int *</c> value.
 *
 * @param param unused
 *
 * @param argstr text representation of an unsigned 16-bit integer,
 * normally decimal but optionally in hexadecimal (with leading @c 0x)
 * or octal (with leading @c 0).
 * 
 * @param argstr_len length of the @a argstr text */
int iBSP430cliHandlerStoreUI (struct sBSP430cliCommandLink * chain,
                              void * param,
                              const char * argstr,
                              size_t argstr_len);

/** Handler to store a signed 32-bit integer expressed in text.
 *
 * See iBSP430cliHandlerFunction().
 *
 * @param chain Pointer to the end of the command chain.  @link
 * sBSP430cliCommand::param @a chain->cmd->param @endlink is expected to
 * be a <c>long *</c> value.
 *
 * @param param unused
 *
 * @param argstr text representation of a signed 32-bit integer,
 * normally decimal but optionally in hexadecimal (with leading @c 0x)
 * or octal (with leading @c 0).
 * 
 * @param argstr_len length of the @a argstr text */
int iBSP430cliHandlerStoreL (struct sBSP430cliCommandLink * chain,
                             void * param,
                             const char * argstr,
                             size_t argstr_len);

/** Handler to store an unsigned 32-bit integer expressed in text.
 *
 * See iBSP430cliHandlerFunction().
 *
 * @param chain Pointer to the end of the command chain.  @link
 * sBSP430cliCommand::param @a chain->cmd->param @endlink is expected to
 * be a <c>unsigned long *</c> value.
 *
 * @param param unused
 *
 * @param argstr text representation of an unsigned 32-bit integer,
 * normally decimal but optionally in hexadecimal (with leading @c 0x)
 * or octal (with leading @c 0).
 * 
 * @param argstr_len length of the @a argstr text */
int iBSP430cliHandlerStoreUL (struct sBSP430cliCommandLink * chain,
                              void * param,
                              const char * argstr,
                              size_t argstr_len);

/** A set of values identifying errors that are detected by the
 * command parsing infrastructure.  Generally an error detected in
 * parsing is reflected by a negative return value, where the positive
 * value is the integral value of one of these enumeration tags.  */
enum eBSP430cliErrorType {
  /** An unused tag with value zero. */
  eBSP430_CLI_ERR_NONE,

  /** A mis-configured command structure has been found.  Examples
   * include a handler that needs to store a value extracted from the
   * command but was not told where to place it. */
  eBSP430_CLI_ERR_Config,

  /** Returned when a (sub-)command is required, but the command
   * string has no more tokens. */
  eBSP430_CLI_ERR_Missing,

  /** Returned when a token in the command string is required to
   * identify a (sub-)command, but is not a prefix of any acceptable
   * command. */
  eBSP430_CLI_ERR_Unrecognized,

  /** Returned when a token does not uniquely identify a command among
   * the set of acceptable commands. */
  eBSP430_CLI_ERR_MultiMatch,

  /** A sentinal value strictly greater than any valid enumeration
   * tag */
  eBSP430_CLIERRORTYPES,
};

/** Type for a function that reports errors processing commands.
 *
 * The command processing infrastructure can be customized to diagnose
 * errors by implementing a function meeting this interface and
 * registering it with the infrastructure through
 * vBSP430cliSetDiagnosticFunction().
 * 
 * @param chain the chain of command links that did not produce an error
 * @param errtype the type of error discovered
 * @param argstr the remainder of the command string at the point of error
 * @param argstr_len the length of @a argstr
 * @return the value of <c>-(int)errtype</c> */
typedef int (* iBSP430cliDiagnosticFunction) (struct sBSP430cliCommandLink * chain,
                                              enum eBSP430cliErrorType errtype,
                                              const char * argstr,
                                              size_t argstr_len);

/** Register a diagnostic function.
 *
 * The command processing infrastructure will detect parsing errors
 * and return an error code to the user application, but the context
 * of those errors will have been lost.  Application code may register
 * a function like iBSP430cliConsoleDiagnostic() which will display
 * useful information to allow the user to correct the command and
 * resubmit it.
 *
 * @param diagnostic_function the function to be registered.  If a
 * null pointer is provided, #iBSP430cliNullDiagnostic will be
 * used. */
void vBSP430cliSetDiagnosticFunction (iBSP430cliDiagnosticFunction diagnostic_function);

/** A diagnostic function that returns the error code.
 *
 * @param chain ignored
 * @param errtype the type of error discovered
 * @param argstr ignored
 * @param argstr_len ignored
 * @return the value of <c>-(int)errtype</c> */
int iBSP430cliNullDiagnostic (struct sBSP430cliCommandLink * chain,
                              enum eBSP430cliErrorType errtype,
                              const char * argstr,
                              size_t argstr_len);

/** A diagnostic function that displays on the console.
 *
 * @param chain the chain of command links that did not produce an error
 * @param errtype the type of error discovered
 * @param argstr the remainder of the command string at the point of error
 * @param argstr_len the length of @a argstr
 * @return the value of <c>-(int)errtype</c>
 *
 * @dependency #BSP430_CONSOLE */
#if defined(BSP430_DOXYGEN) || (BSP430_CONSOLE - 0)
int iBSP430cliConsoleDiagnostic (struct sBSP430cliCommandLink * chain,
                                 enum eBSP430cliErrorType errtype,
                                 const char * argstr,
                                 size_t argstr_len);
#endif /* configBSP430_CONSOLE */

/** Display the reconstructed command string on the console.
 *
 * This function is usually invoked in the context of diagnostics to
 * indicate where in the input an error was detected.  It prints on
 * the console the sequence of prefix commands encoded within @a
 * chain, then displays the remaining argument string.  (Note that to
 * do this it reverses the chain, then restores it to its original
 * direction before returning.)
 *
 * @param chain the chain of commands from the innermost to the outermost
 *
 * @param argstr the unprocessed remainder of the original command string
 */
#if defined(BSP430_DOXYGEN) || (BSP430_CONSOLE - 0)
void vBSP430cliConsoleDisplayChain (struct sBSP430cliCommandLink * chain,
                                    const char * argstr);
#endif /* configBSP430_CONSOLE */


/** Display the deep command structure on the console.
 *
 * Nested commands are displayed indented from their parent command.
 *
 * @param cmd pointer to the first in a sequence of sibling commands
 *
 * @dependency #BSP430_CONSOLE */
#if defined(BSP430_DOXYGEN) || (BSP430_CONSOLE - 0)
void vBSP430cliConsoleDisplayHelp (const sBSP430cliCommand * cmd);
#endif /* configBSP430_CONSOLE */

/** Reverse a command chain.
 *
 * The chain constructed during command parsing leads from the deepest
 * command to the outermost command.  In some situations, such as
 * displaying a diagnostic or the canonical representation of the
 * command, it is helpful to walk it in outer-to-inner order.
 *
 * Unless the caller knows the chain is not to be consulted again, it
 * is advisable to re-invoke this function after the forward walk, so
 * the links are restored to their original direction.
 *
 * @param chain A chain from the end of a command up to its beginning.
 *
 * @return The reversed chain */
sBSP430cliCommandLink * xBSP430cliReverseChain (sBSP430cliCommandLink * chain);


#endif /* BSP430_UTILITY_CLI_H */
