# sed script to update for interface change post 20130624 release
# Use: sed -i -r -f xni_console.sed
# Find: git ls-tree HEAD -r -z --name-only | xargs -0 ${Use}
s/\b(cputs)_ni\b/\1/g
s/\b(cputchar)_ni\b/\1/g
s/\b(cputi)_ni\b/\1/g
s/\b(cputu)_ni\b/\1/g
s/\b(cputl)_ni\b/\1/g
s/\b(cputul)_ni\b/\1/g
s/\b(cgetchar)_ni\b/\1/g
s/\b(cpeekchar)_ni\b/\1/g
s/\b(cputtext)_ni\b/\1/g
s/\b(vBSP430cliConsoleBufferClear)_ni\b/\1/g
s/\b(iBSP430cliConsoleBufferProcessInput)_ni\b/\1/g
s/\b(xBSP430cliConsoleBuffer)_ni\b/\1/g
