/*
 * File:   main.c
 * Author: Miguel Chacón
 * Programa: LAB 04
 * Fecha de creación: 10/10/2022
 */

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#define _XTAL_FREQ 4000000
//funciones
void setup(void);
void setupADC(void);
//void displays(uint8_t D1, uint8_t D2);

//variables
int Volt = 0;
int ADCvolt = 0;
int D1 = 0;
int D2 = 0;

//unsigned char numDisp1 = 6;
//unsigned char numDisp2 = 10;

//char const displayAC[10] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x18};
//char const displayCC[6] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D};
   
uint8_t tabla[16] = {0b00111111,   //0
                     0b00000110,   //1
                     0b01011011,   //2
                     0b01001111,   //3
                     0b01100110,   //4
                     0b01101101,   //5
                     0b01111101,   //6
                     0b00000111,   //7
                     0b01111111,   //8
                     0b01101111,   //9
                     0b01110111,   //A
                     0b01111100,   //B
                     0b00111001,   //C
                     0b01011110,   //D
                     0b01111001,   //E
                     0b01110001};  //F

//******************************************************************************
//INTERRUPCIONES
//******************************************************************************
void __interrupt() ISR (void){
	
	if (RBIF == 1){
    if (PORTBbits.RB0 == 0)
    {
        __delay_ms(30);
        if (PORTBbits.RB0 == 1){
            PORTD++;
            INTCONbits.RBIF = 0;
        }
    }
    else if (PORTBbits.RB1 == 0){
        __delay_ms(30);
        if (PORTBbits.RB1 == 1){
            PORTD--;
            INTCONbits.RBIF = 0;
		}
    }
	else if (PORTBbits.RB2 == 0){
		__delay_ms(30);
		if (PORTBbits.RB2 == 1){
			if (ADRESH > PORTD){
                PORTBbits.RB3 = 1;
            }else {
                PORTBbits.RB3 = 0;
            }
			INTCONbits.RBIF = 0;}
    }
    }
}
//******************************************************************************
// Código Principal
//******************************************************************************
void main(void) {
    
    setup();
    setupADC();
    while(1){
        ADCON0bits.GO = 1;
        while(ADCON0bits.GO == 1){
            ;
        }
        ADIF = 0;        
        D2 = (ADRESH%16); // decenas
        D1 = (ADRESH/16);// unidades
//        multiplexado(uint8_t D1, uint8_t D2);
        //if (PORTCbits.RC0 == 1){
        PORTA = tabla[D2];
        PORTCbits.RC1 = 1;
        PORTCbits.RC0 = 0;
        __delay_ms(5);
          //  }
        //else if (PORTCbits.RC1 == 1){
        PORTA = tabla[D1];
        PORTCbits.RC1 = 0;
        PORTCbits.RC0 = 1;
        __delay_ms(5);
            //}
        
    }
}
//******************************************************************************
// Función para configurar GPIOs
//******************************************************************************
void setup(void){
    //Internal CLOCK
    //OSCCONbits.IRCF = 0b110;        // 4MHz
    //OSCCONbits.SCS = 1;
    
    //PUERTOS
    ANSELbits.ANS6 = 1;
    ANSELH = 0;
    TRISB = 0b00000111;
    TRISA = 0;
    TRISC = 0;
    TRISD = 0;
    
    PORTA = 0; //inicializar puertos
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
    
    //Interrupciones
    INTCONbits.RBIE = 1; 
    INTCONbits.RBIF = 0;
    INTCONbits.GIE = 1; //interrupciones globales
    
    WPUBbits.WPUB0 = 1; //inputs
    WPUBbits.WPUB1 = 1;
    WPUBbits.WPUB2 = 1;
    IOCBbits.IOCB0 = 1; //inputs
    IOCBbits.IOCB1 = 1;
    IOCBbits.IOCB2 = 1;
    
    OPTION_REGbits.nRBPU = 0; //no RBPU, habilitan los pullups internos
}
//******************************************************************************
// Función para configurar ADC
//******************************************************************************
void setupADC(void){
    
    // Paso 1 Seleccionar puerto de entrada
    //TRISAbits.TRISA0 = 1;
    ANSELH = 0;
    TRISEbits.TRISE1 = 1;
    ANSELbits.ANS6 = 1;
    ANSEL = 0b01000000;
    
    // Paso 2 Configurar módulo ADC
    
    ADCON0bits.ADCS1 = 0;
    ADCON0bits.ADCS0 = 1;       // Fosc/ 8
    
    ADCON1bits.VCFG1 = 0;       // Ref VSS
    ADCON1bits.VCFG0 = 0;       // Ref VDD
    
    ADCON1bits.ADFM = 0;        // Justificado hacia izquierda
    
    ADCON0bits.CHS3 = 0;
    ADCON0bits.CHS2 = 1;
    ADCON0bits.CHS1 = 1;
    ADCON0bits.CHS0 = 0;        // Canal AN6
    
    ADCON0bits.ADON = 1;        // Habilitamos el ADC
    __delay_us(100);
    
}