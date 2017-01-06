#include "common.h"

static Ecore_Con_Client *_c = NULL;

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
   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_conn_del(void *data EINA_UNUSED, int type EINA_UNUSED, void *event)
{
   Ecore_Con_Event_Client_Del *ev = event;

   if (_c == ev->client)
     {
        _c = NULL;
     }
   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_server_launch()
{
   Ecore_Con_Server *srv = ecore_con_server_add(ECORE_CON_REMOTE_TCP, "127.0.0.1", CAR_PORT, NULL);
   ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_ADD, _conn_add, NULL);
   ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_DEL, _conn_del, NULL);
   return EINA_TRUE;
}

int main()
{
   eina_init();
   ecore_init();
   ecore_con_init();

   _server_launch();

   elm_run();

   ecore_con_shutdown();
   ecore_shutdown();
   eina_shutdown();
   return 0;
}
