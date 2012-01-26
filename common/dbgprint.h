/********************************************************************
*  Licence: The MIT license
*
*  File: dbgprint.h
*
*
********************************************************************/

#ifndef _DBGPRINT_INCLUDED

#define _DBGPRINT_INCLUDED

#ifdef _DEBUG_PRINT

    #define DBG_PRINT(...) printf(__VA_ARGS__)
    #define DBG_PRINTF(fmt, ...) printf("%s: " fmt,__FUNCTION__, __VA_ARGS__)

#else   //ifdef _DEBUG_PRINT

    #define DBG_PRINT(...)
    #define DBG_PRINTF(fmt, ...)

#endif  //ifdef _DEBUG_PRINT



#endif //ifndef _DBGPRINT_INCLUDED
