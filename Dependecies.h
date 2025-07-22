#pragma once
#include <math.h>
#include <time.h>
#include <string.h>
#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include "Perlin.h"

//Forward initializations
typedef struct s_LandGroup LandGroup;
typedef struct s_LandCell LandCell;

//_______GLOBAL CONSTANTS_______
//Terrain constants
#define TERRAIN_CELL_SIZE 25
#define WORLD_WIDTH 675
#define WORLD_HEIGHT 675

//Start array size constants
#define MAX_ENTITIES 10000
#define STARTING_ENTITIES 1000
#define MAX_NEIGHBOURS_SIZE 100
#define MAX_NEIGHBOURS_PLANT_SIZE 100
#define PLANT_OVER_PREDATOR_MULT 1;

//Starting entities values
#define STARTING_REPRODUCTION_ADVANCMENT 0
#define STARTING_AGE_ADVANCMENT 0
#define ENTITY_SIZE 20
#define ENTITY_MAX_SPEED 10
#define ENTITY_MAX_ACCELERATION 25
#define PREDATORS_SPEED_MULT 1.3
#define ENTITY_SPEED_DECAY 0.99f

//Entities consumption constatnts
#define TREE_ASSIMILATION_DISTANCE (TERRAIN_CELL_SIZE*2)
#define PREY_ASSIMILATION_DISTANCE (TERRAIN_CELL_SIZE*2)
#define ENTITY_SIGHT_MULTIPLYER (TERRAIN_CELL_SIZE*50)
#define ENTITY_PREDATOR_DANGER_ZONE (TERRAIN_CELL_SIZE*10)
#define ENTITIES_CONSUMPTION_MULTIPLIER 0.5
#define PREDATORS_SIGHT_MULTIPLIER 0.75

//Entity energy constants
#define STARTING_ENERGY_BALANCE 10
#define MAX_ENERGY_BALANCE 30
#define MIN_ENERGY_BALANCE -30
#define MIN_REPRODUCTION_ENERGY 28

//Trees constants
#define TREE_SIZE 30
#define TREE_ENERGY 20

//Genome constants
#define GENOME_MUTATION_CHANCE 0.0001f
#define GENOME_TRAITS 6

//Time constants
#define DELTA_TIME 0.05


//_______STRUCTS_______
//Terrain defining structs
typedef struct s_tree {
	int DEBUG;
	int zoneIndex;
	float treeEnergy;
	float energyValue;
	float reproductionTimer;
	Vector2 worldPosition;
	LandCell* assignedCell;
} Tree;

typedef struct s_LandProprieties {
	float plantDevelopMult; //Multiplies the random value to instantiate trees in instantiate_Terrain
	float speedMultPred;
	float speedMultPrey;
	float energyConsMultPred;
	float energyConsMultPrey;
	float reproductionSpeedMult;
	float energyPerPlantMult; //Is the actual value of energy per plant
	float geneticChangeProbMult;
	float agingMult;
} LandProprieties;

typedef struct s_LandCell {
	Vector2 position;
	LandGroup* group;
	Tree* cellTree;
	int index;
} LandCell;

typedef struct s_LandGroup {
	LandCell* landTiles;
	LandProprieties proprieties;
	Color landColor;
	float minValZoneColor;
	float maxValZoneColor;
} LandGroup;

//Entity defining structs
typedef struct s_genome {
	float speed;
	float velocity;
	float acceleration;
	float sightSize;
	float reproductionSpeed;
	float agingSpeed; 
		//SE NE AGGIUNGI, CAMBIA GENOME_TRAITS!!! cambia anche in entityUpdater
} Genome;

typedef struct s_entity {
	int debug;
	int zoneIndex;
	int passingCellIndex;
	int prey;
	
	float consumedEnergyThisTurn;
	float energyBalance;
	float reproductionAdvancment;
	float ageAdvancment;
	
	Vector2 position;
	Vector2 velocity;
	Genome genome;
	Color color;
} Entity;


//_______GLOBAL VARIABLES_______
//Global arrays
LandGroup** TerrainGroups;
LandCell* TerrainCells;
Tree* trees;
Entity* entities;
Entity** entitiesZones;

//Global arrays infos
int terrainCellsN = 0;
int treesN = 0;
int entitiesN;