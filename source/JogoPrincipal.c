#include <nds.h>
#include <stdio.h>
#include <time.h>

//#include "../data/bg0.h"
#include "../data/bg3.h"
//#include "../data/tileset0.h"
#include "../data/sprites.h"

#include "../header/MenuPrincipal.h"
#include "../header/JogoPrincipal.h"

PrintConsole consoleMain;



spritetiro sptiro[3] = {
                            {0, 0, 1, true},
                            {0, 0, 2, true},
                            {0, 0, 3, true}
                        }; //Vetor fixo que representa o número máximo de tiros simultâneos que podem ser disparados.

spriteinimigo **spinimigo;

unsigned int itotalinimigos = 0;
unsigned int iplacar = 0;
bool bfim = false;                  // Controla o fim de jogo.


//O código está feio, cheio de funções acopladas mas funciona.

int fnc_inicializar_fase(void){

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankB(VRAM_B_MAIN_SPRITE);

    videoSetMode(MODE_3_2D | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE);

    int bg3 = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 1, 0);
    dmaCopy(bg3Bitmap, bgGetGfxPtr(bg3), 256*256*2);

    consoleInit(&consoleMain, 2, BgType_Text4bpp, BgSize_T_256x256 , 2, 0, true, true);

    iprintf("Placar \n");
    iprintf("%6d", iplacar);

    oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false);
    u16 *spt = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_Bmp);
    //dmaCopy(tileset0Bitmap, spt, 32*32*2);
    dmaCopy(spritesBitmap, spt, 32*224*2);

    spnave.x = NAVEPOSICAOX;
    spnave.y = NAVEPOSICAOY;
    spnave.sprite = SPRITENAVE;
    spnave.frame = CENTRO;

    oamSet(
     &oamMain,                      //sub display
     spnave.sprite,                 //oam entry to set
     spnave.x, spnave.y,            //position
     0,                             //priority
     15,                            //palette for 16 color sprite or alpha for bmp sprite
     SpriteSize_32x32,
     SpriteColorFormat_Bmp,
     oamGetGfxPtr(&oamMain, CENTRO),
     -1,
     false,                         //double the size of rotated sprites
     false,                         //don't hide the sprite
     false, false,                  //vflip, hflip
     false                          //apply mosaic
     );
/*
    oamSet(
     &oamMain,                      //sub display
     sptiro[0].sprite,              //oam entry to set
     sptiro[0].x, sptiro[0].y,      //position
     0,                             //priority
     15,                            //palette for 16 color sprite or alpha for bmp sprite
     SpriteSize_32x32,
     SpriteColorFormat_Bmp,
     oamGetGfxPtr(&oamMain, 16*5),
     -1,
     false,                         //double the size of rotated sprites
     false,                         //don't hide the sprite
     false, false,                  //vflip, hflip
     false                          //apply mosaic
     );

*/
    oamUpdate(&oamMain);

    fnc_alocar_inimigos();

    return 0;
}

void fnc_jogar() {
    int itecla = 0;     // Armazena a tecla atual pressionada.
    int iulttecla = 0;  // Armazena a última tecla pressionada.

    short sframe = 0;   // Número do frame para controlar a animação da nave.
    bfim = false;
    iplacar = 0;

    fnc_inicializar_fase();


    while (!bfim) {

        spnave.frame = CENTRO;

        itecla = 0;

        scanKeys();
        itecla = keysHeld();

        if ((itecla == 0) || (iulttecla != ((itecla & KEY_LEFT) | (itecla & KEY_RIGHT))))  { //Para executar a animação apenas preciso saber se foi pressionado a tecla direita ou esqueda.

            sframe = 0;

        }

        iulttecla = ((itecla & KEY_LEFT) | (itecla & KEY_RIGHT)); //Para executar a animação apenas preciso saber se foi pressionado a tecla direita ou esqueda.

        if (itecla & KEY_LEFT) { //Controla o posicionamento da nave e animação quando vira para esquerda

            sframe ++;
            if (spnave.x >=0) {
                spnave.x--;
                if (sframe >= 60) {
                    spnave.frame = ESQUERDA100;
                } else if (sframe >= 30) {
                    spnave.frame = ESQUERDA50;
                }
            }

        }

        if (itecla & KEY_RIGHT) { //Controla o posicionamento da nave e animação quando vira para direita

            sframe ++;
            if (spnave.x <= (LARGURATELA - SPRITELARGURA)) {
                spnave.x++;
                if (sframe >= 60) {
                    spnave.frame = DIREITA100;
                } else if (sframe >= 30) {
                    spnave.frame = DIREITA50;
                }

            }
        }

        if (itecla & KEY_UP) {

            if (spnave.y >= 0) {
                spnave.y--;
            }

        }

        if (itecla & KEY_DOWN) {

            if (spnave.y <= (ALTURATELA - SPRITEALTURA))
            spnave.y++;

        }


        iprintf("\x1b[1;0H %6d", iplacar);
        iprintf("\x1b[0;15H %05d %05d", spnave.x, spnave.y);
        iprintf("\x1b[1;15H %05d", sframe);

        oamSet  (
                    &oamMain,                           //main display
                    spnave.sprite,                      //oam entry to set
                    spnave.x, spnave.y,                 //position
                    0,                                  //priority
                    15,                                 //palette for 16 color sprite or alpha for bmp sprite
                    SpriteSize_32x32,
                    SpriteColorFormat_Bmp,
                    oamGetGfxPtr(&oamMain, 16*spnave.frame),
                    -1,
                    false,                              //double the size of rotated sprites
                    false,                              //don't hide the sprite
                    false, false,                       //vflip, hflip
                    false                               //apply mosaic
                );

        //Melhorando um pouco a função para tratar o acoplamento.
        f_animar_tiro (itecla, sptiro);
        //Função tem muito para melhorar mas funciona.
        f_animar_inimigos();
        f_tratarcolisao();

        swiWaitForVBlank();
        oamUpdate(&oamMain);

        if (bfim) {

            oamDisable (&oamMain);
            iprintf("\x1b[13;2H Se ferrou, fim de jogo !!!");
            iprintf("\x1b[14;4H Pressione START <ENTER>.");
            bfim = true;

            while (!(itecla & KEY_START)) {
                scanKeys();
                itecla = keysHeld();
            }
        }

    }
}

//Randomiza o número de inimigos que serão criados para a fase.
int fnc_alocar_inimigos() {
    time_t t;

    srand(time(&t));
    itotalinimigos = rand() % MAXINIMIGOS;

    if (itotalinimigos < MININIMIGOS) itotalinimigos = MININIMIGOS;//Número mínimo de inimigos.
//itotalinimigos=1;
    spinimigo = (spriteinimigo**) malloc(itotalinimigos * sizeof(spriteinimigo*));

    //spinimigo[0] = (spriteinimigo*) malloc(sizeof(spriteinimigo));
    //spinimigo[1] = (spriteinimigo*) malloc(sizeof(spriteinimigo));

    for (int iinimigo = 0; iinimigo <itotalinimigos; iinimigo++) {

        spinimigo[iinimigo] = (spriteinimigo*) malloc(sizeof(spriteinimigo));

        spinimigo[iinimigo]->x = rand() % (LARGURATELA - SPRITELARGURA);
        spinimigo[iinimigo]->y = -(rand() % 200);
        spinimigo[iinimigo]->sprite = 6;
        spinimigo[iinimigo]->invisivel = false;

    }

    return 0;
}
int f_animar_tiro (int itecla, spritetiro *sp) {
    static short stiro = 0;    // Controlar mais de um tiro simultâneo.

   if (itecla & KEY_A) {

        if (sp[stiro].invisivel) {
            sp[stiro].x = spnave.x;      //O tiro sai na posição X da nave.
            sp[stiro].y = spnave.y - 20; //O tiro sai de uma distância mínima Y da nave.
            sp[stiro].invisivel = false;
        }
        if ((spnave.y - sp[stiro].y) > 50) stiro++; //Controla quando será dado o próximo tiro para evitar um tiro muito proximo do outro.
        if (stiro == MAXTIROS) stiro=0; //Se alcançado o número máximo de tiros para de atirar e reinicia o contador.

    }

    for (short s = 0; s < MAXTIROS; s++) {

        if (sp[s].y >=-50) {
            sp[s].y-=VELOCIDADETIRO;
        }

       // if (!sptiro[s].invisivel) {

            if (sp[s].y <=-50) {
                sp[s].invisivel = true;
            }

            oamSet  (
                        &oamMain,                       //main display
                        sp[s].sprite,               //oam entry to set
                        sp[s].x, sp[s].y,       //position
                        0,                              //priority
                        15,                             //palette for 16 color sprite or alpha for bmp sprite
                        SpriteSize_32x32,
                        SpriteColorFormat_Bmp,
                        oamGetGfxPtr(&oamMain, 16*5),
                        -1,
                        false,                          //double the size of rotated sprites
                        sptiro[s].invisivel,            //don't hide the sprite
                        false, false,                   //vflip, hflip
                        false                           //apply mosaic
                        );

        //}
    //oamSetXY(&oamMain, spnave.sprite , spnave.x, spnave.y);
    }

    return 0;
}


int f_animar_inimigos() {


    for (int iinimigo = 0; iinimigo < itotalinimigos; iinimigo++) {


        if (spinimigo[iinimigo]->y < 200) {

            spinimigo[iinimigo]->y++;

        } else {

            spinimigo[iinimigo]->y = -(rand() % 200);
            spinimigo[iinimigo]->x = rand() % (LARGURATELA - SPRITELARGURA);
            spinimigo[iinimigo]->invisivel = false;
        }


        oamSet  (
                &oamMain,                           //main display
                5 + iinimigo,                                  //oam entry to set
                spinimigo[iinimigo]->x, (spinimigo[iinimigo]->y < 0? 0:spinimigo[iinimigo]->y),           //position
                0,                                  //priority
                15,                                 //palette for 16 color sprite or alpha for bmp sprite
                SpriteSize_32x32,
                SpriteColorFormat_Bmp,
                oamGetGfxPtr(&oamMain, 16*spinimigo[iinimigo]->sprite),
                -1,
                false,                              //double the size of rotated sprites
                spinimigo[iinimigo]->invisivel,                              //don't hide the sprite
                false, false,                       //vflip, hflip
                false                               //apply mosaic
                );
    }
    return 0;
}

int f_tratarcolisao() {

    for (int it = 0; it < MAXTIROS; it++) {

        for (int ii = 0; ii < itotalinimigos; ii++) {
            //Mapeia a localização pontual do tiro a um quadrado representado pelo nave inimiga.
            if ((( (sptiro[it].x + (SPRITELARGURA /2)) >= spinimigo[ii]->x) && ( (sptiro[it].x + (SPRITELARGURA /2)) <= (spinimigo[ii]->x + SPRITELARGURA))) && (( (sptiro[it].y + (SPRITEALTURA / 2)) >=spinimigo[ii]->y) && ( (sptiro[it].y + (SPRITEALTURA / 2))<= (spinimigo[ii]->y + SPRITEALTURA)))) {

                if ((!spinimigo[ii]->invisivel) && (!sptiro[it].invisivel)) {
                    spinimigo[ii]->invisivel = true;
                    sptiro[it].invisivel = true;
                    iplacar += rand() % 20;
                }
            }
        }

    }

    for (int ii = 0; ii < itotalinimigos; ii++) {
        //Mapeia a localização pontual de um inimigo a um quadrado representado pela nave amiga.

        if ((( (spinimigo[ii]->x + (SPRITELARGURA / 2)) >= spnave.x) && ( (spinimigo[ii]->x + (SPRITELARGURA / 2)) <= (spnave.x + SPRITELARGURA))) && (( (spinimigo[ii]->y + (SPRITEALTURA / 2)) >=spnave.y) && ( (spinimigo[ii]->y + (SPRITEALTURA /2))<= (spnave.y + SPRITEALTURA)))) {

            if (!spinimigo[ii]->invisivel) {
                bfim = true;
            }
        }

    }
    return 0;
}
