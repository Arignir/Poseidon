# 🔱 Poseidon

Yet another work-in-progress kernel. Eh.

## Build dependencies

  * `make`
  * `grub-mkrescue` and `libisoburn` (usually packed with other binaries as `grub2` or `grub`)

## Buil the kernel/ISO

To build the iso, run:

```bash
make iso
```

The Makefile will first build the cross-compiler. This necessary step will probably take a while but will only be done once, the first time. Don't worry.

By default, the kernel is built using the `release` profile (optimisation flags on).
The profile can be switched to `debug` (debug flags on) by appending `profile=debug` at the end of any `make` commands.

Eg:
```
make iso profile=debug
```

The iso, kernel and cross-compiler will be located in the `target/` directory.

## Run on QEMU

If you want to run Poseidon on QEMU, run

```bash
make run
```

The ISO can also be fed to any virtual machine, it *should* work.
