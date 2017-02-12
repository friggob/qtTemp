#include "debugform.h"
#include "ui_debugform.h"
#include <QDebug>

debugForm::debugForm(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::debugForm)
{
  ui->setupUi(this);

  addAction(ui->actionQuit);

  //QObject::connect(parent,SIGNAL(hInfoChanged(hInfo)),this,SLOT(setData(hInfo)));

}

debugForm::~debugForm()
{
  delete ui;
}

void debugForm::setData(hInfo hosts){
  qDebug() << "Setting data";
  ui->hMonLabel->setText(hosts.hMon);
  ui->hTempLabel->setText(hosts.hTemp);
  ui->pMonLabel->setText(QString::number(hosts.pMon));
  ui->pTempLabel->setText(QString::number(hosts.pTemp));
}

void debugForm::on_actionQuit_triggered()
{
  close();
}
