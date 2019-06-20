#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOGIN_MAX_SIZE 13
#define MAX_CLIENTS 4
#define MAX_HP 500

#define NENHUM 'p'
#define ATACANDO 'l'
#define ESQUERDA 'a'
#define DIREITA 'd'
#define CIMA 'w'
#define BAIXO 's'

#define BOI_1 1
#define BOI_2 2
#define BOI_3 3
#define BOI_4 4

#define ALTURA 630
#define LARGURA 940

#define ALTURA_MAPA 12
#define LARGURA_MAPA 25

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
    char estado; //indica o estado do boi
    int personagem; //é um personagem de define igual BOI_X, em que X está no intervalo [1,4]
    int blocked; //número de frames em que o boi vai executar uma animacao de levar dano, ficando sem poder executar acoes
    char direcao; //indica a direcao que o boi está virado
    Pos posicao; //posicao (x,y) que o boi está
}Player;

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

int checkPosition(int xCheck, int yCheck, Player* playerList){
  int i;
  for(i=0;i<MAX_CLIENTS;i++){
    if(strcmp(playerList[i].login,"")!=0 && playerList[i].posicao.x == xCheck && playerList[i].posicao.y == yCheck){
      return 0;
    }
  }
  return 1;
}

Player defaultPlayer(int id_player){
  Player temp;
  temp.id = id_player;
  temp.vida=MAX_HP; 
  temp.movimento=NENHUM;
  temp.ataque=NENHUM;
  strcpy(temp.login,"");
  temp.estado=DIREITA;
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
  return temp;
}

int clearListPlayers(Player* lista_jogadores){
  int i;
  for(i=0;i<MAX_CLIENTS;i++){
    lista_jogadores[i] = defaultPlayer(i);
  }
  return 0;
}

Player lista_jogadores[MAX_CLIENTS];

void movePlayer(Player * playerList, int id, int xi, int yi, int xd, int yd){
  if(yd < 0){
    yd = 0;
  }else if(yd > ALTURA_MAPA - 1){
    yd = ALTURA_MAPA-1;
  }
  if(xd < 0){
    xd = 0;
  }else if(xd > LARGURA_MAPA - 1){
    xd = LARGURA_MAPA-1;
  }
  int canWalk;
  canWalk = checkPosition(xd,yd,lista_jogadores);
  if(canWalk){
    playerList[id].posicao.y+=yd-yi;
    playerList[id].posicao.x+=xd-xi;
  }
  if(playerList[id].movimento == DIREITA || playerList[id].movimento == ESQUERDA) {
    playerList[id].direcao = playerList[id].movimento;
  }
  playerList[id].movimento = NENHUM;
}
void attack(int id){
  switch(lista_jogadores[id].direcao){
    case CIMA:
      for(int i=0; i<MAX_CLIENTS; i++){
        if(lista_jogadores[id].posicao.x == lista_jogadores[i].posicao.x && lista_jogadores[id].posicao.y-1 == lista_jogadores[i].posicao.y){
          lista_jogadores[i].vida -= 25;
        }
      }
      break;
    case BAIXO:
      for(int i=0; i<MAX_CLIENTS; i++){
        if(lista_jogadores[id].posicao.x == lista_jogadores[i].posicao.x && lista_jogadores[id].posicao.y+1 == lista_jogadores[i].posicao.y){
          lista_jogadores[i].vida -= 25;
        }
      }
      break;
    case ESQUERDA:
      for(int i=0; i<MAX_CLIENTS; i++){
        if(lista_jogadores[id].posicao.x-1 == lista_jogadores[i].posicao.x && lista_jogadores[id].posicao.y == lista_jogadores[i].posicao.y){
          lista_jogadores[i].vida -= 25;
        }
      }
      break;
    case DIREITA:
      for(int i=0; i<MAX_CLIENTS; i++){
        if(lista_jogadores[id].posicao.x+1 == lista_jogadores[i].posicao.x && lista_jogadores[id].posicao.y == lista_jogadores[i].posicao.y){
          lista_jogadores[i].vida -= 25;
        }
      }
      break;
  }
  lista_jogadores[id].ataque = NENHUM;
}

int x, y;

int main() {
  clearListPlayers(lista_jogadores);
  serverInit(MAX_CLIENTS);
  puts("Server is running!!");
  int connected = 0;
  while (1) {
    while(connected<MAX_CLIENTS){
      int id_player = acceptConnection();
      if (id_player != NO_CONNECTION) {
        char login[LOGIN_MAX_SIZE];
        recvMsgFromClient(login, id_player, WAIT_FOR_IT);
        printf("LOGIN:::%s - id:%d conectou ao servidor\n", login, id_player);
        strcpy(lista_jogadores[id_player].login, login);
        lista_jogadores[id_player].id = id_player;
        sendMsgToClient(&id_player, sizeof(int), id_player);
        recvMsgFromClient(&lista_jogadores[id_player].personagem, id_player, WAIT_FOR_IT);
        broadcast(lista_jogadores, sizeof(Player)*MAX_CLIENTS);
        connected++;
      }
      struct msg_ret_t msg_ret = recvMsg(lista_jogadores);
      if (msg_ret.status == DISCONNECT_MSG) {
        printf("%s disconnected, id = %d is free\n", lista_jogadores[msg_ret.client_id].login, msg_ret.client_id);
        lista_jogadores[msg_ret.client_id] = defaultPlayer(msg_ret.client_id);
        broadcast(lista_jogadores, sizeof(Player)*MAX_CLIENTS);
        connected--;
      }
    }
    broadcast(lista_jogadores, sizeof(Player)*MAX_CLIENTS);
    while(connected!=0){
      struct msg_ret_t msg_ret = recvMsg(lista_jogadores);
      if (msg_ret.status == MESSAGE_OK) {
        //printf("Mensagem recebida pelo servidor!\n");
        if(lista_jogadores[msg_ret.client_id].movimento != NENHUM){
          x = lista_jogadores[msg_ret.client_id].posicao.x;
          y = lista_jogadores[msg_ret.client_id].posicao.y;
          switch(lista_jogadores[msg_ret.client_id].movimento){
            case CIMA:
              movePlayer(lista_jogadores, msg_ret.client_id, x, y, x, y-1);
              break;
            case DIREITA:
              movePlayer(lista_jogadores, msg_ret.client_id, x, y, x+1, y);
              break;
            case ESQUERDA:
              movePlayer(lista_jogadores, msg_ret.client_id, x, y, x-1, y);
              break;
            case BAIXO:
              movePlayer(lista_jogadores, msg_ret.client_id, x, y, x, y+1);
              break;
          }
        }
        if(lista_jogadores[msg_ret.client_id].ataque != NENHUM){
          switch(lista_jogadores[msg_ret.client_id].ataque){
            case ATAQUE_1:
              attack(msg_ret.client_id);
              break;
            case ATAQUE_2:
              attack(msg_ret.client_id);
              break;
          }
        }
        int i;
        for(i=0;i<MAX_CLIENTS;i++){
          if(lista_jogadores[i].vida<=0){
            lista_jogadores[i] = defaultPlayer(i);
          }
        }
        broadcast(lista_jogadores, sizeof(Player)*MAX_CLIENTS);
      }else if (msg_ret.status == DISCONNECT_MSG) {
        printf("%s disconnected, id = %d is free\n", lista_jogadores[msg_ret.client_id].login, msg_ret.client_id);
        lista_jogadores[msg_ret.client_id] = defaultPlayer(msg_ret.client_id);
        broadcast(lista_jogadores, sizeof(Player)*MAX_CLIENTS);
        connected--;
      }
    }
  }
}
