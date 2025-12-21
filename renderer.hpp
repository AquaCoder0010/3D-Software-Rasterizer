#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TT_CLEAR_SCREEN "\033[2J"
#define TT_CURSOR_HOME  "\033[H"

struct BufferWindow {
  int w, h;
  char *buf;
};

struct Point {
  unsigned int x, y;  
};



static BufferWindow create(int w, int h, char fill) {
  BufferWindow c;
  
  c.w = w;
  c.h = h;
  c.buf = (char*)malloc(w * h);
  memset(c.buf, fill, w * h);

  return c;
}

static void destroy(BufferWindow *c) {
  free(c->buf);
  c->buf = NULL;
}

static void clear_window(BufferWindow *c, char fill) {
  memset(c->buf, fill, c->w * c->h);
}

static void draw_pixel(BufferWindow *c, unsigned int x, unsigned int y, char ch) {
  if (x < 0 || y < 0 || x >= c->w || y >= c->h) {
    printf("ERROR while drawing value\n");
    return;
  }
  c->buf[y * c->w + x] = ch;
}

static void draw_pixel(BufferWindow *c, Point p, char ch) {
  if (p.x < 0 || p.y < 0 || p.x >= c->w || p.y >= c->h) {
    printf("ERROR while drawing value\n");
    return;
  }
  c->buf[p.y * c->w + p.x] = ch;

}

static void draw_text(BufferWindow *c, int x, int y, const char *s) {
  for (int i = 0; s[i]; i++)
	  draw_pixel(c, x + i, y, s[i]);
}

static void draw_text(BufferWindow *c, Point p, const char *s) {
  for (int i = 0; s[i]; i++)
	  draw_pixel(c, p.x + i, p.y, s[i]);
}



static void render(BufferWindow *c) {
  // ANSI codes
  printf("\033[2J \033[H");
  for (int y = 0; y < c->h; y++) {
	fwrite(&c->buf[y * c->w], 1, c->w, stdout);
	putchar('\n');
  }
  fflush(stdout);
}



///* Bresenham line */
//static void tt_line(TT_Canvas *c, int x0, int y0, int x1, int y1, char ch) {
//  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
//  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
//  int err = dx + dy, e2;
//
//  while (1) {
//	tt_pixel(c, x0, y0, ch);
//	if (x0 == x1 && y0 == y1) break;
//	e2 = 2 * err;
//	if (e2 >= dy) { err += dy; x0 += sx; }
//	if (e2 <= dx) { err += dx; y0 += sy; }
//  }
//}
//
//static void tt_rect(TT_Canvas *c, int x, int y, int w, int h, char ch) {
//  for (int i = 0; i < w; i++) {
//	tt_pixel(c, x + i, y, ch);
//	tt_pixel(c, x + i, y + h - 1, ch);
//  }
//  for (int i = 0; i < h; i++) {
//	tt_pixel(c, x, y + i, ch);
//	tt_pixel(c, x + w - 1, y + i, ch);
//  }
//}
//

