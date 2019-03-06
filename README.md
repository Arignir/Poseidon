# Poseidon

Yet another work-in-progress kernel. Eh.

## Build dependencies

  * `make`
  * `gcc` (>= 7)
  * `grub-mkrescue` and `libisoburn` (usually packed with other binaries as `grub2` or `grub`)

## Buil the kernel/ISO

To build the kernel, run

```bash
make kernel
```

To build a complete iso with grub installed (suitable for USB flash drives or virtual machines), run
```bash
make iso
```

By default, the kernel is built using the `release` target (optimisation flags on).
The target can be switched to `debug` (debug flags on) by appending `target=debug` at the end of the previous commands.

Eg:
```
make iso target=debug
```

The iso and the kernel will be located in the `target/` directory.

## Run on QEMU

If you want to run Poseidon on QEMU, run

```bash
make run
```

The ISO can also be fed to any virtual machine, it *should* work.
