#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <iostream>
#include <thread>

#define CAN_BATT_ADDRESS 0x1E08500A
#define CAN_CTS_ADDRESS 0x1E07D00A
#define CAN_RPM_ADDRESS 0x1E00500A
#define CAN_AFR_ADDRESS 0x1E02100A
#define CAN_TARGETAFR_ADDRESS 0x1E01500A
#define CAN_IGNADV_ADDRESS 0x1E05900A
#define CAN_SPEED_ADDRESS 0x1E4A900A

using namespace std;

struct SensorData{
    float RPM;
    float CTS;
    float BATT;
    float AFR;
    float IgnitionTiming;
    float TargetAFR;
    float Speed;
};
class CanBus_Comms;

class CanBus_Comms {
public:
    int s = 0;
    int i = 0;
    int nbytes = 0;
    struct sockaddr_can addr{};
    struct ifreq ifr{};
    struct can_frame frame{};
    SensorData sensorData{};
    CanBus_Comms()
    {
        //Run command line commands to bring the can0 interface online.
        std::string output;//exec("ip link set can0 up type can bitrate 1000000");
        if(output.length() > 0)
        {
            //Do not continue
        }
        output = "";//exec("ifconfig can0 up");
        if(output.length() > 0)
        {
            //Do not continue
        }
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

     std::string exec(char* cmd)
    {
        char buffer[128];
        std::string result;
        FILE* pipe = popen(cmd,"r");
        if(!pipe) throw std::runtime_error("popen() failed!");
        try{
            while(fgets(buffer,sizeof buffer,pipe) != nullptr)
            {
                result += buffer;
            }
        } catch(...){
            pclose(pipe);
            throw;
        }
        pclose(pipe);
        return result;
    }

    int getStatus()
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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-use-auto"
  void readFrame()
    {
        nbytes = read(s, &frame, sizeof(struct can_frame));

        if (nbytes < 0) {
            perror("Read");
            //return 1;
        }
        //Fill sensor data struct
        //Generate a char array from the frame data
        char frameData[4];
        int count = 3;
        for(int index = 0; index < 4; index++) // NOLINT(modernize-loop-convert)
        {
            frameData[index] = frame.data[count];
            count--;
        }
        //cout << (frame.can_id & 0x1FFFFFFFU) << endl;
        switch(frame.can_id & 0x1FFFFFFFU)
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
            case CAN_IGNADV_ADDRESS:{
                float *data = (float *) &(frameData[0]);
                sensorData.IgnitionTiming = *data;
                break;
            }
            case CAN_SPEED_ADDRESS:{
              float *data = (float*)&(frameData[0]);
              sensorData.Speed = *data;
              break; 
            }
        }
        //
    }
#pragma clang diagnostic pop
};
