#include"SaveAndLoad.h"
//open file
FILE *openfile(type t,const char file[])
{
	char tp[4];//type of opening file
	if (t == type::fsave)
		strcpy(tp, "a+b");
	else
		strcpy(tp, "r+b");
	FILE *infile;
	infile = fopen(file, tp);
	if (infile)
		return infile;
	else
		return NULL;
}
//will search for specific variables
int search_token(const char token[],FILE *infile)
{
	
	Load_date dat;
	fread(&dat, sizeof(Load_date), 1, infile);
	if (!feof(infile))
	{
		do
		{
			//check is the same token
			if (!strcmp(dat.token, token))
			{
				fseek(infile, -1l * sizeof(Load_date), SEEK_CUR);
				int x = ftell(infile);
				fseek(infile, 0, SEEK_SET);
				return x;

			}
			fread(&dat, sizeof(Load_date), 1, infile);
			printf("%d", feof(infile));
		} while (!feof(infile));
	}
	fseek(infile, 0, SEEK_SET);//turn back seek to the begining
	return -1;
}
//save params
bool save_param(Load_date &ld)
{
	FILE *parfile = NULL;
	parfile = fopen("paramFile", "a+b");
	//check that variable is on file
	if (ld.st_point == -1)
	{
		 fseek(parfile, 0l, SEEK_END);
		 ld.st_point = ftell(parfile);
	}
	
	fseek(parfile, ld.st_point, SEEK_SET);
	switch (ld.d_type)
	{
	case data_type::i_type:
	//	fwrite(((int*)ld.data), sizeof(int), ld.length, parfile);
		printf("size: %d\n", sizeof(ld.data));
		fwrite(ld.data, sizeof(int), ld.length, parfile);
		 fseek(parfile, 0, SEEK_CUR);
		 ld.end_point = ftell(parfile);
		printf("save %d", *((int*)ld.data));
		fclose(parfile);
		return true;
	case data_type::f_type:
		fwrite(ld.data, sizeof(float), ld.length, parfile);
		fseek(parfile, 0, SEEK_CUR);
		ld.end_point = ftell(parfile);
		fclose(parfile);
		return true;
	case data_type::c_type:
		fwrite(ld.data, sizeof(char), ld.length, parfile);
		fseek(parfile, 0, SEEK_CUR);
		ld.end_point = ftell(parfile);
		fclose(parfile);
		return true;
	default:
		break;
	}
	
	fclose(parfile);
	return false;


}
//load param of variables on file
bool load_param(Load_date &ld)
{
	FILE *parfile = NULL;
	parfile = openfile(type::fload, "paramFile");
	if (!parfile)
		return false;
	//go to param of variable
	fseek(parfile, ld.st_point, SEEK_SET);
	
	switch (ld.d_type)
	{
	case data_type::c_type:
		ld.data = new char[ld.length];
		fread(ld.data, sizeof(char), ld.length, parfile);
		fclose(parfile);
		return true;
	case data_type::i_type:
		//del
		ld.data = new int[ld.length];
		fread(ld.data, sizeof(int), ld.length, parfile);
		fclose(parfile);
		printf("save %d", *((int*)ld.data));
		return true;
	case data_type::f_type:
		ld.data = new float[ld.length];
		fread(ld.data, sizeof(float), ld.length, parfile);
		fclose(parfile);
		return true;
	default:
		break;
	}
	fclose(parfile);
	return false;
}
//load variable
bool load(const char token[], void *data)
{
	Load_date ld;
	FILE *infile = fopen("test", "r+b");
	if (!infile)
		return false;
	else
	{
		int x = search_token(token, infile);
		if (x == -1)//found variable
		{
			return false;
		}
		fseek(infile, x, SEEK_SET);
		fread(&ld, sizeof(Load_date), 1, infile);
		load_param(ld);//load param of variable
		switch (ld.d_type)
		{
		case data_type::i_type:
			for (int i = 0; i < ld.length; i++)
			{
				*((int*)data + i) = *((int*)ld.data + i);
			}
			//data = ld.data;
			break;
		case data_type::f_type:
			*(float*)data = *((float*)ld.data);//load data
			break;
		case data_type::c_type:
			*(char*)data = *((char*)ld.data);//load data
			break;
		default:
			break;
		}
		
		//printf("%d", *((int*)ld.data));//experimental
		return true;
	}


}
//save variable
bool save(const char token[],int length,void *data,data_type di)
{
	
	FILE *infile = openfile(type::fsave,"test");
	if (!infile)
		return false;
	fseek(infile, 0, SEEK_SET);
	Load_date dat;
	//printf("%d",EOF);
	int x = search_token(token, infile);//found variable
	if (x!=-1)//if found variable
	{
		fseek(infile, x, SEEK_SET);
		fread(&dat, sizeof(Load_date), 1, infile);
		dat.length = length;
		dat.data = data;
		save_param(dat);
	}
	else
	{
		fseek(infile, 0, SEEK_END);
		dat.d_type = di;
		strcpy(dat.token, token);
		dat.length = length;
		dat.data = data;
		save_param(dat);
	}
	fflush(infile);
	fwrite(&dat, sizeof(Load_date), 1, infile);
	
	fclose(infile);
	return true;
}
