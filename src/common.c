#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "common.h"

Eina_Bool is_test = EINA_TRUE;

void
common_init(void)
{
   is_test = !!getenv("CAR_TEST");
}

Eina_Bool
GPIOExport(int pin)
{
   if (is_test) return EINA_TRUE;
#define BUFFER_MAX 3
   char buffer[BUFFER_MAX];
   ssize_t bytes_written;
   int fd = open("/sys/class/gpio/export", O_WRONLY);

   if (-1 == fd)
     {
        fprintf(stderr, "Failed to open export for writing (pin %d)!\n", pin);
        return EINA_FALSE;
     }

   bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
   write(fd, buffer, bytes_written);
   close(fd);
   return EINA_TRUE;
}

Eina_Bool
GPIOUnexport(int pin)
{
   if (is_test) return EINA_TRUE;
   char buffer[BUFFER_MAX];
   ssize_t bytes_written;

   int fd = open("/sys/class/gpio/unexport", O_WRONLY);
   if (-1 == fd)
     {
        fprintf(stderr, "Failed to open unexport for writing (pin %d)!\n", pin);
        return EINA_FALSE;
     }

   bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
   write(fd, buffer, bytes_written);
   close(fd);
   return EINA_TRUE;
}

Eina_Bool
GPIOExists(int pin)
{
   if (is_test) return EINA_TRUE;
#define DIRECTION_MAX 35
   char path[DIRECTION_MAX];
   int fd;

   snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
   fd = open(path, O_WRONLY);
   if (fd != -1) close(fd);
   return (fd > 0);
}

Eina_Bool
GPIODirection(int pin, const char *dir)
{
   if (is_test) return EINA_TRUE;
#define DIRECTION_MAX 35
   char path[DIRECTION_MAX];
   int fd;

   snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
   fd = open(path, O_WRONLY);
   if (-1 == fd)
     {
        fprintf(stderr, "Failed to open gpio %d direction for writing!\n", pin);
        return EINA_FALSE;
     }

   if (-1 == write(fd, dir, strlen(dir)))
     {
        fprintf(stderr, "Failed to set %s directioni for pin %d!\n", dir, pin);
        return EINA_FALSE;
     }

   close(fd);
   return EINA_TRUE;
}

#define VALUE_MAX 30
Eina_Bool
GPIORead(int pin, int *value)
{
   char path[VALUE_MAX];
   char value_str[3];
   int fd;

   *value = 0;
   if (is_test) return EINA_TRUE;

   snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
   fd = open(path, O_RDONLY);
   if (-1 == fd)
     {
        fprintf(stderr, "Failed to open gpio value for reading!\n");
        return EINA_FALSE;
     }

   if (-1 == read(fd, value_str, 3))
     {
        fprintf(stderr, "Failed to read value!\n");
        return EINA_FALSE;
     }

   close(fd);

   *value = *value_str - '0';
   return EINA_TRUE;
}

Eina_Bool
GPIOWrite(int pin, int value)
{
   if (is_test) return EINA_TRUE;
   static const char s_values_str[] = "01";

   char path[VALUE_MAX];
   int fd;

   snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
   fd = open(path, O_WRONLY);
   if (-1 == fd)
     {
        fprintf(stderr, "Failed to open gpio value for writing!\n");
        return EINA_FALSE;
     }

   if (1 != write(fd, &s_values_str[value % 2], 1))
     {
        fprintf(stderr, "Failed to write value!\n");
        return EINA_FALSE;
     }

   close(fd);
   return EINA_TRUE;
}


