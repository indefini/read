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
Eo* _win;


/* START - Callbacks for gestures */
static Evas_Event_Flags
n_finger_tap_start(void *data , void *event_info)
{
   Elm_Gesture_Taps_Info *p = (Elm_Gesture_Taps_Info *) event_info;
   printf("N tap started <%p> x,y=<%d,%d> count=<%d>\n",
         event_info, p->x, p->y, p->n);
   return EVAS_EVENT_FLAG_ON_HOLD;
}

static Evas_Event_Flags
n_finger_tap_end(void *data , void *event_info)
{
   Elm_Gesture_Taps_Info *p = (Elm_Gesture_Taps_Info *) event_info;
   printf("N tap ended <%p> x,y=<%d,%d> count=<%d>\n",
         event_info, p->x, p->y, p->n);

   Evas_Coord x,y,w,h;
   evas_object_geometry_get(_win, &x, &y, &w, &h);
   printf("x = %d, w = <%d>\n", p->x, w);
   if (p->x > w/2) {
     _next(_data);
   }
   else {
     _previous(_data);
   }

   return EVAS_EVENT_FLAG_ON_HOLD;
}

static Evas_Event_Flags
n_finger_tap_abort(void *data , void *event_info)
{
   Elm_Gesture_Taps_Info *p = (Elm_Gesture_Taps_Info *) event_info;
   printf("N tap abort\n");
   return EVAS_EVENT_FLAG_ON_HOLD;
}


void ui_create(Evas_Object* win, void* data, rust_cb previous, rust_cb next)
{
  _previous = previous;
  _next = next;
  _data = data;
  _win = win;

  Eo* box = elm_box_add(win);
  evas_object_show(box);
  evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_win_resize_object_add(win, box);

  img = elm_image_add(win);
  //const char* path = "/home/chris/tmp/08/001.jpg";
  //elm_image_file_set(img, path, NULL);
  evas_object_size_hint_weight_set(img, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(img, EVAS_HINT_FILL, EVAS_HINT_FILL);

  elm_box_pack_end(box, img);
  evas_object_show(img);


  Eo* r = evas_object_rectangle_add(evas_object_evas_get(win));
  evas_object_move(r, 0, 0);
  //evas_object_color_set(r, 100, 0, 0, 100);
  evas_object_color_set(r, 0, 0, 0, 0);
  elm_win_resize_object_add(win, r);
  evas_object_size_hint_weight_set(r, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(r, EVAS_HINT_FILL, EVAS_HINT_FILL);

  Eo* g = elm_gesture_layer_add(win);
  elm_gesture_layer_attach(g, r);
  evas_object_show(r);

  Eo* infra = NULL;

  elm_gesture_layer_cb_set(g, ELM_GESTURE_N_TAPS,
        ELM_GESTURE_STATE_START, n_finger_tap_start, infra);
  elm_gesture_layer_cb_set(g, ELM_GESTURE_N_TAPS,
        ELM_GESTURE_STATE_END, n_finger_tap_end, infra);
  elm_gesture_layer_cb_set(g, ELM_GESTURE_N_TAPS,
        ELM_GESTURE_STATE_ABORT, n_finger_tap_abort, infra);


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

void _slideshow_create(Eo* win)
{
  Eo* slideshow = elm_slideshow_add(win);

  evas_object_size_hint_weight_set(slideshow, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(slideshow, EVAS_HINT_FILL, EVAS_HINT_FILL);
  //evas_object_smart_callback_add(slideshow, "changed", _changed_cb, NULL);
  elm_win_resize_object_add(win, slideshow);
  evas_object_show(slideshow);
}

void show_image(const char* path)
{
  elm_image_file_set(img, path, NULL);
}

