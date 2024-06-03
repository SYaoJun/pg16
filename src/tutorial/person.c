/*
 * src/tutorial/person.c
 *
 ******************************************************************************
  This file contains routines that can be bound to a Persontgres backend and
  called by the backend in the process of processing queries.  The calling
  format for these routines is dictated by Persontgres architecture.
******************************************************************************/

#include "postgres.h"

#include "fmgr.h"
#include "libpq/pqformat.h"		/* needed for send/recv functions */

PG_MODULE_MAGIC;

typedef struct Person
{
	double		x;
	double		y;
}			Person;


/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(person_in);

Datum
person_in(PG_FUNCTION_ARGS)
{
	char	   *str = PG_GETARG_CSTRING(0);
	double		x,
				y;
	Person    *result;

	if (sscanf(str, " ( %lf , %lf )", &x, &y) != 2)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for type %s: \"%s\"",
						"person", str)));

	result = (Person *) palloc(sizeof(Person));
	result->x = x;
	result->y = y;
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(person_out);

Datum
person_out(PG_FUNCTION_ARGS)
{
	Person    *person = (Person *) PG_GETARG_POINTER(0);
	char	   *result;

	result = psprintf("(%g,%g)", person->x, person->y);
	PG_RETURN_CSTRING(result);
}

/*****************************************************************************
 * Binary Input/Output functions
 *
 * These are optional.
 *****************************************************************************/

PG_FUNCTION_INFO_V1(person_recv);

Datum
person_recv(PG_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) PG_GETARG_POINTER(0);
	Person    *result;

	result = (Person *) palloc(sizeof(Person));
	result->x = pq_getmsgfloat8(buf);
	result->y = pq_getmsgfloat8(buf);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(person_send);

Datum
person_send(PG_FUNCTION_ARGS)
{
	Person    *person = (Person *) PG_GETARG_POINTER(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendfloat8(&buf, person->x);
	pq_sendfloat8(&buf, person->y);
	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/*****************************************************************************
 * New Operators
 *
 * A practical Person datatype would provide much more than this, of course.
 *****************************************************************************/

PG_FUNCTION_INFO_V1(person_add);

Datum
person_add(PG_FUNCTION_ARGS)
{
	Person    *a = (Person *) PG_GETARG_POINTER(0);
	Person    *b = (Person *) PG_GETARG_POINTER(1);
	Person    *result;

	result = (Person *) palloc(sizeof(Person));
	result->x = a->x + b->x;
	result->y = a->y + b->y;
	PG_RETURN_POINTER(result);
}



