#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <vector>


class dataCommunication{
private:
    char* readBuffer;
    std::vector<float> processedDataVector;
public:
    int serial_port = -1;
    char* read_data()
    {
        int n = 0,spot = 0;
        char buf = '\0';
        memset(readBuffer, '\0', sizeof readBuffer);
        do {
            n = read( serial_port, &buf, 1 );
            sprintf(&readBuffer[spot], "%c", buf );
            spot += n;
        } while( buf != '\r' && n > 0);
        return readBuffer;
    }
    int write_data()
    {
        unsigned char cmd[] = "INIT \r";
        int n_written = 0,
                spot = 0;

        do {
            n_written = write(serial_port, &cmd[spot], 1 );
            spot += n_written;
        } while (cmd[spot-1] != '\r' && n_written > 0);
    }
    std::vector<float>* processDataString(char* dataString)
    {
        //find where the beginning characters are located to determine string validity
        int firstIndex = -1;
        int secondIndex = -1;
        for(int index = 0; index < strlen(dataString); index++)
        {
            if(dataString[index] == '{')
            {
                firstIndex = index;
            }
            if(dataString[index] == '}')
            {
                secondIndex = index;
            }
        }
        if(firstIndex != -1 && secondIndex != -1 && firstIndex < secondIndex) {
            dataString[secondIndex] = '\0';
            dataString = &dataString[firstIndex+1];
            processedDataVector.clear();
            char *token = strtok(dataString, ",");
            while (token != NULL) {
                processedDataVector.push_back(atof(token));
                token = strtok(NULL,",");
            }
            return (std::vector<float>*)&processedDataVector;
        } else
        {
            return nullptr;
        }
    }



    dataCommunication()
    {
        readBuffer = (char*)malloc(1024);
        serial_port = open("/dev/ttyACM0",O_RDWR);
        struct termios tty;
        struct termios tty_old;
        memset (&tty, 0, sizeof tty);

/* Error Handling */
        if ( tcgetattr ( serial_port, &tty ) != 0 ) {
            std::cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
        }

/* Save old tty parameters */
        tty_old = tty;

/* Set Baud Rate */
        cfsetospeed (&tty, (speed_t)B9600);
        cfsetispeed (&tty, (speed_t)B9600);

/* Setting other Port Stuff */
        tty.c_cflag     &=  ~PARENB;            // Make 8n1
        tty.c_cflag     &=  ~CSTOPB;
        tty.c_cflag     &=  ~CSIZE;
        tty.c_cflag     |=  CS8;

        tty.c_cflag     &=  ~CRTSCTS;           // no flow control
        tty.c_cc[VMIN]   =  1;                  // read doesn't block
        tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
        tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

/* Make raw */
        cfmakeraw(&tty);

/* Flush Port, then applies attributes */
        tcflush( serial_port, TCIFLUSH );
        if ( tcsetattr ( serial_port, TCSANOW, &tty ) != 0) {
            std::cout << "Error " << errno << " from tcsetattr" << std::endl;
        }
    }
};