extern crate libc;
extern crate elementary_sys as elm;

use std::cmp;
use std::ptr;
use std::ffi::CString;
//use libc::{c_void, c_int, c_char, c_ulong, c_long, c_uint, c_uchar, size_t};
use libc::{c_char, c_void, c_int};
use std::mem;

fn main() {
    unsafe { elm::init() };

    let win = unsafe {elm::window_new()};
    unsafe {elm::ui_create(win)};

    unsafe {
        elm::run();
    }
}

fn cstring_new(s : &str) -> *const c_char
{
    //let to_print = &b"Hello, world!"[..];
    CString::new(s).unwrap().as_ptr()
}

extern fn close(data : *mut c_void) {
    unsafe { elm::kexit() };
}

