#include "hostdialog.h"
#include "mainwindow.h"
#include "ui_hostdialog.h"

hostDialog::hostDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::hostDialog)
{
  ui->setupUi(this);
}

hostDialog::~hostDialog()
{
  delete ui;
}

void hostDialog::setHosts(hInfo hosts){
  h = hosts;
  ui->hPiEdit->setText(h.hMon);
  ui->pPiEdit->setText(QString::number(h.pMon));
  ui->hTempEdit->setText(h.hTemp);
  ui->pTempEdit->setText(QString::number(h.pTemp));
}

void hostDialog::on_buttonBox_accepted()
{
  QString port;
  QString host;

  if((port = ui->pPiEdit->text()) != ""){
	if(port.toInt() != 0){
	  h.pMon = port.toInt();
	}
  }
  if((port = ui->pTempEdit->text()) != ""){
	if(port.toInt() != 0){
	  h.pTemp = port.toInt();
	}
  }
  if((host = ui->hPiEdit->text()) != ""){
	h.hMon = host;
  }
  if((host = ui->hTempEdit->text()) != ""){
	h.hTemp = host;
  }
  emit hostsChanged(h);
  this->close();
}

void hostDialog::on_buttonBox_rejected()
{
	this->close();
}
