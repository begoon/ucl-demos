#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main() {
  unsigned char buf[30000];
  int sz = 0, i;
  FILE* f = fopen("UCL.COM", "r+b");
  assert(f != NULL);
  while (!feof(f)) {
    int read = fread(buf + sz + 0x100, 1, sizeof(buf) - sz - 0x100, f);
    assert(read != -1);
    if (read == 0) break;
    sz += read;
  }
  fclose(f);
  printf("Read %04X (%d) bytes\n", sz, sz);
  int jmp = *(unsigned short *)&buf[0x101] + 3 + 0x100;
  printf("Jump %04X\n", jmp);
  *(unsigned short *)&buf[0x100] = *(unsigned short *)&buf[jmp + 3];
  buf[0x102] = buf[jmp + 8];
  int cx = *(unsigned short *)&buf[jmp + 10];
  int di = *(unsigned short *)&buf[jmp + 13];
  printf("CX = %04X, DI = %04X\n", cx, di);
  int si = 0x100;
  for (i = 0; i < cx; ++i) {
    unsigned char ch = buf[si++];
    unsigned char p = buf[di + (i & 0x0f)];
    ch ^= p;
    int dx = si - 1;
    ch = (ch + (dx & 0xff));
    buf[si - 1] = ch;
  }
  f = fopen("UCL-DEC.COM", "w+b");
  assert(f != NULL);
  assert(fwrite(buf + 0x100, sz + 0x000, 1, f) != sz);
  fclose(f);
}
