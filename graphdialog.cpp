#include "graphdialog.h"
#include "ui_graphdialog.h"

#include <QDebug>
#include <QDateTime>
#include <QProcess>
#include <QFileInfo>
#include <QDir>

graphDialog::graphDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::graphDialog),
  qpm(new QPixmap())
{
#ifndef Q_OS_WIN
  rrdCmd = "rrdtool";
#else
  rrdCmd = "rrdtool.exe";
#endif
  offset = hour;
  ui->setupUi(this);
  addAction(ui->actionClose);
  QGraphicsScene *sc = new QGraphicsScene(ui->graphicsView);
  ui->graphicsView->setScene(sc);
  connect(this,SIGNAL(pixmapChanged()),this,SLOT(updateView()));
  connect(this,SIGNAL(rrdpathChanged()),this,SLOT(createPixmap()));
  connect(this,SIGNAL(offsetChanged()),this,SLOT(createPixmap()));
  ui->hourButton->setChecked(true);
}

void graphDialog::createPixmap(){
  if(rrdPath.isNull() || rrdPath.isEmpty()){
	return;
  }

  QFileInfo fi(rrdPath);
  if(!fi.exists()){
	  return;
  }

  QDir rrdDir = fi.absoluteDir();
  QDir oldDir;

  QStringList rrdCmdArgs;
  QDateTime t;
  QByteArray ret;

  t.setTime_t(QDateTime::currentDateTime().toTime_t()-offset);


  rrdCmdArgs << "graph" << "-"
			 <<"-S" << "60"
			<< "-w" << "559"
			<< "-h" << "250"
			<< "-s" << QString::number(t.toTime_t())
			<< "-e" << "now"
			<< "DEF:temp="+fi.fileName()+":Temp:AVERAGE"
			<< "LINE1:temp#0000FF"
			<< "LINE1:0#FF0000";

  qDebug() << rrdCmdArgs;

  QDir::setCurrent(rrdDir.absolutePath());
  QProcess *proc = new QProcess(this);
  proc->setProgram(rrdCmd);
  proc->setArguments(rrdCmdArgs);
  proc->start(QIODevice::ReadOnly);

  proc->setReadChannel(QProcess::StandardOutput);

  while(proc->waitForReadyRead()){
	ret += proc->readAll();
	qDebug() << "ret.count:" << ret.count();
  }

  qpm->loadFromData(ret);

  qDebug() << "qpm.size:" << qpm->size();

  emit pixmapChanged();

  proc->close();
  delete proc;
  QDir::setCurrent(oldDir.absolutePath());
}

void graphDialog::setRrdPath(QString p){
  rrdPath = p;
  emit rrdpathChanged();
}

void graphDialog::setRrdCmd(QString p){
  rrdCmd = p;
}

void graphDialog::updateView(){
  if(qpm->isNull()){
	return;
  }
  QGraphicsScene *sc = ui->graphicsView->scene();
  QGraphicsPixmapItem *p = sc->addPixmap(*qpm);
  sc->setSceneRect(qpm->rect());
  ui->graphicsView->fitInView(p,Qt::KeepAspectRatio);
}

void graphDialog::showEvent(QShowEvent *event){
  QDialog::showEvent(event);
  emit updateView();
}

void graphDialog::setPixmap(QPixmap *pm){
  qpm = pm;
  emit pixmapChanged();
}

void graphDialog::resizeEvent(QResizeEvent *event){
  QDialog::resizeEvent(event);
  emit updateView();
}

graphDialog::~graphDialog()
{
  delete ui;
}

void graphDialog::on_actionClose_triggered()
{
  close();
}

void graphDialog::on_hourButton_clicked(bool checked)
{
  if(checked){
	offset = ui->spinBox->value() * hour;
	emit offsetChanged();
  }
}

void graphDialog::on_dayButton_clicked(bool checked)
{
  if(checked){
	offset = day;
	emit offsetChanged();
  }
}

void graphDialog::on_weekButton_clicked(bool checked)
{
  if(checked){
	offset = week;
	emit offsetChanged();
  }
}

void graphDialog::on_monthButton_clicked(bool checked)
{
  if(checked){
	offset = month;
	emit offsetChanged();
  }
}

void graphDialog::on_yearButton_clicked(bool checked)
{
  if(checked){
	offset = year;
	emit offsetChanged();
  }
}

void graphDialog::on_spinBox_valueChanged(int arg1)
{
  if(ui->hourButton->isChecked()){
	offset = arg1 * hour;
	emit offsetChanged();
  }
}
