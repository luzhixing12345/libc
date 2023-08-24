
# prinf

左对齐 `-`

补零 %016lx

```c
printf("  %-15s0x%016lx 0x%016lx 0x%016lx\n", phdr_type, phdr[i].p_offset, phdr[i].p_vaddr, phdr[i].p_paddr);
```