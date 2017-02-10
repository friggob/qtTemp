#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QTextStream>
#include <QThread>
#include <QDebug>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  QTimer *timer = new QTimer(this);
  connect(timer,SIGNAL(timeout()), this, SLOT(getTempNet()));
  connect(timer,SIGNAL(timeout()), this, SLOT(testNet()));
  timer->start(30 * 1000);

  cSettings = new QSettings("JFO Soft","qtTemp");

  QAction *s = new QAction(this);
  QAction *s1 = new QAction(this);
  s->setSeparator(true);
  s1->setSeparator(true);

  ui->setupUi(this);

  setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);

  dp = ui->label->palette();
  ep = dp;
  ep.setColor(QPalette::Background, QColor("#FFCCCC"));
  dp.setColor(QPalette::Background, QColor("#CCFFCC"));
  hPi = "www.google.se";
  pPi = 80;
  hTemp = "z.jfo.im";
  pTemp = 1313;

  addAction(ui->action_Update);
  addAction(ui->actionSet_host);
  addAction(s1);
  addAction(ui->action_Change);
  addAction(ui->actionSavecfg);
  addAction(ui->actionPrint);
  addAction(s);
  addAction(ui->action_Quit);

  readConfig();
  getTempNet();
  testNet();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::readConfig(){
  QVariant sValue;

  cSettings->sync();
  cSettings->beginGroup("net");

  qDebug() << cSettings->fileName();

  if((sValue = cSettings->value("hPi")).isValid()){
	hPi = sValue.toString();
	qDebug() << "Setting hPi to" << hPi;
  }
  if((sValue = cSettings->value("pPi")).isValid()){
	pPi = sValue.toInt();
	qDebug() << "Setting pPi to" << pPi;
  }
  if((sValue = cSettings->value("hTemp")).isValid()){
	hTemp = sValue.toString();
	qDebug() << "Setting hTemp to" << hTemp;
  }
  if((sValue = cSettings->value("pTemp")).isValid()){
	pTemp = sValue.toInt();
	qDebug() << "Setting pTemp to" << pTemp;
  }
  cSettings->endGroup();
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

  socket->connectToHost(hTemp, pTemp);

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

  s->connectToHost(hPi,pPi);

  if(s->waitForConnected(3000)){
	qDebug() << "Pi is up!";
	setPalette(dp);
	s->close();
  }else{
	qDebug() << "Pi is Down!";
	setPalette(ep);
  }
}

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

void MainWindow::updatepPi(int port){
  pPi = port;
}

void MainWindow::updatepTemp(int port){
  pTemp = port;
}

void MainWindow::updatehPi(const QString host){
  hPi = host;
}

void MainWindow::updatehTemp(const QString host){
  hTemp = host;
}

void MainWindow::on_actionSet_host_triggered()
{
  hd = new hostDialog(this);
  QObject::connect(hd,SIGNAL(pPiChanged(int)),this,SLOT(updatepPi(int)));
  QObject::connect(hd,SIGNAL(pTempChanged(int)),this,SLOT(updatepTemp(int)));
  QObject::connect(hd,SIGNAL(hTempChanged(QString)),this,SLOT(updatehTemp(QString)));
  QObject::connect(hd,SIGNAL(hPiChanged(QString)),this,SLOT(updatehPi(QString)));

  hd->sethPi(hPi);
  hd->setpPi(pPi);
  hd->sethTemp(hTemp);
  hd->setpTemp(pTemp);

  hd->show();
  qDebug() << "Set host closed";
}

void MainWindow::on_actionSavecfg_triggered()
{
  cSettings->sync();
  cSettings->beginGroup("net");

  cSettings->setValue("hPi",hPi);
  cSettings->setValue("pPi",pPi);
  cSettings->setValue("hTemp",hTemp);
  cSettings->setValue("pTemp",pTemp);

  cSettings->endGroup();
  cSettings->sync();
}

void MainWindow::on_actionPrint_triggered()
{
  qDebug() << "hPi =" << hPi;
  qDebug() << "pPi =" << pPi;
  qDebug() << "hTemp =" << hTemp;
  qDebug() << "pTemp =" << pTemp;
}
