#include"Group.h"
#include<stdio.h>



group _group[10][10];
//add an elem to a group
void addGroup(int i,int j,int token)
{
	//_group[i][j].i = i;
	//_group[i][j].j = j;
	_group[i][j].token = token;
	
}
//merge two exiting group
void addGroup(int token,int tokento)
{
	int temp1,temp2;
	if (token > tokento)
	{
		temp1 = tokento;
		temp2 = token;
	}
	else
	{
		temp2 = tokento;
		temp1 = token;
	}
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if (_group[i][j].token == temp2)
			{
				
				_group[i][j].token = temp1;
			}
		}
	}
}
//will create a new group (just is for preventing from conflicts with other overload
void addGroup(int i, int j, int color,char just)
{	
	//_group[i][j].i = i;
	//_group[i][j].j = j;
	_group[i][j].token = usednumber * 10 + color + 1;
	usednumber++;
	printf("number is:%d\n", usednumber);
}
int getColorOfGroup(int i, int j)
{
	return _group[i][j].token % 10 - 1;
}
bool checkGrouping(int i, int j, int color,int array[][10])
{
	int tempi[4] = { 0 }, tempj[4] = { 0 };
	bool sw = false;
	bool switches[4] = { 0 };
	if (j + 1 < 10)
	{
		if (array[i][j + 1] == color)
		{
			tempi[0] = i;
			tempj[0] = j + 1;
			switches[0] = true;
			sw = true;
		}
	}
	if (j - 1 >= 0)
	{
		if (array[i][j - 1] == color)
		{
			tempi[1] = i;
			tempj[1] = j - 1;
			switches[1] = true;
			sw = true;
		}
	}
	if (i + 1 < 10)
	{
		if (array[i + 1][j] == color)
		{
			tempi[2] = i + 1;
			tempj[2] = j;
			switches[2] = true;
			sw = true;
		}
	}
	if (i - 1)
	{
		if (array[i - 1][j] == color)
		{
			tempi[3] = i - 1;
			tempj[3] = j;
			switches[3] = true;
			sw = true;
		}
	}
	if (!sw)
	{
		addGroup(i, j, color,'1');
		return true;
	}
	else
	{
		bool m = false;
		for (int k = 0; k < 4; k++)
		{
			if (!switches[k])
				continue;
			for (int l = k+1; l < 4 ; l++)
			{
				if (!switches[l])
					continue;
				addGroup(i, j, _group[tempi[k]][tempj[k]].token);
				addGroup(_group[tempi[k]][tempj[k]].token, _group[tempi[l]][tempj[l]].token);
				m = true;
			}
		}
		if (m)
			return true;
		else
		{
			for (int k = 0; k < 4 ; k++)
			{
				if (!switches[k])
					continue;
				addGroup(i, j, _group[tempi[k]][tempj[k]].token);
				return true;
			}
		}
	}
	return false;
}
void deleteGroup(int i,int j,int array[][10])
{
	int temp = _group[i][j].token;
	if (temp != -1) {
		for (int k = 0; k < 10; k++)
		{
			for (int l = 0; l < 10; l++)
			{
				if (_group[k][l].token == temp)
				{
					_group[k][l].token = -1;
					_group[k][l].isBlocked = false;
					//al_destroy_bitmap();
					array[k][l] = 0;
					printf("%d %d has been deleted.\n",k,l);
				}
			}
		}
	}
}
int gettoken(int i, int j)
{
	return _group[i][j].token;
}
void settoken(int i, int j,int token)
{
	_group[i][j].token = token;
	if (token / 10 > usednumber)
		usednumber = token / 10;
}
void deleteGroup(int token)
{

}
void printgroup()
{

}
bool getGroupBlock(int i, int j) {
	return _group[i][j].isBlocked;
}
void setGroupBlock(int i,int j,blocktype bl,bool isbolck)
{
	if (bl == blocktype::ONE)
	{
		_group[i][j].isBlocked == isbolck;
	}
	else
	{
		for (int k = 0; k < 10; k++)
		{
			for (int l = 0; l < 10; l++)
			{
				if (_group[k][l].token == _group[i][j].token)
				{
					_group[k][l].isBlocked = isbolck;
				}
			}
		}
	}
}