/*
Author: Liam Long
Class: ECE4122
Last Date Modified: 9/22/25
Description:
Implementation file for the ECE_LaserBlast class. Defines the constructor and
the helper function used to detect when a laser sprite has moved off the screen.
*/
#include "ECE_LaserBlast.h"

ECE_LaserBlast::ECE_LaserBlast(const Texture& texture, bool fromPlayer)         // "::" denotes that this is the constructor for ECE_LaserBlase class
                                                                                // member initializer list ":"
                                                                                // const Texture& texture is read-only address for image of laser
                                                                                // fromPlayer - true if player fired it, false if enemy fired
: sf::Sprite(texture), m_fromPlayer(fromPlayer)     // immediately calls constructor of Sprite to add image
                                                    // immediately sets private variable with argument
{
    // Center origin so movement/clamping is symmetric
    auto b = getLocalBounds();
    setOrigin(b.width / 2.f, b.height / 2.f);

    // scale to a thin bolt if texture is large
    setScale(6.f / b.width, 18.f / b.height);
}

/*
 *  Purpose: set the laser's velocity.
 *  Input(s):
 *      Vector2f v – desired velocity (pixels per second)
 *  Output:
 *      None
 */
void ECE_LaserBlast::setVelocity(Vector2f v) 
    {
        m_vel = v;
    }

/*
 *  Purpose: get the laser's velocity.
 *  Input(s):
 *      None
 *  Output:
 *      Vector2f – current velocity (pixels per second)
 */
Vector2f ECE_LaserBlast::getVelocity() const
    {
        return m_vel;
    }

/*
 * Purpose: Update the laser’s position based on elapsed time.
 * Input(s):
 *      float dt – seconds since last frame
 * Output:
 *      None
 * Notes:
 *      Uses Sprite class's inherited move() to add offset = velocity * dt
 */  
void ECE_LaserBlast::update(float dt)
    {
        move(m_vel * dt);
    }
    
/*
 * Purpose: Identify whether the laser was fired by the player.
 * Input(s):
 *      None
 * Output:
 *      bool – true if fired from  player, false if enemy
 */
bool ECE_LaserBlast::fromPlayer() const
    {
        return m_fromPlayer;
    }

bool ECE_LaserBlast::isOffScreen(float windowHeight) const
{   
    auto gb = getGlobalBounds();                                  // rectangle of sprite in global coordinates
    return (gb.top + gb.height < 0.f) || (gb.top > windowHeight); // if the laser has left the screen return true
}

