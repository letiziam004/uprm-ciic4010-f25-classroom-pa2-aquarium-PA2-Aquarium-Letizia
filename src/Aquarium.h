#pragma once
#define NOMINMAX // To avoid min/max macro conflict on Windows
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#include "Core.h"


enum class AquariumCreatureType {
    NPCreature,
    BiggerFish,
    PowerUp,
    ZigZagFish,
    LurkerFish,
    BlueFish,
    RedFish,
    VioletFish,
    Shark
};

string AquariumCreatureTypeToString(AquariumCreatureType t);

class AquariumLevelPopulationNode{
    public:
        AquariumLevelPopulationNode() = default;
        AquariumLevelPopulationNode(AquariumCreatureType creature_type, int population) {
            this->creatureType = creature_type;
            this->population = population;
            this->currentPopulation = 0;
        };
        AquariumCreatureType creatureType;
        int population;
        int currentPopulation;
};

class AquariumLevel : public GameLevel {
    public:
        AquariumLevel(int levelNumber, int targetScore)
        : GameLevel(levelNumber), m_level_score(0), m_targetScore(targetScore){};
        void ConsumePopulation(AquariumCreatureType creature, int power);
        bool isCompleted() override;
        void populationReset();
        void levelReset(){m_level_score=0;this->populationReset();}
        virtual std::vector<AquariumCreatureType> Repopulate() = 0;
    protected:
        std::vector<std::shared_ptr<AquariumLevelPopulationNode>> m_levelPopulation;
        int m_level_score;
        int m_targetScore;

};


class PlayerCreature : public Creature {
public:

    PlayerCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    void move();
    void draw() const;
    void update();
    void changeSpeed(int speed);
    void setLives(int lives) { m_lives = lives; }
    void setDirection(float dx, float dy);
    float isXDirectionActive() { return m_dx != 0; }
    float isYDirectionActive() {return m_dy != 0; }
    float getDx() { return m_dx; }
    float getDy() { return m_dy; }

    int getScore()const { return m_score; }
    int getLives() const { return m_lives; }
    int getPower() const { return m_power; }
    
    void addToScore(int amount, int weight=1) { m_score += amount * weight; }
    void loseLife(int debounce);
    void increasePower(int value) { m_power += value; }
    void reduceDamageDebounce();
    void setSpeedMultiplier(float mult) { m_speedMultiplier = mult; }
    void setTintColor(const ofColor& color) { m_tintColor = color; }
    
private:
    int m_score = 0;
    int m_lives = 3;
    int m_power = 1; // mark current power lvl
    int m_damage_debounce = 0; // frames to wait after eating
    float m_speedMultiplier = 1.0f;
    ofColor m_tintColor = ofColor::white;
};

class NPCreature : public Creature {
public:
    NPCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    AquariumCreatureType GetType() {return this->m_creatureType;}
    void move() override;
    void draw() const override;
protected:
    AquariumCreatureType m_creatureType;

};

class BiggerFish : public NPCreature {
public:
    BiggerFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    void move() override;
    void draw() const override;
};

class ZigZagFish : public NPCreature {
public:
    ZigZagFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
        : NPCreature(x, y, speed, sprite), counter(0) {
        m_dx = (rand() % 2 == 0) ? 1 : -1;  // Random horizontal direction
        m_dy = (rand() % 2 == 0) ? 1 : -1;  // Random vertical direction
        Creature::setValue(7); 
        m_creatureType = AquariumCreatureType::ZigZagFish;
    }

    void move() override {
        counter++;
        if (counter % 30 == 0) { // Faster zigzag - every 0.5 seconds
            m_dy = -m_dy; // zigzag pattern
        }
        Creature::normalize();
        m_x += m_dx * m_speed;
        m_y += m_dy * m_speed;
        if(m_dx < 0) {
            this->m_sprite->setFlipped(true);
        } else {
            this->m_sprite->setFlipped(false);
        }
        Creature::bounce();
    }
    
    void draw() const override {
        ofSetColor(ofColor::white);
        if (m_sprite) {
            m_sprite->draw(m_x, m_y);
        }
    }

private:
    int counter;
};

class LurkerFish : public NPCreature {
public:
    LurkerFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
        : NPCreature(x, y, speed, sprite), darting(false), dartTimer(0), growthTimer(0), growthCounter(0), currentSize(60) {
        m_dx = (rand() % 2 == 0) ? 1 : -1;  // Random horizontal direction
        m_dy = 0;
        Creature::setValue(6); 
        m_creatureType = AquariumCreatureType::LurkerFish; 
    }

    void move() override {
        dartTimer++;
        growthTimer++;
        
        growthCounter++;
        if (growthCounter % 300 == 0) { // every 5 seconds at 60fps
            m_collisionRadius += 2.0f;
            currentSize += 10; // Also increase visual size
            ofLogNotice() << " LurkerFish growing! Size: " << currentSize << " Radius: " << m_collisionRadius;
        }
        
        // Grow visual size slowly over time
        if (growthTimer % 120 == 0 && currentSize < 120) {
            currentSize += 5;
        }
        
        if (!darting && dartTimer > 180 && ofRandom(1.0) < 0.05f) {
            darting = true;
            dartTimer = 0;
            m_dy = -3;
        }
        if (darting) {
            m_x += m_dx * m_speed;
            m_y += m_dy;
            m_dy += 0.2f;
            if (m_dy > 2) {
                m_dy = 0;
                darting = false;
            }
        } else {
            m_x += m_dx * m_speed;
        }
        if(m_dx < 0) {
            this->m_sprite->setFlipped(true);
        } else {
            this->m_sprite->setFlipped(false);
        }
        Creature::bounce();
    }
    
    void draw() const override {
        ofLogNotice() << "🐟 Drawing LurkerFish at (" << m_x << ", " << m_y << ") size: " << currentSize;
        ofSetColor(ofColor::white);
        if (m_sprite) {
            // Draw with dynamic size
            ofPushMatrix();
            ofTranslate(m_x, m_y);
            float scale = currentSize / 60.0f;
            ofScale(scale, scale);
            m_sprite->draw(0, 0);
            ofPopMatrix();
        } else {
            ofLogError() << "LurkerFish sprite is NULL!";
        }
    }

private:
    bool darting;
    int dartTimer;
    int growthTimer;
    int growthCounter;
    float currentSize;
};


class AquariumSpriteManager {
    public:
        AquariumSpriteManager();
        ~AquariumSpriteManager() = default;
        std::shared_ptr<GameSprite>GetSprite(AquariumCreatureType t);
    private:
        std::shared_ptr<GameSprite> m_npc_fish;
        std::shared_ptr<GameSprite> m_big_fish;
        std::shared_ptr<GameSprite> m_power_up;
        std::shared_ptr<GameSprite> m_zigzag_fish;
        std::shared_ptr<GameSprite> m_lurker_fish;
        std::shared_ptr<GameSprite> m_blue_fish;
        std::shared_ptr<GameSprite> m_red_fish;
        std::shared_ptr<GameSprite> m_violet_fish;
        std::shared_ptr<GameSprite> m_shark;
};


class Aquarium{
public:
    Aquarium(int width, int height, std::shared_ptr<AquariumSpriteManager> spriteManager);
    void addCreature(std::shared_ptr<Creature> creature);
    void addAquariumLevel(std::shared_ptr<AquariumLevel> level);
    void removeCreature(std::shared_ptr<Creature> creature);
    void clearCreatures();
    void update();
    void draw() const;
    void setBounds(int w, int h) { m_width = w; m_height = h; }
    void setMaxPopulation(int n) { m_maxPopulation = n; }
    void Repopulate();
    void SpawnCreature(AquariumCreatureType type);
    
    std::shared_ptr<Creature> getCreatureAt(int index);
    int getCreatureCount() const { return m_creatures.size(); }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    int getCurrentLevel() const { return currentLevel; }
    int getLevelCount() const { return m_aquariumlevels.size(); }
    
    float getSpeedMultiplier() const { return m_speedMultiplier; }
    void setSpeedMultiplier(float mult) { m_speedMultiplier = mult; }
    void setPowerUpActiveTimer(int frames) { m_powerUpActiveTimer = frames; }
    
    bool hasJustLeveledUp() const { return m_justLeveledUp; }
    void clearLevelUpFlag() { m_justLeveledUp = false; }


private:
    int m_maxPopulation = 0;
    int m_width;
    int m_height;
    int currentLevel = 0;
    int m_powerUpTimer = 0;
    int m_powerUpActiveTimer = 0;
    float m_speedMultiplier = 1.0f;
    bool m_justLeveledUp = false;
    std::vector<std::shared_ptr<Creature>> m_creatures;
    std::vector<std::shared_ptr<Creature>> m_next_creatures;
    std::vector<std::shared_ptr<AquariumLevel>> m_aquariumlevels;
    std::shared_ptr<AquariumSpriteManager> m_sprite_manager;
};


std::shared_ptr<GameEvent> DetectAquariumCollisions(std::shared_ptr<Aquarium> aquarium, std::shared_ptr<PlayerCreature> player);


class AquariumGameScene : public GameScene {
    public:
        AquariumGameScene(std::shared_ptr<PlayerCreature> player, std::shared_ptr<Aquarium> aquarium, string name)
        : m_player(std::move(player)) , m_aquarium(std::move(aquarium)), m_name(name){}
        std::shared_ptr<GameEvent> GetLastEvent(){return m_lastEvent;}
        void SetLastEvent(std::shared_ptr<GameEvent> event){this->m_lastEvent = event;}
        std::shared_ptr<PlayerCreature> GetPlayer(){return this->m_player;}
        std::shared_ptr<Aquarium> GetAquarium(){return this->m_aquarium;}
        string GetName()override {return this->m_name;}
        void Update() override;
        void Draw() override;
        void SetHitSound(ofSoundPlayer* sound) { m_hitSound = sound; }
        void SetLevelUpSound(ofSoundPlayer* sound) { m_levelUpSound = sound; }
        void PreloadLevelUpImage() { 
            if (!m_levelUpImage.isAllocated()) {
                m_levelUpImage.load("LevelUp!.png"); 
            }
        }
        void PreloadVictoryImage() { 
            if (!m_victoryImage.isAllocated()) {
                m_victoryImage.load("You Won.png"); 
            }
        }
        bool isPlayerInvincible() const { return m_invincibilityTimer > 0; }
        void resetInvincibility() { m_invincibilityTimer = 300; } // 5 seconds
    private:
        void paintAquariumHUD();
        std::shared_ptr<PlayerCreature> m_player;
        std::shared_ptr<Aquarium> m_aquarium;
        std::shared_ptr<GameEvent> m_lastEvent;
        string m_name;
        AwaitFrames updateControl{5};
        ofSoundPlayer* m_hitSound = nullptr;
        ofSoundPlayer* m_levelUpSound = nullptr;
        int m_levelUpTimer = 0; 
        ofImage m_levelUpImage;
        int m_victoryTimer = 0;
        ofImage m_victoryImage;
        int m_invincibilityTimer = 0; // No invincibility at start, only on level-ups
        bool m_hasWon = false;
};


class Level_0 : public AquariumLevel  {
    public:
        Level_0(int levelNumber, int targetScore): AquariumLevel(levelNumber, targetScore){
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::NPCreature, 15)); // Increased from 10
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::ZigZagFish, 5)); // Increased from 3
        };
        std::vector<AquariumCreatureType> Repopulate() override;

};
class Level_1 : public AquariumLevel  {
    public:
        Level_1(int levelNumber, int targetScore): AquariumLevel(levelNumber, targetScore){
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::NPCreature, 20)); // Increased from 15
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::ZigZagFish, 8)); // Increased from 5
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::LurkerFish, 3)); // Increased from 2
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::BlueFish, 5)); // Increased from 3
        };
        std::vector<AquariumCreatureType> Repopulate() override;


};
class Level_2 : public AquariumLevel  {
    public:
        Level_2(int levelNumber, int targetScore): AquariumLevel(levelNumber, targetScore){
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::NPCreature, 25)); // Increased from 20
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::BiggerFish, 7)); // Increased from 5
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::ZigZagFish, 6)); // Increased from 3
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::LurkerFish, 5)); // Increased from 3
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::RedFish, 6)); // Increased from 4
        };
        std::vector<AquariumCreatureType> Repopulate() override;
};

class Level_3 : public AquariumLevel  {
public:
    Level_3(int levelNumber, int targetScore)
    : AquariumLevel(levelNumber, targetScore) {
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::NPCreature, 25)); // Increased from 20
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::ZigZagFish, 8)); // Increased from 5
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::LurkerFish, 6)); // Increased from 4
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::VioletFish, 5)); // Increased from 3
    }

    std::vector<AquariumCreatureType> Repopulate() override {
        std::vector<AquariumCreatureType> toRepopulate;
        for (auto& node : m_levelPopulation) {
            int delta = node->population - node->currentPopulation;
            if (delta > 0) {
                toRepopulate.insert(toRepopulate.end(), delta, node->creatureType);
                node->currentPopulation += delta;
            }
        }
        return toRepopulate;
    }
};

class Level_4 : public AquariumLevel  {
public:
    Level_4(int levelNumber, int targetScore)
    : AquariumLevel(levelNumber, targetScore) {
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::NPCreature, 20)); // Increased from 15
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::ZigZagFish, 10)); // Increased from 7
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::BiggerFish, 8)); // Increased from 6
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::BlueFish, 6)); // Increased from 4
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::Shark, 4)); // Increased from 2
    }

    std::vector<AquariumCreatureType> Repopulate() override {
        std::vector<AquariumCreatureType> toRepopulate;
        for (auto& node : m_levelPopulation) {
            int delta = node->population - node->currentPopulation;
            if (delta > 0) {
                toRepopulate.insert(toRepopulate.end(), delta, node->creatureType);
                node->currentPopulation += delta;
            }
        }
        return toRepopulate;
    }
};

class Level_5 : public AquariumLevel  {
public:
    Level_5(int levelNumber, int targetScore)
    : AquariumLevel(levelNumber, targetScore) {
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::NPCreature, 15)); // Increased from 10
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::BiggerFish, 10)); // Increased from 7
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::LurkerFish, 8)); // Increased from 5
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::RedFish, 5)); // Increased from 3
        m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::Shark, 6)); // Increased from 4
    }

    std::vector<AquariumCreatureType> Repopulate() override {
        std::vector<AquariumCreatureType> toRepopulate;
        for (auto& node : m_levelPopulation) {
            int delta = node->population - node->currentPopulation;
            if (delta > 0) {
                toRepopulate.insert(toRepopulate.end(), delta, node->creatureType);
                node->currentPopulation += delta;
            }
        }
        return toRepopulate;
    }
};