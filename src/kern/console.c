/**
 * @file console.c
 * @desc
 */
#include <x86.h>
#include <vmmap.h>
#include <kbdreg.h>
#include <string.h>
#include <assert.h>
#include <kern/console.h>

static void cons_intr(int (*proc)(void));
static void cons_putc(int c);

/* I/O delay routine necessitated by historical PC design flaws */
static void delay(void)
{
  inb(0x84);
  inb(0x84);
  inb(0x84);
  inb(0x84);
}

/* serial I/O code */
#define COM1          0x3F8
#define COM_RX        0        /* in: receive buffer (DLAB=0) */
#define COM_TX        0        /* out: transmit buffer (DLAB=0) */
#define COM_DLL       0        /* out: divisor latch low (DLAB=1) */
#define COM_DLM       1        /* out: divisor latch high (DLAB=1) */
#define COM_IER       1        /* out: interrupt enable register */
#define COM_IER_RDI   0x01     /*   enable receiver data interrupt */
#define COM_IIR       2        /* in:  interrupt ID register */
#define COM_FCR       2        /* out: FIFO control register */
#define COM_LCR       3        /* out: line control register */
#define COM_LCR_DLAB  0x80     /*   divisor latch access bit */
#define COM_LCR_WLEN8 0x03     /*   wordlength: 8 bits */
#define COM_MCR       4        /* out: modem control register */
#define COM_MCR_RTS   0x02     /*   RTS complement */
#define COM_MCR_DTR   0x01     /*   DTR complement */
#define COM_MCR_OUT2  0x08     /*   out2 complement */
#define COM_LSR       5        /* in: line status register */
#define COM_LSR_DATA  0x01     /*   data available */
#define COM_LSR_TXRDY 0x20     /*   transmit buffer avail */
#define COM_LSR_TSRE  0x40     /*   transmitter off */

static bool serial_exists;

static int serial_proc_data(void)
{
  if (!(inb(COM1+COM_LSR) & COM_LSR_DATA))
    return -1;
  return inb(COM1+COM_RX);
}

void serial_intr(void)
{
  if (serial_exists)
    cons_intr(serial_proc_data);
}

static void serial_putc(int c)
{
  int i;
  for (i = 0; !(inb(COM1 + COM_LSR) & COM_LSR_TXRDY) && i < 12800;  i++)
    delay();
  
  outb(COM1 + COM_TX, c);
}

static void serial_init(void)
{
  /* turn off the FIFO */
  outb(COM1+COM_FCR, 0);
  
  /* set speed; requires DLAB latch */
  outb(COM1+COM_LCR, COM_LCR_DLAB);
  outb(COM1+COM_DLL, (uint8_t) (115200 / 9600));
  outb(COM1+COM_DLM, 0);

  /* 8 data bits, 1 stop bit, parity off; turn off DLAB latch */
  outb(COM1+COM_LCR, COM_LCR_WLEN8 & ~COM_LCR_DLAB);

  /* no modem controls */
  outb(COM1+COM_MCR, 0);
  /* enable rcv interrupts */
  outb(COM1+COM_IER, COM_IER_RDI);

  /* clear any preexisting overrun indications and interrupts; serial port 
		 doesn't exist if COM_LSR returns 0xFF */
  serial_exists = (inb(COM1+COM_LSR) != 0xFF);
  (void) inb(COM1+COM_IIR);
  (void) inb(COM1+COM_RX);
}

/* parallel port output code */
static void lpt_putc(int c)
{
  int i;

  for (i = 0; !(inb(0x378+1) & 0x80) && i < 12800; i++)
    delay();
  outb(0x378+0, c);
  outb(0x378+2, 0x08|0x04|0x01);
  outb(0x378+2, 0x08);
}

/* text-mode CGA/VGA display output */
static unsigned addr_6845;
static uint16_t *crt_buf;
static uint16_t crt_pos;

static void cga_init(void)
{
  volatile uint16_t *cp;
  uint16_t was;
  unsigned pos;

	/* cp points to the VGA buffer location */
  cp = (uint16_t*) (KERNBASE + CGA_BUF);
  was = *cp;
  *cp = (uint16_t) 0xA55A;
  if (*cp != 0xA55A) {
    cp = (uint16_t*) (KERNBASE + MONO_BUF);
    addr_6845 = MONO_BASE;
  } 
	else {
    *cp = was;
    addr_6845 = CGA_BASE;
  }
  
  /* extract cursor location */
  outb(addr_6845, 14);
  pos = inb(addr_6845 + 1) << 8;
  outb(addr_6845, 15);
  pos |= inb(addr_6845 + 1);

  crt_buf = (uint16_t*) cp;
  crt_pos = pos;
}

static void cga_putc(int c)
{
  /* if no attribute given, then use black on white */
  if (!(c & ~0xFF))
    c |= 0x0700;

  switch (c & 0xff) {
  case '\b':
    if (crt_pos > 0) {
      crt_pos--;
      crt_buf[crt_pos] = (c & ~0xff) | ' ';
    }
    break;
  case '\n': /* carriage return (0x0d) */
    crt_pos += CRT_COLS;
    /* fall through */
  case '\r': /* linefeed (0x0a) */
    crt_pos -= (crt_pos % CRT_COLS);
    break;
  case '\t':
    cons_putc(' ');
    cons_putc(' ');
    cons_putc(' ');
    cons_putc(' ');
    cons_putc(' ');
    break;
  default:
    crt_buf[crt_pos++] = c;   /* write the character */
    break;
  }

  /* if screen is filled up, scroll one line down */
  if (crt_pos >= CRT_SIZE) {
    int i;
    memmove(crt_buf, crt_buf + CRT_COLS, (CRT_SIZE - CRT_COLS) * 
						sizeof(uint16_t));
    for (i = CRT_SIZE - CRT_COLS; i < CRT_SIZE; i++)
      crt_buf[i] = 0x0700 | ' ';
    crt_pos -= CRT_COLS;
  }

  /* move the cursor */
  outb(addr_6845, 14);
  outb(addr_6845 + 1, crt_pos >> 8);
  outb(addr_6845, 15);
  outb(addr_6845 + 1, crt_pos);
}

/* keyboard input code */
#define NO         0
#define SHIFT      (1 << 0)
#define CTL        (1 << 1)
#define ALT        (1 << 2)
#define CAPSLOCK   (1 << 3)
#define NUMLOCK    (1 << 4)
#define SCROLLLOCK (1 << 5)
#define E0ESC      (1 << 6)

static uint8_t shiftcode[256] = 
{
  [0x1D] = CTL,
  [0x2A] = SHIFT,
  [0x36] = SHIFT,
  [0x38] = ALT,
  [0x9D] = CTL,
  [0xB8] = ALT
};

static uint8_t togglecode[256] = 
{
  [0x3A] = CAPSLOCK,
  [0x45] = NUMLOCK,
  [0x46] = SCROLLLOCK
};

static uint8_t normalmap[256] =
{
  NO,   0x1B, '1',  '2',  '3',  '4',  '5',  '6',  /* 0x00 */
  '7',  '8',  '9',  '0',  '-',  '=',  '\b', '\t',
  'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',  /* 0x10 */
  'o',  'p',  '[',  ']',  '\n', NO,   'a',  's',
  'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',  /* 0x20 */
  '\'', '`',  NO,   '\\', 'z',  'x',  'c',  'v',
  'b',  'n',  'm',  ',',  '.',  '/',  NO,   '*',  /* 0x30 */
  NO,   ' ',  NO,   NO,   NO,   NO,   NO,   NO,
  NO,   NO,   NO,   NO,   NO,   NO,   NO,   '7',  /* 0x40 */
  '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
  '2',  '3',  '0',  '.',  NO,   NO,   NO,   NO,   /* 0x50 */
  [0xC7] = KEY_HOME,        [0x9C] = '\n' /*KP_Enter*/,
  [0xB5] = '/' /*KP_Div*/,  [0xC8] = KEY_UP,
  [0xC9] = KEY_PGUP,        [0xCB] = KEY_LF,
  [0xCD] = KEY_RT,          [0xCF] = KEY_END,
  [0xD0] = KEY_DN,          [0xD1] = KEY_PGDN,
  [0xD2] = KEY_INS,         [0xD3] = KEY_DEL
};

static uint8_t shiftmap[256] = 
{
  NO,   033,  '!',  '@',  '#',  '$',  '%',  '^',  /* 0x00 */
  '&',  '*',  '(',  ')',  '_',  '+',  '\b', '\t',
  'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',  /* 0x10 */
  'O',  'P',  '{',  '}',  '\n', NO,   'A',  'S',
  'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',  /* 0x20 */
  '"',  '~',  NO,   '|',  'Z',  'X',  'C',  'V',
  'B',  'N',  'M',  '<',  '>',  '?',  NO,   '*',  /* 0x30 */
  NO,   ' ',  NO,   NO,   NO,   NO,   NO,   NO,
  NO,   NO,   NO,   NO,   NO,   NO,   NO,   '7',  /* 0x40 */
  '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
  '2',  '3',  '0',  '.',  NO,   NO,   NO,   NO,   /* 0x50 */
  [0xC7] = KEY_HOME,        [0x9C] = '\n' /*KP_Enter*/,
  [0xB5] = '/' /*KP_Div*/,  [0xC8] = KEY_UP,
  [0xC9] = KEY_PGUP,        [0xCB] = KEY_LF,
  [0xCD] = KEY_RT,          [0xCF] = KEY_END,
  [0xD0] = KEY_DN,          [0xD1] = KEY_PGDN,
  [0xD2] = KEY_INS,         [0xD3] = KEY_DEL
};

#define C(x) (x - '@')

static uint8_t ctlmap[256] = 
{
  NO,      NO,      NO,      NO,      NO,      NO,      NO,      NO, 
  NO,      NO,      NO,      NO,      NO,      NO,      NO,      NO, 
  C('Q'),  C('W'),  C('E'),  C('R'),  C('T'),  C('Y'),  C('U'),  C('I'),
  C('O'),  C('P'),  NO,      NO,      '\r',    NO,      C('A'),  C('S'),
  C('D'),  C('F'),  C('G'),  C('H'),  C('J'),  C('K'),  C('L'),  NO, 
  NO,      NO,      NO,      C('\\'), C('Z'),  C('X'),  C('C'),  C('V'),
  C('B'),  C('N'),  C('M'),  NO,      NO,      C('/'),  NO,      NO,
  [0x97] = KEY_HOME,
  [0xB5] = C('/'),    [0xC8] = KEY_UP,
  [0xC9] = KEY_PGUP,  [0xCB] = KEY_LF,
  [0xCD] = KEY_RT,    [0xCF] = KEY_END,
  [0xD0] = KEY_DN,    [0xD1] = KEY_PGDN,
  [0xD2] = KEY_INS,   [0xD3] = KEY_DEL
};

static uint8_t *charcode[4] = {
  normalmap,
  shiftmap,
  ctlmap,
  ctlmap
};

/* get data from the keyboard; if we finish a character, return it;
	 else return 0; return -1 if no data */
static int kbd_proc_data(void)
{
  int c;
  uint8_t data;
  static uint32_t shift;

  if ((inb(KBSTATP) & KBS_DIB) == 0)
    return -1;

  data = inb(KBDATAP);

  if (data == 0xE0) {
    /* E0 escape character */
    shift |= E0ESC;
    return 0;
  } 
	else if (data & 0x80) {
    /* Key released */
    data = (shift & E0ESC ? data : data & 0x7F);
    shift &= ~(shiftcode[data] | E0ESC);
    return 0;
  } 
	else if (shift & E0ESC) {
    /* Last character was an E0 escape; or with 0x80 */
    data |= 0x80;
    shift &= ~E0ESC;
  }

  shift |= shiftcode[data];
  shift ^= togglecode[data];

  c = charcode[shift & (CTL | SHIFT)][data];
  if (shift & CAPSLOCK) {
    if ('a' <= c && c <= 'z')
      c += 'A' - 'a';
    else if ('A' <= c && c <= 'Z')
      c += 'a' - 'A';
  }

  /* process special keys; ctrl-alt-del: reboot */
  if (!(~shift & (CTL | ALT)) && c == KEY_DEL) {
    cprintf("Rebooting...\n");
    outb(0x92, 0x3);
  }

  return c;
}

void kbd_intr(void)
{
  cons_intr(kbd_proc_data);
}

static void kbd_init(void)
{
}

/* general device-independent console code:
	 here we manage the console input buffer, where we stash characters 
	 received from the keyboard or serial port whenever the corresponding 
	 interrupt occurs */
#define CONSBUFSIZE 512

static struct {
  uint8_t buf[CONSBUFSIZE];
  uint32_t rpos;
  uint32_t wpos;
} cons;

/* called by device interrupt routines to feed input characters
	 into the circular console input buffer */
static void
cons_intr(int (*proc)(void))
{
  int c;

  while ((c = (*proc)()) != -1) {
    if (c == 0)
      continue;
    cons.buf[cons.wpos++] = c;
    if (cons.wpos == CONSBUFSIZE)
      cons.wpos = 0;
  }
}

/* return the next input character from the console, or 0 if none waiting */
int cons_getc(void)
{
  int c;

  /* poll for any pending input characters, so this function works even 
		 when interrupts are disabled (e.g., when called from the kernel 
		 monitor) */
  serial_intr();
  kbd_intr();

  /* grab the next character from the input buffer */
  if (cons.rpos != cons.wpos) {
    c = cons.buf[cons.rpos++];
    if (cons.rpos == CONSBUFSIZE)
      cons.rpos = 0;
    return c;
  }
  return 0;
}

/* output a character to the console */
static void cons_putc(int c)
{
  serial_putc(c);
  lpt_putc(c);
  cga_putc(c);
}

/* initialize the console devices */
void cons_init(void)
{
	/* initialize CGA/VGA display */
  cga_init();

	/* initialize keyboard */
  kbd_init();
	
	/* initialize serial I/O */
  serial_init();

  if (!serial_exists)
    cprintf("Serial port does not exist!\n");
}


/* "high"-level console I/O; used by readline and cprintf */
void cputchar(int c)
{
  cons_putc(c);
}

int getchar(void)
{
  int c;

  while ((c = cons_getc()) == 0)
    ; /* do nothing */
  return c;
}

int iscons(int fdnum)
{
  /* used by readline */
  return 1;
}
