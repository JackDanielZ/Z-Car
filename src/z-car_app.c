#include "common.h"

static Ecore_Con_Server *_s = NULL, *_l = NULL;
static Ecore_Timer *_joystick_pos_status_timer = NULL;
static Ecore_Timer *_spi_timer = NULL;
static Eina_Bool _joystick_pos_active = EINA_FALSE;

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
   static unsigned int _right_key = 0, _left_key = 0;
   static unsigned int _up_key = 0, _down_key = 0;
   static char _vpressure = 0, _hpressure = 0;
   Ecore_Event_Key *e = event;

   if (!_up_key && !strcmp(e->keyname, "Up")) _up_key = e->keycode;
   if (!_down_key && !strcmp(e->keyname, "Down")) _down_key = e->keycode;
   if (!_right_key && !strcmp(e->keyname, "Right")) _right_key = e->keycode;
   if (!_left_key && !strcmp(e->keyname, "Left")) _left_key = e->keycode;

   int old_vpressure = _vpressure, old_hpressure = _hpressure;
   /* Released */
   if (type == ECORE_EVENT_KEY_UP)
     {
        if (e->keycode == _up_key && _vpressure > 0) _vpressure = 0;
        if (e->keycode == _down_key && _vpressure < 0) _vpressure = 0;
        if (e->keycode == _right_key && _hpressure > 0) _hpressure = 0;
        if (e->keycode == _left_key && _hpressure < 0) _hpressure = 0;
     }

   /* Pressed */
   if (type == ECORE_EVENT_KEY_DOWN)
     {
        if (e->keycode == _up_key && !_vpressure) _vpressure = 127;
        if (e->keycode == _down_key && !_vpressure) _vpressure = -128;
        if (e->keycode == _right_key && !_hpressure) _hpressure = 127;
        if (e->keycode == _left_key && !_hpressure) _hpressure = -128;
     }

   if (old_vpressure != _vpressure || old_hpressure != _hpressure)
     {
        printf("Pressure: (%d, %d) -> (%d, %d)\n",
              old_vpressure, old_hpressure,
              _vpressure, _hpressure);
        if (_s)
          {
             char tmp[2];
             tmp[0] = _vpressure;
             tmp[1] = _hpressure;
             ecore_con_server_send(_s, tmp, 2);
             ecore_con_server_flush(_s);
          }
     }

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

#define SWITCH_PIN 17
#define SPI_MOSI_PIN 10
#define SPI_MISO_PIN 9
#define SPI_CLK_PIN 11
#define SPI_CS_PIN 5

static Eina_Bool
_spi_init(void)
{
   if (!GPIOExport(SPI_MOSI_PIN) || !GPIOExport(SPI_MISO_PIN) ||
         !GPIOExport(SPI_CLK_PIN) || !GPIOExport(SPI_CS_PIN) ||
         !GPIOExport(SWITCH_PIN))
      return EINA_FALSE;

   while (!GPIOExists(SPI_MOSI_PIN) || !GPIOExists(SPI_MISO_PIN) ||
         !GPIOExists(SPI_CLK_PIN) || !GPIOExists(SPI_CS_PIN) ||
         !GPIOExists(SWITCH_PIN));

   if (!GPIODirection(SPI_CLK_PIN, OUT) || !GPIODirection(SPI_MISO_PIN, IN) ||
         !GPIODirection(SPI_MOSI_PIN, OUT) || !GPIODirection(SPI_CS_PIN, OUT) ||
         !GPIODirection(SWITCH_PIN, IN))
      return EINA_FALSE;
   return EINA_TRUE;
}

static Eina_Bool
_spi_a2d_read(int channel, int *value)
{
   int i;
   GPIOWrite(SPI_CS_PIN, 1);
   GPIOWrite(SPI_CS_PIN, 0);
   GPIOWrite(SPI_CLK_PIN, 0);

   /* Start byte */
   GPIOWrite(SPI_MOSI_PIN, 0);
   for (i = 0; i < 7; i++)
     {
        GPIOWrite(SPI_CLK_PIN, 0);
        GPIOWrite(SPI_CLK_PIN, 1);
     }
   GPIOWrite(SPI_CLK_PIN, 0);
   GPIOWrite(SPI_MOSI_PIN, 1);
   GPIOWrite(SPI_CLK_PIN, 1);

   /* SGL/DIFF */
   GPIOWrite(SPI_CLK_PIN, 0);
   GPIOWrite(SPI_MOSI_PIN, 1);
   GPIOWrite(SPI_CLK_PIN, 1);

   /* D2 D1 D0 */
   for (i = 2; i >= 0; i--)
     {
        GPIOWrite(SPI_CLK_PIN, 0);
        GPIOWrite(SPI_MOSI_PIN, (channel & (1 << i)) >> i);
        GPIOWrite(SPI_CLK_PIN, 1);
     }

   /* 2 NULL bits + B9...B0 */
   *value = 0;
   for (i = 0; i < 12; i++)
     {
        char bit = 0;
        GPIOWrite(SPI_CLK_PIN, 1);
        GPIORead(SPI_MISO_PIN, &bit);
        GPIOWrite(SPI_CLK_PIN, 0);
        *value |= bit;
        *value <<= 1;
     }

   return EINA_TRUE;
}

static Eina_Bool
_spi_poll(void *data EINA_UNUSED)
{
   static signed char _vpressure = 0, _hpressure = 0;
   int valueX = 0, valueY = 0;
   int old_vpressure = _vpressure, old_hpressure = _hpressure;

   _spi_a2d_read(0, &valueX);
   _spi_a2d_read(1, &valueY);

   if (valueX < 400) _hpressure = -128;
   else if (valueX > 600) _hpressure = 127;
   else _hpressure = 0;

   if (valueY < 400) _vpressure = 127;
   else if (valueY > 600) _vpressure = -128;
   else _vpressure = 0;

   if (_hpressure || _vpressure) _joystick_pos_active = EINA_TRUE;

   if (old_vpressure != _vpressure || old_hpressure != _hpressure)
     {
        printf("Pressure: (%d, %d) -> (%d, %d)\n",
              old_vpressure, old_hpressure,
              _vpressure, _hpressure);
        if (_s)
          {
             char tmp[2];
             tmp[0] = _vpressure;
             tmp[1] = _hpressure;
             ecore_con_server_send(_s, tmp, 2);
             ecore_con_server_flush(_s);
          }
     }
   return EINA_TRUE;
}

static Eina_Bool
_joystick_pos_status_check(void *data EINA_UNUSED)
{
   static int n = 0;
   if (_joystick_pos_active)
     {
        _joystick_pos_active = EINA_FALSE;
        n = 10;
     }
   else
     {
        n--;
        if (!n)
          {
             ecore_timer_del(_spi_timer);
             _spi_timer = NULL;
             _joystick_pos_status_timer = NULL;
             return EINA_FALSE;
          }
     }
   return EINA_TRUE;
}

static Eina_Bool
_switch_changed_cb(void *data EINA_UNUSED, Ecore_Fd_Handler *h)
{
   char bit;
   int fd = ecore_main_fd_handler_fd_get(h);
   lseek(fd, 0, SEEK_SET);
   read(fd, &bit, 1);
   if (!_spi_timer)
     {
        _spi_timer = ecore_timer_add(0.1, _spi_poll, NULL); // change to 0.1
     }
   if (!_joystick_pos_status_timer)
     {
        _joystick_pos_status_timer = ecore_timer_add(1, _joystick_pos_status_check, NULL);
        _joystick_pos_active = EINA_TRUE;
     }
   return EINA_TRUE;
}

int main(int argc, char **argv)
{
   eina_init();
   ecore_init();
   ecore_con_init();
   common_init();
   elm_init(argc, argv);

   _server_connect(argc==2?argv[1]:"Car");

   if (is_test)
     {
        ecore_event_handler_add(ECORE_EVENT_KEY_DOWN, _keyboard_event, NULL);
        ecore_event_handler_add(ECORE_EVENT_KEY_UP, _keyboard_event, NULL);

        Eo *win = elm_win_add(NULL, "App", ELM_WIN_BASIC);
        evas_object_resize(win, 200, 200);
        evas_object_show(win);
     }
   else
     {
        _spi_init();
        int switch_fd = GPIO_fd_get_for_interrupt(SWITCH_PIN);
        ecore_main_fd_handler_add(switch_fd, ECORE_FD_ERROR, _switch_changed_cb, NULL, NULL, NULL);
     }
   elm_run();

   elm_shutdown();
   ecore_con_shutdown();
   ecore_shutdown();
   eina_shutdown();
   return 0;
}
