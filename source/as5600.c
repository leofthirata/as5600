#include <stdio.h>

// for close()
#include <unistd.h>

// for open() and O_RDWR
#include <fcntl.h>

// for ioctl()
#include <sys/ioctl.h>

#include <linux/i2c.h>

// for I2C_SLAVE
#include <linux/i2c-dev.h>

#include "as5600.h"

int init()
{
    int fd = open("/dev/i2c-1", O_RDWR);
    if (fd < 0)
        return fd;
    
    int ret = ioctl(fd, I2C_SLAVE, DEV_ADDR);
    if (ret < 0)
    {
        close(fd);
        return ret;
    }

    return fd;
}

int get_id(int fd)
{
    char addr = 0x07;
    char reg = 0;

    int ret = write(fd, &addr, sizeof(addr));
    if (ret < 0)
    {
        printf("I2C write failed\n");
        return ret;
    }

    ret = read(fd, &reg, sizeof(reg));
    if (ret != sizeof(reg))
    {
        printf("I2C read failed\n");
        return ret;
    }

    return reg;
}

int get_reg(int fd, char addr)
{
    int ret = write(fd, &addr, sizeof(addr));
    if (ret != sizeof(addr))
    {
        printf("I2C write reg failed\n");
        return ret;
    }

    char reg = 0;
    ret = read(fd, &reg, sizeof(reg));
    if (ret != sizeof(reg))
    {
        printf("I2C read reg failed\n");
        return ret;
    }

    return reg;
}

int set_reg(int fd, __u8 reg, char data)
{
    char buf[2];

    buf[0] = reg;
    buf[1] = data;
  
    int ret = write(fd, buf, 2);
    if (ret != 2) {
        printf("I2C write reg failed\n");
        return ret;
    }

    return 0;
}

int main(void)
{
    int fd = init();
    if (fd < 0)
    {
        printf("I2C init failed\n");
        return -1;
    }

    char conf_0x07 = get_reg(fd, 0x07);
    printf("CONF_0x07: %d\n", conf_0x07);

    int ret = set_reg(fd, 0x07, 0x08);
    if (ret < 0)
    {
        printf("I2C set reg failed\n");
        return ret;
    }

    conf_0x07 = get_reg(fd, 0x07);
    printf("CONF_0x07: %d\n", conf_0x07);

    close(fd);

    return 0;
}