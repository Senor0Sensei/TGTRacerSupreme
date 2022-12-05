/**********************************/
/*  The Grand Tour Racer Supreme  */
/*     Start: 11 - 22 - 2022      */
/*      End: 12 - 04 - 2022       */
/**********************************/

// Code Written by Luke Murray & Christopher Coppel

// C++ Libraries

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <fstream>
#include <iostream>

using namespace std;

// FEH Proteus Libraries

#include "FEHLCD.h"
#include "FEHUtility.h"
#include "FEHFile.h"
#include "LCDColors.h"
#include "FEHImages.h"
#include "FEHRandom.h"

//Classes

class CharacterStats {
    
    public:

        CharacterStats();

        //Getters
        int getMaxScore(int character, int difficulty);
        int getkmDriven(int character, int difficulty);
        int getGamesPlayed(int character, int difficulty);

        //Setters
        void updateStats(int Score, int km, int character, int difficulty);

        //Save function
        void fileWrite();

    private:

        char charName[4][20];
        int maxScore[4][3];
        int kmDriven[4][3];
        int gamesPlayed[4][3];

};

class Car {
    
    public:

        Car();
        void reset();

    //Stat Functions

        //Live Updating

        void updateSelection(int, int);

        //Setters

        void setDifficulty(int);

        //Getters

        int getCharacter();
        int getDifficulty();

    //Car Position Functions

        //Live Updating

        void updatePosition();
        void updateVelocity();
        void updateAngle(int, int);

        //Setters

        void setXVel(int);
        void setxPos(int);

        //Getters

        float getXVel();
        float getYVel();
        float getVel();
        float getXPos();
        float getAngle();

    //Scoring Functions

        //Live Updating

        void updateScore();

        //Getters

        int getScore();
        int getKM();

    //Track Functions

        //Setters

        void SetTrackPositions(int, int, float);
        void setTrackX(int);

        //Getters

        int getStage();
        
        //Printing

        void printTrack(Car *, FEHImage *, FEHImage *);

    //Obstacle Functions

        //Setters

        void SetOpstacleData(int, int);
        void resetObstacleData(int);

        //Printing

        void printOpstacle(int, FEHImage *, int);

        //Death Check

        int checkDeath(int, int);

    private:

    // Difficulty & Character

    int character;
    int difficulty;

    // Car Position & Velocity
    
    float velocity;
    float x_vel;
    float y_vel;
    float x_pos;
    float y_pos;
    float angle;
    float hypoteneus;
    int onTrack;

    // Scoring and Data Collection

    int score;
    int km;
    float buffer;
    float liveKM;

    // Track Generation and HitBoxes

    float ry;
    float width;
    float slope;
    float x1, y1;
    float trackBound;
    float trackx[5];
    int trackType;
    int stage;

    //Obstacle Generations and HitBoxes

    int objectType[72];
    int obstacleType [72];
    int obstaclex[72];
    float obstacley[72];
    int obstaclexOffset[72];
    int obstacleyOffset[72];

};

// Function Prototypes
// Functions can Be Found Below int main()

// Intro Function

void dispLogo();
int dispTitleScreen();

// Menu Functions

int dispMainMenu();
int dispCharacterSelect(int *);
int dispDifficultySelect(Car *,int *);
int dispInfoMenu();
int dispStatMenu(CharacterStats *);
int dispCredits();

//Game Functions

int playGame(Car *, int, int);
int dispDeathScreen(Car *, CharacterStats *, int, int);

// Transition Functions

void menuTransition(Car *, CharacterStats *, int, int, int);

// Custom LCD Functions

void DrawTrack(int, int, int, int, Car *, FEHImage *, FEHImage *);

// Main Function

int main() {

//Code Written by: Luke Murray

/* The main function holds the classes created and the pointers to 
those classes that are passed through funtions.
It also controls what menu is displayed when, using a state machine 
to change the displayed menu (or cause the game to play)
depending on the imput of the player.*/

    // Introductory Menus
    dispLogo();
    dispTitleScreen();

    // Class Declarations
    CharacterStats Player, *ptrPlayer;
    Car UserCar, *ptrCar;

    // Defining the values of class pointers
    ptrCar = &UserCar;
    ptrPlayer = &Player;

    // Variable Declarations
    int state = 0, subState, prevState;
    int a = 0;      // Character Value
    int *b = &a;    // Pointer to Character Value
    int c = 0;      // Difficulty Value
    int *d = &c;    // Pointer to Difficulty Value

    /* While loop that checks the value of the state each time it runs. 
    If the state is 8 (which is produced by the Save and Quit Button) the application will force quit.
    Otherwise, the while loop will be executed, displaying a different menu depending on the state*/

    while (state != 8) {

        subState = 0; // Controls which menu will be displayed next.

        // If the new state being checked is different from the recorded value for the 
        // previous state, the menu transition function will be called before the menu changes.
        if (state != prevState) { menuTransition(ptrCar, ptrPlayer, state, a, c); } 

        // Sets the current state as the previous state.
        prevState = state;


        // Checks and compares the value of state to determine what menu function will be called.
        switch (state) {
            
            // Main menu display, calls the function that will display our main menu.
            case 0:
            state = dispMainMenu();
            break;

            // Character Selection Screen, calls the function that displays character selection menu.
            case 1:
            subState = dispCharacterSelect(b);
            if (subState == 1) { state = 0; }           // Go Back
            else if (subState == 2) { state = 2; }      // Go to Difficulty Selection
            break;

            // Difficulty Selection Screen, calls the function that displays difficulty selection menu.
            case 2:
            subState = dispDifficultySelect(ptrCar, d);
            if (subState == 1) { state = 1; }           // Go Back
            else if (subState == 2) { state = 3; }      // Start Game
            break;

            // Value for state that starts the game. Will call the play game 
            // function that handles all of the gameplay components of the code.
            case 3:
            state = playGame(ptrCar, a, c);  // Goes to Death Screen (case 4) once the player dies
            break;

            // Game Over/Death Screen, calls the function that displays the death screen, 
            // updates character stats, and saves stats to the save file.
            case 4: 
            Player.updateStats(a,c,UserCar.getScore(),UserCar.getKM());  // Updates the stats in Character Class to account for new data from the last played game.
            Player.fileWrite();  // Save all values in the Character Class to the save data.
            state = dispDeathScreen(ptrCar,ptrPlayer,a,c);  // Go to Main Menu when User clicks Next.
            break;

            // Help Screen, calls the function that displays the game's instructions.
            case 5:
            state = dispInfoMenu(); // Returns to Main Menu when User clicks back
            break;

            // Statistics Menu, calls the function that displays all the recorded stats for each character and difficulty.
            case 6:
            state = dispStatMenu(ptrPlayer); // Returns to Main Menu when User clicks back
            break;

            // Credits Menu, calls the function that displays the credits.
            case 7:
            state = dispCredits(); // Returns to Main Menu when User clicks back
            break;

            // Main Menu, an easter egg that will return the User to the Main menu screen if a certain button is pressed.
            case 9:
            state = dispTitleScreen();  // Easter Egg
            break;

        }

    }

    Sleep(500);  //Before closing the application, wait for half a second

}

// User Functions

void dispLogo() {

    // Code Written by Luke Murray

    /* Loads the Get Schwifty Logo, animates the logo, then closes the image after a set amount of time*/

    // Load the image
    FEHImage logo1;
    logo1.Open("logo_1.pic");

    Sleep(1000);  // Waits for one second before the animation begins

    // Image Animation

    for (int i = 2; i < 120; i+=2) {

        logo1.Draw(0,0,1);  //Draws logo

        // Prints black rectangle to give the appearence of a fade
        LCD.SetFontColor(BLACK); 
        LCD.FillRectangle(0,(i*2),320,240-(i*2),1);
        LCD.Update();

        // Delays for 1 millisecond.
        Sleep(1);

    }

    logo1.Draw(0,0,1);  // Redraws the logo

    Sleep(1500);        // Waits for 1.5 seconds

    logo1.Close();      // Closes the image file

}

int dispTitleScreen() {

    // Code Written by Luke Murray

    /* This Function is in charge of loading and animating the Title Screen, 
    and only ends when the User clicks on the Play Game button on the menu.*/

    // Variable Declarations for state, x and y click postitions, as well as any trashed x and y click postions
    int state = 1, x, y, xt, yt;

    // Loading the image file
    FEHImage title1;
    title1.Open("Title_1.pic");


    // Image Animation 
    for (int i = 2; i < 120; i+=4) {

        // Prints black rectangle to give the appearence of a fade
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(0,0,320,i*2,1);
        LCD.Update();

        Sleep(1);  // Waits for 1 millisecond

    }

    for (int i = 2; i < 120; i+=2) {

        title1.Draw(0,0,1); // Prints the title screen image

        // Prints black rectangle to give the appearence of a fade
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(0,(i*2),320,240-(i*2),1);
        LCD.Update();

        Sleep(1);  // Waits for 1 millisecond

    }

    // Prints the title screen image
    title1.Draw(0,0,1);
    LCD.Update();

    // Opens a second title screen image that has a play game function
    FEHImage title2;
    title2.Open("Title_2.pic");

    Sleep(1500); // Waits for 1.5 seconds

    // Prints the title screen with the play game function
    title2.Draw(0,0,1);
    LCD.Update();
    
    // Checks if the state is 0 or not, if it isn't, the while loop executes, 
    // checking for if the user clicks the Play Game button.
    while (state != 0) {

        // Checks if the user clicks on the screen, saving the click's location
        while (!LCD.Touch(&x, &y)) {}
        while (LCD.Touch(&xt, &yt)) {}

        // Checks if the location the User clicked was the location of the Play Game button, if it is, it changes the state to 0.
        if ((x > 130 and x < 191) and (y > 206 and y < 228)) {
            state = 0; // Go To Main Menu
        }

    }

    // Animates the Play Game button to flicker by switching back and forth between the 
    // two title screen images, waiting for a 100 milliseconds each time an image is drawn.
    title1.Draw(0,0,1);
    LCD.Update();
    Sleep(100);
    title2.Draw(0,0,1);
    LCD.Update();
    Sleep(100);
    title1.Draw(0,0,1);
    LCD.Update();
    Sleep(100);
    title2.Draw(0,0,1);
    LCD.Update();
    Sleep(100);
    title1.Draw(0,0,1);
    LCD.Update();
    Sleep(100);
    title2.Draw(0,0,1);
    LCD.Update();
    Sleep(100);

    // Closes the image files.
    title1.Close();
    title2.Close();

    // Returns the value of the variabls state.
    return state;

}

int dispMainMenu() {                // State 0

    // Code Written by Luke Murray

    /* This function prints the main menu and check for the location of anywhere the user clicks, changing the state depending on what button the user pressed. */

    // Loads the menu image file.
    FEHImage menu;
    menu.Open("Main_Menu_1.pic");

    // Clearing the LCD Display
    LCD.Clear();

    // Printing the menu.
    menu.Draw(0,0,1);
    LCD.Update();

    // Variable Declarations for state, x and y click postitions, as well as any trashed x and y click postions
    int state = 0, x, y, xt, yt;

    // Checks if the user clicks on the screen, saving the click's location
    while (!LCD.Touch(&x, &y)) {}
    while (LCD.Touch(&xt, &yt)) {}

    // Checks if click location is on the Play button, changin state to 1 if it is.
    if ((x > 19 and x < 48) and (y > 99 and y < 113)) { // Play
        state = 1; 
    }

    // Checks if click location is on the Help button, changin state to 5 if it is.
    if ((x > 285 and x < 312) and (y > 205 and y < 230)) { // Instructions
        state = 5; 
    }

    // Checks if click location is on the Statistics button, changin state to 6 if it is.
    if ((x > 19 and x < 86) and (y > 121 and y < 133)) { // Statistics
        state = 6;
    }

    // Checks if click location is on the Credits button, changin state to 7 if it is.
    if ((x > 19 and x < 70) and (y > 142 and y < 154)) { // Credits
        state = 7;
    }

    // Checks if click location is on the Save and Quit button, changin state to 8 if it is.
    if ((x > 19 and x < 96) and (y > 182 and y < 196)) { // Exit
        state = 8;
    }


    // Checks if click location is on the location of the easter egg, changing state to 9 if it is.
    if ((x > 48 and x < 108) and (y > 25 and y < 37)) { // Go Back to Title Screen Easter Egg
        state = 9; 
    }


    // Closes the image files
    menu.Close();

    //Returns the value of state
    return state;

}

int dispCharacterSelect(int *a) {   // State 1

    // Code Written by Luke Murray

    /* This function takes in a pointer to the character value varible 
    in INT MAIN, and displays the character select screen. 
    It also edits the displayed character images depending on which 
    character is selected, sending the character value back to INT MAIN once the user clicks next.*/
    
    // Loading All of the images

    FEHImage Char_0;
    Char_0.Open("Character_Select_0.pic");

    FEHImage Char_1;
    Char_1.Open("Character_Select_1.pic");

    FEHImage Char_2;
    Char_2.Open("Character_Select_2.pic");

    FEHImage Char_3;
    Char_3.Open("Character_Select_3.pic");

    FEHImage JC_1;
    JC_1.Open("JC_1.pic");

    FEHImage JC_2;
    JC_2.Open("JC_2.pic");

    FEHImage JM_1;
    JM_1.Open("JM_1.pic");

    FEHImage JM_2;
    JM_2.Open("JM_2.pic");

    FEHImage RH_1;
    RH_1.Open("RH_1.pic");

    FEHImage RH_2;
    RH_2.Open("RH_2.pic");

    //Clearing the LCD Display
    LCD.Clear();

    // Drawing the greyed out character portaits.
    Char_0.Draw(0,0,1);
    JC_1.Draw(52,76,1);
    JM_1.Draw(128,76,1);
    RH_1.Draw(204,76,1);

    // Drawing borders around the character portraits.
    LCD.SetFontColor(WHITE);
    LCD.DrawRectangle(52,76,64,64,1);
    LCD.DrawRectangle(128,76,64,64,1);
    LCD.DrawRectangle(204,76,64,64,1);
    LCD.Update();

    // Variable Declarations for character number, state, x and y click postitions, as well as any trashed x and y click postions
    int Character = 0;
    int state = 0;
    int x, y, xt, yt;

    // While the state is set to 0, all User inputs will be tracked, 
    // either changing the character number (changing the portrait of the character seclected)
    // or changing the state depending on the location selected
    while (state == 0) {

        // Checks if the user clicks on the screen, saving the click's location
        while (!LCD.Touch(&x, &y)) {}
        while (LCD.Touch(&xt, &yt)) {}

        // Checks if the first character is selected, changing character to 1 if it is
        if ((x > 52 and x < 130) and (y > 76 and y < 140)) {
            Character = 1; 
        }

        // Checks if the second character is selected, changing character to 2 if it is
        else if ((x > 128 and x < 192) and (y > 76 and y < 140)) {
            Character = 2; 
        }

        // Checks if the third character is selected, changing character to 3 if it is
        else if ((x > 204 and x < 268) and (y > 76 and y < 140)) {
            Character = 3;
        }

        // Checks if the back button is selected, changing state to 1 if it is
        else if ((x > 25 and x < 75) and (y > 203 and y < 219)) {
            state = 1; // Go Back to Main Menu
        }
        
        // Checks if the next button is selected, changing state to 2 if it is
        else if (((x > 250 and x < 307) and (y > 209 and y < 224)) and Character != 0) {
            state = 2; // Go to Difficulty Selection
        }

        // If no button was selected, character is set to 0
        else {
            Character = 0;
        }

        // Checks the Value of character, changing the portrait for the 
        // character selected, and displaing the character's name
        switch (Character) {

            case 0:

            //Clearing the LCD Display
            LCD.Clear();

            // Drawing the greyed out character portaits.
            Char_0.Draw(0,0,1);
            JC_1.Draw(52,76,1);
            JM_1.Draw(128,76,1);
            RH_1.Draw(204,76,1);

            // Drawing borders around the character portraits.
            LCD.SetFontColor(WHITE);
            LCD.DrawRectangle(52,76,64,64,1);
            LCD.DrawRectangle(128,76,64,64,1);
            LCD.DrawRectangle(204,76,64,64,1);
            LCD.Update();

            break;

            case 1:

            //Clearing the LCD Display
            LCD.Clear();

            // Diplaying character 1's name and changing their portrait.
            Char_1.Draw(0,0,1);
            JC_2.Draw(52,76,1);
            JM_1.Draw(128,76,1);
            RH_1.Draw(204,76,1);

            // Drawing borders around the character portraits.
            LCD.SetFontColor(YELLOW);
            LCD.DrawRectangle(52,76,64,64,1);
            LCD.SetFontColor(WHITE);
            LCD.DrawRectangle(128,76,64,64,1);
            LCD.DrawRectangle(204,76,64,64,1);
            LCD.Update();

            break;

            case 2:

            //Clearing the LCD Display
            LCD.Clear();

            // Diplaying character 2's name and changing their portrait.
            Char_2.Draw(0,0,1);
            JC_1.Draw(52,76,1);
            JM_2.Draw(128,76,1);
            RH_1.Draw(204,76,1);

            // Drawing borders around the character portraits.
            LCD.SetFontColor(WHITE);
            LCD.DrawRectangle(52,76,64,64,1);
            LCD.SetFontColor(YELLOW);
            LCD.DrawRectangle(128,76,64,64,1);
            LCD.SetFontColor(WHITE);
            LCD.DrawRectangle(204,76,64,64,1);
            LCD.Update();

            break;

            case 3:

            //Clearing the LCD Display
            LCD.Clear();

            // Diplaying character 3's name and changing their portrait.
            Char_3.Draw(0,0,1);
            JC_1.Draw(52,76,1);
            JM_1.Draw(128,76,1);
            RH_2.Draw(204,76,1);

            // Drawing borders around the character portraits.
            LCD.SetFontColor(WHITE);
            LCD.DrawRectangle(52,76,64,64,1);
            LCD.DrawRectangle(128,76,64,64,1);
            LCD.SetFontColor(YELLOW);
            LCD.DrawRectangle(204,76,64,64,1);
            LCD.SetFontColor(WHITE);
            LCD.Update();

        }

    }

    // Closing the image files
    Char_0.Close();
    Char_1.Close();
    Char_2.Close();
    Char_3.Close();

    JM_1.Close();
    JM_2.Close();
    RH_1.Close();
    RH_2.Close();
    JC_1.Close();
    JC_2.Close();

    // Change the character nnumber in int main to the value of Character
    *a = Character;

    // Returns the value of state
    return state;

}

int dispDifficultySelect(Car* UserCar, int *a) {  // State 2

    // Code Written by Luke Murray

    /* This function takes in a pointer to the difficulty value variable
    and a pointer to the Class Car in INT MAIN, 
    and displays the difficulty select screen,
    editing the value of difficulty depending on User selection and
    sending the difficulty value back to INT MAIN and the class once the user clicks next.*/
    
    // Loading All of the images

    FEHImage Diff_0;
    Diff_0.Open("Difficulty_Select_0.pic");

    FEHImage Diff_1;
    Diff_1.Open("Difficulty_Select_1.pic");

    FEHImage Diff_2;
    Diff_2.Open("Difficulty_Select_2.pic");

    FEHImage Diff_3;
    Diff_3.Open("Difficulty_Select_3.pic");

    FEHImage Diff_4;
    Diff_4.Open("Difficulty_Select_4.pic");

    // Clearing the LCD
    LCD.Clear();

    // Drawing the menu
    Diff_0.Draw(0,0,1);
    LCD.Update();

     // Variable Declarations for difficulty number, state, x and y click postitions, as well as any trashed x and y click postions
    int Difficulty = 0;
    int state = 0;
    int x, y, xt, yt;

    // While the state is 0, the function will check for user input, and change the difficulty or state depending on the area selected 
    while (state == 0) {

        // Checks if the user clicks on the screen, saving the click's location
        while (!LCD.Touch(&x, &y)) {}
        while (LCD.Touch(&xt, &yt)) {}

        // If the Hard button is selected changes the difficulty to 1.
        if ((x >= 10 and x <= 69) and (y >= 62 and y <= 76)) {
            Difficulty = 1;
            *a = 1; // Sends a difficulty value of 1 back to MAIN
        }

        // If the Hard+ button is selected changes the difficulty to 2.
        else if ((x >= 10 and x <= 81) and (y >= 87 and y <= 102)) {
            Difficulty = 2;
            *a = 2; // Sends a difficulty value of 2 back to MAIN
        }

        // If the Hard++ button is selected changes the difficulty to 3.
        else if ((x >= 10 and x <= 95) and (y >= 114 and y <= 129)) {
            Difficulty = 3;
            *a = 3; // Sends a difficulty value of 3 back to MAIN
        }

        // If the Grand Tour (Free Play) button is selected changes the difficulty to 4.
        else if ((x >= 10 and x <= 191) and (y >= 141 and y <= 156)) {
            Difficulty = 4;
            *a = 0; // Sends a difficulty value of 0 back to MAIN
        }

        // If the back button is selected changes the state to 1.
        else if ((x >= 10 and x <= 75) and (y >= 209 and y <= 224)) {
            state = 1; // Go Back to Character Select
        }
        
        // If the next button is selected, and a difficulty has been chosen, changes the state to 2.
        else if (((x >= 250 and x <= 307) and (y >= 209 and y <= 224)) and Difficulty != 0) {
            state = 2; // Play Game
        }

        // If anywhere else is clicked, difficulty is set to 0
        else {
            Difficulty = 0;
        }

        // Checks the value to difficulty, changing the screen's appearance based on its value
        switch (Difficulty) {

            //When difficulty is 0
            case 0:

            // Clears the LCD display
            LCD.Clear();

            // Draws menu version 1
            Diff_0.Draw(0,0,1);
            LCD.Update();

            break;

            //When difficulty is 1
            case 1:

            // Clears the LCD display
            LCD.Clear();

            // Draws menu version 2
            Diff_1.Draw(0,0,1);
            LCD.Update();

            break;

            //When difficulty is 2
            case 2:

            // Clears the LCD display
            LCD.Clear();

            // Draws menu version 3
            Diff_2.Draw(0,0,1);
            LCD.Update();

            break;

            //When difficulty is 3
            case 3:

            // Clears the LCD display
            LCD.Clear();

            // Draws menu version 4
            Diff_3.Draw(0,0,1);
            LCD.Update();

            break;

            //When difficulty is 4
            case 4:

            // Clears the LCD display
            LCD.Clear();

            // Draws menu version 5
            Diff_4.Draw(0,0,1);
            LCD.Update();

        }

    }

    // Sets the difficulty value in Class Car to the value stroed in the adress of a
    UserCar->setDifficulty(*a);

    // Returns the value of state.
    return state;

}

int playGame(Car *UserCar, int character, int difficulty) {                    // State 3

    // Code Written by Christopher Coppel and Luke Murray

    /* This function takes in a pointer to the class car, the character number, and the difficulty
    number, using these values to start the game. Within this function lies the printing commands for the HUD
    and every function that has an effect on gameplay.*/
    
    // Loading All of the images

    FEHImage Background_1;
    Background_1.Open("Background_1.pic");

    FEHImage Background_2;
    Background_2.Open("Background_2.pic");
    
    FEHImage Background_3;
    Background_3.Open("Background_3.pic");

    FEHImage Background_4;
    Background_4.Open("Background_4.pic");

    FEHImage Background_5;
    Background_5.Open("Background_5.pic");

    FEHImage Background_6;
    Background_6.Open("Background_6.pic");

    FEHImage CarAtlas;
    CarAtlas.Open("Cars.pic");

    FEHImage StageAtlas;
    StageAtlas.Open("StageAtlas.pic");

    FEHImage TrackImg;
    TrackImg.Open("Track.pic");

    FEHImage *Track;
    Track = &TrackImg;

    FEHImage Stage_6;
    Stage_6.Open("Stage_6.pic");

    FEHImage *Lava;
    Lava = &Stage_6;

    FEHImage TGT;
    TGT.Open("TGT.pic");

    FEHImage Objects;
    Objects.Open("ObjectAtlas.pic");

    FEHImage *ObjectAtlas;
    ObjectAtlas = &Objects;

    FEHImage SpedometerAtlas;
    SpedometerAtlas.Open("SpedometerAtlas.pic");
    
    // Declaring Variables
    int state = 0; 
    int x, y, xt, yt, xClick, yClick;
    float buffer; // Used for anything that needs a temporary variable
    int atlas = 5; // Which car/spedometer image will be printed
    int Dead = 1; // If the player is dead or not
    int ObstacleCount = 2; // How many obstacles are in play at that moment
    bool Press = false; // Whether the user is clicking or not

    // Previous stage
    int prev;

    // Finds the correct car to display
    character *= 32;

    // Resets the data the Car and obstacle data
    UserCar->reset();
    for (int i = 0; i < 12; i++) { UserCar->resetObstacleData(i); }

    // Sets initial track x postions
    UserCar->setTrackX(1);
    UserCar->setTrackX(2);
    UserCar->setTrackX(3);
    UserCar->setTrackX(4);

    // Sets the initial obstacle postions
    for (int i = 0; i < ObstacleCount; i++) { UserCar->SetOpstacleData(i, difficulty); }

    // Draws an image of the car that depends on its angle in the cars x position.
    CarAtlas.DrawAtlas(UserCar->getXPos()-16,184,5,character,32,32,0);
    
    // While the state is not 1
    while (state != 1) {

        // Set the initial click postion (used for a starting velocity)
        x = 160;
        y = 100;

        // Used for lava effects
        float ry = 0;
        
        // While the player has not lost the game
        while (Dead == 1) {

            // Press is set to false 
            Press = false;
            
            // Set the click coordinates to x and y
            xClick = x;
            yClick = y;

            // Update the angle and velocity of the car
            UserCar->updateAngle(xClick,yClick);
            UserCar->updateVelocity();

            // While press is false
            while (!Press) {

                //Update the car's position and score
                UserCar->updatePosition();
                UserCar->updateScore();

                // Checks if car is past the leftside boundary, stopping it from moving left if it is
                if (UserCar->getXPos() <= 16 and UserCar->getXVel() > 0) {
                    
                    UserCar->setxPos(16);
                    UserCar->setXVel(0);

                }

                // Checks if car is past the rightside boundary, stopping it from moving right if it is
                else if (UserCar->getXPos() >= 304 and UserCar->getXVel() < 0) {
                    
                    UserCar->setxPos(304);
                    UserCar->setXVel(0);

                }

                // Reduces Background Speed if Stage is 6 
                // to make Lava Look like it is Farther Away

                if (UserCar->getStage() != 6) { ry += UserCar->getYVel(); }
                else { ry += (UserCar->getYVel() * .4); }
                if (ry > 240) { ry = 0; }

                // Clears the LCD Display
                LCD.Clear();

                // Print Background Based on Stage
                if (UserCar->getStage() == 1) { Background_1.Draw(0,ry,0); }
                else if (UserCar->getStage() == 2) { Background_2.Draw(0,ry,0); }
                else if (UserCar->getStage() == 3) { Background_3.Draw(0,ry,0); }
                else if (UserCar->getStage() == 4) { Background_4.Draw(0,ry,0); }
                else if (UserCar->getStage() == 5) { Background_5.Draw(0,ry,0); }
                else if (UserCar->getStage() == 6) { Background_6.Draw(0,ry,0); }

                // Draw Track
                UserCar->printTrack(UserCar, Track, Lava);

                // Draw Obstacles
                for (int i = 0; i < ObstacleCount; i++) { UserCar->printOpstacle(i, ObjectAtlas, difficulty); }

                // Choose Which Car to Draw
                if (UserCar->getAngle() >= 0 and UserCar->getAngle() < 0.499798) { atlas = 0; }
                else if (UserCar->getAngle() >= 0.499798000 and UserCar->getAngle() < 0.737797360) { atlas = 1; }
                else if (UserCar->getAngle() >= 0.737797360 and UserCar->getAngle() < 0.975796500) { atlas = 2; }
                else if (UserCar->getAngle() >= 0.975796500 and UserCar->getAngle() < 1.213795727) { atlas = 3; }
                else if (UserCar->getAngle() >= 1.213795727 and UserCar->getAngle() < 1.451794909) { atlas = 4; }
                else if (UserCar->getAngle() >= 1.451794909 and UserCar->getAngle() <= 3.14159/2.0) { atlas = 5; }
                else if (abs(UserCar->getAngle()) > 4.522857143 and abs(UserCar->getAngle()) <= 5) { atlas = 5; }
                else if (UserCar->getAngle() >= -4.522857143 and UserCar->getAngle() < -4.335714286) { atlas = 6; }
                else if (UserCar->getAngle() >= -4.335714286 and UserCar->getAngle() < -4.148571429) { atlas = 7; }
                else if (UserCar->getAngle() >= -4.148571429 and UserCar->getAngle() < -3.961428572) { atlas = 8; }
                else if (UserCar->getAngle() >= -3.961428572 and UserCar->getAngle() < -3.774285715) { atlas = 9; }
                else if (UserCar->getAngle() >= -3.774285715 and UserCar->getAngle() < -3.4) { atlas = 10; }
                else { atlas = 5; }

                atlas *= 32;

                // Draw Car Based on Angle Selection, Character, Difficulty, and Stage

                CarAtlas.DrawAtlas(UserCar->getXPos()-16,184,atlas,character,32,32,0);

                // Draw Blank HUD
                LCD.SetFontColor(BLACK);
                LCD.FillRectangle(4,4,312,26,1);
                LCD.SetFontColor(WHITE);
                LCD.DrawRectangle(4,4,312,26,1);

                // Checks if the player isn't playing in Grand Tour mode (0)
                if (difficulty != 0) {

                    // Print Stage Based on Score
                    StageAtlas.DrawAtlas(10,10,0,(UserCar->getStage()-1)*16,69,16,1); 

                    // Print Current Score (check number of digits in the score, and adds 0's before hand to make the score have 8 digits)
                    LCD.SetFontColor(WHITE);
                    if (UserCar->getScore() == 0) {LCD.WriteAt("  00000000",190,10,1); }
                    else if (UserCar->getScore() < 10) { LCD.WriteAt("  0000000",190,10,1); LCD.WriteAt(UserCar->getScore(),298,10,1); }
                    else if (UserCar->getScore() < 100) { LCD.WriteAt("  000000",190,10,1); LCD.WriteAt(UserCar->getScore(),286,10,1); }
                    else if (UserCar->getScore() < 1000) { LCD.WriteAt("  00000",190,10,1); LCD.WriteAt(UserCar->getScore(),274,10,1); }
                    else if (UserCar->getScore() < 10000) { LCD.WriteAt("  0000",190,10,1); LCD.WriteAt(UserCar->getScore(),262,10,1); }
                    else if (UserCar->getScore() < 100000) { LCD.WriteAt("  000",190,10,1); LCD.WriteAt(UserCar->getScore(),250,10,1); }
                    else if (UserCar->getScore() < 1000000) { LCD.WriteAt("  00",190,10,1); LCD.WriteAt(UserCar->getScore(),238,10,1); }
                    else if (UserCar->getScore() < 9999999) { LCD.WriteAt("  0",190,10,1); LCD.WriteAt(UserCar->getScore(),226,10,1); }
                    else {LCD.WriteAt("  99999999",190,10,1); }

                } 
                
                // Draw the HUD design for Grand Tour mode
                else { TGT.Draw(5,5,1); }

                // Determines Which Spedometer Image to Draw Based on Speed

                atlas = 0;
                buffer = .6153846;

                if (UserCar->getVel() < buffer) { atlas = 0;}
                else if (UserCar->getVel() < buffer*2) { atlas = 1;}
                else if (UserCar->getVel() < buffer*3) { atlas = 2;}
                else if (UserCar->getVel() < buffer*4) { atlas = 3;}
                else if (UserCar->getVel() < buffer*5) { atlas = 4;}
                else if (UserCar->getVel() < buffer*6) { atlas = 5;}
                else if (UserCar->getVel() < buffer*7) { atlas = 6;}
                else if (UserCar->getVel() < buffer*8) { atlas = 7;}
                else if (UserCar->getVel() < buffer*9) { atlas = 8;}
                else if (UserCar->getVel() < buffer*10) { atlas = 9;}
                else if (UserCar->getVel() < buffer*11) { atlas = 10;}
                else if (UserCar->getVel() < buffer*12) { atlas = 11;}
                else if (UserCar->getVel() < 8.1) { atlas = 12;}

                atlas *= 64;

                printf("%f\n", UserCar->getVel());

                // Draws an image of the spedometer
                SpedometerAtlas.DrawAtlas(5,171,atlas,0,64,64,1);

                // Update the LCD display
                LCD.Update();

                // If grand tour mode is selected and the quit button is pressed, go to the main menu
                if ( difficulty == 0 ) {
                    if (xClick > 272 and xClick < 308 and yClick > 10 and yClick < 23) { return 0; }
                }

                // Check for death
                Dead = UserCar->checkDeath(ObstacleCount,1);

                // Sets the base obstacle count
                ObstacleCount = 4;

                // If the stage isn't 6, multiply set obstacle count to be obstcale count times difficulty time stage number
                if (UserCar->getStage() != 6) { ObstacleCount *= (difficulty * UserCar->getStage()); }
                
                // If stage is 6, set obstacle count to 0
                else { ObstacleCount = 0; }

                // If Grand Tour mode is selected, and stage isn't 6 obstacle count becomes 8
                if (difficulty == 0 and UserCar->getStage() != 6) { ObstacleCount = 8; }
                
                // If the LCD display is help down and Press is false, Press becomes true
                if (LCD.Touch(&x,&y) and !Press) { Press = true; } 
                
                // If the LCD display is released and Press is true, Press becomes false
                else if (!LCD.Touch(&x,&y) and Press) {Press = false;}
                
                // Wait for 25 milliseconds
                Sleep(25);

            }

        }

        // Close Images

        Background_1.Close();
        Background_2.Close();
        Background_3.Close();
        Background_4.Close();
        Background_5.Close();
        Background_6.Close();
        CarAtlas.Close();
        StageAtlas.Close();
        TrackImg.Close();
        Track->Close();
        Stage_6.Close();
        Lava->Close();
        TGT.Close();
        ObjectAtlas->Close();
        Objects.Close();
        SpedometerAtlas.Close();

        // Return that the new state is 4
        return 4;

    }

}

int dispDeathScreen(Car *UserCar, CharacterStats *Player, int i, int j) {            // State 4
    
    // Code Written by Luke Murray

    /* This function takes in a pointer to the class ChacracterStats Player
    and a pointer to the Class Car in INT MAIN, as well as the character number
    and the difficulty number. It uses these to display the death screen
    with your score and whether or not this was a new record */
    
    // Loading All of the images

    FEHImage Death0;
    Death0.Open("Death_Screen_0.pic");

    FEHImage Death1;
    Death1.Open("Death_Screen_1.pic");

    FEHImage Death2;
    Death2.Open("Death_Screen_2.pic");

    FEHImage Death3;
    Death3.Open("Death_Screen_3.pic");

    // Variable declarations for the x and y pixel where score is displayed
    int px = 124, py = 75;
    
    // Checks if the score obtained during the game played is lower than the recorded high score for that character and mode.
    if (UserCar->getScore() < Player->getMaxScore(i,j)) {

        // Clears the LCD Display
        LCD.Clear();

        // Draws the menu.
        Death0.Draw(0,0,1);

        // Sets font color to white
        LCD.SetFontColor(WHITE);

        // Checks how many digits your score has and displays the score with as many 0's in front of it to make a 8 digit number
        if (UserCar->getScore() == 0) {LCD.WriteAt("  00000000",px,py,1); }
        else if (UserCar->getScore() < 10) { LCD.WriteAt("  0000000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+108,py,1); }
        else if (UserCar->getScore() < 100) { LCD.WriteAt("  000000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+96,py,1); }
        else if (UserCar->getScore() < 1000) { LCD.WriteAt("  00000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+84,py,1); }
        else if (UserCar->getScore() < 10000) { LCD.WriteAt("  0000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+72,py,1); }
        else if (UserCar->getScore() < 100000) { LCD.WriteAt("  000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+60,py,1); }
        else if (UserCar->getScore() < 1000000) { LCD.WriteAt("  00",px,py,1); LCD.WriteAt(UserCar->getScore(),px+48,py,1); }
        else if (UserCar->getScore() < 9999999) { LCD.WriteAt("  0",px,py,1); LCD.WriteAt(UserCar->getScore(),px+36,py,1); }
        else {LCD.WriteAt("  99999999",px,py,1); }
        LCD.Update();

        Sleep(2000); //Waits for 2 seconds

        //Clears the LCD Display
        LCD.Clear();

        //Draws the menu with a next button
        Death1.Draw(0,0,1);

        // Sets font color to white
        LCD.SetFontColor(WHITE);

        // Checks how many digits your score has and displays the score with as many 0's in front of it to make a 8 digit number
        if (UserCar->getScore() == 0) {LCD.WriteAt("  00000000",px,py,1); }
        else if (UserCar->getScore() < 10) { LCD.WriteAt("  0000000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+108,py,1); }
        else if (UserCar->getScore() < 100) { LCD.WriteAt("  000000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+96,py,1); }
        else if (UserCar->getScore() < 1000) { LCD.WriteAt("  00000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+84,py,1); }
        else if (UserCar->getScore() < 10000) { LCD.WriteAt("  0000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+72,py,1); }
        else if (UserCar->getScore() < 100000) { LCD.WriteAt("  000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+60,py,1); }
        else if (UserCar->getScore() < 1000000) { LCD.WriteAt("  00",px,py,1); LCD.WriteAt(UserCar->getScore(),px+48,py,1); }
        else if (UserCar->getScore() < 9999999) { LCD.WriteAt("  0",px,py,1); LCD.WriteAt(UserCar->getScore(),px+36,py,1); }
        else {LCD.WriteAt("  99999999",px,py,1); }
        LCD.Update();

    }

    // Checks if the score obtained during the game played is greater than the recorded high score for that character and mode.
    else {

        // Clears the LCD Display
        LCD.Clear();

        // Draws the menu.
        Death0.Draw(0,0,1);

        // Sets font color to white
        LCD.SetFontColor(WHITE);

        // Checks if the score obtained during the game played is lower than the recorded high score for that character and mode.
        if (UserCar->getScore() == 0) {LCD.WriteAt("  00000000",px,py,1); }
        else if (UserCar->getScore() < 10) { LCD.WriteAt("  0000000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+108,py,1); }
        else if (UserCar->getScore() < 100) { LCD.WriteAt("  000000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+96,py,1); }
        else if (UserCar->getScore() < 1000) { LCD.WriteAt("  00000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+84,py,1); }
        else if (UserCar->getScore() < 10000) { LCD.WriteAt("  0000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+72,py,1); }
        else if (UserCar->getScore() < 100000) { LCD.WriteAt("  000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+60,py,1); }
        else if (UserCar->getScore() < 1000000) { LCD.WriteAt("  00",px,py,1); LCD.WriteAt(UserCar->getScore(),px+48,py,1); }
        else if (UserCar->getScore() < 9999999) { LCD.WriteAt("  0",px,py,1); LCD.WriteAt(UserCar->getScore(),px+36,py,1); }
        else {LCD.WriteAt("  99999999",px,py,1); }
        LCD.Update();

        Sleep(1.0); // Waits for 1 seconds

        // Draws the menu with HIGH SCORE printed
        Death2.Draw(0,0,1);

        // Sets font color to white
        LCD.SetFontColor(WHITE);

        // Checks if the score obtained during the game played is lower than the recorded high score for that character and mode.
        if (UserCar->getScore() == 0) {LCD.WriteAt("  00000000",px,py,1); }
        else if (UserCar->getScore() < 10) { LCD.WriteAt("  0000000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+108,py,1); }
        else if (UserCar->getScore() < 100) { LCD.WriteAt("  000000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+96,py,1); }
        else if (UserCar->getScore() < 1000) { LCD.WriteAt("  00000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+84,py,1); }
        else if (UserCar->getScore() < 10000) { LCD.WriteAt("  0000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+72,py,1); }
        else if (UserCar->getScore() < 100000) { LCD.WriteAt("  000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+60,py,1); }
        else if (UserCar->getScore() < 1000000) { LCD.WriteAt("  00",px,py,1); LCD.WriteAt(UserCar->getScore(),px+48,py,1); }
        else if (UserCar->getScore() < 9999999) { LCD.WriteAt("  0",px,py,1); LCD.WriteAt(UserCar->getScore(),px+36,py,1); }
        else {LCD.WriteAt("  99999999",px,py,1); }
        LCD.Update();

        Sleep(2.0); // Waits for 2 seconds

        // Clears the LCD Display
        LCD.Clear();

        // Draws the menu with HIGH SCORE printed and a next button
        Death3.Draw(0,0,1);

        // Sets font color to white
        LCD.SetFontColor(WHITE);

        // Checks if the score obtained during the game played is lower than the recorded high score for that character and mode.
        if (UserCar->getScore() == 0) {LCD.WriteAt("  00000000",px,py,1); }
        else if (UserCar->getScore() < 10) { LCD.WriteAt("  0000000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+108,py,1); }
        else if (UserCar->getScore() < 100) { LCD.WriteAt("  000000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+96,py,1); }
        else if (UserCar->getScore() < 1000) { LCD.WriteAt("  00000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+84,py,1); }
        else if (UserCar->getScore() < 10000) { LCD.WriteAt("  0000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+72,py,1); }
        else if (UserCar->getScore() < 100000) { LCD.WriteAt("  000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+60,py,1); }
        else if (UserCar->getScore() < 1000000) { LCD.WriteAt("  00",px,py,1); LCD.WriteAt(UserCar->getScore(),px+48,py,1); }
        else if (UserCar->getScore() < 9999999) { LCD.WriteAt("  0",px,py,1); LCD.WriteAt(UserCar->getScore(),px+36,py,1); }
        else {LCD.WriteAt("  99999999",px,py,1); }
        LCD.Update();

    }

    // Variable Declarations for state, x and y click postitions, as well as any trashed x and y click postions
    int state = 1;
    int x, y, xt, yt;

    // While the state is not 0 checks if the next button has been clicked or not, changing the state to 0 if it has.
    while (state != 0) {

        // Checks if the user clicks on the screen, saving the click's location
        while (!LCD.Touch(&x, &y)) {}
        while (LCD.Touch(&xt, &yt)) {}

        // If next button was clicked, changes state to 0
        if ((x > 250 and x < 307) and (y > 209 and y < 224)) {
            state = 0; // Go To Main Menu
        }

    }

    // Close Images

    Death0.Close();
    Death1.Close();
    Death2.Close();
    Death3.Close();

    // returns the value of state.
    return state;

}

int dispInfoMenu() {                // State 5

    // Code Written by Luke Murray

    /* This Function is in charge of loading the Instructions Screen, 
    and only ends when the User clicks on the back button on the menu.*/

    // Loading the image
    FEHImage instructions;
    instructions.Open("Instructions_1.pic");

    // Clearing the LCD
    LCD.Clear();

    // Drawing the menu.
    instructions.Draw(0,0,1);
    LCD.Update();

    // Variable Declarations for state, x and y click postitions, as well as any trashed x and y click postions
    int state = 1;
    int x, y, xt, yt;

    // While the state isn't 0, checks for user input and changes state to 0 if the back button was pressed
    while (state != 0) {

        // Checks if the user clicks on the screen, saving the click's location
        while (!LCD.Touch(&x, &y)) {}
        while (LCD.Touch(&xt, &yt)) {}

        // If the back button was clicked, change state to 0
        if ((x > 13 and x < 75) and (y > 206 and y < 223)) {
            state = 0; // Go Back to Main Menu
        }

    }

    // Closes the image files
    instructions.Close();

    // Return the value of state
    return state;

}

int dispStatMenu(CharacterStats *Player) {                // State 6
    
    // Code Written by Luke Murray

    /* This Function is in charge of loading the Statistics Screen, which displays
    the high score, total km driven, and games played for each character in each
    difficulty. The function takes in a pointer to the class CharacterStats Player to access save data.
    It ends when the User clicks on the back button on the menu.*/

    // Loading the images
    FEHImage stats;
    stats.Open("Statistics.pic");

    FEHImage statsD1;
    statsD1.Open("Stats_D_1.pic");

    FEHImage statsD2;
    statsD2.Open("Stats_D_2.pic");

    FEHImage statsD3;
    statsD3.Open("Stats_D_3.pic");

    FEHImage statsC1;
    statsC1.Open("Stats_C_1.pic");

    FEHImage statsC2;
    statsC2.Open("Stats_C_2.pic");

    FEHImage statsC3;
    statsC3.Open("Stats_C_3.pic");

    FEHImage Arrow_1;
    Arrow_1.Open("Arrow_1.pic");

    FEHImage Arrow_2;
    Arrow_2.Open("Arrow_2.pic");

    // Declaring Position and spacing variables for printing.
    int x = 32;
    int y = 112;
    int spacing = 28;

    // Clearing the LCD Display
    LCD.Clear();

    // Sets the Font color to white
    LCD.SetFontColor(WHITE);

    // Prints header text for High score, km driven, and race count with space between each
    LCD.WriteAt("High Score:",x,y,1);
    y += spacing;
    LCD.WriteAt(" Km Driven:",x,y,1);
    y += spacing;
    LCD.WriteAt("# of Races:",x,y,1);

    // Draws character 1's name and that difficulty 1 is selected
    statsC1.Draw(25,40,1);
    statsD1.Draw(25,72,1);

    // Draws the stats menu frame
    stats.Draw(0,0,1);
    LCD.Update();

    // Variable Declarations for state, character number, difficulty number, and any trashed x and y click postions
    int state = 1;
    int xt, yt;
    int character = 0, difficulty = 1;

    // While the state isn't 0, the stats for the character and difficulty value
    // will be printed, and a different menu screen will be shown depending on the difficulty
    // and the character.
    while (state != 0) {
        
        // Sets Position
        x = 32;
        y = 112;

        // Clearing the LCD Display
        LCD.Clear();

        // Sets the Font color to white
        LCD.SetFontColor(WHITE);

        // Prints header text for High score, km driven, and race count with space between each
        LCD.WriteAt("High Score:",x,y,1);
        y += spacing;
        LCD.WriteAt(" Km Driven:",x,y,1);
        y += spacing;
        LCD.WriteAt("# of Races:",x,y,1);

        // Sets the Font color to white
        LCD.SetFontColor(WHITE);

        // Prints the score, km driven, and race count for the given difficulty and character.
        LCD.WriteAt(Player->getMaxScore(character+1,difficulty-1),x + 144,112,1);
        LCD.WriteAt(Player->getkmDriven(character+1,difficulty-1),x + 144,140,1);
        LCD.WriteAt(Player->getGamesPlayed(character+1,difficulty-1),x + 144,168,1);

        // Checks the value of character, displaying a different screen depending on its value
        switch (character) {

            // If character is 0 print name of character 1
            case 0:

                statsC1.Draw(25,40,1);

            break;

            // If character is 1 print name of character 2
            case 1:

                statsC2.Draw(25,40,1);

                break;

            // If character is 2 print name of character 3
            case 2:

                statsC3.Draw(23,40,1);

            break;

        }

        // Checks the value of difficulty, displaying a different screen depending on its value
        switch (difficulty) {

            // If difficulty is 1, display Hard difficulty menu
            case 1:

                statsD1.Draw(25,72,1);

            break;

            // If difficulty is 2, display Hard+ difficulty menu
            case 2:

                statsD2.Draw(25,72,1);

            break;

            // If difficulty is 3, display Hard++ difficulty menu
            case 3:

                statsD3.Draw(25,72,1);
                
            break;

        }

        // Displays images of the arrow buttons
        Arrow_1.Draw(30,45,1);
        Arrow_2.Draw(269,45,1);

        // Draws the menu frame
        stats.Draw(0,0,1);
        LCD.Update();

        // Checks if the user clicks on the screen, saving the click's location
        while (!LCD.Touch(&x, &y)) {}
        while (LCD.Touch(&xt, &yt)) {}

        // If the user hits the back button, change staet to 0
        if ((x > 13 and x < 75) and (y > 206 and y < 223)) {
            state = 0; // Go Back to Main Menu
        }

        // If the user hits the left arrow, reduce character by 1
        if ((x > 30 and x < 52) and (y > 45 and y < 67)) {
            character -= 1; // Go to Next Character 
        }

        // If the user hits the right arrow, increase character by 1
        if ((x > 269 and x < 288) and (y > 45 and y < 67)) {
            character += 1; // Go to Previous Character
        }

        // If the user hits the Hard button, set difficulty to 1
        if ((x > 25 and x < 105) and (y > 72 and y < 104)) {
            difficulty = 1; // Go to Hard Difficulty
        }

        // If the user hits the Hard+ button, set difficulty to 2
        if ((x > 105 and x < 195) and (y > 72 and y < 104)) {
            difficulty = 2; // Go to Hard+ Difficulty
        }

        // If the user hits the Hard++ button, set difficulty to 3
        if ((x > 195 and x < 293) and (y > 72 and y < 104)) {
            difficulty = 3; // Go to Hard++ Difficulty
        }

        // Set character to either -1, 0, 1 , or 2
        character %= 3;

        // Makes the value -1 into a 2
        if ( character == -1 ) { character = 2; }

    }

    // Closes the image files
    stats.Close();
    statsD1.Close();
    statsD2.Close();
    statsD3.Close();
    statsC1.Close();
    statsC2.Close();
    statsC3.Close();
    Arrow_1.Close();
    Arrow_2.Close();

    // Returns the value of state
    return state;

}

int dispCredits() {                 // State 7

    // Code Written by Luke Murray

    /* This function prints the credits and checks for the location of anywhere the user clicks, changing the state depending on what button the user pressed. */

    // Loads the menu image file.
    FEHImage credits;
    credits.Open("Credits_1.pic");

    // Clears the LCD Dipslay
    LCD.Clear();

    // Prints the menu
    credits.Draw(0,0,1);
    LCD.Update();

    // Variable Declarations for state, x and y click postitions, as well as any trashed x and y click postions
    int state = 1;
    int x, y, xt, yt;

    // While the state is not 0, checks if the user clicks the back button, if they do sets state to 0.
    while (state != 0) {

        // Checks if the user clicks on the screen, saving the click's location
        while (!LCD.Touch(&x, &y)) {}
        while (LCD.Touch(&xt, &yt)) {}

        // If the back button was clicked, set state to 0.
        if ((x > 13 and x < 75) and (y > 206 and y < 223)) {
            state = 0; // Go Back to Main Menu
        }

    }

    // Closes the image file
    credits.Close();

    // Returns the value of state
    return state;

}

void menuTransition(Car *UserCar, CharacterStats *Player, int a, int c, int d) {

    // Code Written by Luke Murray

    /* This function animates the transtion between menus by taking in both classes, 
    the current sate (a), the character (c), and the difficulty (d) */

    // Loads the menu image file.
    FEHImage menu;
    menu.Open("Main_Menu_1.pic");

    FEHImage character;
    character.Open("Character_Select_0.pic");

    FEHImage JC;
    JC.Open("JC_1.pic");

    FEHImage JM;
    JM.Open("JM_1.pic");

    FEHImage RH;
    RH.Open("RH_1.pic");

    FEHImage difficulty;
    difficulty.Open("Difficulty_Select_0.pic");

    FEHImage death;
    death.Open("Death_Screen_0.pic");

    FEHImage instructions;
    instructions.Open("Instructions_1.pic");

    FEHImage stats;
    stats.Open("Statistics.pic");

    FEHImage stats_C;
    stats_C.Open("Stats_C_1.pic");

    FEHImage stats_D;
    stats_D.Open("Stats_D_1.pic");

    FEHImage Arrow_1;
    Arrow_1.Open("Arrow_1.pic");

    FEHImage Arrow_2;
    Arrow_2.Open("Arrow_2.pic");

    FEHImage credits;
    credits.Open("Credits_1.pic");

    int x = 32;
    int y = 112;
    int spacing = 28;

    // Variable declarations for the x and y pixel where score is displayed
    int px = 124, py = 75;

    // Prints black rectangles over current image for first half of transition
    for (int i = 4; i <= 60; i+=4) {

        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(0,0,320,i*4,1);
        LCD.Update();
        Sleep(10);

    }

    // Checks the value of variable a (state), printing the selected menu for the given value.
    switch (a) {

        // Prints main menu and finishes the animation
        case 0:
            for (int i = 4; i <= 120; i+=4) {

                menu.Draw(0,0,1);
                LCD.SetFontColor(BLACK);
                LCD.FillRectangle(0,(i*2),320,240-(i*2),1);
                LCD.Update();
                Sleep(1);

            }
        break;

        // Prints character select menu and finishes the animation
        case 1:
            for (int i = 4; i <= 120; i+=4) {

                character.Draw(0,0,1);
                JC.Draw(52,76,1);
                JM.Draw(128,76,1);
                RH.Draw(204,76,1);
                LCD.SetFontColor(WHITE);
                LCD.DrawRectangle(52,76,64,64,1);
                LCD.DrawRectangle(128,76,64,64,1);
                LCD.DrawRectangle(204,76,64,64,1);

                LCD.SetFontColor(BLACK);
                LCD.FillRectangle(0,(i*2),320,240-(i*2),1);
                LCD.Update();
                Sleep(1);

            }
        break;

        // Prints difficulty select menu and finishes the animation
        case 2:
            for (int i = 4; i <= 120; i+=4) {

                difficulty.Draw(0,0,1);
                LCD.SetFontColor(BLACK);
                LCD.FillRectangle(0,(i*2),320,240-(i*2),1);
                LCD.Update();
                Sleep(1);

            }
        break;

        // Decided not to make a transition for playing the game
        case 3:
            for (int i = 4; i <= 120; i+=4) {

                Sleep(1); // "I'm not making a transition for this." (Luke Murray 2022)

            }
        break;

        // Prints death menu and finishes the animation
        case 4:
            for (int i = 4; i <= 120; i+=4) {

                death.Draw(0,0,1);
                LCD.SetFontColor(WHITE);
                if (UserCar->getScore() == 0) {LCD.WriteAt("  00000000",px,py,1); }
                else if (UserCar->getScore() < 10) { LCD.WriteAt("  0000000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+108,py,1); }
                else if (UserCar->getScore() < 100) { LCD.WriteAt("  000000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+96,py,1); }
                else if (UserCar->getScore() < 1000) { LCD.WriteAt("  00000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+84,py,1); }
                else if (UserCar->getScore() < 10000) { LCD.WriteAt("  0000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+72,py,1); }
                else if (UserCar->getScore() < 100000) { LCD.WriteAt("  000",px,py,1); LCD.WriteAt(UserCar->getScore(),px+60,py,1); }
                else if (UserCar->getScore() < 1000000) { LCD.WriteAt("  00",px,py,1); LCD.WriteAt(UserCar->getScore(),px+48,py,1); }
                else if (UserCar->getScore() < 9999999) { LCD.WriteAt("  0",px,py,1); LCD.WriteAt(UserCar->getScore(),px+36,py,1); }
                else {LCD.WriteAt("  99999999",px,py,1); }
                LCD.Update();
                LCD.SetFontColor(BLACK);
                LCD.FillRectangle(0,(i*2),320,240-(i*2),1);
                LCD.Update();
                Sleep(1);

            }
        break;

        // Prints info menu and finishes the animation
        case 5:
            for (int i = 4; i <= 120; i+=4) {

                instructions.Draw(0,0,1); 
                LCD.SetFontColor(BLACK);
                LCD.FillRectangle(0,(i*2),320,240-(i*2),1);
                LCD.Update();
                Sleep(1);

            }
        break;

        // Prints statistics menu and finishes the animation
        case 6:
            for (int i = 4; i <= 120; i+=4) {
                
                y = 112;
                x = 32;
                LCD.Clear();
                LCD.SetFontColor(WHITE);
                LCD.WriteAt("High Score:",x,y,1);
                y += spacing;
                LCD.WriteAt(" Km Driven:",x,y,1);
                y += spacing;
                LCD.WriteAt("# of Races:",x,y,1);
                LCD.WriteAt(Player->getMaxScore(1,0),x + 144,112,1);
                LCD.WriteAt(Player->getkmDriven(1,0),x + 144,140,1);
                LCD.WriteAt(Player->getGamesPlayed(1,0),x + 144,168,1);
                stats.Draw(0,0,1);
                stats_C.Draw(25,40,1);
                stats_D.Draw(25,72,1);
                LCD.SetFontColor(WHITE);
                Arrow_1.Draw(30,45,1);
                Arrow_2.Draw(269,45,1);
                LCD.SetFontColor(BLACK);
                LCD.FillRectangle(0,(i*2),320,240-(i*2),1);
                LCD.Update();
                Sleep(1);

            }
        break;

        // Prints credits menu and finishes the animation
        case 7:
            for (int i = 4; i <= 120; i+=4) {

                credits.Draw(0,0,1);
                LCD.SetFontColor(BLACK);
                LCD.FillRectangle(0,(i*2),320,240-(i*2),1);
                LCD.Update();
                Sleep(1);

            }

    }

    // Closes all of the image files.

    menu.Close();
    character.Close();
    JC.Close();
    JM.Close();
    RH.Close();
    instructions.Close();
    stats.Close();
    stats_C.Close();
    stats_D.Close();
    Arrow_1.Close();
    Arrow_2.Close();
    credits.Close();

}

// Custom LCD Functions

void DrawTrack(int x1, int x2, int y1, int width, Car *UserCar, FEHImage *TrackImg, FEHImage *Lava) {
    // Code Written by Luke Murray and Christopher Coppel

    /* This function prints the track image in either a rectangle or a rhombas depending on the 
    x values input into the function, and scrolls the image to the y position input. Sends and recieves data from
    the class Car and send images back to the playgame function*/
    
    // Set new images to the values of the pointer images (doesn't work if this is not done)
    FEHImage Track;
    Track = *TrackImg;

    FEHImage Stage_6;
    Stage_6 = *Lava;

    // Sets the font color to grey
    LCD.SetFontColor(GRAY);

    // Draw a Rectangle if the x1 = x2
    if (x1 == x2) {

        // If the stage is 6, prints the stage 6 track
        if (UserCar->getStage() == 6) { Stage_6.Draw(x1-12,y1,1); }

        // Prints the track
        Track.Draw(x1,y1,1);

        //Sets the x coordinate, y coordinate and slope of the track in the class Car
        UserCar->SetTrackPositions(x1,y1,0);

        // Ends the function
        return;

    }

    // Using a float to be more precise, will cast the end result to an int
    float slope = (float) -120 / ((float) x2 - x1);

    //Sets the x coordinate, y coordinate and slope of the track in the class Car
    UserCar->SetTrackPositions(x2,y1,slope);

    // Prints a line of pixels for all 120 y values in the track segment
    for (int i = 0; i < 120; i++ ) {
        
        //determines the x postion at a given y coordinate
        float x = ((120)-i)/slope + x2;

        //If the stage is 6 draws a line of pixels from the stage 6 image
        if (UserCar->getStage() == 6) { Stage_6.DrawLine(x-12,y1+i,i,1); }

        //Prints a single row of pixels from the track image
        Track.DrawLine(x,y1+i,i,1);
    }

    // Close Images

    Track.Close();
    Stage_6.Close();

}

// Class CharacterStats: Functions

CharacterStats::CharacterStats() {
    
    // Code Written by Christopher Coppel

    /* This Function reads in the save data and initializes 
    the variables in CharacterStats to the read values*/

    // Sets a pointer to a file
    FILE * Data;

    // Loading the txt file
    Data = fopen("SaveFile.txt", "r");

    // Checks if the txt file opens, if it didn't creates a new save file
    if (Data == NULL) {     // Create New Save File if one does not exist.

        fclose(Data);
        Data = fopen("SaveFile.txt","w");
        fprintf(Data, "0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n");
        fclose(Data);
        Data = fopen("SaveFile.txt", "r");

    }

    // Declaring matrix variables
    int i = 1;
    int j = 0;

    // Buffer values
    strcpy(charName[0],"BLANK");
    maxScore[0][0] = 0;
    kmDriven[0][0] = 0;
    gamesPlayed[0][0] = 0;
    maxScore[0][1] = 0;
    kmDriven[0][1] = 0;
    gamesPlayed[0][1] = 0;
    maxScore[0][2] = 0;
    kmDriven[0][2] = 0;
    gamesPlayed[0][2] = 0;

    // Copying character names into class
    strcpy(charName[1],"Jeremy Clarkson");
    strcpy(charName[2],"James May");
    strcpy(charName[3],"Richard Hammond");

    // Reads the file until the end of the line, reading three values at a time and placing them in maxScore, kmDriven, and gamesPlayed matrices.
    while (fscanf(Data,"%d %d %d", &maxScore[i][j], &kmDriven[i][j], &gamesPlayed[i][j]) != EOF) { 

        j++; //increments j by 1

        // Checks if j is 3, if it is it increments i and resets j to 0
        if (j == 3) {

            i++; // increments i by 1
            j=0; // sets j to be 0

        }
    
    }
    
    // Closes the txt file
    fclose(Data);

}

void CharacterStats::updateStats(int i, int j, int Score, int km) {

    // Code Written by Christopher Coppel

    /* This Function takes in the character (i), the difficulty (j), the Score obtained, 
    and the km driven in a game and edits the stats according to those values*/

    // Checks if the value of j is 0 or not, executes only if j isn't 0.
    if (j != 0) {

        // decrease the value of j by 1
        j -= 1;
        
        // Compares the score obtained to the max score of that character and 
        // difficulty, setting the maxscore to score if score is greater than maxscore
        if (Score > maxScore[i][j]) { maxScore[i][j] = Score; }

        kmDriven[i][j] += km; // Adds km to the kmDriven for the character and score
        gamesPlayed[i][j]++; //Add 1 to the gamesPlayed with that character on that difficulty

    }

}

void CharacterStats::fileWrite() {

    // Code Written by Christopher Coppel

    /* This function writes the max scores, km driven, and games played into the save data txt file*/

    // Opening the txt file
    ofstream Data;
    Data.open("SaveFile.txt");

    // For each value of i and value of j, prints into the txt file the maxScore, kmDriven, and gamesPlayed
    for (int i = 1; i<4; i++) {

        for (int j = 0; j<3; j++) {

            Data << maxScore[i][j] << "\n";
            Data << kmDriven[i][j] << "\n";
            Data << gamesPlayed[i][j] << "\n"; 

        }

    }   

    // Closes the txt file
    Data.close();

}

int CharacterStats::getMaxScore(int i, int j) { return maxScore[i][j]; }  // Code written by Christopher Coppel   // This code returns the maxScore of a chosen character (i) and difficulty (j)

int CharacterStats::getkmDriven(int i, int j) {return kmDriven[i][j]; }  // Code written by Christopher Coppel   // This code returns the kmDriven of a chosen character (i) and difficulty (j)

int CharacterStats::getGamesPlayed(int i, int j) {return gamesPlayed[i][j]; } // Code written by Christopher Coppel   // This code returns the gamesPlayed of a chosen character (i) and difficulty (j)

// Class Car: Functions

Car::Car() {

    // Code Written by Christopher Coppel

    /* This Function sets the base values of each variable in the class Car*/

    velocity = 5;
    x_vel = 0;
    y_vel = velocity;
    x_pos = 160;
    y_pos = 200;
    angle = 3.14159/2.0;
    hypoteneus = 0;
    score = 0;
    buffer = 0;
    km = 0;
    ry = 120;
    width = 64;
    slope = 0;
    x1 = 0;
    y1 = 0;
    trackBound = 0;
    trackx[0] = 128;
    trackType = 0;
    liveKM = 0;
    stage = 1;
    onTrack = 0;

}

void Car::updateSelection(int c, int d){
    
    // Code Written by Luke Murray

    /* This Function sets the value of character to the character input (c)
    and sets the difficulty to the difficulty input (d)*/

    character = c;
    difficulty = d;

}

int Car::getCharacter() { return character; }  // Code written by Luke Murray   // This code returns the value of character

int Car::getDifficulty() { return difficulty; }  // Code written by Luke Murray   // This code returns the value of difficulty

void Car::setDifficulty(int a) { difficulty = a; }  // Code written by Luke Murray   // This code sets difficulty to the difficulty input (a).

void Car::updateAngle(int x, int y) {

    // Code Written by Christopher Coppel and Luke Murray

    /* This Function takes in the x and y positions of the users click and uses them
    to determine the angle the car will move, and the length of the hypoteneus found
    betwwen the two coordinate pairs*/

    // Declaring Variables
    float deltaY, deltaX;

    // Sets deltaY to be the difference between the y position of the click, and the y position of the car
    deltaY = y-y_pos;

    // Sets deltaX to be the difference between the x position of the click, and the x position of the car
    deltaX = x-x_pos;
    
    // Checks if the x position of the click is greater than the x position of the car, finding the angle if it is
    if (x > x_pos) {

        angle = atan(deltaY/deltaX) - 3.141592;

    } 
    
    // Checks if the x position of the click is equal to the x position of the car, setting the angle to pi/2 if it is
    else if (x == x_pos) {

        angle = 3.141592/2.0;

    } 
    
    // Checks if the x position of the click is less than the x position of the car, finding the angle if it is
    else {

        angle = atan(deltaY/deltaX);

    }

    // Limits the values of the angles so they can not go under 0.26 if x is less than the car's x position
    // or above -3.4 if x is greater than the car's x position.
    if ((angle < .26) and (x < x_pos)) { angle = .26; } 
    
    else if ((angle > -3.40) and (x > x_pos)) { angle = -3.40; }

    // Determines the length of the hypoteneus
    hypoteneus = pow((pow(deltaX,2) + pow(deltaY,2)),0.5);
    
}

void Car::updateVelocity() {

    // Code Written by Christopher Coppel and Luke Murray

    /* This Function determines the velocity of the car, limits it if it is too large,
    then determines the x and y components of the velocity */


    // Declaring Variables
    int multiplier = 100; //Decreases velocity in an easy way to control
    float gamma = 5; //Increase velocity in an easy way to control
    int maxVelocity = 8;

    // Sets onTrack to 1
    onTrack = 1;

    // Determines the velocity
    velocity = hypoteneus*(gamma/multiplier);

    // Checks if the car is off the track, if it is, the velocity is decreased.
    if (x_pos < trackBound or x_pos > (trackBound + width)) {
        
        // Checks if stage is 5, if is isn't, off road modifier is 0.6, if it is, off road modifier is 0.2
        if (stage != 5) { velocity *= .6; }
        else { velocity *=.2; }

        // Set onTrack to 0
        onTrack = 0;

    }

    // Checks if the velocity is greater than max velocity. If it is velocity is set to be the max velocity
    if (velocity > maxVelocity) { velocity = maxVelocity; }

    // Calculates the x and y components of the velocity
    x_vel = velocity*(cos(angle));
    y_vel = velocity*(sin(angle));

}

void Car::updatePosition() { x_pos -= x_vel; }  // Code written by Christopher Coppel  // This code subtracts the x_vel from the x_pos to determine the new x position of the car

void Car::updateScore() {   
    
    // Code Written by Christopher Coppel and Luke Murray

    /* This Function calculates the player's score, updating a score modifier 
    and determining the stage as score increases, and determines the km driven during a game*/

    // Declaring Variables
    float modifier;  // Score Multiplier
    float reduction = 0.1; // Reduction for how speed effects score increase
    float add;

    // Checks the value of buffer, depending on its value it will change the stage number and the score modifier.
    if (buffer <1000) { modifier = 1; stage = 1; }
    else if (buffer > 1000 and buffer <= 5000) { modifier = 2; stage = 2; }
    else if (buffer > 5000 and buffer <= 10000) { modifier = 3; stage = 3; }
    else if (buffer > 10000 and buffer <= 50000) { modifier = 4; stage = 4; }
    else if (buffer > 50000 and buffer <= 100000) { modifier = 5; stage = 5; }
    else { modifier = 10; stage = 6; }

    // Calculates how many points will be added to buffer
    add = (y_vel * modifier * reduction);

    // Adds points to buffer and sets score to buffer (these are seperate as score must be tracked as float but needs to be printed as an int)
    buffer += add;
    score = buffer;

    // Calculates how many km will be added to liveKM, adds points to liveKM and 
    // sets km to liveKM (these are seperate as km must be tracked as float but needs to be printed as an int)
    liveKM += (velocity * reduction)/1000;
    km = liveKM;
    
}

void Car::reset() {
    
    // Code Written by Christopher Coppel

    /* This Function sets the base values of each variable in the class Car to there original values*/

    velocity = 5;
    x_vel = 0;
    y_vel = velocity;
    x_pos = 160;
    y_pos = 200;
    angle = 3.14159/2.0;
    hypoteneus = 0;
    score = 0;
    buffer = 0;
    km = 0;
    ry = 120;
    width = 64;
    slope = 0;
    x1 = 0;
    y1 = 0;
    trackBound = 0;
    trackx[0] = 128;
    trackType = 0;
    liveKM = 0;
    stage = 1;
    onTrack = 0;

}

int Car::getScore() { return score; }  // Code written by Christopher Coppel   // This code returns the value of score

float Car::getXPos() { return x_pos; }  // Code written by Christopher Coppel   // This code returns the value of the car's x position

float Car::getXVel() { return x_vel; }  // Code written by Christopher Coppel   // This code returns the value of the car's x velocity

float Car::getYVel() { return y_vel; }  // Code written by Christopher Coppel   // This code returns the value of the car's y velocity

float Car::getVel() { return velocity; } // Code written by Luke Murray        // This code returns the value of the car's velocity

float Car::getAngle() { return angle; }  // Code written by Christopher Coppel   // This code returns the value of the car's angle

void Car::setxPos(int i) { x_pos = i; }  // Code written by Christopher Coppel   // This code sets the car's x position to a given value (i)

void Car::setXVel(int i) { x_vel = i; }  // Code written by Christopher Coppel   // This code sets the car's x velocity to a given value (i)

int Car::getKM() { return km; }  // Code written by Christopher Coppel   // This code returns the value of km

int Car::getStage() { return stage; }  // Code written by Luke Murray   // This code returns the value of stage

void Car::printTrack(Car *UserCar ,FEHImage *Track, FEHImage *Lava) {

    // Code Written by Christopher Coppel

    /* This Function moves calls a pointer to the class Car, and ponters to two images, 
    using them to move the track segments downward, changing the values of each track's x 
    component if ry becomes greater than 240, then printing each track. It also determines 
    which track is at y = 200, finding the track's left most x position for that point 
    and setting it as the track boundary.*/

    // Moves the track's y positions downward 
    ry += y_vel;
    
    // Executes if the first track's y position is greater than 240
    if (ry > 240) {

        // Decrease the first track's postion by 120 pixels
        ry -= 120;

        // A shift register that sets each track x position as the x position for the next 
        // piece of track (therefore moving each value down by one track number). It then 
        // generate a new final x value for track 4. This is important as it saves a large amount
        // of memory.
        trackx[0]=trackx[1];
        trackx[1]=trackx[2];
        trackx[2]=trackx[3];
        trackx[3]=trackx[4];
        setTrackX(4);

    }

    // Prints the first 3 tracks in the sequence
    for (int i=0; i<3; i++) {

        // If the first track is being printed
        if (i == 0) { 
            
            // Draw track 1
            DrawTrack(trackx[i+1],trackx[i],ry-(120*i),width,UserCar, Track, Lava); 

            // If the track's y position is between 80 and 200 pixels
            if (ry>=80 and ry<=200) {
                
                // If slope is 0, set track bound to the x coordinate of the track segment
                if (slope == 0) { trackBound = x1; } 

                // If slope isn't 0, calculate the track bound using y position, x position, and slope
                else { trackBound = abs(80-y1)/(slope)+x1; }

            }

            // If the track's position isn't between 80 and 200 pixels
            else {
                
                // Draw track 2
                DrawTrack(trackx[i+2],trackx[i+1],ry-(120),width,UserCar, Track, Lava);

                // If slope is 0, set track bound to the x coordinate of the track segment
                if (slope == 0) { trackBound = x1; } 

                // If slope isn't 0, calculate the track bound using y position, x position, and slope
                else { trackBound = abs(80-y1)/(slope)+x1; }

            }

        }

        // If the first track isn't being printed, print the track (used as track boundary must be found using the first track)
        else { DrawTrack(trackx[i+1],trackx[i],ry-(120*i),width, UserCar, Track, Lava); }

    }

}

void Car::SetTrackPositions(int i, int j, float k) {

    // Code Written by Christopher Coppel

    /* This Function sets the values of the x track's x position (i), y position (j), and slope (k) to the inputs*/

    x1 = i;
    y1 = j;
    slope = k;

}

void Car::setTrackX(int i) {

    // Code Written by Christopher Coppel

    /* This Function alternates between creating a straight portion of track and an angle portion of track
    randomizing the left most x coordinate the angled tracks will reach while setting the nex tx coordinate 
    of a straight track to be the same as the previous coordinate.*/

    // Checks if track type is straight, if it is sets track x as the previous x value and sets track type to 1
    if (trackType == 0) { 
        
        trackx[i] = trackx[i-1]; 
        trackType = 1;

    }

    // Checks if track type is random, if it is generates a random x value and sets track type to 0
    else if (trackType == 1) {
        
        int remainder;
        int baseline = 40;
        remainder = Random.RandInt() % 176;
        trackx[i] = baseline + remainder;
        trackType = 0;

    }

}

void Car::SetOpstacleData(int i, int a) {

    // Code Written by Christopher Coppel

    /* This Function takes in an object number (i) and a difficulty (a), 
    randomly generating an object type and obstacle type each time to function is called*/

    // Generates either a destructive object (1) or a nondestructive object (0)
    objectType[i] = Random.RandInt() % 2;

    // If playing in Grand Tour mode (0), all objects will become nondestructive
    if (difficulty == 0) { objectType[i] = 0; }

    // Checks the value of object type
    switch (objectType[i]) {

        // If the object is nondestructive (0), randomize an obstacle type
        case 0:

            // Sets obstacleType to 0, 1, 2, or 3
            obstacleType[i] = Random.RandInt() % 4;

            // Checks the value of obstacleType
            switch (obstacleType[i]) {

                // If it is obstacleType 1, randomize an x and y coordinate and save data into each array
                case 1:
                obstaclex[i] = 40 + Random.RandInt() % 220;
                obstacley[i] = Random.RandInt() % 240-240;
                obstaclexOffset[i] = 4;
                obstacleyOffset[i] = 4;
                break;

                // If it is obstacleType 2, randomize an x and y coordinate and save data into each array
                case 2:
                obstaclex[i] = 40 + Random.RandInt() % 220;
                obstacley[i] = Random.RandInt() % 240-240;
                obstaclexOffset[i] = 8;
                obstacleyOffset[i] = 8;
                break;

                // If it is obstacleType 3, randomize an x and y coordinate and save data into each array
                case 3:
                obstaclex[i] = 40 + Random.RandInt() % 220;
                obstacley[i] = Random.RandInt() % 240-240;
                obstaclexOffset[i] = 16;
                obstacleyOffset[i] = 10;
                break;

                // If obstacleType is 0, the obstacle data is reset
                default:
                resetObstacleData(i);
                break;

            }

        break;

        // If the object is destructive (1), randomize an obstacle type
        case 1:

            // Sets obstacleType to 0, 1, 2, or 3
            obstacleType[i] = Random.RandInt() % 4;

            // Checks the value of obstacleType
            switch (obstacleType[i]) {

                // If it is obstacleType 1, randomize an x and y coordinate and save data into each array
                case 1:
                obstaclex[i] = 40 + Random.RandInt() % 220;
                obstacley[i] = Random.RandInt() % 240-240;
                obstaclexOffset[i] = 4;
                obstacleyOffset[i] = 4;
                break;

                // If it is obstacleType 2, randomize an x and y coordinate and save data into each array
                case 2:
                obstaclex[i] = 40 + Random.RandInt() % 220;
                obstacley[i] = Random.RandInt() % 240-240;
                obstaclexOffset[i] = 8;
                obstacleyOffset[i] = 8;
                break;

                // If it is obstacleType 3, randomize an x and y coordinate and save data into each array
                case 3:
                obstaclex[i] = 40 + Random.RandInt() % 220;
                obstacley[i] = Random.RandInt() % 240-240;
                obstaclexOffset[i] = 16;
                obstacleyOffset[i] = 10;
                break;

                // If obstacleType is 0, the obstacle data is reset
                default:
                resetObstacleData(i);
                break;

            }

        break;

        // Otherwise end the switch case
        default:
        break;

    }

}

void Car::resetObstacleData(int i) {

    // Code Written by Christopher Coppel

    /* This Function resets all of a chosen object's (i) data to zeros */

    obstaclex[i] = 0;
    obstacley[i] = 0;
    obstaclexOffset[i] = 0;
    obstacleyOffset[i] = 0;

}

void Car::printOpstacle(int i, FEHImage *Objects, int a) {

    // Code Written by Luke Murray

    /* This Function calls an obstacle number (i), a pointer to an image file, and a difficulty to
    print a certain part of the image to the screen depending on the object and obstacles types
    to the object's x and y positions.*/

    // Opens and Sets an image to the pointer
    FEHImage ObjectAtlas;
    ObjectAtlas = *Objects;

    // Determines what type of object is placed depending on the stage
    int object = 0;
    
    // Updates the obstacle's new y position by subtracting the y velocity of the car
    obstacley[i] += y_vel;

    // Checks if the obstacles y position is greater than 240, if it is a new obstacle is generated
    if (obstacley[i] >= 240) { SetOpstacleData(i, a); }
    
    // Checks the whether the obstacle is nondestructive(0) or destructive(1) changing the image taken depending on the value
    switch (objectType[i]) {
        
        // If the object is nondestructive
        case 0:
            
            // Changes the value of object depending on the current stage
            object = stage * 10;

            // Checks the obstacle type, printing a different image depending on its value
            switch (obstacleType[i]) {

                case 1:
                    ObjectAtlas.DrawAtlas(obstaclex[i],obstacley[i],0,object,4,4,1);
                break;

                case 2:
                    ObjectAtlas.DrawAtlas(obstaclex[i],obstacley[i],4,object,8,8,1);
                break;

                case 3:
                    ObjectAtlas.DrawAtlas(obstaclex[i],obstacley[i],12,object,16,10,1);
                break;

            }

        break;

        // If the object is destructive
        case 1:

            // Object set to be 0
            object = 0;

            // Checks the obstacle type, printing a different image depending on its value
            switch (obstacleType[i]) {

                case 1:
                    ObjectAtlas.DrawAtlas(obstaclex[i],obstacley[i],0,object,4,4,1);
                break;

                case 2:
                    ObjectAtlas.DrawAtlas(obstaclex[i],obstacley[i],4,object,8,8,1);
                break;

                case 3:
                    ObjectAtlas.DrawAtlas(obstaclex[i],obstacley[i],12,object,16,10,1);
                break;

            }

        break;

    }

    // Close the image
    ObjectAtlas.Close();

}

int Car::checkDeath(int ObstacleCount, int invincibility) {

    // Code Written by Christopher Coppel

    /* This Function calls an ObstacleCount and an invincibility value then proceeds to check 
    if the car hits each obstacle, ending the game if the car hits 
    an obstacle with an obstacle type that is not 0*/

    // check if invincibilty is turned off (1) or if difficulty is Grand Tour mode (0)
    if (invincibility == 1 or difficulty == 0) { 

        // Checks if the stage is not stage 6
        if (stage != 6) {
        
            // For every obstacle currently created check its type and check for collision
            for (int i = 0; i < ObstacleCount; i++) {

                // checks if the object type is not a 0
                if (objectType[i] != 0) {

                    // Compares car hit box to the hit box of each object. If they are in the same position returns 0, resulting in a game over
                    if (((x_pos-7 >= obstaclex[i] and x_pos-7 <= obstaclex[i] + obstaclexOffset[i]) or (x_pos+7 >= obstaclex[i] and x_pos+7 <= obstaclex[i] + obstaclexOffset[i])) and ((y_pos-7 >= obstacley[i] and y_pos-7 <= obstacley[i] + obstacleyOffset[i]) or (y_pos+7 >= obstacley[i] and y_pos+7 <= obstacley[i] + obstacleyOffset[i]))) { return 0; }
                    else if (x_pos >= obstaclex[i] and x_pos <= obstaclex[i] + obstaclexOffset[i] and y_pos >= obstacley[i] and y_pos <= obstacley[i] + obstacleyOffset[i]) { return 0; }
                    
                }

            }

        } 
        
        // If stage is 6, if the car goes off the track return 0, resulting in a game over
        else if (x_pos < trackBound - 12 or x_pos > (trackBound + width + 12)) { return 0; } 

    }

    // Return 1, continuing the game
    return 1;

}