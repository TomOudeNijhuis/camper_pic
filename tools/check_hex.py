def parse(p):
    mem, ext = {}, 0
    with open(p) as f:
        for l in f:
            l = l.strip()
            if not l.startswith(":"):
                continue
            n = int(l[1:3], 16)
            a = int(l[3:7], 16)
            t = int(l[7:9], 16)
            d = l[9 : 9 + 2 * n]
            if t == 4:
                ext = int(d, 16) << 16
                continue
            if t != 0:
                continue
            for i in range(n):
                mem[ext + a + i] = int(d[2 * i : 2 * i + 2], 16)
    return mem


chip = parse("readback2.hex")
src = parse("camper_pic/out/camper_pic/Bootloaded.hex")


def chip_word(w):
    b = w * 2
    return chip.get(b, 0xFF) | (chip.get(b + 1, 0x3F) << 8)


def src_word(w):
    b = w * 2
    if b in src or b + 1 in src:
        return src.get(b, 0xFF) | (src.get(b + 1, 0x3F) << 8)
    return 0x3FFF


# Verify the bootloader is correctly in boot block (0x0000..0x0FFF)
print("=== Boot block (0x0000..0x0FFF) snapshot ===")
print(f"  word 0x0000: 0x{chip_word(0):04X}")
print(f"  word 0x0001: 0x{chip_word(1):04X}")
print(f"  word 0x0002: 0x{chip_word(2):04X}")
print(f"  word 0x0FFF: 0x{chip_word(0xFFF):04X}")

# Check app region 0x1000..0x3FFF
print("\n=== Application region (0x1000..0x3FFF) ===")
print(f"  word 0x1000: 0x{chip_word(0x1000):04X}  (src=0x{src_word(0x1000):04X})")
print(f"  word 0x1001: 0x{chip_word(0x1001):04X}  (src=0x{src_word(0x1001):04X})")
print(
    f"  word 0x1004: 0x{chip_word(0x1004):04X}  (src=0x{src_word(0x1004):04X})  ← ISR"
)
print(
    f"  word 0x3FFE: 0x{chip_word(0x3FFE):04X}  (src=0x{src_word(0x3FFE):04X})  ← CRC lo"
)
print(
    f"  word 0x3FFF: 0x{chip_word(0x3FFF):04X}  (src=0x{src_word(0x3FFF):04X})  ← CRC hi"
)

# Diff app region word-by-word
diffs = [
    (w, src_word(w), chip_word(w))
    for w in range(0x1000, 0x4000)
    if chip_word(w) != src_word(w)
]
print(f"\n=== Word-level diffs in 0x1000..0x3FFF: {len(diffs)} ===")
for w, sv, cv in diffs[:20]:
    print(f"  word 0x{w:04X}: src=0x{sv:04X}  chip=0x{cv:04X}")
if len(diffs) > 20:
    print(f"  ... ({len(diffs) - 20} more)")

# Compute bootloader CRC over the CHIP's actual flash
crc, POLY = 0xFFFF, 0x1021
for w in range(0x1000, 0x3FFE):
    word = chip_word(w)
    for b in (word & 0xFF, (word >> 8) & 0xFF):
        crc ^= b << 8
        for _ in range(8):
            crc = ((crc << 1) ^ POLY) & 0xFFFF if crc & 0x8000 else (crc << 1) & 0xFFFF
stored = (chip_word(0x3FFE) & 0xFF) | ((chip_word(0x3FFF) & 0xFF) << 8)
print(f"\n=== Bootloader CRC over actual chip flash ===")
print(f"  computed: 0x{crc:04X}")
print(f"  stored  : 0x{stored:04X}")
print(
    f"  result  : {'PASS — bootloader should say VALID' if crc == stored else 'FAIL — bootloader will say INVALID'}"
)
