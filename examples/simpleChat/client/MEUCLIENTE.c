#include "ACore.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NENHUM 'p'
#define ATACANDO 'k'
#define ESQUERDA 'a'
#define DIREITA 'd'
#define CIMA 'w'
#define BAIXO 's'

#define BOI_1 1
#define BOI_2 2
#define BOI_3 3
#define BOI_4 4

#define BUFFER_SIZE 100
#define LOGIN_MAX_SIZE 13
#define MAX_HP 500
#define MAX_CLIENTS 2

#define ALTURA 630
#define LARGURA 940

#define ATAQUE_1 'z'
#define ATAQUE_2 'x'

typedef struct Pos{
        int x, y; //posicao (x,y) que o boi está
}Pos;

typedef struct Player{
    int id; //numero no intervalo [0,MAX_CLIENTS)
    int vida; //numero entre [0,MAX_HP]
    char movimento; //é algum dos defines: NENHUM, DIREITA, ESQUERDA, CIMA, BAIXO
    char ataque; //é algum dos defines: ATAQUE_1 ou ATAQUE_2
    char login[LOGIN_MAX_SIZE];
	char estado;
    char personagem; //é um personagem de define igual BOI_X, em que X está no intervalo [1,4]
    int blocked; //número de frames em que o boi vai executar uma animacao de levar dano, ficando sem poder executar acoes
    char direcao; //indica a direcao que o boi está virado
	char direcao2;
	Pos posicao; //posicao (x,y) que o boi está
}Player;

//Gera a estrutura definida pra o jogador padrão
Player defaultPlayer(int id_player){
  Player temp;
  temp.id = id_player;
  temp.vida=MAX_HP; 
  temp.movimento=NENHUM;
  temp.ataque=NENHUM;
  strcpy(temp.login,"");
  temp.estado = DIREITA;
  temp.personagem=BOI_1;
  temp.blocked=0;
  switch(id_player){
    case 0:
      temp.posicao.x=3;
      temp.posicao.y=3;
	  break;
    case 1:
      temp.posicao.x=22;
      temp.posicao.y=3;
	  break;
    case 2:
      temp.posicao.x=3;
      temp.posicao.y=9;
	  break;
    case 3:
      temp.posicao.x=22;
      temp.posicao.y=9;
	  break;
  }
  temp.direcao = DIREITA;
  temp.direcao = BAIXO;
  return temp;
}

//Transforma todos os players do vetor em players padrão
//Só é chamada uma vez no inicio da execução do programa
void clearListPlayers(Player * lista_jogadores){
  int i;
  for(i=0;i<MAX_CLIENTS;i++){
    lista_jogadores[i] = defaultPlayer(i);
  }
}

Player lista_jogadores[MAX_CLIENTS];

//Imprime no terminal todos os parametros de uma certa estrutura do tipo Player
//Usada para depuração, por permitir ver os valores trabalhados pelo servidor e pelo cliente
void printPlayer(Player one){
  printf("Id: %d\n", one.id);
  printf("Vida: %d\n", one.vida);
  printf("Movimento: %c\n", one.movimento);
  printf("Ataque: %c\n", one.ataque);
  printf("Login: %s\n", one.login);
  printf("Personagem: %d\n", one.personagem);
  printf("Blockeado: %d\n", one.blocked);
  printf("X: %d\n", one.posicao.x);
  printf("Y: %d\n", one.posicao.y);
  printf("Direcao: %c\n", one.direcao);
}

//Guarda o id do usario numa variavel global, permitindo a ele acessar seus proprios dados no vetor de Players
int meu_id;

//Função que permite que o usuario se conecte a um certo IP do servidor
enum conn_ret_t tryConnect(char IP[]) {
  char server_ip[30];
  //printf("Please enter the server IP: ");
  //scanf(" %s", server_ip);
  //getchar();
  return connectToServer(IP);
}

//Tenta se conectar com IP e login fornecidos a um certo servidor
//Verifica a conexao com o servidor e guarda o ID fornecido pelo servidor na variavel "meu_id"
void assertConnection(char IP[], char login[]) {
  //printHello();
  enum conn_ret_t ans = tryConnect(IP);

  while (ans != SERVER_UP) 
  {
    if (ans == SERVER_DOWN) {
      puts("Server is down!");
    } else if (ans == SERVER_FULL) {
      puts("Server is full!");
    } else if (ans == SERVER_CLOSED) {
      puts("Server is closed for new connections!");
    } else {
      puts("Server didn't respond to connection!");
    }
    printf("Want to try again? [Y/n] ");
    int res;
    while (res = tolower(getchar()), res != 'n' && res != 'y' && res != '\n'){
      puts("anh???");
    }
    if (res == 'n') {
      exit(EXIT_SUCCESS);
    }
    ans = tryConnect(IP);
  }
  int len = (int)strlen(login);
  sendMsgToServer(login, len + 1);
  recvMsgFromServer(&meu_id, WAIT_FOR_IT);
}

//Função em allegro que exibe a tela de conexão
void printConnectScreen(char str[])
{
    al_draw_bitmap(fundoDeTela,-80,0,0);

    al_draw_text(start, al_map_rgb(255, 255, 255), LARGURA/ 2, 30, ALLEGRO_ALIGN_CENTRE, "Digite o IP do server:");

    if (strlen(str) > 0)
    {
        al_draw_text(start, al_map_rgb(255, 255, 255), LARGURA / 2,
                     (ALTURA - al_get_font_ascent(start)) / 2,
                     ALLEGRO_ALIGN_CENTRE, str);
    }
    else
    {
        al_draw_text(start, al_map_rgb(255, 255, 255), LARGURA / 2,
                     (ALTURA - al_get_font_ascent(start)) / 2,
                     ALLEGRO_ALIGN_CENTRE, "0.0.0.0");
    }
}

//Funcao em allegro que exibe a tela de login
void printLoginScreen(char str[])
{
    al_draw_bitmap(fundoDeTela,-80,0,0);

    al_draw_text(start, al_map_rgb(255, 255, 255), LARGURA / 2, 30, ALLEGRO_ALIGN_CENTRE, "Digite o Login desejado: ");

    if (strlen(str) > 0)
    {
        al_draw_text(start, al_map_rgb(255, 255, 255), LARGURA / 2,
                     (ALTURA - al_get_font_ascent(start)) / 2,
                     ALLEGRO_ALIGN_CENTRE, str);
    }
    else
    {
        al_draw_text(start, al_map_rgb(255, 255, 255), LARGURA / 2,
                     (ALTURA - al_get_font_ascent(start)) / 2,
                     ALLEGRO_ALIGN_CENTRE, "<login>");
    }
}

void printWaitroom(){
	al_draw_bitmap(fundoDeTela,-80,0,0);
	al_draw_text(start, al_map_rgb(255, 255, 255), LARGURA / 2, ALTURA/2, ALLEGRO_ALIGN_CENTRE, "ESPERANDO OS JOGADORES SE CONECTAREM");
}
//Função em allegro que exibe o background
void printBackground(){
	al_clear_to_color(al_map_rgb(255, 255, 255));
	al_draw_bitmap(background, 0, 0, 0);
}

void printCharacter(int boi, float largura, float altura){
	switch(boi){
			case BOI_1:
				al_draw_bitmap(boi_1, largura, altura, 0);
				break;
			case BOI_2:
				al_draw_bitmap(boi_2, largura, altura, 0);
				break;
			case BOI_3:
				al_draw_bitmap(boi_3, largura, altura, 0);
				break;
			case BOI_4:
				al_draw_bitmap(boi_4, largura, altura, 0);
				break;
		}
}
void printGameCharacter(int boi, float largura, float altura, char estado, char direcao, char direcao2, char ataque){
	if(estado == DIREITA){
		switch(boi){
			case BOI_1:
				if(direcao2 == BAIXO) al_draw_bitmap(boi_1_pequeno, largura, altura, 0);
				else al_draw_bitmap(boi_1_pequenoUp, largura, altura, 0);
				break;
			case BOI_2:
				if(direcao2 == BAIXO) al_draw_bitmap(boi_2_pequeno, largura, altura, 0);
				else al_draw_bitmap(boi_2_pequenoUp, largura, altura, 0);
				break;
			case BOI_3:
				if(direcao2 == BAIXO) al_draw_bitmap(boi_3_pequeno, largura, altura, 0);
				else al_draw_bitmap(boi_3_pequenoUp, largura, altura, 0);
				break;
			case BOI_4:
				if(direcao2 == BAIXO) al_draw_bitmap(boi_4_pequeno, largura, altura, 0);
				else al_draw_bitmap(boi_4_pequenoUp, largura, altura, 0);
				break;
		}
	}else if(estado == ESQUERDA){
		switch(boi){
			case BOI_1:
				if(direcao2 == BAIXO) al_draw_bitmap(boi_1_pequeno, largura, altura, ALLEGRO_FLIP_HORIZONTAL);
				else al_draw_bitmap(boi_1_pequenoUp, largura, altura, ALLEGRO_FLIP_HORIZONTAL);
				break;
			case BOI_2:
				if(direcao2 == BAIXO) al_draw_bitmap(boi_2_pequeno, largura, altura, ALLEGRO_FLIP_HORIZONTAL);
				else al_draw_bitmap(boi_2_pequenoUp, largura, altura, ALLEGRO_FLIP_HORIZONTAL);
				break;
			case BOI_3:
				if(direcao2 == BAIXO) al_draw_bitmap(boi_3_pequeno, largura, altura, ALLEGRO_FLIP_HORIZONTAL);
				else al_draw_bitmap(boi_3_pequenoUp, largura, altura, ALLEGRO_FLIP_HORIZONTAL);
				break;
			case BOI_4:
				if(direcao2 == BAIXO) al_draw_bitmap(boi_4_pequeno, largura, altura, ALLEGRO_FLIP_HORIZONTAL);
				else al_draw_bitmap(boi_4_pequenoUp, largura, altura, ALLEGRO_FLIP_HORIZONTAL);
				break;
		}
	}else if(estado == ATACANDO){
		switch(boi){
			case BOI_1:
				if(direcao == DIREITA){
					if(ataque == ATAQUE_1) al_draw_bitmap(boi_1_ataqueUp, largura, altura, 0);
					else al_draw_bitmap(boi_1_ataque, largura, altura, 0);
				}else{
					if(ataque == ATAQUE_1) al_draw_bitmap(boi_1_ataqueUp, largura, altura, ALLEGRO_FLIP_HORIZONTAL);
					else al_draw_bitmap(boi_1_ataque, largura, altura, ALLEGRO_FLIP_HORIZONTAL);
				}
				break;
			case BOI_2:
				if(direcao == DIREITA){
					if(ataque == ATAQUE_1) al_draw_bitmap(boi_2_ataqueUp, largura, altura, 0);
					else al_draw_bitmap(boi_2_ataque, largura, altura, 0);
				}else{
					if(ataque == ATAQUE_1) al_draw_bitmap(boi_2_ataqueUp, largura, altura, ALLEGRO_FLIP_HORIZONTAL);
					else al_draw_bitmap(boi_2_ataque, largura, altura, ALLEGRO_FLIP_HORIZONTAL);
				}
				break;
			case BOI_3:
				if(direcao == DIREITA){
					if(ataque == ATAQUE_1) al_draw_bitmap(boi_3_ataqueUp, largura, altura, 0);
					else al_draw_bitmap(boi_3_ataque, largura, altura, 0);
				}else{
					if(ataque == ATAQUE_1) al_draw_bitmap(boi_3_ataqueUp, largura, altura, ALLEGRO_FLIP_HORIZONTAL);
					else al_draw_bitmap(boi_3_ataque, largura, altura, ALLEGRO_FLIP_HORIZONTAL);
				}
				break;
			case BOI_4:
				if(direcao == DIREITA){
					if(ataque == ATAQUE_1) al_draw_bitmap(boi_4_ataqueUp, largura, altura, 0);
					else al_draw_bitmap(boi_4_ataque, largura, altura, 0);
				}else{
					if(ataque == ATAQUE_1) al_draw_bitmap(boi_4_ataqueUp, largura, altura, ALLEGRO_FLIP_HORIZONTAL);
					else al_draw_bitmap(boi_4_ataque, largura, altura, ALLEGRO_FLIP_HORIZONTAL);
				}
				break;
		}
	}
}

//Funçao que exibe os player de login diferente de "" na posição fornecida pela sua estrutura
void printPlayers(Player * lista_jogadores){
	int i;
	//ALLEGRO_COLOR cor = al_map_rgb(255,0,0);
	for(i=0;i<MAX_CLIENTS;i++){
		if(strcmp(lista_jogadores[i].login,"") != 0){
			//al_draw_filled_circle(LARGURA/22*(2*(float)lista_jogadores[i].posicao.x+1),ALTURA/22*(2*(float)lista_jogadores[i].posicao.y+1), 10, cor);
			printGameCharacter(lista_jogadores[i].personagem,58+32*(float)lista_jogadores[i].posicao.x, 64+32*(float) lista_jogadores[i].posicao.y, lista_jogadores[i].estado, lista_jogadores[i].direcao, lista_jogadores[i].direcao2, lista_jogadores[1].ataque);
		}
	}
}
void printSeta(int botao){
	switch(botao){
		case 1:
			al_draw_bitmap(seta, 310, 320, 0);
			break;
		case 2:
			al_draw_bitmap(seta, 310, 380, 0);
			break;
		case 3:
			al_draw_bitmap(seta, 310, 440, 0);
			break;
	}
}
void printStoryScreen(){
	al_clear_to_color(al_map_rgb(255, 255, 255));
	al_draw_bitmap(story, 0, 0, 0);
}
void printBotao(){
	al_draw_bitmap(botao_1, LARGURA/2 - 64, 320, 0);
	al_draw_bitmap(botao_2, LARGURA/2 - 64, 380, 0);
	al_draw_bitmap(botao_3, LARGURA/2 - 64, 440, 0);
}
int playersReady(Player * playerList){
	int i, ready = 0;
	for(i=0;i<MAX_CLIENTS;i++){
		if(strcmp(playerList[i].login,"")!=0){
			ready++;
		}
	}
	return ready;
}
void printWinScreen(){
	al_clear_to_color(al_map_rgb(255, 255, 255));
	al_draw_bitmap(vitoria, 0, 0, 0);
	al_flip_display();
}
void printLossScreen(){
	al_clear_to_color(al_map_rgb(255, 255, 255));
	al_draw_bitmap(derrota, 0, 0, 0);
	al_flip_display();
}
int main(){
	clearListPlayers(lista_jogadores);
	//para sair do jogo
	bool open = true;
	bool introScreen = true;
	bool storyScreen = false;
    bool gameScreen = true;
	bool characterScreen = true;
    bool connectScreen = true;
    bool loginScreen = true;
	bool waitingScreen = true;
	bool lossScreen = false;
	bool winScreen = false;
    //Salva o servidor
    char ServerIP[30] = {"127.0.0.1"};
    //Salva o login
    char loginMsg[BUFFER_SIZE]={0};

    //inicializa a parte do allegro
    //A primeira coisa a ser feita é inicializar os módulos centrais. Caso algum deles falhe, o programa já para por aí.
    if (!coreInit()){
        return -1;
	}
    //Agora vamos criar a nossa janela. Largura e Altura em pixels, o título é uma string.
    if (!windowInit(LARGURA, ALTURA, "A lenda de Zilboi")){
	    return -1;
	}
    //Agora inicializamos nosso teclado e mouse, para que a janela responda às entradas deles
    if (!inputInit()){
		printf("Erro no teclado");
        return -1;
	}
    //Agora inicializamos nossas fontes
    if(!fontInit()){
      return -1;
	}
    //E por fim todas as imagens que vamos utilizar no programa.
    if(!loadGraphics()){
        return -1;
	}
    ///////

    while(open){
		int botao = 1;
		while(introScreen){
			startTimer();
	        while(!al_is_event_queue_empty(fila_eventos))
	        {
	            ALLEGRO_EVENT eventoIntro;
	            al_wait_for_event(fila_eventos, &eventoIntro);

	            if (eventoIntro.type == ALLEGRO_EVENT_KEY_DOWN)
	            {
	                switch(eventoIntro.keyboard.keycode)
	                {
	                    case ALLEGRO_KEY_DOWN:
							botao+=1;
							botao%=4;
							if(botao == 0){
								botao++;
							}
	                        break;
						case ALLEGRO_KEY_UP:
							botao-=1;
							if(botao == 0){
								botao+=3;
							}
	                        break;
						case ALLEGRO_KEY_ENTER:
							if(botao == 1){
								introScreen = false;
							}else if(botao == 2){
								storyScreen = true;
							}else if(botao == 3){
								introScreen = false;
								connectScreen = false;
								loginScreen = false;
								characterScreen = false;
								gameScreen = false;
								waitingScreen = false;
								open = false;
							}
	                        break;
	                }
	            }

	            if(eventoIntro.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
	                introScreen = false;
					connectScreen = false;
	                loginScreen = false;
					characterScreen = false;
					waitingScreen = false;
	                gameScreen = false;
	                open = false;
	                //quit = true;
	            }
	        }
			while(storyScreen){
				startTimer();

				while(!al_is_event_queue_empty(fila_eventos))
				{
					ALLEGRO_EVENT eventoStory;
					al_wait_for_event(fila_eventos, &eventoStory);

					if (eventoStory.type == ALLEGRO_EVENT_KEY_DOWN)
					{
						switch(eventoStory.keyboard.keycode)
						{
							case ALLEGRO_KEY_ENTER:
								storyScreen = false;
								break;
						}
					}

					if(eventoStory.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
						connectScreen = false;
						loginScreen = false;
						characterScreen = false;
						waitingScreen = false;
						gameScreen = false;
						open = false;;
					}
				}

				printStoryScreen();
				al_flip_display();
				al_clear_to_color(al_map_rgb(0, 0, 0));
				FPSLimit();
			}
	        al_draw_bitmap(intro, 0,0,0);
			printBotao();
			printSeta(botao);
	        al_flip_display();
	        al_clear_to_color(al_map_rgb(0, 0, 0));
	        FPSLimit();

		}
    	//executa a tela de conexao se connectScreen = true
	    while(connectScreen){
	        startTimer();

	        while(!al_is_event_queue_empty(fila_eventos))
	        {
	            ALLEGRO_EVENT eventoIP;
	            al_wait_for_event(fila_eventos, &eventoIP);

	            readInput(eventoIP, ServerIP, IP_MAX_SIZE);

	            if (eventoIP.type == ALLEGRO_EVENT_KEY_DOWN)
	            {
	                switch(eventoIP.keyboard.keycode)
	                {
	                    case ALLEGRO_KEY_ENTER:
	                        connectScreen = false;
	                        break;
	                }
	            }

	            if(eventoIP.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
	                connectScreen = false;
	                loginScreen = false;
					characterScreen = false;
					waitingScreen = false;
	                gameScreen = false;
	                open = false;
	                //quit = true;
	            }
	        }

	        printConnectScreen(ServerIP);
	        al_flip_display();
	        al_clear_to_color(al_map_rgb(0, 0, 0));
	        FPSLimit();
	    }
	    //executa a tela de conexao se loginScreen = true
	    while(loginScreen){
	        
	        startTimer();
	        
	        while(!al_is_event_queue_empty(fila_eventos)){
	            
	            ALLEGRO_EVENT eventoLogin;
	            al_wait_for_event(fila_eventos, &eventoLogin);

	            readInput(eventoLogin, loginMsg, LOGIN_MAX_SIZE);

	            if (eventoLogin.type == ALLEGRO_EVENT_KEY_DOWN)
	            {
	                switch(eventoLogin.keyboard.keycode)
	                {
	                    case ALLEGRO_KEY_ENTER:
	                        loginScreen = false;
	                        break;
	                }
	            }

	            if(eventoLogin.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
	                loginScreen = false;
					characterScreen = false;
					waitingScreen = false;
	                gameScreen = false;
	                open = false;
	            }
	        }

	        printLoginScreen(loginMsg);
	        al_flip_display();
	        al_clear_to_color(al_map_rgb(0, 0, 0));
	        FPSLimit();
	    }
		int personagem_exibido = BOI_1;
		//executa a tela de escolhaDePersonagem se characterScreen = true
		while(characterScreen){
			startTimer();

			while(!al_is_event_queue_empty(fila_eventos)){
	            
	            ALLEGRO_EVENT eventoPersonagem;
	            al_wait_for_event(fila_eventos, &eventoPersonagem);

	            if (eventoPersonagem.type == ALLEGRO_EVENT_KEY_DOWN)
	            {
	                switch(eventoPersonagem.keyboard.keycode){
	                    case ALLEGRO_KEY_RIGHT:
							personagem_exibido+=1;
							personagem_exibido%=5;
							if(personagem_exibido == 0){
								personagem_exibido++;
							}
	                        break;
						case ALLEGRO_KEY_LEFT:
							personagem_exibido-=1;
							if(personagem_exibido == 0){
								personagem_exibido+=4;
							}
	                        break;
						case ALLEGRO_KEY_ENTER:
							characterScreen = false;
	                        break;
	                }
	            }

	            if(eventoPersonagem.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
					characterScreen = false;
					waitingScreen = false;
	                gameScreen = false;
	                open = false;
	            }
	        }
			al_draw_bitmap(fundoDeTela,-80,0,0);
			al_draw_text(start, al_map_rgb(255, 255, 255), LARGURA/ 2, 30, ALLEGRO_ALIGN_CENTRE, "Escolha o seu personagem:");
			printCharacter(personagem_exibido, LARGURA/4, ALTURA/4);
			al_flip_display();
	    	al_clear_to_color(al_map_rgb(0, 0, 0));
			FPSLimit();
		}
	    //Realiza conexão com o server
		if(waitingScreen){
			assertConnection(ServerIP, loginMsg);
			sendMsgToServer((void*)&personagem_exibido, sizeof(personagem_exibido));
			printWaitroom();
			al_flip_display();
		}
		while(waitingScreen){
			int ret = recvMsgFromServer(lista_jogadores, DONT_WAIT);
		    if (ret == SERVER_DISCONNECTED) {
		      return -1;
		    } else if (ret != NO_MESSAGE) {
		    	int ready = playersReady(lista_jogadores);
				printf("[[%d]]",ready);
				if(ready == MAX_CLIENTS){
					waitingScreen = false;
				}
		    }
			while(!al_is_event_queue_empty(fila_eventos)){
	            
	            ALLEGRO_EVENT eventoWait;
	            al_wait_for_event(fila_eventos, &eventoWait);

	            if(eventoWait.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
					waitingScreen = false;
	                gameScreen = false;
	                open = false;
	            }
	        }
		}
	    //executa a tela de conexao se gameScreen = true
	    while(gameScreen){
	    	startTimer();
			while(!al_is_event_queue_empty(fila_eventos)){
				ALLEGRO_EVENT eventoGame;
				al_wait_for_event(fila_eventos, &eventoGame);
				if (eventoGame.type == ALLEGRO_EVENT_KEY_DOWN){
					switch(eventoGame.keyboard.keycode){
						int ret;
						case ALLEGRO_KEY_DOWN:
							lista_jogadores[meu_id].movimento = BAIXO;
							ret = sendMsgToServer((void *)lista_jogadores, sizeof(Player)*MAX_CLIENTS);
							if (ret == SERVER_DISCONNECTED) {
								return -1;
							}
							break;
						case ALLEGRO_KEY_LEFT:
							lista_jogadores[meu_id].movimento = ESQUERDA;
							lista_jogadores[meu_id].estado = ESQUERDA;
							ret = sendMsgToServer((void *)lista_jogadores, sizeof(Player)*MAX_CLIENTS);
							if (ret == SERVER_DISCONNECTED) {
								return -1;
							}
							break;
						case ALLEGRO_KEY_RIGHT:
							lista_jogadores[meu_id].movimento = DIREITA;
							lista_jogadores[meu_id].estado = DIREITA;
							ret = sendMsgToServer((void *)lista_jogadores, sizeof(Player)*MAX_CLIENTS);
							if (ret == SERVER_DISCONNECTED) {
								return -1;
							}
							break;
						case ALLEGRO_KEY_UP:
							lista_jogadores[meu_id].movimento = CIMA;
							ret = sendMsgToServer((void *)lista_jogadores, sizeof(Player)*MAX_CLIENTS);
							if (ret == SERVER_DISCONNECTED) {
								return -1;
							}
							break;
						case ALLEGRO_KEY_Z:
							lista_jogadores[meu_id].ataque = ATAQUE_1;
							lista_jogadores[meu_id].estado = ATACANDO;
							ret = sendMsgToServer((void *)lista_jogadores, sizeof(Player)*MAX_CLIENTS);
							if (ret == SERVER_DISCONNECTED) {
								return -1;
							}
							break;
						case ALLEGRO_KEY_X:
							lista_jogadores[meu_id].ataque = ATAQUE_2;
							lista_jogadores[meu_id].estado = ATACANDO;
							ret = sendMsgToServer((void *)lista_jogadores, sizeof(Player)*MAX_CLIENTS);
							if (ret == SERVER_DISCONNECTED) {
								return -1;
							}
							break;
					}
				}

				if(eventoGame.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
					gameScreen = false;
					open = false;
				}
			}
	        int ret = recvMsgFromServer(lista_jogadores, DONT_WAIT);
		    if (ret == SERVER_DISCONNECTED) {
		      return -1;
		    } else if (ret != NO_MESSAGE) {
		    	printf("Recebeu uma mensagem do servidor!\n");
		    	printPlayer(lista_jogadores[meu_id]);
		    }

			if(strcmp(lista_jogadores[meu_id].login,"")==0){
				gameScreen = false;
				lossScreen = true;
			}
			else if(playersReady(lista_jogadores) == 1){
				gameScreen = false;
				winScreen = true;
			}


	    	printBackground();
	    	printPlayers(lista_jogadores);
	    	al_flip_display();
	    	al_clear_to_color(al_map_rgb(0, 0, 0));

			int i;
			for(i=0;i<MAX_CLIENTS;i++){
				lista_jogadores[i].estado = lista_jogadores[i].direcao;
			}
	    	printBackground();
	    	printPlayers(lista_jogadores);
	    	al_flip_display();
	    	al_clear_to_color(al_map_rgb(0, 0, 0));

	    	FPSLimit();
	    }
		//se o cliente perdeu
		if(lossScreen){
		printLossScreen();
		al_rest(5);
		open = false;
		}
		//se o cliente ganhou
		else if(winScreen){
			printWinScreen();
			al_rest(5);
			open = false;
		}
	}
    allegroEnd();
}
