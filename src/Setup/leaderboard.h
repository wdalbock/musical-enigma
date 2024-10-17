#ifndef LEADERBOARD_H
#define LEADERBOARD_H

void readFile(const char* filename, int* numbers);
void clearFile(const char* path);
void writeScores(const char* filename, int* scoreArray);
bool fileExists(const char* filepath);
void addDescendingScore(int score, int* leaderboard);
void addAscendingScore(int score, int* leaderboard);

#endif