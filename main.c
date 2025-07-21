#define _CRT_SECURE_NO_WARNINGS
#include <omp.h>
#include "Dependecies.h"
#include "main.h"


//Creates the whole terrain texture, speeding up Draw calls.
//Initializes TerrainCells and trees array. Assigns proprieties to each tile.
RenderTexture2D instantiate_Terrain() {
    int x = 0;
    int y = 0;
    RenderTexture2D terrainTex = LoadRenderTexture(WORLD_WIDTH * TERRAIN_CELL_SIZE, WORLD_HEIGHT * TERRAIN_CELL_SIZE);
    
    TerrainCells = (LandCell*)malloc(sizeof(LandCell)*WORLD_WIDTH*WORLD_HEIGHT);
    if (TerrainCells == NULL) {
        printf("TerrainCells malloc failed! in instantiate_Terrain()\n");
        exit(-1);
    }

    trees = (Tree*)malloc(sizeof(Tree) * WORLD_WIDTH * WORLD_HEIGHT);
    if (trees == NULL) {
        printf("trees malloc failed! in instantiate_Terrain()\n");
        exit(-1);
    }

    BeginTextureMode(terrainTex);
    ClearBackground((Color) { 255, 255, 255, 255 });
    srand((unsigned)time(NULL));
    SEED = (int)GetTime();

    //if there are bugs, i changed x and y: before it was while x ... while y
    while (y < WORLD_HEIGHT) {
        while (x < WORLD_WIDTH) {
            float jitterX = (float)(rand() % 100 - 50) / 20.0f;
            float jitterY = (float)(rand() % 100 - 50) / 20.0f;
            float perl = (perlin2d(x+jitterX, y+jitterY, 0.003f, 3) + perlin2d(x+1000.0f, y+1000.0f, 0.006f, 1))/2.0f;
            
            LandCell currCell;
            Vector2 pos = (Vector2){(float)x,(float)y};
            currCell.position = pos;
            currCell.index = x + y * WORLD_WIDTH;

            //Terrain types division
            if (perl >= TerrainGroups[0]->minValZoneColor && perl < TerrainGroups[0]->maxValZoneColor) {
                currCell.group = TerrainGroups[0];
            }
            else if (perl >= TerrainGroups[1]->minValZoneColor && perl < TerrainGroups[1]->maxValZoneColor) {
                currCell.group = TerrainGroups[1];
            }
            else if (perl >= TerrainGroups[2]->minValZoneColor && perl < TerrainGroups[2]->maxValZoneColor) {
                currCell.group = TerrainGroups[2];
            }
            else if (perl >= TerrainGroups[3]->minValZoneColor && perl < TerrainGroups[3]->maxValZoneColor) {
                currCell.group = TerrainGroups[3];
                
            }
            else if (perl >= TerrainGroups[4]->minValZoneColor && perl < TerrainGroups[4]->maxValZoneColor) {
                currCell.group = TerrainGroups[4];
            }
            
            //DEBUG INFO
            
            //DrawText(TextFormat("x:%i", x* TERRAIN_CELL_SIZE), x* TERRAIN_CELL_SIZE, 10 + y * TERRAIN_CELL_SIZE, 1, DARKGRAY);
            //DrawText(TextFormat("y:%i", y* TERRAIN_CELL_SIZE), x* TERRAIN_CELL_SIZE, y* TERRAIN_CELL_SIZE, 1, DARKGRAY);
            //DrawText(TextFormat("%i", x + y * WORLD_WIDTH), x* TERRAIN_CELL_SIZE, y* TERRAIN_CELL_SIZE, 1, DARKGRAY);

            
            
            //Tree spawn
            currCell.cellTree = NULL;
            float randomTreeChance = ((float)rand() / RAND_MAX) * (currCell.group->proprieties.plantDevelopMult);
            float inner_rand = (float)rand() / RAND_MAX;
            if (randomTreeChance > 0.99 && inner_rand > 0.7)
            {
                Tree currtree;
                currtree.assignedCell = &TerrainCells[terrainCellsN];
                currtree.energyValue = currCell.group->proprieties.energyPerPlantMult;
                currtree.worldPosition = (Vector2){ currCell.position.x * TERRAIN_CELL_SIZE, currCell.position.y * TERRAIN_CELL_SIZE };
                currtree.treeEnergy = TREE_ENERGY;
                currtree.DEBUG = 0;
                
                
                //Tree zone index
                int xZone = (int)floor(currtree.worldPosition.x / (2 * ENTITY_SIGHT_MULTIPLYER));
                int yZone = (int)floor(currtree.worldPosition.y / (2 * ENTITY_SIGHT_MULTIPLYER));
                int index = xZone + yZone * (int)((WORLD_WIDTH * TERRAIN_CELL_SIZE) / (2 * ENTITY_SIGHT_MULTIPLYER)); //TODO: FIRST OF EVERY ROW HAS SAME AS LAST OF PREVIOUS
                currtree.zoneIndex = index;
                trees[treesN] = currtree;
                currCell.cellTree = &trees[treesN];
                treesN++;
            }
            DrawRectangle(x* TERRAIN_CELL_SIZE, y* TERRAIN_CELL_SIZE, TERRAIN_CELL_SIZE, TERRAIN_CELL_SIZE, currCell.group->landColor);
            //Add cell to terrain array
            TerrainCells[terrainCellsN] = currCell;
            terrainCellsN++;
            x++;
        }
        y++;
        x = 0;
    }
    EndTextureMode();

    Tree* resizedTreeS = realloc(trees, treesN * sizeof(Tree));
    if (resizedTreeS == NULL) {
        printf("trees realloc failed! in instantiate_Terrain()\n");
        free(trees);
        exit(-1);
    }
    trees = resizedTreeS;


    return terrainTex;
}

//Generates a single entity (both cell starting stats and genome). 
//For debug purposes, "custom" argument is passed. 0, random instantiation. 1, custom instantiation
Entity generate_Entity(int custom, int prey, float speed, float velocity, float acceleration, float sightSize, float reproductionSpeed, float agingSpeed, Vector2 customPosition) {
    Vector2 position;
    position.x = (float)GetRandomValue(0, WORLD_WIDTH * TERRAIN_CELL_SIZE);
    position.y = (float)GetRandomValue(0, WORLD_WIDTH * TERRAIN_CELL_SIZE);

    Entity currEntity;
    currEntity.consumedEnergyThisTurn = 0;
    currEntity.energyBalance = STARTING_ENERGY_BALANCE;
    currEntity.reproductionAdvancment = STARTING_REPRODUCTION_ADVANCMENT;
    currEntity.ageAdvancment = STARTING_AGE_ADVANCMENT;
    currEntity.color = BLACK;
    currEntity.velocity = (Vector2){ 0,0 };
    Genome genome;

    //Allows for custom entity stats if custom == 1
    if (custom == 0) {
        currEntity.prey = GetRandomValue(0, 1);
        genome.speed = (float)rand() / RAND_MAX;
        genome.velocity = (float)rand() / RAND_MAX;
        genome.acceleration = (float)rand() / RAND_MAX;
        genome.sightSize = (float)rand() / RAND_MAX;
        genome.reproductionSpeed = (float)rand() / RAND_MAX;
        genome.agingSpeed = (float)rand() / RAND_MAX;
        currEntity.position = position;
    }
    else {
        currEntity.prey = prey;
        genome.speed = speed;
        genome.velocity = velocity;
        genome.acceleration = acceleration;
        genome.sightSize = sightSize;
        genome.reproductionSpeed = reproductionSpeed;
        genome.agingSpeed = agingSpeed;
        currEntity.position = customPosition;
    }
    currEntity.genome = genome;

    //Assigns a custom color to each entity based on it's genome traits
    float geneSum = genome.speed + genome.velocity + genome.acceleration + genome.sightSize + genome.reproductionSpeed + genome.agingSpeed;
    currEntity.color = get_Unique_Color(geneSum, GENOME_TRAITS);

    int xZone = (int)floor(currEntity.position.x / (2 * ENTITY_SIGHT_MULTIPLYER));
    int yZone = (int)floor(currEntity.position.y / (2 * ENTITY_SIGHT_MULTIPLYER));
    int index = xZone + yZone * (int)((WORLD_WIDTH * TERRAIN_CELL_SIZE) / (2 * ENTITY_SIGHT_MULTIPLYER)); //TODO: FIRST OF EVERY ROW HAS SAME AS LAST OF PREVIOUS
    currEntity.zoneIndex = index;
    currEntity.debug = 0;
    currEntity.passingCellIndex = -1;

    return currEntity;
}

//Instantiates initial entities, and initializes entities array
void instantiate_Entities_Start() {
    int i = 0;
    entities = (Entity*)malloc(sizeof(Entity) * MAX_ENTITIES);
    if (entities == NULL) {
        printf("Entities malloc failed! in instantiate_Entities_Start()\n");
        return;
    }
    //Creates entity zones array, used to store entities in a certain zone. Not yet implemented, probably high gains in fps.
    entitiesZones = (Entity**)malloc(sizeof(Entity*) * (int)((WORLD_WIDTH * TERRAIN_CELL_SIZE) / (2 * ENTITY_SIGHT_MULTIPLYER)) * (int)((WORLD_WIDTH * TERRAIN_CELL_SIZE) / (2 * ENTITY_SIGHT_MULTIPLYER)));

    while (i < STARTING_ENTITIES) {
        Entity curr = generate_Entity(0, 0, 0, 0, 0, 0, 0, 0, (Vector2) { 0, 0 });
        entities[i] = curr;
        i++;
    }
    entitiesN = STARTING_ENTITIES;
}

//Creates proprieties for terrain types, used at beginnig of main call
LandProprieties create_Proprieties(float plantDevelop, float speedMultPred, float energyConsMultPred, float speedMultPrey, float energyConsMultPrey, float reproductionSpeedMult, float energyPerPlantMult, float geneticChangeProbMult, float agingMult) {
    LandProprieties currProp;
    currProp.plantDevelopMult = plantDevelop;
    currProp.speedMultPred = speedMultPred;
    currProp.energyConsMultPred = energyConsMultPred;
    currProp.speedMultPrey = speedMultPrey;
    currProp.energyConsMultPrey = energyConsMultPrey;
    currProp.reproductionSpeedMult = reproductionSpeedMult;
    currProp.energyPerPlantMult = energyPerPlantMult;
    currProp.geneticChangeProbMult = geneticChangeProbMult;
    currProp.agingMult = agingMult;
    return currProp;
}

//Draws all entities.
void draw_Entities() {
    int i = 0;
    while (i < entitiesN) {
        float size = ((entities[i].energyBalance + abs(MIN_ENERGY_BALANCE)) / MAX_ENERGY_BALANCE) * ENTITY_SIZE;
        DrawCircleV(entities[i].position, size, entities[i].color);
        
        if (entities[i].prey == 0) {
            DrawRing(entities[i].position, size, size+5, 0, 360, 20, RED);
        }

        //DEBUG
        if (entities[i].debug == 1) {
            char xpos[10];
            _itoa(entities[i].position.x, xpos, 10);

            char ypos[10];
            _itoa(entities[i].position.y, ypos, 10);
            
            char energy[10];
            _itoa(entities[i].energyBalance, energy, 10);
            

            DrawText(xpos, entities[i].position.x-20, entities[i].position.y, 10, BLACK);
            DrawText(ypos, entities[i].position.x+10, entities[i].position.y, 10, BLACK);
            DrawText(energy, entities[i].position.x, entities[i].position.y -10, 10, BLACK);
            float sightMult = 0;
            if (entities[i].prey == 1) sightMult = 1;
            else sightMult = 0.75;
            DrawCircleLinesV(entities[i].position, entities[i].genome.sightSize*ENTITY_SIGHT_MULTIPLYER*sightMult, YELLOW);
        }

        DrawText(TextFormat("%i", entities[i].passingCellIndex), entities[i].position.x, entities[i].position.y, 1, DARKGRAY);
        DrawText(TextFormat("%f", entities[i].velocity.x), entities[i].position.x+5, entities[i].position.y-10, 1, RED);

        i++;

        /* Other momentary debug infos
        */
    }
}

//Draws trees.
void draw_Trees() {
    int i = 0;
    while (i < treesN) {
        float size = (trees[i].treeEnergy / TREE_ENERGY) * TREE_SIZE;
        DrawCircle(trees[i].assignedCell->position.x * TERRAIN_CELL_SIZE, trees[i].assignedCell->position.y * TERRAIN_CELL_SIZE, size, DARKGREEN);

        //Debug infos
        char index[20];
        _itoa(trees[i].treeEnergy, index, 10);
        DrawText(index, trees[i].worldPosition.x, trees[i].worldPosition.y, 10, BLACK);
        
        i++;
    }
}

//Very simple close zones alghorithm. Function generated with chatgpt, too lazy.
int find_Close_Zones(int zone, int* out) {
    
    int count = 0;
    int width = (int)((WORLD_WIDTH * TERRAIN_CELL_SIZE) / (2 * ENTITY_SIGHT_MULTIPLYER));
    int height = (int)((WORLD_WIDTH * TERRAIN_CELL_SIZE) / (2 * ENTITY_SIGHT_MULTIPLYER));

    int x = (zone - 1) % width;
    int y = (zone - 1) / width;

    for (int dy = -1; dy <= 1; dy++) {
        int ny = y + dy;
        if (ny < 0 || ny >= height) continue;

        for (int dx = -1; dx <= 1; dx++) {
            int nx = x + dx;
            if (nx < 0 || nx >= width) continue;

            int index = ny * width + nx + 1;
            if (index > 0)
                out[count++] = index;
        }
    }
    
    for (int i = count; i < 9; i++)
    {
        out[i] = -1;
    }

    return count;
}

//Find plants within ENTITY_SIGHT_MULTIPLYER
void find_Nearby_Plants(int index, int* closePlants, int* closePlantN, int* closestPlant, float *plantDistanceClosest) {
    int i = 0;
    int neighboursIndex = 0;

    int closest = NULL;
    int closestDistance = NULL;
    int distance = INFINITY;
    
    int* close = (int*)malloc(sizeof(int) * 9);
    if (close == NULL) return;

    int n = find_Close_Zones(entities[index].zoneIndex, close);

    
    while (i < treesN && neighboursIndex < MAX_NEIGHBOURS_PLANT_SIZE) {
        //if (is_In_Array(close, n, trees[i].zoneIndex)) { FIX THIS!!
        distance = calc_Distance(entities[index].position, trees[i].worldPosition);
        //Distance is squared. Could use Pow, but i think it's slower.
        if (distance < (ENTITY_SIGHT_MULTIPLYER * entities[index].genome.sightSize) * (ENTITY_SIGHT_MULTIPLYER * entities[index].genome.sightSize)) {
            closePlants[neighboursIndex] = i;
            neighboursIndex++;
            if (closest == NULL || distance < closestDistance) {
                closest = i;
                closestDistance = distance;
            }
        }
        //}

        i++;
    }
    *closePlantN = neighboursIndex;
    *closestPlant = closest;
    *plantDistanceClosest = closestDistance;
}

//Find entities within ENTITY_SIGHT_MULTIPLYER, returns ScanInfo.
void find_Nearby_Entities(int index, int* closeEntities, int* predators, int* preys, int* closeN, int* closePraysN, int* closePredatorsN, int* closestPrey, float *closestPrayDistance) {
    int i = 0; 
    int neighboursIndex = 0;
    int preyIndex = 0;
    int predatorIndex = 0;
    
    int closest = -1;
    float closestDistance = INFINITY;
    float distance = INFINITY;

    int *close = (int*)malloc(sizeof(int) * 9);
    if (close == NULL) return NULL;

    float sightMultiplier = entities[index].prey ? 1.0f : PREDATORS_SIGHT_MULTIPLIER;
    float sightRadius = ENTITY_SIGHT_MULTIPLYER * entities[index].genome.sightSize * sightMultiplier;
    float sightRadiusSq = sightRadius * sightRadius;

    int n = find_Close_Zones(entities[index].zoneIndex, close);
    while(i<entitiesN && neighboursIndex<MAX_NEIGHBOURS_SIZE){
        if (entities[i].energyBalance >= MIN_ENERGY_BALANCE) { //is_In_Array(close, n, entities[i].zoneIndex) == 1 &&  FIX THIS!!
            distance = calc_Distance(entities[index].position, entities[i].position);
            
            if (distance < sightRadiusSq && i != index) {
                
                closeEntities[neighboursIndex] = i;
                neighboursIndex++;

                //Different methods for predators and preys.
                if (entities[i].prey == 0) {
                    predators[predatorIndex] = i;
                    predatorIndex++;
                }
                else {
                    preys[preyIndex] = i;
                    preyIndex++;
                    if (closest == -1 || distance < closestDistance) {
                        closest = i;
                        closestDistance = distance;
                    }
                }
            }
        }
        
        i++;
    }

    *closeN = neighboursIndex;
    *closePraysN = preyIndex;
    *closePredatorsN = predatorIndex;
    *closestPrey = closest;
    *closestPrayDistance = closestDistance;
}

//Returns bonuses to be applied to the cell, based on the type of terrain it's currently walking on.
LandProprieties calculate_Terrain_Bonuses(int entityIndex) {
    LandProprieties Bonuses = create_Proprieties(1, 1, 1, 1, 1, 1, 1, 1, 1);
    if (entities[entityIndex].passingCellIndex != -1 && entities[entityIndex].passingCellIndex > 0 && entities[entityIndex].passingCellIndex < WORLD_WIDTH * WORLD_HEIGHT) {
        Bonuses = TerrainCells[entities[entityIndex].passingCellIndex].group->proprieties;
    }
    return Bonuses;
}

//Returns energy consumed this iteration.
float calculate_Energy_Consumption(int entityIndex, LandProprieties Bonuses) {
    float consumptionSum = entities[entityIndex].genome.speed + entities[entityIndex].genome.acceleration + entities[entityIndex].genome.sightSize + entities[entityIndex].genome.reproductionSpeed - entities[entityIndex].genome.reproductionSpeed;
    float consumptionDelta = (consumptionSum / GENOME_TRAITS) * ENTITIES_CONSUMPTION_MULTIPLIER;
    //move it in other if to improve performance (slightly)
    if (entities[entityIndex].prey == 1) consumptionDelta *= Bonuses.energyConsMultPrey;
    else consumptionDelta *= Bonuses.energyConsMultPred;
    return consumptionDelta;
}

//Returns the current zone an entity is in. Used to calculate distances.
int calculate_Updated_Zone(int entityIndex) {
    int xZone = (int)(entities[entityIndex].position.x / (2 * ENTITY_SIGHT_MULTIPLYER));
    int yZone = (int)(entities[entityIndex].position.y / (2 * ENTITY_SIGHT_MULTIPLYER));
    int index = xZone + yZone * (int)((WORLD_WIDTH * TERRAIN_CELL_SIZE) / (2 * ENTITY_SIGHT_MULTIPLYER));
    return index;
}

//Updates entity position, going towards closest plant/entity.
void update_Entity_Position(int i, LandProprieties Bonuses) {
    //Close entities / plants arrays
    int closePredatorsIndexes[MAX_NEIGHBOURS_SIZE];
    int closePreysIndexes[MAX_NEIGHBOURS_SIZE];
    int closeEntitiesIndexes[MAX_NEIGHBOURS_SIZE];
    int closePlantIndexes[MAX_NEIGHBOURS_PLANT_SIZE];

    //Close arrays infos
    int closeEntitiesN = 0;
    int closePredatorsN = 0;
    int closePreysN = 0;
    int closestPlantsN = 0;

    //Closest entities info
    int closestPreyIndex = -1;
    int closestPlantIndex = -1;
    float closestPlantDistance = INFINITY;
    float closestPreyDistance = INFINITY;
    float maxVelocity = ENTITY_MAX_SPEED * Bonuses.speedMultPred * entities[i].genome.velocity;

    find_Nearby_Entities(i, closeEntitiesIndexes, closePredatorsIndexes, closePreysIndexes, &closeEntitiesN, &closePreysN, &closePredatorsN, &closestPreyIndex, &closestPreyDistance);
    find_Nearby_Plants(i, closePlantIndexes, &closestPlantsN, &closestPlantIndex, &closestPlantDistance);

    //TO BE LATER IMPLEMENTED IN SEPARATE FUNCTION!
    if (entities[i].prey == 1) {
        if (closePredatorsN != 0) {
            float maxDistance = entities[i].genome.sightSize * ENTITY_SIGHT_MULTIPLYER;
            float maxDistanceSquared = maxDistance * maxDistance;
            Vector2 velocityChange = (Vector2){ 0,0 };

            for (int j = 0; j < closePredatorsN; j++) {
                float cDist = calc_Distance(entities[i].position, entities[closePredatorsIndexes[j]].position);
                float velocityMultiplier = maxVelocity - maxVelocity * sqrtf(cDist/maxDistance);
                Vector2 direction = (Vector2){ -entities[closePredatorsIndexes[j]].position.x + entities[i].position.x, - entities[closePredatorsIndexes[j]].position.y + entities[i].position.y };
                direction = Vector2Normalize(direction);
                
                velocityChange.x += fabs(velocityMultiplier) * direction.x * DELTA_TIME;
                velocityChange.y += fabs(velocityMultiplier) * direction.y * DELTA_TIME;

            }

            if (fabs(entities[i].velocity.x) < ENTITY_MAX_SPEED) entities[i].velocity.x += velocityChange.x;
            if (fabs(entities[i].velocity.y) < ENTITY_MAX_SPEED) entities[i].velocity.y += velocityChange.y;
        }
        
        entities[i].velocity.x = entities[i].velocity.x * ENTITY_SPEED_DECAY;
        entities[i].velocity.y = entities[i].velocity.y * ENTITY_SPEED_DECAY;


        entities[i].position = Vector2Add(entities[i].position, Vector2Scale(entities[i].velocity, DELTA_TIME));
    
    }
    
    /*
    if(entities[i].prey == 0) {
        if (closePreysN != 0) {
            DrawLineV(entities[i].position, entities[closestPreyIndex].position, RED);
            Vector2 direction = Vector2Subtract(entities[closestPreyIndex].position, entities[i].position);
            direction = Vector2Normalize(direction);
            entities[i].position = Vector2Add(entities[i].position, Vector2Scale(direction, DELTA_TIME * Bonuses.speedMultPred * entities[i].genome.speed * 2));
            if (closestPreyDistance < PREY_ASSIMILATION_DISTANCE * TERRAIN_CELL_SIZE && entities[closestPreyIndex].energyBalance > MIN_ENERGY_BALANCE && entities[i].energyBalance < MAX_ENERGY_BALANCE) {
                entities[closestPreyIndex].energyBalance -= 1 * DELTA_TIME;
                entities[i].energyBalance += 1 * DELTA_TIME;
            }
        }
    }*/


}

//Updates all entities each frames. Handles dead and reproducting cells, and empty trees.
void entity_Updater() {
    int i;
    int deadN = 0;
    int reproductingN = 0;

    int* deadEntities = (int*)malloc(sizeof(int)*entitiesN);
    if (deadEntities == NULL) { 
        printf("Failed to allocate deadEntities! in entity_Updater()\n");
        return;
    }

    int* reproductingEntities = (int*)malloc(sizeof(int) * entitiesN);
    if (reproductingEntities == NULL) {
        printf("Failed to allocate reproductingEntities! in entity_Updater()\n");
        return;
    }

    for (i = 0; i < entitiesN; i++) {
        //Entity walking on terrainCell, applies proprieties.
        entities[i].passingCellIndex = (int)(entities[i].position.x / TERRAIN_CELL_SIZE) + (int)(entities[i].position.y / TERRAIN_CELL_SIZE) * WORLD_WIDTH;
        LandProprieties Bonuses = calculate_Terrain_Bonuses(i);

        //Energy consumption, death
        float consumptionDelta = calculate_Energy_Consumption(i, Bonuses);
        entities[i].energyBalance -= consumptionDelta * DELTA_TIME;
        if (entities[i].energyBalance < MIN_ENERGY_BALANCE) {
            deadEntities[deadN] = i;
            deadN++;
        }

        //Entity zone and position update
        entities[i].zoneIndex = calculate_Updated_Zone(i);
        update_Entity_Position(i, Bonuses);

        //Reproduction conditions
        if (entities[i].energyBalance * Bonuses.reproductionSpeedMult >= MIN_REPRODUCTION_ENERGY) {
            reproductingEntities[reproductingN] = i;
            reproductingN++;
        }
    }

    //Reproduce
    for (int K = 0; K < reproductingN;K++) {
        if (entitiesN < MAX_ENTITIES) {
            Entity newEntity = generate_Entity(1, entities[reproductingEntities[K]].prey, entities[reproductingEntities[K]].genome.speed, entities[reproductingEntities[K]].genome.velocity, entities[reproductingEntities[K]].genome.acceleration, entities[reproductingEntities[K]].genome.sightSize, entities[reproductingEntities[K]].genome.reproductionSpeed, entities[reproductingEntities[K]].genome.agingSpeed, Vector2Add(entities[reproductingEntities[K]].position, (Vector2) { GetRandomValue(-10, 10), GetRandomValue(-10, 10) }));
            entities[entitiesN] = newEntity;
            entitiesN++;
            entities[reproductingEntities[K]].energyBalance -= 20;
        }
    }

    //Delete entities
    for (i = 0; i < deadN - 1; i++) {
        for (int j = i + 1; j < deadN; j++) {
            if (deadEntities[i] < deadEntities[j]) {
                int temp = deadEntities[i];
                deadEntities[i] = deadEntities[j];
                deadEntities[j] = temp;
            }
        }
    }

    //Finally deletes them
    for (i = 0; i < deadN; i++)
    {
        for (int j = deadEntities[i]; j < entitiesN; j++)
        {
            memcpy(&entities[j], &entities[j+1], sizeof entities[j+1]);
        }
        entitiesN--;
    }


    

    free(deadEntities);
    free(reproductingEntities);
}

int main(void)
{
    const int screenWidth = 1820;
    const int screenHeight = 980;
    TerrainGroups = (LandGroup**)malloc(sizeof(LandGroup*) * 5);
    if (TerrainGroups == NULL) return -1;

    //Custom Zones TODO: create file to create zones
    //FOREST
    LandGroup forest;
    Color forestColor;
    forestColor.r = 49;
    forestColor.g = 87;
    forestColor.b = 44;
    forestColor.a = 255;
    forest.landColor = forestColor;
    LandProprieties forestProp = create_Proprieties(1.1, 1.2, 1, 0.7, 1.1, 1, 1.1, 1, 1);
    forest.proprieties = forestProp;
    forest.minValZoneColor = 0;
    forest.maxValZoneColor = 0.30;
    TerrainGroups[0] = &forest;

    //VOLCANIC AREA
    LandGroup volcanic;
    Color volcanicColor;
    volcanicColor.r = 254;
    volcanicColor.g = 104;
    volcanicColor.b = 70;
    volcanicColor.a = 255;
    volcanic.landColor = volcanicColor;
    LandProprieties volcanicProp = create_Proprieties(1, 1, 1.3, 1, 1.2, 1.5, 1.1, 1, 1);
    volcanic.proprieties = volcanicProp;
    volcanic.minValZoneColor = 0.30f;
    volcanic.maxValZoneColor = 0.475f;
    TerrainGroups[1] = &volcanic;


    //RADIOACTIVE
    LandGroup radioactive;
    Color radioactiveColor;
    radioactiveColor.r = 94;
    radioactiveColor.g = 255;
    radioactiveColor.b = 70;
    radioactiveColor.a = 255;
    radioactive.landColor = radioactiveColor;
    LandProprieties radioactiveProp = create_Proprieties(1.05, 1, 1.3, 1, 1.2, 1.5, 1.1, 1, 1);
    radioactive.proprieties = radioactiveProp;
    radioactive.minValZoneColor = 0.475f;
    radioactive.maxValZoneColor = 0.525f;
    TerrainGroups[2] = &radioactive;

    //LOW OXYGEN
    LandGroup lowOxygen;
    Color lowOxygenColor;
    lowOxygenColor.r = 206;
    lowOxygenColor.g = 214;
    lowOxygenColor.b = 223;
    lowOxygenColor.a = 255;
    lowOxygen.landColor = lowOxygenColor;
    LandProprieties lowOxygenProp = create_Proprieties(1.001, 1, 1.5, 1, 1.1, 1, 1, 1, 1.2);
    lowOxygen.proprieties = lowOxygenProp;
    lowOxygen.minValZoneColor = 0.525f;
    lowOxygen.maxValZoneColor = 0.70f;
    TerrainGroups[3] = &lowOxygen;

    //LOW TEMPERATURE
    LandGroup cold;
    Color coldColor;
    coldColor.r = 83;
    coldColor.g = 218;
    coldColor.b = 223;
    coldColor.a = 255;
    cold.landColor = coldColor;
    LandProprieties coldProp = create_Proprieties(1.02, 1, 1.5, 1, 1.1, 1, 1, 1, 1.2);
    cold.proprieties = coldProp;
    cold.minValZoneColor = 0.70f;
    cold.maxValZoneColor = 1.00f;
    TerrainGroups[4] = &cold;
    int zoomMode = 0;


    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    Camera2D camera = { 0 };
    camera.zoom = 1.0f;

    //INSTANTIATE TERRAIN
    RenderTexture2D terrainTexture = instantiate_Terrain();

    //INSTANTIATE ENTITIES
    instantiate_Entities_Start();
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        //Entities update


        //Camera control
        if (IsKeyPressed(KEY_ONE)) zoomMode = 0;
        else if (IsKeyPressed(KEY_TWO)) zoomMode = 1;
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f / camera.zoom);
            camera.target = Vector2Add(camera.target, delta);
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            Vector2 mousePosition = GetScreenToWorld2D(GetMousePosition(), camera);
            int j;
            for (j = 0; j < entitiesN; j++)
            {
                int differenceX = abs(mousePosition.x - entities[j].position.x);
                int differenceY = abs(mousePosition.y - entities[j].position.y);
                if (differenceX < ENTITY_SIZE && differenceY < ENTITY_SIZE) {
                    entities[j].debug = 1;
                }
                else entities[j].debug = 0;

            }

        }
        if (zoomMode == 0)
        {
            // Zoom based on mouse wheel
            float wheel = GetMouseWheelMove();
            if (wheel != 0)
            {
                // Get the world point that is under the mouse
                Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

                // Set the offset to where the mouse is
                camera.offset = GetMousePosition();

                // Set the target to match, so that the camera maps the world space point 
                // under the cursor to the screen space point under the cursor at any zoom
                camera.target = mouseWorldPos;

                // Zoom increment
                // Uses log scaling to provide consistent zoom speed
                float scale = 0.2f * wheel;
                camera.zoom = Clamp(expf(logf(camera.zoom) + scale), 0.125f, 64.0f);
            }
        }


        //Draw calls
        BeginDrawing();
        ClearBackground(LIGHTGRAY);
        
        BeginMode2D(camera);

        DrawTextureRec(terrainTexture.texture, (Rectangle) { 0, 0, terrainTexture.texture.width, -terrainTexture.texture.height }, (Vector2) { 0, 0 }, WHITE);

        draw_Trees();
        draw_Entities();
        entity_Updater();
        EndMode2D();
        
        DrawText(TextFormat("FPS: %i", GetFPS()), 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("ENTITIES: %i", entitiesN), 10, 30, 20, DARKGRAY);
        
        EndDrawing();
    }


    free(TerrainCells);
    free(trees);
    free(TerrainGroups);
    CloseWindow();
    return 0;
}