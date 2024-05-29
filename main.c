#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

#define WIDTH 1920
#define HEIGHT 1080

#define SLOT_WIDTH 50
#define SLOT_HEIGHT 100

#define PARK_WIDTH 24
#define PARK_HEIGHT (HEIGHT/SLOT_HEIGHT)-1

#define STRDIM 100
#define BTN 3

Font font;

typedef struct{
    char id[STRDIM];

    int animationY;
}car;

typedef struct{
    Rectangle rect;

    bool active;
    car car;
}slot;

typedef struct{
    Rectangle button;

    bool clicked;
}button;

typedef struct{
    Texture insertButton;
    Texture moveButton;
    Texture removeButton;
}buttonTextures;

void setPark(slot park[][PARK_WIDTH]);
void managePark(slot park[][PARK_WIDTH]);

void drawPark(slot park[][PARK_WIDTH], Texture slotTexture);
void drawCars(slot park[][PARK_WIDTH], Texture carTexture);

void showId(slot park[][PARK_WIDTH]);

void setButtons(button buttons[]);
void manageButtons(button buttons[]);
void drawButtons(button buttons[], buttonTextures* buttonTextures);

void placeCar(slot park[][PARK_WIDTH]);
void swapCar(slot park[][PARK_WIDTH]);
void removeCar(slot park[][PARK_WIDTH]);

bool idExists(slot park[][PARK_WIDTH], char id[]);
void removeId(slot park[][PARK_WIDTH], char id[]);

void manageSingleTextBox(Rectangle textBox, char *tempSting, int* letterCount, int max);

int main(){
    slot park[PARK_HEIGHT][PARK_WIDTH];
    button buttons[BTN];
    Texture slotTexture, carTexture;
    buttonTextures btns;

    InitWindow(WIDTH, HEIGHT, "parking");
    ToggleFullscreen();
    SetTargetFPS(60);

    slotTexture = LoadTextureFromImage(LoadImage("images/slot.png"));
    carTexture = LoadTextureFromImage(LoadImage("images/car.png"));

    btns.insertButton = LoadTextureFromImage(LoadImage("images/insertButton.png"));
    btns.moveButton = LoadTextureFromImage(LoadImage("images/moveButton.png"));
    btns.removeButton = LoadTextureFromImage(LoadImage("images/removeButton.png"));

    font = LoadFont("fonts/JetBrainsMonoNL-Regular.ttf");

    setPark(park);
    setButtons(buttons);

    while (!WindowShouldClose()){
        managePark(park);
        manageButtons(buttons);

        if (buttons[0].clicked)
            placeCar(park);
        else if (buttons[1].clicked)
            swapCar(park);
        else if (buttons[2].clicked)
            removeCar(park);

        BeginDrawing();
            ClearBackground(LIGHTGRAY);
        
            drawPark(park, slotTexture);
            drawCars(park, carTexture);
            showId(park);

            drawButtons(buttons, &btns);
        EndDrawing();
    }

    return 0;
}

void setPark(slot park[][PARK_WIDTH]){
    int i, ii;

    for (i = 0; i < PARK_HEIGHT; i++){
        for (ii = 0; ii < PARK_WIDTH; ii++){
            park[i][ii].active = false;
            park[i][ii].rect = (Rectangle){(ii*3/2.0*SLOT_WIDTH)+(SLOT_WIDTH*3/2.0), (i*SLOT_HEIGHT)+SLOT_HEIGHT, 50, 100};
            park[i][ii].car.animationY = HEIGHT;

            strcpy(park[i][ii].car.id, "\0");
        }
    }
}

void managePark(slot park[][PARK_WIDTH]){
    int i, ii;

    for (i = 0; i < PARK_HEIGHT; i++){
        for (ii = 0; ii < PARK_WIDTH; ii++){
            if (CheckCollisionPointRec(GetMousePosition(), park[i][ii].rect)){
                if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
                    park[i][ii].active = false;
                    park[i][ii].car.animationY = HEIGHT;
                    strcpy(park[i][ii].car.id, "\0");
                }
            }
        }
    }
}



void drawPark(slot park[][PARK_WIDTH], Texture slotTexture){
    int i, ii;

    for (i = 0; i < PARK_HEIGHT; i++){
        for (ii = 0; ii < PARK_WIDTH; ii++){
            DrawTexture(slotTexture, park[i][ii].rect.x, park[i][ii].rect.y, WHITE);
        }
    }

    for (i = 0; i < PARK_HEIGHT; i++){
        DrawTextEx(font, TextFormat("%d", i+1), (Vector2){30, (i*SLOT_HEIGHT)+SLOT_HEIGHT+((MeasureTextEx(font, TextFormat("%d", i+1), 40, 0).y)/2)}, 40, 0, BLACK);
    }
    for (i = 0; i < PARK_WIDTH; i++){
        DrawTextEx(font, TextFormat("%c", (char)(i+'A')), (Vector2){((i*SLOT_WIDTH*3/2.0)+(SLOT_WIDTH*3/2.0))+15, 40}, 40, 0, BLACK);
    }
}

void drawCars(slot park[][PARK_WIDTH], Texture carTexture){
    int i, ii;

    for (i = 0; i < PARK_HEIGHT; i++){
        for (ii = 0; ii < PARK_WIDTH; ii++){
            if (park[i][ii].active){
                if (park[i][ii].car.animationY > park[i][ii].rect.y)
                    park[i][ii].car.animationY -= 10;

                DrawTexture(carTexture, park[i][ii].rect.x, park[i][ii].car.animationY, WHITE);
            }
        }
    }
}




void showId(slot park[][PARK_WIDTH]){
    int i, ii;

    for (i = 0; i < PARK_HEIGHT; i++){
        for (ii = 0; ii < PARK_WIDTH; ii++){
            if (CheckCollisionPointRec(GetMousePosition(), park[i][ii].rect) && park[i][ii].active){
                DrawRectangle(GetMouseX(), GetMouseY(), MeasureTextEx(font, park[i][ii].car.id, 40, 0).x+10, 50, BLACK);
                DrawRectangleLinesEx((Rectangle){GetMouseX(), GetMouseY(), MeasureTextEx(font, park[i][ii].car.id, 40, 0).x+10, 50}, 4, DARKPURPLE);
                DrawTextEx(font, park[i][ii].car.id, (Vector2){GetMouseX()+5, GetMouseY()}, 40, 0, WHITE);
            }
        }
    }
}

void removeId(slot park[][PARK_WIDTH], char id[]){
    int i, ii;

    for (i = 0; i < PARK_HEIGHT; i++){
        for (ii = 0; ii < PARK_WIDTH; ii++){
            if (strcmp(id, park[i][ii].car.id) == 0){
                park[i][ii].active = false;
                park[i][ii].car.animationY = HEIGHT;
                strcpy(park[i][ii].car.id, "\0");
            }
        }
    }
}

void removeCar(slot park[][PARK_WIDTH]){
    bool exitVar = false;
    int letterCount = 0;

    char idString[STRDIM] = "\0";
    Rectangle idTextBox = (Rectangle){(WIDTH/2.0)-200, (HEIGHT/2.0)-25, 400, 50};

    while (!exitVar){
        manageSingleTextBox(idTextBox, idString, &letterCount, STRDIM-1);

        if (IsKeyPressed(KEY_ENTER) && idString[0] != '\0'){
            removeId(park, idString);
            exitVar = true;
        }

        if (WindowShouldClose()){
            CloseWindow();
            exit(0);
            return;
        }

        BeginDrawing();
            DrawRectangleRec(idTextBox, BLACK);
            DrawRectangleLinesEx(idTextBox, 4, DARKPURPLE);
            DrawTextEx(font, "insert id (ex: 5A)", (Vector2){idTextBox.x, idTextBox.y-100}, 40, 0, BLACK);
            DrawTextEx(font, idString, (Vector2){idTextBox.x, idTextBox.y}, 40, 0, WHITE);

            ClearBackground(LIGHTGRAY);
        EndDrawing();
    }
}



void setButtons(button buttons[]){
    int i;

    for (i = 0; i < BTN; i++){
        buttons[i].clicked = false;
        buttons[i].button = (Rectangle){(i*100)+10, HEIGHT-60, 50, 50};
    }
}

void manageButtons(button buttons[]){
    int i;

    for (i = 0; i < BTN; i++){
        if (CheckCollisionPointRec(GetMousePosition(), buttons[i].button) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            buttons[i].clicked = true;
        }else
            buttons[i].clicked = false;
    }
}

void drawButtons(button buttons[], buttonTextures *buttonTextures){
    DrawTexture(buttonTextures->insertButton, buttons[0].button.x, buttons[0].button.y, WHITE);
    DrawTexture(buttonTextures->moveButton, buttons[1].button.x, buttons[1].button.y, WHITE);
    DrawTexture(buttonTextures->removeButton, buttons[2].button.x, buttons[2].button.y, WHITE);
}



void placeCar(slot park[][PARK_WIDTH]){
    bool exitVar = false;
    int letterCountPosition = 0;
    int letterCountId = 0;

    char positionTempString[3];
    char idTempString[STRDIM];

    Rectangle positionTextBox = (Rectangle){(WIDTH/2.0)-200, HEIGHT/4.0, 400, 50};
    Rectangle idTextBox = (Rectangle){(WIDTH/2.0)-200, (HEIGHT/2.0)+(HEIGHT/4.0), 400, 50};

    strcpy(positionTempString, "\0");
    strcpy(idTempString, "\0");

    while (!exitVar){
        manageSingleTextBox(positionTextBox, positionTempString, &letterCountPosition, 2);
        manageSingleTextBox(idTextBox, idTempString, &letterCountId, STRDIM-1);


        if (IsKeyPressed(KEY_ENTER) && idTempString[0] != '\0' && positionTempString[0] != '\0' && positionTempString[1] != '\0'){
            //input example 3A (y, x)
            park[(int)(positionTempString[0]-'1')][(int)(positionTempString[1]-'A')].active = true;
            park[(int)(positionTempString[0]-'1')][(int)(positionTempString[1]-'A')].car.animationY = HEIGHT;
            strcpy(park[(int)(positionTempString[0]-'1')][(int)(positionTempString[1]-'A')].car.id, idTempString);

            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
            return;
        }

        if (WindowShouldClose()){
            CloseWindow();
            exit(0);
        }

        BeginDrawing();
            DrawRectangleRec(positionTextBox, BLACK);
            DrawRectangleLinesEx(positionTextBox, 4, DARKPURPLE);
            DrawRectangleRec(idTextBox, BLACK);
            DrawRectangleLinesEx(idTextBox, 4, DARKPURPLE);

            DrawTextEx(font, "POSITION (EX: 5A)", (Vector2){positionTextBox.x, positionTextBox.y-100}, 40, 0, BLACK);
            DrawTextEx(font, positionTempString, (Vector2){positionTextBox.x, positionTextBox.y}, 40, 0, WHITE);
            DrawTextEx(font, idTempString, (Vector2){idTextBox.x, idTextBox.y}, 40, 0, WHITE);

            ClearBackground(LIGHTGRAY);
        EndDrawing();
    }
}

void swapCar(slot park[][PARK_WIDTH]){
    bool exitVar = false;
    int idCount = 0;
    int positionCount = 0;

    char idString[STRDIM] = "\0";
    char PositionString[3] = "\0";

    Rectangle idRect = (Rectangle){(WIDTH/2.0)-200, HEIGHT/4.0, 400, 50};
    Rectangle PositionRect = (Rectangle){(WIDTH/2.0)-200, (HEIGHT/2.0)+(HEIGHT/4.0), 400, 50};

    while (!exitVar){
        manageSingleTextBox(idRect, idString, &idCount, STRDIM-1);
        manageSingleTextBox(PositionRect, PositionString, &positionCount, 2);

        if (WindowShouldClose()){
            CloseWindow();
            exit(0);
            return;
        }

        if (IsKeyPressed(KEY_ENTER)){
            if (idExists(park, idString) && idString[0] != '\0' && !park[PositionString[0]-'1'][PositionString[1]-'A'].active){
                removeId(park, idString);
                park[PositionString[0]-'1'][PositionString[1]-'A'].active = true;
                park[PositionString[0]-'1'][PositionString[1]-'A'].car.animationY = HEIGHT;
                strcpy(park[PositionString[0]-'1'][PositionString[1]-'A'].car.id, idString);
                
                exitVar = true;
            }
        }

        BeginDrawing();
            DrawRectangleRec(idRect, BLACK);
            DrawRectangleRec(PositionRect, BLACK);

            DrawRectangleLinesEx(idRect, 4, DARKPURPLE);
            DrawRectangleLinesEx(PositionRect, 4, DARKPURPLE);

            DrawTextEx(font, "insert id", (Vector2){idRect.x, idRect.y-100}, 40, 0, BLACK);
            DrawTextEx(font, "move to (ex: A5)", (Vector2){PositionRect.x, PositionRect.y}, 40, 0, BLACK);

            DrawTextEx(font, idString, (Vector2){idRect.x, idRect.y}, 40, 0, WHITE);
            DrawTextEx(font, PositionString, (Vector2){PositionRect.x, PositionRect.y}, 40, 0, WHITE);

            ClearBackground(LIGHTGRAY);
        EndDrawing();
    }
}

bool idExists(slot park[][PARK_WIDTH], char id[]){
    int i, ii;

    for (i = 0; i < PARK_HEIGHT; i++){
        for (ii = 0; ii < PARK_WIDTH; i++){
            if (strcmp(id, park[i][ii].car.id))
                return true;
        }
    }

    return false;
}



void manageSingleTextBox(Rectangle textBox, char *tempString, int* letterCount, int max){
    bool textOnMouse = false;
    int key;

    if (CheckCollisionPointRec(GetMousePosition(), textBox)) textOnMouse = true;
    else textOnMouse = false;

    if (textOnMouse){
        SetMouseCursor(MOUSE_CURSOR_IBEAM);

        key = GetCharPressed();

        if (key > 0){
            if (*letterCount < max){
                tempString[*letterCount] = (char)key;
                tempString[*letterCount+1] = '\0';
                *letterCount += 1;
            }

            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)){
            *letterCount -= 1;
            if (*letterCount < 0) *letterCount = 0;
            tempString[*letterCount] = '\0';
        }
    }else{
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }
}