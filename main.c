/* p4_2.c UART0 Receive
* Receiving any key from terminal emulator (TeraTerm) of the
* host PC to the UART0 on Freescale FRDM-KL25Z board.
* UART0 is connected to openSDA debug agent and has
* a virtual connection to the host PC COM port.
* Launch TeraTerm on a PC and hit any key.
* The LED program from P2_7 of Chapter 2 is used to turn
* on the tri-color LEDs according to the key received.
*
* By default in SystemInit(), FLL clock output is 41.94 MHz.
* Setting BDH=0, BDL=0x17, and OSR=0x0F yields 115200 Baud.
*/
#include <MKL25Z4.H>
#include <stdio.h>
#define WHEEL1_POS (3)
#define WHEEL2_POS (4)
#define WHEEL3_POS (5)
#define WHEEL4_POS (6)
#define SW1_POS (12)
#define SW2_POS (15)
#define MASK(x) (1UL << (x))

void UART0_init(void);
void delayMs(int n);
void LED_init(void);
void LED_set(int value);

void set_motors(void);
void Delay(volatile unsigned int time_del);
void ready(void);
void data_send(unsigned char data);
void turnLeft(void);
void turnRight(void);
void goForward(void);
void goBackward(void);
void Stop(void);

void pulse1(uint32_t mod,uint32_t cnv);
void pulse2(uint32_t mod,uint32_t cnv);
int mod1=6530;
volatile int cnv1 =530;
volatile int cnv2 =450;
/6530,400 down/
/6530,580 up/
/*6530,200 right */
/*6530,700 left */
int main (void) {
char c;

UART0_init();
set_motors();

	
pulse1(mod1,cnv1);
pulse1(mod1,cnv2);
while (1) {
while(!(UART0->S1 & 0x20)) {

} /* wait for receive buffer full */
c = UART0->D; /* read the char received */
LED_set(c);
}
}






/* initialize UART0 to receive at 115200 Baud */
void UART0_init(void) {
SIM->SCGC4 |= 0x0400; /* enable clock for UART0 */
SIM->SOPT2 |= 0x04000000; /* use FLL output for UART Baud rate generator
*/
UART0->C2 = 0; /* turn off UART0 while changing configurations */
UART0->BDH =  0x00;
UART0->BDL = 0x8A; /* 9600 Baud */
UART0->C4 = 0x0F; /* Over Sampling Ratio 16 */
UART0->C1 = 0x00; /* 8-bit data */
UART0->C2 = 0x04; /* enable receive */
SIM->SCGC5 |= 0x0200; /* enable clock for PORTA */
PORTA->PCR[1] = 0x0200; /* make PTA1 UART0_Rx pin */
}
/* initialize all three LEDs on the FRDM board */
void LED_init(void)
{
SIM->SCGC5 |= 0x400; /* enable clock to Port B */
SIM->SCGC5 |= 0x1000; /* enable clock to Port D */
PORTB->PCR[18] = 0x100; /* make PTB18 pin as GPIO */
PTB->PDDR |= 0x40000; /* make PTB18 as output pin */
PTB->PSOR = 0x40000; /* turn off red LED */

PORTB->PCR[19] = 0x100; /* make PTB19 pin as GPIO */
PTB->PDDR |= 0x80000; /* make PTB19 as output pin */
PTB->PSOR = 0x80000; /* turn off green LED */
PORTD->PCR[1] = 0x100; /* make PTD1 pin as GPIO */
PTD->PDDR |= 0x02; /* make PTD1 as output pin */
PTD->PSOR = 0x02; /* turn off blue LED */
}
/* turn on or off the LEDs according to bit 2-0 of the value */
void LED_set(int value)
{
if (value == 1){
PTD->PCOR = 0x02;
goForward();
Delay(5000000);
Stop(); }

if (value == 2){
PTD->PCOR = 0x02;
goBackward();
Delay(5000000);
Stop(); }

if (value == 3){
turnLeft();
PTB->PCOR = 0x80000;}

if (value == 4){
turnRight();}

if (value == 5){
Stop();}

if (value == 6){
	Delay(1);
	cnv1+=10;
		if( cnv1 >= 580){cnv1 =580;}
		pulse1(mod1,cnv1);
/Turret up/
}
if (value == 7){
	Delay(1);
	cnv2+=5;
		if( cnv2 >= 700){cnv2 =700;}
		pulse2(mod1,cnv2);
/*Turret left*/
}
if (value == 8){
	Delay(1);
	cnv2-=5;
		if( cnv2 <= 200){cnv2 =200;}
		pulse2(mod1,cnv2);
/*Turret right*/
}
if (value == 9){
	Delay(1);
	cnv1-=10;
		if( cnv1 <= 400){cnv1 =400;}
		pulse1(mod1,cnv1);
/*Turret down*/
}
if (value == 10){SIM->SCGC5 |= 0x400; /* enable clock to Port B */

PORTB->PCR[18] = 0x100; /* make PTB18 pin as GPIO */
PTB->PDDR |= 0x40000; /* make PTB18 as output pin */
PTB->PSOR = 0x40000; /* turn off red LED */
PTB->PCOR = 0x40000; /* turn on red LED */
Delay(100);
PTB->PSOR = 0x40000;	
/*Turret Fire*/
}
}


void Delay(volatile unsigned int time_del) {
  while (time_del--) 
		{
  }
}

void set_motors(void){ // Enable Clock to Port A
// Enable Clock to Port A
SIM->SCGC5 |= (1UL << 12);
// Make 3 pins GPIO
PORTD->PCR[3] &= ~PORT_PCR_MUX_MASK; 
PORTD -> PCR[3] = 0x100; 
PORTD->PCR[4] &= ~PORT_PCR_MUX_MASK;
PORTD -> PCR[4] = 0x100; 
/*	
PORTA->PCR[SW1_POS] &= ~PORT_PCR_MUX_MASK;
PORTA->PCR[SW1_POS] |= PORT_PCR_MUX(1)| PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_IRQC(0x0a);
PTA->PDDR &= ~MASK(SW1_POS);  */

//diger taraf
PORTD->PCR[WHEEL3_POS] &= ~PORT_PCR_MUX_MASK;
PORTD->PCR[WHEEL3_POS] |= PORT_PCR_MUX(1);    /* PORTA -> PCR[16] = 0x100; */
PORTD->PCR[WHEEL4_POS] &= ~PORT_PCR_MUX_MASK;
PORTD->PCR[WHEEL4_POS] |= PORT_PCR_MUX(1);   /* PORTA -> PCR[17] = 0x100; */
PORTD->PCR[SW2_POS] &= ~PORT_PCR_MUX_MASK;
PORTD->PCR[SW2_POS] |= PORT_PCR_MUX(1);    /* PORTA -> PCR[15] = 0x100; */

PTD->PDDR |= MASK(3) | MASK (4); // set LED bits to outputs  /*PTA ->PDDR |= 0x1 | 0x10; */
PTD->PDDR |= MASK(WHEEL3_POS) | MASK (WHEEL4_POS); // set LED bits to outputs  /*PTA ->PDDR |= 0x1 | 0x10; */ 
}
	
	
void turnLeft(void){
Delay(10000);
Stop();
PTD->PSOR = MASK(WHEEL1_POS); // turn on LED1, turn off LED2
PTD->PCOR = MASK(WHEEL2_POS);
PTD->PCOR = MASK(WHEEL3_POS); // turn on LED1, turn off LED2
PTD->PSOR = MASK(WHEEL4_POS);
Delay(10000000);
Stop();
Delay(10000); 


}

void turnRight(void){

Delay(10000);
Stop();
PTD->PSOR = MASK(WHEEL3_POS); // turn on LED1, turn off LED2
PTD->PCOR = MASK(WHEEL4_POS); 
PTD->PCOR = MASK(WHEEL1_POS); // turn on LED1, turn off LED2
PTD->PSOR = MASK(WHEEL2_POS); 
Delay(10000000);
Stop();
Delay(10000);

}	

void goForward(void){
PTD->PSOR = MASK(3); // turn on LED1, turn off LED2
PTD->PCOR = MASK(4); 
PTD->PSOR = MASK(5); // turn on LED1, turn off LED2
PTD->PCOR = MASK(6); 
}
void goBackward(void){
PTD->PCOR = MASK(3); // turn on LED1, turn off LED2
PTD->PSOR = MASK(4); 
PTD->PCOR = MASK(5); // turn on LED1, turn off LED2
PTD->PSOR = MASK(6); 
}

void Stop(void){
PTD->PCOR = MASK(3); // turn on LED1, turn off LED2
PTD->PCOR = MASK(4); 
PTD->PCOR = MASK(5); // turn on LED1, turn off LED2
PTD->PCOR = MASK(6); 
}

void pulse1(uint32_t mod,uint32_t cnv)
{
	SIM->SCGC5 |= (1UL << 13); /* enable clock to Port E */
PORTE->PCR[20] = 0x0300; /* PTE20 used by TPM0 */
SIM->SCGC6 |= (1UL << 25); /* enable clock to TPM0 */
SIM->SOPT2 |= 0x01000000; /* use MCGFLLCLK as timer counter clock */
TPM1->SC = 0; /* disable timer */
TPM1->CONTROLS[0].CnSC = 0x20 | 0x08; /* edge-aligned, pulse high */
TPM1->MOD = mod; /* Set up modulo register for 60 kHz */
TPM1->CONTROLS[0].CnV = cnv; /* Set up channel value for 50% dutycycle */
TPM1->SC = 0x0E; /* enable TPM0 with prescaler /16 */
}

void pulse2(uint32_t mod,uint32_t cnv)
{PORTE->PCR[31] = 0x0300; /* PTE20 used by TPM0 */
SIM->SCGC6 |= (1UL << 24); /* enable clock to TPM0 */
SIM->SOPT2 |= 0x01000000; /* use MCGFLLCLK as timer counter clock */
TPM0->SC = 0; /* disable timer */
TPM0->CONTROLS[4].CnSC = 0x20 | 0x08; /* edge-aligned, pulse high */
TPM0->MOD = mod; /* Set up modulo register for 60 kHz */
TPM0->CONTROLS[4].CnV = cnv; /* Set up channel value for 50% dutycycle */
TPM0->SC = 0x0E; /* enable TPM0 with prescaler /16 */
}

void ultra_sensor(void){}
