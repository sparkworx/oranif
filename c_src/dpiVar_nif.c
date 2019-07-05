#include "dpiVar_nif.h"
#include "dpiData_nif.h"
#include "dpiStmt_nif.h"

ErlNifResourceType *dpiVar_type;

void dpiVar_res_dtor(ErlNifEnv *env, void *resource)
{
    CALL_TRACE;
    RETURNED_TRACE;
}

DPI_NIF_FUN(var_setNumElementsInArray)
{
    CHECK_ARGCOUNT(2);

    dpiVar_res *vRes = NULL;
    uint32_t numElements;

    if ((!enif_get_resource(env, argv[0], dpiVar_type, (void **)&vRes)))
        BADARG_EXCEPTION(0, "resource var");
    if (!enif_get_uint(env, argv[1], &numElements))
        BADARG_EXCEPTION(1, "uint numElements");

    RAISE_EXCEPTION_ON_DPI_ERROR(
        vRes->context,
        dpiVar_setNumElementsInArray(vRes->var, numElements), NULL);

    RETURNED_TRACE;
    return ATOM_OK;
}

DPI_NIF_FUN(var_setFromBytes)
{
    CHECK_ARGCOUNT(3);

    dpiVar_res *vRes = NULL;
    ErlNifBinary value;
    uint32_t pos;

    if ((!enif_get_resource(env, argv[0], dpiVar_type, (void **)&vRes)))
        BADARG_EXCEPTION(0, "resource vat");
    if (!enif_get_uint(env, argv[1], &pos))
        BADARG_EXCEPTION(1, "uint pos");

    if (!enif_inspect_binary(env, argv[2], &value))
        BADARG_EXCEPTION(2, "binary/string value");

    RAISE_EXCEPTION_ON_DPI_ERROR(
        vRes->context,
        dpiVar_setFromBytes(
            vRes->var, pos, (const char *)value.data, value.size),
        NULL);

    RETURNED_TRACE;
    return ATOM_OK;
}

DPI_NIF_FUN(var_setFromStmt) // TODO: integration test
{
    CHECK_ARGCOUNT(3);

    dpiVar_res *vRes = NULL;
    dpiStmt_res *stmt = NULL;
    uint32_t pos;

    if ((!enif_get_resource(env, argv[0], dpiVar_type, (void **)&vRes)))
        BADARG_EXCEPTION(0, "resource vat");
    if (!enif_get_uint(env, argv[1], &pos))
        BADARG_EXCEPTION(1, "uint pos");

if (!enif_get_resource(env, argv[2], dpiStmt_type, (void **)&stmt))
        BADARG_EXCEPTION(1, "resource statement");

    RAISE_EXCEPTION_ON_DPI_ERROR(
        vRes->context,
        dpiVar_setFromStmt(
            vRes->var, pos, stmt->stmt),
        NULL);

    RETURNED_TRACE;
    return ATOM_OK;
}

DPI_NIF_FUN(var_release)
{
    CHECK_ARGCOUNT(1);

    dpiVar_res *vRes = NULL;

    if ((!enif_get_resource(env, argv[0], dpiVar_type, (void **)&vRes)))
        BADARG_EXCEPTION(0, "resource var");

    RAISE_EXCEPTION_ON_DPI_ERROR(
        vRes->context, dpiVar_release(vRes->var), NULL);

    dpiDataPtr_res *t_itr;
    for (dpiDataPtr_res *itr = vRes->head; itr != NULL;)
    {
        t_itr = itr;
        itr = itr->next;
        enif_release_resource(t_itr);
    }

    enif_release_resource(vRes);

    RETURNED_TRACE;
    return ATOM_OK;
}
