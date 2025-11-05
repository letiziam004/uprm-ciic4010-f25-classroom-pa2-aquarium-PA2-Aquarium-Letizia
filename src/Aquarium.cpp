#include "Aquarium.h"
#include <cstdlib>
#include "Core.h"


string AquariumCreatureTypeToString(AquariumCreatureType t){
    switch(t){
        case AquariumCreatureType::BiggerFish:
            return "BiggerFish";
        case AquariumCreatureType::NPCreature:
            return "BaseFish";
        case AquariumCreatureType::PowerUp:
            return "PowerUp";
        case AquariumCreatureType::ZigZagFish:
            return "ZigZagFish";
        case AquariumCreatureType::LurkerFish:
            return "LurkerFish";
        case AquariumCreatureType::BlueFish:
            return "BlueFish";
        case AquariumCreatureType::RedFish:
            return "RedFish";
        case AquariumCreatureType::VioletFish:
            return "VioletFish";
        case AquariumCreatureType::Shark:
            return "Shark";
        default:
            return "UknownFish";
    }
}

// PlayerCreature Implementation
PlayerCreature::PlayerCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: Creature(x, y, speed, 10.0f, 1, sprite) {}


void PlayerCreature::setDirection(float dx, float dy) {
    m_dx = dx;
    m_dy = dy;
    normalize();
}

void PlayerCreature::move() {
    m_x += m_dx * m_speed * m_speedMultiplier;
    m_y += m_dy * m_speed * m_speedMultiplier;
    this->bounce();
}

void PlayerCreature::reduceDamageDebounce() {
    if (m_damage_debounce > 0) {
        --m_damage_debounce;
    }
}

void PlayerCreature::update() {
    this->reduceDamageDebounce();
    this->move();
}


void PlayerCreature::draw() const {
    ofLogVerbose() << "PlayerCreature at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;

    // Scale based on power level 
    float scale = 1.0f + m_power * 0.05f; // 5% growth per power level

    if (this->m_damage_debounce > 0) {
        ofSetColor(ofColor::red); // Flash red if hit
    } else {
        ofSetColor(m_tintColor); // Use tint color (set on level-up)
    }

    if (m_sprite) {
        ofPushMatrix();
        ofTranslate(m_x, m_y);
        ofScale(scale, scale);
        m_sprite->draw(0, 0); 
        ofPopMatrix();
    }

    ofSetColor(ofColor::white); // Reset color
}

void PlayerCreature::changeSpeed(int speed) {
    m_speed = speed;
}

void PlayerCreature::loseLife(int debounce) {
    if (m_damage_debounce <= 0) {
        if (m_lives > 0) this->m_lives -= 1;
        m_damage_debounce = debounce; // Set debounce frames
        ofLogNotice() << "Player lost a life! Lives remaining: " << m_lives << std::endl;
    }
    // If in debounce period, do nothing
    if (m_damage_debounce > 0) {
        ofLogVerbose() << "Player is in damage debounce period. Frames left: " << m_damage_debounce << std::endl;
    }
}

// NPCreature Implementation
NPCreature::NPCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: Creature(x, y, speed, 22.f, 1, sprite) {
    if (speed < 1) speed = 1;
    if (speed > 2) speed = 2;
    m_speed = speed;

    m_dx = (rand() % 3 - 1); // -1, 0 o 1
    m_dy = (rand() % 3 - 1);
    if (m_dx == 0 && m_dy == 0) { m_dx = 1; m_dy = 0; } // evita quedar quieto
    normalize();

    m_creatureType = AquariumCreatureType::NPCreature;
}

void NPCreature::move() {
   
    m_x += m_dx * m_speed;
    m_y += m_dy * m_speed;
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }
    bounce();
}

void NPCreature::draw() const {
    ofLogVerbose() << "NPCreature at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    ofSetColor(ofColor::white);
    if (m_sprite) {
        m_sprite->draw(m_x, m_y);
    }
}


BiggerFish::BiggerFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: NPCreature(x, y, speed, sprite) {
     if (speed < 1) speed = 1;
    if (speed > 3) speed = 3;  // más lento
    m_speed = speed;

    m_dx = (rand() % 3 - 1);
    m_dy = (rand() % 3 - 1);
    if (m_dx == 0 && m_dy == 0) { m_dx = -1; m_dy = 0; }
    normalize();

    setCollisionRadius(50); // Bigger fish have a larger collision radius
    m_value = 7; // Bigger fish have a higher value (increased from 5)
    m_creatureType = AquariumCreatureType::BiggerFish;
}

void BiggerFish::move() {
    // Bigger fish might move slower or have different logic
    m_x += m_dx * (m_speed * 0.5); // Moves at half speed
    m_y += m_dy * (m_speed * 0.5);
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }

    bounce();
}

void BiggerFish::draw() const {
    ofLogVerbose() << "BiggerFish at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    this->m_sprite->draw(this->m_x, this->m_y);
}


// AquariumSpriteManager
AquariumSpriteManager::AquariumSpriteManager(){
    this->m_npc_fish = std::make_shared<GameSprite>("base-fish.png", 70,70);
    this->m_big_fish = std::make_shared<GameSprite>("bigger-fish.png", 120, 120);
    this->m_power_up = std::make_shared<GameSprite>("base-fish.png", 50, 50);
    this->m_power_up->setTintColor(ofColor::yellow);
    this->m_zigzag_fish = std::make_shared<GameSprite>("zigzag-fish.png", 70, 70);
    this->m_lurker_fish = std::make_shared<GameSprite>("lurker-fish.png", 70, 70);
    this->m_blue_fish = std::make_shared<GameSprite>("Blue-fish.png", 70, 70);
    this->m_red_fish = std::make_shared<GameSprite>("Red-fish.png", 70, 70);
    this->m_violet_fish = std::make_shared<GameSprite>("Violet-fish.png", 70, 70);
    this->m_shark = std::make_shared<GameSprite>("shark.png", 180, 180); 
}

std::shared_ptr<GameSprite> AquariumSpriteManager::GetSprite(AquariumCreatureType t){
    switch(t){
        case AquariumCreatureType::BiggerFish:
            return std::make_shared<GameSprite>(*this->m_big_fish);
            
        case AquariumCreatureType::NPCreature:
            return std::make_shared<GameSprite>(*this->m_npc_fish);
            
        case AquariumCreatureType::PowerUp:
            return std::make_shared<GameSprite>(*this->m_power_up);
            
        case AquariumCreatureType::ZigZagFish:
            return std::make_shared<GameSprite>(*this->m_zigzag_fish);
            
        case AquariumCreatureType::LurkerFish:
            return std::make_shared<GameSprite>(*this->m_lurker_fish);
            
        case AquariumCreatureType::BlueFish:
            return std::make_shared<GameSprite>(*this->m_blue_fish);
            
        case AquariumCreatureType::RedFish:
            return std::make_shared<GameSprite>(*this->m_red_fish);
            
        case AquariumCreatureType::VioletFish:
            return std::make_shared<GameSprite>(*this->m_violet_fish);
            
        case AquariumCreatureType::Shark:
            return std::make_shared<GameSprite>(*this->m_shark);
            
        default:
            return nullptr;
    }
}


// Aquarium Implementation
Aquarium::Aquarium(int width, int height, std::shared_ptr<AquariumSpriteManager> spriteManager)
    : m_width(width), m_height(height) {
        m_sprite_manager =  spriteManager;
    }



void Aquarium::addCreature(std::shared_ptr<Creature> creature) {
    creature->setBounds(m_width - 20, m_height - 20);
    m_creatures.push_back(creature);
}

void Aquarium::addAquariumLevel(std::shared_ptr<AquariumLevel> level){
    if(level == nullptr){return;} // guard to not add noise
    this->m_aquariumlevels.push_back(level);
}

void Aquarium::update() {
    for (auto& creature : m_creatures) {
        creature->move();
    }
    this->Repopulate();
    
    // ⚡ Power-Up spawning timer (every 20 seconds at 60fps = 1200 frames)
    m_powerUpTimer++;
    if (m_powerUpTimer > 1200) {
        this->SpawnCreature(AquariumCreatureType::PowerUp);
        m_powerUpTimer = 0;
        ofLogNotice() << " Speed Power-Up spawned!";
    }
    
    // ⏱ Power-Up active timer countdown
    if (m_powerUpActiveTimer > 0) {
        m_powerUpActiveTimer--;
        if (m_powerUpActiveTimer == 0) {
            m_speedMultiplier = 1.0f;
            ofLogNotice() << "Speed Power-Up expired.";
        }
    }
}

void Aquarium::draw() const {
    for (const auto& creature : m_creatures) {
        creature->draw();
    }
}


void Aquarium::removeCreature(std::shared_ptr<Creature> creature) {
    auto it = std::find(m_creatures.begin(), m_creatures.end(), creature);
    if (it != m_creatures.end()) {
        ofLogVerbose() << "removing creature " << endl;
        
        // Don't consume power-ups from level population (they have value -999)
        if (creature->getValue() != -999) {
            int selectLvl = this->currentLevel % this->m_aquariumlevels.size();
            auto npcCreature = std::static_pointer_cast<NPCreature>(creature);
            this->m_aquariumlevels.at(selectLvl)->ConsumePopulation(npcCreature->GetType(), npcCreature->getValue());
        }
        
        m_creatures.erase(it);
    }
}

void Aquarium::clearCreatures() {
    m_creatures.clear();
}

std::shared_ptr<Creature> Aquarium::getCreatureAt(int index) {
    if (index < 0 || size_t(index) >= m_creatures.size()) {
        return nullptr;
    }
    return m_creatures[index];
}



void Aquarium::SpawnCreature(AquariumCreatureType type) {
    int x = rand() % this->getWidth();
    int y = rand() % this->getHeight();
    int speed = 1 + rand() % 3; // Speed between 1 and 3 (slower)

    switch (type) {
        case AquariumCreatureType::NPCreature:
            this->addCreature(std::make_shared<NPCreature>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::NPCreature)));
            break;
        case AquariumCreatureType::BiggerFish:
            this->addCreature(std::make_shared<BiggerFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::BiggerFish)));
            break;
        case AquariumCreatureType::ZigZagFish:
            this->addCreature(std::make_shared<ZigZagFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::ZigZagFish)));
            break;
        case AquariumCreatureType::LurkerFish:
            this->addCreature(std::make_shared<LurkerFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::LurkerFish)));
            break;
        case AquariumCreatureType::BlueFish:
            {
                auto fish = std::make_shared<NPCreature>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::BlueFish));
                fish->setValue(3); // Blue fish have value 3
                this->addCreature(fish);
            }
            break;
        case AquariumCreatureType::RedFish:
            {
                auto fish = std::make_shared<NPCreature>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::RedFish));
                fish->setValue(4); // Red fish have value 4
                this->addCreature(fish);
            }
            break;
        case AquariumCreatureType::VioletFish:
            {
                auto fish = std::make_shared<NPCreature>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::VioletFish));
                fish->setValue(5); // Violet fish have value 5
                this->addCreature(fish);
            }
            break;
        case AquariumCreatureType::Shark:
            {
                auto shark = std::make_shared<BiggerFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::Shark));
                shark->setValue(8); // Sharks have highest value (8)
                this->addCreature(shark);
            }
            break;
        case AquariumCreatureType::PowerUp: {
            
            auto sprite = this->m_sprite_manager->GetSprite(AquariumCreatureType::PowerUp);
            float x = ofRandom(0, m_width);
            float y = ofRandom(0, m_height);
            int speed = 3; 
            
            
            auto powerUp = std::make_shared<NPCreature>(x, y, speed, sprite);
            powerUp->setValue(-999);
            powerUp->setCollisionRadius(30.0f);
            this->addCreature(powerUp);
            break;
        }
        default:
            ofLogError() << "Unknown creature type to spawn!";
            break;
    }

}


// repopulation will be called from the levl class
// it will compose into aquarium so eating eats frm the pool of NPCs in the lvl class
// once lvl criteria met, we move to new lvl through inner signal asking for new lvl
// which will mean incrementing the buffer and pointing to a new lvl index
void Aquarium::Repopulate() {
    ofLogVerbose("entering phase repopulation");
    // Loop back to level 1 after completing all levels
    int selectedLevelIdx = this->currentLevel % this->m_aquariumlevels.size();
    ofLogNotice() << "Current level index: " << selectedLevelIdx << " (Level " << (selectedLevelIdx + 1) << ")" << endl;
    std::shared_ptr<AquariumLevel> level = this->m_aquariumlevels.at(selectedLevelIdx);


    if(level->isCompleted()){
        level->levelReset();
        this->currentLevel += 1;
        
        // Check if all levels completed (6 levels: 0-5, so currentLevel == 6 means victory)
        if (this->currentLevel >= this->m_aquariumlevels.size()) {
            ofLogNotice() << "🎉 VICTORY! All levels completed!";
            m_justLeveledUp = true; // Trigger victory display
            return; // Don't repopulate, game is won
        }
        
        // Loop back to the beginning
        selectedLevelIdx = this->currentLevel % this->m_aquariumlevels.size();
        ofLogNotice() << " LEVEL UP! New level reached: " << selectedLevelIdx << " (Level " << (selectedLevelIdx + 1) << ")" << std::endl;
        level = this->m_aquariumlevels.at(selectedLevelIdx);
        this->clearCreatures();

        // Level-Up signal
        m_justLeveledUp = true;
    }

    
    // now lets find how many to respawn if needed 
    std::vector<AquariumCreatureType> toRespawn = level->Repopulate();
    ofLogVerbose() << "amount to repopulate : " << toRespawn.size() << endl;
    if(toRespawn.size() <= 0 ){return;} // there is nothing for me to do here
    for(AquariumCreatureType newCreatureType : toRespawn){
        this->SpawnCreature(newCreatureType);
    }
}


// Aquarium collision detection
std::shared_ptr<GameEvent> DetectAquariumCollisions(std::shared_ptr<Aquarium> aquarium, std::shared_ptr<PlayerCreature> player) {
    if (!aquarium || !player) return nullptr;
    
    for (int i = 0; i < aquarium->getCreatureCount(); ++i) {
        auto npc = aquarium->getCreatureAt(i);
        if (npc && checkCollision(player, npc)) {
            return std::make_shared<GameEvent>(GameEventType::COLLISION, player, npc);
        }
    }
    return nullptr;
};

//  Imlementation of the AquariumScene

void AquariumGameScene::Update(){
     if (!m_aquarium || !m_player) return;

    // 1) mover player
    m_player->update();

    // 2) mover NPCs / repoblar / niveles
    m_aquarium->update();
    
    // Decrement invincibility timer
    if (m_invincibilityTimer > 0) {
        m_invincibilityTimer--;
    }
    
    if (m_aquarium->hasJustLeveledUp()) {
        m_aquarium->clearLevelUpFlag();
        
        // Check if player has completed all levels (victory condition)
        if (m_aquarium->getCurrentLevel() >= m_aquarium->getLevelCount()) {
            m_victoryTimer = 300; // Show victory message for 5 seconds
            m_hasWon = true;
            ofLogNotice() << "🏆 VICTORY! You Won!";
            return; // Stop normal gameplay
        }
        
        // Reset invincibility timer for new level (5 seconds)
        m_invincibilityTimer = 300;
        ofLogNotice() << "🛡️ NEW LEVEL - 5 seconds of invincibility!";
        
        m_levelUpTimer = 180; // spawn message for 3s
        m_lastEvent = std::make_shared<GameEvent>(GameEventType::NEW_LEVEL, m_player, nullptr);

        // Increase player power on level-up
        m_player->increasePower(1);
        
        // Update player tint color and size based on new power level
        int power = m_player->getPower();
        switch(power) {
            case 1: m_player->setTintColor(ofColor::white); break;
            case 2: m_player->setTintColor(ofColor::green); break;
            case 3: m_player->setTintColor(ofColor::cyan); break;
            case 4: m_player->setTintColor(ofColor::yellow); break;
            case 5: m_player->setTintColor(ofColor::orange); break;
            case 6: m_player->setTintColor(ofColor::red); break;
            default: m_player->setTintColor(ofColor::purple); break;
        }
        
        ofLogNotice() << " Player leveled up! Power: " << power << " Size: " << (1.0f + power * 0.05f) << "x";

        // Plays level-Up sound
        if (m_levelUpSound != nullptr) {
            ofLogNotice() << " LEVEL UP! Playing level-up sound...";
            m_levelUpSound->play();
        }
    }

    // 3) detectar colisiones
    auto event = DetectAquariumCollisions(m_aquarium, m_player);
    if (event && event->isCollisionEvent() && event->creatureA && event->creatureB) {
        ofLogNotice() << "⚡ COLLISION DETECTED! Hit sound ptr: " << (void*)m_hitSound;
        auto A = event->creatureA; // player
        auto B = event->creatureB; // npc

        
        if (B->getValue() == -999) {
            ofLogNotice() << " Player picked up Speed Power-Up!";
            
           
            m_player->setSpeedMultiplier(1.5f);
            m_aquarium->setSpeedMultiplier(1.5f);
            m_aquarium->setPowerUpActiveTimer(600); 
            
            m_aquarium->removeCreature(B);
            m_lastEvent = std::make_shared<GameEvent>(GameEventType::POWER_UP_COLLECTED, m_player, B);
            return;
        }

        // normal del impacto
        float nx = A->getX() - B->getX();
        float ny = A->getY() - B->getY();
        float len = std::sqrt(nx*nx + ny*ny);
        if (len > 0.0001f) { nx /= len; ny /= len; }

        const float pushWeak = 20.0f;  // Strong bounce when hitting enemy fish
        const float pushEat  = 4.0f;  // empujón suave al comer

        ofLogNotice() << "🐟 COLLISION! Player power: " << m_player->getPower() << " vs Fish value: " << B->getValue() << " (Type: " << AquariumCreatureTypeToString(std::static_pointer_cast<NPCreature>(B)->GetType()) << ")";

        //  balance: solo come si es estrictamente mayor
        if (m_player->getPower() < B->getValue()) {
            // Check if player is invincible
            if (m_invincibilityTimer > 0) {
                ofLogNotice() << "🛡️ Player is INVINCIBLE! No damage taken. Time left: " << (m_invincibilityTimer / 60.0f) << "s";
                // Still bounce off the fish, but no damage
                A->moveBy( nx * pushWeak,  ny * pushWeak);
                B->moveBy(-nx * pushWeak, -ny * pushWeak);
                A->bounce();
                B->bounce();
                return; // Skip damage
            }
            
            ofLogNotice() << " Player WEAKER - Losing life!";
            // Strong bounce to prevent passing through enemy fish
            A->moveBy( nx * pushWeak,  ny * pushWeak);
            B->moveBy(-nx * pushWeak, -ny * pushWeak);
            A->bounce();
            B->bounce();

            m_player->loseLife(10); // Very short debounce - about 0.16 seconds
            
            // Play hit sound directly
            if (m_hitSound != nullptr) {
                ofLogNotice() << " Playing hit sound directly!";
                m_hitSound->play();
            } else {
                ofLogWarning() << " Hit sound not available!";
            }
            
            // Also emit event for tracking
            m_lastEvent = std::make_shared<GameEvent>(GameEventType::PLAYER_HIT, m_player, B);
            
            if (m_player->getLives() <= 0) {
                ofLogNotice() << "💀 Game Over - No lives left!";
                m_lastEvent = std::make_shared<GameEvent>(GameEventType::GAME_OVER, m_player, nullptr);
                return;
            }
        } else {
            ofLogNotice() << "Player STRONGER or EQUAL - Eating fish!";
            // come → empujón solo al player, luego remove
            A->moveBy(nx * pushEat, ny * pushEat);
            A->bounce();

            m_aquarium->removeCreature(B);
            m_player->addToScore(1, B->getValue());

            m_lastEvent = std::make_shared<GameEvent>(GameEventType::CREATURE_REMOVED, m_player, B);
        }
    }
}

void AquariumGameScene::Draw() {
    // Draw player with blinking effect if invincible
    if (m_invincibilityTimer > 0) {
        // Blink every 10 frames (fast blink)
        if ((m_invincibilityTimer / 10) % 2 == 0) {
            this->m_player->draw();
        }
    } else {
        this->m_player->draw();
    }
    
    this->m_aquarium->draw();
    this->paintAquariumHUD();
    
    // Draw invincibility indicator
    if (m_invincibilityTimer > 0) {
        ofPushStyle();
        ofSetColor(ofColor::cyan);
        float timeLeft = m_invincibilityTimer / 60.0f; // Convert frames to seconds
        string invincText = "INVINCIBLE: " + ofToString(timeLeft, 1) + "s";
        float textWidth = invincText.length() * 8; // Approximate width
        ofDrawBitmapString(invincText, (ofGetWidth() - textWidth) / 2, 30);
        ofPopStyle();
    }
    
    // Draw VICTORY image (takes priority over level-up)
    if (m_victoryTimer > 0) {
        if (m_victoryImage.isAllocated()) {
            ofPushStyle();
            ofSetColor(255, 255, 255, 255); 
            
            // Center and scale the victory image
            float scale = 0.7f; // Slightly larger for victory
            float imgWidth = m_victoryImage.getWidth() * scale;
            float imgHeight = m_victoryImage.getHeight() * scale;
            float x = (ofGetWidth() - imgWidth) / 2;
            float y = (ofGetHeight() - imgHeight) / 2;
            
            ofEnableAlphaBlending();
            m_victoryImage.draw(x, y, imgWidth, imgHeight);
            ofDisableAlphaBlending();
            
            ofPopStyle();
        }
        m_victoryTimer--;
        return; // Don't draw level-up if showing victory
    }
    
    // Draw LEVEL UP image
    if (m_levelUpTimer > 0) {
        if (m_levelUpImage.isAllocated()) {
           
            ofPushStyle();
            ofSetColor(255, 255, 255, 255); 
            
            //size of image 
            float scale = 0.5f;
            float imgWidth = m_levelUpImage.getWidth() * scale;
            float imgHeight = m_levelUpImage.getHeight() * scale;
            float x = (ofGetWidth() - imgWidth) / 2;
            float y = (ofGetHeight() - imgHeight) / 2;
            
            ofEnableAlphaBlending();
            m_levelUpImage.draw(x, y, imgWidth, imgHeight);
            ofDisableAlphaBlending();
            
            ofPopStyle();
        }
        m_levelUpTimer--;
    }

}


void AquariumGameScene::paintAquariumHUD(){
    float panelWidth = ofGetWindowWidth() - 150;
    int currentLevelIndex = m_aquarium->getCurrentLevel() % m_aquarium->getLevelCount();
    ofDrawBitmapString("Level: " + std::to_string(currentLevelIndex + 1), panelWidth, 10);
    ofDrawBitmapString("Score: " + std::to_string(this->m_player->getScore()), panelWidth, 20);
    ofDrawBitmapString("Power: " + std::to_string(this->m_player->getPower()), panelWidth, 30);
    ofDrawBitmapString("Lives: " + std::to_string(this->m_player->getLives()), panelWidth, 40);
    for (int i = 0; i < this->m_player->getLives(); ++i) {
        ofSetColor(ofColor::red);
        ofDrawCircle(panelWidth + i * 20, 50, 5);
    }
    ofSetColor(ofColor::white); // Reset color to white for other drawings
}

void AquariumLevel::populationReset(){
    for(auto node: this->m_levelPopulation){
        node->currentPopulation = 0; // need to reset the population to ensure they are made a new in the next level
    }
}

void AquariumLevel::ConsumePopulation(AquariumCreatureType creatureType, int power){
    for(std::shared_ptr<AquariumLevelPopulationNode> node: this->m_levelPopulation){
        ofLogVerbose() << "consuming from this level creatures" << endl;
        if(node->creatureType == creatureType){
            ofLogVerbose() << "-cosuming from type: " << AquariumCreatureTypeToString(node->creatureType) <<" , currPop: " << node->currentPopulation << endl;
            if(node->currentPopulation == 0){
                return;
            }
            node->currentPopulation -= 1;
            ofLogVerbose() << "+cosuming from type: " << AquariumCreatureTypeToString(node->creatureType) <<" , currPop: " << node->currentPopulation << endl;
            this->m_level_score += power;
            return;
        }
    }
}

bool AquariumLevel::isCompleted(){
    return this->m_level_score >= this->m_targetScore;
}




std::vector<AquariumCreatureType> Level_0::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    for(std::shared_ptr<AquariumLevelPopulationNode> node : this->m_levelPopulation){
        int delta = node->population - node->currentPopulation;
        ofLogVerbose() << "to Repopulate :  " << delta << endl;
        if(delta >0){
            for(int i = 0; i<delta; i++){
                toRepopulate.push_back(node->creatureType);
            }
            node->currentPopulation += delta;
        }
    }
    return toRepopulate;

}

std::vector<AquariumCreatureType> Level_1::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    for(std::shared_ptr<AquariumLevelPopulationNode> node : this->m_levelPopulation){
        int delta = node->population - node->currentPopulation;
        if(delta >0){
            for(int i=0; i<delta; i++){
                toRepopulate.push_back(node->creatureType);
            }
            node->currentPopulation += delta;
        }
    }
    return toRepopulate;
}

std::vector<AquariumCreatureType> Level_2::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    for(std::shared_ptr<AquariumLevelPopulationNode> node : this->m_levelPopulation){
        int delta = node->population - node->currentPopulation;
        if(delta >0){
            for(int i=0; i<delta; i++){
                toRepopulate.push_back(node->creatureType);
            }
            node->currentPopulation += delta;
        }
    }
    return toRepopulate;
}
