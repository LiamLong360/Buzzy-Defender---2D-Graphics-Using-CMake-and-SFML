/*
Author: Liam Long
Class: ECE4122 
Last Date Modified: 2025-09-15
Description:
Implementation file for the ECE_Enemy class. Defines the constructor and
member functions for the enemy sprite, including scaling to window size
and managing its alive/dead state during gameplay.
*/

#include "ECE_Enemy.h"          // Class declaration and interface
#include <SFML/Graphics.hpp>    // SFML graphics library for Sprite, Texture, Vector2u, etc.
#include <algorithm>            // Provides std::min and other algorithms for scaling calculations

/*
 * Purpose:
 *      Constructor for an enemy sprite that initializes its base sf::Sprite
 *      with the given texture.
 * Input(s):
 *      const Texture& texture - read-only reference to the enemy's image texture
 * Output:
 *      None (constructor).
 */
ECE_Enemy::ECE_Enemy(const sf::Texture& texture) // uses scope resolution operator "::" - only this constructor belogns to class ECE_Enemy
                                                 // constructor that uses a reference to a texture - does NOT change
: sf::Sprite(texture)   // call base-class constructor
                        // When creating an ECE_Enemy, first construct its sf::Sprite part, passing it the texture so the sprite is ready to display that image
{
    const auto b = getLocalBounds();
    // scale/center
    setOrigin(b.width / 2.f,                                                    // Sprite's texture width in pixels
              b.height / 2.f);                                                  // Sprite's texture height in pixels
}

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
void ECE_Enemy::scaleForWindow(const sf::Vector2u& windowSize,
                               float boxFracW,
                               float boxFracH)
{
    const auto b = getLocalBounds();
                                                                                // target box in pixels (fraction of window size)
    const float targetW = windowSize.x * boxFracW;
    const float targetH = windowSize.y * boxFracH;
                                                                                // uniform scale so the sprite fits inside the target box
    const float sx = targetW / b.width;
    const float sy = targetH / b.height;
    const float s  = std::min(sx, sy);                                          // make texture a square & choose minumum of width vs height

    setScale(s, s);
}
 
/*
 * Purpose:
 *      Checks whether the enemy is still alive (not destroyed by a player shot).
 * Input(s):
 *      None
 * Output:
 *      bool - true if the enemy is alive, false otherwise
 */
bool ECE_Enemy::isAlive() const
    {
        return m_alive;
    }
    
/*
 * Purpose:
 *      Marks the enemy as dead.
 * Input(s):
 *      None
 * Output:
 *      None
 */    
void ECE_Enemy::kill()
    {
        m_alive = false;
    }
    
/*
 * Purpose:
 *      Sets the alive/dead state of the enemy explicitly.
 * Input(s):
 *      bool a - true to mark as alive, false to mark as dead
 * Output:
 *      None
 */
void ECE_Enemy::setAlive(bool a)
    {
        m_alive = a;
    }