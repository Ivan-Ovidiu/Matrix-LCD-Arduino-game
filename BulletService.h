#include "Game.h"
#include "GameService.h"
#define MAX_BULLETS 5

class BulletService {
public:
    Bullet bullets[MAX_BULLETS];
    unsigned long lastBulletSpawn;
    unsigned long bulletSpawnDelay;
    Game* gameRef;  // Reference to game object so that it has the actual game values

    //blinking state tracking
    bool bulletBlinking[MAX_BULLETS];
    unsigned long lastBlinkTime[MAX_BULLETS];
    int blinkCount[MAX_BULLETS];
    bool blinkOn[MAX_BULLETS];
    int moveCount[MAX_BULLETS];

    BulletService(Game* game) : gameRef(game), lastBulletSpawn(0), bulletSpawnDelay(5000) {}

    void initialize() {
        for (int i = 0; i < MAX_BULLETS; i++) {
            bullets[i].deactivate();
            bulletBlinking[i] = false;
            blinkCount[i] = 0;
            blinkOn[i] = true;
            moveCount[i] = 0;        }
        lastBulletSpawn = millis();
    }

    void spawnBullet() {
        //finding an inactive bullet slot
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (!bullets[i].isActive()) {
                //spawning bullets from a random edge (not corners or  walls)
                int edge = random(0, 4);  // 0=top, 1=bottom, 2=left, 3=right
                int spawnX, spawnY;

                //after choosing the direction form which the bullet is coming,
                // here im randomly choosing which of the 6 available dots is the bullet coming from                
                switch(edge) {
                    case 0: //top edge
                        spawnX = 1;
                        spawnY = random(1, 7);
                        break;
                    case 1: //bottom edge
                        spawnX = 6;
                        spawnY = random(1, 7);
                        break;
                    case 2: //left edge
                        spawnX = random(1, 7);
                        spawnY = 1;
                        break;
                    case 3: //right edge
                        spawnX = random(1, 7);
                        spawnY = 6;
                        break;
                }

                bullets[i].spawn(spawnX, spawnY);
                gameRef->setLed(spawnX, spawnY);
                return;
            }
        }
    }

    bool shouldSpawn(unsigned long currentTime) {
        return (currentTime - lastBulletSpawn >= bulletSpawnDelay);
    }

    void updateSpawnTime(unsigned long currentTime) {
        lastBulletSpawn = currentTime;
    }

    void updateBullets(bool& matrixChanged) {
        unsigned long currentTime = millis();

        for (int i = 0; i < MAX_BULLETS; i++) {
            if (!bullets[i].isActive()) continue;

            int bulletX = bullets[i].getX();
            int bulletY = bullets[i].getY();

            // Handle blinking state
            if (bulletBlinking[i]) {
                if (currentTime - lastBlinkTime[i] >= 100) {  // Blink every 100ms
                    // Toggle LED state
                    if (blinkOn[i]) {
                        gameRef->clearLed(bulletX, bulletY);
                        blinkOn[i] = false;
                    } else {
                        gameRef->setLed(bulletX, bulletY);
                        blinkOn[i] = true;
                    }
                    blinkCount[i]++;
                    lastBlinkTime[i] = currentTime;
                    matrixChanged = true;

                    // After 6 blinks (3 on/off cycles), despawn
                    if (blinkCount[i] >= 6) {
                        gameRef->clearLed(bulletX, bulletY);
                        bullets[i].deactivate();
                        bulletBlinking[i] = false;
                        moveCount[i] = 0;
                        gameRef->bulletsDefeatedInRoom++;

                    }
                }
                continue;  //skip movement while blinking
            }
            
            // Move bullet toward player
if (bullets[i].shouldMove(currentTime)) {
    //clear old position
    gameRef->clearLed(bulletX, bulletY);

    //calculate direction toward player by increasing or decreasing the value with 1 or -1 up || down and left || right each step
    int dx = 0, dy = 0;

    //If the bullet is left of the player, move right (dx = 1) and if right of the player, move left (dx = -1)
    if (bulletX < gameRef->xPos) dx = 1;
    else if (bulletX > gameRef->xPos) dx = -1;

    //If the bullet is above the player, move down (dy = 1) and below, move up (dy = -1)
    if (bulletY < gameRef->yPos) dy = 1;
    else if (bulletY > gameRef->yPos) dy = -1;

    //update bullet position
    bulletX += dx;
    bulletY += dy;
    bullets[i].setPosition(bulletX, bulletY);
    moveCount[i]++;

    //check collision with player
    if (bulletX == gameRef->xPos && bulletY == gameRef->yPos) {
         if (!gameRef->isInvincible) {
        gameRef->takeDamage(1);
        Serial.println(gameRef->playerHealth);
    } else {
        Serial.println("Bullet blocked by invincibility!");
    }
    bullets[i].deactivate();
    }

    //check if bullet hit wall
    else if (bulletX <= 0 || bulletX >= 7 || bulletY <= 0 || bulletY >= 7) {
        bullets[i].deactivate();
    }
    //check if bullet has moved 5 times - start blinking
    else if (moveCount[i] >= 5) {
        bulletBlinking[i] = true;
        lastBlinkTime[i] = currentTime;
        blinkCount[i] = 0;
        blinkOn[i] = true;
        gameRef->setLed(bulletX, bulletY);
    }
    // Draw bullet at new position
    else {
        gameRef->setLed(bulletX, bulletY);
    }

    bullets[i].updateMoveTime(currentTime);
    matrixChanged = true;
            }
        }
    }

    void setSpawnDelay(unsigned long delay) {
        bulletSpawnDelay = delay;
    }
};
