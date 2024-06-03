/*
 * src/tutorial/pos.c
 *
 ******************************************************************************
  This file contains routines that can be bound to a Postgres backend and
  called by the backend in the process of processing queries.  The calling
  format for these routines is dictated by Postgres architecture.
******************************************************************************/

#include "postgres.h"

#include "fmgr.h"
#include "libpq/pqformat.h"		/* needed for send/recv functions */

PG_MODULE_MAGIC;

typedef struct Pos
{
	double		x;
	double		y;
}			Pos;


/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(pos_in);

Datum
pos_in(PG_FUNCTION_ARGS)
{
	char	   *str = PG_GETARG_CSTRING(0);
	double		x,
				y;
	Pos    *result;

	if (sscanf(str, " ( %lf , %lf )", &x, &y) != 2)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for type %s: \"%s\"",
						"pos", str)));

	result = (Pos *) palloc(sizeof(Pos));
	result->x = x;
	result->y = y;
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(pos_out);

Datum
pos_out(PG_FUNCTION_ARGS)
{
	Pos    *pos = (Pos *) PG_GETARG_POINTER(0);
	char	   *result;

	result = psprintf("(%g,%g)", pos->x, pos->y);
	PG_RETURN_CSTRING(result);
}

/*****************************************************************************
 * Binary Input/Output functions
 *
 * These are optional.
 *****************************************************************************/

PG_FUNCTION_INFO_V1(pos_recv);

Datum
pos_recv(PG_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) PG_GETARG_POINTER(0);
	Pos    *result;

	result = (Pos *) palloc(sizeof(Pos));
	result->x = pq_getmsgfloat8(buf);
	result->y = pq_getmsgfloat8(buf);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(pos_send);

Datum
pos_send(PG_FUNCTION_ARGS)
{
	Pos    *pos = (Pos *) PG_GETARG_POINTER(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendfloat8(&buf, pos->x);
	pq_sendfloat8(&buf, pos->y);
	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/*****************************************************************************
 * New Operators
 *
 * A practical Pos datatype would provide much more than this, of course.
 *****************************************************************************/

PG_FUNCTION_INFO_V1(pos_add);

Datum
pos_add(PG_FUNCTION_ARGS)
{
	Pos    *a = (Pos *) PG_GETARG_POINTER(0);
	Pos    *b = (Pos *) PG_GETARG_POINTER(1);
	Pos    *result;

	result = (Pos *) palloc(sizeof(Pos));
	result->x = a->x + b->x;
	result->y = a->y + b->y;
	PG_RETURN_POINTER(result);
}



