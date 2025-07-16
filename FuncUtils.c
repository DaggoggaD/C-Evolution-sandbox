#include "FuncUtils.h"

//Creates an unique color, based on value and max value.
Color get_Unique_Color(float t, float n) {
    if (n <= 0.0f) return BLACK;
    if (t < 0.0f) t = 0.0f;
    if (t > n) t = n;

    float hue = (t / n) * 360.0f;
    float saturation = 0.8f;
    float value = 0.9f;

    return ColorFromHSV(hue, saturation, value);
}

//Calculates [SQUARED!] distance between two points.
float calc_Distance(Vector2 source, Vector2 destination) {
    float distancex = source.x - destination.x;
    float distancey = source.y - destination.y;

    return distancex * distancex + distancey * distancey;
}

//Checks if element is in array.
int is_In_Array(int* arr, int lenght, int find) {
    for (int i = 0; i < lenght; i++)
    {
        if (arr[i] == find) return 1;
    }
    return 0;
}