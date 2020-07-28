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

#define CAN_BATT_ADDRESS 0x1E08500A
#define CAN_CTS_ADDRESS 1543//0x1E07D00A
#define CAN_RPM_ADDRESS 0x1E00500A
#define CAN_AFR_ADDRESS 0x1E02100A
#define CAN_TARGETAFR_ADDRESS 0x1E01500A
#define CAN_ADV_ADDRESS 0x1E05900A

using namespace std;

struct SensorData{
    float RPM;
    float CTS;
    float BATT;
    float AFR;
    float IgnitionTiming;
    float TargetAFR;
};
class CanBus_Comms;
void canBusDataAcquireThread(CanBus_Comms* comms);

class CanBus_Comms {
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
        if(getStatus() == 1)//Start processing thread
        {
            //std::thread processingThread(canBusDataAcquireThread,this);
        }
    }

    bool getStatus()
    {
        if(ifr.ifr_ifru.ifru_hwaddr.sa_family == 0)
        {
            return -1;
        }
        else
        {
            return 1;
        }
    }

    char* reverseEndianness(char* inputPTR, int byteSize)//Reverses a char array
    {
        char* outputArray = (char*)calloc(0,byteSize);
        int count = 0;
        for(int i = byteSize; i > 0; i--)
        {
            outputArray[count] = inputPTR[i];
        }
        return outputArray;
    }

    SensorData* readFrame()
    {
        //Determine if the can interface is open

        nbytes = read(s, &frame, sizeof(struct can_frame));

        if (nbytes < 0) {
            perror("Read");
            //return 1;
        }
        //Fill sensor data struct
        SensorData sensorData;
        //Generate a char array from the frame data
        char frameData[4];
        int count = 3;
        for(int i = 0; i < 4; i++)
        {
            frameData[i] = frame.data[count];
            count--;
        }
        switch(frame.can_id)
        {
            case CAN_CTS_ADDRESS: {
                float *data = (float *) &(frameData[0]);
                sensorData.CTS = *data;
                break;
            }
            case CAN_BATT_ADDRESS:{
                float *data = (float *) &(frameData[0]);
                sensorData.BATT = *data;
                break;
            }
            case CAN_RPM_ADDRESS:{
                float *data = (float *) &(frameData[0]);
                sensorData.RPM = *data;
                break;
            }
            case CAN_AFR_ADDRESS: {
                float *data = (float *) &(frameData[0]);
                sensorData.AFR = *data;
                break;
            }
            case CAN_TARGETAFR_ADDRESS:{
                float *data = (float *) &(frameData[0]);
                sensorData.TargetAFR = *data;
                break;
            }
        }

        //cout << frame.data[0] << endl;
    }

};


void canBusDataAcquireThread(CanBus_Comms* comms)
{
    while(true)
    {

    }
}