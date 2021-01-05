#include <QCoreApplication>
#include <linux/serial.h>
#include <asm/termios.h>
#include <asm/ioctls.h>
#include <asm/termbits.h>
#include <stropts.h>
#include <QThread>

#include <fcntl.h>    /* For O_RDWR */
#include <unistd.h>   /* For open(), creat() */

#include <QDebug>

int main(){
    int fd;
    fd = open("/dev/ttyUSB0",O_RDWR|O_NOCTTY);

    //========================= setup serial =========================
    struct termios2 tio;
    ioctl(fd, TCGETS2, &tio);
    tio.c_cflag &= ~CBAUD;
    tio.c_cflag |= BOTHER;
    tio.c_ispeed = 5760;
    tio.c_ospeed = 5760;
    tio.c_cflag |= PARENB; // even parity
    tio.c_cflag &= ~CSTOPB; // 1 stop bit
    tio.c_cflag &= ~CSIZE;
    tio.c_cflag |= CS8;
    ioctl(fd, TCSETS2, &tio);
    //========================= setup serial =========================

    char ping_cmd[] = {2,1,0};
    char ping_rec[7];
    write(fd, &ping_cmd, sizeof(ping_cmd) );
    memset(ping_rec,0,sizeof (ping_rec));
    QThread::sleep(1);
    int m=0;
    while(ping_rec[0]!=0x30){
        read(fd,&ping_rec,sizeof(ping_rec));
        for(int i=0;i<sizeof (ping_rec);i++){
            if(ping_rec[i]!=0){
                m++;
            }
        }
        if(m!=0){
            for(int j=0;j<sizeof (ping_rec);j++){
                qDebug() << ping_rec[j];
            }
        }
        m=0;
        memset(ping_rec,0,sizeof (ping_rec));
    }
    close(fd);
}

