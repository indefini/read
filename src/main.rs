extern crate libc;
extern crate elementary_sys as elm;

use std::cmp;
use std::ptr;
//use libc::{c_void, c_int, c_char, c_ulong, c_long, c_uint, c_uchar, size_t};
use libc::{c_void, c_int, size_t, c_char};
use std::mem;
use std::path::{Path, PathBuf};
use std::fs;

use std::ffi::{CString, CStr};
use std::str;


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

pub fn get_files_in_dir(path : &str) -> Vec<PathBuf>
{
    let files = fs::read_dir(path).unwrap();
    /*
    for file in files {
        println!("Name: {}", file.unwrap().path().display())
    }
    */

    files.map(|x| x.unwrap().path()).collect()
}

pub fn to_cstring(v : Vec<PathBuf>) -> Vec<CString>
{
    v.iter().map(|x| CString::new(x.to_str().unwrap()).unwrap()).collect()
}

pub fn string_to_cstring(v : Vec<String>) -> Vec<CString>
{
    v.iter().map(|x| CString::new(x.as_str()).unwrap()).collect()
}

/*
pub fn print_vec_cstring(v : Vec<CString>)
{
    let y : Vec<*const c_char> = v.iter().map( |x| x.as_ptr()).collect();

    unsafe { elm::do_something_with_slice(
            y.as_ptr() as *const c_void,
            y.len() as size_t); }
}
*/


