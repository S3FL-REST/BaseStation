#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "Joystick/joystick.hh"
#include "Protocols/rest_network.h"

#include <QTcpSocket>
#include <QTimer>
#include <QMessageBox>

#include <climits>
#include <cmath>

#include <QDebug>

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

    Joystick js;
    double left;
    double right;

    rest_network networkData;

    QTcpSocket socket;
    QTimer joystickTimer;
    QTimer networkTimer;

private slots:
    void JoystickUpdate();
    void NetworkUpdate();

    void StopPressed();
    void TeleopPressed();
    void SemiAutonPressed();
    void FullAutonPressed();

    void ConnectPressed();
    void DisconnectPressed();
};

#endif // MAINWINDOW_H
