
The following is the formal definition of the C standard features that are supported by the C~~ compiler.
Note that these features are categorized based on their respective definitions in the [C standard](https://www.iso-9899.info/n1570.html#A.).
# A. \[Annex A (informative) Language syntax summary]
## A.1 \[Lexical grammar]
### A.1.2 \[Keywords]
char
else
if
int
return
void
while
**Note: as written, the only primitive support offered is for signed integers and signed characters**
### A.1.3 \[Identifiers]
no change
### A.1.5 \[Constants]
integer-constant
* only in base 10.
character-constant
* including escaped characters
### A.1.6 \[String literals]
* no prefixes/suffixes.
* including escaped characters
### A.1.7 \[Punctuators]
punctuator: one of
	 \[] () {}  -> &
	 * + -  ! / %
	 << >> > <
	 <= >= == !=
	 ^  | && ||
	 ; =
**Notable Left Outs:**
	in-place arithmetic operations, e.g. +=, /=, etc.
	ternary expression (condition ? true : false)
	ALL PREPROCESSOR PUNCTUATORS - no support for the preprocessor
### A.1.8 \[Header names]
Unimplemented, as it is a part of the unimplemented preprocessor
### A.1.9 \[Preprocessing numbers]
Unimplemented, as it is a part of the unimplemented preprocessor
## A.2 \[Phrase structure grammar]
### A.2.1 \[Expressions]
