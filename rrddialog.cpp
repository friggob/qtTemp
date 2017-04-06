#include "rrddialog.h"
#include "ui_rrddialog.h"
#include <QFileDialog>
#include <QDebug>

rrdDialog::rrdDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::rrdDialog)
{
  ui->setupUi(this);
}

rrdDialog::~rrdDialog()
{
  delete ui;
}

void rrdDialog::setRrdPath(QString rp){
  rrdPath = rp;
  ui->rrdPathEdit->setText(rp);
}

void rrdDialog::setRrdCmdPath(QString rcp){
  rrdCmdPath = rcp;
  ui->rrdCmdPathEdit->setText(rcp);
}

void rrdDialog::on_rrdPathButton_clicked()
{
  rrdPath = QFileDialog::getOpenFileName(this,tr("Open rrd file"),"",tr("Rrd files (*rrd);;Any files (*)"));
  qDebug() << rrdPath;
  ui->rrdPathEdit->setText(rrdPath);
}

void rrdDialog::on_rrdCmdPathButton_clicked()
{
  rrdCmdPath = QFileDialog::getOpenFileName(this,tr("Open rrdtool"),"",tr("Rrdtool (*)"));
  qDebug() << rrdCmdPath;
  ui->rrdCmdPathEdit->setText(rrdCmdPath);
}

void rrdDialog::on_buttonBox_accepted()
{
  emit rrdConfigChanged(rrdPath,rrdCmdPath);
  this->close();
}

void rrdDialog::on_buttonBox_rejected()
{
  this->close();
}
