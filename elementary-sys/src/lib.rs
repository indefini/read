extern crate libc;

use libc::{c_void, c_int, c_char, c_float};//, c_ulong, c_long, c_uint, c_uchar, size_t};

pub type AnimCb = extern fn(data : *mut c_void) -> bool;
pub type RustCb = extern fn(data : *mut c_void);
pub type PressedCb = extern fn(data : *mut c_void, device : c_int, x : c_int, y : c_int);

#[repr(C)]
pub struct Keyboard;
#[repr(C)]
pub struct Evas_Object;
#[repr(C)]
pub struct Ecore_Animator;


extern "C" {
    pub fn init();
    pub fn run();
    pub fn kexit();
    pub fn reduce();

    pub fn window_new() -> *const Evas_Object;
    pub fn ui_create(win : *const Evas_Object);

    pub fn ecore_animator_add(cb : AnimCb, data :*const c_void) -> *const Ecore_Animator;
    pub fn ecore_animator_del(animator : *const Ecore_Animator);
}

