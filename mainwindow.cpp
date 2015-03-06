#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCamera>
#include <QCameraInfo>

#include <QImage>
#include <QGraphicsScene>
#include <QGraphicsView>

#include <QVideoWidget>

const int JOYSTICK_NUMBER = 1;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    js(JOYSTICK_NUMBER), left(0.0), right(0.0)
{
    ui->setupUi(this);

    connect(ui->StopButton, SIGNAL(clicked()), this, SLOT(StopPressed()));
    connect(ui->TeleopButton, SIGNAL(clicked()), this, SLOT(TeleopPressed()));
    connect(ui->SemiAutonButton, SIGNAL(clicked()), this, SLOT(SemiAutonPressed()));
    connect(ui->FullAutonButton, SIGNAL(clicked()), this, SLOT(FullAutonPressed()));

    connect(ui->ConnectButton, SIGNAL(clicked()), this, SLOT(ConnectPressed()));
    connect(ui->DisconnectButton, SIGNAL(clicked()), this, SLOT(DisconnectPressed()));

    connect(&joystickTimer, SIGNAL(timeout()), this, SLOT(JoystickUpdate()));

    if (js.isFound()) {
        joystickTimer.start(10);
    } else {
        qDebug() << "Joystick not connected";
        QMessageBox::information(this, "Joystick", "No Joystick Connected");
    }

    connect(&networkTimer, SIGNAL(timeout()), this, SLOT(NetworkUpdate()));

    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();

    if (cameras.size() > 0) {
        QCamera camera(cameras.at(0));

        QVideoWidget videoWidget;
        videoWidget.setFixedSize(640, 480);

        camera.setViewfinder(&videoWidget);

        camera.start();

        videoWidget.grab().toImage();
    } else {
        QMessageBox::information(this, "Camera", "No Camera Detected");
        qDebug() << "No Camera Detected";
    }
}

void MainWindow::JoystickUpdate() {
    double limit = SHRT_MAX;

    JoystickEvent event;

    while (js.sample(&event)) {
        if (event.isInitialState()) continue;

        if (event.isAxis()) {
            if (event.number == 0) left = event.value / limit;
            else if (event.number == 1) right = event.value / limit;
        }
    }
}

void MainWindow::NetworkUpdate() {
    networkData.SetLeftJoystick(static_cast<int>(left * 255));
    networkData.SetRightJoystick(static_cast<int>(right * 255));

    if (socket.state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Socket Not Connected";
        networkTimer.stop();
    }

    socket.write(networkData.ToByteArray());
    socket.flush();
}

//Button Code

void MainWindow::StopPressed() {
    networkData.SetRunMode(STOP);
}

void MainWindow::TeleopPressed() {
    networkData.SetRunMode(TELEOP);
}

void MainWindow::SemiAutonPressed() {
    networkData.SetRunMode(SAFE_AUTON);
}

void MainWindow::FullAutonPressed() {
    networkData.SetRunMode(FULL_AUTON);
}

void MainWindow::DisconnectPressed() {
    if (networkTimer.isActive()) {
        networkTimer.stop();
        socket.disconnectFromHost();
    }
}

void MainWindow::ConnectPressed() {
    if (!networkTimer.isActive()) {
        socket.connectToHost(ui->IPAddress->text(), 3141);

        if (!socket.waitForConnected()) {
            QMessageBox::information(this, "Connection", "Unable to Connnect to " + ui->IPAddress->text());
        } else {
            networkTimer.start(100);
        }
    }
}

MainWindow::~MainWindow() {
    delete ui;
}
