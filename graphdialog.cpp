#include "graphdialog.h"
#include "ui_graphdialog.h"

#include <QDebug>
#include <QDateTime>
#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QFont>
#include <QTimeZone>
#include <QMessageBox>

#define NOW QDateTime::currentDateTime().toTime_t()
#define SNOW(X) QDateTime::currentDateTime().addSecs(X).toTime_t()
#define DNOW(X) QDateTime::currentDateTime().addDays(X).toTime_t()
#define MNOW(X) QDateTime::currentDateTime().addMonths(X).toTime_t()
#define YNOW(X) QDateTime::currentDateTime().addYears(X).toTime_t()
#define DFROM ui->dateTimeEditFrom->dateTime()
#define DTO ui->dateTimeEditTo->dateTime()

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
  ui->setupUi(this);
  addAction(ui->actionClose);
  QGraphicsScene *sc = new QGraphicsScene(ui->graphicsView);
  ui->graphicsView->setScene(sc);
  connect(this,SIGNAL(pixmapChanged()),this,SLOT(updateView()));
  connect(this,SIGNAL(rrdpathChanged()),this,SLOT(createPixmap()));
  connect(this,SIGNAL(offsetChanged()),this,SLOT(createPixmap()));
  ui->hourButton->setChecked(true);
  setDateTimeEdit(SNOW(-3600),
				  NOW);

}

void graphDialog::setDateTimeEdit(qint64 secFrom, qint64 secTo){
  ui->dateTimeEditTo->setDateTime(QDateTime::fromTime_t(secTo,QTimeZone::systemTimeZone()));
  ui->dateTimeEditFrom->setDateTime(QDateTime::fromTime_t(secFrom,QTimeZone::systemTimeZone()));
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
  QDateTime fTime,tTime;
  QByteArray ret;

  //t.setTime_t(QDateTime::currentDateTime().toTime_t()-offset);
  fTime = DFROM;
  tTime = DTO;

  rrdCmdArgs << "graph" << "-"
#ifndef Q_OS_WIN
			 << "--border" << "0"
			 << "-n" << "DEFAULT:8:"+rrdFont
			 << "-E"
#endif
			 << "-w" << "559"
			 << "-h" << "250"
			 << "-s" << QString::number(fTime.toTime_t())
			 << "-e" << QString::number(tTime.toTime_t())
			 << "DEF:temp="+fi.fileName()+":Temp:AVERAGE"
			 << "VDEF:tempmax=temp,MAXIMUM"
			 << "VDEF:tempmin=temp,MINIMUM"
			 << "VDEF:tempcur=temp,LAST"
			 << "LINE1:tempmax#00FFFF:Max\\:  "
			 << "GPRINT:tempmax:% 2.2lf°C\\t"
			 << "LINE1:tempmin#FF00FF:Min\\:  "
			 << "GPRINT:tempmin:% 2.2lf°C\\n"
			 << "LINE1:temp#0000FF:Last\\: "
			 << "GPRINT:tempcur:% 2.2lf°C\\n"
			 << "HRULE:0#000000";

  qDebug() << rrdCmdArgs;

  QDir::setCurrent(rrdDir.absolutePath());
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LC_TIME","sv_SE.UTF-8");

  QProcess *proc = new QProcess(this);

  proc->setProcessEnvironment(env);
  proc->setProgram(rrdCmd);
  proc->setArguments(rrdCmdArgs);
  proc->start(QIODevice::ReadOnly);

  proc->setReadChannel(QProcess::StandardOutput);

  while(proc->waitForReadyRead()){
	ret += proc->readAll();
	//qDebug() << "ret.count:" << ret.count();
  }

  qpm->loadFromData(ret);

  //qDebug() << "qpm.size:" << qpm->size();

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

void graphDialog::setRrdFont(QString f)
{
  rrdFont = f;
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
  QFont f(rrdFont);
  QDialog::showEvent(event);
  ui->fontComboBox->setCurrentFont(f);
  this->createPixmap();
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
  qint64 offset;
  if(checked){
	offset = ui->spinBox->value() * hour;
	setDateTimeEdit(SNOW(-offset),NOW);
	emit offsetChanged();
  }
}

void graphDialog::on_dayButton_clicked(bool checked)
{
  if(checked){
	setDateTimeEdit(DNOW(-1),NOW);
	emit offsetChanged();
  }
}

void graphDialog::on_weekButton_clicked(bool checked)
{
  if(checked){
	setDateTimeEdit(DNOW(-7),NOW);
	emit offsetChanged();
  }
}

void graphDialog::on_monthButton_clicked(bool checked)
{
  if(checked){
	setDateTimeEdit(MNOW(-1),NOW);
	emit offsetChanged();
  }
}

void graphDialog::on_yearButton_clicked(bool checked)
{
  if(checked){
	setDateTimeEdit(YNOW(-1),NOW);
	emit offsetChanged();
  }
}

void graphDialog::on_spinBox_valueChanged(int arg1)
{
  qint64 offset;
  if(ui->hourButton->isChecked()){
	offset = arg1 * hour;
	setDateTimeEdit(SNOW(-offset),NOW);
	emit offsetChanged();
  }
}

void graphDialog::on_fontComboBox_activated(const QString &arg1)
{
  qDebug() << arg1;
#ifdef Q_OS_WIN
  return;
#endif
  rrdFont = arg1;
  emit createPixmap();
  emit rrdFontChanged(arg1);
}

void graphDialog::on_pushButton_clicked()
{
  if(DFROM > DTO){
	qDebug() << "Error! From cannot be later than To!";
  }else{
	emit offsetChanged();
  }
}

void graphDialog::on_dateTimeEditTo_dateTimeChanged(const QDateTime &dateTime)
{
  if(dateTime < DFROM){
	QMessageBox::critical(this,"Wrong date!","To date cannot be earlier than From date!");
	ui->dateTimeEditTo->setDateTime(DFROM);
  }else{
	emit offsetChanged();
  }
}

void graphDialog::on_dateTimeEditFrom_dateTimeChanged(const QDateTime &dateTime)
{
  if(dateTime > DTO){
	QMessageBox::critical(this,"Wrong date!","From date cannot be later than To date!");
	ui->dateTimeEditFrom->setDateTime(DTO);
  }else{
	emit offsetChanged();
  }
}
