# *patches* directory
This directory contains patched files of libraries and SDK for **ESP32 C3** target.
- **error_constants.h**  
  copy this file to *~/Arduino15/packages/esp32/tools/riscv32-esp-elf-gcc/esp-2021r2-patch5-8.4.0/riscv32-esp-elf/include/c++/8.4.0/bits*.
- **LegoinoCommon.h**  
  copy this file to *~/Arduino/libraries/Legoino/src*.

For Linux:
```bash
cp error_constants.h ~/Arduino15/packages/esp32/tools/riscv32-esp-elf-gcc/esp-2021r2-patch5-8.4.0/riscv32-esp-elf/include/c++/8.4.0/bits
cp LegoinoCommon.h ~/Arduino/libraries/Legoino/src
```
