/** @defgroup shooty Two Player Shooty Module
 * @file two_player_shooty.h
 * @addtogroup shooty Two Player Shooty Module
 *
 *
 * Created on: May 10, 2019
 * @author Author DJ Stahlberger
 *
 * @brief Learn more about Two Player Shooty
 *
 * Two Player Shooty is a family friendly, 2-D shooter of epic proportions. It features alien spacecraft
 * with unique weaponry along with a one of a kind endgame! It takes a single UART input from a keyboard
 * and uses seperate keys to determine actions for players 1 and 2. Player 1 uses 'A' and 'S' to move left and right,
 * and 'D' to shoot. Player 2 uses '<', '>', and '?' to move left, right, and shoot. The goal of the game is to hit
 * the other player three times, then you win the game.
 *
 * In order to use this module, $game shooty play must be typed into a terminal window. That will initiate
 * the Play function. This initializes the game with visuals and and waits for user input.
 *
 * @{
 */

#include "game.h"

#ifndef TWO_PLAYER_SHOOTY_H_
#define TWO_PLAYER_SHOOTY_H_


#define MAP_WIDTH 30 /*!< Define for map width. Since multiple functions use the same variable
it makes sense to define it once so there aren't repetetive changes made to the code. Allows for quick
modification with a single number change. */
#define MAP_HEIGHT 20 /*!< Define for the height of the map. Since multiple functions use the same variable
it makes sense to define it once so there aren't repetetive changes made to the code. Allows for quick
modification with a single number change. */
// times in ms
#define FIRE_SPEED 60 /*!< Defines the maximum speed at which the players can fire */

/*!< Game Structure */
struct shooty_t {
    char x1; /*! x coordinate of gun 1 */
    char x2; /*! x coordinate of gun 2 */
    char c1; /*! character of gun 1 */
    char c2; /*! character of gun 2 */
    char sx1;/*! x coordinate of shield 1 */
    char sx2;/*! x coordinate of shield 2 */
    char s1; /*! character of shield 1 */
    char s2; /*! character of shield 2 */
    int sh1; /*! health of shield 1 */
    int sh2; /*! health of shield 2 */
    int hasShield1; /*! status of player 1 shield */
    int hasShield2; /*! status of player 2 shield */
    unsigned int score1; /*! score for the round of player 1 */
    unsigned int score2; /*! score for the round of player 2 */
    int shots_fired1; /*! shots fired for the round for player 1 */
    int shots_fired2; /*! shots fired for the round for player 1 */
    uint8_t id; /*! ID of game */
};

#define MAX_SHOTS 5 /*!maximum shots allowed per player */
char_object_t shots1[MAX_SHOTS];  /*!Keeps track of shots fired */
char_object_t shots2[MAX_SHOTS];  /*!Keeps track of shots fired */
struct shooty_t game;

/**
 * @brief Input from the keyboard in the form of a char
 *
 * function interprets the input of the keyboard and calls the correct function
 * based on the key pressed.
 * 'A', 'S', and 'D' are left, right, and shoot for player 1
 * '<', '>', and '?' are left, right, and shoot for player 2
 * if 'P' is pressed the game ends
 *
 * @param c character passed from keyboard
 *
 */
void Receiver1(uint8_t c);

/**
 *
 * @brief Initializes the game when $game shooty play is typed into the terminal window
 * does this by drawing all the necessary boxes and then waits for user input
 *
 * Draws a main map with a title
 * Draws side boxes for scores, shots fired, accuracy, and shield remaining
 * Displays how to play underneath map
 * Initializes game variables to their initial values
 * Places player 1 in lower left corner
 * Places Player 2 in upper right corner
 * Waits for input
 *
 */
void Play(void);

/**
 *
 * @brief displays how to play the game
 *
 */
void Help(void);

/**
 *
 * @brief moves the shot up the screen
 *
 * Checks to make sure shot can move up (not collide with boundary of map)
 * Erases location of old shot
 * Draws location of new shot
 *
 * @param o character used to represent the shot on screen
 *
 */
void MoveUpShot(char_object_t * o);

/**
 *
 * @brief moves the shot up the screen
 *
 * Opposite of MoveUpShot
 *
 * @param o character used to represent the shot on screen
 *
 */
void MoveDownShot(char_object_t * o);


/**
 *
 * @brief checks how many shots are on screen
 * Makes sure shooting can happen
 * Schedules the task to fire the shot
 * Initiates the first instance of the shot on screen
 * (player 1)
 *
 */
void Fire1(void);

/**
 *
 * @brief checks how many shots are on screen
 * Makes sure shooting can happen
 * Schedules the task to fire the shot
 * Initiates the first instance of the shot on screen
 * (player 2)
 *
 */
void Fire2(void);

/**
 *
 * @brief performs end of game tasks
 *
 * Deletes game screen and everything on it
 * Shows ascii art of game over
 * Display score boxes for:
 * Score
 * Shots Fired
 * Accuracy
 * These boxes are drawn one by one on the screen using a delay and sound effects
 * Unregisters receivers and sends terminal back to pre-game state
 *
 */
void GameOver(void);

/**
 *
 * @brief moves player 1 right while checking to see if it can
 *
 * Deletes Old location
 * Updates Game Variable
 * Loads in new location
 *
 */
void MoveRight1(void);

/**
 *
 * @brief moves player 1 left while checking to see if it can
 *
 * implementation opposite of move right
 *
 */
void MoveLeft1(void);

/**
 *
 * @brief moves player 2 right while checking to see if it can
 *
 * same implementation as move right but for player 2
 *
 */
void MoveRight2(void);

/**
 *
 * @brief moves player 2 left while checking to see if it can
 *
 * same implementation as move left for player 2
 *
 */
void MoveLeft2(void);

/**
 *
 * @brief checks to see if player 1 was hit by player 2's shot
 * if they were then player 2 scores a point
 *
 * Clears location where collision occured
 * Adds point to scoreboard for payer 2
 *
 * @param o character of player 2's shot for reference
 *
 */
void CheckCollisionPlayer1(char_object_t * o);

/**
 *
 * @brief checks to see if player 2 was hit by player 1's shot
 * if they were then player 1 scores a point
 *
 * Clears location where collision occurred
 * Adds points to scoreboard for player 1
 *
 * @param o character of player 1's shot for reference
 *
 */
void CheckCollisionPlayer2(char_object_t * o);

/**
 *
 * @brief adds a point to player 1's score board and checks to see if score >= 3
 *
 * If score gets to 3 the game should end
 * Otherwise add one to the score
 *
 */
void ScorePoint1(void);

/**
 *
 * @brief adds a point to player 2's score board
 *
 * Similar to ScorePoint1
 *
 */
void ScorePoint2(void);

/** @}*/

#endif /* TWO_PLAYER_SHOOTY_H_ */
