#include "mainwindow.h"
#include "ui_mainwindow.h"

const int JOYSTICK_NUMBER = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    js(JOYSTICK_NUMBER), left(0.0), right(0.0),
    binActuator(Network2Rover::L_STOP), scoopActuator(Network2Rover::L_STOP), suspensionActuator(Network2Rover::L_STOP),
<<<<<<< HEAD
    armRate(0.0), ar_1(0), ar_2(0)
=======
    connectedState(QPalette::Background, Qt::green), disconnectedState(QPalette::Background, Qt::red)
>>>>>>> 29b78995696f1e956763ac8b047273654d370248
{
    ui->setupUi(this);

    connect(ui->StopButton, SIGNAL(clicked()), this, SLOT(StopPressed()));
    connect(ui->TeleopButton, SIGNAL(clicked()), this, SLOT(TeleopPressed()));
    connect(ui->SemiAutonButton, SIGNAL(clicked()), this, SLOT(SemiAutonPressed()));
    connect(ui->FullAutonButton, SIGNAL(clicked()), this, SLOT(FullAutonPressed()));

    connect(ui->ConnectButton, SIGNAL(clicked()), this, SLOT(ConnectPressed()));
    connect(ui->DisconnectButton, SIGNAL(clicked()), this, SLOT(DisconnectPressed()));

    connect(&joystickTimer, SIGNAL(timeout()), this, SLOT(JoystickUpdate()));

    connect(&socket, SIGNAL(readyRead()), this, SLOT(DataReady()));

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
        //if (event.isInitialState()) continue;

        if (event.isAxis()) {
            if (event.number == 4) right = (floor(10 * event.value / limit) / 10.0);
            else if (event.number == 1) left = (floor(10 * event.value / limit) / 10.0);

            if (event.number == 2 || event.number == 5) {
                if (event.number == 2) ar_1 = event.value;
                else if (event.number == 5) ar_2 = event.value;

                armRate = (ar_2 - ar_1) / (2 * limit);
            }

            //Arm Rate -> Event number and rates need updating and validating
            else if (event.number == 3) armRate = 100 * event.value / limit;

            const double MOTOR_DEADZONE = 0.12;
            const double RATE_DEADZONE = 0.1;

            if (abs(right) < MOTOR_DEADZONE) right = 0;
            if (abs(left) < MOTOR_DEADZONE) left = 0;

            if (abs(armRate) < RATE_DEADZONE) armRate = 0;
        } if (event.isButton()) {
            if (event.value != 0) qDebug() << "Button Pressed: " << event.number;

            //Bin Actuator -> Event numbers need changing
            if (event.number == 2)
                binActuator = (event.value == 0) ? Network2Rover::L_STOP : Network2Rover::L_FORWARD;
            else if (event.number == 3)
                binActuator = (event.value == 0) ? Network2Rover::L_STOP : Network2Rover::L_REVERSE;

            //Scoop Actuator -> Event numbers need changing
            if (event.number == 1)
                scoopActuator = (event.value == 0) ? Network2Rover::L_STOP : Network2Rover::L_FORWARD;
            else if (event.number == 0)
                scoopActuator = (event.value == 0) ? Network2Rover::L_STOP : Network2Rover::L_REVERSE;

            //Suspension Actuator -> Event numbers need changing
            if (event.number == 4 && event.value)
                suspensionActuator = Network2Rover::L_FORWARD;
            else if (event.number == 5 && event.value)
                suspensionActuator = Network2Rover::L_REVERSE;
        }
    }
}

void MainWindow::NetworkUpdate() {
    const int MOTOR_MODIFIER = 255;
    const int RATE_MODIFIER = 1000;

    networkData.SetLeftJoystick(static_cast<int>(left * MOTOR_MODIFIER));
    networkData.SetRightJoystick(static_cast<int>(right * MOTOR_MODIFIER));
    networkData.SetBinActuator(binActuator);
    networkData.SetScoopActuator(scoopActuator);
    networkData.SetSuspension(suspensionActuator);
    networkData.SetArmRate(static_cast<int>(armRate * RATE_MODIFIER));

    if (socket.state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Socket Not Connected";
        DisconnectPressed();
    }

    socket.write(networkData.ToByteArray());
    socket.flush();
}

//Button Code

void MainWindow::StopPressed() {
    networkData.SetRunMode(Network2Rover::STOP);
}

void MainWindow::TeleopPressed() {
    networkData.SetRunMode(Network2Rover::TELEOP);
}

void MainWindow::SemiAutonPressed() {
    networkData.SetRunMode(Network2Rover::SAFE_AUTON);
}

void MainWindow::FullAutonPressed() {
    networkData.SetRunMode(Network2Rover::FULL_AUTON);
}

void MainWindow::DisconnectPressed() {
    if (networkTimer.isActive()) {
        networkTimer.stop();
        socket.disconnectFromHost();

        ui->imageViewLabel->clear();
    }

    ui->ConnectButton->setStyleSheet("background-color:red");
}

void MainWindow::ConnectPressed() {
    if (!networkTimer.isActive()) {
        networkData.SetLeftJoystick(0);
        networkData.SetRightJoystick(0);

        if (networkData.GetRunMode() != Network2Rover::FULL_AUTON)
            networkData.SetRunMode(Network2Rover::STOP);

        socket.connectToHost(ui->IPAddress->text(), 3141);

        if (!socket.waitForConnected()) {
            QMessageBox::information(this, "Connection", "Unable to Connnect to " + ui->IPAddress->text());
            ui->ConnectButton->setStyleSheet("background-color:red");
        } else {
            networkTimer.start(100);
            ui->ConnectButton->setStyleSheet("background-color:green");
        }
    }
}

void MainWindow::DataReady() {
    QByteArray data = socket.readAll();

    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);

    QImage image;
    image.load(&buffer, "JPG");

    ui->imageViewLabel->setPixmap(QPixmap::fromImage(image));
}

MainWindow::~MainWindow() {
    delete ui;
}
