/*
 * LexerDefs.h: Macros specialized for usage with Common/Device/Lexer.h.
 */

#ifndef COMMON_UTIL_LEXERDEFS_H_
#define COMMON_UTIL_LEXERDEFS_H_

#include <string>

// ("Internal" definitions for creating more useful ones.)
#define _IS(in,chr)     ((in) == (chr))
#define _IN(in,min,max) (((min) <= (in)) && ((in) <= (max)))

// Macros for 'next state' transitions (ie "in match condtion? then out").
#define IS(in,chr,out)     if (_IS(in,chr)) {return (out); break;}
#define IN(in,min,max,out) if (_IN(in,min,max)) {return (out); break;}
#define LABEL(in,out)      if (_IN(in,'a','z') || _IN(in,'A','Z') ||  \
	                           _IN(in,'0','9') || _IS(in, '_'))       \
		                   {return (out); break;}
#define HEX(in,out)        if (_IN(in,'a','f') || _IN(in,'A','F')) || \
	                           _IN(in,'0','9'))                       \
		                   {return (out); break;}
#define WS(in,out)         if (_IS(in,' ')  || _IS(in,'\n')        || \
                               _IS(in,'\r') || _IS(in,'\t'))          \
						   {return (out); break;}
#define EOFILE(in,out)     if (_IS(in,0xFF)) {return (out); break;}
#define ELSE(out)          return (out); break;
#define KWORD(in,str,out)  if (0 == in.compare(str)) {return (out);}

#endif /* COMMON_UTIL_LEXERDEFS_H_ */
