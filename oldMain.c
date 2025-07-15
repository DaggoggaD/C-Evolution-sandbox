/*#define _CRT_SECURE_NO_WARNINGS
#include "Dependecies.h"
#include "main.h"

static int SEED = 1000;

//PERLIN NOISE IMPLEMENTATION (TAKEN FROM GITHUB assolutamente 0 voglia. Se trovo il link lo metto)
static int hash[] = { 208,34,231,213,32,248,233,56,161,78,24,140,71,48,140,254,245,255,247,247,40,
                     185,248,251,245,28,124,204,204,76,36,1,107,28,234,163,202,224,245,128,167,204,
                     9,92,217,54,239,174,173,102,193,189,190,121,100,108,167,44,43,77,180,204,8,81,
                     70,223,11,38,24,254,210,210,177,32,81,195,243,125,8,169,112,32,97,53,195,13,
                     203,9,47,104,125,117,114,124,165,203,181,235,193,206,70,180,174,0,167,181,41,
                     164,30,116,127,198,245,146,87,224,149,206,57,4,192,210,65,210,129,240,178,105,
                     228,108,245,148,140,40,35,195,38,58,65,207,215,253,65,85,208,76,62,3,237,55,89,
                     232,50,217,64,244,157,199,121,252,90,17,212,203,149,152,140,187,234,177,73,174,
                     193,100,192,143,97,53,145,135,19,103,13,90,135,151,199,91,239,247,33,39,145,
                     101,120,99,3,186,86,99,41,237,203,111,79,220,135,158,42,30,154,120,67,87,167,
                     135,176,183,191,253,115,184,21,233,58,129,233,142,39,128,211,118,137,139,255,
                     114,20,218,113,154,27,127,246,250,1,8,198,250,209,92,222,173,21,88,102,219 };

int noise2(int x, int y)
{
    int tmp = hash[(y + SEED) % 256];
    return hash[(tmp + x) % 256];
}

float lin_inter(float x, float y, float s)
{
    return x + s * (y - x);
}

float smooth_inter(float x, float y, float s)
{
    return lin_inter(x, y, s * s * (3 - 2 * s));
}

float noise2d(float x, float y)
{
    int x_int = x;
    int y_int = y;
    float x_frac = x - x_int;
    float y_frac = y - y_int;
    int s = noise2(x_int, y_int);
    int t = noise2(x_int + 1, y_int);
    int u = noise2(x_int, y_int + 1);
    int v = noise2(x_int + 1, y_int + 1);
    float low = smooth_inter(s, t, x_frac);
    float high = smooth_inter(u, v, x_frac);
    return smooth_inter(low, high, y_frac);
}

float perlin2d(float x, float y, float freq, int depth)
{
    float xa = x * freq;
    float ya = y * freq;
    float amp = 1.0;
    float fin = 0;
    float div = 0.0;

    int i;
    for (i = 0; i < depth; i++)
    {
        div += 256 * amp;
        fin += noise2d(xa, ya) * amp;
        amp /= 2;
        xa *= 2;
        ya *= 2;
    }

    return fin / div;
}
//END PERLIN NOISE IMPLEMENTATION

//Creates the whole terrain texture, speeding up Draw calls.
//Initializes TerrainCells and trees array. Assigns proprieties to each tile.
RenderTexture2D instantiate_Terrain() {
    int x = 0;
    int y = 0;
    TerrainCells = (LandCell*)malloc(sizeof(LandCell) * WORLD_WIDTH * WORLD_HEIGHT);
    trees = (LandCell*)malloc(sizeof(LandCell) * WORLD_WIDTH * WORLD_HEIGHT);
    if (TerrainCells == NULL) return;
    RenderTexture2D terrainTex = LoadRenderTexture(WORLD_WIDTH * TERRAIN_CELL_SIZE, WORLD_HEIGHT * TERRAIN_CELL_SIZE);
    BeginTextureMode(terrainTex);
    ClearBackground((Color) { 255, 255, 255, 255 });  // usa un bianco opaco, non trasparente
    srand(time(NULL));

    while (x < WORLD_WIDTH) {
        while (y < WORLD_HEIGHT) {
            Color color;
            SEED = GetTime();
            float jitterX = (float)(rand() % 100 - 50) / 20.0f;
            float jitterY = (float)(rand() % 100 - 50) / 20.0f;
            float perl = (perlin2d(x + jitterX, y + jitterY, 0.003, 3) + perlin2d(x + 1000, y + 1000, 0.006, 1)) / 2;

            Vector2 pos;
            pos.x = x;
            pos.y = y;
            LandCell currCell;
            currCell.position = pos;


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

            TerrainCells[terrainCellsN] = currCell;
            terrainCellsN++;

            float randomTreeChance = ((float)rand() / RAND_MAX) * (currCell.group->proprieties.plantDevelopMult);
            float inner_rand = (float)rand() / RAND_MAX;
            if (randomTreeChance > 0.999 && inner_rand > 0.7)
            {
                Tree currtree;
                currtree.assignedCell = &TerrainCells[terrainCellsN];
                currtree.energyValue = currCell.group->proprieties.energyPerPlantMult;
                trees[treesN] = currtree;

                int xZone = (int)floor(currCell.position.x / (2 * ENTITY_SIGHT_MULTIPLYER));
                int yZone = (int)floor(currCell.position.y / (2 * ENTITY_SIGHT_MULTIPLYER));
                int index = xZone + yZone * (int)((WORLD_WIDTH * TERRAIN_CELL_SIZE) / (2 * ENTITY_SIGHT_MULTIPLYER)); //TODO: FIRST OF EVERY ROW HAS SAME AS LAST OF PREVIOUS
                currtree.zoneIndex = index;


                treesN++;
            }

            DrawRectangle(x * TERRAIN_CELL_SIZE, y * TERRAIN_CELL_SIZE, TERRAIN_CELL_SIZE, TERRAIN_CELL_SIZE, currCell.group->landColor);
            y++;

        }
        x++;
        y = 0;
    }
    EndTextureMode();

    Tree* resizedTreeS = realloc(trees, treesN * sizeof(Tree));
    if (resizedTreeS == NULL) {
        perror("realloc failed");
        free(trees);
    }
    trees = resizedTreeS;


    return terrainTex;
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

//Creates an unique color, based on value and max value. (TAKEN FROM CHATGPT no sbatti)
Color get_Unique_Color(float t, float n) {
    if (n <= 0.0f) return BLACK;
    if (t < 0.0f) t = 0.0f;
    if (t > n) t = n;

    float hue = (t / n) * 360.0f;
    float saturation = 0.8f;
    float value = 0.9f;

    return ColorFromHSV(hue, saturation, value);
}

//Generates a single entity (both cell starting stats and genome). 
//For debug purposes, "custom" argument is passed. 0, random instantiation. 1, custom instantiation
Entity generate_Entity(int custom, int prey, float speed, float turnSpeed, float acceleration, float sightSize, float reproductionSpeed, float agingSpeed) {
    Vector2 position;
    position.x = GetRandomValue(0, WORLD_WIDTH * TERRAIN_CELL_SIZE);
    position.y = GetRandomValue(0, WORLD_WIDTH * TERRAIN_CELL_SIZE);

    Entity currEntity;
    currEntity.prey = GetRandomValue(0, 1);
    currEntity.consumedEnergyThisTurn = 0;
    currEntity.energyBalance = STARTING_ENERGY_BALANCE;
    currEntity.reproductionAdvancment = STARTING_REPRODUCTION_ADVANCMENT;
    currEntity.ageAdvancment = STARTING_AGE_ADVANCMENT;
    currEntity.color = BLACK;
    currEntity.position = position;
    Genome genome;

    if (custom == 0) {
        genome.speed = (float)rand() / RAND_MAX;
        genome.turnSpeed = (float)rand() / RAND_MAX;
        genome.acceleration = (float)rand() / RAND_MAX;
        genome.sightSize = (float)rand() / RAND_MAX;
        genome.reproductionSpeed = (float)rand() / RAND_MAX;
        genome.agingSpeed = (float)rand() / RAND_MAX;
    }
    else {
        genome.speed = speed;
        genome.turnSpeed = turnSpeed;
        genome.acceleration = acceleration;
        genome.sightSize = sightSize;
        genome.reproductionSpeed = reproductionSpeed;
        genome.agingSpeed = agingSpeed;
    }
    currEntity.genome = genome;

    float geneSum = genome.speed + genome.turnSpeed + genome.acceleration + genome.sightSize + genome.reproductionSpeed + genome.agingSpeed;
    currEntity.color = get_Unique_Color(geneSum, GENOME_TRAITS);

    int xZone = (int)floor(currEntity.position.x / (2 * ENTITY_SIGHT_MULTIPLYER));
    int yZone = (int)floor(currEntity.position.y / (2 * ENTITY_SIGHT_MULTIPLYER));
    int index = xZone + yZone * (int)((WORLD_WIDTH * TERRAIN_CELL_SIZE) / (2 * ENTITY_SIGHT_MULTIPLYER)); //TODO: FIRST OF EVERY ROW HAS SAME AS LAST OF PREVIOUS
    currEntity.zoneIndex = index;

    return currEntity;
}

//Instantiates initial entities, and initializes entities array
void instantiate_Entities_Start() {
    int i = 0;
    entities = (Entity*)malloc(sizeof(Entity) * STARTING_ENTITIES);
    entitiesZones = (Entity**)malloc(sizeof(Entity*) * (int)((WORLD_WIDTH * TERRAIN_CELL_SIZE) / (2 * ENTITY_SIGHT_MULTIPLYER)) * (int)((WORLD_WIDTH * TERRAIN_CELL_SIZE) / (2 * ENTITY_SIGHT_MULTIPLYER)));
    if (entities == NULL) return;
    while (i < STARTING_ENTITIES) {
        Entity curr = generate_Entity(0, 0, 0, 0, 0, 0, 0, 0);
        entities[i] = curr;
        i++;
    }
    entitiesN = STARTING_ENTITIES;
}

//Draws all entities.
void draw_Entities() {
    int i = 0;
    while (i < entitiesN) {
        DrawCircleV(entities[i].position, ENTITY_SIZE, entities[i].color);

        //DEBUG
        /*
        char index[20];
        _itoa(entities[i].zoneIndex, index, 10);
        char str[100];
        int *close = (int*)malloc(sizeof(int)*9);
        if (close == NULL) return;
        int n = find_Close_Zones(entities[i].zoneIndex, close);
        sprintf(str, "%d,%d,%d,\n%d,%d,%d,\n,%d,%d,%d", close[0], close[1], close[2], close[3], close[4], close[5], close[6], close[7], close[8]);

        DrawText(str, entities[i].position.x, entities[i].position.y, 10, BLACK);

        if (i==0)
        {
            DrawCircleLinesV(entities[i].position, ENTITY_SIGHT_MULTIPLYER, YELLOW);
        }

        i++;
    }
}

//Draws trees.
void draw_Trees() {
    int i = 0;
    while (i < treesN) {
        DrawCircle(trees[i].assignedCell->position.x * TERRAIN_CELL_SIZE, trees[i].assignedCell->position.y * TERRAIN_CELL_SIZE, TREE_SIZE, DARKGREEN);
        i++;
    }
}

int find_Close_Zones(int zone, int* out) {

    int count = 0;
    int width = (int)((WORLD_WIDTH * TERRAIN_CELL_SIZE) / (2 * ENTITY_SIGHT_MULTIPLYER));
    int height = (int)((WORLD_WIDTH * TERRAIN_CELL_SIZE) / (2 * ENTITY_SIGHT_MULTIPLYER));

    // Converti zona 1D in coordinate 2D (x, y)
    int x = (zone - 1) % width;
    int y = (zone - 1) / width;

    for (int dy = -1; dy <= 1; dy++) {
        int ny = y + dy;
        if (ny < 0 || ny >= height) continue;

        for (int dx = -1; dx <= 1; dx++) {
            int nx = x + dx;
            if (nx < 0 || nx >= width) continue;

            int index = ny * width + nx + 1;  // +1 perché zona parte da 1
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

float calc_Distance(Vector2 source, Vector2 destination) {
    float distancex = source.x - destination.x;
    float distancey = source.y - destination.y;

    return distancex * distancex + distancey * distancey;
}

int is_In_Array(int* arr, int lenght, int find) {
    for (int i = 0; i < lenght; i++)
    {
        if (arr[i] == find) return 1;
    }
    return 0;
}

int* find_Nearby_Plants(int index, int* closePlantN, int* closestPlant) {
    int i = 0;
    int neighboursIndex = 0;
    int closest = NULL;
    int* neighbours = (int*)malloc(sizeof(int) * MAX_NEIGHBOURS_PLANT_SIZE);
    if (neighbours == NULL) return NULL;
    int* close = (int*)malloc(sizeof(int) * 9);
    if (close == NULL) return NULL;
    /*
    while (i < treesN && neighboursIndex < MAX_NEIGHBOURS_PLANT_SIZE) {

        int n = find_Close_Zones(entities[index].zoneIndex, close);
        int isNear = is_In_Array(close, n, trees[i].zoneIndex);

        if (!isNear) {
            i++;
            continue;
        }
        int distance = calc_Distance(entities[index].position, trees[i].assignedCell->position);
        if (distance  < (ENTITY_SIGHT_MULTIPLYER * entities[index].genome.sightSize) * (ENTITY_SIGHT_MULTIPLYER * entities[index].genome.sightSize)) {
            neighbours[neighboursIndex] = i;
            neighboursIndex++;
            if (distance < closest || closest == NULL) closest = i;
        }

        i++;
    }
    *closePlantN = neighboursIndex;
    *closestPlant = closest;
    return neighbours;
}

void find_Nearby_Entities(int index, int* closeEntities, int* predators, int* preys, int* closeN, int* closePrays, int* closePredators, int* closestPrey) {
    int i = 0;
    int neighboursIndex = 0;
    int preyIndex = 0;
    int predatorIndex = 0;
    int closest = NULL;
    int closestDistance = NULL;
    int count = 0;
    int distance = INFINITY;
    int* close = (int*)malloc(sizeof(int) * 9);
    if (close == NULL) return NULL;

    int n = find_Close_Zones(entities[index].zoneIndex, close);

    while (i < entitiesN && neighboursIndex < MAX_NEIGHBOURS_SIZE) {
        if (is_In_Array(close, n, entities[i].zoneIndex) == 1) {
            count++;
            distance = calc_Distance(entities[index].position, entities[i].position);

            if (distance < (ENTITY_SIGHT_MULTIPLYER * entities[index].genome.sightSize) * (ENTITY_SIGHT_MULTIPLYER * entities[index].genome.sightSize) && i != index) {
                closeEntities[neighboursIndex] = i;
                neighboursIndex++;

                if (entities[i].prey == 0) {
                    predators[predatorIndex] = i;
                    predatorIndex++;
                    if (closest == NULL || distance < closestDistance) {
                        closest = i;
                        closestDistance = distance;
                    }
                }
                else {
                    preys[preyIndex] = i;
                    preyIndex++;
                }
            }
        }
        i++;
    }

    *closeN = neighboursIndex;
    *closePrays = preyIndex;
    *closePredators = predatorIndex;
    *closestPrey = closest;
}

void entity_Updater() {
    int i = 0;
    int* closePredatorsIndexes = (int*)malloc(sizeof(int) * MAX_NEIGHBOURS_SIZE);
    int* closePreysIndexes = (int*)malloc(sizeof(int) * MAX_NEIGHBOURS_SIZE);
    int* closeEntitiesIndexes = (int*)malloc(sizeof(int) * MAX_NEIGHBOURS_SIZE);
    if (closeEntitiesIndexes == NULL) return;

    int closeEntitiesN = 0;
    int closePredatorsN = 0;
    int closePreysN = 0;
    int closePlantsN = 0;
    int closestPreyIndex = NULL;
    int closestPlantIndex = NULL;

    while (i < entitiesN) {

        //zone update
        int xZone = (int)floor(entities[i].position.x / (2 * ENTITY_SIGHT_MULTIPLYER));
        int yZone = (int)floor(entities[i].position.y / (2 * ENTITY_SIGHT_MULTIPLYER));
        int index = xZone + yZone * (int)((WORLD_WIDTH * TERRAIN_CELL_SIZE) / (2 * ENTITY_SIGHT_MULTIPLYER)); //TODO: FIRST OF EVERY ROW HAS SAME AS LAST OF PREVIOUS
        entities[i].zoneIndex = index;


        //close entities - plants
        find_Nearby_Entities(i, closeEntitiesIndexes, closePredatorsIndexes, closePreysIndexes, &closeEntitiesN, &closePreysN, &closePredatorsN, &closestPreyIndex);
        //int* closePlantIndexes = find_Nearby_Plants(i, &closePlantsN, &closestPlantIndex);


        //movement


        //energy


        //age increase - death


        //reproduction - birth



        //end update

        i++;
    }
    if (closeEntitiesIndexes) free(closeEntitiesIndexes);
    if (closePreysIndexes) free(closePreysIndexes);
    if (closePredatorsIndexes) free(closePredatorsIndexes);
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
        entity_Updater();

        //Trees update


        //Camera control
        if (IsKeyPressed(KEY_ONE)) zoomMode = 0;
        else if (IsKeyPressed(KEY_TWO)) zoomMode = 1;
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f / camera.zoom);
            camera.target = Vector2Add(camera.target, delta);
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

        EndDrawing();
    }


    free(TerrainCells);
    free(trees);
    free(TerrainGroups);
    CloseWindow();
    return 0;
}
*/