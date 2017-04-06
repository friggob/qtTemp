#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QTextStream>
#include <QThread>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QToolTip>
#include <QStandardPaths>
#include <QDir>
#include <QProcess>
#include <QDateTime>
#include <QMenu>

#ifdef Q_OS_WIN
#include "windows.h"
#endif

MainWindow::~MainWindow()
{
  delete hd;
  delete df;
  delete gd;
  delete ui;
}

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  version = QVersionNumber::fromString(APP_VERSION);
  QTimer *timer = new QTimer(this);
  cSettings = new QSettings("JFO Soft","qtTemp");
  hd = new hostDialog(this);
  df = new debugForm(this);
  gd = new graphDialog(this);
  rd = new rrdDialog(this);
  mon = true;

  connect(rd,SIGNAL(rrdConfigChanged(QString,QString)),this,SLOT(rrdConfigChanged(QString,QString)));
  connect(gd,SIGNAL(rrdFontChanged(QString)),this,SLOT(setRrdFont(QString)));
  connect(hd,SIGNAL(hostsChanged(hInfo)),this,SLOT(updateHosts(hInfo)));
  //connect(this,SIGNAL(hInfoChanged(hInfo)),df,SLOT(setData(hInfo)));
  connect(timer,SIGNAL(timeout()), this, SLOT(getTempNet()));
  connect(timer,SIGNAL(timeout()), this, SLOT(testNet()));

  timer->start(30 * 1000);

  ui->setupUi(this);

  setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
#ifdef Q_OS_WIN
  SetWindowPos(reinterpret_cast<HWND>(this->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
#endif
  dp = ui->label->palette();
  ep = dp;
  ep.setColor(QPalette::Background, QColor("#FFCCCC"));
  dp.setColor(QPalette::Background, QColor("#CCFFCC"));

  readConfig();
  if(!oldPos.isNull()){
	//qDebug() << "Moving window to" << oldPos;
	this->move(oldPos);
  }
  setupRrd();
  createMenu();
  getTempNet();
  testNet();
}

void MainWindow::rrdConfigChanged(QString rp, QString rcp){
  rrdPath = rp;
  rrdCmdPath = rcp;

  cSettings->sync();
  cSettings->beginGroup("rrd");
  cSettings->setValue("rrdPath",rrdPath);
  cSettings->setValue("rrdCmdPath",rrdCmdPath);
  cSettings->endGroup();
  cSettings->sync();

  gd->setRrdCmd(rrdCmdPath);
  gd->setRrdPath(rrdPath);
}

void MainWindow::setRrdFont(QString f)
{
  rrdFont = f;
  qDebug() << "Setting rrdFont to" << f;
  cSettings->sync();
  cSettings->beginGroup("rrd");
  if(!rrdFont.isEmpty() && !rrdFont.isNull()){
	cSettings->setValue("rrdFont",rrdFont);
  }
  cSettings->endGroup();
  cSettings->sync();
}

void MainWindow::setupRrd(){
  if(rrdPath.isNull() || rrdPath.isEmpty()){
	QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	//qDebug() << "AppDataLocation:" << dir;
	QDir d(dir);
	if(!d.mkpath(d.absolutePath())){
	  qWarning() << "Could not create appDataPath:" << dir;
	}else{
	  rrdPath = d.absoluteFilePath("Temp.rrd");
	  //qDebug() << "rrdPath:" << rrdPath;
	}
  }
  if(rrdCmdPath.isNull() || rrdCmdPath.isEmpty()){
#ifdef Q_OS_WIN
	rrdCmdPath = "rrdtool.exe";
#else
	rrdCmdPath = "rrdtool";
#endif
  }
  gd->setRrdCmd(rrdCmdPath);

  if(rrdFont.isNull() || rrdFont.isEmpty()){
#ifdef Q_OS_WIN
	rrdFont = "C:\\windows\\fonts\\cour.ttf";
#else
	rrdFont = "Liberation Mono";
#endif
  }
  gd->setRrdFont(rrdFont);

  QFileInfo fi(rrdPath);

  if(!fi.exists()){
	QProcess p;
	QStringList rCa;
	QDateTime t;

	rCa << "create" << rrdPath
		<< "-s" << "60"
		<< "-b" << QString::number(QDateTime::currentDateTime().toTime_t()-1)
		<< "DS:Temp:GAUGE:600:U:U"
		<< "RRA:AVERAGE:0.5:1:60"
		<< "RRA:AVERAGE:0.5:1:1440"
		<< "RRA:AVERAGE:0.5:1:10080"
		<< "RRA:AVERAGE:0.5:1:43200"
		<< "RRA:AVERAGE:0.5:1:525600";

	//qDebug() << "rrd tool args:" << rCa;

	p.setProgram(rrdCmdPath);
	p.setArguments(rCa);
	p.start(QIODevice::NotOpen);
	if(!p.waitForFinished(3000)){
	  qWarning() << "rrd database at:" << rrdPath << "could not be created";
	}
  }
  gd->setRrdPath(rrdPath);
}

void MainWindow::updateRrd(QString data){
  QString now = QString::number(QDateTime::currentDateTime().toTime_t());
  QStringList rCa;

  rCa << "update"
	  << rrdPath
	  << now + ":" + data;

  QFileInfo fi(rrdPath);
  if(fi.isFile() && fi.isWritable()){
	QProcess p;
	p.startDetached(rrdCmdPath,rCa);
	//qDebug() << "rrdtool" << rCa;
  }
}

void MainWindow::createMenu(){

  QList<QAction*> a;

  QAction *s = new QAction(this);
  QAction *s1 = new QAction(this);
  QAction *s2 = new QAction(this);
  QAction *confAction = new QAction(this);

  QMenu *confMenu = new QMenu("Config",this);

  a += ui->actionSet_host;
  a += ui->actionRrdConfig;
  a += ui->actionSavecfg;
  a += ui->actionPrint;
  a += ui->action_Change;

  confMenu->addActions(a);

  confAction->setText("Config");
  confAction->setMenu(confMenu);

  a.clear();

  s->setSeparator(true);
  s1->setSeparator(true);
  s2->setSeparator(true);
#ifdef Q_OS_WIN
  ui->actionOnTop->setChecked(true);
#else
  //ui->actionOnTop->setChecked(true);
#endif
  a += ui->actionGraph;
  a += ui->action_Update;
#ifdef Q_OS_WIN
  a += ui->actionOnTop;
#endif
  a += ui->actionMonitor;
  a += s;
  a += confAction;
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

  //qDebug() << cSettings->fileName();

  if((sValue = cSettings->value("hMon")).isValid()){
	hi.hMon = sValue.toString();
	//qDebug() << "Setting hPi to" << hi.hMon;
  }
  if((sValue = cSettings->value("pMon")).isValid()){
	hi.pMon = sValue.toInt();
	//qDebug() << "Setting pPi to" << hi.pMon;
  }
  if((sValue = cSettings->value("hTemp")).isValid()){
	hi.hTemp = sValue.toString();
	//qDebug() << "Setting hTemp to" << hi.hTemp;
  }
  if((sValue = cSettings->value("pTemp")).isValid()){
	hi.pTemp = sValue.toInt();
	//qDebug() << "Setting pTemp to" << hi.pTemp;
  }
  cSettings->endGroup();
  cSettings->beginGroup("main");
  if((sValue = cSettings->value("winPos")).isValid()){
	oldPos = sValue.toPoint();
	//qDebug() << "Setting pos to" << oldPos;
  }
  if((sValue = cSettings->value("monitor")).isValid()){
	mon = sValue.toBool();
	//qDebug() << "Setting mon to" << mon;
  }
  cSettings->endGroup();
  cSettings->beginGroup("rrd");
  if((sValue = cSettings->value("rrdCmdPath")).isValid()){
	rrdCmdPath = sValue.toString();
	//qDebug() << "Setting rrdCmdPath to" << rrdCmdPath;
  }
  if((sValue = cSettings->value("rrdPath")).isValid()){
	rrdPath = sValue.toString();
	//qDebug() << "Setting rrdPath to" << rrdPath;
  }
  if((sValue = cSettings->value("rrdFont")).isValid()){
	rrdFont = sValue.toString();
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

	socket->waitForReadyRead(3000);

	data = socket->readAll();

	updateRrd(QString::fromLatin1(data.data()).trimmed());
	nt = QString::fromLatin1(data.data()).toDouble();

	temp = QString::asprintf("% 2.2f",nt);

	//qDebug() << temp;

	socket->close();
  }
  else{
	temp = "-99";
	qDebug() << "Not connected!";
  }
  setTemp();
}

void MainWindow::testNet(){
  if(!mon){
	return;
  }
  QTcpSocket *s = new QTcpSocket(this);

  setAutoFillBackground(true);

  s->connectToHost(hi.hMon,hi.pMon);

  if(s->waitForConnected(3000)){
	setPalette(dp);
	s->close();
  }else{
	setPalette(ep);
	s->close();
  }
  delete s;
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
  //qDebug() << "Hosts updated";
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
  cSettings->beginGroup("main");
  cSettings->setValue("winPos",this->pos());
  cSettings->setValue("monitor",mon);
  cSettings->endGroup();

  cSettings->beginGroup("rrd");
  if(!rrdCmdPath.isEmpty() && !rrdCmdPath.isNull()){
	cSettings->setValue("rrdCmdPath",rrdCmdPath);
  }
  if(!rrdPath.isEmpty() && !rrdPath.isNull()){
	cSettings->setValue("rrdPath",rrdPath);
  }
  if(!rrdFont.isEmpty() && !rrdFont.isNull()){
	cSettings->setValue("rrdFont",rrdFont);
  }
  cSettings->endGroup();

  cSettings->sync();
}

void MainWindow::on_actionPrint_triggered()
{
  QPoint p;

  p.setX(this->x()-df->width());
  p.setY(this->y());

  //qDebug() << cSettings->organizationName() << cSettings->applicationName();

  df->setData(cSettings->organizationName(),cSettings->applicationName());

  df->show();
  df->move(p);
}

void MainWindow::on_action_About_triggered()
{
  QString Message;
  Message = "Temp version v" + version.toString();
  Message += "\n\nCreated by:\nFredrik Olausson <fredrik@bluppfisk.org>\n";
  QMessageBox::about(this,"About Temp",Message);
}

void MainWindow::on_actionOnTop_triggered()
{
  if(ui->actionOnTop->isChecked()){
#ifdef Q_OS_WIN
	SetWindowPos(reinterpret_cast<HWND>(this->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
#endif
	this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
	this->show();
  }else{
#ifdef Q_OS_WIN
	SetWindowPos(reinterpret_cast<HWND>(this->winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
#endif
	this->setWindowFlags(this->windowFlags() ^ Qt::WindowStaysOnTopHint);
	this->show();
  }
}

void MainWindow::on_actionGraph_triggered()
{
  gd->show();
}

void MainWindow::on_actionMonitor_triggered(bool checked)
{
  if(checked){
	mon = true;
  }else{
	mon = false;
  }
}

void MainWindow::on_actionRrdConfig_triggered()
{
  rd->setRrdPath(rrdPath);
  rd->setRrdCmdPath(rrdCmdPath);
  rd->show();
}
