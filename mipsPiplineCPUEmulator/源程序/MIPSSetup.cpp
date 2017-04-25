#include "stdafx.h"
#include "MIPSSetup.h"
namespace MipsVirtual {
	/******************************************/
	/*               仿计算机存储结构         */
	const unsigned int nk_data_memory_size = 1000;
	const unsigned int nk_text_memory_size = 1000;
	int nk_data[nk_data_memory_size];
	string nk_text[nk_text_memory_size];
	map<string, int> static_data_map;
	map<string, int> static_data_length;
	map<string, int> label_text_map;
	
	unsigned int instructionCount = 0;
	unsigned int datahead = 0;
	/*******register*****/
	string register_name[32] = { "zero","at","v0","v1","a0" ,"a1" ,"a2" ,"a3","t0","t1","t2","t3","t4","t5","t6","t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","s8","ra" };
	map<string, int> nk_register;
	int nk_reg[32];
	/*******supported opcode*******/
	string opcode_name[16] = { "addiu","li","beq","lw","syscall","sw","sll","la","move","bne","addu","subu","slt","jr","j","jal" };
	map<string, int> nk_opcode;
	/*******programConter*****/
	unsigned int pc = 0;
	bool terminal;
	/********************流水阻塞和旁通结构*******/
	boost::mutex flyingRegMutex;
	set<string> flyingReg_inerSet;
	ThreadSafeSet flyingReg(flyingRegMutex, flyingReg_inerSet);
	bool pipLineStallStatus[5] = { false,false,false,false,false };
	bool workFinished[5] = { false,false,false,false,false};
	void freeFlyingReg(string currentInstruction ,int currOpcodeNum) {
		string currOprand1;
		string currOprand2;
		string currOprand3;
		string currOpcode = opcode_name[currOpcodeNum];
		string tmp;
		switch (currOpcodeNum) {
		case 10: //addu	v0,v1,v0
		case 11: //subu	v1,v1,v0
		case 12: //slt	v0,v0,v1 if ($2 < $3), $1 = 1 else $1 = 0
		case 0: //addiu	sp,sp,9000 
		case 6: //sll	v0,v0,2
			parse_Operand(currOpcode, currentInstruction, currOprand1, currOprand2, currOprand3);
			/********结果寄存器释放*****/
			flyingReg.erase(currOprand1);
			/*****************************/
			break;
		case 8: //move	sp,zero
		case 7: //la 		t1, num   memory address
		case 1: //li		a1,32	
		case 3: //lw		a0,0(t1) # parameter
			parse_Operand(currOpcode, currentInstruction, currOprand1, currOprand2);
			/********结果寄存器被释放*****/
			flyingReg.erase(currOprand1);
			/*****************************/
			break;
		}
	}
	/********************流水线方便显示数据结构******/
	boost::condition clockCon;
	int timeBar[timeSlideNum][6];
	/********************段流水寄存器******/
	InstructionSturct IF_ID_IR ;
	InstructionSturct ID_EX_IR ;
	InstructionSturct EX_MEM_IR;
	InstructionSturct MEM_WB_IR ;
	unsigned int IF_ID_NPC = 0;
	unsigned int ID_EX_NPC = 0;
	int EX_MEM_opCode = 0;
	int ID_EX_A=0;
	int ID_EX_B=0;
	int ID_EX_IMM = 0;
	int EX_MEM_ALUOutput = 0;
	int EX_MEM_B = 0;
	bool EX_MEM_cond = false;
	int MEM_WB_ALUOutput = 0;
	int MEM_WB_LMD = 0;
	/********************段流水中间结果******/
	bool getIDregisterResult(const string& currentInstruction,int currOpcodeNum, int& A, int& B, int& Imm) {
		string currOprand1;
		string currOprand2;
		string currOprand3;
		string currOpcode = opcode_name[currOpcodeNum];
		string tmp;
		switch (currOpcodeNum) {
		case 9: //bne zero,v0,inerloop_ini
		case 2://beq		t2,t3,print_loop_end  I'm weird,but so so,because offset jump is practical
			parse_Operand(currOpcode, currentInstruction, currOprand1, currOprand2, currOprand3);
			/********检查资源寄存器是否可用*****/
			if (flyingReg.find(currOprand1) != flyingReg.end() || flyingReg.find(currOprand2) != flyingReg.end())
				return false;
			/*****************************/
			A = nk_reg[nk_register[currOprand1]]; //opA
			B = nk_reg[nk_register[currOprand2]]; //opB
			Imm = stoi(currOprand3); //opImC
			break;
		case 0: //addiu	sp,sp,9000 
		case 6: //sll	v0,v0,2
			parse_Operand(currOpcode, currentInstruction, currOprand1, currOprand2, currOprand3);
			/********检查资源寄存器是否可用*****/
			if (flyingReg.find(currOprand2) != flyingReg.end())
				return false;
			/********结果寄存器被阻塞*****/
			flyingReg.insert(currOprand1);
			/*****************************/
			B = nk_reg[nk_register[currOprand1]]; //des
			A = nk_reg[nk_register[currOprand2]]; //opA
			Imm = stoi(currOprand3); //opB
			break;
		case 7: //la 		t1, num   memory address
		case 1: //li		a1,32	
			parse_Operand(currOpcode, currentInstruction, currOprand1, currOprand2);
			/********结果寄存器被阻塞*****/
			flyingReg.insert(currOprand1);
			/*****************************/
			B = nk_reg[nk_register[currOprand1]]; //des
			Imm = stoi(currOprand2); //opA
			break;
		case 8: //move	sp,zero
			parse_Operand(currOpcode, currentInstruction, currOprand1, currOprand2);
			/********检查资源寄存器是否可用*****/
			if (flyingReg.find(currOprand2) != flyingReg.end())
				return false;
			/********结果寄存器被阻塞*****/
			flyingReg.insert(currOprand1);
			/*****************************/
			B = nk_reg[nk_register[currOprand1]]; //des
			A = nk_reg[nk_register[currOprand2]]; //opA
			break;
		case 3: //lw		a0,0(t1) # parameter
			parse_Operand(currOpcode, currentInstruction, currOprand1, currOprand2);
			Imm = parse_memory_offset(currOprand2, tmp);//opB
			/********检查资源寄存器*****/
			if (flyingReg.find(tmp) != flyingReg.end())
				return false;
			/********结果寄存器被阻塞*****/
			flyingReg.insert(currOprand1);
			/*****************************/
			A = nk_reg[nk_register[tmp]]; //opA
			B = nk_reg[nk_register[currOprand1]]; //des
			break;
		case 5: //sw		s8,28(sp)
			parse_Operand(currOpcode, currentInstruction, currOprand1, currOprand2);
			Imm = parse_memory_offset(currOprand2, tmp);//opB
			/********检查资源寄存器是否可用*****/
			if (flyingReg.find(tmp) != flyingReg.end()|| flyingReg.find(currOprand1) != flyingReg.end())
				return false;
			/********结果内存单元被阻塞*****/

			/*****************************/
			A = nk_reg[nk_register[tmp]]; //opA
			B = nk_reg[nk_register[currOprand1]]; //des
			break;
		case 10: //addu	v0,v1,v0
		case 11: //subu	v1,v1,v0
		case 12: //slt	v0,v0,v1 if ($2 < $3), $1 = 1 else $1 = 0
			parse_Operand(currOpcode, currentInstruction, currOprand1, currOprand2, currOprand3);
			/********检查资源寄存器是否可用*****/
			if (flyingReg.find(currOprand2) != flyingReg.end() || flyingReg.find(currOprand3) != flyingReg.end())
				return false;
			/********结果寄存器被阻塞*****/
			flyingReg.insert(currOprand1);
			/*****************************/
			A = nk_reg[nk_register[currOprand2]]; //opA
			Imm = nk_reg[nk_register[currOprand3]]; //opB
			B = nk_reg[nk_register[currOprand1]]; //des
			break;
		case 13:  //jr	ra
			parse_Operand(currOpcode, currentInstruction, currOprand1);
			/********检查资源寄存器是否可用*****/
			if (flyingReg.find(currOprand1) != flyingReg.end())
				return false;
			/*****************************/
			A = nk_reg[nk_register[currOprand1]]; //opA
			break;
		case 14:	//j 10000
		case 15:	// jal		bubble_sort
			parse_Operand(currOpcode, currentInstruction, currOprand1);
			Imm = stoi(currOprand1); //opB
			break;
		case 4: //syscall number 0 is terminate 1 is print func
			//haven't found it solutions yet
			break;
		}
		return true;
	}
	void getEXregisterResult(int opCodeNum, int& ALUoutput, int& tmpB, bool& Cond,int A,int B,int Imm) {
		switch (opCodeNum) {
		case 0: //addiu	sp,sp,9000 
		case 10: //addu	v0,v1,v0
			ALUoutput = A + Imm;
			break;
		case 12: //slt	v0,v0,v1 if ($2 < $3), $1 = 1 else $1 = 0
		case 11: //subu	v1,v1,v0
			ALUoutput = A - Imm;
			break;
		case 7: //la 		t1, num   memory address
		case 1: //li		a1,32	
			ALUoutput = Imm;
			break;
		case 2:  //beq		t2,t3,print_loop_end  I'm weird,but so so,because offset jump is practical
			if (A == B)
				Cond = true;
			else
				Cond = false;
			ALUoutput = Imm;
			break;
		case 9: //bne zero,v0,inerloop_ini
			if (A == B)
				Cond = false;
			else
				Cond = true;
			ALUoutput = Imm;
			break;
		case 5: //sw		s8,28(sp)
		case 3: //lw		a0,0(t1) # parameter
			ALUoutput = A + Imm;
			tmpB = B;
			break;
		case 4: //syscall number 0 is terminate 1 is print func
			//I tempily have no idea of this one
			break;
		case 6: //sll	v0,v0,2
			ALUoutput = A<<Imm;
			break;
		case 8: //move	sp,zero
			ALUoutput = A;
			break;
		case 13:  //jr	ra
			ALUoutput = A;
			Cond = true;
			break;
		case 14:	//j 10000
		case 15:	// jal		bubble_sort
			nk_reg[nk_register["ra"]] = pc + 1;
			ALUoutput = Imm;
			Cond = true;
			break;
		}
	}
	void getMEMregisterResult(int opCodeNum, int& ALUoutput, int& LMD, int EX_MEMOutput, int B) {
		switch (opCodeNum) {
		case 0: //addiu	sp,sp,9000 
		case 10: //addu	v0,v1,v0
		case 12: //slt	v0,v0,v1 if ($2 < $3), $1 = 1 else $1 = 0
		case 11: //subu	v1,v1,v0
		case 8: //move	sp,zero
		case 7: //la 		t1, num   memory address
		case 6: //sll	v0,v0,2
		case 1: //li		a1,32
			ALUoutput = EX_MEMOutput;
			break;
		case 5: //sw		s8,28(sp)
			nk_data[EX_MEMOutput] = B;
			break;
		case 3: //lw		a0,0(t1) # parameter
			LMD = nk_data[EX_MEMOutput];
			break;
		case 4: //syscall number 0 is terminate 1 is print func
				//I tempily have no idea of this one
			break;
		}
	}
	void WriteBack2Register(unsigned int InstrN, int ALUOutput, int LMD){
		string currentInstruction = nk_text[InstrN];
		string currOpcode;
		string currOprand1;
		string currOprand2;
		string currOprand3;
		parse_Opcode(currOpcode, currentInstruction);
		switch (nk_opcode[currOpcode]) {
		case 0: //addiu	sp,sp,9000 
		case 10: //addu	v0,v1,v0
		case 11: //subu	v1,v1,v0
		case 6: //sll	v0,v0,2
			parse_Operand(currOpcode, currentInstruction, currOprand1, currOprand2, currOprand3);
			nk_reg[nk_register[currOprand1]] = ALUOutput;
			/********释放被阻塞的结果寄存器*****/
			flyingReg.erase(currOprand1);
			/*****************************/
			break;
		case 12: //slt	v0,v0,v1 if ($2 < $3), $1 = 1 else $1 = 0
			parse_Operand(currOpcode, currentInstruction, currOprand1, currOprand2, currOprand3);
			if (ALUOutput < 0)
				nk_reg[nk_register[currOprand1]] = 1;
			else
				nk_reg[nk_register[currOprand1]] = 0;
			/********释放被阻塞的结果寄存器*****/
			flyingReg.erase(currOprand1);
			/*****************************/
			break;
		case 8: //move	sp,zero
		case 7: //la 		t1, num   memory address
		case 1: //li		a1,32
			parse_Operand(currOpcode, currentInstruction, currOprand1, currOprand2);
			nk_reg[nk_register[currOprand1]] = ALUOutput;
			/********释放被阻塞的结果寄存器*****/
			flyingReg.erase(currOprand1);
			/*****************************/
			break;
		case 3: //lw		a0,0(t1) # parameter
			parse_Operand(currOpcode, currentInstruction, currOprand1, currOprand2);
			nk_reg[nk_register[currOprand1]] = LMD;
			/********释放被阻塞的结果寄存器*****/
			flyingReg.erase(currOprand1);
			/*****************************/
			break;
		case 4: //syscall number 0 is terminate 1 is print func
				//I tempily have no idea of this one
			break;
		}

	}
	/***************************************************************/
	/// 从指定文件装载指令到内存
	void mipsVM::InitialVM() {
		initialHardware();
		loadMemory(assemblyFileName);
		convertLable2LineNumber();
	}
	int loadMemory(string fileName) {
		ifstream file(fileName, ios::in);
		char tmpcode[256];
		string tmpstr;
		bool fail = true;
		//dealing with data section
		///find data sect
		while (file.getline(tmpcode, 256, '\n')) {
			tmpstr = cutcomment(tmpcode, 256);
			const char* tc = tmpstr.c_str();
			strncpy(tmpcode, tc, strlen(tc));
			if (strstr(tmpcode, ".data") != NULL) {
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
			if (strstr(tmpcode, ".text") != NULL) {
				fail = false;
				break;
			}
			else if (tmpstr.empty())
				continue;
			string variableName = getVariableName(tmpcode, 256);
			if (getVariableData(tmpcode, 256) != word || variableName.empty())
				break;
			if (isArray(tmpcode)) {
				int arraylength = 0;
				int* arr = NULL;
				getArrayData(tmpcode, 256, arraylength, arr);
				static_data_map[variableName] = datahead;
				for (int i = 0; i < arraylength; i++)
					nk_data[i + datahead] = arr[i];
				static_data_length[variableName] = arraylength;
				delete[] arr;
				datahead = datahead + arraylength;
			}
			else {
				nk_data[datahead++] = getSingleData(tmpcode, 256);
				static_data_length[variableName] = 1;
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
			if (strlen(pch) == 0)
				continue;
			if (tmpstr.find(':') != -1) {
				char* pch1 = strstr(pch, ":");
				strncpy(tmpcode, pch, pch1 - pch);
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
	void convertLable2LineNumber() {
		for (int i = 0; i < instructionCount; i++) {
			convertLable2LineNumber(nk_text[i]);
		}
	}
	void convertLable2LineNumber(string& Instruction) {
		for (map<string, int>::iterator it = label_text_map.begin(); it != label_text_map.end(); ++it) {
			if (Instruction.find(it->first) != std::string::npos) {
				int len = (it->first).length();
				int secondCheck = Instruction.find(it->first) + len;
				if (!(Instruction[secondCheck] == '\0' || Instruction[secondCheck] == ' ' || Instruction[secondCheck] == '\t' || Instruction[secondCheck] == '\n' || Instruction[secondCheck] == ','))
					continue;
				Instruction.replace(Instruction.find(it->first), len,to_string(it->second));
				break;
			}
		}
		for (map<string, int>::iterator it = static_data_map.begin(); it != static_data_map.end(); ++it) {
			if (Instruction.find(it->first) != std::string::npos) {
				int len = (it->first).length();
				int secondCheck = Instruction.find(it->first) + len;
				if (!(Instruction[secondCheck] == '\0' || Instruction[secondCheck] == ' ' || Instruction[secondCheck] == '\t' || Instruction[secondCheck] == '\n' || Instruction[secondCheck] == ','))
					continue;
				Instruction.replace(Instruction.find(it->first), len, to_string(it->second));
				break;
			}
		}
	}
	string cutcomment(char* str, int len) {
		char tmp[256];
		char* pch = strstr(str, "#");
		if (pch == NULL)
			return string(str);
		strncpy(tmp, str, (pch - str));
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
		strncpy(tmp, pch, strlen(str) - (pch - str));
		tmp[strlen(str) - (pch - str)] = '\0';
		pch = strim(tmp);
		return atoi(pch);
	}
	void getArrayData(char* str, int len, int& arrayLength, int *& tmpdata) {
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

	void parse_Operand(const string& opcode, const string& instruct, string& oprand1) {
		char tmp[256];
		char instruction[256];
		strcpy(instruction, instruct.c_str());
		char* pch = strim(instruction);
		strncpy(tmp, pch + opcode.length(), strlen(pch) - opcode.length());
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
		strcpy(soprand1, pch1 + 1);
		oprand2 = string(soprand1);
	}
	void parse_Operand(const string& opcode, const string& instruct, string& oprand1, string& oprand2, string& oprand3) {
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
		pch = pch1 + 1;
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
		for (i = 0; i < instruct.length() - j; i++) {
			if (instruct[i + j] != ' ' && instruct[i + j] != '\t')
				tmp[i] = instruct[i + j];
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
		int j = i + 1;
		for (i = 0; i < instruct.length() - j; i++) {
			if (instruct[i + j] != ')')
				tmp[i] = instruct[i + j];
			else {
				tmp[i] = '\0';
				break;
			}
		}
		tmpint = tmpint + nk_reg[nk_register[string(tmp)]];
		return tmpint;
	}
	unsigned int parse_memory_offset(const string& instruct, string& baseRegister) {
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
		int j = i + 1;
		for (i = 0; i < instruct.length() - j; i++) {
			if (instruct[i + j] != ')')
				tmp[i] = instruct[i + j];
			else {
				tmp[i] = '\0';
				break;
			}
		}
		baseRegister = string(tmp);
		return tmpint;
	}
	bool isBranchOpcode(const int opcode) {
		if (opcode == 2 || opcode == 9 || opcode == 13 || opcode == 14 || opcode == 15)
			return true;
		else
			return false;
	}
	dataType getVariableData(char* str, int len) {
		char* pch = strstr(str, ".word");
		if (pch != NULL)
			return word;
		else
			return erro_type;
	}
	const wchar_t *GetWC(const string& s)
	{
		char c[512];
		strcpy(c, s.c_str());
		const size_t cSize = strlen(c) + 1;
		wchar_t* wc = new wchar_t[cSize];
		mbstowcs(wc, c, cSize);
		return wc;
	}
	string getRecentInstructionString() {
		int start = pc-4;
		if (0 > start)
			start = 0;
		const int insNum = 6;
		string tmp = to_string(start) +":\t"+ nk_text[start];
		for (int i = start+1; i < start + insNum; i++) {
			tmp = tmp + '\n' + to_string(i) + ":\t"+nk_text[i];
		}
		return tmp;
	}
	string getRegisterStatus() {
		string tmp = register_name[0] + " :\t" + to_string(nk_reg[0]) + "\t\t" + register_name[1] + " :\t" + to_string(nk_reg[1]);
		for (int i = 1; i < 16; i++) {
			tmp = tmp + '\n'+register_name[i*2] + " :\t" + to_string(nk_reg[i * 2]) + "\t\t" + register_name[i*2+1] + " :\t" + to_string(nk_reg[i * 2 + 1]);
		}
		return tmp;
	}
	string getPcString() {
		return "pc:  " + to_string(pc);
	}
	string getWorkHubLabelString() {
		string tmp = "Time\nIF\nID\nEX\nMEM\nWB";
		return tmp;
	}
	string getTimebarString(int index) {
		string tmp = to_string(timeBar[index][0]);
		for (int i = 1; i < 6; i++)
			tmp = tmp + '\n' + to_string(timeBar[index][i]);
		return tmp;
	}
}
