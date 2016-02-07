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
  //Evas_Object* win = elm_win_add(NULL, "keyboard", ELM_WIN_BASIC);
  Eo* win = elm_win_util_standard_add("read", "read");
  evas_object_name_set(win, strdup("win"));
  elm_win_autodel_set(win, EINA_TRUE);
  evas_object_smart_callback_add(win, "delete,request", _window_del, NULL);

  Evas_Object* bg = elm_bg_add(win);
  evas_object_name_set(bg, strdup("bg"));
  evas_object_show(bg);
  evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_win_resize_object_add(win, bg);

  evas_object_resize(win, 300,500);
  evas_object_show(win);
  return win;
}

Eo* img;
rust_cb _previous;
rust_cb _next;
void* _data;

void ui_create(Evas_Object* win, void* data, rust_cb previous, rust_cb next)
{
  _previous = previous;
  _next = next;
  _data = data;

  Eo* box = elm_box_add(win);
  evas_object_show(box);
  evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_win_resize_object_add(win, box);

  img = elm_image_add(win);
  const char* path = "/home/chris/tmp/08/001.jpg";
  elm_image_file_set(img, path, NULL);
  evas_object_size_hint_weight_set(img, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(img, EVAS_HINT_FILL, EVAS_HINT_FILL);

  elm_box_pack_end(box, img);
  evas_object_show(img);

  Eo* gl = elm_gesture_layer_add(img);
  elm_gesture_layer_hold_events_set(gl, EINA_TRUE);
  elm_gesture_layer_attach(gl, img);//po->hit);

  /*
  Eo *po;
  Photo_Object *po;
  po = calloc(1, sizeof(*po));
  po->base_zoom = po->zoom = BASE_ZOOM;

  elm_gesture_layer_cb_set(gl, ELM_GESTURE_ZOOM, ELM_GESTURE_STATE_START, zoom_start, po);
  elm_gesture_layer_cb_set(gl, ELM_GESTURE_ZOOM, ELM_GESTURE_STATE_MOVE, zoom_move, po);
  elm_gesture_layer_cb_set(gl, ELM_GESTURE_ZOOM, ELM_GESTURE_STATE_END, zoom_end, po);
  elm_gesture_layer_cb_set(gl, ELM_GESTURE_ZOOM, ELM_GESTURE_STATE_ABORT, zoom_end, po);
  */
}

void show_image(const char* path)
{
  elm_image_file_set(img, path, NULL);
}

