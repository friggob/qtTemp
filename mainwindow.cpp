#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QTextStream>
#include <QThread>
#include <QDebug>
#include <QFile>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  QTimer *timer = new QTimer(this);
  connect(timer,SIGNAL(timeout()), this, SLOT(getTempNet()));
  connect(timer,SIGNAL(timeout()), this, SLOT(testNet()));
  timer->start(30 * 1000);

  cSettings = new QSettings("JFO Soft","qtTemp");

  hd = new hostDialog(this);
  QObject::connect(hd,SIGNAL(hostsChanged(hInfo)),this,SLOT(updateHosts(hInfo)));

  df = new debugForm(this);
  QObject::connect(this,SIGNAL(hInfoChanged(hInfo)),df,SLOT(setData(hInfo)));

  ui->setupUi(this);

  setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);

  dp = ui->label->palette();
  ep = dp;
  ep.setColor(QPalette::Background, QColor("#FFCCCC"));
  dp.setColor(QPalette::Background, QColor("#CCFFCC"));

  hi.hMon = "www.google.se";
  hi.pMon = 80;
  hi.hTemp = "z.jfo.im";
  hi.pTemp = 1313;

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

  QAction *s = new QAction(this);
  QAction *s1 = new QAction(this);
  QAction *s2 = new QAction(this);
  s->setSeparator(true);
  s1->setSeparator(true);
  s2->setSeparator(true);

  addAction(ui->action_Update);
  addAction(ui->actionSet_host);
  addAction(s1);
  addAction(ui->action_Change);
  addAction(ui->actionSavecfg);
  addAction(ui->actionPrint);
  addAction(s);
  addAction(ui->action_About);
  addAction(s2);
  addAction(ui->action_Quit);

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

/*
void MainWindow::get_temp()
{
  QFile file("/home/fredrik/tmp/last-temp.txt");

  if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
	temp = "-99";

  QTextStream inf(&file);

  double nt = inf.readLine().toDouble();

  temp = QString().asprintf("% 2.2f",nt);

  qDebug() << "Temp is" << temp;

  setTemp();

  file.close();
}
*/

void MainWindow::on_action_Quit_triggered()
{
  close();
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
  hd->setHosts(hi);
  hd->show();
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
  qDebug() << "hPi =" << hi.hMon;
  qDebug() << "pPi =" << hi.pMon;
  qDebug() << "hTemp =" << hi.hTemp;
  qDebug() << "pTemp =" << hi.pTemp;
  df->show();
  emit hInfoChanged(hi);
}

void MainWindow::on_action_About_triggered()
{
  QString Message;
  Message = "Temp version " + version;
  Message += "\nCreated by gh@jfo.im\n";
  QMessageBox::about(NULL,"About Temp",Message);
}
