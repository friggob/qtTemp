#include "debugform.h"
#include "ui_debugform.h"
#include <QDebug>

debugForm::debugForm(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::debugForm)
{
  ui->setupUi(this);

  addAction(ui->actionQuit);

}

debugForm::~debugForm()
{
  delete ui;
}

void debugForm::showEvent(QShowEvent *event){
  QListWidget *lw;
  QString t;
  QString key;
  lw = ui->listWidget;

  QDialog::showEvent(event);

  foreach (key, settings->allKeys()) {
	//t = key + ": " + settings->value(key).toString();
	t =  QString("%1: %2").arg(key,-15).arg(settings->value(key).toString());
	lw->addItem(t);
  }

  lw->show();
}

void debugForm::setData(QString org, QString app){
  settings = new QSettings(org,app);
}

void debugForm::on_actionQuit_triggered()
{
  close();
}
