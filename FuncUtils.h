#pragma once
#include "raylib.h"

//Creates an unique color, based on value and max value.
Color get_Unique_Color(float t, float n);

//Calculates [SQUARED!] distance between two points.
float calc_Distance(Vector2 source, Vector2 destination);

//Checks if element is in array.
int is_In_Array(int* arr, int lenght, int find);