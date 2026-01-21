#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

int main()
{
    const char* device = "/dev/spidev0.0";
    uint8_t mode = SPI_MODE_0;
    uint8_t bits = 8;
    uint32_t speed = 1'000'000; // 1 MHz

    int fd = open(device, O_RDWR);
    if (fd < 0) {
        perror("Failed to open SPI device");
        return 1;
    }

    // Configure SPI
    ioctl(fd, SPI_IOC_WR_MODE, &mode);
    ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

    std::cout << "SPI test started on " << device << std::endl;
    std::cout << "Mode 0, 8 bits, 1 MHz\n";

    uint8_t tx[16];

    while (true) {
        // Fill test pattern
        for (int i = 0; i < 16; i++) {
            tx[i] = static_cast<uint8_t>(i);
        }

        struct spi_ioc_transfer tr{};
        tr.tx_buf = reinterpret_cast<unsigned long>(tx);
        tr.len = sizeof(tx);
        tr.speed_hz = speed;
        tr.bits_per_word = bits;

        int ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1) {
            perror("SPI transfer failed");
            break;
        }

        std::cout << "Sent: ";
        for (auto b : tx) {
            std::cout << std::hex << static_cast<int>(b) << " ";
        }
        std::cout << std::dec << std::endl;

        sleep(1);
    }

    close(fd);
    return 0;
}
