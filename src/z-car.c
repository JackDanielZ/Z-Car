#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"

static Ecore_Con_Client *_c = NULL;

#define IN  "in"
#define OUT "out"

typedef struct
{
   int in1_pin;
   int in2_pin;
} Motor_Config;

static Motor_Config motors [] =
{
     {
        .in1_pin = 20,
        .in2_pin = 21
     },
     {
        .in1_pin = 23,
        .in2_pin = 24
     }
};

static Eina_Bool _is_test = EINA_TRUE;

static Eina_Bool
_GPIOExport(int pin)
{
   if (_is_test) return EINA_TRUE;
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

static Eina_Bool
_GPIOUnexport(int pin)
{
   if (_is_test) return EINA_TRUE;
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

static Eina_Bool
_GPIOExists(int pin)
{
   if (_is_test) return EINA_TRUE;
#define DIRECTION_MAX 35
   char path[DIRECTION_MAX];
   int fd;

   snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
   fd = open(path, O_WRONLY);
   if (fd != -1) close(fd);
   return (fd > 0);
}

static Eina_Bool
_GPIODirection(int pin, const char *dir)
{
   if (_is_test) return EINA_TRUE;
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
#if 0
static Eina_Bool
_GPIORead(int pin)
{
   if (_is_test) return EINA_TRUE;
   char path[VALUE_MAX];
   char value_str[3];
   int fd;

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

   return *value_str - '0';
}
#endif

static Eina_Bool
_GPIOWrite(int pin, int value)
{
   if (_is_test) return EINA_TRUE;
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

static void
_motor_configure(int motor_id, int in1, int in2)
{
   printf("Motor %d In1 %d in2 %d\n", motor_id, in1, in2);
   _GPIOWrite(motors[motor_id].in1_pin, !!in1);
   _GPIOWrite(motors[motor_id].in2_pin, !!in2);
}

static Eina_Bool
_conn_add(void *data EINA_UNUSED, int type EINA_UNUSED, void *event)
{
   Ecore_Con_Event_Client_Add *ev = event;

   if (_c) ecore_con_client_del(ev->client);
   else
     {
        _c = ev->client;
        printf("INFO: client added %p: %s\n", _c, ecore_con_client_ip_get(_c));
     }
   return ECORE_CALLBACK_DONE;
}

static Eina_Bool
_conn_del(void *data EINA_UNUSED, int type EINA_UNUSED, void *event)
{
   Ecore_Con_Event_Client_Del *ev = event;

   if (_c == ev->client)
     {
        _c = NULL;
        return ECORE_CALLBACK_DONE;
     }
   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_conn_data(void *data EINA_UNUSED, int type EINA_UNUSED, void *event)
{
   Ecore_Con_Event_Client_Data *ev = event;
   static int curr_pv = 0, curr_ph = 0;
   if (_c == ev->client && ev->size == 2)
     {
        int new_pv = 0, new_ph = 0;
        signed char *new_p = ev->data;
        printf("Pressure: (%d, %d)\n", new_p[0], new_p[1]);
        if (new_p[0] && !new_p[1])
          {
             new_pv = new_p[0];
          }
        else if (new_p[1] && !new_p[0])
          {
             new_ph = new_p[1];
          }
        else if (new_p[0] && new_p[1])
          {
             if (new_p[0] != curr_pv) new_pv = new_p[0];
             else new_ph = new_p[1];
          }
        curr_pv = new_p[0];
        curr_ph = new_p[1];
        if (!new_pv && !new_ph)
          {
             /* No move */
             _motor_configure(0, 0, 0);
             _motor_configure(1, 0, 0);
          }
        else if (new_pv)
          {
             /* Vertical move */
             _motor_configure(0, new_pv > 0, new_pv < 0);
             _motor_configure(1, new_pv < 0, new_pv > 0);
          }
        else if (new_ph)
          {
             /* Horizontal move */
             _motor_configure(0, new_ph < 0, new_ph > 0);
             _motor_configure(1, new_ph < 0, new_ph > 0);
          }
        return ECORE_CALLBACK_DONE;
     }
   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_server_launch()
{
   ecore_con_server_add(ECORE_CON_REMOTE_TCP, "0.0.0.0", CAR_PORT, NULL);
   ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_ADD, _conn_add, NULL);
   ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_DEL, _conn_del, NULL);
   ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_DATA, _conn_data, NULL);
   return EINA_TRUE;
}

int main()
{
   int i;
   eina_init();
   ecore_init();
   ecore_con_init();

   _is_test = !!getenv("CAR_TEST");
   /*
    * Enable GPIO pins
    */
   for (i = 0; i < 2; i++)
     {
        if (!_GPIOExport(motors[i].in1_pin) ||
              !_GPIOExport(motors[i].in2_pin)) goto end;
     }

   /*
    * Check GPIO pins are exported
    */
   for (i = 0; i < 2; i++)
     {
        while (!_GPIOExists(motors[i].in1_pin) ||
              !_GPIOExists(motors[i].in2_pin));
     }
   /*
    * Set GPIO directions
    */
   for (i = 0; i < 2; i++)
     {
        if (!_GPIODirection(motors[i].in1_pin, OUT) ||
              !_GPIODirection(motors[i].in2_pin, OUT)) goto end;
     }

   _server_launch();

   printf("Ready\n");
   elm_run();

end:
   /*
    * Disable GPIO pins
    */
   for (i = 0; i < 2; i++)
     {
        _GPIOUnexport(motors[i].in1_pin);
        _GPIOUnexport(motors[i].in2_pin);
     }

   ecore_con_shutdown();
   ecore_shutdown();
   eina_shutdown();
   return 0;
}
