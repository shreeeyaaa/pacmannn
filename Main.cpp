#include <array>
#include <chrono>
#include <ctime>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Global.h"
#include "DrawText.h"
#include "Pacman.h"
#include "Ghost.h"
#include "GhostManager.h"
#include "ConvertSketch.h"
#include "DrawMap.h"
#include "MapCollision.h"
#include "Sound.h"
#include<fstream>
#include"MainMenu.h"
#include"Howtoplay.h"
#include<iostream>
#include "SnowAnimation.h"
#include <iostream>
#include"file.h"


const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;




const int SNOW_EFFECT_DELAY = 5; // 3 minutes in seconds
const int SNOW_EFFECT_DURATION = 30;
const int NUM_SNOWFLAKES = 100;


enum class GameState {
    Menu,
    Playing,
    GameOver
};
int main()
{
    enum GameState {
        mainmenu,
        HowToPlay,
        PlayGame,
        Leaderboard,
        Exit
    };

    GameState currentState = mainmenu;
    bool game_won = false;
    int lives = 3;
    unsigned lag = 0;
    unsigned char level = 0;
    //int a=0, b=0;
    //Is the game won?
    //bool game_won = 0;

    //Used to make the game framerate-independent.
    //unsigned lag = 0;
    //lag along with delta time stores the time elapsed between last frame change

   // unsigned char level = 0;

    //Similar to lag, used to make the game framerate-independent.
    std::chrono::time_point<std::chrono::steady_clock> previous_time;
    //creates a variable named previous_time of time_point(template class of c++ standard library chrono)
    //holds a particular time point of <std::chrono::steady_clock> clock
    bool freeze = false;


    std::chrono::time_point<std::chrono::steady_clock> start_snow_time = std::chrono::steady_clock::now() + std::chrono::seconds(SNOW_EFFECT_DELAY);

    //It's not exactly like the map from the original Pac-Man game, but it's close enough.
    //creates a two-dimensional array (technically an array of strings) named map_sketch to represent a map layout.
    std::array<std::string, MAP_HEIGHT> map_sketch = {
        " ################### ",
        " #........#........# ",
        " #o##.###.#.###.##o# ",
        " #.................# ",
        " #.##.#.#####.#.##.# ",
        " #....#...#...#....# ",
        " ####.### # ###.#### ",
        "    #.#   0   #.#    ",
        "#####.# ##=## #.#####",
        "     .  #123#  .     ",
        "#####.# ##### #.#####",
        "    #.#       #.#    ",
        " ####.# ##### #.#### ",
        " #........#........# ",
        " #.##.###.#.###.##.# ",
        " #o.#.....P.....#.o# ",
        " ##.#.#.#####.#.#.## ",
        " #....#...#...#....# ",
        " #.######.#.######.# ",
        " #.................# ",
        " ################### "
    };

    std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> map{};

    //Initial ghost positions.
    std::array<Position, 4> ghost_positions;

    //SFML thing. Stores events, I think.
    sf::Event event;

    sf::RenderWindow window(sf::VideoMode(672,704), "Pac-Man", sf::Style::Close);
    //Resizing the window making sure the player sees the relevant parts of the game environment.
    //window.setView(sf::View(sf::FloatRect(0, 0, CELL_SIZE * MAP_WIDTH, FONT_HEIGHT + CELL_SIZE * MAP_HEIGHT)));
    user u1(window);

    MainMenu mainMenu(window);
    //Howtoplay howtoplayy;

    while (currentState != Exit) {
        switch (currentState) {
        case mainmenu:

            int choice;
            choice = mainMenu.draw();
            switch (choice) {
            case 1:
                currentState = PlayGame;
                break;
            case 2:
                currentState = Leaderboard;
                break;

            case 3:
                currentState = Exit;
                break;

            case 4:
                currentState = HowToPlay;
                Howtoplay howtoplayy;
                int a;
                a = howtoplayy.show();
                if (a == 1) {
                    currentState = mainmenu;
                }
                break;
            }

            break;

        case HowToPlay:
            //Howtoplay howtoplayy;
            //howtoplayy.show();
            currentState = mainmenu;
            break;

        case Exit:
            //std::cout << "hi" << std::endl;
            window.close();
            //currentState = mainmenu;
            break;

        case Leaderboard:
            int a;
            a = u1.onlylb();
            //int b;
            //Howtoplay howtoplay;
            //b=howtoplay.show();
            //std::cout << "How to Play:\n";
            // Display instructions on how to play the game
            ///*if (b == 1) {
            //	currentState = mainmenu;
            //}*/
            // Return to the main menu after reading instructions
            //break;
            currentState = mainmenu;
            /*if (a == 1) {
                mainMenu.draw();
            }*/

            break;

        case PlayGame:
            std::cout << "Playing the game...\n";
            //window.setView(sf::View(sf::FloatRect(0, 0, 672,704)));

            //window.setView(sf::View(sf::FloatRect(0, 0, CELL_SIZE* MAP_WIDTH, FONT_HEIGHT + CELL_SIZE * MAP_HEIGHT)));
            bool snow_animation_active = false;
            sf::Clock snow_animation_clock;

            //window.setView(sf::View(sf::FloatRect(0, -CELL_SIZE * 2, CELL_SIZE * (MAP_WIDTH), FONT_HEIGHT + CELL_SIZE * (MAP_HEIGHT + 3))));
            window.setFramerateLimit(60);

            SnowAnimation snowAnimation(WINDOW_WIDTH, WINDOW_HEIGHT, NUM_SNOWFLAKES);
            sf::Music music;



            GhostManager ghost_manager;

            std::array<Ghost, 4>& ghosts = ghost_manager.getGhosts();
            Pacman pacman;

            SoundManager sound;
            srand(static_cast<unsigned>(time(0)));
            map = convert_sketch(map_sketch, ghost_positions, pacman);
            ghost_manager.reset(level, ghost_positions);
            previous_time = std::chrono::steady_clock::now();

            sf::Clock waitingClock;
            enum class GameStartState {
                WaitingForKeyPress,
                Playing
            };
            GameStartState startState = GameStartState::WaitingForKeyPress;
            if (!music.openFromFile("pacman_beginning.ogg"))
            {
                std::cout << "cant open";
            }

            else {
                music.play();

            }
            window.setFramerateLimit(60);


            bool escapePressed = false;

            while (1 == window.isOpen() && !escapePressed)
            {
                // ... (Your event handling code)

                //std:: cout << "helo";
                window.setView(sf::View(sf::FloatRect(0, -CELL_SIZE * 2, CELL_SIZE* (MAP_WIDTH), FONT_HEIGHT + CELL_SIZE * (MAP_HEIGHT + 3))));

                unsigned delta_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - previous_time).count();
                lag += delta_time;
                previous_time += std::chrono::microseconds(delta_time);
                auto current_time = std::chrono::steady_clock::now();
                auto elapsed_snow_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_snow_time).count();
                window.setVerticalSyncEnabled(true);
                while (FRAME_DURATION <= lag) {
                    lag -= FRAME_DURATION;
                    while (window.pollEvent(event)) {
                        switch (event.type) {
                            // ... (Your event handling code)
                        case sf::Event::Closed:
                            window.close();
                            break;

                        case sf::Event::KeyPressed:
                            if (event.key.code == sf::Keyboard::Escape)
                            {
                                // Exit the while loop if the Escape key is pressed.
                                std::cout << "escape pressed" << std::endl;
                                currentState = mainmenu;
                                //currentState = mainmenu; // Return to the main menu after playing
                                escapePressed = true; // Set the flag to exit the loop
                                break;
                                //return 3;
                                //mainMenu.draw();
                                // Return to the main menu after playing

                            }
                            break;
                        }
                    }

                    if (sf::seconds(SNOW_EFFECT_DELAY) <= snow_animation_clock.getElapsedTime() && snow_animation_clock.getElapsedTime() <= sf::seconds(SNOW_EFFECT_DURATION)) {
                        snow_animation_active = true;
                    }
                    else {
                        snow_animation_active = false;
                    }

                    // window.clear(sf::Color::Black);

                  /*  if (snow_animation_active) {
                          snowAnimation.update(2.0f / 60.0f); // Pass the elapsed time
                          snowAnimation.draw(window);

                        // draw_map(map, window);

                        // pacman.draw(game_won, window, freeze);
                        //pacman.snowupdate(level, map, delta_time, freeze);
                        /// ghost_manager.draw(true, window, freeze);
                         //ghost_manager.update(level, map, pacman);
                         std::cout << " >>" << delta_time << std::endl;


                         //pacman.update

                         if (snow_animation_clock.getElapsedTime() >= sf::seconds(SNOW_EFFECT_DURATION)) {
                             waitingClock.restart();
                             snow_animation_active = false;
                             window.clear();
                         }

                     }
                     */


                    waitingClock.restart();
                    // ... (Your game logic and collision detection code)




                    if (startState == GameStartState::WaitingForKeyPress && music.getStatus() == sf::SoundSource::Stopped) {
                        // ... (Your waiting for keypress code)
                        previous_time = std::chrono::steady_clock::now();
                        waitingClock.restart();
                        startState = GameStartState::Playing;

                    }

                    if (startState == GameStartState::Playing && !game_won && !pacman.get_dead()) {
                        // ... (Your game logic and collision detection code)
                        {
                            bool pelletsLeft = false;
                            // if (!snow_animation_active) 
                            pacman.update(level, map, delta_time, pacman.getFreeze());

                            // else 
                              //  pacman.snowupdate(level, map, delta_time, pacman.getFreeze());

                            ghost_manager.update(level, map, pacman);

                            for (const std::array<Cell, MAP_HEIGHT>& column : map)
                            {
                                for (const Cell& cell : column)
                                {
                                    if (Cell::Pellet == cell)
                                    {
                                        pelletsLeft = true;
                                        break;
                                    }
                                }

                                if (pelletsLeft)
                                {
                                    break;
                                }
                            }

                            if (!pelletsLeft)
                            {
                                game_won = true;
                            }
                            else if (pacman.get_dead())
                            {
                                lives--;
                                std::cout << "lives left: " << lives << std::endl;

                                if (pacman.Score() > pacman.getHighScore()) {
                                    pacman.updateHighScore(pacman.Score());
                                    std::ofstream MyFile("highScore.txt", std::ios::out | std::ios::trunc);

                                    if (!MyFile.is_open()) {
                                        std::cout << "Failed to open the file." << std::endl;

                                    }

                                    else {
                                        MyFile << pacman.getHighScore();



                                    }

                                    MyFile.close();

                                }

                                if (lives <= 0)
                                {
                                    sound.Dead();
                                    game_won = false;
                                    std::cout << "Game Over" << std::endl;
                                }
                                else
                                {
                                    window.clear();
                                    sound.Dead();
                                    sf::sleep(sf::milliseconds(1000));
                                    pacman.draw_death_animation(window);
                                    window.display();
                                    window.clear();
                                    while (sound.IsPlayingDeathSound()) {
                                        continue;
                                    }
                                    pacman.reset();
                                    ghost_manager.reset(level, ghost_positions);

                                    previous_time = std::chrono::steady_clock::now();
                                    startState = GameStartState::Playing;


                                    waitingClock.restart();
                                }
                            }




                        }
                    }

                    if (startState == GameStartState::Playing && !game_won && !pacman.get_dead()) {
                        // Draw the maze
                        draw_map(map, window);
                        ghost_manager.draw(GHOST_FLASH_START >= pacman.get_energizer_timer(), window, pacman.getFreeze());
                        draw_text(0, 0, -FONT_HEIGHT, "Score:" + std::to_string(pacman.Score()), window);
                        draw_text(0, 0, -2 * FONT_HEIGHT, "High Score" + std::to_string(pacman.getHighScore()), window);
                        draw_text(0, 0, CELL_SIZE * MAP_HEIGHT, "Level: " + std::to_string(1 + level), window);
                        draw_text(0, 0, CELL_SIZE * MAP_HEIGHT + FONT_HEIGHT, "Lives: " + std::to_string(lives), window);
                        pacman.draw(game_won, window, pacman.getFreeze());
                        //startState = GameStartState::WaitingForKeyPress;


                    }
                    else if (startState == GameStartState::WaitingForKeyPress) {
                        // Draw the maze
                        draw_map(map, window);
                        ghost_manager.draw(GHOST_FLASH_START >= pacman.get_energizer_timer(), window, pacman.getFreeze());
                        draw_text(0, 0, -FONT_HEIGHT, "Game Starting...", window);
                        draw_text(0, 0, CELL_SIZE * MAP_HEIGHT, "Level: " + std::to_string(1 + level), window);
                        draw_text(0, 0, CELL_SIZE * MAP_HEIGHT + FONT_HEIGHT, "Lives: " + std::to_string(lives), window);
                        pacman.pacman_sprite(window);
                    }

                    if (!pacman.get_dead() || !pacman.get_animation_over()) {
                        pacman.draw(game_won, window, pacman.getFreeze());
                    }

                    if (pacman.get_animation_over()) {
                        // ... (Your continue game or next level code)
                        if (game_won)
                        {
                            draw_text(1, 0, 0, "Next level! Press Enter key to continue", window);
                        }
                        else
                        {
                            draw_text(1, 0, 0, "Game over. Press Enter key to continue", window);
                            u1.display();
                        }

                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
                        {
                            game_won = false;
                            if (pacman.get_dead())
                            {
                                level = 0;
                            }
                            else
                            {
                                level++;
                            }

                            map = convert_sketch(map_sketch, ghost_positions, pacman);
                            ghost_manager.reset(level, ghost_positions);
                            pacman.reset();
                            lives = 3;
                            pacman.Score() = 0;

                            startState = GameStartState::WaitingForKeyPress; // Place it here
                            if (startState == GameStartState::WaitingForKeyPress) {
                                std::cout << "waiting for keypress state";
                            }
                            music.play();

                            waitingClock.restart();


                        }

                        window.display();


                    }


                    window.display();
                    window.clear();
                }

            }
            //mainMenu.draw();
            //if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            //{
            //	std::cout << "escape pressed" << std::endl;
            //	currentState = mainmenu; // Return to the main menu after playing
            //	break;
            //}

            // Your game logic goes here
            currentState = mainmenu; // Return to the main menu after playing
            break;
        }
    }

}


//#include <array>
//#include <chrono>
//#include <ctime>
//#include <iostream>
//#include <SFML/Graphics.hpp>
//#include <SFML/Audio.hpp>
//
//#include "Global.h"
//#include "DrawText.h"
//#include "Pacman.h"
//#include "Ghost.h"
//#include "GhostManager.h"
//#include "ConvertSketch.h"
//#include "DrawMap.h"
//#include "MapCollision.h"
//#include "Sound.h"
//#include<fstream>
//#include"MainMenu.h"
//#include"Howtoplay.h"
//#include<iostream>
//#include "SnowAnimation.h"
//#include <iostream>
//
//
//const int WINDOW_WIDTH = 800;
//const int WINDOW_HEIGHT = 600;
//
//
//
//
//const int SNOW_EFFECT_DELAY = 5; // 3 minutes in seconds
//const int SNOW_EFFECT_DURATION = 30;
//const int NUM_SNOWFLAKES = 100;
//
//
//enum class GameState {
//    Menu,
//    Playing,
//    GameOver
//};
//
//int main()
//{
//    // ... (Your initialization code)
//    int a = 0, b = 0;
//    bool game_won = false;
//    int lives = 3;
//    unsigned lag = 0;
//    unsigned char level = 0;
//    std::chrono::time_point<std::chrono::steady_clock> previous_time;
//
//    bool freeze = false;
//
//
//    std::chrono::time_point<std::chrono::steady_clock> start_snow_time = std::chrono::steady_clock::now() + std::chrono::seconds(SNOW_EFFECT_DELAY);
//
//
//
//    std::array<std::string, MAP_HEIGHT> map_sketch = {
//        // ... Your map sketch data ...
//        " ################### ",
//        " #........#........# ",
//        " #o##.###.#.###.##o# ",
//        " #.................# ",
//        " #.##.#.#####.#.##.# ",
//        " #....#...#...#....# ",
//        " ####.### # ###.#### ",
//        "    #.#   0   #.#    ",
//        "#####.# ##=## #.#####",
//        "     .  #123#  .     ",
//        "#####.# ##### #.#####",
//        "    #.#       #.#    ",
//        " ####.# ##### #.#### ",
//        " #........#........# ",
//        " #.##.###.#.###.##.# ",
//        " #o.#.....P.....#.o# ",
//        " ##.#.#.#####.#.#.## ",
//        " #....#...#...#....# ",
//        " #.######.#.######.# ",
//        " #.................# ",
//        " ################### "
//
//    };
//
//    std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> map{};
//    std::array<Position, 4> ghost_positions;
//    sf::Event event;
//
//    sf::RenderWindow window(sf::VideoMode(CELL_SIZE * (MAP_WIDTH)*SCREEN_RESIZE, (FONT_HEIGHT + CELL_SIZE * (MAP_HEIGHT + 3)) * SCREEN_RESIZE), "Pac-Man", sf::Style::Close);
//    //window.setView(sf::View(sf::FloatRect(0, -CELL_SIZE * 2, CELL_SIZE * (MAP_WIDTH), FONT_HEIGHT + CELL_SIZE * (MAP_HEIGHT + 3))));
//    MainMenu mainMenu(window);
//    a = mainMenu.draw();
//
//    if (b == 1) {
//        a = mainMenu.draw();
//    }
//    if (a == 1)
//    {
//
//        bool snow_animation_active = false;
//        sf::Clock snow_animation_clock;
//
//        window.setView(sf::View(sf::FloatRect(0, -CELL_SIZE * 2, CELL_SIZE * (MAP_WIDTH), FONT_HEIGHT + CELL_SIZE * (MAP_HEIGHT + 3))));
//        window.setFramerateLimit(60);
//
//        SnowAnimation snowAnimation(WINDOW_WIDTH, WINDOW_HEIGHT, NUM_SNOWFLAKES);
//        sf::Music music;
//
//
//
//        GhostManager ghost_manager;
//
//        std::array<Ghost, 4>& ghosts = ghost_manager.getGhosts();
//        Pacman pacman;
//
//        SoundManager sound;
//        srand(static_cast<unsigned>(time(0)));
//        map = convert_sketch(map_sketch, ghost_positions, pacman);
//        ghost_manager.reset(level, ghost_positions);
//        previous_time = std::chrono::steady_clock::now();
//
//        sf::Clock waitingClock;
//        enum class GameStartState {
//            WaitingForKeyPress,
//            Playing
//        };
//        GameStartState startState = GameStartState::WaitingForKeyPress;
//        if (!music.openFromFile("pacman_beginning.ogg"))
//        {
//            std::cout << "cant open";
//        }
//
//        else {
//            music.play();
//
//        }
//        window.setFramerateLimit(60);
//
//
//        while (window.isOpen())
//        {
//            // ... (Your event handling code)
//
//            //std:: cout << "helo";
//
//            unsigned delta_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - previous_time).count();
//            lag += delta_time;
//            previous_time += std::chrono::microseconds(delta_time);
//            auto current_time = std::chrono::steady_clock::now();
//            auto elapsed_snow_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_snow_time).count();
//            window.setVerticalSyncEnabled(true);
//            while (FRAME_DURATION <= lag) {
//                lag -= FRAME_DURATION;
//                while (window.pollEvent(event)) {
//                    switch (event.type) {
//                        // ... (Your event handling code)
//                    case sf::Event::Closed:
//                        window.close();
//                        break;
//                    }
//                }
//
//                if (sf::seconds(SNOW_EFFECT_DELAY) <= snow_animation_clock.getElapsedTime() && snow_animation_clock.getElapsedTime() <= sf::seconds(SNOW_EFFECT_DURATION)) {
//                    snow_animation_active = true;
//                }
//                else {
//                    snow_animation_active = false;
//                }
//
//                // window.clear(sf::Color::Black);
//
//              /*  if (snow_animation_active) {
//                      snowAnimation.update(2.0f / 60.0f); // Pass the elapsed time
//                      snowAnimation.draw(window);
//
//                    // draw_map(map, window);
//
//                    // pacman.draw(game_won, window, freeze);
//                    //pacman.snowupdate(level, map, delta_time, freeze);
//                    /// ghost_manager.draw(true, window, freeze);
//                     //ghost_manager.update(level, map, pacman);
//                     std::cout << " >>" << delta_time << std::endl;
//
//
//                     //pacman.update
//
//                     if (snow_animation_clock.getElapsedTime() >= sf::seconds(SNOW_EFFECT_DURATION)) {
//                         waitingClock.restart();
//                         snow_animation_active = false;
//                         window.clear();
//                     }
//
//                 }
//                 */
//
//
//                waitingClock.restart();
//                // ... (Your game logic and collision detection code)
//
//
//
//
//                if (startState == GameStartState::WaitingForKeyPress && music.getStatus() == sf::SoundSource::Stopped) {
//                    // ... (Your waiting for keypress code)
//                    previous_time = std::chrono::steady_clock::now();
//                    waitingClock.restart();
//                    startState = GameStartState::Playing;
//
//                }
//
//                if (startState == GameStartState::Playing && !game_won && !pacman.get_dead()) {
//                    // ... (Your game logic and collision detection code)
//                    {
//                        bool pelletsLeft = false;
//                        // if (!snow_animation_active) 
//                        pacman.update(level, map, delta_time, pacman.getFreeze());
//
//                        // else 
//                          //  pacman.snowupdate(level, map, delta_time, pacman.getFreeze());
//
//                        ghost_manager.update(level, map, pacman);
//
//                        for (const std::array<Cell, MAP_HEIGHT>& column : map)
//                        {
//                            for (const Cell& cell : column)
//                            {
//                                if (Cell::Pellet == cell)
//                                {
//                                    pelletsLeft = true;
//                                    break;
//                                }
//                            }
//
//                            if (pelletsLeft)
//                            {
//                                break;
//                            }
//                        }
//
//                        if (!pelletsLeft)
//                        {
//                            game_won = true;
//                        }
//                        else if (pacman.get_dead())
//                        {
//                            lives--;
//                            std::cout << "lives left: " << lives << std::endl;
//
//                            if (pacman.Score() > pacman.getHighScore()) {
//                                pacman.updateHighScore(pacman.Score());
//                                std::ofstream MyFile("highScore.txt", std::ios::out | std::ios::trunc);
//
//                                if (!MyFile.is_open()) {
//                                    std::cout << "Failed to open the file." << std::endl;
//
//                                }
//
//                                else {
//                                    MyFile << pacman.getHighScore();
//
//
//
//                                }
//
//                                MyFile.close();
//
//                            }
//
//                            if (lives <= 0)
//                            {
//                                sound.Dead();
//                                game_won = false;
//                                std::cout << "Game Over" << std::endl;
//                            }
//                            else
//                            {
//                                window.clear();
//                                sound.Dead();
//                                sf::sleep(sf::milliseconds(1000));
//                                pacman.draw_death_animation(window);
//                                window.display();
//                                window.clear();
//                                while (sound.IsPlayingDeathSound()) {
//                                    continue;
//                                }
//                                pacman.reset();
//                                ghost_manager.reset(level, ghost_positions);
//
//                                previous_time = std::chrono::steady_clock::now();
//                                startState = GameStartState::WaitingForKeyPress;
//
//
//                                waitingClock.restart();
//                            }
//                        }
//
//
//
//
//                    }
//                }
//
//                if (startState == GameStartState::Playing && !game_won && !pacman.get_dead()) {
//                    // Draw the maze
//                    draw_map(map, window);
//                    ghost_manager.draw(GHOST_FLASH_START >= pacman.get_energizer_timer(), window, pacman.getFreeze());
//                    draw_text(0, 0, -FONT_HEIGHT, "Score:" + std::to_string(pacman.Score()), window);
//                    draw_text(0, 0, -2 * FONT_HEIGHT, "High Score" + std::to_string(pacman.getHighScore()), window);
//                    draw_text(0, 0, CELL_SIZE * MAP_HEIGHT, "Level: " + std::to_string(1 + level), window);
//                    draw_text(0, 0, CELL_SIZE * MAP_HEIGHT + FONT_HEIGHT, "Lives: " + std::to_string(lives), window);
//                    pacman.draw(game_won, window, pacman.getFreeze());
//
//                }
//                else if (startState == GameStartState::WaitingForKeyPress) {
//                    // Draw the maze
//                    draw_map(map, window);
//                    ghost_manager.draw(GHOST_FLASH_START >= pacman.get_energizer_timer(), window, pacman.getFreeze());
//                    draw_text(0, 0, -FONT_HEIGHT, "Game Starting...", window);
//                    draw_text(0, 0, CELL_SIZE * MAP_HEIGHT, "Level: " + std::to_string(1 + level), window);
//                    draw_text(0, 0, CELL_SIZE * MAP_HEIGHT + FONT_HEIGHT, "Lives: " + std::to_string(lives), window);
//                    pacman.pacman_sprite(window);
//                }
//
//                if (!pacman.get_dead() || !pacman.get_animation_over()) {
//                    pacman.draw(game_won, window, pacman.getFreeze());
//                }
//
//                if (pacman.get_animation_over()) {
//                    // ... (Your continue game or next level code)
//                    if (game_won)
//                    {
//                        draw_text(1, 0, 0, "Next level! Press Enter key to continue", window);
//                    }
//                    else
//                    {
//                        draw_text(1, 0, 0, "Game over. Press Enter key to continue", window);
//                    }
//
//                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
//                    {
//                        game_won = false;
//                        if (pacman.get_dead())
//                        {
//                            level = 0;
//                        }
//                        else
//                        {
//                            level++;
//                        }
//
//                        map = convert_sketch(map_sketch, ghost_positions, pacman);
//                        ghost_manager.reset(level, ghost_positions);
//                        pacman.reset();
//                        lives = 3;
//                        pacman.Score() = 0;
//
//                        startState = GameStartState::WaitingForKeyPress; // Place it here
//                        if (startState == GameStartState::WaitingForKeyPress) {
//                            std::cout << "waiting for keypress state";
//                        }
//                        music.play();
//
//                        waitingClock.restart();
//
//
//                    }
//
//                    window.display();
//
//
//                }
//
//
//                window.display();
//                window.clear();
//            }
//
//        }
//    }
//
//    return 0;
//}