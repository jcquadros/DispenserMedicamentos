
// --- Bibliotecas Auxiliares ---
#include <DS3231.h>          //Inclui a biblioteca do DS3231 Shield
#include <LiquidCrystal.h>

//******* CONFIGURACAO DO PROJETO *********
#define pinBotoes A0

#define pinRs 8
#define pinEn 9
#define pinD4 4
#define pinD5 5
#define pinD6 6
#define pinD7 7
#define pinBackLight 10
//*****************************************

#define btNENHUM 0
#define btSELECT 1
#define btLEFT   2
#define btUP     3
#define btDOWN   4
#define btRIGHT  5

#define tempoDebounce 90

//****************************************
#define ALARME1 0
#define ALARME2 1
#define ALARME3 2
#define SAIR 3


#define modoONOFF 0
#define modoHORA 1
#define modoMINUTO 2

#define SINO 1
#define HEART 2
#define CURSOR 3
#define REMEDIO 4

unsigned long delayBotao = 0;
int estadoBotaoAnt = btNENHUM;

// --- Declaração de Objetos ---
DS3231  rtc(SDA, SCL);
Time t;
int horas, ano;

LiquidCrystal lcd(pinRs, pinEn, pinD4, pinD5, pinD6, pinD7);

int contador = 0;

//********ALARMES***********
int horaAlarme[3] ={0,0,0};
int minutosAlarme[3] ={0,0,0};
int onAlarme[3] = {false,false,false};

byte heart[] = {0x00,0x0A,0x1F,0x1F,0x0E,0x04,0x00,0x00};
byte cursor[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F};
byte sino[] = {0x4,0xe,0xe,0xe,0x1f,0x0,0x4}; 
byte remedio[] = { 0x0E,0x19,0x11,0x11,0x1F,0x1F,0x1D,0x0E};

void setup() {
  pinMode(pinBackLight, OUTPUT);
  digitalWrite(pinBackLight, HIGH);   //liga a luz de fundo
  lcd.begin(16, 2);                   // inicializa lcd
  lcd.createChar(SINO, sino);            // cria caracter de sino
  lcd.createChar(HEART, heart);           // cria caracter de coracao
  lcd.createChar(CURSOR, cursor);           // cria caracter do cursor
  lcd.createChar(REMEDIO, remedio);
  Serial.begin(9600);
  saudacao();
}

void saudacao(){                                                         
  lcd.setCursor(0,0);                                               
  lcd.print("Aguarde ... ");
  lcd.setCursor(0,1);                                                
  lcd.print("Preparando RTC ");
  lcd.print((char)1); 
  delay(3000);                                                    
  lcd.clear();                                                      
  lcd.setCursor(0,0);                                              
  lcd.print("Bem vindo, Sr! ");  
  lcd.print((char)4);                              
  delay(3000);                                                    
  lcd.clear();
}  

void loop() {
  telaHome();
  int botao = leBotoes();
  if(botao == btUP){ // Abre o menu de configuracoes ao pressionar o botao up
    configuracoesDeAlarmeMenu();
    lcd.clear();
  }
}
void telaHome(){
  int i, count = 0;
  lcd.clear();
  for(i = 0; i<3; i++){
    if(onAlarme[i])
      count++;
  }
  lcd.print("Alarmes :");
  if(!count){
    lcd.setCursor(15,0);
    lcd.print("0");
  }
  for(i = 0; i<count; i++){
    lcd.setCursor(15-i, 0);
    lcd.print((char)1);  
  }
}

int leBotoes(){
  int valBotoes = analogRead(pinBotoes);
  delay(100);
  if(estadoBotaoAnt != valBotoes){
    estadoBotaoAnt = valBotoes;    
    if ((valBotoes < 800) && (valBotoes >= 600)) {
      return btSELECT;
      
    } else if ((valBotoes < 600) && (valBotoes >= 400)) {
      return btLEFT;
      
    } else if ((valBotoes < 400) && (valBotoes >= 200)) {
      return btDOWN;
      
    } else if ((valBotoes < 200) && (valBotoes >= 60)) {
      return btUP;   
    } else if  (valBotoes < 60) {
      return btRIGHT;
    }else {
      return btNENHUM;
    }
  }  
  else {
    return btNENHUM;
  }
}

void configuracoesDeAlarmeMenu(){
  int botao = btNENHUM;
  int opcao = 0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Configuracoes de");
  lcd.setCursor(0,1);
  lcd.print("Alarme");

  delay(1000);
  while(true){
    botao = leBotoes();
    
      // navega pelo menu
      if(botao == btRIGHT){
        opcao++;
      }else if(botao == btLEFT){
        opcao--;
      }else if(botao == btSELECT){
        if(opcao !=SAIR){
          configuracaoAlarme(opcao);
        }
        break;
      }
      // trata extrapolacoes
      if(opcao > SAIR){
        opcao = ALARME1;
      }
      if(opcao < ALARME1){
        opcao = SAIR;
      }
      imprimeOpcaoSelecionada(opcao);
    
  }

}

void imprimeOpcaoSelecionada(int opcao){
  
    if(opcao!=SAIR){
      lcd.clear();
      lcd.print("Alarme ");
      lcd.print(opcao+1); // imprime o numero do alarme
      lcd.setCursor(12,1);
      if(onAlarme[opcao]){ // imprime on ou off
        lcd.print("ON ");
        lcd.setCursor(15, 0);
        lcd.print((char)1); // sininho de alarme ativado
      }else{
        lcd.print("OFF");
      }
      // // imprime o horario do alarme
      lcd.setCursor(0,1);
      if(horaAlarme[opcao] <10){ // trata escrita de uma hora com menos de 2 digitos
        lcd.print("0");
      }
      lcd.print(horaAlarme[opcao]);
      lcd.print(" :");
      if(minutosAlarme[opcao] <10){ // trata a escrita de um minuto com menos de 2 digitos
        lcd.print("0");
      }
      lcd.print(minutosAlarme[opcao]);
      
   
    }else{
      lcd.clear();
      lcd.print("Sair");
    }
  
}

void configuracaoAlarme(int alarme){
  int botao = btNENHUM;
  int opcao = modoHORA;
  int incrementaHorario = 0;
  alteraPosicaoCursor(opcao);
  lcd.print(char(CURSOR));
  while(true){
    botao = leBotoes();
    incrementaHorario = 0;
    // navega pelo menu
    if(botao == btRIGHT){
      opcao++;
    }else if(botao == btLEFT){
      opcao--;
    }else if(botao == btSELECT){
      break;
    }else if(botao == btUP){
      incrementaHorario++;
    }else if(botao == btDOWN){
      incrementaHorario--;
    }
    // configura hora minuto ou ligar/desligar alarme
    if(botao == btUP || botao == btDOWN){
      if(opcao == modoHORA){
        horaAlarme[alarme] += incrementaHorario;
      }else if(opcao == modoMINUTO){
        minutosAlarme[alarme] +=incrementaHorario;
      }else if(opcao == modoONOFF){
        onAlarme[alarme] = !onAlarme[alarme]; // ativa ou desatica o alarme
      }
    }
    
    // trata extrapolacoes
    if(opcao > modoMINUTO){
      opcao = modoONOFF;
    }else if(opcao < modoONOFF){
      opcao = modoMINUTO;
    }

    trataHorario(alarme); //  nao permite que extrapole
    imprimeOpcaoSelecionada(alarme);
    alteraPosicaoCursor(opcao);
    lcd.print(char(CURSOR));

    // imprime a opcao
  }
}

/*
*   Trata valores invalidos de horas e minutos de um alarme
*/
void trataHorario(int i){
  if(horaAlarme[i]>23){
    horaAlarme[i] = 0;
  }
  if(horaAlarme[i]<0){
    horaAlarme[i] = 23;
  }

  if(minutosAlarme[i]>59){
    minutosAlarme[i] = 0;
  }
  if(minutosAlarme[i]<0){
    minutosAlarme[i] = 59;
  }
}
/*
* De acordo com o modo selecionado retorna a posicao a qual
* o cursor devera ser movido no lcd
*/
void alteraPosicaoCursor(int modo){
  // seta a casa do cursor
  if(modo == modoONOFF){
    lcd.setCursor(15,1);
  }else if(modo == modoHORA){
    lcd.setCursor(2,1);
  }else if(modo == modoMINUTO){
    lcd.setCursor(6,1);
  } 
}








