#include "common.h"

static Ecore_Con_Server *_s = NULL, *_l = NULL;

static Eina_Bool
_conn_add(void *data EINA_UNUSED, int type EINA_UNUSED, void *event)
{
   Ecore_Con_Event_Server_Add *ev = event;
   printf("Server with ip %s connected!\n", ecore_con_server_ip_get(ev->server));
   _s = ev->server;

   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_conn_del(void *data EINA_UNUSED, int type EINA_UNUSED, void *event)
{
   Ecore_Con_Event_Server_Del *ev = event;
   printf("Lost server with ip %s!\n", ecore_con_server_ip_get(ev->server));
   if (_s == ev->server) _s = NULL;
   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_keyboard_event(void *data EINA_UNUSED, int type, void *event)
{
   static unsigned int _up_key = 0, _down_key = 0;
   static int _up_down_pressure = 0;
   Ecore_Event_Key *e = event;

   if (!_up_key && !strcmp(e->keyname, "Up")) _up_key = e->keycode;
   if (!_down_key && !strcmp(e->keyname, "Down")) _down_key = e->keycode;

   int old_pressure = _up_down_pressure;
   /* Released */
   if (type == ECORE_EVENT_KEY_UP)
     {
        if (e->keycode == _up_key && _up_down_pressure > 0) _up_down_pressure = 0;
        if (e->keycode == _down_key && _up_down_pressure < 0) _up_down_pressure = 0;
     }

   /* Pressed */
   if (type == ECORE_EVENT_KEY_DOWN)
     {
        if (e->keycode == _up_key && !_up_down_pressure) _up_down_pressure = 127;
        if (e->keycode == _down_key && !_up_down_pressure) _up_down_pressure = -128;
     }

   if (old_pressure != _up_down_pressure)
      printf("Pressure: %d -> %d\n", old_pressure, _up_down_pressure);

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_server_connect(const char *name)
{
   _l = ecore_con_server_connect(ECORE_CON_REMOTE_TCP, name, CAR_PORT, NULL);
   ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD, _conn_add, NULL);
   ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL, _conn_del, NULL);

   return EINA_TRUE;
}

int main(int argc, char **argv)
{
   eina_init();
   ecore_init();
   ecore_con_init();
   elm_init(argc, argv);

   _server_connect("127.0.0.1");

   ecore_event_handler_add(ECORE_EVENT_KEY_DOWN, _keyboard_event, NULL);
   ecore_event_handler_add(ECORE_EVENT_KEY_UP, _keyboard_event, NULL);

   Eo *win = elm_win_add(NULL, "App", ELM_WIN_BASIC);
   evas_object_resize(win, 200, 200);
   evas_object_show(win);
   elm_run();

   elm_shutdown();
   ecore_con_shutdown();
   ecore_shutdown();
   eina_shutdown();
   return 0;
}
