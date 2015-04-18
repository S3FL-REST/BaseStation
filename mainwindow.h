#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "Joystick/joystick.hh"
#include "Protocols/rest_network.h"

#include <QTcpSocket>
#include <QTimer>
#include <QMessageBox>

#include <QByteArray>
#include <QBuffer>

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
    Network2Rover::linear_actuator binActuator;
    Network2Rover::linear_actuator scoopActuator;
    Network2Rover::linear_actuator suspensionActuator;

    double armRate;
    int ar_1;
    int ar_2;

    Network2Rover networkData;

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

    void DataReady();
};

#endif // MAINWINDOW_H
