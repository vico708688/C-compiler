# MiniC Compiler

A simple compiler for a subset of the C language (MiniC).  
This project performs **lexical and syntax analysis** of MiniC source files.

---

## Requirements

Make sure the following tools are installed:

- `gcc` or `clang`
- `make`

---

## Build

### Debug mode

Compile the project in debug mode:

```bash
make debug
```

This mode prints the results of **lexical analysis** and **syntax analysis**.

### Normal mode

Compile the project normally:

```bash
make
```

Currently, no additional compilation flags are available.

---

## Usage

To analyze a MiniC source file:

```bash
./build/main <file.mc>
```

Example:

```bash
./build/main source_code/code.mc
```

---

## Project Structure

```
.
├── build/          # Compiled binaries
├── source/         # Source code
├── include/        # Headers
├── Makefile
└── README.md
```

---

## Notes

- The compiler currently supports **basic MiniC syntax analysis**.
- Additional features will be added in future versions.
