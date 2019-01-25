#pragma once
struct group
{
	int token=-1;//-1 mean its an empty group
	bool isBlocked = false;
};
enum blocktype {
	ALL,ONE
};
/*
//will create a new group
void addGroup(int i, int j, int color, char just );
//merge two exiting group
void addGroup(int token, int tokento);

void addGroup(int i, int j, int token);
*/
int gettoken(int, int);
void settoken(int i, int j, int token);
bool getGroupBlock(int i, int j);
int getColorOfGroup(int i, int j);
void setGroupBlock(int i, int j, blocktype bl, bool isbolck);
bool checkGrouping(int i, int j, int color, int array[][10]);
void deleteGroup(int i, int j, int array[][10]);
int static usednumber = 0;
//