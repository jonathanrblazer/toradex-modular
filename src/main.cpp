#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <dirent.h>

static const int BAUDRATE = B115200;

std::atomic<bool> running{true};

std::vector<std::string> findACMports()
{
    std::vector<std::string> ports;
    DIR* dir = opendir("/dev");
    if (!dir)
        return ports;

    dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name(entry->d_name);
        if (name.rfind("ttyACM", 0) == 0) {
            ports.emplace_back("/dev/" + name);
        }
    }

    closedir(dir);
    return ports;
}

int open_serial(const char *dev)
{
    int fd = open(dev, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        perror("open_serial");
        return -1;
    }

    struct termios tty{};
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        close(fd);
        return -1;
    }

    cfmakeraw(&tty);
    cfsetispeed(&tty, BAUDRATE);
    cfsetospeed(&tty, BAUDRATE);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    tty.c_cc[VMIN]  = 1;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        close(fd);
        return -1;
    }

    return fd;
}

void rx_thread(int fd)
{
    std::string line;
    char ch;

    while (running) {
        ssize_t n = read(fd, &ch, 1);
        if (n <= 0)
            continue;

        if (ch == '\n') {
            std::cout << line << std::endl;

            if (line == "END_DEBUGMODE_106") {
                std::cout << "=== END OF IMAGE DUMP ===" << std::endl;
            }

            line.clear();
        } else if (ch != '\r') {
            line.push_back(ch);
        }
    }
}

int main()
{
    auto ports = findACMports();

    if (ports.empty()) {
        std::cerr << "No ttyACM devices found." << std::endl;
        return 1;
    }

    std::string port = ports[0];
    std::cout << "Opening serial device: " << port << std::endl;

    int fd = open_serial(port.c_str());
    if (fd < 0)
        return 1;

    std::thread rx(rx_thread, fd);

    std::cout << "Interactive mode ready." << std::endl;
    std::cout << "Type commands (e.g. 'debugmode 106;') and press Enter." << std::endl;

    std::string cmd;
    while (running && std::getline(std::cin, cmd)) {
        cmd.push_back('\n');
        write(fd, cmd.c_str(), cmd.size());
    }

    running = false;
    rx.join();
    close(fd);
    return 0;
}
