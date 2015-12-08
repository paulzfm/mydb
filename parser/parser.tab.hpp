/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     DATABASE = 258,
     DATABASES = 259,
     TABLE = 260,
     TABLES = 261,
     SHOW = 262,
     CREATE = 263,
     DROP = 264,
     USE = 265,
     CHECK = 266,
     PRIMARY = 267,
     KEY = 268,
     UNIQUE = 269,
     NOT = 270,
     NIL = 271,
     AUTO_INCREMENT = 272,
     INSERT = 273,
     INTO = 274,
     VALUES = 275,
     DELETE = 276,
     FROM = 277,
     WHERE = 278,
     UPDATE = 279,
     SET = 280,
     SELECT = 281,
     GROUP = 282,
     BY = 283,
     IS = 284,
     IN = 285,
     BETWEEN = 286,
     LIKE = 287,
     AND = 288,
     OR = 289,
     SUM = 290,
     AVG = 291,
     MAX = 292,
     MIN = 293,
     INT = 294,
     SMALLINT = 295,
     BIGINT = 296,
     FLOAT = 297,
     REAL = 298,
     DOUBLE = 299,
     VARCHAR = 300,
     STRING = 301,
     CHAR = 302,
     BOOLEAN = 303,
     DATETIME = 304,
     EQ = 305,
     NE = 306,
     LE = 307,
     GE = 308,
     LT = 309,
     GT = 310,
     LP = 311,
     RP = 312,
     COMMA = 313,
     STOP = 314,
     INTEGER = 315,
     IDENT = 316
   };
#endif
/* Tokens.  */
#define DATABASE 258
#define DATABASES 259
#define TABLE 260
#define TABLES 261
#define SHOW 262
#define CREATE 263
#define DROP 264
#define USE 265
#define CHECK 266
#define PRIMARY 267
#define KEY 268
#define UNIQUE 269
#define NOT 270
#define NIL 271
#define AUTO_INCREMENT 272
#define INSERT 273
#define INTO 274
#define VALUES 275
#define DELETE 276
#define FROM 277
#define WHERE 278
#define UPDATE 279
#define SET 280
#define SELECT 281
#define GROUP 282
#define BY 283
#define IS 284
#define IN 285
#define BETWEEN 286
#define LIKE 287
#define AND 288
#define OR 289
#define SUM 290
#define AVG 291
#define MAX 292
#define MIN 293
#define INT 294
#define SMALLINT 295
#define BIGINT 296
#define FLOAT 297
#define REAL 298
#define DOUBLE 299
#define VARCHAR 300
#define STRING 301
#define CHAR 302
#define BOOLEAN 303
#define DATETIME 304
#define EQ 305
#define NE 306
#define LE 307
#define GE 308
#define LT 309
#define GT 310
#define LP 311
#define RP 312
#define COMMA 313
#define STOP 314
#define INTEGER 315
#define IDENT 316




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

