#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#define STRING_SIZE 100       // max size for some strings
#define TABLE_SIZE 650        // main game space size
#define LEFT_BAR_SIZE 150     // left white bar size
#define WINDOW_POSX 200       // initial position of the window: x
#define WINDOW_POSY 200       // initial position of the window: y
#define SQUARE_SEPARATOR 8    // square separator in px
#define BOARD_SIZE_PER 0.7f   // board size in % wrt to table size
#define MAX_BOARD_POS 11      // maximum size of the board
#define MAX_LEVELS 23
#define MARGIN 5
#define MAX_UNDO 25
#define GAME_TIME 2
#define GAME_MAX_CELL 1
#define GAME_SCORE 0
#define GAME_WIN 1
#define GAME_LOSE 2
#define GAME_RUNNING 0
#define UNDO_STORE 0
#define UNDO 1
#define UNDO_INIT 2

/**
*Jogo 2048, código fonte. Elaborado por Guilherme Guerreiro aluno nº 87010, 1º ano 2º semestre do curso MEEC
*/


// declaration of the functions related to graphical issues
void InitEverything(int , int , TTF_Font **, SDL_Surface **, SDL_Window ** , SDL_Renderer ** );
void InitSDL();
void InitFont();
SDL_Window* CreateWindow(int , int );
SDL_Renderer* CreateRenderer(int , int , SDL_Window *);
int RenderText(int, int, const char *, TTF_Font *, SDL_Color *, SDL_Renderer *);
int RenderLogo(int, int, SDL_Surface *, SDL_Renderer *);
void RenderTable(int, int *, int *, TTF_Font *, SDL_Surface **, SDL_Renderer *);
void RenderBoard(int [][MAX_BOARD_POS], SDL_Surface **, int, int, int, SDL_Renderer *);
void RenderStats( SDL_Renderer *, int , long , long);
void LoadValues(SDL_Surface **);
void UnLoadValues(SDL_Surface **);
//declaração das funções responsavei pelo gameplay
void ParamUser (char [15], int*, int*);  //Recebe parametros de jogo
int getValue (int, int, char*);     //Validação dos parametros introduzidos pelo user
void up (int, int[][MAX_BOARD_POS], int*, long*, int, int*);    //Função movimento up, soma peças, verifica situação win
void down (int, int[][MAX_BOARD_POS], int*, long*, int, int*);  //Função movimento down, soma peças, verifica situação win
void left(int, int[][MAX_BOARD_POS], int*, long*, int, int*);   //Função movimento left, soma peças, verifica situação win
void right(int, int[][MAX_BOARD_POS], int*, long*, int, int*);  //Função movimento right, soma peças, verifica situação win
void randomgen(int, int[][MAX_BOARD_POS], int, int*);      //Função para gerar peças aleatorias em locais aleatorios
void newgame(int, int[][MAX_BOARD_POS]);         //Função new game, responsável por inicializar um novo jogo
void sumscore(int, long*);    //Função de soma da pontuação
long gametime(int, int);    //Função que conta o tempo de jogo
void winlosemsg(SDL_Renderer *, int);   //Função que faz render das mensagens win/lose
void losecheck(int, int[][MAX_BOARD_POS], int*);    //Função que verifica situação lose
void managegamedata(int, int[][MAX_BOARD_POS], long*, int, int); //Função responsável por guardar informação dos movimentos e fazer undo
void highercell(int, int[][MAX_BOARD_POS], long*);
void writestats(char*, long[3]);


// definition of some strings: they cannot be changed when the program is executed !
const char myName[] = "Guilherme Guerreiro";
const char myNumber[] = "IST187010";

/**
 * main function: entry point of the program
 * only to invoke other functions !
 */
int main( int argc, char* args[] )
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *serif = NULL;
    SDL_Surface *array_of_numbers[MAX_LEVELS], *imgs[2];
    SDL_Event event;
    int delay = 300;
    int quit = 0;
    int width = (TABLE_SIZE + LEFT_BAR_SIZE);
    int height = TABLE_SIZE;
    int square_size_px, board_size_px, board_pos = 0;
    int board[MAX_BOARD_POS][MAX_BOARD_POS] = {{0}};
    int level=0, newgameflag=0, moveflag=0;
    int endgame=0; // 0 -> jogo em curso; 1 -> win; 2->lose

    char player[15];
    long stats[3]={0}; //0->pontuação, 1-> valor máximo atingido numa célula, 2-> duração do jogo

    //initialize parameters(user input)
    ParamUser(player, &board_pos, &level);

//    board[5][0]=1;
//    board[4][0]=1;
//    board[3][0]=0;
//    board[2][0]=0;
//    board[1][0]=1;
//    board[0][0]=1;

    // initialize graphics
    InitEverything(width, height, &serif, imgs, &window, &renderer);
    // loads numbers as images
    LoadValues(array_of_numbers);


    while( quit == 0 )
    {
        losecheck(board_pos, board, &endgame); //Verifica se o jogador ainda tem jogadas disponíveis
        stats[GAME_TIME] = gametime(newgameflag, endgame); //time = tempo decorrido desde o inicio do jogo

        // while there's events to handle
        while( SDL_PollEvent( &event ) )
        {
            if( event.type == SDL_QUIT )
            {
                quit=1;

            }
            else if ( event.type == SDL_KEYDOWN )
                {
                switch ( event.key.keysym.sym )
                {
                    case SDLK_UP:
                        managegamedata(board_pos, board, &stats[GAME_SCORE], UNDO_STORE, endgame);
                        up(board_pos, board, &moveflag, &stats[GAME_SCORE], level, &endgame);
                        randomgen(board_pos, board, moveflag, &endgame);
                        break;
                    case SDLK_DOWN:
                        managegamedata(board_pos, board, &stats[GAME_SCORE], UNDO_STORE, endgame);
                        down(board_pos, board, &moveflag, &stats[GAME_SCORE], level, &endgame);
                        randomgen(board_pos, board, moveflag, &endgame);
                        break;
                    case SDLK_LEFT:
                        managegamedata(board_pos, board, &stats[GAME_SCORE], UNDO_STORE, endgame);
                        left(board_pos, board, &moveflag, &stats[GAME_SCORE], level, &endgame);
                        randomgen(board_pos, board, moveflag, &endgame);
                        break;
                    case SDLK_RIGHT:
                        managegamedata(board_pos, board, &stats[GAME_SCORE], UNDO_STORE, endgame);
                        right(board_pos, board, &moveflag, &stats[GAME_SCORE], level, &endgame);
                        randomgen(board_pos, board, moveflag, &endgame);
                        break;
                    case SDLK_q:
                        quit = 1;
                        break;
                    case SDLK_n:
                        newgame(board_pos, board);
                        newgameflag=1; //Sinaliza inicio de um novo jogo à contagem do tempo(reset do tempo)
                        gametime(0, 0);//começa a contar o tempo outra vez
                        endgame=0;  //reinicia a flag de fim de jogo a 0
                        stats[GAME_SCORE]=0;    //reinicia a pontuação total
                        stats[GAME_MAX_CELL]=0;  //reinicia o valor maximo atingido numa peça
                        managegamedata(board_pos, board, &stats[GAME_SCORE], UNDO_INIT, endgame); //Inicialização da memória Undo
                        break;
                    case SDLK_u:
                        managegamedata(board_pos, board, &stats[GAME_SCORE], UNDO, endgame); //4ºparam=1, carrega dados da matriz de armazenamento para a matriz de jogo
                        break;
                    default:
                        break;
                }
            }
        }

        // render game table
        RenderTable( board_pos, &board_size_px, &square_size_px, serif, imgs, renderer);
        // render board
        RenderBoard(board, array_of_numbers, board_pos, board_size_px, square_size_px, renderer);
        //render info rectangles
        RenderStats(renderer, level, stats[GAME_SCORE], stats[GAME_TIME]);
        //Render win/lose message
        winlosemsg(renderer, endgame);
        // render in the screen all changes above
        SDL_RenderPresent(renderer);
        // add a delay
        SDL_Delay( delay );
    }

    //Determina o valor da maior celula atingida
    highercell(board_pos, board, &stats[GAME_MAX_CELL]);
    //Escreve o ficheiro de estatistica
    writestats(player, stats);

    // free memory allocated for images and textures and closes everything including fonts
    UnLoadValues(array_of_numbers);
    TTF_CloseFont(serif);
    SDL_FreeSurface(imgs[0]);
    SDL_FreeSurface(imgs[1]);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
/** Função de escrita do ficheiro de estatistica
* \param _player nome do jogador
* \param _score pontuação maxima
* \param _maxcellvalue valor maximo atingido numa celula
* \param _time duração do jogo
*/
void writestats(char *_player, long _stats[3])
{
    FILE *fp;

    fp = fopen("stats.txt","a+"); //cria/abre o ficheiro stat.txt
    if (ferror(fp))
    {
        printf("Unable to open file"); //basic check
    }
    else
    {
        fprintf(fp,"Nome do jogador: %s; Pontuação Máxima: %ld; Peça com maior valor: %ld; Tempo de jogo: %lds\n", _player, _stats[GAME_SCORE], _stats[GAME_MAX_CELL], _stats[GAME_TIME]);
    }

    fclose(fp);
}
/** Função para determinar a celula com o valor mais alto
* \param _board_pos tamanho do tabuleiro
* \param board tabuleiro de jogo
* \param _maxvalue valor maximo encontrado
*/
void highercell(int _board_pos, int _board[][MAX_BOARD_POS], long *_maxvalue)
{
    int c=0, l=0, exp=0;

    for(c=0; c<_board_pos; c++)
        {
            for(l=0; l<_board_pos; l++)     //percorre o tabuleiro todo
            {
                if(_board[c][l]>exp)        //se alguma entrada for maior que a ultima, é essa a maior
                    exp=_board[c][l];
            }
        }

        *_maxvalue=pow(2,exp); //converte expoente para potencia de 2
}

/**Função para tratar do UNDO (store data and undo data)
*\param _board_pos, tamanho do tabuleiro
*\param _board, matriz de jogo
*\param _score, pontuação
*\param undo, flag (undo==0 guarda dados, undo==1 faz undo, undo==2 condiçao de newgame)
*/
void managegamedata(int _board_pos, int _board[][MAX_BOARD_POS], long* _score, int _undo, int _endgame)
{
    static int storeboard [MAX_UNDO][MAX_BOARD_POS][MAX_BOARD_POS];
    static int storescore [MAX_UNDO];
    static int totalstore =0;
    int i=0, c=0, l=0, counter=0, initpoint=0;

    if(_undo==UNDO_INIT) //Condição de newgame (2)
        totalstore=0;

    if(_undo==UNDO_STORE) //Condição de guardar o jogo (0)
    {
        for(c=0; c<_board_pos; c++)         //Verifica se a board no ecra é igual à ultima board guardada
        {
            for(l=0; l<_board_pos; l++)
            {
                if(storeboard[0][c][l] == _board[c][l]) //nº de entradas iguais
                counter++;
            }
        }
        if(counter == pow(_board_pos,2)) //se o numero de entradas iguais for == ao numero de divisoes da board, então as matrizes sao iguais
            return;

        if(totalstore<MAX_UNDO) //Basic check
            totalstore++;

        for(i=MAX_UNDO-1; i>0; i--)
        {
            storescore[i]=storescore[i-1];   //trata da movimentação da pontuação dentro do registo

            for(c=0; c<_board_pos; c++)
            {
                for(l=0; l<_board_pos; l++)
                {
                    storeboard[i][c][l]=storeboard[i-1][c][l];  //trata da movimentação das matrizes dentro do registo
                }
            }
        }
        storescore[0]=*_score;      //guarda o ultimo valor de score

        for(c=0; c<_board_pos; c++)
        {
            for(l=0; l<_board_pos; l++)
            {
                storeboard[0][c][l]=_board[c][l];  //guarda o ultimo valor da board
            }
        }
    }
    else if(totalstore>0 && _undo==UNDO && _endgame==GAME_RUNNING)  //Se há matrizes guardadas e foi dada ordem de undo(1)
    {
        for(c=0; c<_board_pos; c++)
        {
            for(l=0; l<_board_pos; l++)
            {
                if(storeboard[0][c][l] == _board[c][l])
                counter++;                   //Verifica se a board no ecra é igual à ultima board guardada
            }
        }
        if(counter == pow(_board_pos,2))    //Se for igual, o ponto de partida para o undo é uma "casa" à frente nos registos
            initpoint=1;

        totalstore=totalstore-1-initpoint;

        *_score=storescore[initpoint]; //Reposição do score

        for(c=0; c<_board_pos; c++)
        {
            for(l=0; l<_board_pos; l++)
            {
                _board[c][l]=storeboard[initpoint][c][l]; //Reposição da matriz
            }
        }
        for(i=0; i<MAX_UNDO-1-initpoint; i++)
        {
            storescore[i]=storescore[i+1+initpoint];    //Movimenta os dados no registo uma casa para a frente

            for(c=0; c<_board_pos; c++)
            {
                for(l=0; l<_board_pos; l++)
                {
                    storeboard[i][c][l]=storeboard[i+1+initpoint][c][l];
                }
            }
        }
    }

}

/**Função para verificar a jogabilidade(situação de lose)
*\param board_pos, tamanho do tabuleiro
*\param board, tabuleiro de jogo
*\param _endgame, flag final de jogo (_endgame==2 game lost)
*/
void losecheck(int board_pos,int board[][MAX_BOARD_POS],int* _endgame)
{
    int c=0, l=0;
    int playable=0;

    for(c=0; c<board_pos-1; c++)
    {                                   //Corre as posições do tabuleiro
        for(l=0; l<board_pos-1; l++)
        {
            if(board[c][l]==board[c][l+1] || board[c][l]==board[c+1][l] || board[c][l+1]==0 || board[c+1][l]==0 || board[c][l]==0)
                playable=1; //if statement, se houverem duas peças consecutivas iguais ou uma peça igual a 0 ainda há jogadas possíveis

        }
    }

    if(playable==0) //lose condition
        *_endgame=GAME_LOSE;
}


/**Função para contar o tempo de jogo
*\param init, inicializador do contador de tempo(start when 1 (newgame))
*\param startstop, interrompe a contagem (stop when 1 (win/lose situation)
*/
long gametime(int init, int startstop)
{
    static long inittime=0;
    static long actime=0;
    long duration=0;

    if(startstop==0) //enquanto houver jogadas actime = tempo da máquina (aumenta cada segundo)
        actime = time(NULL);

    if(init==0)
        inittime=actime; //inicialização, tempo atual = tempo inicial, ou seja duration =0

    duration = actime - inittime; //actime incrementa com a máquina e init time é sp igual até à proxima inicialização
    return(duration);
}

/**Função soma da pontuação
*\param exp, expoente retirado da posição da board em que ocorreu uma fusão
*\param pontuacao, pontuação total do jogo
*/
void sumscore(int exp, long *pontuacao)
{
    *pontuacao = *pontuacao + pow(2,exp); //converte os expoentes usados na matriz, para a respetiva potencia de 2

}

/**Função newgame
* Inicializa todas as posições a 0 e chama a função randomgen()
* \param board_pos, tamanho do tabuleiro
* \param board, tabuleiro de jogo
*/
void newgame(int board_pos, int board[][MAX_BOARD_POS])
{
    int c=0, l=0;

    for(c=0; c<board_pos; c++)      //coloca todas as posições da board a 0
        for(l=0; l<board_pos; l++)
        {
            board[c][l]=0;
        }

    randomgen(board_pos, board, 1, NULL); //gera os dois numeros iniciais
    randomgen(board_pos, board, 1, NULL);
}

/**Função para gerar números inteiros aleatórios entre 1 e 2
* \param board_pos, tamanho do tabuleiro
* \param board, tabuleiro de jogo
* \param _moveflag, FLAG para saber se há movimento de peças no tabuleiro (_moveflag==1 houve movimento, else não houve)
* \param _endgame, FLAG de fim de jogo(_endgame>0 fim de jogo, _endgame==0 still playable
*/
void randomgen(int board_pos, int board[][MAX_BOARD_POS], int _moveflag, int* _endgame)
{
    int c=0, l=0, i=0;
    srand(time(NULL));

   do
   {
        c = rand() % (board_pos);  //gera posição aleatoria
        l = rand() % (board_pos);
        i = rand() % 2;   //gera nums entre 0 e 1
        i=i+1;        //converte para expoente utilizavel

   }while(board[c][l]!=0 && _moveflag==1);//apenas gera se houver movimento e enquanto não encontrar uma posição vazia

   if(_moveflag==1) //se houver movimento coloca uma peça nova
        board[c][l]=i;
}

/**Função para mover para cima, somar e testar se ganhou
* \param board_pos, tamanho do tabuleiro
* \param board, tabuleiro de jogo
* \param _moveflag, FLAG informa se há movimento de peças (==1 há movimento)
* \param score, pontuação total do jogo
* \param _level, nivel de dificuldade introduzido pelo jogador
* \param _endgame, FLAG de fim de jogo(==0 still playable, ==1 situação de WIN)
*/
void up(int board_pos, int board[][MAX_BOARD_POS], int *_moveflag, long *score, int _level, int *_endgame)
{
    int c=0, l=0, i=0, imin=0;

    *_moveflag=0;

    if(*_endgame>0)  //impede movimentos apos fim de jogo
        return;

    for(c=0; c<board_pos; c++) //corre as colunas da matriz entre 0 e board_pos-1
    {
        imin=0;
        for(l=1; l<board_pos; l++) //corre as linhas da matriz entre 0 e board_pos-1
        {
            i=l;
                    /** Bloco de codigo associado ao movimento*/
            while(board[c][i-1]==0 && i>0 && board[c][i]!=0)  //Condição que verifica se há um espaço vazio acima da peça
            {
                board[c][i-1] = board[c][i];    //movimenta a peça um espaço para cima
                board[c][i]=0;

                i--;    //repete o ciclo while uma linha acima até ao limite do tabuleiro ou até encontrar uma posição com uma peça

                *_moveflag=1; //flag que indica que houve movimento
            }

                    /**Bloco de codigo associado à soma*/

            if(board[c][i-1] == board[c][i] && board[c][i] != 0 && imin<i) //Condição que verifica se duas peças verticalmente consecutivas são iguais
            {
                imin=i;
                board[c][i-1]++;  //incrementa a peça acima
                board[c][i]=0;

                    /**Bloco de codigo associado à verificação da condição WIN*/
                if(board[c][i-1]!=_level)
                {                   //Se a peça resultante da soma nao for igual à dificuldade escolhida
                    *_moveflag=1;   //há movimento de peças
                }
                else
                {                  //Se for igual à dificuldade
                    *_moveflag=0;
                    *_endgame=GAME_WIN;   //_endgame=1 equivale à condição WIN
                }

                sumscore(board[c][i-1], score); //envia o expoente gerado a partir da fusão para a função de soma
            }                                   //que irá atualizar a pontuação
        }
    }
}

/**Função para mover para baixo, somar e testar se ganhou
* \param board_pos, tamanho do tabuleiro
* \param board, tabuleiro de jogo
* \param _moveflag, FLAG informa se há movimento de peças (==1 há movimento)
* \param score, pontuação total do jogo
* \param _level, nivel de dificuldade introduzido pelo jogador
* \param _endgame, FLAG de fim de jogo(==0 still playable, ==1 situação de WIN)
*/
void down(int board_pos, int board[][MAX_BOARD_POS], int *_moveflag, long *score, int _level, int *_endgame)
{
    int c=0, l=0, i=0, imaxim=0;
                            //Ver comentários da função up
    *_moveflag=0;
    if(*_endgame>0)
        return;
    for(c=0; c<board_pos; c++)
    {
        imaxim=board_pos-1;
        for(l=board_pos-2; l>=0; l--)
        {
            i=l;

                while (board[c][i+1]==0 && i<board_pos-1 && board[c][i]!=0)
                {
                board[c][i+1] = board[c][i];
                board[c][i]=0;

                i++;
                *_moveflag=1;
                }

                if(board[c][i+1] == board[c][i] && board[c][i] != 0 && imaxim>i)
                {
                    imaxim=i;
                    board[c][i+1]++;
                    board[c][i]=0;
                    if(board[c][i+1]!=_level)
                {
                    *_moveflag=1;
                }
                else
                {
                    *_moveflag=0;
                    *_endgame=GAME_WIN;
                }
                    sumscore(board[c][i+1], score);
                }
        }
    }

}

/**Função mover esquerda, somar e testar se ganhou
* \param board_pos, tamanho do tabuleiro
* \param board, tabuleiro de jogo
* \param _moveflag, FLAG informa se há movimento de peças (==1 há movimento)
* \param score, pontuação total do jogo
* \param _level, nivel de dificuldade introduzido pelo jogador
* \param _endgame, FLAG de fim de jogo(==0 still playable, ==1 situação de WIN)
*/
void left(int board_pos, int board[][MAX_BOARD_POS], int *_moveflag, long *score, int _level, int *_endgame)
{
    int c=0, l=0, i=0, imin=0;

    *_moveflag=0;
                   //Ver comentários da função up
    if(*_endgame>0)
        return;

    for(l=0; l<board_pos; l++)
    {
        imin=0;
        for(c=1; c<board_pos; c++)
        {
            i=c;
            while(board[i-1][l]==0 && i>0 && board[i][l]!=0)
            {
                board[i-1][l] = board[i][l];
                board[i][l]=0;

                i--;
                *_moveflag=1;
            }

            if(board[i-1][l] == board[i][l] && board[i][l] != 0 && imin<i)
            {
                imin=i;
                board[i-1][l]++;
                board[i][l]=0;
                if(board[i-1][l]!=_level)
                {
                    *_moveflag=1;
                }
                else
                {
                    *_moveflag=0;
                    *_endgame=GAME_WIN;
                }
                sumscore(board[i-1][l], score);
            }
        }
    }
}

/**Função mover direita, somar e testar se ganhou
* \param board_pos, tamanho do tabuleiro
* \param board, tabuleiro de jogo
* \param _moveflag, FLAG informa se há movimento de peças (==1 há movimento)
* \param score, pontuação total do jogo
* \param _level, nivel de dificuldade introduzido pelo jogador
* \param _endgame, FLAG de fim de jogo(==0 still playable, ==1 situação de WIN)
*/
void right(int board_pos, int board[][MAX_BOARD_POS], int *_moveflag, long *score, int _level, int *_endgame)
{
    int c=0, l=0, i=0, imaxim=0;
    *_moveflag=0;
                            //Ver comentários da função up
    if(*_endgame>0)
        return;

    for(l=0; l<board_pos; l++)
    {
        imaxim=board_pos-1;
        for(c=board_pos-2; c>=0; c--)
        {
            i=c;
            while(board[i+1][l]==0 && i<board_pos-1 && board[i][l]!=0)
            {
                board[i+1][l]=board[i][l];
                board[i][l]=0;
                i++;
                *_moveflag=1;
            }

            if(board[i+1][l] == board[i][l] && board[i][l] != 0 && imaxim>i)
            {
                imaxim=i;
                board[i+1][l]++;
                board[i][l]=0;
                if(board[i+1][l]!=_level)
                {
                    *_moveflag=1;
                }
                else
                {
                    *_moveflag=0;
                    *_endgame=GAME_WIN;
                }
                sumscore(board[i+1][l], score);
            }
        }
    }
}

/**
*Função para receber os parametros do user
* \param _player, nome do jogador
* \param _boardSize, tamanho do tabuleiro de jogo(3-11)
* \param _level, dificuldade do jogo (1-20)
*/
void ParamUser(char* _player, int* _boardSize, int* _level)
{
    int len=0, i=0, error=0;
    int level=0;

    do  //Username
    {
        error=0;
        printf("Username:\n");
        fgets(_player, STRING_SIZE, stdin);

        len = strlen(_player);

        if(_player[len-1]=='\n') _player[len-1]='\0'; //Remove \n da string
        for(i=0; i<len-1; i++)
        {
            if(isalnum(_player[i])==0 && isspace(_player[i])==0)  //Verifica se todos os caracteres são alfanumericos
                error=1;
        }


        if (len>=2 && len<=9 && error==0) break;
      	else printf("Invalid Username\n");
    }while(1);

    //Tamanho do tabuleiro
    *_boardSize = getValue(3, 11, "Enter board size (a value between 3 and 11)");


    //Dificuldade

     level = getValue(1, 20, "Enter desired difficulty (a value between 1 and 20)");

        *_level = level + 3;  //Dificuldade é uma potência de 2 entre 2⁴ e 2²³

    return;
}

/**
*Função para validar valores introduzidos
*\param limMin, LimMax  limites dos parametros introduzidos
*\param msg Mensagem para o user
 */
int getValue(int limMin, int limMax, char *msg)
{
  int dec=0;
  char uInput[STRING_SIZE];

    do
    {
        printf("%s\n", msg);
        scanf("%s", uInput);

        dec = atoi(uInput);

        if (dec >= limMin && dec <= limMax) break;
        else printf("Not a valid value\n");
    }
  while(1);

    return(dec);
}


/**
 * LoadCards: Loads all images with the values
 * \param _array_of_numbers vector with all loaded numbers
 */
void LoadValues(SDL_Surface **_array_of_numbers)
{
    int i;
    char filename[STRING_SIZE];

    // loads all images to an array
    for ( i = 0 ; i < MAX_LEVELS; i++ )
    {
        // create the filename !
        sprintf(filename, ".//numbers//value_%02d.jpg", i+1);
        // loads the image !
        _array_of_numbers[i] = IMG_Load(filename);
        // check for errors: deleted files ?
        if (_array_of_numbers[i] == NULL)
        {
            printf("Unable to load image: %s\n", SDL_GetError());
            exit(EXIT_FAILURE);
        }
    }
}


/**
 * UnLoadCards: unloads all values images of the memory
 * \param _array_of_numbers vector with all loaded image numbers
 */
void UnLoadValues(SDL_Surface **_array_of_numbers)
{
    // unload all cards of the memory: +1 for the card back
    for ( int i = 0 ; i < MAX_LEVELS; i++ )
    {
        SDL_FreeSurface(_array_of_numbers[i]);
    }
}

/**
 * RenderBoard: renders the board
 * \param _board multidimensional array with the board numbers
 * \param _array_of_numbers vector with all loaded images
 * \param _board_pos number of positions in the board
 * \param _board_size_px size of the board in pixels
 * \param _square_size_px size of each square
 * \param _renderer renderer to handle all rendering in a window
 */
void RenderBoard(int _board[][MAX_BOARD_POS], SDL_Surface **_array_of_numbers, int _board_pos,
        int _board_size_px, int _square_size_px, SDL_Renderer *_renderer )
{
    int x_corner, y_corner;
    SDL_Rect boardPos;
    SDL_Texture *number_text;

    // corner of the board
    x_corner = (TABLE_SIZE - _board_size_px) >> 1;
    y_corner = (TABLE_SIZE - _board_size_px - 15);

    // renders the squares where the numbers will appear
    for ( int i = 0; i < _board_pos; i++ )
    {
        for ( int j = 0; j < _board_pos; j++ )
        {
            // basic check
            if ( _board[i][j] > 23 || _board[i][j] < 0 )
            {
                printf("Invalid board value: RenderBoard\n");
                exit(EXIT_FAILURE);
            }
            // only draws a number if it corresponds to a non-empty space
            if ( _board[i][j] != 0 )
            {
                // define the size and copy the image to display
                boardPos.x = x_corner + (i+1)*SQUARE_SEPARATOR + i*_square_size_px;
                boardPos.y = y_corner + (j+1)*SQUARE_SEPARATOR + j*_square_size_px;
                boardPos.w = _square_size_px;
                boardPos.h = _square_size_px;
                number_text = SDL_CreateTextureFromSurface( _renderer, _array_of_numbers[ _board[i][j] - 1] );
                SDL_RenderCopy( _renderer, number_text, NULL, &boardPos );
                // destroy texture
                SDL_DestroyTexture(number_text);
            }
        }
    }
}

/*
 * Shows some information about the game:
 * - Level to achieve (difficulty)
 * - Score of the game
 * - Time spent in seconds
 * \param _renderer renderer to handle all rendering in a window
 * \param _level current level
 * \param _score actual score
 * \param _time time to display in seconds
 */
void RenderStats( SDL_Renderer *_renderer, int _level, long _score, long _time)
{
    SDL_Rect esq;
    SDL_Rect mid;
    SDL_Rect direita;
    SDL_Color rect_cor = {209, 201, 196, 0};
    SDL_Color text_cor = {0, 0, 0};
    double level=0;
    char slevel[10], stime[10], sscore[10];
    TTF_Font *font35 = TTF_OpenFont("OpenSans.ttf", 35);
    TTF_Font *font24 = TTF_OpenFont("OpenSans.ttf", 24);

    //Cor dos retangulos
    SDL_SetRenderDrawColor(_renderer, rect_cor.r, rect_cor.g, rect_cor.b, rect_cor.a);

    //desenha retangulo da esquerda
    esq.x=97;
    esq.y=50;
    esq.w=148;
    esq.h=75;

    SDL_RenderDrawRect(_renderer, &esq);
    SDL_RenderFillRect(_renderer, &esq);

    //desenha retangulo do meio
    mid.x=250;
    mid.y=50;
    mid.w=150;
    mid.h=75;

    SDL_RenderDrawRect(_renderer, &mid);
    SDL_RenderFillRect(_renderer, &mid);

    //desenha retangulo da direita
    direita.x=405;
    direita.y=50;
    direita.w=145;
    direita.h=75;

    SDL_RenderDrawRect(_renderer, &direita);
    SDL_RenderFillRect(_renderer, &direita);

    //Texto nivel de dificuldade

    level = pow(2,_level);

    sprintf(slevel,"%.0f", level);

    RenderText(100, 50, "Level:", font24, &text_cor, _renderer);
    RenderText(100, 75, slevel, font35, &text_cor, _renderer);

    //Tempo de jogo

    sprintf(stime, "%ld", _time);

    RenderText(255, 50, "Time:", font24, &text_cor, _renderer);
    RenderText(255, 75, stime, font35, &text_cor, _renderer);

    //Pontuação

    sprintf(sscore, "%ld", _score);

    RenderText(410, 50, "Points:", font24, &text_cor, _renderer);
    RenderText(410, 75, sscore, font35, &text_cor, _renderer);

    TTF_CloseFont(font24);
    TTF_CloseFont(font35);

}

/**Faz render das mensagens de Win/Lose
* \param _endgame FLAG sinaliza fim de jogo e o seu tipo (==1 situação win, ==2 situação lose)*/
void winlosemsg(SDL_Renderer *_renderer, int _endgame)
{
   SDL_Rect cover;
   SDL_Color win={33, 214, 17};
   SDL_Color lose={247, 17, 17};
   SDL_Color wintxt={255, 255, 255};
   TTF_Font *font50 = TTF_OpenFont("OpenSans.ttf", 100);
   cover.x=99;
   cover.y=183;
   cover.w=452;
   cover.h=480;




    if(_endgame==1) //win situation
    {
        SDL_SetRenderDrawColor(_renderer, win.r, win.g, win.b, win.a);
        SDL_RenderDrawRect(_renderer, &cover);
        SDL_RenderFillRect(_renderer, &cover);
        RenderText(115, 350, "You win!", font50, &wintxt, _renderer);
    }
    else if(_endgame==2) //lose situation
    {
        SDL_SetRenderDrawColor(_renderer, lose.r, lose.g, lose.b, lose.a);
        SDL_RenderDrawRect(_renderer, &cover);
        SDL_RenderFillRect(_renderer, &cover);
        RenderText(115, 350, "You lose!", font50, &wintxt, _renderer);
    }

TTF_CloseFont(font50);
}


/*
 * RenderTable: Draws the table where the game will be played, namely:
 * -  some texture for the background
 * -  the right part with the IST logo and the student name and number
 * -  the grid for game board with squares and seperator lines
 * \param _board_pos number of squares in the board
 * \param _font font used to render the text
 * \param _img surfaces with the table background and IST logo (already loaded)
 * \param _renderer renderer to handle all rendering in a window
 */
void RenderTable( int _board_pos, int *_board_size_px, int *_square_size_px, TTF_Font *_font, SDL_Surface *_img[], SDL_Renderer* _renderer)
{
    SDL_Color black = { 0, 0, 0 }; // black
    SDL_Color light = { 205, 193, 181 };
    SDL_Color dark = { 120, 110, 102 };
    SDL_Texture *table_texture;
    SDL_Rect tableSrc, tableDest, board, board_square;
    int height, board_size_px, square_size_px;

    // set color of renderer to some color
    SDL_SetRenderDrawColor( _renderer, 255, 255, 255, 255 );

    // clear the window
    SDL_RenderClear( _renderer );

    tableDest.x = tableSrc.x = 0;
    tableDest.y = tableSrc.y = 0;
    tableSrc.w = _img[0]->w;
    tableSrc.h = _img[0]->h;
    tableDest.w = TABLE_SIZE;
    tableDest.h = TABLE_SIZE;

    // draws the table texture
    table_texture = SDL_CreateTextureFromSurface(_renderer, _img[0]);
    SDL_RenderCopy(_renderer, table_texture, &tableSrc, &tableDest);

    // render the IST Logo
    height = RenderLogo(TABLE_SIZE, 0, _img[1], _renderer);

    // render the student name
    height += RenderText(TABLE_SIZE+3*MARGIN, height, myName, _font, &black, _renderer);

    // this renders the student number
    RenderText(TABLE_SIZE+3*MARGIN, height, myNumber, _font, &black, _renderer);

    // compute and adjust the size of the table and squares
    board_size_px = (int)(BOARD_SIZE_PER*TABLE_SIZE);
    square_size_px = (board_size_px - (_board_pos+1)*SQUARE_SEPARATOR) / _board_pos;
    board_size_px -= board_size_px % (_board_pos*(square_size_px+SQUARE_SEPARATOR));
    board_size_px += SQUARE_SEPARATOR;


    // renders the entire board background
    SDL_SetRenderDrawColor(_renderer, dark.r, dark.g, dark.b, dark.a );


    board.x = (TABLE_SIZE - board_size_px) >> 1;
    board.y = (TABLE_SIZE - board_size_px - 15);
    board.w = board_size_px;
    board.h = board_size_px;
    SDL_RenderFillRect(_renderer, &board);

    // renders the squares where the numbers will appear
    SDL_SetRenderDrawColor(_renderer, light.r, light.g, light.b, light.a );

    // iterate over all squares
    for ( int i = 0; i < _board_pos; i++ )
    {
        for ( int j = 0; j < _board_pos; j++ )
        {
            board_square.x = board.x + (i+1)*SQUARE_SEPARATOR + i*square_size_px;
            board_square.y = board.y + (j+1)*SQUARE_SEPARATOR + j*square_size_px;
            board_square.w = square_size_px;
            board_square.h = square_size_px;
            SDL_RenderFillRect(_renderer, &board_square);
        }
    }

    // destroy everything
    SDL_DestroyTexture(table_texture);
    // store some variables for later use
    *_board_size_px = board_size_px;
    *_square_size_px = square_size_px;
}

/**
 * RenderLogo function: Renders the IST logo on the app window
 * \param x X coordinate of the Logo
 * \param y Y coordinate of the Logo
 * \param _logoIST surface with the IST logo image to render
 * \param _renderer renderer to handle all rendering in a window
 */
int RenderLogo(int x, int y, SDL_Surface *_logoIST, SDL_Renderer* _renderer)
{
    SDL_Texture *text_IST;
    SDL_Rect boardPos;

    // space occupied by the logo
    boardPos.x = x;
    boardPos.y = y;
    boardPos.w = _logoIST->w;
    boardPos.h = _logoIST->h;

    // render it
    text_IST = SDL_CreateTextureFromSurface(_renderer, _logoIST);
    SDL_RenderCopy(_renderer, text_IST, NULL, &boardPos);

    // destroy associated texture !
    SDL_DestroyTexture(text_IST);
    return _logoIST->h;
}

/**
 * RenderText function: Renders some text on a position inside the app window
 * \param x X coordinate of the text
 * \param y Y coordinate of the text
 * \param text string with the text to be written
 * \param _font TTF font used to render the text
 * \param _color color of the text
 * \param _renderer renderer to handle all rendering in a window
 */
int RenderText(int x, int y, const char *text, TTF_Font *_font, SDL_Color *_color, SDL_Renderer* _renderer)
{
    SDL_Surface *text_surface;
    SDL_Texture *text_texture;
    SDL_Rect solidRect;

    solidRect.x = x;
    solidRect.y = y;



    // create a surface from the string text with a predefined font
    text_surface = TTF_RenderText_Blended(_font,text,*_color);
    if(!text_surface)
    {
        printf("TTF_RenderText_Blended: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    // create texture

    text_texture = SDL_CreateTextureFromSurface(_renderer, text_surface);
    // obtain size

    SDL_QueryTexture( text_texture, NULL, NULL, &solidRect.w, &solidRect.h );
    // render it !

    SDL_RenderCopy(_renderer, text_texture, NULL, &solidRect);
    // clear memory

    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);
    return solidRect.h;
}

/**
 * InitEverything: Initializes the SDL2 library and all graphical components: font, window, renderer
 * \param width width in px of the window
 * \param height height in px of the window
 * \param _font font that will be used to render the text
 * \param _img surface to be created with the table background and IST logo
 * \param _window represents the window of the application
 * \param _renderer renderer to handle all rendering in a window
 */
void InitEverything(int width, int height, TTF_Font **_font, SDL_Surface *_img[], SDL_Window** _window, SDL_Renderer** _renderer)
{
    InitSDL();
    InitFont();
    *_window = CreateWindow(width, height);
    *_renderer = CreateRenderer(width, height, *_window);

    // load the table texture
    _img[0] = IMG_Load("table_texture.png");
    if (_img[0] == NULL)
    {
        printf("Unable to load image: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // load IST logo
    _img[1] = SDL_LoadBMP("ist_logo.bmp");
    if (_img[1] == NULL)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    // this opens (loads) a font file and sets a size
    *_font = TTF_OpenFont("FreeSerif.ttf", 16);
    if(!*_font)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * InitSDL: Initializes the SDL2 graphic library
 */
void InitSDL()
{
    // init SDL library
    if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        printf(" Failed to initialize SDL : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * InitFont: Initializes the SDL2_ttf font library
 */
void InitFont()
{
    // Init font library
    if(TTF_Init()==-1)
    {
        printf("TTF_Init: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * CreateWindow: Creates a window for the application
 * \param width width in px of the window
 * \param height height in px of the window
 * \return pointer to the window created
 */
SDL_Window* CreateWindow(int width, int height)
{
    SDL_Window *window;
    // init window
    window = SDL_CreateWindow( "1024", WINDOW_POSX, WINDOW_POSY, width, height, 0 );
    // check for error !
    if ( window == NULL )
    {
        printf("Failed to create window : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    return window;
}

/**
 * CreateRenderer: Creates a renderer for the application
 * \param width width in px of the window
 * \param height height in px of the window
 * \param _window represents the window for which the renderer is associated
 * \return pointer to the renderer created
 */
SDL_Renderer* CreateRenderer(int width, int height, SDL_Window *_window)
{
    SDL_Renderer *renderer;
    // init renderer
    renderer = SDL_CreateRenderer( _window, -1, 0 );

    if ( renderer == NULL )
    {
        printf("Failed to create renderer : %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // set size of renderer to the same as window
    SDL_RenderSetLogicalSize( renderer, width, height );

    return renderer;
}
