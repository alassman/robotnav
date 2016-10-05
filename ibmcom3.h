#define STOP_BIT_1      1               /* 1 Stop bit */

#define STOP_BIT_2      2               /* 2 Stop bits */



#define COM_NONE        0               /* 8 data bits, no parity */

#define COM_EVEN        1               /* 7 data bits, even parity */

#define COM_ODD         2               /* 7 data bits, odd parity */



extern  int  com_install(int portnum);

extern  void com_deinstall(void);

extern  void com_set_speed(long speed);

extern  void com_set_parity(enum par_code parity, int stop_bits);

extern  void com_lower_dtr(void);

extern  void com_raise_dtr(void);

extern  void com_tx(char c);

extern  void com_tx_string(char *s);

extern  char com_rx(void);

extern  int  com_tx_ready(void);

extern  int  com_tx_empty(void);

extern  int  com_rx_empty(void);

extern  void com_flush_tx(void);

extern  void com_flush_rx(void);

extern  int  com_carrier(void);
