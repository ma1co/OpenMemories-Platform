.eabi_attribute Tag_ABI_align_preserved, 1

.macro T name
  .global \name
  .type \name, "function"
  \name:
.endm
