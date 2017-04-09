#include <iostream>
#include <fstream>
#include <string>
#include <map>
using namespace std;
/******************************************/
/*               仿计算机存储结构         */
const unsigned int nk_data_memory_size = 1000;
const unsigned int nk_text_memory_size = 1000;
int nk_data[nk_data_memory_size];
string nk_text[nk_text_memory_size];
map<string, int> static_data_map;
map<string, int> label_text_map;
enum dataType{word,erro_type}; // this time just support 4bytes word
unsigned int instructionCount = 0;
unsigned int datahead = 0;
/*******register*****/
string register_name[32] = { "zero","at","v0","v1","a0" ,"a1" ,"a2" ,"a3","t0","t1","t2","t3","t4","t5","t6","t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","s8","r0" };
map<string, int> nk_register;
int nk_reg[32];
/*******supported opcode*******/
string opcode_name[16] = { "addiu","li","beq","lw","syscall","sw","sll","la","move","bne","addu","subu","slt","jr","j","jal" };
map<string, int> nk_opcode;
/*******programConter*****/
unsigned int pc = 0;
bool terminal;
string currentInstruction;
string currOpcode;
string currOprand1;
string currOprand2;
string currOprand3;
/******************************************/
/// 从指定文件装载指令到内存
void initialHardware();
int loadMemory(string fileName);
string cutcomment(char*, int len);
string getVariableName(char*, int len);
dataType getVariableData(char*, int len);
int getSingleData(char* str, int len);
void getArrayData(char* str, int len, int& arrayLength, int *& tmpdata);
char * strim(char * str);
bool isArray(char * str);
void execution();
/******************指令处理辅助函数************/
void parse_Operand(const string& opcode,const string& instruction, string& oprand1);
void parse_Operand(const string& opcode, const string& instruction, string& oprand1,string& oprand2);
void parse_Operand(const string& opcode, const string& instruction, string& oprand1, string& oprand2,string& oprand3);
void parse_Opcode(string& opcode,const  string& insturciton);
bool check_number(const string& str);
unsigned int parse_memory_offset(const string& instruction);
/***********************************************/
void main() {
	/// initial hardware
	initialHardware();
	/// load mips asm file
	loadMemory("bubble_assemblycode.mipsasm");
	/// execution program
	execution();
	system("pause");
}
int loadMemory(string fileName) {
	ifstream file(fileName,ios::in);
	char tmpcode[256];
	string tmpstr;
	bool fail = true;
	//dealing with data section
	///find data sect
	while (file.getline(tmpcode, 256, '\n')) {
		tmpstr = cutcomment(tmpcode,256);
		const char* tc = tmpstr.c_str();
		strncpy(tmpcode, tc, strlen(tc));
		if (strstr(tmpcode,".data")!=NULL) {
			fail = false;
			break;
		}
	}
	if (fail) {
		file.close();
		return 0;
	}
	/// load data in data memory
	fail = true;
	while (file.getline(tmpcode, 256, '\n')) {
		tmpstr = cutcomment(tmpcode, 256);
		const char* tc = tmpstr.c_str();
		strncpy(tmpcode, tc, strlen(tc));
		if (strstr(tmpcode,".text")!=NULL) {
			fail = false;
			break;
		}
		else if (tmpstr.empty())
			continue;
		string variableName= getVariableName(tmpcode, 256);
		if (getVariableData(tmpcode, 256) != word || variableName.empty())
			break;
		if (isArray(tmpcode)) {
			int arraylength = 0;
			int* arr=NULL;
			getArrayData(tmpcode, 256, arraylength, arr);
			static_data_map[variableName] = datahead;
			for (int i = 0; i < arraylength; i++)
				nk_data[i + datahead] = arr[i];
			delete[] arr;
			datahead = datahead + arraylength;
		}
		else {
			nk_data[datahead++] = getSingleData(tmpcode, 256);
		}
	}
	if (fail) {
		file.close();
		return 0;
	}
	fail = true;
	/// load test in test memory
	while (file.getline(tmpcode, 256, '\n')) {
		tmpstr = cutcomment(tmpcode, 256);
		const char* tc = tmpstr.c_str();
		strncpy(tmpcode, tc, strlen(tc));
		tmpcode[strlen(tc)] = '\0';
		char* pch = strim(tmpcode);
		if (strlen(pch)==0)
			continue;
		if (tmpstr.find(':') != -1) {
			char* pch1 = strstr(pch, ":");
			strncpy(tmpcode, pch, pch1-pch);
			tmpcode[pch1 - pch] = '\0';
			label_text_map[string(tmpcode)] = instructionCount;
		}
		else {
			nk_text[instructionCount++] = tmpstr;
		}
	}
	fail = false;
	file.close();
	return 1;
}
string cutcomment(char* str, int len) {
	char tmp[256];
	char* pch = strstr(str, "#");
	if (pch == NULL)
		return string(str);
	strncpy(tmp, str, (pch-str));
	tmp[pch - str] = '\0';
	return string(tmp);
}
char * strim(char * str)
{
	char * tail = str;
	char * next = str;
	while (*next)
	{
		if (*next != ' '&& *next != '\t')
		{
			if (tail < next)
				*tail = *next;
			tail++;
		}
		next++;
	}
	*tail = '\0';
	return str;
}
string getVariableName(char* str, int len) {
	char tmp[256];
	char* pch = strstr(str, ":");
	if (pch == NULL)
		return string();
	strncpy(tmp, str, (pch - str));
	tmp[pch - str] = '\0';
	pch = strim(tmp);
	return string(pch);
}
dataType getVariableData(char* str, int len) {
	char* pch = strstr(str, ".word");
	if (pch != NULL)
		return word;
	else
		return erro_type;
}
bool isArray(char * str) {
	char* pch = strstr(str, ",");
	if (pch == NULL)
		return false;
	else
		return true;
}
int getSingleData(char* str, int len) {
	char tmp[256];
	char* pch = strstr(str, ".word");
	pch = pch + 5;
	strncpy(tmp, pch, strlen(str)-(pch - str));
	tmp[strlen(str) - (pch - str)] = '\0';
	pch = strim(tmp);
	return atoi(pch);
}
void getArrayData(char* str, int len, int& arrayLength,int *& tmpdata) {
	tmpdata = new int[256];
	int count = 0;
	char tmp[256];
	char tmpint[5];
	char* pch = strstr(str, ".word");
	pch = pch + 5;
	strncpy(tmp, pch, strlen(str) - (pch - str));
	tmp[strlen(str) - (pch - str)] = '\0';
	char* striC = strim(tmp);
	pch = strstr(striC, ",");
	while (pch != NULL) {
		strncpy(tmpint, striC, pch - striC);
		tmpint[pch - striC] = '\0';
		tmpdata[count++] = atoi(tmpint);
		striC = pch + 1;
		pch = strstr(striC, ",");
	}
	strncpy(tmpint, striC, 5);
	tmpint[4] = '\0';
	pch = strim(tmpint);
	tmpdata[count++] = atoi(pch);
	arrayLength = count;
}
void initialHardware() {
	//initial register
	for (int i = 0; i < 32; i++) {
		nk_register[register_name[i]] = i;
		nk_reg[i] = 0;
	}
	pc = 0;
	terminal = false;
	for (int i = 0; i < 16; i++)
		nk_opcode[opcode_name[i]] = i;
}
void execution() {
	while (!terminal) {
		//fetch instruction
		currentInstruction = nk_text[pc];
		if (pc == 75)
			int s = 0;
		pc = pc + 1;
		//excution
		parse_Opcode(currOpcode, currentInstruction);
		switch (nk_opcode[currOpcode]) {
		case 0: //addiu	sp,sp,9000 
			parse_Operand(currOpcode, currentInstruction, currOprand1, currOprand2, currOprand3);
			nk_reg[nk_register[currOprand1]] = nk_reg[nk_register[currOprand2]] + stoi(currOprand3);
			break;
		case 1: //li		a1,32	
			parse_Operand(currOpcode, currentInstruction, currOprand1, currOprand2);
			nk_reg[nk_register[currOprand1]] = stoi(currOprand2);
			break;
		case 2:  //beq		t2,t3,print_loop_end  I'm weird,but so so,because offset jump is practical
			parse_Operand(currOpcode, currentInstruction, currOprand1, currOprand2, currOprand3);
			if(nk_reg[nk_register[currOprand1]] == nk_reg[nk_register[currOprand2]])
				pc = check_number(currOprand3) ? stoi(currOprand3) : label_text_map[currOprand3];
			break;
		case 3: //lw		a0,0(t1) # parameter
			parse_Operand(currOpcode, currentInstruction, currOprand1, currOprand2);
			nk_reg[nk_register[currOprand1]] = nk_data[parse_memory_offset(currOprand2)];
			break;
		case 4: //syscall number 0 is terminate 1 is print func
			if (nk_reg[nk_register["a1"]] == 0)
				return;
			if (nk_reg[nk_register["a1"]] == 1)
				cout << nk_reg[nk_register["a0"]] << "  ";
			break;
		case 5: //sw		s8,28(sp)
			parse_Operand(currOpcode, currentInstruction, currOprand1, currOprand2);
			nk_data[parse_memory_offset(currOprand2)]=nk_reg[nk_register[currOprand1]];
			break;
		case 6: //sll	v0,v0,2
			parse_Operand(currOpcode, currentInstruction, currOprand1, currOprand2, currOprand3);
			nk_reg[nk_register[currOprand1]] = (nk_reg[nk_register[currOprand2]]) << stoi(currOprand3);
			break;
		case 7: //la 		t1, num   memory address
			parse_Operand(currOpcode, currentInstruction, currOprand1, currOprand2);
			nk_reg[nk_register[currOprand1]] = check_number(currOprand2) ? stoi(currOprand2) : static_data_map[currOprand2];
			break;
		case 8: //move	sp,zero
			parse_Operand(currOpcode, currentInstruction, currOprand1, currOprand2);
			nk_reg[nk_register[currOprand1]] = nk_reg[nk_register[currOprand2]];
			break;
		case 9: //bne zero,v0,inerloop_ini
			parse_Operand(currOpcode, currentInstruction, currOprand1, currOprand2,currOprand3);
			if (nk_reg[nk_register[currOprand1]] != nk_reg[nk_register[currOprand2]])
				pc = check_number(currOprand3) ? stoi(currOprand3) : label_text_map[currOprand3];
			break;
		case 10: //addu	v0,v1,v0
			parse_Operand(currOpcode, currentInstruction,currOprand1, currOprand2, currOprand3);
			nk_reg[nk_register[currOprand1]] = nk_reg[nk_register[currOprand2]] + nk_reg[nk_register[currOprand3]];
			break;
		case 11: //subu	v1,v1,v0
			parse_Operand(currOpcode, currentInstruction,currOprand1, currOprand2, currOprand3);
			nk_reg[nk_register[currOprand1]] = nk_reg[nk_register[currOprand2]] - nk_reg[nk_register[currOprand3]];
			break;
		case 12: //slt	v0,v0,v1 if ($2 < $3), $1 = 1 else $1 = 0
			parse_Operand(currOpcode, currentInstruction,currOprand1, currOprand2, currOprand3);
			if (nk_reg[nk_register[currOprand2]] < nk_reg[nk_register[currOprand3]])
				nk_reg[nk_register[currOprand1]] = 1;
			else
				nk_reg[nk_register[currOprand1]] = 0;
			break;
		case 13:  //jr	ra
			parse_Operand(currOpcode, currentInstruction,currOprand1);
			if (currOprand1 == "ra") {
				pc = nk_reg[31];
			}
			else
				throw;
			break;
		case 14:	//j 10000
			parse_Operand(currOpcode, currentInstruction, currOprand1);
			pc = check_number(currOprand1) ? stoi(currOprand1) : label_text_map[currOprand1];
			break;
		case 15:	// jal		bubble_sort
			nk_reg[31] = pc;
			parse_Operand(currOpcode, currentInstruction, currOprand1);
			pc = check_number(currOprand1) ? stoi(currOprand1) : label_text_map[currOprand1];
			break;
		}
	}
}
void parse_Operand(const string& opcode, const string& instruct, string& oprand1) {
	char tmp[256];
	char instruction[256];
	strcpy(instruction, instruct.c_str());
	char* pch = strim(instruction);
	strncpy(tmp, pch +opcode.length(),strlen(pch)-opcode.length());
	tmp[strlen(pch) - opcode.length()] = '\0';
	pch = strim(tmp);
	oprand1 = string(pch);
}
void parse_Operand(const string& opcode, const string& instruct, string& oprand1, string& oprand2) {
	char tmp[256];
	char instruction[256];
	char soprand1[256];
	strcpy(instruction, instruct.c_str());
	char* pch = strim(instruction);
	strncpy(tmp, pch + opcode.length(), strlen(pch) - opcode.length());
	tmp[strlen(pch) - opcode.length()] = '\0';
	pch = strim(tmp);
	char* pch1 = strstr(pch, ",");
	strncpy(soprand1, pch, pch1 - pch);
	soprand1[pch1 - pch] = '\0';
	oprand1 = string(soprand1);
	strcpy(soprand1, pch1+1);
	oprand2 = string(soprand1);
}
void parse_Operand(const string& opcode, const string& instruct, string& oprand1, string& oprand2,string& oprand3) {
	char tmp[256];
	char instruction[256];
	char soprand1[256];
	strcpy(instruction, instruct.c_str());
	char* pch = strim(instruction);
	strncpy(tmp, pch + opcode.length(), strlen(pch) - opcode.length());
	tmp[strlen(pch) - opcode.length()] = '\0';
	pch = strim(tmp);
	char* pch1 = strstr(pch, ",");
	strncpy(soprand1, pch, pch1 - pch);
	soprand1[pch1 - pch] = '\0';
	oprand1 = string(soprand1);
	pch = pch1+1;
	pch1 = strstr(pch, ",");
	strncpy(soprand1, pch, pch1 - pch);
	soprand1[pch1 - pch] = '\0';
	oprand2 = string(soprand1);
	strcpy(soprand1, pch1 + 1);
	oprand3 = string(soprand1);
}
void parse_Opcode(string& opcode, const  string& instruct) {
	char tmp[256];
	int i = 0;
	while (instruct[i] == ' ' || instruct[i] == '\t')
		i++;
	int j = i;
	for (i = 0; i < instruct.length()-j; i++) {
		if (instruct[i+j] != ' ' && instruct[i+j] != '\t')
			tmp[i] = instruct[i+j];
		else {
			tmp[i] = '\0';
			break;
		}
	}
	if (i + j == instruct.length())
		tmp[i] = '\0';
	opcode = string(tmp);
}
bool check_number(const string& str) {
	for (int i = 0; i < str.length(); i++) {
		if (str[i] >= '0'&&str[i] <= '9')
			continue;
		else
			return false;
	}
	return true;
}
unsigned int parse_memory_offset(const string& instruct) {
	char tmp[256];
	int tmpint = 0;
	int i;
	for (i = 0; i < instruct.length(); i++) {
		if (instruct[i] != '(')
			tmp[i] = instruct[i];
		else {
			tmp[i] = '\0';
			break;
		}
	}
	tmpint = stoi(string(tmp));
	int j = i+1;
	for (i=0; i < instruct.length()-j; i++) {
		if (instruct[i+j] != ')')
			tmp[i] = instruct[i+j];
		else {
			tmp[i] = '\0';
			break;
		}
	}
	tmpint = tmpint + nk_reg[nk_register[string(tmp)]];
	return tmpint;
}