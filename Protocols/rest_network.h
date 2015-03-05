#ifndef REST_NETWORK_H
#define REST_NETWORK_H

#include <QString>
#include <QStringList>

#include <QByteArray>

enum run_mode {
  STOP = 0,
  TELEOP = 1,
  SAFE_AUTON = 2,
  FULL_AUTON = 3
};

class rest_network
{
public:
    rest_network();

    void SetLeftJoystick(int);
    void SetRightJoystick(int);
    void SetRunMode(run_mode);

    int GetLeftJoystick();
    int GetRightJoystick();
    run_mode GetRunMode();

    QByteArray ToByteArray();
    bool ParseString(QString);


private:
    int left_joystick;
    int right_joystick;
    run_mode current_mode;
};

#endif // REST_NETWORK_H
