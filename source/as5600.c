#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "as5600.h"

#define I2C_BUS "/dev/i2c-1"

int init()
{
    int fd = open(I2C_BUS, O_RDWR);
    if (fd < 0)
        return fd;
    
    int ret = ioctl(fd, I2C_SLAVE, DEV_ADDR);
    if (ret < 0) {
        close(fd);
        return ret;
    }

    return fd;
}

int get_status(int fd)
{
    char addr = AS5600_STATUS;
    return read_reg(fd, addr);
}

int is_magnet(int fd)
{
    return (get_status(fd) >> 5) & 0x01;
}

int is_magnet_weak(int fd)
{
    return (get_status(fd) >> 4) & 0x01;
}

int is_magnet_strong(int fd)
{
    return (get_status(fd) >> 3) & 0x01;
}

// magnitude bits (11:8)
int get_magnitude_h(int fd)
{
    char addr = AS5600_MAGNITUDE_H;
    return read_reg(fd, addr);
}

// magnitude bits (7:0)
int get_magnitude_l(int fd)
{
    char addr = AS5600_MAGNITUDE_L;
    return read_reg(fd, addr);
}

int get_magnitude(int fd)
{
    char addr = AS5600_MAGNITUDE_H;
    char buf[2];
    read_regs(fd, addr, buf, 2);

    return buf[0] << 8 | buf[1];
}

int set_zpos(int fd, int pos)
{
    char addr = AS5600_ZPOS_H;
    return write_regs(fd, addr, pos);
}

int get_zpos(int fd)
{
    char addr = AS5600_ZPOS_H;
    return get_bytes(fd, addr);
}

int get_angle(int fd)
{
    char addr = AS5600_ANGLE_H;
    return get_bytes(fd, addr);
}

int get_angle_raw(int fd)
{
    char addr = AS5600_RAW_ANGLE_H;
    return get_bytes(fd, addr);
}

int get_bytes(int fd, char addr)
{
    char buf[2];
    read_regs(fd, addr, buf, 2);

    return buf[0] << 8 | buf[1];
}

int read_regs(int fd, char reg, char *buf, int size)
{
    int ret = write(fd, &reg, sizeof(reg));
    if (ret != sizeof(reg)) {
        printf("I2C write reg failed\n");
        return ret;
    }

    ret = read(fd, buf, size);
    if (ret != size)
        printf("I2C read reg failed\n");

    return ret;
}

int read_reg(int fd, char reg)
{
    int ret = write(fd, &reg, sizeof(reg));
    if (ret != sizeof(reg)) {
        printf("I2C write reg failed\n");
        return ret;
    }

    char data = 0;
    ret = read(fd, &data, sizeof(data));
    if (ret != sizeof(data)) {
        printf("I2C read data failed\n");
        return ret;
    }

    return data;
}

int write_reg(int fd, char reg, char data)
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

int write_regs(int fd, char reg, int data)
{
    char buf[3];

    buf[0] = reg;
    buf[1] = data >> 8;
    buf[2] = data & 0x0F;
  
    int ret = write(fd, buf, 3);
    if (ret != 3) {
        printf("I2C write reg failed\n");
        return ret;
    }

    return 0;
}

int main(void)
{
    int fd = init();
    if (fd < 0) {
        printf("I2C init failed\n");
        return -1;
    }

    char conf_0x07 = read_reg(fd, 0x07);
    printf("CONF_0x07: %d\n", conf_0x07);

    int ret = write_reg(fd, 0x07, 0x08);
    if (ret < 0) {
        printf("I2C set reg failed\n");
        return ret;
    }

    conf_0x07 = read_reg(fd, 0x07);
    printf("CONF_0x07: %d\n", conf_0x07);

    if (is_magnet(fd) == 1)
        printf("magnet found\n");
    else
        printf("magnet not found\n");

    if (is_magnet_weak(fd) == 1)
        printf("magnet weak\n");
    else
        printf("magnet not weak\n");

    if (is_magnet_strong(fd) == 1)
        printf("magnet strong\n");
    else
        printf("magnet not strong\n");

    char magn_h = get_magnitude_h(fd);
    char magn_l = get_magnitude_l(fd);
    int magn = get_magnitude(fd);
    printf("AS5600_MAGNITUDE_H: %d\n", magn_h);
    printf("AS5600_MAGNITUDE_L: %d\n", magn_l);
    printf("AS5600_MAGNITUDE: %d\n", magn);

    set_zpos(fd, magn);
    printf("ZPOS: %d\n", get_zpos(fd));

    int angle_raw = get_angle_raw(fd);
    int angle = get_angle(fd);
    printf("angle_raw: %d\n", angle_raw);
    printf("angle: %d\n", angle);

    close(fd);

    return 0;
}