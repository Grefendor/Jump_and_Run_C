#include <C:\Users\lasse\Desktop\ProzProjektFinal\imports.h>
#include <C:\Users\lasse\Desktop\ProzProjektFinal\strukturen.h>

//Von Jannis Gentz, Lukas Hertel, Lasse Grefe



//Globale definition von den Wichtigsten structs

App app;
Entity spieler;
Entity entHead;
Entity entTail;
Entity arr[MAP_HEIGHT * MAP_WIDTH];
//Entity block;
//Entity block2;
//Entity enemy;
void initializeSDL(){
    //Standard initialize Prozess wie im WIki beschrieben
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    Uint32 rendererFlags;
    rendererFlags = SDL_RENDERER_ACCELERATED;
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
        {
            printf("error initializing SDL: %s\n", SDL_GetError());
        }
    app.window = SDL_CreateWindow("Jumpi", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if(!app.window){
        printf("error creating window: %s\n", SDL_GetError());
        SDL_Quit();
    }
    app.renderer = SDL_CreateRenderer(app.window, -1, rendererFlags);
    if(!app.renderer){
        printf("error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(app.window);
        SDL_Quit();
    }
}
//Render funktionen
void prepareScene(void)
{
	SDL_RenderClear(app.renderer);
}

void presentScene(void)
{
	SDL_RenderPresent(app.renderer);
}

//Aufräumen nach dem schließen
void cleanup()
{
	SDL_DestroyRenderer(app.renderer);

	SDL_DestroyWindow(app.window);

	SDL_Quit();
}

//Blitting funktion
void blit(Entity thing){
    //Blitting und Skalierung des Spielers
    Entity *e;
    SDL_Rect dest;
    e = thing.next;
    dest.x = e->x;
    dest.y = e->y;
    SDL_QueryTexture(e->texture, NULL, NULL, &dest.w, &dest.h);
    dest.w /= 6;
    dest.h /= 6;
    e->w = dest.w;
    e->h = dest.h;
    SDL_RenderCopy(app.renderer, e->texture, NULL, &dest);
	for (e = thing.next->next ; e->next != NULL ; e = e->next){
        //Blitting von allem im Fenster
        if(e->x < WINDOW_WIDTH && e->x > 0){
            dest.x = e->x;
            dest.y = e->y;
            SDL_QueryTexture(e->texture, NULL, NULL, &dest.w, &dest.h);
            dest.w /= 4;
            dest.h /= 4;
            e->w = dest.w;
            e->h = dest.h;
            SDL_RenderCopy(app.renderer, e->texture, NULL, &dest);
        }
	}
}


//Konstantes scrolling der Map
void fortschritt(Entity start){
    Entity *e;
    for (e = start.next->next ; e->next != NULL ; e = e->next){
        e->x -= 1;
    }
}

//Collisionsbestimmung mit allem des Spieler
void SpielerInteract(Entity *Spieler, Entity entHead){
    Entity *i;
    int collision = 0;
    Spieler->coll = 0;
    for(i = Spieler->next; i->next != NULL; i= i->next){
        //gucken ob ein Block sich auf der X-Koordinate des Spielers aufhaelt
        if(Spieler->x > i->x && Spieler->x < (i->x + i->w) || (Spieler->x + Spieler->w) > i->x
           && (Spieler->x + Spieler->w) < (i->x + i->w) || Spieler->x <= i->x && (Spieler->x + Spieler->w) >= (i->x + i->w)){
               //Seitencoll bedingung
               if(Spieler->y + Spieler->h > i->y && Spieler->y + Spieler->h < i->y + i->h +1){
                    //Check side coll
                    if(Spieler->x + Spieler->w < i->x + i->w/2 && Spieler->x + Spieler->w > i->x){
                        //Handelt es sich um einen Gegner kommt es zum Pushback
                        if(i->AI == 1){
                            Spieler->x = i->x - Spieler->w - 50;
                            Spieler->leben -= 1;
                        }
                        else{
                            Spieler->x = i->x - Spieler->w;
                        }
                        }
                    if(Spieler->x > i->x + i->w/2){
                        if(i->AI == 1){
                            Spieler->x = i->x + i->w + 50;
                            Spieler->leben -= 1;
                        }
                        else{
                            Spieler->x = i->x + i->w;
                        }
                    }
               }
               //Check bottom coll
               if(Spieler->y + Spieler->h > i->y - 2 && Spieler->y + Spieler->h < i->y + 2 && Spieler->yV >= 0){
                   //Sicherstellung das der Spieler auch wirklich faellt
                    Spieler->coll = 1;
                    Spieler->y = i->y - Spieler->h;
                    Spieler->gelandet = 1;
                    Spieler->wasOnBlock = 1;
                    if(i->AI == 1){
                        //"Goomba" stomb
                        i->leben -= 1;
                        Spieler->yV = -150;
                        Spieler->gelandet = 0;
                        Spieler->wasOnBlock = 0;
                    }
                    if(i->AI == -1){
                        exit(0);
                    }
               }
        }
    }
    //Sicherstellung das der Spieler auch wirklich faellt
    if(!Spieler->coll){
        Spieler->gelandet = 0;
        Spieler->wasOnBlock = 0;
    }
}

//Das gleiche wie SpielerInteract bloß mit jedem Gegner

 void enemInteract(Entity *enem, Entity entHead, Entity *target){
    Entity *i;
    int collision = 0;
    for(i = entHead.next; i->next != NULL; i= i->next){
        if(!collision && enem->wasOnBlock){
            enem->gelandet = 0;
            enem->wasOnBlock = 0;
        }
        if(enem->x == i->x && enem->y == i->y)
            continue;
        if(i->x != target->x || i->y != target->y){
            if(enem->x > i->x && enem->x < (i->x + i->w) || (enem->x + enem->w) > i->x && (enem->x + enem->w) < (i->x + i->w) || enem->x <= i->x && (enem->x + enem->w) >= (i->x + i->w)){ //&& e->y < i->y && (e->y + e->h) > (i->y + i->h)){
                collision = 1;
                if(enem->y + enem->h > i->y && enem->y + enem->h < i->y + i->h +1){
                    //Check side coll
                    if(enem->x + enem->w < i->x + i->w/2 && enem->x + enem->w > i->x){
                        enem->x = i->x - enem->w;
                    }
                    if(enem->x > i->x + i->w/2){
                        enem->x = i->x + i->w;
                    }
                }
                if(enem->y + enem->h > i->y - 2 && enem->y + enem->h < i->y + 2 && enem->yV >= 0){
                    enem->y = i->y - enem->h;
                    enem->gelandet = 1;
                    enem->wasOnBlock = 1;
                }
            }
        }
    }
}



//Abfrage des Keyboard inputs

void eingabe(){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch (event.type)
        {
        //Hier wird exit aufgerufen was die atexit funktion cleanup() aufruft
        case SDL_QUIT:
            exit(0);
            break;
        case SDL_KEYDOWN:
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    app.left = 1;
                    break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    app.right = 1;
                    break;
                case SDL_SCANCODE_SPACE:
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    app.jump = 1;
                    break;

                }
                break;
        case SDL_KEYUP:
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    app.left = 0;
                    break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    app.right = 0;
                    break;
                }
                case SDL_SCANCODE_SPACE:
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    app.jump = 0;
                    break;

                }
                break;
            }
        }


//Funktion zum einfacheren Aufrufen von Texturen

SDL_Texture *loadTexture(char *filename)
{
	SDL_Texture *texture;

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);

	texture = IMG_LoadTexture(app.renderer, filename);

	return texture;
}

//Gravitation fuer jedes Wesen

void gravi(Entity thing){
    Entity *e;
	for (e = thing.next ; e->next != NULL ; e = e->next){
        if(!e->gelandet){
            e->y += e->yV / 60.0;
            e->yV += 9;
        }
        if(e->y + e->h >= 800){
            e->leben -= 1;
        }
        if(e->gelandet){
            e->yV = 0;
        }
    }
}

//Abfrage der Leben und alles was nicht mehr als null hat wird aus der doppelt verketteten Liste entfernt

void lifeCheck(Entity start){
    Entity *e;
    e = start.next;
    if(e->leben <= 0){
        exit(0);
    }
    e = e->next;
	for (e = e->next ; e->next != NULL ; e = e->next){
	    if(e->leben <= 0){
            e->next->before = e->before;
            e->before->next = e->next;
	    }
	}
}

//Unsere Gegner "KI"
//Diese sorgt dafuer das der Gegner auf den Spieler zu laeuft wenn er steht und weg laeuft wenn er springt

void moveenems(Entity start, Entity *target){
  Entity *enem;
  for (enem = start.next; enem!= NULL; enem = enem->next){
    if(enem->x >= (target->x +1000))
        continue;
    if(enem->AI==1){
        if(enem->detected >= 30){
            if(target->gelandet){
                if(enem->x > target->x ){
                    enem->x -= 1;
                }
                if (enem->x < target->x){
                    enem->x += 3;
                }
            }
            else{
                if(enem->x > target->x ){
                    enem->x += 3;
                }
                if (enem->x < target->x){
                    enem->x -= 1;
                }
            }
        }
        //Nur eingreifen wenn der Spieler in der Naehe ist
        if(enem->x - target-> x < 300 && target->x - enem->x < 300){
            if (enem->detected++ >= 200)
                enem->detected = 200;
        }
        else{
            if (enem->detected-- >= 0)
                enem->detected = 0;
        }
        enemInteract(enem,start,target);
    }
  }
}

int main(int argc, char *argv[])
{
    //Aufruf der Funktionen und Spieler erschaffung
    initializeSDL();
    SDL_Surface* surface = IMG_Load("resources/hello.png");
    spieler.texture = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_FreeSurface(surface);
    spieler.x = 100;
    spieler.y = 0;
    spieler.leben = 3;
    spieler.gelandet = 0;
    entHead.next = &spieler;
    spieler.next = &arr[0];
    entTail.next = NULL;
    atexit(cleanup);
    //Einlesen der Map in ein Array
    FILE *myFile = fopen("resources/map.dat", "r");
    if(myFile == NULL){
        exit(0);
        return 0;
    }
    char map[MAP_HEIGHT + 1][MAP_WIDTH + 1];
    int n = 0;
    int m = 1;
    arr[0].before = &spieler;
    Entity *e = &arr[0];
    //Je nach typ wird entweder Gegner Plattform oder Ziel erschaffen
    for(int j = 0; j < MAP_HEIGHT; j++){
        for(int k = 0; k < MAP_WIDTH; k++){
            map[j][k] = fgetc(myFile);
            if (map[j][k] == 'x'){
                arr[m].before = e;
                arr[m].gelandet = 1;
                arr[m].y = j*TILE_SIZE + 200;
                arr[m].x = k * TILE_SIZE;
                arr[m].leben = 1;
                surface = IMG_Load("resources/tile1.png");
                arr[m].texture = SDL_CreateTextureFromSurface(app.renderer, surface);
                SDL_FreeSurface(surface);
                e->next = &arr[m];
                e = &arr[m];
                m++;
            }
            if(map[j][k] == 'g'){
                arr[m].before = e;
                arr[m].gelandet = 0;
                arr[m].y = j*TILE_SIZE + 200;
                arr[m].x = k * TILE_SIZE;
                arr[m].AI = 1;
                arr[m].leben = 1;
                surface = IMG_Load("resources/enemy.png");
                arr[m].texture = SDL_CreateTextureFromSurface(app.renderer, surface);
                SDL_FreeSurface(surface);
                e->next = &arr[m];
                e = &arr[m];
                m++;
            }
            if(map[j][k] == 'f'){
                arr[m].before = e;
                arr[m].gelandet = 1;
                arr[m].y = j*TILE_SIZE + 200;
                arr[m].x = k * TILE_SIZE;
                arr[m].AI = -1;
                arr[m].leben = 1;
                surface = IMG_Load("resources/tile2.png");
                arr[m].texture = SDL_CreateTextureFromSurface(app.renderer, surface);
                SDL_FreeSurface(surface);
                e->next = &arr[m];
                e = &arr[m];
                m++;
            }
        }
    }
    arr[m].next = &entTail;
    //Animationsschleife
    while(1){
        prepareScene();
        eingabe();
        if (app.jump && spieler.gelandet == 1)
		{
			spieler.yV -= SPEED;
			spieler.gelandet = 0;
		}
		if (app.left)
		{
			spieler.x -= 4;
		}

		if (app.right)
		{
			spieler.x += 4;
		}
		gravi(entHead);
		lifeCheck(entHead);
		//moveenems(entHead, &spieler);
        //blit(spieler.texture, spieler.x, spieler.y);
        SpielerInteract(&spieler, entHead);
        moveenems(entHead, &spieler);
        fortschritt(entHead);
        //SpielerInteract(&enemy, entHead);
        blit(entHead);
		presentScene();
        //Anpassbar um 60 fps zu erhalten
		SDL_Delay(14);
    }
    return 0;
}
