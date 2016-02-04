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
use std::env;

struct Folder {
  files :  Vec<PathBuf>,
  index : usize
}

impl Folder 
{
    pub fn new(path : &Path) -> Self
    {
        Folder {
            files : get_files_in_dir(path),
            index : 0
        }
    }

    pub fn get_first(&self) -> Option<CString>
    {
        if !self.files.is_empty() {
            Some(CString::new(self.files[0].to_str().unwrap()).unwrap())
        }
        else {
            None
        }
    }

    pub fn next(&mut self) -> Option<CString>
    {
        if let Some(n) = self.files.get(self.index + 1) {
            self.index = self.index + 1;
            Some(CString::new(n.to_str().unwrap()).unwrap())
        }
        else {
            None
        }
    }

    pub fn previous(&mut self) -> Option<CString>
    {
        if self.index == 0 {
            return None;
        }

        if let Some(n) = self.files.get(self.index - 1) {
            self.index = self.index - 1;
            Some(CString::new(n.to_str().unwrap()).unwrap())
        }
        else {
            None
        }
    }
}

fn main() {
    unsafe { elm::init() };
    let mut args = env::args();
    let path = if let Some(p) = args.nth(1) {
        PathBuf::from(p)
    }
    else {
        env::current_dir().unwrap()
    };

    println!("argument : {:?}", path);
    let folder = Folder::new(path.as_ref());

    let win = unsafe {elm::window_new()};
    unsafe {
        /*
        elm::ui_create(
            win,
            mem::transmute(&folder),
            previous_page,
            next_page)}
            */

        let sl = elm::_slideshow_create(
            win,
            mem::transmute(&folder),
            previous_page,
            next_page);

        for img in &folder.files
        {
            let s = CString::new(img.to_str().unwrap()).unwrap();
            elm::image_add(sl, s.as_ptr());
        }
}

/*
    if let Some(s) = folder.get_first() {
        unsafe { elm::show_image(s.as_ptr()); }
    }
    */

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

pub fn get_files_in_dir(path : &Path) -> Vec<PathBuf>
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

pub extern fn next_page(folder : *mut c_void)// Folder)
{
    let folder : &mut Folder = unsafe {mem::transmute(folder)};
    println!("next: {}", folder.index);

    if let Some(s) = folder.next() {
        unsafe { elm::show_image(s.as_ptr()); }
    }

}

pub extern fn previous_page(folder : *mut c_void) //Folder)
{
    let folder : &mut Folder = unsafe {mem::transmute(folder)};
    println!("previous: {}", folder.index);

    if let Some(s) = folder.previous() {
        unsafe { elm::show_image(s.as_ptr()); }
    }

}



