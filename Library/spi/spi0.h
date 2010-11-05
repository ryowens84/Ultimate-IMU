#define SPIF	(1<<7)

void SPI0_Init(void);
void SPI0_send(char c);
char SPI0_recv(void);
char SPI0_send_recv(char c);


