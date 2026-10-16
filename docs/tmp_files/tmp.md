- [x] Lexer.h:17:56: error: "remains with the callee": the caller keeps ownership of context \[-Wcallee-caller]
- [x] List.h:89:52: error: "remains at the callee": element stays with the caller \[-Wcallee-caller]
- [x] Text.h:16:18: error: "callee owned buffer": the buffer is owned by the caller \[-Wcallee-caller]
- [x] tests/test\_lexer.c:20:55: error: "remains with the callee": the buffer stays with the caller \[-Wcallee-caller]
- [x] Text.c:62:5: error: text\_list\_equals returns text\_equals (1 when equal); List treats 0 as a match
  \[-Wequals-polarity]
- [x] List.h:77:58: warning: callback named 'equals' but returns 0 when equal; also List.h:85:59, List.h:97:5
  \[-Wequals-polarity]
- [x] tests/test\_text\_view.c:12:5: warning: equals\_int returns 0 when equal (copied from tests/test\_list.c:12:5)
  \[-Wequals-polarity]
- [x] Lexer.c:65:38: warning: @retval refers to '@b offset', but lexer\_peek has no such parameter \[-Wdoc-rot]
- [x] Lexer.c:97:20: warning: '@b character' but the parameter is 'c'; also Lexer.c:106, 114, 124, 138 \[-Wdoc-rot]
- [ ] Lexer.c:220:59: warning: doc says '@b buffer4096', the parameter is 'buff4096' \[-Wdoc-rot]
- [x] Token.h:90:11: warning: undocumented sentinel: returns TOKEN\_NONE when no spelling matches
  \[-Wdoc-missing-contract]
- [x] Range.h:30:6: warning: range\_extend undocumented (negative n shrinks the range) \[-Wdoc-missing-contract]
- [x] Lexer.c:57:38: note: const Lexer\* calls the non-const getter; deep const holds by discipline only
  \[-Wdesign-deep-const]
- [x] Lexer.c:263:17: note: braceless else-if body indented one level too deep \[-Wdesign-braceless]
- [x] Token.h:92:6: note: 'token\_is\_EOF' is not snake\_case \[-Wnaming]
- [ ] Lexer.c:13:1: note: object-like #define where conventions prefer enum; also Lexer.c:14:1, Lexer.c:15:1 \[-Wnaming]
- [x] FilePosition.h:8:1: note: header includes itself \[-Wself-include]
- [x] Lexer.h:8:1: note: header includes itself \[-Wself-include]
- [x] Lexer.h:22:43: note: 'source\_file\_index' (definition uses source\_text\_index); also its doc at Lexer.h:19:11
  \[-Wrename-leftover]
- [x] Range.h:12:74: note: '@c Location' does not exist; it's FilePosition \[-Wrename-leftover]
- [x] FilePosition.h:17:101: note: '@c LineColumn' does not exist; it's FileLineColumn \[-Wrename-leftover]
- [x] FilePosition.c:20:60: note: assert message says 'source\_file\_table'; also variable 'source\_file' at :22:23 and
  message at :23:94 \[-Wrename-leftover]
- [x] Lexer.c:28:12: note: 'lexer\_stream', and "lexer stream" in docs (Lexer.c:52 and others); ADR-009 says it's not a
  stream \[-Wrename-leftover]
- [x] Context.h:14:54: note: '@c List of @c StringView' should be TextView \[-Wrename-leftover]
- [x] TextView.h:16:57: note: '@c StringView' in the macro doc; also TextView.h:20:74, TextView.h:21:17
  \[-Wrename-leftover]
- [x] TextView.h:35:44: note: parameter 'string\_view'; also TextView.c:18:50 \[-Wrename-leftover]
- [x] Text.h:18:30: note: '@c String' should be Text; also Text.h:26:20 \[-Wrename-leftover]
- [x] Text.c:14:17: note: variable 'string\_view' holds a Text \[-Wrename-leftover]
- [x] tests/test\_text\_view.c:25:13: note: 'test\_string\_view\_equals' \[-Wrename-leftover]
- [x] Driver.c:34:42: note: east const 'int const'; also FilePosition.c:33:55, Text.c:32:5, tests/test\_lexer.c:22:62
  \[-Wconst-placement]
- [x] Lexer.c:5:1: note: own header first here; system headers first in Context.c, FilePosition.c, List.c, Range.c,
  Text.c, TextView.c, Driver.c \[-Winclude-order]
- [ ] Lexer.c:224:67: note: 'buff4096' encodes LEXER\_MAX\_DECODE\_LITERAL\_LENGTH + 1 in its name \[-Wnaming]
- [x] Context.h:32:17: note: typo 'SrouceText' \[-Wtypo]
- [x] Token.h:13:47: note: typo 'DO NOT FORGOT' \[-Wtypo]
- [x] Token.h:38:11: note: 'TOKEN\_PERCENTAGE' (PERCENT); also Token.h:27:11 'PARENTHESES' (PAREN) \[-Wtypo]

