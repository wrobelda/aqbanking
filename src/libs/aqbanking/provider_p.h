/***************************************************************************
 $RCSfile$
 -------------------
 cvs         : $Id$
 begin       : Mon Mar 01 2004
 copyright   : (C) 2004 by Martin Preuss
 email       : martin@libchipcard.de

 ***************************************************************************
 *          Please see toplevel file COPYING for license details           *
 ***************************************************************************/


#ifndef AQBANKING_PROVIDER_P_H
#define AQBANKING_PROVIDER_P_H

#include "provider_l.h"
#include <aqbanking/banking.h>
#include <gwenhywfar/misc.h>
#include <gwenhywfar/inherit.h>
#include <gwenhywfar/misc2.h>
#include <gwenhywfar/libloader.h>



struct AB_PROVIDER {
  GWEN_INHERIT_ELEMENT(AB_PROVIDER);
  GWEN_LIST_ELEMENT(AB_PROVIDER);

  AB_BANKING *banking;
  char *name;

  AB_PROVIDER_UPDATEJOB_FN updateJobFn;
  AB_PROVIDER_ADDJOB_FN addJobFn;
  AB_PROVIDER_EXECUTE_FN executeFn;
  AB_PROVIDER_GETACCOUNTLIST_FN getAccountListFn;
  AB_PROVIDER_UPDATEACCOUNT_FN updateAccountFn;
  AB_PROVIDER_ADDACCOUNT_FN addAccountFn;
  AB_PROVIDER_IMPORTTRANSACTIONS_FN importTransactionsFn;
  GWEN_LIBLOADER *libLoader;
  GWEN_TYPE_UINT32 usage;
};



struct AB_PROVIDER_WIZARD {
  GWEN_INHERIT_ELEMENT(AB_PROVIDER_WIZARD);
  GWEN_LIST_ELEMENT(AB_PROVIDER_WIZARD);

  AB_PROVIDER *provider;
  char *name;

  AB_PROVIDER_WIZARD_SETUP_FN setupFn;

  GWEN_LIBLOADER *libLoader;
};


#endif /* AQBANKING_PROVIDER_P_H */
