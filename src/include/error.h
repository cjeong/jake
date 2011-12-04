/**
 * @file error.h
 * @desc kernel error codes
 */
#ifndef ERROR_H
#define ERROR_H

/* kernel error codes; keep in sync with list in lib/printfmt.c */
#define E_UNSPECIFIED	1	     /* unspecified or unknown problem */
#define E_BAD_ENV     2      /* environment doesn't exist or otherwise 
																cannot be used in requested action */
#define E_INVAL       3      /* invalid parameter */
#define E_NO_MEM	    4      /* request failed due to memory shortage */
#define E_NO_FREE_ENV	5      /* attempt to create a new env beyond maximum */
#define E_FAULT       6      /* memory fault */
#define	MAXERROR	    6

#endif /* ERROR_H */
