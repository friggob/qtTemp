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

void hostDialog::sethPi(const QString host){
  ui->hPiEdit->setText(host);
}

void hostDialog::setpPi(const int port){
  ui->pPiEdit->setText(QString::number(port));
}

void hostDialog::sethTemp(const QString host){
  ui->hTempEdit->setText(host);
}

void hostDialog::setpTemp(const int port){
  ui->pTempEdit->setText(QString::number(port));
}

void hostDialog::on_buttonBox_accepted()
{
  QString port;
  QString host;

  if((port = ui->pPiEdit->text()) != ""){
	if(port.toInt() != 0){
	  emit pPiChanged(port.toInt());
	}
  }
  if((port = ui->pTempEdit->text()) != ""){
	if(port.toInt() != 0){
	  emit pTempChanged(port.toInt());
	}
  }
  if((host = ui->hPiEdit->text()) != ""){
	emit hPiChanged(host);
  }
  if((host = ui->hTempEdit->text()) != ""){
	emit hTempChanged(host);
  }
  this->close();
}

void hostDialog::on_buttonBox_rejected()
{
	this->close();
}

void hostDialog::on_hPiEdit_returnPressed()
{
  return;
}
