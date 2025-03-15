# Project RISCII: Cprep (C-preprocessor) Manual (PR3002)
*"Reference for design of the RISCII C-preprocessor program"*

---

This document describes the abilities of the RISCII C-preprocessor program (ie cprep.exe). Contents include the RISCII C-preprocessor syntax and program usage.

**Table of Contents**
- [C-preprocessor Syntax](#c-preprocessor-syntax)
- [C-preprocessor Program](#c-preprocessor-program)
- [Future Considerations](#future-considerations)

**Referenced Terms**

|Term            |Description                                              |
|----------------|---------------------------------------------------------|
|cprep.exe       |C-preprocessor program (ie topic of this manual)         |

directive
variable
block

---

## C-preprocessor Syntax
---
This section covers the primary components of the C-preprocessor "language" and their effects.

In general, the syntax is made up of "directives" that must be the first token on a line to be recognized. See example below.

```
#include "foo.h"            // recognized
    #include "foo.h"        // recognized
int a;  #include "foo.h"    // not recognized
```

### Include Directive

Used to copy contents of given file to location of inclusion. Invoked with `#include` token followed directly by a file path in quotations. See example below.

```
#include "foo.h"
#include "dir/file.h"
#include "../anyfile.txt"
#include                     // invalid- token and file must be on same line
"err.h"
```

The included file is searched for first in the directory of the file invoking the include, then in all include directories (in the order they were specified- see [Command Line Usage](#command-line-usage)). Use standard directory navigation syntax (e.g. `./../foo.h` as needed).

*Note: In essence, this is a subset of GCC's preprocessor #include directive.*

### Define Directive

Used to define a variable name (for preprocessor use ONLY). Invoked with `#define` token followed directly by an alpha-numeric variable name (underscores allowed). See example below.

```
#define FOOBAR    // valid
#define _FOO_2    // valid
#define 3PO       // invalid- first character can't be number
#define           // invalid- token and variable must be on same line
ERR
```

Variables can only be defined once (per translation unit/source file), even if defined by an included file. Useful when paired with conditional directives.

*Note: In essence, this is a subset of GCC's preprocessor #define directive.*

### Conditional Directives

Used to conditionally add/remove source text based on preprocessor variables. Invoked with `#ifdef` or `#ifndef` token followed directly by a variable name and eventually by a `#endif` token. See example below.

```
#ifdef FOO
// some contents
#endif

#ifndef BAR
// some contents
#endif

#ifdef              // invalid- token and variable not on same line and
ERR                 //          missing matching #endif token
```

The contents of the created "block" will be included in the output file if the condition is met (`#ifdef` requiring the variable is defined, `#ifndef` requiring the opposite).

Conditional blocks can be nested, allowing source text to require multiple conditions to be met to be added. See example below.

```
#ifdef FOO
#ifdef BAR
// FOO + BAR contents
#endif
// FOO contents
#endif
```

*Note: In essence, this is a subset of GCC's preprocessor #ifdef, #ifndef, and #endif directives.*

## C-preprocessor Program
---
This section covers the high level usage of the cprep.exe program.

### Command Line Usage

The cprep.exe program is called via the command line. Arguments are either flags (beginning with `-`) or input files. Below is the program's help menu.

```
Project RISCII C Preprocessor
Usage: cprep.exe [options...] <input files...>

Options:
    -h        print this menu and exit
    -v        print version info and exit
    -ll <arg> sets stdout log level to:
                  silent  no log info
                  error   only errors
              (d) warning errors and warnings
                  info    process related info
                  debug   all available output
    -I  <arg> add include directory path
    -D  <arg> define variable for all files

```

One or more input files are required. Flags requiring arguments must have a non-flag argument (if not a specific argument). See example below.

```
> ./cprep                           // no input files
requires at least one input file
> ./cprep -I                        // argument required
-I requires an argument
> ./cprep -ll foo                   // wrong argument given
unknown log level 'foo'
```

Arguments for `-I` and `-D` must follow formatting requirements. Repeating either in a call is allowed, but redundant. See example below.

```
> ls
cprep.exe foo.c ope/
> ./cprep foo.c -I ./ope         // directory must end with '/'
Bad directory './ope'
> ./cprep foo.c -D 3D_GLASS      // variables cannot begin with number
Bad definition '3D_GLASS'
> ./cprep foo.c -D FOO -D FOO    // same as "./cprep foo.c -D FOO"
>ls
cprep.exe foo.c foo.i ope/
```

### Output File

Each input file is used to creat an output file the same name/path, but with the `.i` extension (overwritten if source file was already a `.i` file). Each input file is handled separately to the others. See example below.

```
> ./cprep foo.c                     // reused path ./foo
> ls
foo.c foo.i
> ./cprep ./dir/bar.c               // reused path ./dir/bar
>ls ./dir
bar.c bar.i
> ./cprep foo.c ./dir/bar.c         // equal to the last two cprep calls
```

Include directories and definitions (ie via `-I` and `-D`) are provided to all input files, despite each file being handled separately. See example below.

```
> ./cprep red.c blu.c -I ./tf2     // ./tf2/* included for both translations
> ./cprep green.c ham.c -D EGGS    // EGGS defined for both translations
```

## Future Considerations
---
This section covers developer's thoughts about the program's (potential) future.

### Copy Paste Define

At present, the `#define` keyword is strictly limited to preprocessing internals (ie resolving conditionals). Adding GCC's "copy/paste" feature to it would seriously increase its utility.

This, however, would require larger lexer/parser changes. It also would have to be smart enough to not copy/paste within "sensitive" source code (e.g. string literals).

### Boolean Logic Conditionals

At present, the conditions for conditionals are strictly based on (preprocessor) variables being defined or not. Adding boolean logic conditions, even between preprocessor variables, would add some flexibility in determining if source text was addded or not.

This would require moderate lexing/parsing changes, along with moderate to severe "CondNode" changes or extensions. The benefit is small for the effort, but a worthy idea to note here.