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

The Makefile will first build the cross-compiler. This step might take a while but will only be done once.

By default, the kernel is built using the `release` target (optimisation flags on).
The target can be switched to `debug` (debug flags on) by appending `target=debug` at the end of the previous commands.

Eg:
```
make iso target=debug
```

The iso, kernel and cross-compiler will be located in the `target/` directory.

## Run on QEMU

If you want to run Poseidon on QEMU, run

```bash
make run
```

The ISO can also be fed to any virtual machine, it *should* work.
