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

  //evas_object_resize(win, 300,500);
  evas_object_resize(win, 60,100);
  evas_object_show(win);
  return win;
}

struct _Images
{
  Eina_List* before_str;
  Eina_List* before;
  Eo* current;
  Eina_List* after;
  Eina_List* after_str;

  Eo* win;
};
typedef struct _Images Images;


Images* _images;
Eo* img;
rust_cb _previous;
rust_cb _next;
void* _data;
Eo* _win;
Eo* _slide;
Eo* _box;


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
     //_next(_data);
     //elm_slideshow_next(_slide);
     
     if (_images) {
       image_next2(_images);
     }
   }
   else {
     //_previous(_data);
       image_previous2(_images);
     //elm_slideshow_previous(_slide);
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

static Evas_Event_Flags
zoom_start(void *data , void *event_info)
{
   Elm_Gesture_Zoom_Info *p = (Elm_Gesture_Zoom_Info *) event_info;
   //_color_and_icon_set(data, ZOOM_NAME, MAX_ZOOM, MAX_ZOOM, START_COLOR);
   printf("zoom started <%d,%d> zoom=<%f> radius=<%d> momentum=<%f>\n",
         p->x, p->y, p->zoom, p->radius, p->momentum);
   return EVAS_EVENT_FLAG_ON_HOLD;
}

static Evas_Event_Flags
zoom_move(void *data , void *event_info)
{
   Elm_Gesture_Zoom_Info *p = (Elm_Gesture_Zoom_Info *) event_info;
   //_color_and_icon_set(data, ZOOM_NAME, MAX_ZOOM, MAX_ZOOM, MOVE_COLOR);
   printf("zoom move <%d,%d> zoom=<%f> radius=<%d> momentum=<%f>\n",
         p->x, p->y, p->zoom, p->radius, p->momentum);
   return EVAS_EVENT_FLAG_ON_HOLD;
}

static Evas_Event_Flags
zoom_end(void *data , void *event_info)
{
   Elm_Gesture_Zoom_Info *p = (Elm_Gesture_Zoom_Info *) event_info;
   //_color_and_icon_set(data, ZOOM_NAME, MAX_ZOOM, MAX_ZOOM, END_COLOR);
   printf("zoom end <%d,%d> zoom=<%f> radius=<%d> momentum=<%f>\n",
         p->x, p->y, p->zoom, p->radius, p->momentum);
   return EVAS_EVENT_FLAG_ON_HOLD;
}

static Evas_Event_Flags
zoom_abort(void *data , void *event_info EINA_UNUSED)
{
   printf("zoom abort\n");
   //_color_and_icon_set(data, ZOOM_NAME, MAX_ZOOM, MAX_ZOOM, ABORT_COLOR);
   return EVAS_EVENT_FLAG_ON_HOLD;
}

static Evas_Event_Flags
flick_start(void *data , void *event_info)
{
   Elm_Gesture_Line_Info *p = (Elm_Gesture_Line_Info *) event_info;
   //_color_and_icon_set(data, FLICK_NAME, p->momentum.n, MAX_FLICK, START_COLOR);

   printf("flick started x1,y1=<%d,%d> tx,ty=<%u,%u> n=<%u>\n",
         p->momentum.x1, p->momentum.y1, p->momentum.tx,
         p->momentum.ty, p->momentum.n);
   return EVAS_EVENT_FLAG_ON_HOLD;
}

static Evas_Event_Flags
flick_end(void *data , void *event_info)
{
   Elm_Gesture_Line_Info *p = (Elm_Gesture_Line_Info *) event_info;
   //_color_and_icon_set(data, FLICK_NAME, p->momentum.n, MAX_FLICK, END_COLOR);

   printf("flick ended angle=%f, x1,y1=<%d,%d> x2,y2=<%d,%d> tx,ty=<%u,%u> mx=<%d> my=<%d> n=<%u>\n",
         p->angle,
         p->momentum.x1, p->momentum.y1,
         p->momentum.x2, p->momentum.y2,
         p->momentum.tx, p->momentum.ty,
         p->momentum.mx, p->momentum.my,
         p->momentum.n);

   if (p->momentum.mx < 0) {
     _next(_data);
     elm_slideshow_next(_slide);
     image_next2(_images);
   }
   else {
     _previous(_data);
     elm_slideshow_previous(_slide);
       image_previous2(_images);
   }


   return EVAS_EVENT_FLAG_ON_HOLD;
}

static Evas_Event_Flags
flick_abort(void *data , void *event_info)
{
   Elm_Gesture_Line_Info *p = (Elm_Gesture_Line_Info *) event_info;
   //_color_and_icon_set(data, FLICK_NAME, p->momentum.n, MAX_FLICK, ABORT_COLOR);
   printf("flick abort\n");
   return EVAS_EVENT_FLAG_ON_HOLD;
}




void _create_rect(Evas_Object* win)
{
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

  //TAP
  elm_gesture_layer_cb_set(g, ELM_GESTURE_N_TAPS,
        ELM_GESTURE_STATE_START, n_finger_tap_start, infra);
  elm_gesture_layer_cb_set(g, ELM_GESTURE_N_TAPS,
        ELM_GESTURE_STATE_END, n_finger_tap_end, infra);
  elm_gesture_layer_cb_set(g, ELM_GESTURE_N_TAPS,
        ELM_GESTURE_STATE_ABORT, n_finger_tap_abort, infra);

  //ZOOM
  elm_gesture_layer_cb_set(g, ELM_GESTURE_ZOOM,
        ELM_GESTURE_STATE_START, zoom_start, infra);
  elm_gesture_layer_cb_set(g, ELM_GESTURE_ZOOM,
        ELM_GESTURE_STATE_END, zoom_end, infra);
  elm_gesture_layer_cb_set(g, ELM_GESTURE_ZOOM,
        ELM_GESTURE_STATE_ABORT, zoom_abort, infra);
  elm_gesture_layer_cb_set(g, ELM_GESTURE_ZOOM,
        ELM_GESTURE_STATE_MOVE, zoom_move, infra);

  //FLICK
  elm_gesture_layer_cb_set(g, ELM_GESTURE_N_FLICKS,
        ELM_GESTURE_STATE_START, flick_start, infra);
  elm_gesture_layer_cb_set(g, ELM_GESTURE_N_FLICKS,
        ELM_GESTURE_STATE_END, flick_end, infra);
  elm_gesture_layer_cb_set(g, ELM_GESTURE_N_FLICKS,
        ELM_GESTURE_STATE_ABORT, flick_abort, infra);




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

Eo* _table;
void ui_create(Evas_Object* win, void* data, rust_cb previous, rust_cb next)
{
  _previous = previous;
  _next = next;
  _data = data;
  _win = win;

  _images = calloc(1, sizeof *_images);
  _images->win = win;

  Eo* box = elm_box_add(win);
  evas_object_show(box);
  evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_win_resize_object_add(win, box);
  _box = box;

  Eo* table = elm_table_add(win);
  evas_object_show(table);
  elm_table_homogeneous_set(table, EINA_TRUE);
  evas_object_size_hint_weight_set(table, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(table, EVAS_HINT_FILL, EVAS_HINT_FILL);
  elm_win_resize_object_add(win, table);

  _table = table;

  /*
  img = elm_image_add(win);
  _images->current = img;
  //const char* path = "/home/chris/tmp/08/001.jpg";
  //elm_image_file_set(img, path, NULL);
  evas_object_size_hint_weight_set(img, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(img, EVAS_HINT_FILL, EVAS_HINT_FILL);

  elm_box_pack_end(box, img);
  evas_object_show(img);
  */

  _create_rect(win);
}

static Elm_Slideshow_Item_Class itc;

static Evas_Object *
_get(void *data, Evas_Object *obj)
{
   printf("_get (item data: '%s')\n", (char*)data);
   Evas_Object *img = elm_image_add(obj);
   elm_image_file_set(img, data, NULL);
   elm_image_fill_outside_set(img, EINA_FALSE);

   return img;
}

static void
_del(void *data, Evas_Object *obj EINA_UNUSED)
{
   printf("_del (item data: '%s')\n", (char*)data);
}

Eo* _slideshow_create(Evas_Object* win, void* data, rust_cb previous, rust_cb next)
{
  _previous = previous;
  _next = next;
  _data = data;
  _win = win;

  Eo* slideshow = elm_slideshow_add(win);
  _slide = slideshow;
  elm_slideshow_loop_set(slideshow, EINA_TRUE);
  elm_slideshow_transition_set(slideshow, "None");


  evas_object_size_hint_weight_set(slideshow, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(slideshow, EVAS_HINT_FILL, EVAS_HINT_FILL);
  //evas_object_smart_callback_add(slideshow, "changed", _changed_cb, NULL);
  elm_win_resize_object_add(win, slideshow);
  evas_object_show(slideshow);

  itc.func.get = _get;
  itc.func.del = _del;

  _create_rect(win);

  return slideshow;
}

Eo* _create_image(Eo* win, const char* path)
{
  Eo* im = elm_image_add(win);
  elm_image_file_set(im, path, NULL);
  evas_object_size_hint_weight_set(im, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(im, EVAS_HINT_FILL, EVAS_HINT_FILL);

  elm_win_resize_object_add(win, im);

  elm_table_pack(_table, im, 0, 0, 1, 1);
  evas_object_lower(im);

  //elm_box_pack_end(_box, im);
  //evas_object_show(im);
  
  return im;
}

void image_add2(Images* images, const char* path)
{
  uint count = eina_list_count(images->after);

  printf("add2 %s, %d\n",path, count);

  if (count > 2) {
    images->after_str = 
     eina_list_append(images->after_str, strdup(path));

    return;
  }

  Eo* im = _create_image(images->win, path);

  if (!_images->current) {
    _images->current = im;
    evas_object_show(im);
    printf("showing image : %s\n", path);
  }
  else {
    images->after = eina_list_append(images->after, im);
  }
}

void _image_move(
      Images* images,
      Eina_List** b_str,
      Eina_List** b_img,
      Eina_List** a_str,
      Eina_List** a_img)
{
  uint count = eina_list_count(*a_img);

  if (count == 0) return;

  uint countbefore = eina_list_count(*b_img);

  if (countbefore > 2) {
    const char* dance;
    const char* group;
    Eo* im = eina_list_last_data_get(*b_img);
    elm_image_file_get(im, &dance, &group);
    *b_str = eina_list_prepend(*b_str, strdup(dance));
    
    *b_img = eina_list_remove_list(*b_img, eina_list_last(*b_img));

    evas_object_del(im);
  }

  evas_object_hide(images->current);
  *b_img = eina_list_prepend(*b_img, images->current);

  uint countafterstr = eina_list_count(*a_str);

  images->current = eina_list_data_get(*a_img);
  evas_object_show(images->current);

  *a_img = eina_list_remove_list(*a_img, *a_img);

  if (countafterstr > 0) {
    const char* path = eina_list_data_get(*a_str);
    Eo* im = _create_image(images->win, path);
    *a_img = eina_list_append(*a_img, im);
    *a_str = eina_list_remove_list(*a_str, *a_str);
  }
}


void image_next2(
      Images* images)
{
  _image_move(images, 
        &images->before_str,
        &images->before,
        &images->after_str,
        &images->after);
}

void image_previous2(
      Images* images)
{
  _image_move(images,
        &images->after_str,
        &images->after,
        &images->before_str,
        &images->before);
}




void image_clear(Eo* slideshow)
{
  elm_slideshow_clear(slideshow);
}



void show_image(const char* path)
{
  printf("show image : %s \n ", path);
  elm_image_file_set(img, strdup(path), NULL);
}

void image_add(Eo* slideshow, const char* path)
{
  if (slideshow) {
    Elm_Object_Item* item = 
     elm_slideshow_item_add(slideshow, &itc, strdup(path));
  }

  printf("images add %p \n", _images);
  if (!_images) return;
  image_add2(_images, path);
}

