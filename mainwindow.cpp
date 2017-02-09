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

  QAction *s = new QAction(this);
  s->setSeparator(true);

  ui->setupUi(this);

  setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
  dp = ui->label->palette();
  ep = dp;
  ep.setColor(QPalette::Background, QColor("#FFCCCC"));
  dp.setColor(QPalette::Background, QColor("#CCFFCC"));
  hPi = "10.10.10.10";
  hTemp = "z.jfo.im";

  addAction(ui->action_Update);
  addAction(ui->action_Change);
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
  QSettings *cSettings = new QSettings("JFO Soft","qtTemp");
  cSettings->sync();
  cSettings->beginGroup("net");

  if(cSettings->value("hPi").isValid()){
	hPi = cSettings->value("hPi").toString();
	qDebug() << "Setting hPi to" << hPi;
  }
  if(cSettings->value("hTemp").isValid()){
	hTemp = cSettings->value("hTemp").toString();
	qDebug() << "Setting hTemp to" << hTemp;
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

  socket->connectToHost(hTemp, 1313);

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

  s->connectToHost(hPi,22);

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
