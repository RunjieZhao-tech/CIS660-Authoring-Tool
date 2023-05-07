#pragma once
#include <vector>
#include <la.h>

void readFile(const char* filename, /*output*/std::vector<glm::vec3>* pos,/*output*/std::vector<std::vector<int>>* face);
void readV(const char* start,/*output*/std::vector<float>* res);
void readF( const char* start, /*output*/std::vector<int>* res);
