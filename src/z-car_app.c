#include "common.h"

static Ecore_Con_Client *_c = NULL;

static Eina_Bool
_conn_add(void *data EINA_UNUSED, int type EINA_UNUSED, void *event)
{
   Ecore_Con_Event_Server_Add *ev = event;
   printf("Server with ip %s connected!\n", ecore_con_server_ip_get(ev->server));

   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_conn_del(void *data EINA_UNUSED, int type EINA_UNUSED, void *event)
{
   Ecore_Con_Event_Server_Del *ev = event;
   printf("Lost server with ip %s!\n", ecore_con_server_ip_get(ev->server));
   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_server_connect(const char *name)
{
   Ecore_Con_Server *svr = ecore_con_server_connect(ECORE_CON_REMOTE_TCP, name, CAR_PORT, NULL);
   ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD, _conn_add, NULL);
   ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL, _conn_del, NULL);
}

int main()
{
   eina_init();
   ecore_init();
   ecore_con_init();

   _server_connect("127.0.0.1");

   elm_run();

   ecore_con_shutdown();
   ecore_shutdown();
   eina_shutdown();
   return 0;
}
