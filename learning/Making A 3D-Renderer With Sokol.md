---
id: Making A 3D-Renderer With Sokol
aliases: Following Tutorial By 'Coding with Sphere' - 3D Renderer in C using Sokol Library
tags:
  - C
author: S.Sunhaloo
date: 2026-06-22
status: In-Progress
---

## List of Contents

- [[#What is Sokol?]]
- [[#The Coding with Sphere Tutorial]]
	- [[#First Video Tutorial]]
		- [[#Setup Project Structure]]
		- [[#Initialise Window]]
		- [[#Deviating From Coding with Sphere]]

---

# What is Sokol?

> [!INFO] Resource(s)
> 
> - Sokol GitHub Repository: https://github.com/floooh/sokol

Compared to [raylib](https://www.raylib.com/) that we used in our, "*semi-failed*" [mouse-c-py](https://github.com/Sunhaloo/mouse-c-py) project. Sokol is much *lighter* both in terms of **file size** and also provides little to **no abstraction** over the GPU API.

raylib can be seen as "*batteries-included*" while to be able to work with Sokol; you are going to have to understand and build the whole rendering pipeline in order to "*draw*" stuff on the screen.

For example, when we did our little GUI with raylib, we cloned the entire repository and then ran the *correct* `make` command to build the library file on our system. Here, there is **nothing** to *build*, its just a "*grab what you need*" type-of-thing.

# The Coding with Sphere Tutorial

Here are the list of videos made by [Coding with Sphere](https://www.youtube.com/@codingwithsphere) that we are going to be following in order:

1. Introduction and Setup: https://www.youtube.com/watch?v=GCnipL4T0Ho
2. Creating Triangle: https://www.youtube.com/watch?v=FFpSEo3geL4
3. Going 3D: https://www.youtube.com/watch?v=e23SJ-6zUrk

## First Video Tutorial

### Setup Project Structure

Below you are going to see how I setup the project structure for this tutorial. Compared to his project structure, I simply renamed the `vender` directory to `dependencies` as its more remember-able for me.

- Here is how my project structure is looking like:

```console
 .
├── 󱧼 build
├──  dependencies
│   └──  sokol
└──  main.c
```

> For the `sokol` folder, we simply `clone` the entire Sokol project... I think you know how to do that!

> [!NOTE]
> The main Sokol **header file** is the `sokol_app.h` file. Similar to what I said about how we should take a look at it even if we don't understand anything.
> 
> > You should definitely take a look and read it as I think the comments are really good for educational purposes!

### Initialise Window

#### Include Sokol's Header File

- Here is what our `main.c` file should have in order to use `sokol_app.h`:

```C
#define SOKOL_APP_IMPL
#define SOKOL_GLCORE
#define SOKOL_NO_ENTRY
#include "dependencies/sokol/sokol_app.h"
```

I need to explain how the preprocessor works before we move onto explaining the above "*includes*" as I also did not explain myself that when I was building our GUI for mouse-c-py.

So let's say that we defined this macro at the top of our file:

```C
#define COUNT 5
#define LEN(arr) (sizeof(arr) / sizeof(arr[0]))
```

Therefore, we can use `COUNT` instead of '5' wherever we need to, for example in a `for` loop and also; whenever we need to find the size of an array, we could simply pass the array into the `LEN` macro instead of typing the long `sizeof` *thingy* to find the length of said array.

What the preprocessor is going to do when it simply read our code; its going to go about and **replace** these *macros* like `COUNT` and `LEN` into the actual *values* ( which in this case is '5' and `sizeof(arr) / sizeof(arr[0])` ).

- But we can also *guard* and do *conditional compilation* like so:

> I am just using the `sokol_app.h` as example, in this case.

```C
// declaration guard: stops declarations being pasted twice into this file
#ifndef SOKOL_APP_INCLUDED
#define SOKOL_APP_INCLUDED
// function declarations / structs / enums go here - always visible
#endif

// implementation guard: controls whether the actual code gets compiled here
#if defined(SOKOL_APP_IMPL)
// actual function bodies go here - only visible in the ONE file that defines SOKOL_APP_IMPL
#endif
```

> [!WARNING]
> The above code block was given by [Claude](https://claude.ai)!
> 
> I think it's good but still given that I don't really know what I am doing... I need to ask someone knowledgeable about it.

- So right now in our `main.c` file we have the following line of *code*:

```C
#define SOKOL_APP_IMPL
```

This basically means that our `main.c` file "*owns*" the actual implementation of the function found inside the `sokol_app.h` header file. Therefore, let's say that we have another `test.c` file whereby we only have the **actual** `include "<path to sokol_app.h>"` file... The **linker** is going to check where the functions, we are using in `test.c`, are actually **defined** at.

> [!WARNING]
> If we were to use multiple `#define SOKOL_APP_IMPL`; the linker would not actually know what to choose from and it will result in a "*multiple definition*" error.
> 
> > Its like it does **not** have a source of truth!

- Therefore, at the end of this; we should have a `main.c` file that looks something like this:

```C
// own the function implementation found in sokol's header file
#define SOKOL_APP_IMPL
// Linux: using OpenGL's API to communicate with GPU
#define SOKOL_GLCORE
// NOTE: please read line 1154 of "our" `sokol_app.h` header file
#define SOKOL_NO_ENTRY
// include the actual sokol header file
#include "dependencies/sokol/sokol_app.h"

// our main function
int main() { return 0; }
```

#### Creating The Window

> [!NOTE]
> You are going to hear 'Coding with Sphere' talk about **callbacks** when he is showing us what to pass inside the `sapp_run` function.
> 
> > Do you remember function pointers?
> 
> Well, its the same thing... Instead of us calling and using the function directly, we simply pass **our** function that we defined in `sapp_desc` structure and then its going to get *called backed* whenever that point of time is reached.
> 
> > Or something like that.

So basically, compared to raylib and specially raygui whereby we did everything inside a `while` loop as it was an *immediate-mode-gui* library. Here we are going things the traditional way of whereby we are going to write functions / events that are going to be called at runtime.

- Therefore, here is how our `main.c` file currently looks like:

```C
// own the function implementation found in sokol's header file
#define SOKOL_APP_IMPL
// Linux: using OpenGL's API to communicate with GPU
#define SOKOL_GLCORE
// NOTE: please read line 1154 of "our" `sokol_app.h` header file
#define SOKOL_NO_ENTRY
// include the actual sokol header file
#include "dependencies/sokol/sokol_app.h"

// function related to `sapp_run` and `sapp_desc`
void init(void) {};
void frame(void) {};
void cleanup(void) {};
void event(const sapp_event *event) {};

// our main function
int main() {
  // initialise our application ==> windowing, GPU setup...
  sapp_run(&(sapp_desc){
    // setup main point
    .init_cb = init,
    .frame_cb = frame,
    .cleanup_cb = cleanup,
    .event_cb = event
  });

  return 0;
}
```

- Try compiling with the following command:

```bash
# compiling with `clang`
clang main.c -Wall -Wextra -lX11 -lXi -lXcursor -lGL -lasound -ldl -lm -o build/program
```

> [!NOTE]
> I have always used `gcc` to compile my `.c` file and I think I will continue to use it.
> 
> But I always... Most of the time, I see 'Coding with Sphere' uses `clang` more often that not. Asking Claude about it. It tells me that it is "*better*" ( *or should be at least* ).
> 
> GCC was build by the guys over at GNU Project while [Clang](https://clang.llvm.org/) was built on-top of [LLVM](https://llvm.org/) ( *ohh, its the dragon logo* ) which originated at Apple ( *I do hold a grudge against them* ).
> 
> Claude tells me that it give **better** warnings, should compile faster in larger projects and also lower memory use compared to GCC.

> [!IMPORTANT]
> They are completely different things that do happen to compile `.c` files.
> 
> What I am trying to say is that its **not** really an improvement over GCC. They did address the "*issues*" of GCC but they are like: "_Different people trying to solve the **same** problem in there own way_" type of thing.

> [!SUCCESS]
> Running our `./build/program` binary, I do see a black window appears!

#### Writing Makefile

Therefore, I am going to write a simple `Makefile` so that we can use instead of writing that long `clang` command.

```bash
CC = clang
OUTPUT = build/program

program: compile run clean

compile:
	@$(CC) main.c -Wall -Wextra -lX11 -lXi -lXcursor -lGL -lasound -ldl -lm -o $(OUTPUT)

run:
	@./build/program

clean:
	@rm ./build/program
```

### Deviating From Coding with Sphere

So looking and reading at the `sokol_app.h` **header file**. I am seeing a lot of interesting stuff that I think could be... Well, *interesting*.

After my mouse-c-py "*semi-failed*" project; I said to myself that I am **not** going to write Window's C code anymore.

> But I just can't help seeing and trying these interesting things.

Therefore, I am going to try to make this project also works on Windows.

#### Interesting Things That I Keep Seeing

Here are the interesting things that I keep seeing; therefore, making want to try to make this work on Windows systems using [MSYS2](https://www.msys2.org/).

- Selection of 3D-APIs:

```C
/*
    In the same place define one of the following to select the 3D-API
    which should be initialized by sokol_app.h (this must also match
    the backend selected for sokol_gfx.h if both are used in the same
    project):

        #define SOKOL_GLCORE
        #define SOKOL_GLES3
        #define SOKOL_D3D11
        #define SOKOL_METAL
        #define SOKOL_WGPU
        #define SOKOL_VULKAN
        #define SOKOL_NOAPI
*/
```

- Documentation of 3D-APIs on Linux:

```C
/*
    - on Linux:
        - all backends: X11, Xi, Xcursor, dl, pthread, m
        - with SOKOL_GLCORE: GL
        - with SOKOL_GLES3: GLESv2
        - with SOKOL_WGPU: a WebGPU implementation library (tested with webgpu_dawn)
        - with SOKOL_VULKAN: vulkan
        - with EGL: EGL

    On Linux, you also need to use the -pthread compiler and linker option, otherwise weird
    things will happen, see here for details: https://github.com/floooh/sokol/issues/376

    For Linux+Vulkan install the following packages (or equivalents):
        - libvulkan-dev
        - vulkan-validationlayers
        - vulkan-tools
*/
```

- Documentation of 3D-APIs on Windows:

```C
/*
    - on Windows:
        - with MSVC or Clang: library dependencies are defined via `#pragma comment`
        - with SOKOL_WGPU: a WebGPU implementation library (tested with webgpu_dawn)
        - with SOKOL_VULKAN:
            - install the Vulkan SDK
            - set a header search path to $VULKAN_SDK/Include
            - set a library search path to $VULKAN_SDK/Lib
            - link with vulkan-1.lib
        - with MINGW/MSYS2 gcc:
            - compile with '-mwin32' so that _WIN32 is defined
            - link with the following libs: -lkernel32 -luser32 -lshell32
            - additionally with the GL backend: -lgdi32
            - additionally with the D3D11 backend: -ld3d11 -ldxgi
*/
```

> [!TIP]
> I think we should give it a try!
> 
> > What's the worst that could happen... "*Famous Last Words*"

> [!INFO]
> > I am thoroughly going to use the help of Claude here for the Windows part because [Fuck Microsoft](https://www.youtube.com/watch?v=2zpCOYkdvTQ&t=22s)!
> 
> Okay, so apparently, and yes ( *what the fuck I am saying now... I need to take a break* ); Windows does support OpenGL so therefore, to compile and run our current `main.c` file, right now on Windows, we don't actually need to touch anything.
> 
> > Very Nice!
> 
> Nevertheless, we are going to have to **update** our `Makefile` so that it does allow compilation on Windows systems.

- Our updated `Makefile`:

```bash
# compiler to use
CC = clang

# check if we are on a Windows / Linux system
ifeq ($(OS),Windows_NT)
	# Windows system
  OUTPUT = build/program.exe
  LIBS = -lkernel32 -luser32 -lshell32 -lgdi32
else
	# Linux system
  OUTPUT = build/program
  LIBS = -lX11 -lXi -lXcursor -lGL -lasound -ldl -lm -pthread
endif

# local development ==> compiling, running and deleting
program: compile run clean

# compile the program according to system
compile:
	@$(CC) main.c -Wall -Wextra $(LIBS) -o $(OUTPUT)

# compile the program
run:
	@./$(OUTPUT)

# delete / remove any leftover compiled programs ( based on system )
clean:
	@$(RM) $(OUTPUT)
```

#### Creation of GitHub Repository

I am including this here as its going to be the first time that I am going to be using `git submodules`.

Here are all the steps that I took to create the GitHub Remote Repository:

1. Create GitHub repository
	- No `.gitignore`
	- License: MIT
2. Clone GitHub repository locally
3. Add required stuff like `main.c`, `Makefile` to repository
4. Add Git sub-module using: `git submodule add https://github.com/floooh/sokol dependencies/sokol`
	- This cloned the current latest version of Sokol
5. Pin down the current version so that my program does not break when someone tries to clone it
	- Pinning down to commit `28f9d8d44d92dab8536791a9f7d13d7e911a2b39` using `git checkout 28f9d8d44d92dab8536791a9f7d13d7e911a2b39`
	- Check using `git submodule status`
6. Add, Commit and Push
7. Clone again using either:
	- `git clone --recurse-submodules https://github.com/Sunhaoo/sokol-3D-renderer`
	- `git clone --recurse-submodules git@github.com:Sunhaoo/sokol-3D-renderer`

> [!WARNING]
> If you simply clone **without** the `--recurse-submodules` flag, then you are going to see the `dependencies/sokol` *repository* is going to be empty...
> 
> Therefore, you simply need to run the following command to populate it again:
> 
> ```bash
> # populate the `dependencies/sokol` repository
> git submodule update --init
> ```

---

# Socials

- **GitHub**: https://www.github.com/Sunhaloo
- **Instagram**: https://www.instagram.com/s.sunhaloo
- **YouTube**: https://www.youtube.com/@s.sunhaloo

---

S.Sunhaloo
Thank You!