#INCLUDE <16f887.h>
#DEVICE ADC=10
//#USE DELAY(CLOCK=4000000) // Reloj 4MHz
#use delay(clock=4000000,crystal)//Crystal Externo 4MHz
#FUSES XT,NOPROTECT,NOWDT,NOBROWNOUT,NOPUT,NOLVP
//Definiciones para colocar el LCD y el teclado en el puerto B
#DEFINE USE_PORTB_LCD TRUE
#DEFINE USE_PORTB_KBD TRUE  
//Librerías del LCD y Teclado
#INCLUDE <LCD.C>
#INCLUDE <KBD.C> 

//Definiciones
#BYTE PORTC= 7
#BYTE PORTB= 6
#BYTE PORTA= 5 //INICIALIZACION DE PUERTO A
#BYTE PORTD= 8 //INICIALIZACION DE PUERTO D
#DEFINE LED1 PORTC,4
#DEFINE LED2 PORTC,7

//Variables Globales
CHAR K;
INT8 MIL,CEN,DEC,UNI,VAL,cont;
INT16 contra=9999; //Variable entera para almacenar la contraseña
INT8 sinal; //Variable para saber si tengo contraseña

LONG BITS;     //VARIABLE ALMACENA LOS BITS
FLOAT TEM;     //ALMACENA LA TEMPERATURA

byte const HORARIO[4]={0b1100,      //declaracion de arreglo para movimiento del motor en sentido horario a 4 pines
                       0b0110,
                       0b0011,
                       0b1001};

byte const ANTIH[4]={0b1001,        //declaracion de arreglo para movimiento del motor en sentido antihorario a 4 pines
                     0b0011,
                     0b0110,
                     0b1100};

 void girocompleto(){                //Metodo para girar el motor en sus dos direcciones y encender el led 
      cont=0;
      while(cont<4)                  //bucle while para movimiento en sentido horario del motor paso a paso
      {         
         PORTD=(HORARIO[cont]);
         
         //PORTD=(HORARIO[cont]);      //al  puerto D (PORTD) saldra la secuencia de bits que le retorna la funcion HORARIO dependiendo de la posicion cont       
         delay_ms(80);           //retardo 80 milisegundos
         cont++;                     //incremento de la variable "cont"       
      }
      delay_ms(1000);               //tiempo que se emplea o espera para ejecutar la siguiente instruccion             
      cont=0;                       //se reinicia la variable "cont"
      while(cont<4)                 //bucle while para movimiento en sentido antihorario del motor paso a paso
      {      
         PORTD=(ANTIH[cont]);
         delay_ms(90);          //retardo de 90 milisegundos
         cont++;
     }
 }

VOID TECLADO()
{
k=kbd_getc(); //Llamar la función del teclado y almacenar
//el valor digitado en una variable tipo
//carácter. Si no se oprime ninguna tecla el
//teclado retornara el carácter nulo.*/
WHILE(k=='\0') //si no se oprime ninguna tecla sigue llamando al teclado.
{
   k=kbd_getc();
}
IF( (k!='\0'))
   {
      switch( K )
      {
         case '0':
            VAL=0;
            break;
         case '1':
            VAL=1;
            break;
         case '2':
            VAL=2;
            break;
         case '3':
            VAL=3;
            break;
         case '4':
            VAL=4;
            break;
         case '5':
            VAL=5;
            break;
         case '6':
            VAL=6;
            break;
         case '7':
            VAL=7;
            break;
         case '8':
            VAL=8;
            break;
         case '9':
            VAL=9;
            break;            
      }
   }
}

//Rutina para pedir la clave
void pedir_clave(void)
{
   TECLADO();
   IF((k!='#')&&(k!='*'))
   {
      lcd_putc('*');
      MIL=VAL;
   }
   if(k=='*' || k=='#')
   return;
   TECLADO();
   IF((k!='#')&&(k!='*'))
   {
      lcd_putc('*');
      CEN=VAL;
   }
   if(k=='*' || k=='#')
   return;
   TECLADO();
   IF((k!='#')&&(k!='*'))
   {
      lcd_putc('*');
      DEC=VAL;
   }
   if(k=='*' || k=='#')
   return;
   TECLADO();
   IF((k!='#')&&(k!='*'))
   {
      lcd_putc('*');
      UNI=VAL;
   }
   if(k=='*' || k=='#')
   return;
}

void contrasenia_guardada()
{   
   int16 m,c,d,u;
   INT8 m1,c1,d1,u1;
   sinal=READ_EEPROM(4); //Averiguo si tengo una contraseña guardada o no
   if(sinal!=12)
   contra=9999;
   else
   { 
   //Lee los datos del EEPROM
   m1=READ_EEPROM(0); 
   c1=READ_EEPROM(1); 
   d1=READ_EEPROM(2); 
   u1=READ_EEPROM(3); 
   /* Para realizar la multiplicación cuando se tienen numeros del tipo int8, 
   como por ejemplo 100 con otro tipo int8 como por ejemplo CEN, el programa
   hará un producto int8, sin embargo se desea un resultado int16, por lo 
   tanto debe informarce al compilador por medio de un 'cast' (c=(int16)CEN*100)
   o utilizando la función de multiplicación de tipos del compilador (_mul)*/
   m=m1*1000; //Convierto miles a numero
   //c=_mul(c1,100); //Otra alternativa para multiplicar int8*int8=int16
   c=(int16)c1*100; //Convierto centemas, haciendo un CAST para obtener un int16
   d=d1*10; //Convierto decenas a numero 
   u=u1; //Convierto unidades a numero 
   contra=m+c+d+u;
   }
}

int validar_clave(void)
{
   //Variables locales
   int16 clave=0,m,c,d,u;
   /* Para realizar la multiplicación cuando se tienen numeros del tipo int8, 
   como por ejemplo 100 con otro tipo int8 como por ejemplo CEN, el programa
   hará un producto int8, sin embargo se desea un resultado int16, por lo 
   tanto debe informarce al compilador por medio de un 'cast' (c=(int16)CEN*100)
   o utilizando la función de multiplicación de tipos del compilador (_mul)*/
   m=MIL*1000; //Convierto miles a numero
   //c=_mul(CEN,100); //Otra alternativa para multiplicar int8*int8=int16
   c=(int16)CEN*100; //Convierto centemas, haciendo un CAST para obtener un int16
   d=DEC*10; //Convierto decenas a numero 
   u=UNI; //Convierto unidades a numero 
   clave=m+c+d+u;
   //Borra lo último que fue digitado en el teclado
   MIL=0;CEN=0;DEC=0;UNI=0; 
   if(clave==contra) //Si la clave es igual a la contraseña
      return(1);
   else
      return(0);
}

void cambio_clave(void)
{
   int bandera=0;
   int16 clave=0,m,c,d,u;
   
   printf(LCD_PUTC,"\f CLAVE ACTUAL ");
   LCD_GOTOXY(1,2);
   pedir_clave(); //Llama la funcion de pedir la clave
   bandera=validar_clave(); //Compruebo si la clave actual es correcta
   if(bandera==1)
   {
   printf(LCD_PUTC,"\f CLAVE NUEVA  ");
   LCD_GOTOXY(1,2);   
   TECLADO();
   IF((k!='#')&&(k!='*'))
   {
   lcd_putc('*');
   MIL=VAL;
   }
   TECLADO();
   IF((k!='#')&&(k!='*'))
   {
   lcd_putc('*');
   CEN=VAL;
   }
   TECLADO();
   IF((k!='#')&&(k!='*'))
   {
   lcd_putc('*');
   DEC=VAL;
   }
   TECLADO();
   IF((k!='#')&&(k!='*'))
   {
   lcd_putc('*');
   UNI=VAL;
   }
   m=MIL*1000; //Convierto miles a numero
   c=CEN*10; //Convierto centemas a numero y lo sumo al anterior
   c=c*10;
   d=DEC*10; //Convierto decenas a numero y lo sumo al anterior
   u=UNI; //Convierto unidades a numero y lo sumo al anterior
   clave=m+c+d+u;
   contra=clave;
   WRITE_EEPROM(0,MIL); //Guarda en la eemprom posicion cero la nueva contraseña
   WRITE_EEPROM(1,CEN);
   WRITE_EEPROM(2,DEC);
   WRITE_EEPROM(3,UNI);
   WRITE_EEPROM(4,12); //Guardo un 12 en la posicion 1 de la EEPROM, para decir que tengo
   //una contraseña guardada
   }
   else
   {
   printf(LCD_PUTC,"\f CLAVE INVALIDA ");
   BIT_SET(LED2);
   DELAY_MS(4000);
   BIT_CLEAR(LED2);
   LCD_PUTC("\f");
   }
}

void main()
{
   
   SET_TRIS_C(0B00000000);
   PORTC=0; //Limpia Puerto C
   lcd_init(); //Inicializa el LCD
   kbd_init(); //Inicializa el Teclado
   //Configura las Resistencias PULL-UP del Puerto B
   port_b_pullups(0xFF); //PIC16F887
   
   contrasenia_guardada();
   
   while(true)
   {
      printf(LCD_PUTC,"\f DIGITE CLAVE ");
      LCD_GOTOXY(1,2);  
      DELAY_MS(1000);
      pedir_clave(); //Llama la funcion de pedir la clave
      if(k=='*')
      cambio_clave();
      else
      {
      WHILE((k!='#'))
      {
      TECLADO();
      if(k=='*')
      cambio_clave();
      }
      IF(validar_clave()==1) //Aquí se compara si
      //los números digitados
      //están correctos.*/
      {
      printf(LCD_PUTC,"\f CLAVE CORRECTA ");
      BIT_SET(LED1);
      DELAY_MS(2000);
      BIT_CLEAR(LED1);
      
      SET_TRIS_A(0B00000001);          //DEFINIAR RA0 COMO ENTRADA
      SET_TRIS_D(0);                   //DEFINIR EL PUERTO D COMO SAIDA
      SETUP_ADC_PORTS(sAN1);            //UTILIZAR EN PUERTO AN0 COMO ENTRADA
      SETUP_ADC(ADC_CLOCK_INTERNAL);   //SELECCIONO RELOJ INTERNO PARA CONVERSION
      
      WHILE(1)
      {     
         SET_ADC_CHANNEL(0);          //SELECCIONO EL CANAL 0 (RA0)
         DELAY_MS(1);                 //RETARDO DE 1 MS PARA PROCESAR BIEN LA INFORMACION
         BITS = READ_ADC();             //LA INFORMACION LEIDA LA GUARDO EN VARIABLE BITS
          
         TEM=BITS*0.4882;              //CONVERSION DE BITS A TEMPERATURA
         PRINTF(LCD_PUTC,"\f TEMPERATURA");  //IMPRIME EN LA POSICION 1.1 "LA TEMPERATURA"
         PRINTF(LCD_PUTC,"\n C= %F    ",TEM);  //EN LA POSICION 2.2 MUESTRA EL VALOR NUMERICO DE LA CONVERSION
         DELAY_MS(1000);
         TECLADO();
      switch( VAL ){
         case 0:
            PRINTF(LCD_PUTC,"\f OPC 0");
            DELAY_MS(1000);
            break;
         case 1:
            PRINTF(LCD_PUTC,"\f OPC 1");
            girocompleto();
            break;
         case 2:
            PRINTF(LCD_PUTC,"\f OPC 2");
            DELAY_MS(1000);
            break;
         case 3:
            PRINTF(LCD_PUTC,"\f OPC 3");
            DELAY_MS(1000);
            break;
         case 4:
            PRINTF(LCD_PUTC,"\f OPC 4");
            DELAY_MS(1000);
            break;
         case 5:
            PRINTF(LCD_PUTC,"\f OPC 5");
            DELAY_MS(1000);
            break;
         case 6:
            PRINTF(LCD_PUTC,"\f OPC 6");
            DELAY_MS(1000);
            break;
         case 7:
            PRINTF(LCD_PUTC,"\f OPC 7");
            DELAY_MS(1000);
            break;
         case 8:
            PRINTF(LCD_PUTC,"\f OPC 8");
            DELAY_MS(1000);
            break;
         case 9:
            PRINTF(LCD_PUTC,"\f OPC 9");
            DELAY_MS(1000);
            break;
         default:
            break;
      }
      }
      
      }
      ELSE
      {
      printf(LCD_PUTC,"\f CLAVE INVALIDA ");
      BIT_SET(LED2);
      DELAY_MS(4000);
      BIT_CLEAR(LED2);
      }
      }
      
   }
}


