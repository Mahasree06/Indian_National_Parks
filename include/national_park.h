#ifndef NATIONAL_PARK_H
#define NATIONAL_PARK_H

#define NAME_SIZE 256
#define STATE_SIZE 100
#define LOCATION_SIZE 256
#define YEAR_SIZE 20
#define FEATURES_SIZE 500
#define FLORA_SIZE 500
#define RIVERS_SIZE 500

typedef struct
{
    char name[NAME_SIZE];
    char state_ut[STATE_SIZE];
    char location[LOCATION_SIZE];
    char formed_year[YEAR_SIZE];
    char notable_features[FEATURES_SIZE];
    char flora_fauna[FLORA_SIZE];
    char rivers_lakes[RIVERS_SIZE];

} NationalPark;

#endif
