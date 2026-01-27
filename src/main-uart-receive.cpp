#include <iostream>
#include <iomanip>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <cstring>

static constexpr const char* UART_DEV = "/dev/ttyS0";       // trying uart1 (ttyS0) instead of uart2 which is mapped to WKUP?
static constexpr speed_t BAUDRATE = B1000000;               // no comment
static constexpr size_t BYTES_PER_LINE = 9;

int open_uart(const char* device) {
    int fd = open(device, O_RDONLY | O_NOCTTY);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    termios tty{};
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        close(fd);
        return -1;
    }

    // Raw mode
    cfmakeraw(&tty);

    // Baud rate
    cfsetispeed(&tty, BAUDRATE);
    cfsetospeed(&tty, BAUDRATE);

    // 8-N-1
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;

    tty.c_cflag |= CREAD | CLOCAL;

    // Blocking read: wait for at least 1 byte
    tty.c_cc[VMIN]  = 1;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        close(fd);
        return -1;
    }

    return fd;
}

int main() {
    int fd = open_uart(UART_DEV);
    if (fd < 0) {
        return 1;
    }

    std::vector<uint8_t> buffer;
    buffer.reserve(BYTES_PER_LINE);
    
    int counter = 0;

    while (true) {
        uint8_t byte;
        ssize_t n = read(fd, &byte, 1);
        if (n <= 0) {
            perror("read");
            break;
        }

        buffer.push_back(byte);

        if (buffer.size() == BYTES_PER_LINE) {
            // Print one line of 9 bytes in hex
            for (size_t i = 0; i < buffer.size(); ++i) {
                std::cout << std::hex
                          << std::setw(2)
                          << std::setfill('0')
                          << static_cast<int>(buffer[i]);

                if (i < buffer.size() - 1)
                    std::cout << " ";
            }
            std::cout << std::dec << std::endl;

            buffer.clear();
        }

        if (counter++ == 100) {
            std::cout << "WHILE 100!\n";
            counter = 0;
        }
    }

    close(fd);
    return 0;
}
