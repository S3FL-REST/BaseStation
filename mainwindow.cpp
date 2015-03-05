#include "mainwindow.h"
#include "ui_mainwindow.h"



const int JOYSTICK_NUMBER = 2;

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

    connect(&joystickTimer, SIGNAL(timeout()), this, SLOT(JoystickUpdate()));

    if (js.isFound()) {
        joystickTimer.start(10);
    } else {
        qDebug() << "Joystick not connected";
    }

    connect(&networkTimer, SIGNAL(timeout()), this, SLOT(NetworkUpdate()));
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

void MainWindow::ConnectPressed() {
    if (!networkTimer.isActive()) {
        socket.connectToHost(ui->IPAddress->text(), 3141);

        if (!socket.waitForConnected()) {
            QMessageBox::information(this, "Connection", "Unable to Connnect");
        } else {
            networkTimer.start(100);
        }
    }
}

MainWindow::~MainWindow() {
    delete ui;
}
