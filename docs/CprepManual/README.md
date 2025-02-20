# Project RISCII: Cprep (C-preprocessor) Manual (PR3002)
*"Reference for design of the RISCII C-preprocessor program"*

---

TODO- summary

**Table of Contents**
- [TODO](TODO)

**Referenced Terms**

|Term            |Description                                              |
|----------------|---------------------------------------------------------|
|example doc     |description of doc/how to find it                        |

---

## TODO
---
TODO

Lexing Notes:
- keywords defined by line w/ first non whitespace being '#'
- otherwise lexed per line- source text vs file include vs variable
- idea is line = (opt keyword) + text, include + file, or define + var
- EOF always treated as its own token
- var defined per usual, file include is simply two '"' (ie no escapes)
