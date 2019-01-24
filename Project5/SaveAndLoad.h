#pragma once
#include<stdio.h>
#include<string.h>
//type of work to do with file
enum type
{
	fsave, fload
};
//type of variables to save
enum data_type
{
	i_type, f_type, c_type

};

//save and load data
struct Load_date
{
	data_type d_type;
	char token[50] = { 0 };//specific token of variable
	int length = 0;//length of variable
	void *data;//dato to save or load
	int st_point = -1;//-1 mean that its not saved in file
	int end_point;
	//data = new data_type[length];
};


//load data from file 
bool load(const char token[], void *data);
//save to file
bool save(const char token[] , int length, void *data,data_type di);

