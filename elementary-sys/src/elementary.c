#include <Elementary.h>
#include "common.h"

void init()
{
  elm_init(0,0);
}

void run()
{
  elm_run();
  elm_shutdown();
}

void kexit()
{
  elm_exit();
}

static void
_window_del(void *data, Evas_Object *obj, void *event_info)
{
  elm_exit();
}

Evas_Object* window_new()
{
  Evas_Object* win = elm_win_add(NULL, "keyboard", ELM_WIN_BASIC);
  evas_object_name_set(win, strdup("win"));
  elm_win_autodel_set(win, EINA_TRUE);
  evas_object_smart_callback_add(win, "delete,request", _window_del, NULL);

  Evas_Object* bg = elm_bg_add(win);
  evas_object_name_set(bg, strdup("bg"));
  evas_object_show(bg);
  evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_win_resize_object_add(win, bg);

  elm_win_screen_constrain_set(win, EINA_TRUE);

  evas_object_show(win);
  return win;
}

