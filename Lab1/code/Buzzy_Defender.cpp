/*
Author: Liam Long
Class: ECE4122
Last Date Modified: 9/22/25
Description:
Main game loop and helpers for "Buzzy_Defender!". Handles asset loading,
screen scaling, event handling, entity updates (player, enemies, lasers),
collision detection, win/lose checks, and replay flow.
*/

// ----------------------------- Includes -----------------------------

#include <SFML/Graphics.hpp>   // SFML rendering primitives: RenderWindow, Texture, Sprite, etc.
#include <list>                // std::list for laser shot containers (easy erase while iterating)
#include <vector>              // std::vector for enemy swarm
#include <stdexcept>           // std::runtime_error for texture load failure
#include <string>              // std::string for asset paths
#include <limits>              // std::numeric_limits for ±infinity bounds
#include <cmath>               // std::isfinite to guard empty-swarm edge case

#include "ECE_Buzzy.h"         // Player sprite class
#include "ECE_LaserBlast.h"    // Laser blase class
#include "ECE_Enemy.h"         // Enemy sprite class

// using namespace for readability
using namespace sf;

/*
 * Purpose:
 *      Encodes the outcome of a game round and guides replay logic.
 * Values:
 *      Win  - the player destroyed all enemies.
 *      Lose - the player collided with an enemy or got hit by a laser.
 *      Quit - the player closed the window or pressed Esc at a screen.
 */
enum class GameOutcome
{
    Win, Lose, Quit
};

/*
 * Purpose:
 *      Aggregates all textures used by the game so they are loaded once
 *      and reused across rounds (avoids re-reading from disk).
 * Fields:
 *      startTex  - start screen background
 *      endTex    - lose screen background
 *      winTex    - win screen background
 *      bgTex     - gameplay background
 *      buzzyTex  - player sprite texture
 *      laserTex  - laser sprite texture (used by player and enemies)
 *      enemy1Tex - enemy variant 1
 *      enemy2Tex - enemy variant 2
 * Notes:
 *      Keep this small and so it can be passed by const reference.
 */
struct allTextures                                                                   // Define a tiny struct for textures so don't have to reload every round
{
    Texture startTex, endTex, winTex, bgTex, buzzyTex, laserTex, enemy1Tex, enemy2Tex;
};

// --------------------------- Small Helpers ---------------------------

/*
 * Purpose:
 *      Loads a texture from disk and throws if it fails.
 * Input(s):
 *      const std::string& path - relative/absolute file path to an image.
 * Output:
 *      Texture - a fully loaded SFML Texture by value (NRVO applies).
 * Throws:
 *      std::runtime_error if the file cannot be loaded.
 */
static Texture loadTexture(const std::string& path)
{
    Texture t;
    t.loadFromFile(path);
    return t;
}

/*
 * Purpose:
 *      Creates a full-screen sprite from a texture sized to the current window.
 * Input(s):
 *      const Texture&  tex        - source texture
 *      const Vector2u& windowSize - target window dimensions in pixels
 * Output:
 *      Sprite - scaled & positioned at (0,0), ready to draw as a backdrop.
 * Notes:
 *      Scales non-uniformly to exactly fill the window (letterboxing not used).
 */
static Sprite makeBackground(const Texture& tex, const Vector2u& windowSize)
{
    Sprite b(tex);
    
    Vector2u texSize = tex.getSize();                                           // Texture pixel size
                                 
    float scaleX = static_cast<float>(windowSize.x) / texSize.x;                // Compute scale factors for X and Y
    float scaleY = static_cast<float>(windowSize.y) / texSize.y;                // Scale factor = window size / texture size 
    
    b.setScale(scaleX, scaleY);
    b.setPosition(0.f, 0.f);
    
    return b;
}

/*
 * Purpose:
 *      Modal start screen. Waits for Enter to begin, Esc/Close to quit.
 * Input(s):
 *      RenderWindow& window - active SFML window
 *      Sprite ss            - background sprite to draw (by value is fine)
 * Output:
 *      bool - true to start the game; false to quit.
 */
static bool startScreen(RenderWindow& window,
                        Sprite ss) 
{
    while (window.isOpen())                                          
    { // Keep start screen until X, Esc, or Enter  is pressed
        Event e;
        while (window.pollEvent(e))
        { // infinite loop while one of the following options happen
            if (e.type == Event::Closed)
            { // pressed the X in top right of window
                window.close();
                return false;
            }
            
            if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape)
            { // pressed escape to exit
                window.close();
                return false;
            }
        
            if (e.type == Event::KeyPressed && e.key.code == Keyboard::Enter)
            { // pressed enter to begin playing
                return true;
            }
        }
        
        window.clear();
        window.draw(ss);
        window.display();
    }
    return false;
}

/*
 * Purpose:
 *      Modal end/lose screen. Enter returns true to replay; Esc/Close quits.
 * Input(s):
 *      RenderWindow& window - active SFML window
 *      Sprite es            - end screen background sprite
 * Output:
 *      bool - true if user wants to play again; false to quit.
 */
static bool endScreen(RenderWindow& window,
                      Sprite es) 
{
    while (window.isOpen())                                                     
    { // Keep end screen until Esc or X or Enter is pressed
        Event e;
        while (window.pollEvent(e))
        { // Keep start screen until "Enter" is pressed
            if (e.type == Event::Closed)
            { // pressed the X in top right of window
                window.close();
                return false;
            }
            
            if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape)
            { // pressed escape to exit
                window.close();
                return false;
            }
            
            if (e.type == Event::KeyPressed && e.key.code == Keyboard::Enter)
            { // pressed enter to begin playing
                return true;
            }
        }
        
        window.clear();
        window.draw(es);
        window.display();
    }
    return false;
}


/*
 * Purpose:
 *      Modal win screen. Enter returns true to replay; Esc/Close quits.
 * Input(s):
 *      RenderWindow& window - active SFML window
 *      Sprite ws            - win screen background sprite
 * Output:
 *      bool - true if user wants to play again; false to quit.
 * Notes:
 *      Key mapping can be customized (matches endScreen for consistency).
 */
static bool winScreen(RenderWindow& window,
                      Sprite ws) 
{
    while (window.isOpen())
    { // Keep win screen until Esc or X or Enter is pressed
        Event e;
        while (window.pollEvent(e))
        {  // infinite loop while one of the following options happen
            if (e.type == Event::Closed)
            { // pressed the X in top right of window
                window.close();
                return false;
            }
            
            if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape)
            { // pressed escape to exit
                window.close();
                return false;
            }
            
            if (e.type == Event::KeyPressed && e.key.code == Keyboard::Enter)
            { // pressed enter to begin playing
                return true;            
            }
        }
        
        window.clear();
        window.draw(ws);
        window.display();
    }
    return false;
}

/*
 * Purpose:
 *      Populate the enemy swarm in a grid, scaled relative to the window.
 * Input(s):
 *      vector<ECE_Enemy>& enemies - output container (cleared & filled)
 *      const Texture& enemyTex1   - texture used on even rows
 *      const Texture& enemyTex2   - texture used on odd rows
 *      Vector2u windowSize        - window dimensions (for scaling and layout)
 * Output:
 *      None (enemies vector is modified).
 */
static void createEnemies(std::vector<ECE_Enemy>& enemies,
                          const Texture& enemyTex1,
                          const Texture& enemyTex2,
                          Vector2u windowSize)
{
    const int   cols       = 8;                    // # cols
    const int   rows       = 4;                    // # rows
    const float startY     = windowSize.y * 0.65f; // lower half
    const float xPadding   = 120.f;                // x spacing between enemies
    const float yPadding   = 120.f;                // y spacing between enemies
    const float leftMargin = 120.f;                // horizontal starting offset
    const float topMargin  = startY;               // vertical starting offest

    enemies.clear();
    enemies.reserve(cols * rows);   // reserves memory for all enemies

    for (int r = 0; r < rows; ++r)
    { // create enemies row by row
        for (int c = 0; c < cols; ++c)
        { // create enemies in adjacent columns
            const Texture& enemyTex = (r % 2 == 0) ? enemyTex1 : enemyTex2;     // alternate by row
            ECE_Enemy enemy(enemyTex);
            enemy.scaleForWindow(windowSize);                                   // sizes relative to window
            
            float x = leftMargin + c * xPadding;    // calculate x position
            float y = topMargin  + r * yPadding;    // calculate y position
            enemy.setPosition(x, y);
            enemies.push_back(enemy);   // add to container
        }
    }
}


/*
 * Purpose:
 *      Polls the event queue: handles close/escape and spawns player shots.
 * Input(s):
 *      RenderWindow& window               - active window
 *      const ECE_Buzzy& buzzy             - player (for shot spawn position)
 *      list<ECE_LaserBlast>& playerShots  - output list to append new shots
 *      const Texture& laserTex            - laser texture for new shots
 * Output:
 *      None (window state or playerShots may be modified)
 */
static void handleEvents(RenderWindow& window,
                         const ECE_Buzzy& buzzy,
                         std::list<ECE_LaserBlast>& playerShots,
                         const Texture& laserTex)
{
    Event e;
    while (window.pollEvent(e))
    { // infinite loop until one of the following events happen 
        if (e.type == Event::Closed)
        { // pressed the X in top right of window
            window.close();
        }

        if (e.type == Event::KeyPressed) {
            if (e.key.code == Keyboard::Escape)
            { // pressed escape to exit
                window.close();
            }
            
            if (e.key.code == Keyboard::Space)
            { // pressed space bar to spawn a lser
                ECE_LaserBlast newPlayerShot(laserTex, /*fromPlayer=*/true);                        // spawn a player laser heading downward (+Y)
                Vector2f p = buzzy.getPosition();
                newPlayerShot.setPosition(p.x, p.y + buzzy.getGlobalBounds().height * 0.5f + 10.f); // set position of laser to buzzy's tail
                newPlayerShot.setVelocity({0.f, 400.f});                                            // set velocity of player shot +y (down)
                playerShots.push_back(newPlayerShot);                                               // add new player shot to player shots container
            }
        }
    }
}

/*
 * Purpose:
 *      Periodically spawns an enemy laser from a random alive enemy.
 * Input(s):
 *      Clock& enemyShotsClock           - timer used to control cadence
 *      list<ECE_LaserBlast>& enemyShots - where to push the new laser
 *      vector<ECE_Enemy>& enemies       - used to pick a live shooter
 *      const Texture& laserTex          - texture for the new laser
 * Output:
 *      None (enemyShots is modified; clock is restarted).
 */
static void spawnEnemyLaser(Clock& enemyShotsClock,
                            std::list<ECE_LaserBlast>& enemyShots,
                            std::vector<ECE_Enemy>& enemies,
                            const Texture& laserTex)
{
    std::vector<ECE_Enemy*> alive;
    for (auto &enemy : enemies)
    { // loops through all enemies in container
        if(enemy.isAlive())
        { // add current enemy to alive container
            alive.push_back(&enemy);
        }
    }
    
    if(!alive.empty())
    { // executes only if there are alive enemies
        size_t index = std::rand() % alive.size();                                              // forces index into range 0 <= index <= alive.size() - 1
        ECE_Enemy* shooter = alive[index];                                                      // pick random alive enemy
        ECE_LaserBlast newEnemyShot(laserTex, /*fromPlayer=*/false);                            // spawn an enemy laser heading downward (-Y)
        Vector2f p = shooter->getPosition();                                                    // get bounds of alive enemy
        newEnemyShot.setPosition(p.x, p.y + shooter->getGlobalBounds().height * 0.5f + 10.f);   // set shot position
        newEnemyShot.setVelocity({0.f, -300.f});                                                // set shot velocity (-y = up)
        enemyShots.push_back(newEnemyShot);                                                     // add new enemy shot to enemy shots container
    }
    
    enemyShotsClock.restart(); // restart clock interval
}

/*
 * Purpose:
 *      Update player horizontal movement with clamping to window bounds.
 * Input(s):
 *      ECE_Buzzy& buzzy   - player
 *      float dt           - delta time (seconds)
 *      float windowWidth  - width of the playable area (pixels)
 * Output:
 *      None (buzzy position is changed)
 */
static void updateBuzzy(ECE_Buzzy& buzzy,
                        float dt,
                        float windowWidth)
{
    buzzy.update(dt, windowWidth);
}

/*
 * Purpose:
 *      Update laser positions and remove those that leave the screen.
 * Input(s):
 *      list<ECE_LaserBlast>& playerShots - mutable list of player lasers
 *      list<ECE_LaserBlast>& enemyShots  - mutable list of enemy lasers
 *      float dt                          - delta time (seconds)
 *      float windowHeight                - window height (pixels)
 * Output:
 *      None (both lists may erase elements)
 */
static void updateShots(std::list<ECE_LaserBlast>& playerShots,
                        std::list<ECE_LaserBlast>& enemyShots,
                        float dt,
                        float windowHeight)
{
    for (auto it = playerShots.begin(); it != playerShots.end();)
    { // loops through all shots in player shots container using pointers
        it->update(dt);
        if (it->isOffScreen(windowHeight))
        { // removes shot if it goes off screen
            it = playerShots.erase(it);
        }
        else
        { // move to next player shot in player shots container
            ++it;
        }
    }
    
    for (auto it = enemyShots.begin(); it != enemyShots.end(); )
    { // loops through all shots in enemy shots container using pointers
        it->update(dt);
        if (it->isOffScreen(windowHeight))
        { // removes shot if it goes off screen
            it = enemyShots.erase(it);
        }
        else
        { // move to next player shot in player shots container
            ++it;
        }
    }
}

/*
 * Purpose:
 *      March the enemy swarm left/right and step vertically when hitting walls.
 * Input(s):
 *      vector<ECE_Enemy>& enemies - mutable swarm
 *      float dt                   - delta time (seconds)
 *      float windowWidth          - playfield width (pixels)
 *      float& enemySpeedX         - horizontal speed (px/s); passed by ref for tunability
 *      int& dir                   - direction (+1 right, -1 left); flipped on bounce
 *      float stepUp               - vertical step amount when bouncing (negative to move upward)
 * Output:
 *      None (enemies move; dir may flip).
 * Notes:
 *      Uses a predictive clamp (nextLeft/nextRight) to avoid “wall slide”.
 */
static void updateEnemies(std::vector<ECE_Enemy>& enemies,
                          float dt,
                          float windowWidth,
                          float& enemySpeedX,
                          int& dir,
                          float stepUp)
{
    if (enemies.empty())
    { // all enemies are dead, nothing to udate
        return;
    }

    float minLeft  = std::numeric_limits<float>::infinity();  // smallest x-coordinate of any alive enemy's left edge
    float maxRight = -std::numeric_limits<float>::infinity(); // largest x-coordinate of any alive enemy's right edge

    for (const auto& enemy : enemies)
    { // loops through all enemeis in enemies
        if (!enemy.isAlive())
        { // skips if current enemy is dead
            continue;
        }
        auto gb = enemy.getGlobalBounds();
        minLeft  = std::min(minLeft,  gb.left);             // update the smallest left edge seen so far
        maxRight = std::max(maxRight, gb.left + gb.width);  // update the largest right edge seen so far
    }
    if (!std::isfinite(minLeft))
    {
        return; // no alive enemies
    }

    const float dx       = enemySpeedX * dir * dt;
    const float nextLeft  = minLeft  + dx;
    const float nextRight = maxRight + dx;

    if (nextLeft < 0.f || nextRight > windowWidth)
    { // executes if the next update hits a wall
        // Compute a horizontal correction that puts the group just inside the window
        float correctionX = 0.f;
        if (nextLeft < 0.f)
        { // next update hits a wall
            correctionX = -minLeft;                   // push so minLeft == 0
        }
        else
        { // nextRight > windowWidth
            correctionX = windowWidth - maxRight;    // push so maxRight == windowWidth
        }

        for (auto& enemy : enemies)
        { // loops through all the enemies in enemies container
            if (enemy.isAlive())
            { // if the enemy is alive, update vertically and horizontally
                enemy.move(correctionX, stepUp);  // clamp + vertical step
            }
        }
        
        dir *= -1;    // flip once
        return;       // no horizontal move this frame beyond the clamp
    }

    // Normal horizontal move
    for (auto& enemy : enemies)
        { // loops through all the enemies in enemies container
            if (enemy.isAlive())
            { // if the enemy is alive, update horizontally
                enemy.move(dx, 0.f);
            }
        }
}

/*
 * Purpose:
 *      Resolve player-shot vs enemy collisions; kill enemy & remove shot.
 * Input(s):
 *      list<ECE_LaserBlast>& playerShots - player lasers
 *      vector<ECE_Enemy>& enemies        - enemy swarm
 * Output:
 *      None
 */
static void checkPlayerShotCollisions(std::list<ECE_LaserBlast>& playerShots,
                                      std::vector<ECE_Enemy>& enemies) 
{
    for (auto shotIt = playerShots.begin(); shotIt != playerShots.end();)
    { // loops through all shots in player shots container
        bool hitEnemy = false;
        const FloatRect playerShotBounds = shotIt->getGlobalBounds();
        
        for (auto& enemy : enemies)
        { // loops through all enemies in enemies container
            if (enemy.isAlive())
            { // only considers enemies that are alive
                if (playerShotBounds.intersects(enemy.getGlobalBounds()))
                { // kills enemy if the current player shot intersects with the current enemy's bounds
                    enemy.kill();
                    shotIt = playerShots.erase(shotIt);                             // removes the element at the iterator's position and returns a new iterator that points to the element after the erased one
                    hitEnemy = true;
                    break;
                }
            }
        }
        
        if(!hitEnemy)
        { // move on to the next shot in the player shots container
            shotIt++;
        }
    }
}

/*
 * Purpose:
 *      Detect direct player vs enemy sprite overlap (touch = lose).
 * Input(s):
 *      const ECE_Buzzy& buzzy        - player
 *      vector<ECE_Enemy>& enemies    - swarm
 * Output:
 *      bool - true if any alive enemy intersects the player.
 */
static bool checkPlayerEnemyCollision(const ECE_Buzzy& buzzy,
                                     std::vector<ECE_Enemy>& enemies)
{
    const FloatRect buzzyBounds = buzzy.getGlobalBounds();
    
    for (const auto& enemy : enemies)
    { // loops through all the enemies in enemies container
        if(enemy.isAlive() && buzzyBounds.intersects(enemy.getGlobalBounds()))
        { // return true (player and enemy interection) if they intersect, false otherwise
            return true;
        }
    }
    return false;
}

/*
 * Purpose:
 *      Detect enemy-shot vs player collision. Erases the colliding shot.
 * Input(s):
 *      list<ECE_LaserBlast>& enemyShots - enemy lasers (mutable; may erase)
 *      const ECE_Buzzy& buzzy           - player
 * Output:
 *      bool - true if the player was hit this frame.
 */
static bool checkEnemyShotCollisions(std::list<ECE_LaserBlast>& enemyShots,
                                     const ECE_Buzzy& buzzy)
{
    const FloatRect buzzyBounds = buzzy.getGlobalBounds();
    
    for (auto shotIt = enemyShots.begin(); shotIt != enemyShots.end();)
    { // loops through all shots in enemy shots container
        if (buzzyBounds.intersects(shotIt->getGlobalBounds()))
        { // executes if buzzy intersects the bounds of the current enemy shot
            enemyShots.erase(shotIt);
            return true;
        }
        else
        { // moves on to the next shot in the enemy shots container
            shotIt++;
        }
    }
    return false;
}


/*
 * Purpose:
 *      Returns true when all enemies are dead (win condition).
 * Input(s):
 *      vector<ECE_Enemy>& enemies - swarm
 * Output:
 *      bool - true if no enemy is alive; false otherwise.
 */
static bool checkWin(std::vector<ECE_Enemy>& enemies)
{
    for (const auto& enemy : enemies)
    { // loops through all enemies in enemies container
        if(enemy.isAlive())
        { // returns false if any are alive - haven't won yet
            return false;
        }
    }
    
    return true; // returns true if all enemies are killed - win!
}

/*
 * Purpose:
 *      Draw the current frame: background, player, enemies, and lasers.
 * Input(s):
 *      RenderWindow& window                - target window
 *      const Sprite& background            - pre-scaled background
 *      const ECE_Buzzy& buzzy              - player
 *      const vector<ECE_Enemy>& enemies    - swarm
 *      const list<ECE_LaserBlast>& playerShots - player lasers
 *      const list<ECE_LaserBlast>& enemyShots  - enemy lasers
 * Output:
 *      None (renders to window and displays).
 */
static void drawScene(RenderWindow& window,
                      const Sprite& background,
                      const ECE_Buzzy& buzzy,
                      const std::vector<ECE_Enemy>& enemies,
                      const std::list<ECE_LaserBlast>& playerShots,
                      const std::list<ECE_LaserBlast>& enemyShots)
{
    window.clear();
    window.draw(background);
    window.draw(buzzy);

    for (const auto& enemy : enemies)
    { // loops through all enemies in enemies container
        if (enemy.isAlive())
        { // draw the enemy if it is alive
            window.draw(enemy);
        }
    }

    for (const auto& playerShot : playerShots)
    { // loops through all player shots and draws them
        window.draw(playerShot);
    }
    
    for (const auto& enemyShot : enemyShots)
    { // loops through all enemy shots and draws them
        window.draw(enemyShot);
    }

    window.display();
}

/*
 * Purpose:
 *      Runs a single game round (from start screen to win/lose), including
 *      per-round initialization. Returns outcome and whether the user wants
 *      to replay (via end/win screens).
 * Input(s):
 *      RenderWindow& window - active SFML window
 *      const Assets& assets - preloaded textures reused across rounds
 * Output:
 *      GameOutcome - Win/Lose/Quit indicating what happened and replay choice.
 */
GameOutcome playGame(RenderWindow& window, const allTextures& allTextures)
{
    // Build sprites that depend on window size
    Sprite start = makeBackground(allTextures.startTex, window.getSize());
    Sprite end   = makeBackground(allTextures.endTex,   window.getSize());
    Sprite win   = makeBackground(allTextures.winTex,   window.getSize());
    Sprite bg    = makeBackground(allTextures.bgTex,    window.getSize());

    if (!startScreen(window, start))
    {
        return GameOutcome::Quit;
    }

    // --- per-run state ---
    ECE_Buzzy buzzy(allTextures.buzzyTex);
    buzzy.scaleForWindow(window.getSize(), 0.10f, 0.10f);
    buzzy.setPosition(window.getSize().x / 2.f, window.getSize().y * 0.25f);

    std::list<ECE_LaserBlast> playerShots, enemyShots; // declare lists to hold player and enemy shots
    std::vector<ECE_Enemy> enemies;                    // declare vector to hold enemies
    createEnemies(enemies, allTextures.enemy1Tex, allTextures.enemy2Tex, window.getSize());

    float enemySpeedX = 300.f;  // enemy speed (pixels per second)
    int   dir         = +1;     // initial direction is +x (right)
    float stepUp      = -20.f;  // constant for step up after hitting wall

    Clock clock, enemyShotsClock;           // keep track of enemy shots interval
    const float enemyShotsInterval = 0.5f;  // shot happens every .5 seconds

    // --- main run loop ---
    while (window.isOpen())
    {
        handleEvents(window, buzzy, playerShots, allTextures.laserTex);

        if (enemyShotsClock.getElapsedTime().asSeconds() >= enemyShotsInterval)
            spawnEnemyLaser(enemyShotsClock, enemyShots, enemies, allTextures.laserTex);

        float dt = clock.restart().asSeconds();

        updateBuzzy(buzzy, dt, window.getSize().x);
        updateShots(playerShots, enemyShots, dt, window.getSize().y);
        updateEnemies(enemies, dt, window.getSize().x, enemySpeedX, dir, stepUp);

        checkPlayerShotCollisions(playerShots, enemies);

        const bool killedByShot   = checkEnemyShotCollisions(enemyShots, buzzy);
        const bool collidedEnemy  = checkPlayerEnemyCollision(buzzy, enemies);
        if (killedByShot || collidedEnemy)
        {
            // Ask to replay
            const bool again = endScreen(window, end);
            return again ? GameOutcome::Lose : GameOutcome::Quit;
        }

        const bool winNow = checkWin(enemies);
        if (winNow)
        {
            const bool again = endScreen(window, win);
            return again ? GameOutcome::Win : GameOutcome::Quit;
        }

        drawScene(window, bg, buzzy, enemies, playerShots, enemyShots);
    }

    return GameOutcome::Quit; // window closed
}


// --------------------------- main ---------------------------

/*
 * Purpose:
 *      Program entry. Creates window, loads assets once, then runs rounds
 *      until the player chooses to quit.
 * Input(s):
 *      None
 * Output:
 *      int - standard process exit code (0 on normal termination).
 */
int main()
{
    RenderWindow window(VideoMode(1920,1080), "Buzzy_Defender!", Style::Default);

    allTextures allTextures;
    allTextures.startTex.loadFromFile("graphics/Start_Screen.png");
    allTextures.endTex.loadFromFile("graphics/End_Screen.png");
    allTextures.winTex.loadFromFile("graphics/Win_Screen.png");
    allTextures.bgTex.loadFromFile("graphics/background.png");
    allTextures.buzzyTex.loadFromFile("graphics/Buzzy_blue.png");
    allTextures.laserTex.loadFromFile("graphics/laser.png");
    allTextures.enemy1Tex.loadFromFile("graphics/bulldog.png");
    allTextures.enemy2Tex.loadFromFile("graphics/clemson_tigers.png");

    while (window.isOpen())
    {
        GameOutcome r = playGame(window, allTextures);
        if (r == GameOutcome::Quit)
            break; // user chose to quit (Esc/close)
        // otherwise loop and start a fresh round
    }
    return 0;
}