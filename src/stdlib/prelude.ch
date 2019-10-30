ignoreconst {

  // Cache some internal methods
  const __internal_get_method = Charly.internals.get_method
  const __internal_write = __internal_get_method("write")
  const __internal_getn = __internal_get_method("getn")
  const __internal_import = __internal_get_method("import")
  const __internal_defer = __internal_get_method("defer")
  const __internal_defer_interval = __internal_get_method("defer_interval")
  const __internal_clear_timer = __internal_get_method("clear_timer")
  const __internal_clear_interval = __internal_get_method("clear_interval")
  const __internal_exit = __internal_get_method("exit")

  let __internal_standard_libs_names
  let __internal_standard_libs

  // Import method for loading other files and libraries
  __charly_internal_import = ->(path, source) {

    // Search for the path in the list of standard libraries
    let is_stdlib = false
    let i = 0
    while (i < __internal_standard_libs_names.length) {
      const value = __internal_standard_libs_names[i]
      if (value == path) {
        is_stdlib = true
        break
      }

      i += 1
    }

    if (is_stdlib) {
      return __internal_standard_libs[path]
    }

    __internal_import(path, source)
  }

  // The names of all standard libraries that come with charly
  __internal_standard_libs_names = [
    "math"
  ]

  // All libraries that come with charly
  __internal_standard_libs = {
    math: import "_charly_math"
  }

  // Write a value to stdout, without a trailing newline
  write = func write {
    arguments.each(->(a) __internal_write(a.to_s()))
    null
  }

  // Write a value to stdout, with a trailing newline
  print = func print {
    arguments.each(->(v) __internal_write(v.to_s()))
    __internal_write("\n")
    null
  }

  // Exits the program with a given status code
  exit = func exit {
    __internal_exit(arguments.length ? $0 : 0)
  }

  // Defers the execution of a block
  // Internally this adds a new task to the vms internal task queue
  // Once all other remaining tasks have been executed this callback will
  // be invoked
  defer = func defer(cb) {
    __internal_defer(cb, arguments.length > 1 ? $1 : 0);
  }
  defer.interval = func interval(cb, period) {

    // Count how often the callback has been called
    let c = 0
    __internal_defer_interval(->{
      cb(c)
      c += 1
    }, period)
  }
  defer.clear_timer = __internal_clear_timer
  defer.clear_interval = __internal_clear_interval

  // Setup the charly object
  Charly.io = {
    write,
    print,
    getn: ->(msg) {
      write(msg)
      return __internal_getn()
    },
    dirname: __internal_get_method("dirname")
  }

  // Method to modify the primitive objects
  const set_primitive_object = Charly.internals.get_method("set_primitive_object")
  const set_primitive_class = Charly.internals.get_method("set_primitive_class")
  const set_primitive_array = Charly.internals.get_method("set_primitive_array")
  const set_primitive_string = Charly.internals.get_method("set_primitive_string")
  const set_primitive_number = Charly.internals.get_method("set_primitive_number")
  const set_primitive_function = Charly.internals.get_method("set_primitive_function")
  const set_primitive_generator = Charly.internals.get_method("set_primitive_generator")
  const set_primitive_boolean = Charly.internals.get_method("set_primitive_boolean")
  const set_primitive_null = Charly.internals.get_method("set_primitive_null")

  const Value = (import "_charly_value")()

  Object = set_primitive_object((import "_charly_object")(Value));
  Number = set_primitive_number((import "_charly_number")(Value));
  Array = set_primitive_array((import "_charly_array")(Value));
  Class = set_primitive_class((import "_charly_class")(Value));
  String = set_primitive_string((import "_charly_string")(Value));
  Function = set_primitive_function((import "_charly_function")(Value));
  Generator = set_primitive_generator((import "_charly_generator")(Value));
  Boolean = set_primitive_boolean((import "_charly_boolean")(Value));
  Null = set_primitive_null((import "_charly_null")(Value));
}
