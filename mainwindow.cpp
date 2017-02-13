#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QTextStream>
#include <QThread>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QToolTip>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  QTimer *timer = new QTimer(this);
  cSettings = new QSettings("JFO Soft","qtTemp");
  hd = new hostDialog(this);
  df = new debugForm(this);

  connect(hd,SIGNAL(hostsChanged(hInfo)),this,SLOT(updateHosts(hInfo)));
  connect(this,SIGNAL(hInfoChanged(hInfo)),df,SLOT(setData(hInfo)));
  connect(timer,SIGNAL(timeout()), this, SLOT(getTempNet()));
  connect(timer,SIGNAL(timeout()), this, SLOT(testNet()));

  timer->start(30 * 1000);

  ui->setupUi(this);

  setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint | Qt::ToolTip);

  dp = ui->label->palette();
  ep = dp;
  ep.setColor(QPalette::Background, QColor("#FFCCCC"));
  dp.setColor(QPalette::Background, QColor("#CCFFCC"));

  createMenu();
  readConfig();
  getTempNet();
  testNet();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::createMenu(){

  QList<QAction*> a;

  QAction *s = new QAction(this);
  QAction *s1 = new QAction(this);
  QAction *s2 = new QAction(this);
  s->setSeparator(true);
  s1->setSeparator(true);
  s2->setSeparator(true);

  ui->actionOnTop->setChecked(true);

  a += ui->action_Update;
  a += ui->actionSet_host;
  a += ui->actionOnTop;
  a += s;
  a += ui->action_Change;
  a += ui->actionSavecfg;
  a += ui->actionPrint;
  a += s1;
  a += ui->action_About;
  a += s2;
  a += ui->action_Quit;

  addActions(a);
}

void MainWindow::readConfig(){
  QVariant sValue;

  cSettings->sync();
  cSettings->beginGroup("net");

  qDebug() << cSettings->fileName();

  if((sValue = cSettings->value("hMon")).isValid()){
	hi.hMon = sValue.toString();
	qDebug() << "Setting hPi to" << hi.hMon;
  }
  if((sValue = cSettings->value("pMon")).isValid()){
	hi.pMon = sValue.toInt();
	qDebug() << "Setting pPi to" << hi.pMon;
  }
  if((sValue = cSettings->value("hTemp")).isValid()){
	hi.hTemp = sValue.toString();
	qDebug() << "Setting hTemp to" << hi.hTemp;
  }
  if((sValue = cSettings->value("pTemp")).isValid()){
	hi.pTemp = sValue.toInt();
	qDebug() << "Setting pTemp to" << hi.pTemp;
  }
  cSettings->endGroup();

  emit hInfoChanged(hi);
}

void MainWindow::setTemp(){
  if(temp == "-99"){
	temp = last_temp;
  }else{
	last_temp = temp;
  }

  ui->label->setText(temp);
}

void MainWindow::getTempNet(){
  QByteArray data;
  double nt;

  socket = new QTcpSocket(this);

  socket->connectToHost(hi.hTemp, hi.pTemp);

  if(socket->waitForConnected(3000)){
	qDebug() << "Connected!";

	socket->waitForReadyRead(3000);
	qDebug() << "Reading: " << socket->bytesAvailable();

	data = socket->readAll();

	nt = QString::fromLatin1(data.data()).toDouble();

	temp = QString::asprintf("% 2.2f",nt);

	qDebug() << temp;

	socket->close();
  }
  else{
	temp = "-99";
	qDebug() << "Not connected!";
  }
  setTemp();
}

void MainWindow::testNet(){
  QTcpSocket *s = new QTcpSocket(this);

  setAutoFillBackground(true);

  s->connectToHost(hi.hMon,hi.pMon);

  if(s->waitForConnected(3000)){
	qDebug() << "Pi is up!";
	setPalette(dp);
	s->close();
  }else{
	qDebug() << "Pi is Down!";
	setPalette(ep);
  }
}

void MainWindow::on_action_Quit_triggered()
{
  df->close();
  hd->close();
  close();
  QApplication::quit();
}

void MainWindow::on_action_Update_triggered()
{
  emit getTempNet();
  emit testNet();
}

void MainWindow::on_action_Change_triggered()
{
  readConfig();
  emit testNet();
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
  m_nMouseClick_X_Coordinate = event->x();
  m_nMouseClick_Y_Coordinate = event->y();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
  move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}

void MainWindow::updateHosts(hInfo hosts){
  hi = hosts;
  emit hInfoChanged(hi);
  qDebug() << "Hosts updated";
}

void MainWindow::on_actionSet_host_triggered()
{
  QPoint p;

  p.setX(this->x());
  p.setY(this->y()+this->height());

  hd->setHosts(hi);

  hd->show();
  hd->move(p);
}

void MainWindow::on_actionSavecfg_triggered()
{
  cSettings->sync();
  cSettings->beginGroup("net");

  cSettings->setValue("hMon",hi.hMon);
  cSettings->setValue("pMon",hi.pMon);
  cSettings->setValue("hTemp",hi.hTemp);
  cSettings->setValue("pTemp",hi.pTemp);

  cSettings->endGroup();
  cSettings->sync();
}

void MainWindow::on_actionPrint_triggered()
{
  QPoint p;

  p.setX(this->x()-df->width());
  p.setY(this->y());

  df->show();
  df->move(p);

  emit hInfoChanged(hi);
}

void MainWindow::on_action_About_triggered()
{
  QString Message;
  Message = "Temp version v" + version->toString();
  Message += "\n\nCreated by:\nFredrik Olausson <fredrik@bluppfisk.org>\n";
  QMessageBox::about(this,"About Temp",Message);
}

void MainWindow::on_actionOnTop_triggered()
{
  if(ui->actionOnTop->isChecked()){
	this->setWindowFlags(this->windowFlags()|Qt::ToolTip);
	this->show();
  }else{
	this->setWindowFlags(this->windowFlags()^Qt::ToolTip);
	this->show();
  }
}
