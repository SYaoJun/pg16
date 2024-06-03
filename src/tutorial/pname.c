/*
 * src/tutorial/person_name.c
 *
 ******************************************************************************
  This file contains routines that can be bound to a Postgres backend and
  called by the backend in the process of processing queries.  The calling
  format for these routines is dictated by Postgres architecture.
******************************************************************************/

#include "postgres.h"

#include "fmgr.h"
#include "libpq/pqformat.h"		/* needed for send/recv functions */
#define NAMELEN 64
PG_MODULE_MAGIC;

typedef struct PersonName
{
	char		family_name[NAMELEN];
	char		given_names[NAMELEN];
}			PersonName;


/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(person_name_in);

Datum
person_name_in(PG_FUNCTION_ARGS)
{
	char *str = PG_GETARG_CSTRING(0);
    PersonName *result = (PersonName *) palloc(sizeof(PersonName));
    char *comma = strchr(str, ',');

    if (comma == NULL || comma == str || *(comma + 1) == '\0') {
        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                        errmsg("invalid input syntax for PersonName: \"%s\"", str)));
    }

    *comma = '\0';
    strncpy(result->family_name, str, 64);
    strncpy(result->given_names, comma + 1, 64);

    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(person_name_out);
/*how to change object instance to string*/
Datum
person_name_out(PG_FUNCTION_ARGS)
{
	PersonName    *person_name = (PersonName *) PG_GETARG_POINTER(0);
	char	   *result;

	result = psprintf("%s,%s", person_name->family_name, person_name->given_names);
	PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(person_name_send);

// Datum
// person_name_send(PG_FUNCTION_ARGS)
// {
//     PersonName *personname = (PersonName *) PG_GETARG_POINTER(0);
//     StringInfoData buf;

//     pq_begintypsend(&buf);
//     pq_sendbytes(&buf, (char *) personname->family_name, 64);
//     pq_sendbytes(&buf, (char *) personname->given_names, 64);
    
//     PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
// }


// PG_FUNCTION_INFO_V1(person_name_recv);

// Datum
// person_name_recv(PG_FUNCTION_ARGS)
// {
//     StringInfo  buf = (StringInfo) PG_GETARG_POINTER(0);
//     PersonName *result;

//     result = (PersonName *) palloc(sizeof(PersonName));
    
//     pq_copymsgbytes(buf, result->family_name, 64);
//     pq_copymsgbytes(buf, result->given_names, 64);

//     PG_RETURN_POINTER(result);
// }



