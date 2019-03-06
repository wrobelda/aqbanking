/***************************************************************************
 begin       : Tue May 03 2005
 copyright   : (C) 2018 by Martin Preuss
 email       : martin@libchipcard.de

 ***************************************************************************
 *          Please see toplevel file COPYING for license details           *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif


#include "globals_l.h"
#include "aqhbci/banking/user.h"

#include <gwenhywfar/text.h>



int AH_Control_SendKeys(AB_PROVIDER *pro,
                        GWEN_DB_NODE *dbArgs,
                        int argc,
                        char **argv)
{
  GWEN_DB_NODE *db;
  uint32_t uid;
  AB_USER *u=NULL;
  int rv;
  int withAuthKey=0;
  const GWEN_ARGS args[]= {
    {
      GWEN_ARGS_FLAGS_HAS_ARGUMENT, /* flags */
      GWEN_ArgsType_Int,            /* type */
      "userId",                     /* name */
      0,                            /* minnum */
      1,                            /* maxnum */
      "u",                          /* short option */
      "user",                       /* long option */
      "Specify the unique user id",    /* short description */
      "Specify the unique user id"     /* long description */
    },
    {
      0,                            /* flags */
      GWEN_ArgsType_Int,            /* type */
      "authkey",                    /* name */
      0,                            /* minnum */
      1,                            /* maxnum */
      "A",                          /* short option */
      "authkey",                    /* long option */
      "Also send the authentication key",    /* short description */
      "Also send the authentication key"     /* long description */
    },
    {
      GWEN_ARGS_FLAGS_HELP | GWEN_ARGS_FLAGS_LAST, /* flags */
      GWEN_ArgsType_Int,            /* type */
      "help",                       /* name */
      0,                            /* minnum */
      0,                            /* maxnum */
      "h",                          /* short option */
      "help",                       /* long option */
      "Show this help screen",      /* short description */
      "Show this help screen"       /* long description */
    }
  };

  db=GWEN_DB_GetGroup(dbArgs, GWEN_DB_FLAGS_DEFAULT, "local");
  rv=GWEN_Args_Check(argc, argv, 1,
                     0 /*GWEN_ARGS_MODE_ALLOW_FREEPARAM*/,
                     args,
                     db);
  if (rv==GWEN_ARGS_RESULT_ERROR) {
    fprintf(stderr, "ERROR: Could not parse arguments\n");
    return 1;
  }
  else if (rv==GWEN_ARGS_RESULT_HELP) {
    GWEN_BUFFER *ubuf;

    ubuf=GWEN_Buffer_new(0, 1024, 0, 1);
    if (GWEN_Args_Usage(args, ubuf, GWEN_ArgsOutType_Txt)) {
      fprintf(stderr, "ERROR: Could not create help string\n");
      return 1;
    }
    fprintf(stdout, "%s\n", GWEN_Buffer_GetStart(ubuf));
    GWEN_Buffer_free(ubuf);
    return 0;
  }

  withAuthKey=GWEN_DB_GetIntValue(db, "authKey", 0, 0);

  /* doit */
  uid=(uint32_t) GWEN_DB_GetIntValue(db, "userId", 0, 0);
  if (uid==0) {
    fprintf(stderr, "ERROR: Invalid or missing unique user id\n");
    return 1;
  }

  rv=AB_Provider_HasUser(pro, uid);
  if (rv<0) {
    fprintf(stderr, "ERROR: User with id %lu not found\n", (unsigned long int) uid);
    return 2;
  }
  rv=AB_Provider_GetUser(pro, uid, 1, 1, &u);
  if (rv<0) {
    fprintf(stderr, "ERROR: User with id %lu not found\n", (unsigned long int) uid);
    return 2;
  }
  else {
    AB_IMEXPORTER_CONTEXT *ctx;

    ctx=AB_ImExporterContext_new();
    rv=AH_Provider_SendUserKeys2(pro, u, ctx, withAuthKey, 1, 0, 1);
    AB_ImExporterContext_free(ctx);
    if (rv<0) {
      DBG_ERROR(0, "Error getting system id (%d)", rv);
      AB_User_free(u);
      return 3;
    }
    fprintf(stderr, "Key(s) sent.\n");
  }
  AB_User_free(u);

  return 0;
}




