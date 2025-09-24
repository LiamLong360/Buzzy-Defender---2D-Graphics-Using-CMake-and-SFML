/*
Author: Liam Long
Class: ECE4122 
Last Date Modified: 2025-09-15
Description:
Header file for the ECE_Enemy class. Represents an enemy sprite in the game.
Inherits from Sprite so it can be positioned, drawn, and transformed
like any other SFML sprite while adding enemy-specific state (alive/dead) 
and scaling behavior.
*/

#include <SFML/Graphics.hpp>    // Provides the sf::Sprite, sf::Texture, and related graphics classes

//using namespace for readability
using namespace sf;

class ECE_Enemy : public Sprite
{
public:
    /*
     * Purpose:
     *      Constructor for an enemy sprite that initializes its base sf::Sprite
     *      with the given texture.
     * Input(s):
     *      const Texture& texture - read-only reference to the enemy's image texture
     * Output:
     *      None (constructor).
     */
    explicit ECE_Enemy(const Texture& texture);
    
    /*
     * Purpose:
     *      Scales the enemy sprite so it fits within a target fraction of the window size.
     * Input(s):
     *      const sf::Vector2u& windowSize - current window dimensions in pixels
     *      float boxFracW - fraction of the window width to occupy (default 0.1f)
     *      float boxFracH - fraction of the window height to occupy (default 0.1f)
     * Output:
     *      None
     */
    void scaleForWindow(const sf::Vector2u& windowSize,                         // allow scaling to match window size
                        float boxFracW = 0.1f,                                  // width of enemy texture is 10% of screen width
                        float boxFracH = 0.1f);                                 // width of enemy texture is 10% of screen height
    
    /*
     * Purpose:
     *      Checks whether the enemy is still alive (not destroyed by a player shot).
     * Input(s):
     *      None
     * Output:
     *      bool - true if the enemy is alive, false otherwise
     */
    bool isAlive() const;
    
    /*
     * Purpose:
     *      Marks the enemy as dead.
     * Input(s):
     *      None
     * Output:
     *      None
     */
    void kill();
    
    /*
     * Purpose:
     *      Sets the alive/dead state of the enemy explicitly.
     * Input(s):
     *      bool a - true to mark as alive, false to mark as dead
     * Output:
     *      None
     */
    void setAlive(bool a);
    
private:
    bool m_alive = true;    // flag to track if enemy is alive (default is true)
};