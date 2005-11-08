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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif


#include "accountview.h"
#include "editaccount.h"

#include <qevent.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qmessagebox.h>
#include <qlayout.h>

#include <aqdtaus/provider.h>
#include <aqdtaus/account.h>
#include <gwenhywfar/debug.h>

#ifdef WIN32
# define strcasecmp stricmp
#endif


#define BUTTON_WIDTH 110


AccountView::AccountView(QBanking *app,
                         AB_PROVIDER *pro,
                         QWidget* parent,
                         const char* name,
                         WFlags fl)
:AccountViewUi(parent, name, fl), _app(app), _provider(pro) {
  assert(pro);
  QObject::connect(newButton, SIGNAL(clicked()),
                   this, SLOT(slotNew()));
  QObject::connect(editButton, SIGNAL(clicked()),
                   this, SLOT(slotEdit()));
  QObject::connect(removeButton, SIGNAL(clicked()),
                   this, SLOT(slotRemove()));
  QObject::connect(closeButton, SIGNAL(clicked()),
                   this, SLOT(close()));
}



AccountView::~AccountView(){
}



void AccountView::update(){
  AB_ACCOUNT_LIST2 *al;

  accountListView->clear();

  al=AD_Provider_GetAccounts(_provider);
  if (al)
    accountListView->addAccounts(al);
}


bool AccountView::init(){
  GWEN_DB_NODE *db;

  db=_app->getAppData();
  assert(db);
  db=GWEN_DB_GetGroup(db, GWEN_PATH_FLAGS_NAMEMUSTEXIST,
                      "gui/views/accountview/dynamic");
  if (db) {
    int i, j;
    const char *p;

    p=GWEN_DB_GetCharValue(db, "sortOrder", 0, "ascending");
    if (p) {
      if (strcasecmp(p, "ascending")==0)
        accountListView->setSortOrder(Qt::Ascending);
      else
        if (strcasecmp(p, "descending")==0)
          accountListView->setSortOrder(Qt::Descending);
    }
    i=GWEN_DB_GetIntValue(db, "sortColumn", 0, -1);
    if (i!=-1)
      accountListView->setSortColumn(i);

    /* found settings */
    for (i=0; i<accountListView->columns(); i++) {
      accountListView->setColumnWidthMode(i, QListView::Manual);
      j=GWEN_DB_GetIntValue(db, "columns", i, -1);
      if (j!=-1)
        accountListView->setColumnWidth(i, j);
    } /* for */
  } /* if settings */

  update();
  return true;
}



bool AccountView::fini(){
  GWEN_DB_NODE *db;
  int i, j;

  db=_app->getAppData();
  assert(db);
  db=GWEN_DB_GetGroup(db,
                      GWEN_DB_FLAGS_OVERWRITE_GROUPS,
                      "gui/views/accountview/dynamic");

  switch(accountListView->sortOrder()) {
  case Qt::Ascending:
    GWEN_DB_SetCharValue(db, GWEN_DB_FLAGS_DEFAULT,
                         "sortOrder", "ascending");
    break;
  case Qt::Descending:
    GWEN_DB_SetCharValue(db, GWEN_DB_FLAGS_DEFAULT,
                         "sortOrder", "descending");
    break;
  default:
    break;
  }

  for (i=0; i<accountListView->columns(); i++) {
    j=accountListView->columnWidth(i);
    GWEN_DB_SetIntValue(db, GWEN_DB_FLAGS_DEFAULT,
                        "columns", j);
  } /* for */

  return true;
}



void AccountView::slotNew(){
  AB_ACCOUNT *a;

  a=AD_Account_new(_app->getCInterface(),
                   _provider, 0);

  EditAccount ea(_app, a, true, this, "EditAccount", true);
  ea.init();
  for(;;) {
    if (ea.exec()!=QDialog::Accepted) {
      AB_Account_free(a);
      return;
    }
    else {
      if (AD_Provider_HasAccount(_provider,
                                 AB_Account_GetBankCode(a),
                                 AB_Account_GetAccountNumber(a))){
        int r = QMessageBox::critical(this,
                                  tr("New Account"),
                                  tr("This account already exists.\n"
                                     "Please check your input."),
                                  QMessageBox::Retry,QMessageBox::Abort);
	if (r !=0 && r != QMessageBox::Retry) {
          AB_Account_free(a);
          return;
        }
      }
      else {
        DBG_ERROR(0, "Adding account");
        AD_Provider_AddAccount(_provider, a);
        update();
        return;
      }
    }
  } /* for */
}



void AccountView::slotEdit(){
  AB_ACCOUNT *a;

  /* get my account */
  a=accountListView->getCurrentAccount();
  if (!a) {
    DBG_NOTICE(0, "No account selected");
    QMessageBox::warning(this,
                         tr("No Selection"),
                         tr("Please select an account first."),
                         QMessageBox::Ok,QMessageBox::NoButton);
    return;
  }

  EditAccount ea(_app, a, false, this, "EditAccount", true);
  ea.init();
  if (ea.exec()==QDialog::Accepted) {
    update();
  }
}



void AccountView::slotRemove(){
  AB_ACCOUNT *a;

  /* get my account */
  a=accountListView->getCurrentAccount();
  if (!a) {
    DBG_NOTICE(0, "No account selected");
    QMessageBox::warning(this,
                         tr("No Selection"),
                         tr("Please select an account first."),
                         QMessageBox::Ok,QMessageBox::NoButton);
    return;
  }
  int r = QMessageBox::warning(this,
                           tr("Remove Account"),
                           tr("Are you sure you want to remove "
                              "the selcted account?"),
                           QMessageBox::Yes,QMessageBox::No);
  if (r !=0 && r != QMessageBox::Yes)
    return;

  AD_Provider_RemoveAccount(_provider, a);
  update();
}




