#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <iostream>
#include <thread>

using namespace std;

struct SensorData{
    float RPM;
    float CTS;
    float BATT;
    float AFR;
    float IgnitionTiming;
};
class CanBus_Comms;
void dataThread_Function(CanBus_Comms* canBusCommsPTR);

class CanBus_Comms{
public:
    int status = -1;
    int s, i;
    int nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;

    CanBus_Comms()
    {
        if ((status = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
            perror("Socket");
        }
        strcpy(ifr.ifr_name, "can0");
        ioctl(status, SIOCGIFINDEX, &ifr);
        memset(&addr, 0, sizeof(addr));
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;
        if (bind(status, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("Bind");
        }
        if(getStatus() != 0)//Spawn Sensor Data thread
        {
            std::thread first(dataThread_Function,this);
        }
    }

    int getStatus()
    {
        return addr.can_addr.j1939.name;
    }

    void readFrame()
    {
        nbytes = read(s, &frame, sizeof(struct can_frame));

        if (nbytes < 0) {
            perror("Read");
            //return 1;
        }
        cout << frame.data[0] << endl;
    }

};

void dataThread_Function(CanBus_Comms* CanBusPTR)//Listens for Can data and processes in a separate thread
{
    struct can_frame* frm = &CanBusPTR->frame;
    while(true)
    {
        CanBusPTR->nbytes = read(CanBusPTR->status,frm, sizeof(struct can_frame));
        if (CanBusPTR->nbytes < 0) {
            perror("Read");
            //return 1;
        }
    }
}