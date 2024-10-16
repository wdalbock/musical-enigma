#ifndef LEADERBOARD_H
#define LEADERBOARD_H

void readFile(const char* filename, int* numbers);
void clearFile(const char* path);
void writeScores(const char* filename, int* scoreArray);
bool fileExists(const char* filepath);
void addSnakeScore(int score, int* leaderboard);
void addConnectFourScore(int score, int* leaderboard);

#endif