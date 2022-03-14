//App struct genutzt fuer Input und Renderer
typedef struct{
    SDL_Renderer *renderer;
    SDL_Window *window;
    int jump;
    int right;
    int left;
}App;

typedef struct Entity Entity;

//Enitity struct beinhaltet eigentlich alles was sich auf dem Bildschirm befindet
struct Entity{
    int coll;
    int leben;
    int x;
    int y;
    int w;
    int h;
    float yV;
    int gelandet;
    int wasOnBlock;
    int AI;
    SDL_Texture *texture;
    Entity *before;
    Entity *next;
    int detected;
};
