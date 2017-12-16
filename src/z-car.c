#include <stdio.h>
#include <stdlib.h>

#include "common.h"

static Ecore_Con_Client *_c = NULL;

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

static void
_motor_configure(int motor_id, int in1, int in2)
{
   printf("Motor %d In1 %d in2 %d\n", motor_id, in1, in2);
   GPIOWrite(motors[motor_id].in1_pin, !!in1);
   GPIOWrite(motors[motor_id].in2_pin, !!in2);
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
             else if (new_p[1] != curr_ph) new_ph = new_p[1];
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

   /*
    * Enable GPIO pins
    */
   for (i = 0; i < 2; i++)
     {
        if (!GPIOExport(motors[i].in1_pin) ||
              !GPIOExport(motors[i].in2_pin)) goto end;
     }

   /*
    * Check GPIO pins are exported
    */
   for (i = 0; i < 2; i++)
     {
        while (!GPIOExists(motors[i].in1_pin) ||
              !GPIOExists(motors[i].in2_pin));
     }
   /*
    * Set GPIO directions
    */
   for (i = 0; i < 2; i++)
     {
        if (!GPIODirection(motors[i].in1_pin, OUT) ||
              !GPIODirection(motors[i].in2_pin, OUT)) goto end;
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
        GPIOUnexport(motors[i].in1_pin);
        GPIOUnexport(motors[i].in2_pin);
     }

   ecore_con_shutdown();
   ecore_shutdown();
   eina_shutdown();
   return 0;
}
