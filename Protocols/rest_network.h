#ifndef REST_NETWORK_H
#define REST_NETWORK_H

#include <QString>
#include <QStringList>

#include <QByteArray>

#include <QImage>
#include <QBuffer>
#include <QDataStream>

class Network2Rover
{
public:
    enum run_mode {
      STOP = 0,
      TELEOP = 1,
      SAFE_AUTON = 2,
      FULL_AUTON = 3
    };

    enum linear_actuator {
        L_STOP = 0,
        L_FORWARD = 1,
        L_REVERSE = 2
    };

    Network2Rover();

    static const int NUM_PARAMS = 6;

    void SetLeftJoystick(int);
    void SetRightJoystick(int);
    void SetRunMode(run_mode);
    void SetBinActuator(linear_actuator);
    void SetScoopActuator(linear_actuator);
    void SetArmRate(int);

    int GetLeftJoystick() const;
    int GetRightJoystick() const;
    run_mode GetRunMode() const;
    linear_actuator GetBinActuator() const;
    linear_actuator GetScoopActuator() const;
    int GetArmRate() const;

    QByteArray ToByteArray() const;
    bool ParseData(QByteArray&);

private:
    int left_joystick;
    int right_joystick;
    run_mode current_mode;
    linear_actuator bin_actuator;
    linear_actuator scoop_actuator;
    int rate;
};

class Network2Base
{
public:
    Network2Base();

    void SetImage(QImage&);

    QImage GetImage() const;

    QByteArray ToByteArray() const;
    bool ParseData(QByteArray&);

private:
    QImage image;
};

#endif // REST_NETWORK_H
