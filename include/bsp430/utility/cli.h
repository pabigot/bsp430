/* Copyright 2012-2014, Peter A. Bigot
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
 * This module provides several major features.  This page documents
 * the fundamental structures used by applications to register and
 * search among available commands.  The core structure is
 * #sBSP430cliCommand, which associates a command name (e.g., @c
 * config) with a handler function, optional descriptive text, and
 * links to sibling and child commands to support a hierarchy of
 * application-specific capabilities.
 *
 * Given a string command line, other functions parse the command to
 * identify a specific function and invoke it with the unparsed
 * remainder as function-specific arguments.  The application can also
 * register a diagnostic function to handle errors discovered during
 * command parsing.
 *
 * Supporting facilities also provided by this module are documented
 * in these pages:
 *
 * @li @ref grp_utility_cli_cli
 *
 * @li @ref grp_utility_cli_hci
 *
 * @li @ref grp_utility_cli_completion
 *
 * See @ref ex_utility_cli for a detailed example demonstrating most
 * capabilities of this module.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 *
 * @defgroup grp_utility_cli_cli Console-based Command Line User Interface
 *
 * @brief bsp430/utility/cli.h features for interactive user input.
 *
 * These features use bsp430/utility/console.h, generally with input
 * buffers enabled, to accept user input at a command line interface.
 * Certain keystrokes including backspace are recognized, allowing
 * primitive editing to be done.  Escape sequences are passed back to
 * the application for processing.  Display capabilities, including
 * repainting the screen and displaying the accumulated input are the
 * responsibility of the application, but are assisted with functions
 * to provide help on available commands and diagnostics when the
 * infrastructure detects an error in user input.
 *
 * See @ref ex_utility_cli for a detailed example demonstrating most
 * capabilities of this module.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 *
 * @brief bsp430/utility/cli.h functions for processing user-provided text.
 *
 * @defgroup grp_utility_cli_hci Extracting Data from User Input
 *
 * @brief bsp430/utility/cli.h functions for processing user-provided text.
 *
 * These functions provide a unified mechanism to extract command
 * tokens, quoted strings, and integer values from a text sequence
 * obtained through a command line interface, whether interactive or
 * batch.
 *
 * The interface assumes that command input is immutable; consequently
 * the tokens are identified by a pointer into the original input, and
 * a length identifying how many subsequent characters comprise the
 * complete token.
 *
 * In addition to routines that extract data from the input, there are
 * implementations for sBSP430cliCommand::handler that support storing
 * integer values directly into addresses, removing the need for
 * custom code to process the command.
 *
 * See @ref ex_utility_cli for a detailed example demonstrating most
 * capabilities of this module.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 *
 * @defgroup grp_utility_cli_completion Context-Sensitive Completion of User Input
 *
 * @brief bsp430/utility/cli.h data structures and functions for auto-completion of user input.
 *
 * The core command-line interface of BSP430 is useful, but
 * frustrating when whole commands must be remembered and typed
 * exactly.  The features in this group reference the standard data
 * structures that describe commands and use them to complete commands
 * given the first few letters, or (when a unique command is not
 * selected) provide hints about acceptable input in context.
 *
 * See @ref ex_utility_cli for a detailed example demonstrating most
 * capabilities of this module.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_CLI_H
#define BSP430_UTILITY_CLI_H

#include <bsp430/core.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Define to a true value to request that command completion be enabled.
 *
 * When command completion is enabled, a horizontal tab in command
 * input causes a flag to be returned to the caller indicating a need
 * to auto-fill the command buffer.
 *
 * @cppflag
 * @defaulted
 * @ingroup grp_utility_cli_completion
 */
#ifndef configBSP430_CLI_COMMAND_COMPLETION
#define configBSP430_CLI_COMMAND_COMPLETION 0
#endif /* configBSP430_CLI_COMMAND_COMPLETION */

/** Define to a true value to support customized completion.
 *
 * Default command completion will identify candidates using keys in a
 * chain of subcommands registered as a child of a command.  In some
 * contexts, the set of valid completions may not be a command; for
 * example, a command may accept a set of peripheral register names.
 * Because support for these cases incurs overhead in all command
 * structures and is uncommon, it must be explicitly enabled.
 *
 * Setting this to a true value causes the
 * sBSP430cliCommand::completion_helper field to be declared within
 * the structure.  That in turn may be set to an instance of the
 * #sBSP430cliCompletionHelper structure, which in turn will be
 * invoked to provide completion services in preference to using
 * subcommands.
 *
 * @cppflag
 * @defaulted
 * @dependency #configBSP430_CLI_COMMAND_COMPLETION
 * @ingroup grp_utility_cli_completion
 */
#ifndef configBSP430_CLI_COMMAND_COMPLETION_HELPER
#define configBSP430_CLI_COMMAND_COMPLETION_HELPER 0
#endif /* configBSP430_CLI_COMMAND_COMPLETION_HELPER */

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
 * beginning at the returned pointer.  The pointer may be null if the
 * length of the returned token is not of interest.
 *
 * @return A pointer to the first character of the first token in @p
 * *command.  If there are no such characters, the pointer is just
 * past the end of @p *commandp and both @p *remainingp and @p *lenp
 * are updated to have value zero.
 *
 * @ingroup grp_utility_cli_hci */
const char * xBSP430cliNextToken (const char ** commandp,
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
 * The pointer may be null if the length of the returned token is not
 * of interest.
 *
 * @return as with xBSP430cliNextToken().  If a quoted token is
 * identified, the return value begins after the opening quote.
 *
 * @ingroup grp_utility_cli_hci */
const char * xBSP430cliNextQToken (const char ** commandp,
                                   size_t * remainingp,
                                   size_t * lenp);

/* Forward declarations */
struct sBSP430cliCommand;
struct sBSP430cliCommandLink;
struct sBSP430cliCompletionData;
struct sBSP430cliCompletionHelper;

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
  /** A link to the parent command definition for layered commands.
   * It will be a null pointer if this link is at the top level of the
   * command hierarchy */
  struct sBSP430cliCommandLink * link;

  /** The set of potential comands at the level of this node */
  const struct sBSP430cliCommand * command_set;

  /** The command definition to be applied at this level.  This will
   * be one of the commands in @a command_set. */
  const struct sBSP430cliCommand * cmd;

} sBSP430cliCommandLink;

/** A function that implements customized completion.
 *
 * This is invoked by iBSP430cliCommandCompletion() when completion is
 * required for a command that has a registered @link
 * sBSP430cliCommand::completion_helper completion helper@endlink.
 * It in turn should invoke vBSP430cliCompletionHelperCallback() to
 * register acceptable tokens with the completion infrastructure.
 *
 * @param self the helper object registered within the command.  The
 * techniques of @ref callback_appinfo may be used to provide
 * command-specific context.
 *
 * @param argstr the text, including leading spaces, that comprise
 * the remainder of the input.  One or more tokens from this should
 * be used to effect completion.
 *
 * @param argstr_len the number of valid characters in @p argstr
 *
 * @param cdp the aggregation of completion data
 *
 * @dependency #configBSP430_CLI_COMMAND_COMPLETION_HELPER
 *
 * @ingroup grp_utility_cli_completion */
typedef void (* vBSP430cliCompletionHelper) (const struct sBSP430cliCompletionHelper * self,
                                             const char * argstr,
                                             size_t argstr_len,
                                             struct sBSP430cliCompletionData * cdp);

/** Structure encoding information to identify candidate completions.
 *
 * When #configBSP430_CLI_COMMAND_COMPLETION_HELPER is true
 * sBSP430cliCommand::completion_helper is present in every command
 * definition to support customized completion for commands that do
 * not have subcommands.
 *
 * This base structure contains only the pointer to the function that
 * implements the completion.  A pointer to the structure instance is
 * passed to the function, and the techniques of @ref callback_appinfo
 * may be used to provide additional information.  See
 * #sBSP430cliCompletionHelperStrings.
 *
 * @ingroup grp_utility_cli_completion */
typedef struct sBSP430cliCompletionHelper {
  /** The function that implements the completion operation. */
  vBSP430cliCompletionHelper helper;
} sBSP430cliCompletionHelper;

/** Structure supporting completion based on a static list of
 * acceptable strings.
 *
 * One of the most common cases for non-command completion is
 * selection among a set of permitted tokens.  This structure encodes
 * the necessary information to convey those tokens to
 * #vBSP430cliCompletionHelperStrings, which itself should be stored
 * as @a completion_helper.helper.
 *
 * @ingroup grp_utility_cli_completion
 */
typedef struct sBSP430cliCompletionHelperStrings {
  /** The common completion helper data, which should have its
   * sBSP430cliCompletionHelper::helper field set to
   * vBSP430cliCompletionHelperStrings(). */
  sBSP430cliCompletionHelper completion_helper;

  /** A pointer to a sequence of pointers to valid strings.  Note that
   * it is explicitly permitted for one or more of the contained
   * pointers to be null; those entries are ignored for the purposes
   * of completion. */
  const char * const * strings;

  /** The number of elements in @a strings. */
  size_t len;
} sBSP430cliCompletionHelperStrings;

/** A function that provides completion where a fixed set of tokens is
 * permitted.
 *
 * Specifically, this extracts the first token of @p argstr and
 * compares it against each string in the
 * #sBSP430cliCompletionHelperStrings structure of which @p self is
 * the first member.  Each string for which @p argstr is a prefix is
 * registered as an acceptable completion in @p cdp.
 *
 * See #sBSP430cliCompletionHelperStrings.
 *
 * @param self as in #vBSP430cliCompletionHelper
 *
 * @param argstr as in #vBSP430cliCompletionHelper
 *
 * @param argstr_len as in #vBSP430cliCompletionHelper
 *
 * @param cdp as in #vBSP430cliCompletionHelper
 *
 * @dependency #configBSP430_CLI_COMMAND_COMPLETION_HELPER
 *
 * @ingroup grp_utility_cli_completion */
void vBSP430cliCompletionHelperStrings (const struct sBSP430cliCompletionHelper * self,
                                        const char * argstr,
                                        size_t argstr_len,
                                        struct sBSP430cliCompletionData * cdp);

/** Utility function for user code to use #sBSP430cliCompletionHelperStrings.
 *
 * This function searches for a unique match in the
 * sBSP430cliCompletionHelperStrings::strings array using the first @p
 * argstr_len chars of @p argstr.  If successfully found, a pointer to
 * the element in sBSP430cliCompletionHelperStrings::strings that
 * matches is returned.  This in turn can be used to determine the
 * ordinal position of the match within the array, or to determine the
 * string that was matched.
 *
 * If @p *argstrp does not provide a unique prefix among the
 * candidates, a null pointer is returned, and neither @p *argstrp nor
 * @p *argstr_lenp are updated.
 *
 * @param chsp the record containing the list of acceptable tokens
 *
 * @param argstrp pointer to a pointer to the text used for input.  On
 * success, @p *argstrp is updated to point past the consumed token.
 *
 * @param argstr_lenp pointer to the length of input available at @p
 * *argstrp.  On success, @p *argstr_lenp is updated to the length of
 * the unconsumed input.
 *
 * @return a pointer to an offset within @p chsp->strings, or NULL
 *
 * @ingroup grp_utility_cli_hci */
const char * const * xBSP430cliHelperStringsExtract (const struct sBSP430cliCompletionHelperStrings * chsp,
                                                     const char ** argstrp,
                                                     size_t * argstr_lenp);

/** Type for a function that implements a command.
 *
 * @param cmd a pointer to the command structure that is being executed
 *
 * @param argstr any additional arguments remaining unprocessed after
 * reaching this command
 *
 * @param argstr_len the length of @p argstr in characters.
 *
 * @return a context-specific value. */
typedef int (* iBSP430cliHandlerFunction) (struct sBSP430cliCommandLink * chain,
                                           void * param,
                                           const char * argstr,
                                           size_t argstr_len);

/** Type for a command handler that needs only the remainder of the
 * command string.
 *
 * When the sBSP430cliCommand::handler field is set to this handler,
 * the corresponding sBSP430cliCommand::uParam::simple_handler field
 * should be the address of a conforming function which will be
 * invoked.
 *
 * @param argstr the remainder of the command string
 */
typedef int (* iBSP430cliSimpleHandler) (const char * argstr);

/** The definition of a command, including the token that identifies
 * it, optional help, subordinate and sibling command structures, and
 * the handler that implements the operation given user input. */
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

#if defined(BSP430_DOXYGEN)                                     \
  || ((configBSP430_CLI_COMMAND_COMPLETION - 0)                 \
      && (configBSP430_CLI_COMMAND_COMPLETION_HELPER - 0))
  /** Optional pointer to material supporting customized completion.
   *
   * @dependency #configBSP430_CLI_COMMAND_COMPLETION_HELPER */
  const sBSP430cliCompletionHelper * completion_helper;
#endif /* configBSP430_CLI_COMMAND_COMPLETION_HELPER */

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

  /** @var param
   *
   * A parameter providing additional information required for the
   * command through sBSP430cliCommand::uParam.  The value is
   * generally used by sBSP430cliCommand::handler in this or a parent
   * command structure.  For example, iBSP430cliHandlerStoreUI()
   * extracts the address of the variable into which the value should
   * be stored from uParam::ptr. */
  union uParam {
    /** The parameter field used for a pointer to a data object. */
    void * const ptr;

    /** The parameter field used for iBSP430cliHandlerSimple().  This
     * is a pointer-to-function which is not type-compatible with @p
     * ptr. */
    iBSP430cliSimpleHandler const simple_handler;
  } param;

} sBSP430cliCommand;

/** Callback support for iBSP430cliMatchCommand().  In addition to
 * providing a place to store the function that is called, this
 * structure may be followed in memory by application-specific memory,
 * using the techniques in @ref callback_appinfo. */
typedef struct sBSP430cliMatchCallback {
  /** The function that does something with a matching command structure.
   *
   * @param self a pointer to the callback structure
   *
   * @param cmd the command definition of interest */
  void (* callback) (struct sBSP430cliMatchCallback * self,
                     const sBSP430cliCommand * cmd);
} sBSP430cliMatchCallback;

/** A utility function to extract the next command in the parsed string.
 *
 * The goal here is to extract a non-empty token from the beginning of
 * the provided command (after any leading whitespace), and match it
 * as a prefix of a uniquely identified command in @p cmds.  If this
 * succeeds, we can go on to process the remainder of the command
 * line, which is returned.
 *
 * @param cmds the first in a sequence of sibling commands that may
 * appear at the beginning of the remainder of the command string.
 * This must not be a null pointer.
 *
 * @param command the unprocessed remainder of the command string.  If
 * there is no non-empty token in what remains, @p match_cb (if
 * provided) is invoked on all commands in @p cmds, and the number of
 * available commands is returned as a negative number.
 *
 * @param command_len the number of characters in @p command, possibly
 * including leading and trailing whitespace.
 *
 * @param matchp an optional pointer to where the uniquely identified
 * next command should be stored.  If @p matchp is null, no action is
 * taken; otherwise if a unique command could be identified the
 * pointer to that command is stored in @c *matchp; otherwise a null
 * pointer is stored in @p *matchp.
 *
 * @param match_callback an optional callback that is invoked on each
 * matching command.  If null, no action is taken; otherwise when the
 * key of a command in @p cmds has the first token of @p command as a
 * prefix @p match_callback() is invoked on that command.  The primary
 * use of this parameter is to determine the potential matching
 * commands for the purposes of diagnostics or command completion.
 *
 * @param argstrp if null no action is taken, otherwise the pointer to
 * the remainder of @p command after the initial token has been
 * removed is stored in @p *argstrp.
 *
 * @param argstr_lenp if null no action is taken, otherwise the length
 * of the string (that would be) stored in @p *argstrp is stored in @p
 * *argstr_lenp.
 *
 * @return the number of commands in @p cmds for which the first token
 * of @p command was a prefix, or a negative number if @p command is
 * empty (disregarding whitespace) so no prefix could be identified.
 */
int iBSP430cliMatchCommand (const sBSP430cliCommand * cmds,
                            const char * command,
                            size_t command_len,
                            const sBSP430cliCommand ** matchp,
                            sBSP430cliMatchCallback * match_callback,
                            const char ** argstrp,
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
 * of @p cmds.  If a unique command is identified, its
 * sBSP430cliCommand::handler function is invoked, providing a
 * #sBSP430cliCommandLink reference identifying the command itself,
 * the provided @p param argument, and the remainder of the @p command
 * string after having stripped off the initial token.
 *
 * @return a negative error code if a unique command is not
 * identifiable from the first token in @p command, otherwise the
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
 * @param chain_handler optional handler function to invoke on an
 * interior chain node command chain and remaining arguments prior to
 * continuing the chain on a child command set
 *
 * @param handler the handler function to execute on the
 * fully-resolved command chain and remaining arguments
 *
 * @return the value returned by the handler function */
int
iBSP430cliParseCommand (const sBSP430cliCommand * cmds,
                        void * param,
                        const char * command,
                        iBSP430cliHandlerFunction chain_handler,
                        iBSP430cliHandlerFunction handler);

/** Utility to extract and store a signed 16-bit integer expressed in
 * text.
 *
 * @param argstrp pointer to a pointer to the text representation of a
 * signed 16-bit integer, normally decimal but optionally in
 * hexadecimal (with leading @c 0x) or octal (with leading @c 0).  On
 * success @p *argstrp is updated to point past the consumed integer
 * token.
 *
 * @param argstr_lenp pointer to the length of the @p *argstrp text.
 * On success the @p *argstr_lenp is updated to hold
 * the remaining length of the string.
 *
 * @param destp pointer to where the extracted and converted value
 * should be stored.
 *
 * @return 0 if a valid value can be extracted and converted from the
 * head of @p *argstr; a negative value if the string has no token or
 * the conversion was unsuccessful.
 *
 * @ingroup grp_utility_cli_hci */
int iBSP430cliStoreExtractedI (const char ** argstrp,
                               size_t * argstr_lenp,
                               int * destp);

/** Utility to extract and store an unsigned 16-bit integer expressed in
 * text.
 *
 * @param argstrp pointer to a pointer to the text representation of a
 * signed 16-bit integer, normally decimal but optionally in
 * hexadecimal (with leading @c 0x) or octal (with leading @c 0).  On
 * success @p *argstrp is updated to point past the consumed integer
 * token.
 *
 * @param argstr_lenp pointer to the length of the @p *argstrp text.
 * On success the @p *argstr_lenp is updated to hold
 * the remaining length of the string.
 *
 * @param destp pointer to where the extracted and converted value
 * should be stored.
 *
 * @return 0 if a valid value can be extracted and converted from the
 * head of @p *argstr; a negative value if the string has no token or
 * the conversion was unsuccessful.
 *
 * @ingroup grp_utility_cli_hci */
int iBSP430cliStoreExtractedUI (const char ** argstrp,
                                size_t * argstr_lenp,
                                unsigned int * destp);

/** Utility to extract and store a signed 32-bit integer expressed in
 * text.
 *
 * @param argstrp pointer to a pointer to the text representation of a
 * signed 32-bit integer, normally decimal but optionally in
 * hexadecimal (with leading @c 0x) or octal (with leading @c 0).  On
 * success @p *argstrp is updated to point past the consumed integer
 * token.
 *
 * @param argstr_lenp pointer to the length of the @p *argstrp text.
 * On success the @p *argstr_lenp is updated to hold
 * the remaining length of the string.
 *
 * @param destp pointer to where the extracted and converted value
 * should be stored.
 *
 * @return 0 if a valid value can be extracted and converted from the
 * head of @p *argstr; a negative value if the string has no token or
 * the conversion was unsuccessful.
 *
 * @ingroup grp_utility_cli_hci */
int iBSP430cliStoreExtractedL (const char ** argstrp,
                               size_t * argstr_lenp,
                               long * destp);

/** Utility to extract and store an unsigned 32-bit integer expressed
 * in text.
 *
 * @param argstrp pointer to a pointer to the text representation of an
 * unsigned 32-bit integer, normally decimal but optionally in
 * hexadecimal (with leading @c 0x) or octal (with leading @c 0).  On
 * success @p *argstrp is updated to point past the consumed integer
 * token.
 *
 * @param argstr_lenp pointer to the length of the @p *argstrp text.
 * On success the @p *argstr_lenp is updated to hold
 * the remaining length of the string.
 *
 * @param destp pointer to where the extracted and converted value
 * should be stored.
 *
 * @return 0 if a valid value can be extracted and converted from the
 * head of @p *argstr; a negative value if the string has no token or
 * the conversion was unsuccessful.
 *
 * @ingroup grp_utility_cli_hci */
int iBSP430cliStoreExtractedUL (const char ** argstrp,
                                size_t * argstr_lenp,
                                unsigned long * destp);

/** Handler to invoke a simple command
 *
 * The @link sBSP430cliCommand::uParam::simple_handler @a
 * chain->cmd->param.simple_handler @endlink field should be the
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
 * sBSP430cliCommand::uParam::ptr @a chain->cmd->param.ptr @endlink is expected
 * to be a <c>int *</c> value.
 *
 * @param param unused
 *
 * @param argstr text representation of a signed 16-bit integer,
 * normally decimal but optionally in hexadecimal (with leading @c 0x)
 * or octal (with leading @c 0).
 *
 * @param argstr_len length of the @p argstr text
 *
 * @ingroup grp_utility_cli_hci */
int iBSP430cliHandlerStoreI (struct sBSP430cliCommandLink * chain,
                             void * param,
                             const char * argstr,
                             size_t argstr_len);

/** Handler to store an unsigned 16-bit integer expressed in text.
 *
 * See iBSP430cliHandlerFunction() and iBSP430cliStoreUI().
 *
 * @param chain Pointer to the end of the command chain.  @link
 * sBSP430cliCommand::uParam::ptr @a chain->cmd->param.ptr @endlink is
 * expected to be a <c>unsigned int *</c> value.
 *
 * @param param unused
 *
 * @param argstr text representation of an unsigned 16-bit integer,
 * normally decimal but optionally in hexadecimal (with leading @c 0x)
 * or octal (with leading @c 0).
 *
 * @param argstr_len length of the @p argstr text
 *
 * @ingroup grp_utility_cli_hci */
int iBSP430cliHandlerStoreUI (struct sBSP430cliCommandLink * chain,
                              void * param,
                              const char * argstr,
                              size_t argstr_len);

/** Handler to store a signed 32-bit integer expressed in text.
 *
 * See iBSP430cliHandlerFunction() and iBSP430cliStoreL().
 *
 * @param chain Pointer to the end of the command chain.  @link
 * sBSP430cliCommand::uParam::ptr @a chain->cmd->param.ptr @endlink is
 * expected to be a <c>long *</c> value.
 *
 * @param param unused
 *
 * @param argstr text representation of a signed 32-bit integer,
 * normally decimal but optionally in hexadecimal (with leading @c 0x)
 * or octal (with leading @c 0).
 *
 * @param argstr_len length of the @p argstr text
 *
 * @ingroup grp_utility_cli_hci */
int iBSP430cliHandlerStoreL (struct sBSP430cliCommandLink * chain,
                             void * param,
                             const char * argstr,
                             size_t argstr_len);

/** Handler to store an unsigned 32-bit integer expressed in text.
 *
 * See iBSP430cliHandlerFunction() and iBSP430cliStoreUL().
 *
 * @param chain Pointer to the end of the command chain.  @link
 * sBSP430cliCommand::uParam::ptr @a chain->cmd->param.ptr @endlink is
 * expected to be a <c>unsigned long *</c> value.
 *
 * @param param unused
 *
 * @param argstr text representation of an unsigned 32-bit integer,
 * normally decimal but optionally in hexadecimal (with leading @c 0x)
 * or octal (with leading @c 0).
 *
 * @param argstr_len length of the @p argstr text
 *
 * @ingroup grp_utility_cli_hci */
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
 * @param argstr the remainder of the command string at the point of
 * error
 *
 * @param argstr_len the length of @p argstr
 *
 * @return the value of <c>-(int)errtype</c> */
typedef int (* iBSP430cliDiagnosticFunction) (sBSP430cliCommandLink * chain,
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
 * See #iBSP430cliDiagnosticFunction. */
int iBSP430cliNullDiagnostic (sBSP430cliCommandLink * chain,
                              enum eBSP430cliErrorType errtype,
                              const char * argstr,
                              size_t argstr_len);

/** A diagnostic function that displays on the console.
 *
 * Normal practice would be to install this using
 * iBSP430cliSetDiagnosticFunction() after configuring the console but
 * before accepting any user input; see @ref ex_utility_cli.
 *
 * @consoleoutput
 * @dependency #BSP430_CONSOLE
 * @ingroup grp_utility_cli_cli */
#if defined(BSP430_DOXYGEN) || (BSP430_CONSOLE - 0)
int iBSP430cliConsoleDiagnostic (sBSP430cliCommandLink * chain,
                                 enum eBSP430cliErrorType errtype,
                                 const char * argstr,
                                 size_t argstr_len);
#endif /* configBSP430_CONSOLE */

/** Display the reconstructed command string on the console.
 *
 * This function is usually invoked in the context of diagnostics to
 * indicate where in the input an error was detected.  It prints on
 * the console the sequence of prefix commands encoded within @p
 * chain, then displays the remaining argument string.  (Note that to
 * do this it reverses the chain, then restores it to its original
 * direction before returning.)
 *
 * @consoleoutput
 *
 * @param chain the chain of commands from the innermost to the outermost
 *
 * @param argstr the unprocessed remainder of the original command string
 *
 * @ingroup grp_utility_cli_cli */
#if defined(BSP430_DOXYGEN) || (BSP430_CONSOLE - 0)
void vBSP430cliConsoleDisplayChain (struct sBSP430cliCommandLink * chain,
                                    const char * argstr);
#endif /* configBSP430_CONSOLE */


/** Display the deep command structure on the console.
 *
 * Nested commands are displayed indented from their parent command.
 *
 * @consoleoutput
 *
 * @param cmd pointer to the first in a sequence of sibling commands
 *
 * @dependency #BSP430_CONSOLE
 *
 * @ingroup grp_utility_cli_cli */
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
 * that is used by iBSP430cliConsoleBufferProcessInput() to aggregate
 * keystrokes and produce a command string.
 *
 * If the value of this is zero, iBSP430cliConsoleBufferProcessInput()
 * will not be available.
 *
 * @ingroup grp_utility_cli_cli
 */
#if defined(BSP430_DOXYGEN) || ! defined(BSP430_CLI_CONSOLE_BUFFER_SIZE)
#define BSP430_CLI_CONSOLE_BUFFER_SIZE 0
#endif /* BSP430_CLI_CONSOLE_BUFFER_SIZE */

/** Enumeration of bit values returned from
 * iBSP430cliConsoleBufferProcessInput().
 *
 * @ingroup grp_utility_cli_cli*/
typedef enum eBSP430cliConsole {
  /** Bit set in response if console buffer now holds a completed
   * command.  The application should retrieve it using
   * xBSP430cliConsoleBuffer(), process it, then reset the buffer
   * using vBSP430cliConsoleBufferClear().
   *
   * @note If this bit is set, there may be additional input that has
   * not been processed.  After processing the command that is ready
   * the caller should re-invoke iBSP430cliConsoleBufferProcessInput()
   * to complete reading pending input. */
  eBSP430cliConsole_READY = 0x01,

  /** Bit set in response if caller should repaint the screen in
   * response to keystrokes */
  eBSP430cliConsole_REPAINT = 0x02,

  /** Bit set in response if caller should invoke the completion
   * infrastructure based on the current buffer contents. */
  eBSP430cliConsole_DO_COMPLETION = 0x04,

  /** Bit set in response if an escape character (ESC) has been
   * consumed.  The caller should process the remaining input to
   * consume the escape sequence.  If the sequence is an ANSI
   * sequence, there are two cases the processing code must recognize:
   *
   * @li If the character after the ESC is a left bracket (comprising
   * a Control Sequence Introducer, or CSI), the sequence is
   * terminated by an ASCII character in the range 64 to 126.  See
   * #eBSP430cliConsole_IN_ESCAPE.
   *
   * @li Otherwise, the consumed ESC is expected to be followed by a
   * single ASCII character in the range 64 to 95.
   *
   * If the caller is uninterested in escape sequences but wishes to
   * be robust if the user generates one, use
   * iBSP430cliConsoleBufferConsumeEscape() to filter them without
   * misinterpreting partial escape sequences as valid input.
   *
   * @see http://en.wikipedia.org/wiki/ANSI_escape_code. */
  eBSP430cliConsole_PROCESS_ESCAPE = 0x08,

  /** Bit set in flags if system is processing an escape sequence.
   *
   * This flag is not produced by the infrastructure, but is available
   * for use in applications which preserve the console state while
   * processing.  Suggested practice is to read the first character
   * after #eBSP430cliConsole_PROCESS_ESCAPE has been set, and if that
   * character is recognized as completing a control sequence
   * introducer the application should clear
   * #eBSP430cliConsole_PROCESS_ESCAPE, set
   * #eBSP430cliConsole_IN_ESCAPE, and continue in escape-processing
   * mode until the entire sequence has been consumed (i.e., a
   * character in the range 64 to 126 is recognized).
   *
   * @see @ref ex_utility_cli. */
  eBSP430cliConsole_IN_ESCAPE = 0x10,

  /** Bit mask for flags indicating that some escape-sequence
   * processing is in effect. */
  eBSP430cliConsole_ANY_ESCAPE = eBSP430cliConsole_PROCESS_ESCAPE | eBSP430cliConsole_IN_ESCAPE,

  /** Bit set to signal that the repaint should be followed by a BEL
   * or other indicator.  This is normally used to warn the user that
   * the screen content includes changes not directly entered by the
   * user, e.g. completion has been performed. */
  eBSP430cliConsole_REPAINT_BEL = 0x100,

  /** Bit set in response from completion if the application should
   * add a space after whatever text was suggested.  (This is a
   * distinct flag because suggested text from the completion
   * infrastructure comes from immutable strings that do not include
   * trailing spaces.) */
  eBSP430cliConsole_COMPLETE_SPACE = 0x200,

} eBSP430cliConsole;

/** Data structure used to communicate between user applications and
 * the command completion infrastructure.
 *
 * This is a parameter to iBSP430cliCommandCompletion().
 *
 * @ingroup grp_utility_cli_completion */
typedef struct sBSP430cliCompletionData {
  /** The command that is to be completed.  On the initial call to
   * iBSP430cliCommandCompletion() this should be NUL terminated, and
   * the length will be stored in @a command_len.  During execution @a
   * command and @a command_len will be updated as leading commands
   * are stripped off. */
  const char * command;

  /** The number of valid characters at @a command */
  size_t command_len;

  /** The set of commands against which completion is performed.  This
   * is the set corresponding to the first token in @a command, which
   * is parsed to reach the completion point using the same process as
   * when attempting to execute the command.  @a command_set is
   * ignored once the first command has been resolved. */
  const sBSP430cliCommand * command_set;

  /** Pointer to where the user wants candidate completion text
   * returned.  A null value indicates candidate completions will not
   * be returned to the user.  When used internally, the pointer must
   * not be null: the module will provide a temporary buffer in this
   * case. */
  const char ** returned_candidates;

  /** Maximum number of elements that can be placed in @a
   * candidatesp. */
  size_t max_returned_candidates;

  /** Returned total number of candidates identified */
  size_t ncandidates;

  /** A pointer to the text to be appended.  If this is null,
   * completion failed to identify data to append to the command.
   * Otherwise this will be a reference to a position in one of the
   * keys in @a command_set or a non-command completion string. */
  const char * append;

  /** The number of characters to be appended, when the completion is
   * not unique.  This value is valid only when @a append is not a
   * null pointer. */
  size_t append_len;
} sBSP430cliCompletionData;

/** Register an acceptable candidate for command completion.
 *
 * This function calculates the new common prefix and updates the list
 * of candidate completions to include the provided new candidate.  It
 * is invoked by iBSP430cliCommandCompletion() indirectly through the
 * use of internal and external #sBSP430cliCompletionHelper instances.
 *
 * @param cdp the aggregation of completion data
 *
 * @param candidate a token that would be acceptable at the current
 * point of processing
 *
 * @ingroup grp_utility_cli_completion*/
void vBSP430cliCompletionHelperCallback (sBSP430cliCompletionData * cdp,
                                         const char * candidate);

/** Return a pointer to the internal console buffer.
 *
 * @note Although the value returned from this function is a constant
 * throughout the life of the program, it is the call to this function
 * that stores the NUL character terminating the command based on data
 * received.
 *
 * @return pointer to the NUL-terminated current contents of the
 * internal command buffer, or NULL if #BSP430_CLI_CONSOLE_BUFFER_SIZE
 * is zero.
 *
 * @ingroup grp_utility_cli_cli */
const char * xBSP430cliConsoleBuffer (void);

/** Clear any current console buffer contents.
 *
 * Invoke this after having processed a command that was returned by
 * iBSP430cliConsoleBufferProcessInput() so subsequent input begins a new
 * command.
 *
 * @dependency #BSP430_CONSOLE
 * @dependency #BSP430_CLI_CONSOLE_BUFFER_SIZE
 * @ingroup grp_utility_cli_cli
 */
#if defined(BSP430_DOXYGEN) || (0 < BSP430_CLI_CONSOLE_BUFFER_SIZE)
void vBSP430cliConsoleBufferClear (void);
#endif /* BSP430_CLI_CONSOLE_BUFFER_SIZE */

/** Append characters to the current console buffer contents.
 *
 * Characters are taken from @p src until an end-of-string is
 * encountered or @p len characters have been appended.
 *
 * @param text pointer to the text to be appended
 *
 * @param len number of characters to be appended.  To append
 * everything to the end of @p text, use -1 or another large value.
 *
 * @return the number of characters actually appended, which may be
 * less than @p len if the console buffer is too small, or @p text has
 * a @c NUL before the length is reached.
 *
 * @dependency #BSP430_CONSOLE
 * @dependency #BSP430_CLI_CONSOLE_BUFFER_SIZE
 * @ingroup grp_utility_cli_cli
 */
#if defined(BSP430_DOXYGEN) || (0 < BSP430_CLI_CONSOLE_BUFFER_SIZE)
int iBSP430cliConsoleBufferExtend (const char * text, size_t len);
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
 * C-l (FF)       | Return #eBSP430cliConsole_REPAINT
 * Enter (CR)     | Return #eBSP430cliConsole_READY
 * C-u (NAK)      | Kill command (resets buffer)
 * C-w (ETB)      | Kill previous word (erases back to space)
 * Escape (ESC)   | Return #eBSP430cliConsole_PROCESS_ESCAPE
 * Tab (HT)       | Auto-complete based on legal commands (<b>if enabled</b>)
 *
 * Note that auto-completion is enabled by
 * #configBSP430_CLI_COMMAND_COMPLETION.
 *
 * When carriage return is pressed, a complete command is recognized,
 * and the function returns even if there is additional data to be
 * consumed.
 *
 * @return zero if all pending input was consumed and no actions are
 * required.  A positive result encodes bits from #eBSP430cliConsole
 * indicating available commands or other actions that are required.
 *
 * @dependency #BSP430_CONSOLE
 * @dependency #BSP430_CLI_CONSOLE_BUFFER_SIZE
 * @ingroup grp_utility_cli_cli
 */
#if defined(BSP430_DOXYGEN) || (0 < BSP430_CLI_CONSOLE_BUFFER_SIZE)
int iBSP430cliConsoleBufferProcessInput (void);
#endif /* BSP430_CLI_CONSOLE_BUFFER_SIZE */

/** Consume any pending escape sequences recorded in @p flags.
 *
 * This utility function allows applications that perform console
 * input but do not support escape sequences to discard them without
 * having to implement the escape state machine or incorrectly
 * processing the tail of an escape sequence as text.
 *
 * If @p flags on entry already has no set bits in
 * #eBSP430cliConsole_ANY_ESCAPE then @p flags is returned without
 * change or delay.  Otherwise this function will block until the
 * remainder of the escape sequence has been entered and consumed.
 *
 * The escape sequence is not returned to the caller.
 *
 * @param flags A value returned by
 * iBSP430cliConsoleBufferProcessInput_ni()
 *
 * @return @p flags with #eBSP430cliConsole_ANY_ESCAPE zero after
 * consuming the necessary characters to complete the escape
 * sequence.
 *
 * @blocking */
int iBSP430cliConsoleBufferConsumeEscape (int flags);

/** Interface to command completion.
 *
 * This function traverses the command in @p cdp->command in the
 * context of @p cdp->command_set and identifies text that would
 * unambiguously extend the command, or where ambiguity is present a
 * set of candidate text that could complete the next step of the
 * command.
 *
 * This version simply does the calculations.  For an example, or to
 * use this within a standard CLI console environment, see
 * iBSP430cliConsoleBufferCompletion().
 *
 * @param cdp Pointer to the structure used to pass in and return
 * information about the completion.  On entry, the @a command, @a
 * command_set, @a returned_candidates, and @a max_returned_candidates
 * fields must be set.  All others are set in the process of executing
 * this function.
 *
 * @return a bitset of flags indicating caller update
 * responsibilities, potentially including
 * #eBSP430cliConsole_COMPLETE_SPACE, #eBSP430cliConsole_REPAINT, and
 * #eBSP430cliConsole_REPAINT_BEL.
 *
 * @dependency #configBSP430_CLI_COMMAND_COMPLETION
 *
 * @ingroup grp_utility_cli_completion
 */
#if defined(BSP430_DOXYGEN) || (configBSP430_CLI_COMMAND_COMPLETION - 0)
int iBSP430cliCommandCompletion (sBSP430cliCompletionData * cdp);
#endif /* configBSP430_CLI_COMMAND_COMPLETION */

#ifndef BSP430_CLI_CONSOLE_BUFFER_MAX_COMPLETIONS
/** The maximum number of candidate completions returned by
 * iBSP430cliConsoleBufferCompletion().
 *
 * The value must be at least 1.
 *
 * @defaulted
 * @ingroup grp_utility_cli_completion */
#define BSP430_CLI_CONSOLE_BUFFER_MAX_COMPLETIONS 5
#endif /* BSP430_CLI_CONSOLE_BUFFER_MAX_COMPLETIONS */

/** Complete the command currently in the console buffer
 *
 * This is a wrapper around iBSP430cliCommandCompletion() which
 * applies the results from that function.  If the completion can be
 * uniquely identified, the text is added to the command buffer; the
 * text is also printed to the console unless the returned flags
 * indicate the caller is for repainting.  If alternatives are
 * available, a prompt will be emitted showing available completions.
 *
 * @param command_set the set of commands acceptable at the top level
 * of the command interpreter.
 *
 * @param commandp a pointer to storage for a command pointer.  On
 * input, this should be NULL or pre-initialized with the return value
 * from xBSP430cliConsoleBuffer().  On exit, it will be updated to
 * the new return value from xBSP430cliConsoleBuffer().
 *
 * @return a bitset of flags indicating caller update
 * responsibilities, potentially including #eBSP430cliConsole_REPAINT,
 * and #eBSP430cliConsole_REPAINT_BEL.
 *
 * @dependency #BSP430_CONSOLE
 * @dependency #BSP430_CLI_CONSOLE_BUFFER_SIZE
 * @dependency #configBSP430_CLI_COMMAND_COMPLETION
 *
 * @ingroup grp_utility_cli_completion
 */
#if defined(BSP430_DOXYGEN) || (configBSP430_CLI_COMMAND_COMPLETION - 0)
int iBSP430cliConsoleBufferCompletion (const sBSP430cliCommand * command_set,
                                       const char ** commandp);
#endif /* configBSP430_CLI_COMMAND_COMPLETION */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSP430_UTILITY_CLI_H */
