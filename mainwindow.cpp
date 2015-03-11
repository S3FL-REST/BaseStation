#include "mainwindow.h"
#include "ui_mainwindow.h"

const int JOYSTICK_NUMBER = 0;

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
}

void MainWindow::JoystickUpdate() {
    double limit = SHRT_MAX;

    JoystickEvent event;

    while (js.sample(&event)) {
        if (event.isInitialState()) continue;

        if (event.isAxis()) {
            if (event.number == 4) right = (floor(10 * event.value / limit) / 10.0);
            else if (event.number == 1) left = (floor(10 * event.value / limit) / 10.0);

            if (abs(right * 255.0) < 30) right = 0;
            if (abs(left * 255.0) < 30) left = 0;
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
        networkData.SetLeftJoystick(0);
        networkData.SetRightJoystick(0);

        if (networkData.GetRunMode() != FULL_AUTON)
            networkData.SetRunMode(STOP);

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
