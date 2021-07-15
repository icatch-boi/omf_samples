#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int GetStrTxTIndx(char* m_str,const char* Str)
{
	if (m_str == NULL || Str == NULL){
		return 0;
	}
	int StrSize = strlen(Str);
	if (StrSize <= 0){
		return 0;
	}
	int Str_m_str = strlen(m_str);
	int m_str_index = 0;
	int y = 0;
	for (int i = 0; i <= Str_m_str - 1; i++){
		y = 0;
		m_str_index = i;
		for (int j = 0; j <= StrSize - 1; ++j, ++m_str_index){
			if (Str[j] == m_str[m_str_index]){
				y += 1;
			}
			if (y == StrSize){
				return i + 1;
			}
			if (Str[j] != m_str[m_str_index]){
				break;
 
			}
		}
	}
	return 0;
}

char* GetLineTxT(char* m_str,int LineIndex)
{
	if (LineIndex == 0){
		return 0;
	}
	if (m_str == NULL) {
		return 0;
	}
	int l = 0;
	char *yun = NULL;
	int j = 0;
	int u = 0;
	for (int i = 0; i <= strlen(m_str) + 1; ++i) {
		if (m_str[i] == '\n') {
			++l;
			if (l != LineIndex)
				u = i + 1;
		}
		if (l == LineIndex) {
			for (j = u;; ++j) {
				if (m_str[j] == '\n' || m_str[j] == '\0') {
					int size = 0;
					if (u == 0){
						size = j + 1;	
					}
					else{
						size = j;
					}
					yun = (char*)malloc(size + 1);
					memset(yun, 0, size + 1);
					break;
				}
			}
			int in = 0;
			for (int n = u; n <= j; ++n, ++in) {
				yun[in] = m_str[n];
			}
			return yun;
		}
	}
}

int GetLineNum(char* m_str){
	if (m_str == NULL){
		return 0;
	}
	int Size = 0;
	for (int i = 0; i <= strlen(m_str)-1; ++i){
		if (m_str[i] == '\n'){
			++Size;
		}
		if (m_str[i] == '\0'){
			if (m_str[i - 1] != '\n'){
				Size++;
			}
			return Size;
		}
	}
	return Size;
}

unsigned int JumpTxTExits(char* m_str,char *Str){
	if (GetStrTxTIndx(m_str,Str) == 0){
		return 0;
	}
	return 1;
}

int GetTxTLineNum(char* m_str,char *Str)
{
	if (Str == NULL || m_str == NULL) {
		return 0;
	}
	char *str1 = NULL;
	int i = 1;
	int u = GetLineNum(m_str);
	while (1){
		str1 = GetLineTxT(m_str,i);
		if (str1 == NULL){
			return 0;
		}
		if (JumpTxTExits(str1, Str) == 1){
			break;
		}
		if (i > u){
			return 0;
		}
		++i;
	}
	return i;
}

unsigned int iniGet(const char* section, const char* key, const char *defaultStr, char *strBuf, unsigned int strBufSize, const char *fileName)
{
	
	if (section == NULL || key == NULL || strBuf == NULL || strBufSize == 0 || fileName == NULL) {
		return 0;
	}
	unsigned int Speck = 1;
	
	
	char *formatStr = (char*)malloc(strlen(section) + 3);	//[]\0
	memset(formatStr, 0, strlen(section) + 3);
	sprintf(formatStr, "[%s]", section); 
	
	
	FILE *fp = fopen(fileName, "r");
	if (fp == NULL){
		strcpy(strBuf, defaultStr);
		return 0;
	}
	fseek(fp, 0L, SEEK_END);
	signed long long int size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	char *txt = (char*)malloc(size + 1);
	memset(txt, 0, size+1);
	fread(txt, size, 1, fp);
	fclose(fp);
	
	
	int y = GetStrTxTIndx(txt, formatStr);
	if (y == 0){
		strcpy(strBuf, defaultStr);
		return 0;
	}
	
	
	for (int i = y; i <= size-1; ++i){
		if (txt[i] == '\n'){
			for (int j = i; j <= size - 1; ++j){
				if (txt[j] == '['){	
					for (int h = j; h <= size - 1; ++h){
						txt[h] = '\0';
					}
					goto t;	
				}
			}
		}
	}
t:
	{
		
		int Lin = GetTxTLineNum(txt, (char*)key);
		if (Lin == 0){
			strcpy(strBuf, defaultStr);
			return 0;
		}
		
		char* st = GetLineTxT(txt, Lin);
		char* stcopy = (char*)malloc(strlen(st) + 1);
		memset(stcopy, 0, strlen(st) + 1);
		strcpy(stcopy, st);
		free(st);
		st = NULL;
		int yunsi = 0;
		
		for (int i = 0; i <= strlen(stcopy) - 1; ++i){
			if (stcopy[i] == '='){
				if (Speck){
					if (stcopy[i + 1] == ' '){
						i = i + 1;
						for (; i <= strlen(stcopy) - 1; ++i){
							if (stcopy[i] != ' ')break;
	 
						}
						i = i - 1;
					}
				}
				for (int j = i + 1; j<=strlen(stcopy)-1; ++j){
					++yunsi;
				}
				int hk = 0;
				for (int j = i + 1; j <= strlen(stcopy) - 1; ++j){
					if ((stcopy[j] == '\r' && stcopy[j+1] == '\n')|| (stcopy[j] == '\n')){
						return 1;
					}
					strBuf[hk++] = stcopy[j];
				}
				return 1;
			}
		}
		return 0;
	}
 
}


unsigned int iniSet(const char* section, const char* key, const char* Data, const char *fileName)
{
	if (section == NULL || key == NULL || fileName == NULL) {
		return 0;
	}
	unsigned int Speck = 1;
	char *formatStr = (char*)malloc(strlen(section) + 3);	//[]\0
	memset(formatStr, 0, strlen(section) + 3);
	sprintf(formatStr, "[%s]", section);
	FILE *fp = fopen(fileName, "r");
	char* flun = NULL;	
	if (fp == NULL){
		return 0;
	}
	fseek(fp, 0L, SEEK_END);
	signed long long int size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	char *txt = (char*)malloc(size + 1);
	memset(txt, 0, size + 1);
	flun = (char*)malloc(size + 1);
	memset(flun, 0, size + 1);
	char* jnmhand = (char*)malloc(size + 1+strlen(Data));	
	memset(jnmhand, 0, size + 1 + strlen(Data));
	fread(txt, size, 1, fp);
	int y = GetStrTxTIndx(txt, formatStr);
	if (y == 0){
		return 0;
	}
	int yi = 0;
	if (y != 1){
		for (int i = 0; i <= y - 1; ++i){
			jnmhand[i] = txt[i];
		}
	}
	for (int i = y; i <= size - 1; ++i){
		flun[yi++] = txt[i-1];
		if (txt[i] == '\n'){
			for (int j = i; j <= size - 1; ++j){
				flun[yi++] = txt[j];
				if (txt[j] == '['){	
					for (int h = j; h <= size - 1; ++h){
						//flun[yi++] = txt[h];
						txt[h] = '\0';
					}
					goto t;
				}
			}
		}
	}
t:
	{	
		int Lin = GetTxTLineNum(flun, (char*)key);
		if (Lin == 0){
			return 0;
		}
		char* setdata = (char*)malloc(size + strlen(Data) + 1);
		memset(setdata, 0,size + strlen(Data) + 1);
		int nn = 0;
		int gh = 0;
		unsigned int kj = 0;	
		for (int i = 0; i <= strlen(flun) - 1; ++i){
			if (flun[i] == '\n'){
				++nn;
			}
			if (nn == Lin - 1){	
				++nn;
				for (int j = i; j <= strlen(flun) - 1; ++j){	
					setdata[gh++] = flun[j];
					if (kj == 0){
						//kj = 1;
						if (flun[j] == '='){
							
							if (Speck){
								j = j + 1;
								for (; j <= strlen(flun); ++j){
									if (flun[j] != ' '){
										break;
									}
								}
								j = j - 1;
								i = j;
							}
							int n = 0;
							for (; n <= strlen(Data) - 1; ++n){
								setdata[gh++] = Data[n];
							}
							for (;;){
								++i;
								if (flun[i] == '\n' || flun[i] == '\0'){
									break;
								}
							}
							
							//i = i + n;
							break;
						}
					}
				}
			}
			setdata[gh++] = flun[i];
			
		}
		strcat(jnmhand, setdata);
		
		free(txt);
		free(setdata);
		
		fp = fopen(fileName, "w");
		if (fp == NULL){
			return 0;
		}
		fwrite(jnmhand, strlen(jnmhand), 1, fp);
		fclose(fp);
		return 1;
	}
}

