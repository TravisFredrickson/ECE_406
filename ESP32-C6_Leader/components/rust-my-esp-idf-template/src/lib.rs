/*##############################################################
 * FUNCTIONS
 *############################################################*/

/*--------------------------------------------------------------
 * hello_from_rust()
 *------------------------------------------------------------*/

#[no_mangle]
extern "C" fn hello_from_rust() -> i32 {
    /* Do something unique to this Rust code compared to the rest of the project. */
    log::info!("Hello from Rust!");

    /* `42` is a conventional placeholder value. */
    return 42;
}

/*##############################################################
 * MAIN
 *############################################################*/
 
 /* `#[no_mangle]` and `extern "C"` format the ABI to be C compatible. */
#[no_mangle]
extern "C" fn rust_main() -> i32 {
    /* It is necessary to call this function once. Otherwise some patches to the runtime. */
    /* Implemented by esp-idf-sys might not link properly. See https://github.com/esp-rs/esp-idf-template/issues/71. */
    esp_idf_svc::sys::link_patches();

    /* Bind the log crate to the ESP Logging facilities. */
    esp_idf_svc::log::EspLogger::initialize_default();

    /* Print something to indicate Rust is working. */
    log::info!("Hello world from Rust's `app_main()`!");

    /* `42` is a conventional placeholder value. */
    return 42;
}
