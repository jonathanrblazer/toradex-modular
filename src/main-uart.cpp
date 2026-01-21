#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    const char* device = "/dev/ttySTM1";
    const int baudrate = B921600;

    int fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        return 1;
    }

    cfsetospeed(&tty, baudrate);
    cfsetispeed(&tty, baudrate);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;  // 8-bit chars
    tty.c_cflag |= CLOCAL | CREAD;               // enable receiver
    tty.c_cflag &= ~(PARENB | PARODD);            // no parity
    tty.c_cflag &= ~CSTOPB;                       // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;                      // ❌ no HW flow control (for now)

    tty.c_iflag = 0;
    tty.c_oflag = 0;
    tty.c_lflag = 0;

    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        return 1;
    }

    std::cout << "UART1 TX test running at 921600 baud...\n";

    uint8_t counter = 0;

    while (true) {
        uint8_t buf[64];
        for (int i = 0; i < 64; i++) {
            buf[i] = counter++;
        }

        int n = write(fd, buf, sizeof(buf));
        if (n < 0) {
            perror("write");
            break;
        }

        tcdrain(fd);  // wait until all bytes physically transmitted
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    close(fd);
    return 0;
}
