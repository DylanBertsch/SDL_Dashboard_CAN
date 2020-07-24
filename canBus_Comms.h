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

class CanBus_Comms{
public:
    int s, i;
    int nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;

    CanBus_Comms()
    {
        if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
            perror("Socket");
            //return 1;
        }
        strcpy(ifr.ifr_name, "can0" );
        ioctl(s, SIOCGIFINDEX, &ifr);
        memset(&addr, 0, sizeof(addr));
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;
        if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("Bind");
            //return 1;
        }

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