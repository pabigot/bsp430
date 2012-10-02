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
 * This module provides two major features:
 *
 * @li Support for structures to register and search among available
 * commands.  The core structure is #sBSP430cliCommandLink.
 * iBSP430cliExecuteCommand() processes user input and invokes the
 * specific function.
 *
 * @li A simple command line editor using the @link
 * bsp430/utility/console.h console interface @endlink to collect user
 * input before processing it.
 *
 * Additional functions convert user input values to internal format
 * (e.g. integer or string values).
 *
 * See @ref ex_utility_cli for a detailed example demonstrating most
 * capabilities of this module.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_CLI_H
#define BSP430_UTILITY_CLI_H

#include <bsp430/core.h>

/** Get the next token in the command string.
 *
 * @param commandp pointer to a pointer into an immutable buffer
 * containing a sequence of whitespace-separated tokens.  On return it
 * is updated to point to the first whitespace character following the
 * recognized token.
 *
 * @param remainingp pointer to the length of the sequence beginning
 * at @p *commandp.  On return the value has been updated to account
 * for any adjustment to @p *commandp.
 *
 * @param lenp pointer to the length of the returned token.  On return
 * the value is set to the number of valid characters in the token
 * beginning at the returned pointer.
 *
 * @return A pointer to the first character of the first token in @p
 * *command.  If there are no such characters, the pointer is just
 * past the end of @p *commandp and both @p *remainingp and @p *lenp
 * are updated to have value zero. */
const char * xBSP430cliNextToken (const char * * commandp,
                                  size_t * remainingp,
                                  size_t * lenp);

/** Get the next token or quoted token in the command string.
 *
 * Exactly the same as xBSP430cliNextToken() except that if the first
 * non-whitespace character is a single or double quote, all
 * characters to a following single or double quote are taken as the
 * value to be returned.  The opening and closing quotes are outside
 * the returned token.
 *
 * The function delegates to xBSP430cliNextToken() on these conditions:
 * @li the first character is not a quote;
 * @li if there is no matching end quote;
 * @li if the matching end quote is followed by a non-space character
 *
 * @param commandp as with xBSP430cliNextToken()
 *
 * @param remainingp as with xBSP430cliNextToken()
 *
 * @param lenp as with xBSP430cliNextToken().  If a quoted token is
 * identified, the length excludes the opening and closing quotes.
 *
 * @return as with xBSP430cliNextToken().  If a quoted token is
 * identified, the return value begins after the opening quote.
 */
const char * xBSP430cliNextQToken (const char * * commandp,
                                   size_t * remainingp,
                                   size_t * lenp);

/* Forward declarations */
struct sBSP430cliCommand;
struct sBSP430cliCommandLink;

/** Link in a command chain.
 *
 * When a command is executed from a string representation, each
 * initial token that can be uniquely identified as a command is
 * extracted.  If the identified command definition has child commands
 * and the command string has additional tokens, the interpreter
 * creates a link structure on the stack referencing the command
 * definition and recurses on the remainder of the command string.
 * These link structures create a chain that can be used by the final
 * operation to extract information from higher layers of the command.
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
   * keywords that must be matched against argstr.  When a command
   * with a non-null @a child field is recognized and the remainder of
   * the command string has a non-empty token, processing
   * automatically continues with an attempt to identify a child that
   * can process the remainder. */
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
 * The goal here is to extract a non-empty token from the beginning of
 * the provided command (after any leading whitespace), and match it
 * as a prefix of a uniquely identified command in @a cmds.  If this
 * succeeds, we can go on to process the remainder of the command
 * line, which is returned.
 *
 * @param cmds the first in a sequence of sibling commands that may
 * appear at the beginning of the remainder of the command string.
 * This must not be a null pointer.
 *
 * @param command the unprocessed remainder of the command string.  If
 * there is no non-empty token in what remains, @a match_cb (if
 * provided) is invoked on all commands in @a cmds, and the number of
 * available commands is returned as a negative number.
 *
 * @param command_len the number of characters in @a command, possibly
 * including leading and trailing whitespace.
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
 * of @a command was a prefix, or -1 if @a command is empty
 * (disregarding whitespace).
 */
int iBSP430cliMatchCommand (const sBSP430cliCommand * cmds,
                            const char * command,
                            size_t command_len,
                            const sBSP430cliCommand * * matchp,
                            vBSP430cliMatchCallback match_cb,
                            const char * * argstrp,
                            size_t * argstr_lenp);

/** Entrypoint to command execution.
 *
 * @param cmds the first in a sequence of sibling commands that may
 * appear at the beginning of the parsed command string.
 *
 * @param param A user-provided parameter passed to the handler of the
 * recognized command.
 *
 * @param command A nul-terminated text representation of a command
 * comprising whitespace-separated tokens.  The first token is
 * extracted and used to identify a unique command within the siblings
 * of @a cmds.  If a unique command is identified, its
 * sBSP430cliCommand::handler function is invoked, providing a
 * #sBSP430cliCommandLink reference identifying the command itself,
 * the provided @a param argument, and the remainder of the @a command
 * string after having stripped off the initial token.
 *
 * @return a negative error code if a unique command is not
 * identifiable from the first token in @a command, otherwise the
 * value returned by the sBSP430cliCommand::handler.
 */
int iBSP430cliExecuteCommand (const sBSP430cliCommand * cmds,
                              void * param,
                              const char * command);

/** Entrypoint to command parsing.
 *
 * This is like iBSP430cliExecuteCommand(), except that instead of
 * executing the handlers in the command definitions, the chain of
 * parent commands is constructed and when no further subcommands are
 * identified the caller-provided function is invoked on the results.
 *
 * @param cmds as with iBSP430cliExecuteCommand()
 *
 * @param param as with iBSP430cliExecuteCommand()
 *
 * @param command as with iBSP430cliExecuteCommand()
 *
 * @param handler the handler function to execute on the resolved
 * command chain and remaining arguments
 *
 * @return the value returned by the handler function */
int
iBSP430cliParseCommand (const sBSP430cliCommand * cmds,
                        void * param,
                        const char * command,
                        iBSP430cliHandlerFunction handler);

/** Utility to extract and store a signed 16-bit integer expressed in
 * text.
 *
 * @param argstrp pointer to a pointer to the text representation of a
 * signed 16-bit integer, normally decimal but optionally in
 * hexadecimal (with leading @c 0x) or octal (with leading @c 0).  On
 * success @a *argstrp is updated to point past the consumed integer
 * token.
 *
 * @param argstr_lenp pointer to the length of the @a *argstrp text.
 * On success the @a *argstr_lenp is updated to hold
 * the remaining length of the string.
 *
 * @param destp pointer to where the extracted and converted value
 * should be stored.
 *
 * @return 0 if a valid value can be extracted and converted from the
 * head of @*argstr; a negative value if the string has no token or
 * the conversion was unsuccessful. */
int iBSP430cliStoreExtractedI (const char * * argstrp,
                               size_t * argstr_lenp,
                               int * destp);

/** Utility to extract and store an unsigned 16-bit integer expressed in
 * text.
 *
 * @param argstrp pointer to a pointer to the text representation of a
 * signed 16-bit integer, normally decimal but optionally in
 * hexadecimal (with leading @c 0x) or octal (with leading @c 0).  On
 * success @a *argstrp is updated to point past the consumed integer
 * token.
 *
 * @param argstr_lenp pointer to the length of the @a *argstrp text.
 * On success the @a *argstr_lenp is updated to hold
 * the remaining length of the string.
 *
 * @param destp pointer to where the extracted and converted value
 * should be stored.
 *
 * @return 0 if a valid value can be extracted and converted from the
 * head of @*argstr; a negative value if the string has no token or
 * the conversion was unsuccessful. */
int iBSP430cliStoreExtractedUI (const char * * argstrp,
                                size_t * argstr_lenp,
                                unsigned int * destp);

/** Utility to extract and store a signed 32-bit integer expressed in
 * text.
 *
 * @param argstrp pointer to a pointer to the text representation of a
 * signed 32-bit integer, normally decimal but optionally in
 * hexadecimal (with leading @c 0x) or octal (with leading @c 0).  On
 * success @a *argstrp is updated to point past the consumed integer
 * token.
 *
 * @param argstr_lenp pointer to the length of the @a *argstrp text.
 * On success the @a *argstr_lenp is updated to hold
 * the remaining length of the string.
 *
 * @param destp pointer to where the extracted and converted value
 * should be stored.
 *
 * @return 0 if a valid value can be extracted and converted from the
 * head of @*argstr; a negative value if the string has no token or
 * the conversion was unsuccessful. */
int iBSP430cliStoreExtractedL (const char * * argstrp,
                               size_t * argstr_lenp,
                               long * destp);

/** Utility to extract and store an unsigned 32-bit integer expressed
 * in text.
 *
 * @param argstrp pointer to a pointer to the text representation of an
 * unsigned 32-bit integer, normally decimal but optionally in
 * hexadecimal (with leading @c 0x) or octal (with leading @c 0).  On
 * success @a *argstrp is updated to point past the consumed integer
 * token.
 *
 * @param argstr_lenp pointer to the length of the @a *argstrp text.
 * On success the @a *argstr_lenp is updated to hold
 * the remaining length of the string.
 *
 * @param destp pointer to where the extracted and converted value
 * should be stored.
 *
 * @return 0 if a valid value can be extracted and converted from the
 * head of @*argstr; a negative value if the string has no token or
 * the conversion was unsuccessful. */
int iBSP430cliStoreExtractedUL (const char * * argstrp,
                                size_t * argstr_lenp,
                                unsigned long * destp);

/** Type for a command handler that needs only the remainder of the
 * command string.
 *
 * When the sBSP430cliCommand::handler field is set to this handler,
 * the corresponding sBSP430cliCommand::param field should be the
 * address of a conforming function which will be invoked.
 *
 * @param argstr the remainder of the command string
 */
typedef int (* iBSP430cliSimpleHandler) (const char * argstr);

/** Handler to invoke a simple command
 *
 * The sBSP430cliCommand::param field should be the
 * #iBSP430cliSimpleHandler that is to be invoked.
 *
 * See iBSP430cliHandlerFunction().
 *
 * @param chain ignored
 * @param param ignored
 * @param argstr passed to #iBSP430cliSimpleHandler
 * @param argstr_len ignored
 * @return value returned by simple handler. */
int iBSP430cliHandlerSimple (sBSP430cliCommandLink * chain,
                             void * param,
                             const char * argstr,
                             size_t argstr_len);

/** Handler to store a signed 16-bit integer expressed in text.
 *
 * See iBSP430cliHandlerFunction() and iBSP430cliStoreI().
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
 * See iBSP430cliHandlerFunction() and iBSP430cliStoreUI().
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
 * See iBSP430cliHandlerFunction() and iBSP430cliStoreL().
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
 * See iBSP430cliHandlerFunction() and iBSP430cliStoreUL().
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

  /** Returned when a token in the command string is rejected by the
   * processing command. */
  eBSP430_CLI_ERR_Invalid,

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
 * @param chain the chain of command links that did not produce an
 * error
 *
 * @param errtype the type of error discovered
 *
 * @param cmds the first of a set of sibling commands that could apply
 * at the point of error, or a null pointer if the error is unrelated
 * to failure to identify a subcommand.
 *
 * @param argstr the remainder of the command string at the point of
 * error
 *
 * @param argstr_len the length of @a argstr
 *
 * @return the value of <c>-(int)errtype</c> */
typedef int (* iBSP430cliDiagnosticFunction) (sBSP430cliCommandLink * chain,
                                              enum eBSP430cliErrorType errtype,
                                              const sBSP430cliCommand * cmds,
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
 * See #iBSP430cliDiagnosticFunction. */
int iBSP430cliNullDiagnostic (sBSP430cliCommandLink * chain,
                              enum eBSP430cliErrorType errtype,
                              const sBSP430cliCommand * cmds,
                              const char * argstr,
                              size_t argstr_len);

/** A diagnostic function that displays on the console.
 *
 * See #iBSP430cliDiagnosticFunction.
 *
 * @dependency #BSP430_CONSOLE */
#if defined(BSP430_DOXYGEN) || (BSP430_CONSOLE - 0)
int iBSP430cliConsoleDiagnostic (sBSP430cliCommandLink * chain,
                                 enum eBSP430cliErrorType errtype,
                                 const sBSP430cliCommand * cmds,
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

/** Specify the size of an internal command buffer for editing.
 *
 * A non-zero setting for this parameter allocates an internal buffer
 * that is used by iBSP430cliProcessConsoleInput_ni() to aggregate
 * keystrokes and produce a command string.
 *
 * If the value of this is zero, iBSP430cliProcessConsoleInput_ni()
 * will not be available.
 */
#if defined(BSP430_DOXYGEN) || ! defined(BSP430_CLI_CONSOLE_BUFFER_SIZE)
#define BSP430_CLI_CONSOLE_BUFFER_SIZE 0
#endif /* BSP430_CLI_CONSOLE_BUFFER_SIZE */

/** Enumeration of bit values returned from
 * iBSP430cliConsoleProcessInput_ni(). */
typedef enum eBSP430cliConsole {
  /** Bit set in response if console buffer now holds a completed
   * command.
   *
   * @note If this bit is set, there may be additional input that has
   * not been processed.  After processing the command that is ready
   * the caller should re-invoke #iBSP430cliConsoleProcessInput_ni()
   * to complete reading pending input. */
  eBSP430cliConsole_READY = 0x01,

  /** Bit set in response if caller should repaint the screen in
   * response to keystrokes */
  eBSP430cliConsole_REPAINT = 0x02,
} eBSP430cliConsole;

/** Return a pointer to the internal console buffer.
 *
 * @note Although the value returned from this function is a constant
 * throughout the life of the program, it is the call to this function
 * that stores the NUL character terminating the command based on data
 * received.
 *
 * @return pointer to the NUL-terminated current contents of the
 * internal command buffer, or NULL if #BSP430_CLI_CONSOLE_BUFFER_SIZE
 * is zero. */
const char * xBSP430cliConsoleBuffer_ni (void);

/** Clear any current console buffer contents.
 *
 * Invoke this after having processed a command that was returned by
 * iBSP430cliProcessConsoleInput_ni() so subsequent input begins a new
 * command.
 *
 * @dependency BSP430_CONSOLE
 * @dependency BSP430_CLI_CONSOLE_BUFFER_SIZE
 */
#if defined(BSP430_DOXYGEN) || (0 < BSP430_CLI_CONSOLE_BUFFER_SIZE)
void vBSP430cliClearConsoleBuffer_ni (void);
#endif /* BSP430_CLI_CONSOLE_BUFFER_SIZE */

/** Process pending console input and produce completed commands.
 *
 * This function reads data over the console, and stores the
 * characters into an internal buffer allocated when
 * #BSP430_CLI_CONSOLE_BUFFER_SIZE is nonzero.  Certain keystrokes are
 * recognized as editing commands, as described below.
 *
 * Keystroke      | Function
 * :------------- | :---------------
 * Backspace (BS) | Erase previous character (if any)
 * C-l (FF)       | Redraw screen with prompt and pending command
 * Enter (CR)     | Return with #eBSP430cliConsole_READY
 * C-u (NAK)      | Kill command (resets buffer)
 * C-w (ETB)      | Kill previous word (erases back to space)
 *
 * When carriage return is pressed, a complete command is recognized,
 * and the function returns even if there is additional data to be
 * consumed.
 *
 * @return zero if all pending input was consumed and no actions are
 * required.  A positive result encodes bits from #eBSP430cliConsole
 * indicating available commands or other actions that are required.
 *
 * @dependency BSP430_CONSOLE
 * @dependency BSP430_CLI_CONSOLE_BUFFER_SIZE
 */
#if defined(BSP430_DOXYGEN) || (0 < BSP430_CLI_CONSOLE_BUFFER_SIZE)
int iBSP430cliProcessConsoleInput_ni (void);
#endif /* BSP430_CLI_CONSOLE_BUFFER_SIZE */

#endif /* BSP430_UTILITY_CLI_H */
