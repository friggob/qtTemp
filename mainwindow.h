#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QSettings>
#include <QMouseEvent>

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

  public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

  private:
	Ui::MainWindow *ui;

	void setTemp(void);
	void get_temp();
	void readConfig();
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

	int m_nMouseClick_X_Coordinate;
	int m_nMouseClick_Y_Coordinate;
	int pPi;
	int pTemp;

	QPalette dp;
	QPalette ep;
	QString last_temp;
	QString temp;
	QString hPi;
	QString hTemp;
	QTcpSocket *socket;

/*
  protected:
	void mouseReleaseEvent(QMouseEvent* event);
*/

  private slots:
	void on_action_Quit_triggered();
	void on_action_Update_triggered();
	void getTempNet();
	void on_action_Change_triggered();
	void testNet();
};

#endif // MAINWINDOW_H
