#ifndef STRINGING_H_
#define STRINGING_H_
/*returns size of the output*/
int strsplt(char* string, char token, char** output);
void strjoin(char** input, int inputSize, char* output);
#endif 