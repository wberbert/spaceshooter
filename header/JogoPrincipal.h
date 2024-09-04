#include <stdio.h>
#include <nds.h>

#define NAVEPOSICAOX 112
#define NAVEPOSICAOY 162
#define SPRITENAVE 0

#define SPRITELARGURA 32
#define SPRITEALTURA 32
#define LARGURATELA 256
#define ALTURATELA 194

//#define SPRITETIRO 1
#define VELOCIDADETIRO 3
#define MAXTIROS 3
#define MAXINIMIGOS 20
#define MININIMIGOS 5

enum enaveframe {ESQUERDA100 = 0, ESQUERDA50 = 1, CENTRO = 2, DIREITA50 = 3, DIREITA100 = 4};

typedef struct SpriteTiro {
    int x;
    int y;
    short sprite;
    bool invisivel;
} spritetiro;

struct SpriteNave {
    int x;
    int y;
    enum enaveframe frame;
    short sprite;
};

typedef struct SpriteInimigo {
    int x;
    int y;
    short sprite;
    bool invisivel;
} spriteinimigo;

struct SpriteNave spnave;

int fnc_inicializar_fase(void);
int f_animar_inimigos(void);
int f_animar_tiro (int itecla, spritetiro *);
int fnc_alocar_inimigos(void);
int f_tratarcolisao();

void fnc_jogar(void);
