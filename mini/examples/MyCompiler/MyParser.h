#ifndef YY_MyParser_h_included
#define YY_MyParser_h_included
#define YY_USE_CLASS

#line 1 "/usr/share/bison++/bison.h"
/* before anything */
#ifdef c_plusplus
 #ifndef __cplusplus
  #define __cplusplus
 #endif
#endif


 #line 8 "/usr/share/bison++/bison.h"
#define YY_MyParser_LSP_NEEDED 
#define YY_MyParser_ERROR_BODY  =0
#define YY_MyParser_LEX_BODY  =0
#line 8 "MyParser.y"

#include <iostream>
#include <string>
  using namespace std;
#define YY_DECL int yyFlexLexer::yylex(YY_MyParser_STYPE *val)
#ifndef FLEXFIX
#define FLEXFIX YY_MyParser_STYPE *val
#define FLEXFIX2 val
#endif

#line 19 "MyParser.y"
typedef union {
  int num;
  bool statement;
  } yy_MyParser_stype;
#define YY_MyParser_STYPE yy_MyParser_stype

#line 21 "/usr/share/bison++/bison.h"
 /* %{ and %header{ and %union, during decl */
#ifndef YY_MyParser_COMPATIBILITY
 #ifndef YY_USE_CLASS
  #define  YY_MyParser_COMPATIBILITY 1
 #else
  #define  YY_MyParser_COMPATIBILITY 0
 #endif
#endif

#if YY_MyParser_COMPATIBILITY != 0
/* backward compatibility */
 #ifdef YYLTYPE
  #ifndef YY_MyParser_LTYPE
   #define YY_MyParser_LTYPE YYLTYPE
/* WARNING obsolete !!! user defined YYLTYPE not reported into generated header */
/* use %define LTYPE */
  #endif
 #endif
/*#ifdef YYSTYPE*/
  #ifndef YY_MyParser_STYPE
   #define YY_MyParser_STYPE YYSTYPE
  /* WARNING obsolete !!! user defined YYSTYPE not reported into generated header */
   /* use %define STYPE */
  #endif
/*#endif*/
 #ifdef YYDEBUG
  #ifndef YY_MyParser_DEBUG
   #define  YY_MyParser_DEBUG YYDEBUG
   /* WARNING obsolete !!! user defined YYDEBUG not reported into generated header */
   /* use %define DEBUG */
  #endif
 #endif 
 /* use goto to be compatible */
 #ifndef YY_MyParser_USE_GOTO
  #define YY_MyParser_USE_GOTO 1
 #endif
#endif

/* use no goto to be clean in C++ */
#ifndef YY_MyParser_USE_GOTO
 #define YY_MyParser_USE_GOTO 0
#endif

#ifndef YY_MyParser_PURE

 #line 65 "/usr/share/bison++/bison.h"

#line 65 "/usr/share/bison++/bison.h"
/* YY_MyParser_PURE */
#endif


 #line 68 "/usr/share/bison++/bison.h"

#line 68 "/usr/share/bison++/bison.h"
/* prefix */

#ifndef YY_MyParser_DEBUG

 #line 71 "/usr/share/bison++/bison.h"

#line 71 "/usr/share/bison++/bison.h"
/* YY_MyParser_DEBUG */
#endif

#ifndef YY_MyParser_LSP_NEEDED

 #line 75 "/usr/share/bison++/bison.h"

#line 75 "/usr/share/bison++/bison.h"
 /* YY_MyParser_LSP_NEEDED*/
#endif

/* DEFAULT LTYPE*/
#ifdef YY_MyParser_LSP_NEEDED
 #ifndef YY_MyParser_LTYPE
  #ifndef BISON_YYLTYPE_ISDECLARED
   #define BISON_YYLTYPE_ISDECLARED
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;
  #endif

  #define YY_MyParser_LTYPE yyltype
 #endif
#endif

/* DEFAULT STYPE*/
#ifndef YY_MyParser_STYPE
 #define YY_MyParser_STYPE int
#endif

/* DEFAULT MISCELANEOUS */
#ifndef YY_MyParser_PARSE
 #define YY_MyParser_PARSE yyparse
#endif

#ifndef YY_MyParser_LEX
 #define YY_MyParser_LEX yylex
#endif

#ifndef YY_MyParser_LVAL
 #define YY_MyParser_LVAL yylval
#endif

#ifndef YY_MyParser_LLOC
 #define YY_MyParser_LLOC yylloc
#endif

#ifndef YY_MyParser_CHAR
 #define YY_MyParser_CHAR yychar
#endif

#ifndef YY_MyParser_NERRS
 #define YY_MyParser_NERRS yynerrs
#endif

#ifndef YY_MyParser_DEBUG_FLAG
 #define YY_MyParser_DEBUG_FLAG yydebug
#endif

#ifndef YY_MyParser_ERROR
 #define YY_MyParser_ERROR yyerror
#endif

#ifndef YY_MyParser_PARSE_PARAM
 #ifndef __STDC__
  #ifndef __cplusplus
   #ifndef YY_USE_CLASS
    #define YY_MyParser_PARSE_PARAM
    #ifndef YY_MyParser_PARSE_PARAM_DEF
     #define YY_MyParser_PARSE_PARAM_DEF
    #endif
   #endif
  #endif
 #endif
 #ifndef YY_MyParser_PARSE_PARAM
  #define YY_MyParser_PARSE_PARAM void
 #endif
#endif

/* TOKEN C */
#ifndef YY_USE_CLASS

 #ifndef YY_MyParser_PURE
  #ifndef yylval
   extern YY_MyParser_STYPE YY_MyParser_LVAL;
  #else
   #if yylval != YY_MyParser_LVAL
    extern YY_MyParser_STYPE YY_MyParser_LVAL;
   #else
    #warning "Namespace conflict, disabling some functionality (bison++ only)"
   #endif
  #endif
 #endif


 #line 169 "/usr/share/bison++/bison.h"
#define	PLUS	258
#define	INTEGER	259
#define	MINUS	260
#define	AND	261
#define	OR	262
#define	NOT	263
#define	LPARA	264
#define	RPARA	265
#define	BOOLEAN	266


#line 169 "/usr/share/bison++/bison.h"
 /* #defines token */
/* after #define tokens, before const tokens S5*/
#else
 #ifndef YY_MyParser_CLASS
  #define YY_MyParser_CLASS MyParser
 #endif

 #ifndef YY_MyParser_INHERIT
  #define YY_MyParser_INHERIT
 #endif

 #ifndef YY_MyParser_MEMBERS
  #define YY_MyParser_MEMBERS 
 #endif

 #ifndef YY_MyParser_LEX_BODY
  #define YY_MyParser_LEX_BODY  
 #endif

 #ifndef YY_MyParser_ERROR_BODY
  #define YY_MyParser_ERROR_BODY  
 #endif

 #ifndef YY_MyParser_CONSTRUCTOR_PARAM
  #define YY_MyParser_CONSTRUCTOR_PARAM
 #endif
 /* choose between enum and const */
 #ifndef YY_MyParser_USE_CONST_TOKEN
  #define YY_MyParser_USE_CONST_TOKEN 0
  /* yes enum is more compatible with flex,  */
  /* so by default we use it */ 
 #endif
 #if YY_MyParser_USE_CONST_TOKEN != 0
  #ifndef YY_MyParser_ENUM_TOKEN
   #define YY_MyParser_ENUM_TOKEN yy_MyParser_enum_token
  #endif
 #endif

class YY_MyParser_CLASS YY_MyParser_INHERIT
{
public: 
 #if YY_MyParser_USE_CONST_TOKEN != 0
  /* static const int token ... */
  
 #line 212 "/usr/share/bison++/bison.h"
static const int PLUS;
static const int INTEGER;
static const int MINUS;
static const int AND;
static const int OR;
static const int NOT;
static const int LPARA;
static const int RPARA;
static const int BOOLEAN;


#line 212 "/usr/share/bison++/bison.h"
 /* decl const */
 #else
  enum YY_MyParser_ENUM_TOKEN { YY_MyParser_NULL_TOKEN=0
  
 #line 215 "/usr/share/bison++/bison.h"
	,PLUS=258
	,INTEGER=259
	,MINUS=260
	,AND=261
	,OR=262
	,NOT=263
	,LPARA=264
	,RPARA=265
	,BOOLEAN=266


#line 215 "/usr/share/bison++/bison.h"
 /* enum token */
     }; /* end of enum declaration */
 #endif
public:
 int YY_MyParser_PARSE(YY_MyParser_PARSE_PARAM);
 virtual void YY_MyParser_ERROR(char *msg) YY_MyParser_ERROR_BODY;
 #ifdef YY_MyParser_PURE
  #ifdef YY_MyParser_LSP_NEEDED
   virtual int  YY_MyParser_LEX(YY_MyParser_STYPE *YY_MyParser_LVAL,YY_MyParser_LTYPE *YY_MyParser_LLOC) YY_MyParser_LEX_BODY;
  #else
   virtual int  YY_MyParser_LEX(YY_MyParser_STYPE *YY_MyParser_LVAL) YY_MyParser_LEX_BODY;
  #endif
 #else
  virtual int YY_MyParser_LEX() YY_MyParser_LEX_BODY;
  YY_MyParser_STYPE YY_MyParser_LVAL;
  #ifdef YY_MyParser_LSP_NEEDED
   YY_MyParser_LTYPE YY_MyParser_LLOC;
  #endif
  int YY_MyParser_NERRS;
  int YY_MyParser_CHAR;
 #endif
 #if YY_MyParser_DEBUG != 0
  public:
   int YY_MyParser_DEBUG_FLAG;	/*  nonzero means print parse trace	*/
 #endif
public:
 YY_MyParser_CLASS(YY_MyParser_CONSTRUCTOR_PARAM);
public:
 YY_MyParser_MEMBERS 
};
/* other declare folow */
#endif


#if YY_MyParser_COMPATIBILITY != 0
 /* backward compatibility */
 /* Removed due to bison problems
 /#ifndef YYSTYPE
 / #define YYSTYPE YY_MyParser_STYPE
 /#endif*/

 #ifndef YYLTYPE
  #define YYLTYPE YY_MyParser_LTYPE
 #endif
 #ifndef YYDEBUG
  #ifdef YY_MyParser_DEBUG 
   #define YYDEBUG YY_MyParser_DEBUG
  #endif
 #endif

#endif
/* END */

 #line 267 "/usr/share/bison++/bison.h"
#endif
