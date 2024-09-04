#include <nds.h>
#include <stdio.h>
#include "../header/MenuPrincipal.h"
#include "../header/JogoPrincipal.h"

#include "../data/bg3.h"

PrintConsole consoleSub;

typedef struct MenuItem {
    char *nome;
    int item;
} menuitem;

menuitem items[] = {
    {
        "Iniciar",0
    },
    {
        "Instrucoes",1
    },
    {
        "Sair",2
    }
};

void prc_Instrucoes(void) {

    consoleClear();
    iprintf ("\n\tPatrick Space Shooter, um jogo emocionante.\n");
    iprintf ("\n\tJogo feito para a disciplina \n de PROGII.\n");
    iprintf ("\n\tUtilize as setas direcionais \n para controlar a nave, presione \n X para atirar.\n");
    iprintf ("\n\tJogo implementado em 30 horas \n de trabalho.");

}

int f_InicializarMenu (void) {

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    videoSetModeSub (MODE_3_2D | DISPLAY_BG3_ACTIVE);
    videoSetMode (MODE_3_2D | DISPLAY_BG3_ACTIVE);

    int bg3 = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 1, 0);
    dmaCopy(bg3Bitmap, bgGetGfxPtr(bg3), 256*194*2);

    int bg3sub = bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 1, 0);
    dmaCopy(bg3Bitmap + (256*194), bgGetGfxPtr(bg3sub), 256*194*2);



    consoleInit(&consoleSub, 0, BgType_Text4bpp, BgSize_T_256x256 , 2, 0, false, true);
    consoleSetWindow(&consoleSub, 0, 0, 32, 32);

    return 0;
}

int f_MostrarMenuPrincipal (void) {

    f_InicializarMenu();

    int keys; //Tecla pressionada
    bool bselecionado = false; //Item selecionado
    int lcursor = 0; //Localização do cursor;
    bool bfim = false;
    bool blnlertecla = true;

    while(!bfim) {
        //swiWaitForVBlank();

        keys = 0;

         while (keys==0 && blnlertecla) {
            scanKeys();
            keys = keysDown();
        }
        blnlertecla = true;
        consoleClear();

        iprintf ("\x1b[0;9HMenu principal\n\n");
        //iprintf ("%04d\n", keys);

        if (keys & KEY_DOWN) {
            lcursor++;
            if (lcursor >= MENUITEMS) lcursor= (MENUITEMS - 1);
        }

        if (keys & KEY_UP) {
            lcursor--;
            if (lcursor < 0) lcursor=0;
        }

        if (keys & KEY_A) {

            bselecionado = true;
            //break;
        }

        if (!bselecionado) {
            for (int x = 0; x < MENUITEMS; x++) {
                char cursor = (x==lcursor) ? '>' : ' '; //marca a localização do cursor.
                iprintf ("%c %s\n\n", cursor, items[x].nome);
            }
            iprintf ("Selecione opcao e aperte X");
            iprintf ("\x1b[17;4HPatrick Space Shooter");
        } else {
            iprintf ("Selecionado: %s", items[lcursor].nome);

            switch (lcursor) {
                case 0:
                    consoleClear();
                    iprintf ("\x1b[10;10H Jogar");
                    fnc_jogar();
                    f_InicializarMenu();
                    consoleSelect (&consoleSub);
                    blnlertecla=false;
                    break;
                case 1:
                    prc_Instrucoes();
                    break;
                case 2:
                    bfim =true;
                    break;
            }
            bselecionado=false;
        }

    }
    swiWaitForVBlank();
    consoleClear();

    return 0;
}
