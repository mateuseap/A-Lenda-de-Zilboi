#include "ACore.h"


/*
INITIALIZATION OF BASIC CORE MODULES:
--ALLEGRO
--ALLEGRO IMAGE
--ALLEGRO FONTS
--ALLEGRO TTF FONT SUPPORT
--ALLEGRO PRIMITIVES
--EVENT QUEUE
*/
bool coreInit()
{
    //modules and add-ons initialization
	if (!al_init())
    {
        //fprintf(stderr, "Falha ao inicializar Allegro.\n");
        return false;
    }

    if (!al_init_image_addon())
    {
        //fprintf(stderr, "Falha ao inicializar add-on allegro_image.\n");
        return false;
    }

    if (!al_init_font_addon())
    {
        //fprintf(stderr, "Falha ao inicializar add-on allegro_font.\n");
        return false;
    }

    if (!al_init_ttf_addon())
    {
        //fprintf(stderr, "Falha ao inicializar add-on allegro_ttf.\n");
        return false;
    }

    if (!al_init_primitives_addon())
    {
        //fprintf(stderr, "Falha ao inicializar add-on allegro_primitives.\n");
        return false;
    }

    fila_eventos = al_create_event_queue();
    if (!fila_eventos)
    {
        //fprintf(stderr, "Falha ao criar fila de eventos.\n");
        return false;
    }

 	

    return true;
}


//FOR INITIALIZING A WINDOW OF WxH SIZE WITH TEXT "title[]"
bool windowInit(int W, int H, char title[])
{
    //window and event queue creation
    janela = al_create_display(W, H);
    if (!janela)
    {
        //fprintf(stderr, "Falha ao criar janela.\n");
        return false;
    }
    al_set_window_title(janela, title);

    //registra janela na fila de eventos
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));

    return true;
}


//FOR INITIALIZING MAIN EXTERNAL INPUTS (KEYBOARD, MOUSE AND CURSOR)
bool inputInit()
{
	/*------------------------------MOUSE--------------------------------*/
	//Inicializa Mouse
	if (!al_install_mouse())
    {
        //fprintf(stderr, "Falha ao inicializar o mouse.\n");
        al_destroy_display(janela);
        return false;
    }

    // Atribui o cursor padrão do sistema para ser usado
    if (!al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT))
    {
        //fprintf(stderr, "Falha ao atribuir ponteiro do mouse.\n");
        al_destroy_display(janela);
        return false;
    }
    /*------------------------------MOUSE--------------------------------*/

    /*------------------------------KEYBOARD------------------------------*/
    if (!al_install_keyboard())
    {
        //fprintf(stderr, "Falha ao inicializar o teclado.\n");
        return false;
    }

    /*------------------------------KEYBOARD------------------------------*/


    //Registra mouse e teclado na fila de eventos
    al_register_event_source(fila_eventos, al_get_mouse_event_source());
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());

    return true;
}


//FOR READING KEYBOARD INPUT WITH MAX SIZE = LIMIT AND SAVING AT STR[]
void readInput(ALLEGRO_EVENT event, char str[], int limit)
{
    if (event.type == ALLEGRO_EVENT_KEY_CHAR)
    {
        if (strlen(str) <= limit)
        {
            char temp[] = {event.keyboard.unichar, '\0'};
            if (event.keyboard.unichar == ' ')
            {
                strcat(str, temp);
            }
            else if (event.keyboard.unichar >= '!' &&
                     event.keyboard.unichar <= '?')
            {
                strcat(str, temp);
            }
            else if (event.keyboard.unichar >= 'A' &&
                     event.keyboard.unichar <= 'Z')
            {
                strcat(str, temp);
            }
            else if (event.keyboard.unichar >= 'a' &&
                     event.keyboard.unichar <= 'z')
            {
                strcat(str, temp);
            }
        }

        if (event.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && strlen(str) != 0)
        {
            str[strlen(str) - 1] = '\0';
        }
    }
}


//FPS CONTROL (FPS IS DEFINED IN ACORE.H)
void startTimer()
{
    startingTime = al_get_time();
}

double getTimer()
{
    return al_get_time() - startingTime;
}

void FPSLimit()
{
    if (getTimer() < 1.0/FPS)
    {
        al_rest((1.0 / FPS) - getTimer());
    }
}


//FOR DEALLOCATING ALL ALLEGRO STUFF
void allegroEnd()
{
    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);
}


//MODIFY THIS TO LOAD YOUR OWN FONTS (FONT POINTERS ARE DEFINED AT ACORE.H)
bool fontInit()
{
    /*------------------------------FONTE--------------------------------*/
    start = al_load_font("examples/simpleChat/Resources/Fonts/pressStart.ttf", 16, 0);
    if (!start)
    {
        //fprintf(stderr, "Falha ao carregar \"examples/simpleChat/Resources/Fonts/pressStart.ttf\".\n");
        return false;
    }

    ubuntu = al_load_font("examples/simpleChat/Resources/Fonts/Ubuntu-R.ttf", 32, 0);
    if (!ubuntu)
    {
        //fprintf(stderr, "Falha ao carregar \"Ubuntu-R.ttf\".\n");
        return false;
    }

    return true;
}


//MODIFY THIS TO LOAD YOUR OWN GRAPHICS (BITMAP POINTERS ARE DEFINED AT ACORE.H)
bool loadGraphics()
{
    background = al_load_bitmap("examples/simpleChat/Resources/Tilesets/background.png");
    if (!background)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }

    boi_1 = al_load_bitmap("examples/simpleChat/Resources/Tilesets/boi_1.png");
    if (!boi_1)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }

    boi_2 = al_load_bitmap("examples/simpleChat/Resources/Tilesets/boi_2.png");
    if (!boi_2)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }

    boi_3 = al_load_bitmap("examples/simpleChat/Resources/Tilesets/boi_3.png");
    if (!boi_3)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }

    boi_4 = al_load_bitmap("examples/simpleChat/Resources/Tilesets/boi_4.png");
    if (!boi_4)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }

    botao_1 = al_load_bitmap("examples/simpleChat/Resources/Tilesets/botao_1.png");
    if (!botao_1)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }

    botao_2 = al_load_bitmap("examples/simpleChat/Resources/Tilesets/botao_2.png");
    if (!botao_2)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }

    botao_3 = al_load_bitmap("examples/simpleChat/Resources/Tilesets/botao_3.png");
    if (!botao_3)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }

    intro = al_load_bitmap("examples/simpleChat/Resources/Tilesets/intro.png");
    if (!intro)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }

    seta = al_load_bitmap("examples/simpleChat/Resources/Tilesets/seta.png");
    if (!seta)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }
    fundoDeTela = al_load_bitmap("examples/simpleChat/Resources/Tilesets/telaDeFundo.jpg");
    if(!fundoDeTela){
        return false;
    }
    story = al_load_bitmap("examples/simpleChat/Resources/Tilesets/story.png");
    if(!story){
        return false;
    }
    boi_1_pequeno = al_load_bitmap("examples/simpleChat/Resources/Tilesets/boi_1_pequeno.png");
    if (!boi_1_pequeno)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }
    boi_2_pequeno = al_load_bitmap("examples/simpleChat/Resources/Tilesets/boi_2_pequeno.png");
    if (!boi_2_pequeno)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }
    boi_3_pequeno = al_load_bitmap("examples/simpleChat/Resources/Tilesets/boi_3_pequeno.png");
    if (!boi_3_pequeno)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }
    boi_4_pequeno = al_load_bitmap("examples/simpleChat/Resources/Tilesets/boi_4_pequeno.png");
    if (!boi_4_pequeno)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }
    boi_1_ataque = al_load_bitmap("examples/simpleChat/Resources/Tilesets/boi_1_ataque.png");
    if (!boi_1_ataque)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }
    boi_2_ataque = al_load_bitmap("examples/simpleChat/Resources/Tilesets/boi_2_ataque.png");
    if (!boi_2_ataque)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }
    boi_3_ataque = al_load_bitmap("examples/simpleChat/Resources/Tilesets/boi_3_ataque.png");
    if (!boi_3_ataque)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }
    boi_4_ataque = al_load_bitmap("examples/simpleChat/Resources/Tilesets/boi_4_ataque.png");
    if (!boi_4_ataque)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }
    boi_1_pequenoUp = al_load_bitmap("examples/simpleChat/Resources/Tilesets/boi_1_pequenoUp.png");
    if (!boi_1_pequeno)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }
    boi_2_pequenoUp = al_load_bitmap("examples/simpleChat/Resources/Tilesets/boi_2_pequenoUp.png");
    if (!boi_2_pequeno)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }
    boi_3_pequenoUp = al_load_bitmap("examples/simpleChat/Resources/Tilesets/boi_3_pequenoUp.png");
    if (!boi_3_pequeno)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }
    boi_4_pequenoUp = al_load_bitmap("examples/simpleChat/Resources/Tilesets/boi_4_pequenoUp.png");
    if (!boi_4_pequeno)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }
    boi_1_ataqueUp = al_load_bitmap("examples/simpleChat/Resources/Tilesets/boi_1_ataqueUp.png");
    if (!boi_1_ataque)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }
    boi_2_ataqueUp = al_load_bitmap("examples/simpleChat/Resources/Tilesets/boi_2_ataqueUp.png");
    if (!boi_2_ataque)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }
    boi_3_ataqueUp = al_load_bitmap("examples/simpleChat/Resources/Tilesets/boi_3_ataqueUp.png");
    if (!boi_3_ataque)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }
    boi_4_ataqueUp = al_load_bitmap("examples/simpleChat/Resources/Tilesets/boi_4_ataqueUp.png");
    if (!boi_4_ataque)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }
    derrota = al_load_bitmap("examples/simpleChat/Resources/Tilesets/teladerrota.png");
    if (!derrota)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }
    vitoria = al_load_bitmap("examples/simpleChat/Resources/Tilesets/telavitoria.png");
    if (!vitoria)
    {
        //fprintf(stderr, "Falha carregando background\n");
        return false;
    }
    return true;
}