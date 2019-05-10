/*
 * two_player_shooty.c
 *
 *  Created on: Mar 29, 2014
 *      Author: stahlberger
 */

#include "project_settings.h"
#include "random_int.h"
#include "stddef.h"
#include "strings.h"
#include "game.h"
#include "timing.h"
#include "task.h"
#include "terminal.h"
#include "random_int.h"
#include "subsystem.h"
#include "two_player_shooty.h"






void Shooty_Init(void) {
    // Register the module with the game system and give it the name "shooty"
    game.id = Game_Register("shooty", "2 player shooter", Play, Help);
    Game_EnableMultiPlayer(game.id, 2);
}

void Help(void) { 
    Game_Printf("Player 1 use 'a' and 's' to move your gun left and right\r\n"
        "'d' to FIRE\r\n"
        "Player 2 use '<' and '>' to move your gun left and right\r\n"
        "'?' to FIRE\r\n");
}



void Play(void) {
    volatile uint8_t i;
    // clear the screen
    Game_ClearScreen();
    // set the color of the terminal for the aesthetic
    Game_SetColor(ForegroundGreen);
    // draw a box around our map
    Game_DrawRect(0, 0, MAP_WIDTH, MAP_HEIGHT);
    // draw title in box
    Game_PrintfXY(MAP_WIDTH/2-7, 0, "2 PLAYER SHOOTY");

    //SCORE BOX INITIALIZATION
    // draw a box for the scores
    Game_DrawRect(MAP_WIDTH+1, 0, MAP_WIDTH+20, MAP_HEIGHT/3);
    // print SCORE in the score box
    Game_PrintfXY(MAP_WIDTH+8, 0, "SCORE");
    //initialize player 1 score to screen
    Game_PrintfXY(MAP_WIDTH+6, 2, "Player 1: 0");
    //initialize player 2 score to screen
    Game_PrintfXY(MAP_WIDTH+6, 4, "Player 2: 0");

    //SHOT BOX INITIALIZATION
    //draw box for shots fired
    Game_DrawRect(MAP_WIDTH+1, MAP_HEIGHT/3+1, MAP_WIDTH+20, 2*(MAP_HEIGHT/3)+1);
    // print SHOTS FIRED in the score box
    Game_PrintfXY(MAP_WIDTH+5, MAP_HEIGHT/3+1, "SHOTS FIRED");
    //initialize player 1 shots fired to screen
    Game_PrintfXY(MAP_WIDTH+6, MAP_HEIGHT/3+3, "Player 1: 0");
    //initialize player 2 shots fired to screen
    Game_PrintfXY(MAP_WIDTH+6, MAP_HEIGHT/3+5, "Player 2: 0");

    //ACCURACY BOX INITIALIZATION
    //draw box for Accuracy
    Game_DrawRect(MAP_WIDTH+1, 2*(MAP_HEIGHT/3)+2, MAP_WIDTH+20, MAP_HEIGHT);
    // print ACCURACY in the score box
    Game_PrintfXY(MAP_WIDTH+7, 2*(MAP_HEIGHT/3)+2, "ACCURACY");
    //initialize player 1 accuracy to screen
    Game_PrintfXY(MAP_WIDTH+4, 2*(MAP_HEIGHT/3)+4, "Player 1: 0%%");
    //initialize player 2 accuracy to screen
    Game_PrintfXY(MAP_WIDTH+4, 2*(MAP_HEIGHT/3)+6, "Player 2: 0%%");

    //SHIELD BOX INITIALIZATION
    //draw box for Shield
    Game_DrawRect(MAP_WIDTH+21, 0, MAP_WIDTH+40, MAP_HEIGHT/3);
    // print ACCURACY in the score box
    Game_PrintfXY(MAP_WIDTH+23, 0, "SHIELD REMAINING");
    //initialize player 1 accuracy to screen
    Game_PrintfXY(MAP_WIDTH+24, 2, "Player 1: ");
    //initialize player 2 accuracy to screen
    Game_PrintfXY(MAP_WIDTH+24, 5, "Player 2: ");

    //PRINT GAME INSTRUCTIONS UNDERNEATH THE MAP
    Game_PrintfXY(0, MAP_HEIGHT+1, "--------------------------------------------------------------------------------");
    Game_PrintfXY(0, MAP_HEIGHT+2, "|  Welcome to Two Player Shooty!!!!                                            |");
    Game_PrintfXY(0, MAP_HEIGHT+3, "|  The objective is to outplay and outscore your opponent                      |");
    Game_PrintfXY(0, MAP_HEIGHT+4, "|  Player 1 (BOTTOM): Use 'A' and 'S' to move left and right and 'D' to shoot  |");
    Game_PrintfXY(0, MAP_HEIGHT+5, "|  Player 2  (TOP)  : Use '<' and '>' to move left and right and '?' to shoot  |");
    Game_PrintfXY(0, MAP_HEIGHT+6, "|  At any time press 'P' to end the game                                       |");
    Game_PrintfXY(0, MAP_HEIGHT+7, "--------------------------------------------------------------------------------");

    // initialize game variables
    for(i = 0; i < MAX_SHOTS; i++) shots1[i].status = 0;
    for(i = 0; i < MAX_SHOTS; i++) shots2[i].status = 0;
    //for(i = 0; i < MUH_MAX_ENEMIES; i++) enemies[i].status = 0;
    game.x1 = 2;
    game.x2 = MAP_WIDTH - 2;
    game.c1 = 'Ц';
    game.c2 = 'Ц';
    game.sx1 = 2;
    game.sx2 = MAP_WIDTH - 2;
    game.s1 = '^';
    game.s2 = 'v';
    game.sh1 = 3;
    game.sh2 = 3;
    game.hasShield1 = 1;
    game.hasShield2 = 1;
    game.score1 = 0;
    game.score2 = 0;
    game.shots_fired1 = 0;
    game.shots_fired2 = 0;
    // draw the first "gun" at the bottom left of the map
    Game_CharXY(game.c1, game.x1, MAP_HEIGHT - 1);
    Game_RegisterPlayer1Receiver(Receiver1);
    // draw the second "gun" at the top right of the map
    Game_CharXY(game.c2, game.x2, 1);
    Game_CharXY(game.s1, game.sx1, MAP_HEIGHT - 2);
    Game_CharXY(game.s2, game.sx2, 2);
    // hide cursor
    Game_HideCursor();
}



void Fire1(void) {
    static uint32_t last_fired;
    volatile uint8_t i;
    // pointer to bullet to use
    char_object_t * shot = 0;
    // make sure a shot was not just fired
    if (TimeSince(last_fired) <= FIRE_SPEED) return;

    // find and unused bullet
    for(i = 0; i < MAX_SHOTS; i++) if(shots1[i].status == 0) shot = &shots1[i];
    if(shot) {
        // schedule MoveUpShot1 to run every FIRE_SPEED ms, this is what makes the '^' move
        Task_Schedule((task_t)MoveUpShot, shot, FIRE_SPEED, FIRE_SPEED);
    }else { // if no shots are left
        // indicate out of ammo by changing the pointer
        if (game.c1 == 'Ц') {
            game.c1 = 'o';
            Game_CharXY(game.c1, game.x1, MAP_HEIGHT - 1);
        }
        return;
    }
    // change the status of the shot to indicate that it is used
    shot->status = 1;
    // start the shot at the bottom just above the gun
    shot->y = MAP_HEIGHT - 3;
    shot->x = game.x1;
    shot->c = 'o';
    Game_CharXY('Ф', game.x1, MAP_HEIGHT - 2);
    // track the time the last shot was fired so you can't have two on top of each other
    last_fired = TimeNow();
    game.shots_fired1++;
    Game_PrintfXY(MAP_WIDTH+16, MAP_HEIGHT/3+3, "%d", game.shots_fired1);
    //calculate and print shot accuracy
    int percent = (game.score1*100.0)/game.shots_fired1;
    Game_PrintfXY(MAP_WIDTH+14, 2*(MAP_HEIGHT/3)+4, "     ");
    Game_PrintfXY(MAP_WIDTH+14, 2*(MAP_HEIGHT/3)+4, "%d%%", percent);
}

void Fire2(void) {
    static uint32_t last_fired2;
    volatile uint8_t i;
    // pointer to bullet to use
    char_object_t * shot = 0;
    // make sure a shot was not just fired
    if (TimeSince(last_fired2) <= FIRE_SPEED) return;

    // find and unused bullet
    for(i = 0; i < MAX_SHOTS; i++) if(shots2[i].status == 0) shot = &shots2[i];
    if(shot) {
        // schedule MoveDownShot1 to run every FIRE_SPEED ms, this is what makes the 'v' move
        Task_Schedule((task_t)MoveDownShot, shot, FIRE_SPEED, FIRE_SPEED);
    }else { // if no shots are left
        // indicate out of ammo by changing the pointer
        if (game.c2 == 'Ц') {
            game.c2 = 'o';
            Game_CharXY(game.c2, game.x2, 1); //changed from -1
        }
        return;
    }
    // change the status of the shot to indicate that it is used
    shot->status = 1;
    // start the shot at the bottom just above the gun
    shot->y = 3; //changed from -2
    shot->x = game.x2;
    shot->c = 'o';
    Game_CharXY('Ц', game.x2, 2); //changed from -2
    // track the time the last shot was fired so you can't have two on top of each other
    last_fired2 = TimeNow();
    game.shots_fired2++;
    //Print shots fired
    Game_PrintfXY(MAP_WIDTH+16, MAP_HEIGHT/3+5, "%d", game.shots_fired2);
    //calculate and print shot accuracy
    int percent = (game.score2*100.0)/game.shots_fired2;
    Game_PrintfXY(MAP_WIDTH+14, 2*(MAP_HEIGHT/3)+6, "     ");
    Game_PrintfXY(MAP_WIDTH+14, 2*(MAP_HEIGHT/3)+6, "%d%%", percent);

}


void ScorePoint1(void) {
    if(game.score1 >= 3){
        Game_Bell();
        GameOver();
    }
    else{
        game.score1++;
            // sound the alarm
        Game_Bell();
    }
}

void ScorePoint2(void) {
    if(game.score2 >= 3){
            Game_Bell();
            GameOver();
        }
        else{
            game.score2++;
                // sound the alarm
            Game_Bell();
        }
}

void MoveRight1(void) {
    // make sure we can move right
    if (game.x1 < MAP_WIDTH - 1) {
        // clear location
        Game_CharXY(' ', game.x1, MAP_HEIGHT - 1);
        game.x1++;
        Game_CharXY(game.c1, game.x1, MAP_HEIGHT - 1);
        if(game.hasShield1){
            Game_CharXY(' ', game.sx1, MAP_HEIGHT - 2);
            game.sx1++;
            Game_CharXY(game.s1, game.sx1, MAP_HEIGHT - 2);
        }
    }
}

void MoveRight2(void) {
    // make sure we can move right
    if (game.x2 < MAP_WIDTH - 1) {
        // clear location
        Game_CharXY(' ', game.x2, 1);
        game.x2++;
        Game_CharXY(game.c2, game.x2, 1);
        if(game.hasShield1){
            Game_CharXY(' ', game.sx2, 2);
            game.sx2++;
            Game_CharXY(game.s2, game.sx2, 2);
        }
    }
}

void MoveLeft1(void) {
    // make sure we can move left
    if (game.x1 > 1) {
        // clear location
        Game_CharXY(' ', game.x1, MAP_HEIGHT - 1);
        game.x1--;
        Game_CharXY(game.c1, game.x1, MAP_HEIGHT - 1);
        if(game.hasShield1){
            Game_CharXY(' ', game.sx1, MAP_HEIGHT - 2);
            game.sx1--;
            Game_CharXY(game.s1, game.sx1, MAP_HEIGHT - 2);
        }
    }
}

void MoveLeft2(void) {
    // make sure we can move left
    if (game.x2 > 1) {
        // clear location
        Game_CharXY(' ', game.x2, 1);
        game.x2--;
        Game_CharXY(game.c2, game.x2, 1);
        if(game.hasShield1){
            Game_CharXY(' ', game.sx2, 2);
            game.sx2--;
            Game_CharXY(game.s2, game.sx2, 2);
        }
    }
}

void DeleteShot1(char_object_t * o) {
    // set status to 0
    if(o) o->status = 0;
    // remove the tasks used to move the shot
    Task_Remove((task_t)MoveUpShot, o);

    // if there were no shots left then change the pointer back to ^
    if (game.c1 != 'Ц') {
        game.c1 = 'Ц';
        Game_CharXY(game.c1, game.x1, MAP_HEIGHT - 1);
    }
}

void DeleteShot2(char_object_t * o) {
    // set status to 0
    if(o) o->status = 0;
    // remove the tasks used to move the shot
    Task_Remove((task_t)MoveDownShot, o);

    // if there were no shots left then change the pointer back to v
    if (game.c2 != 'Ц') {
        game.c2 = 'Ц';
        Game_CharXY(game.c2, game.x2, 1);
    }
}


void CheckCollisionPlayer1(char_object_t * shot) {
    if((game.x1 == shot->x) && (shot->y == MAP_HEIGHT -1)){
        DeleteShot2(shot);
        // put a ' ' where the collision happened and score a point
        Game_CharXY('Ц', shot->x, shot->y);
        ScorePoint2();
        //Adds points to the scoreboard when a collision happens
        Terminal_CursorXY(SUBSYSTEM_UART, MAP_WIDTH+16, 4);
        Subsystem_printf("%d", game.score2);
//        Game_CharXY(game.score2 + '0', MAP_WIDTH+12, 6);
    }
}

void CheckCollisionPlayer2(char_object_t * shot) {
    if((game.x2 == shot->x) && (shot->y == 1)){
        DeleteShot1(shot);
        // put a ' ' where the collision happened and score a point
        Game_CharXY('Ц', shot->x, shot->y);
        ScorePoint1();
        //Adds points to the scoreboard when a collision happens
        Terminal_CursorXY(SUBSYSTEM_UART, MAP_WIDTH+16, 2);
        Subsystem_printf("%d", game.score1);
    }
}

/*void CheckCollisionShot(char_object_t * shot1, char_object_t * shot2){
    if(shot1->y == shot2->y){
        DeleteShot1(shot1);
        DeleteShot2(shot2);
        Game_Bell();
    }
}*/
/*future renovations*/

void MoveUpShot(char_object_t * o) {
    // first make sure we can move up
    if (o->y > 1) {
        // clear location
        Game_CharXY(' ', o->x, o->y);
        // update y position
        o->y--;
        // re-print
        Game_CharXY(o->c, o->x, o->y);
        CheckCollisionPlayer2(o);
    } else {
        // clear the shot
        Game_CharXY(' ', o->x, o->y);
        DeleteShot1(o);
    }
}

void MoveDownShot(char_object_t * o) {
    // first make sure we can move down
    if (o->y < MAP_HEIGHT-1) {
        // clear location
        Game_CharXY(' ', o->x, o->y);
        // update y position
        o->y++;
        // re-print
        Game_CharXY(o->c, o->x, o->y);
        CheckCollisionPlayer1(o);
    } else {
        // clear the shot
        Game_CharXY(' ', o->x, o->y);
        DeleteShot2(o);
    }
}




void Receiver1(uint8_t c) {
    switch (c) {
        case 'a':
        case 'A':
            MoveLeft1();
            break;
        case 's':
        case 'S':
            MoveRight1();
            break;
        case 'd':
        case 'D':
            Fire1();
            break;
        case ',':
        case '<':
            MoveLeft2();
            break;
        case '.':
        case '>':
            MoveRight2();
            break;
        case '/':
        case '?':
            Fire2();
            break;
        case 'P':
        case 'p':
            GameOver();
            break;
        default:
            break;
    }
}

void GameOver(void) {
    long i = 0;
    int percent1 = (game.score1*100)/game.shots_fired1;
    int percent2 = (game.score2*100)/game.shots_fired2;
    // set cursor below bottom of map and description
    Game_ClearScreen();
    Game_SetColor(ForegroundRed);
    Game_PrintfXY(0, 0,
"  _____ _    _  _____  ______ _       _____   _____________   _____ _   _ _____  _____ _______   __\r\n"
" |_   _| |  | ||  _  | | ___ \\ |     / _ \\ \\ / /  ___| ___ \\ /  ___| | | |  _  ||  _  |_   _\\ \\ / /\r\n"
"   | | | |  | || | | | | |_/ / |    / /_\\ \\ V /| |__ | |_/ / \\ `--.| |_| | | | || | | | | |  \\ V / \r\n"
"   | | | |/\\| || | | | |  __/| |    |  _  |\\ / |  __||    /   `--. \\  _  | | | || | | | | |   \\ /  \r\n"
"   | | \\  /\\  /\\ \\_/ / | |   | |____| | | || | | |___| |\\ \\  /\\__/ / | | \\ \\_/ /\\ \\_/ / | |   | |  \r\n"
"   \\_/  \\/  \\/  \\___/  \\_|   \\_____/\\_| |_/\\_/ \\____/\\_| \\_| \\____/\\_| |_/\\___/  \\___/  \\_/   \\_/  \r\n"
"                                                                                                  \r\n"
"                                                                                                  \r\n"
"                  _____   ___  ___  ___ _____   _____  _   _ ___________                          \r\n"
"                 |  __ \\ / _ \\ |  \\/  ||  ___| |  _  || | | |  ___| ___ \\                         \r\n"
"                 | |  \\// /_\\ \\| .  . || |__   | | | || | | | |__ | |_/ /                         \r\n"
"                 | | __ |  _  || |\\/| ||  __|  | | | || | | |  __||    /                          \r\n"
"                 | |_\\ \\| | | || |  | || |___  \\ \\_/ /\\ \\_/ / |___| |\\ \\                          \r\n"
"                  \\____/\\_| |_/\\_|  |_/\\____/   \\___/  \\___/\\____/\\_| \\_|                         \r\n"
"                                                                                                  \r\n"
"                                                                                                  \r\n");

    //sound and delay
    for(i=1000000;i>0;i--);

    // show score
    //SCORE BOX INITIALIZATION
    // draw a box for the scores
    Game_DrawRect(0, 20, 20, 25);
    // print SCORE in the score box
    Game_PrintfXY(5, 20, "FINAL SCORE");
    //initialize player 1 score to screen
    Game_PrintfXY(4, 22, "Player 1: %d", game.score1);
    //initialize player 2 score to screen
    Game_PrintfXY(4, 24, "Player 2: %d", game.score2);

    Game_Bell();

    //sound and delay
    for(i=1000000;i>0;i--);


    //SHOT BOX INITIALIZATION
    //draw box for shots fired
    Game_DrawRect(25, 20, 45, 25);
    // print SHOTS FIRED in the score box
    Game_PrintfXY(27, 20, "FINAL SHOTS FIRED");
    //initialize player 1 shots fired to screen
    Game_PrintfXY(30, 22, "Player 1: %d", game.shots_fired1);
    //initialize player 2 shots fired to screen
    Game_PrintfXY(30, 24, "Player 2: %d", game.shots_fired2);

    Game_Bell();

    //sound and delay
    for(i=1000000;i>0;i--);

    //ACCURACY BOX INITIALIZATION
    //draw box for Accuracy
    Game_DrawRect(50, 20, 70, 25);
    // print ACCURACY in the score box
    Game_PrintfXY(54, 20, "FINAL ACCURACY");
    //initialize player 1 accuracy to screen
    Game_PrintfXY(54, 22, "Player 1: %d%%", percent1);
    //initialize player 2 accuracy to screen
    Game_PrintfXY(54, 24, "Player 2: %d%%", percent2);

    Game_Bell();

    // unregister the receivers used to run the game
    Game_UnregisterPlayer1Receiver(Receiver1);
    // show cursor (it was hidden at the beginning
    Game_CursorXY(0, 27);
    Game_ShowCursor();
    Game_GameOver();
}
