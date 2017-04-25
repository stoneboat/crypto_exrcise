#pragma once
#include <set>
#include <ctime>
#include <string>
#include <fstream>
#include <stdio.h>
#include <conio.h>
#include <map>
#include <boost\thread\shared_mutex.hpp>
#include <boost\thread\condition.hpp>
#include <boost\thread\mutex.hpp>
#include <boost\thread.hpp>
#include <windows.h>
using namespace std;
namespace MipsVirtual {
	/*****************Variable**************************************/
	extern boost::condition clockCon;
	/*******programConter*****/
	extern unsigned int pc;
	extern unsigned int instructionCount;
	/********************流水阻塞和旁通结构*******/
	class ThreadSafeSet {
	public:
		ThreadSafeSet(boost::mutex& _mutex,set<string>& __set) : visitMutex(_mutex), _set(__set){}
		boost::mutex& visitMutex;
		set<string>& _set;
		void insert(string value) {
			boost::lock_guard<boost::mutex> lock(visitMutex);
			_set.insert(value);
		}
		void erase(string value) {
			boost::lock_guard<boost::mutex> lock(visitMutex);
			_set.erase(value);
		}
		set<string>::iterator end() {
			boost::lock_guard<boost::mutex> lock(visitMutex);
			return _set.end();
		}
		void clear() {
			boost::lock_guard<boost::mutex> lock(visitMutex);
			_set.clear();
		}
		set<string>::iterator find(string value) {
			boost::lock_guard<boost::mutex> lock(visitMutex);
			return _set.find(value);
		}
	};
	extern bool pipLineStallStatus[5];
	extern ThreadSafeSet flyingReg;
	extern bool workFinished[5];
	void freeFlyingReg(string Instruction, int currOpcodeNum);
	/********************段流水寄存器******/
	class InstructionSturct {
	public:
		string instruction;
		int IM_num;
		int opcodeNum;
	};
	extern InstructionSturct IF_ID_IR;
	extern InstructionSturct ID_EX_IR;
	extern InstructionSturct EX_MEM_IR;
	extern InstructionSturct MEM_WB_IR;
	extern unsigned int IF_ID_NPC;
	extern unsigned int ID_EX_NPC;
	extern int ID_EX_A;
	extern int ID_EX_B;
	extern int ID_EX_IMM;
	extern int EX_MEM_ALUOutput;
	extern int EX_MEM_B;
	extern int EX_MEM_opCode;
	extern bool EX_MEM_cond;
	extern int MEM_WB_ALUOutput;
	extern int MEM_WB_LMD;
	/********************段流水中间结果******/
	bool getIDregisterResult(const string& currentInstruction, int currOpcodeNum, int& A, int& B, int& Imm);//如果没有被阻塞返回true否则返回false
	void getEXregisterResult(int opCodeNum, int& ALUoutput, int& tmpB, bool& Cond, int A, int B, int Imm);
	void getMEMregisterResult(int opCodeNum, int& ALUoutput, int& LMD, int EX_MEMOutput, int B);
	void WriteBack2Register(unsigned int InstrN, int ALUOutput, int LMD);
	/******************************************/
	/*               仿计算机存储结构         */
	extern const unsigned int nk_data_memory_size;
	extern const unsigned int nk_text_memory_size;
	extern int nk_data[1000];
	extern string nk_text[1000];
	extern map<string, int> static_data_map;
	extern map<string, int> static_data_length;
	extern map<string, int> label_text_map;
	extern unsigned int instructionCount;
	extern unsigned int datahead;
	/*******register*****/
	extern string register_name[32];
	extern map<string, int> nk_register;
	extern int nk_reg[32];
	/*******supported opcode*******/
	extern string opcode_name[16];
	extern map<string, int> nk_opcode;
	/******************************************/
	/********************流水线方便显示数据结构******/
	#define timeSlideNum 5
	extern int timeBar[timeSlideNum][6];
	extern bool terminal;
	enum dataType { word, erro_type }; // this time just support 4bytes word
	void initialHardware();
	int loadMemory(string fileName);
	void convertLable2LineNumber();
	void convertLable2LineNumber(string& Instruction);
	/****************字符串处理辅助函数*************/
	string cutcomment(char*, int len);
	string getVariableName(char*, int len);
	dataType getVariableData(char* str, int len); 
	int getSingleData(char* str, int len);
	void getArrayData(char* str, int len, int& arrayLength, int *& tmpdata);
	char * strim(char * str);
	bool isArray(char * str);
	const wchar_t *GetWC(const string& );
	/******************指令处理辅助函数************/
	void parse_Operand(const string& opcode, const string& instruction, string& oprand1);
	void parse_Operand(const string& opcode, const string& instruction, string& oprand1, string& oprand2);
	void parse_Operand(const string& opcode, const string& instruction, string& oprand1, string& oprand2, string& oprand3);
	void parse_Opcode(string& opcode, const  string& insturciton);
	bool check_number(const string& str);
	unsigned int parse_memory_offset(const string& instruction);
	unsigned int parse_memory_offset(const string& instruction, string& baseRegister);
	bool isBranchOpcode(const int opcode);
	/*#################虚拟机#####################*/
	class mipsVM {
	public:
		mipsVM(const string _mcFileName, const string postfrefix_assemblyFileName) {
			mcFileName = _mcFileName;
			assemblyFileName = _mcFileName + ":" + postfrefix_assemblyFileName;
		}
		mipsVM() {		}
		void InitialVM();
	private:
		string mcFileName;
		string assemblyFileName;
	};
	
	class workHub
	{
	public:
		workHub(int& _clockControl, int& _instrucNum, boost::shared_mutex& _clock,bool& _workDown) : clockControl(_clockControl), InstrucNum(_instrucNum), mutex_clock(_clock), workDown(_workDown){  }
		void operator() ()
		{
			boost::shared_lock<boost::shared_mutex> shared_lock(mutex_clock);
			shared_lock.unlock();
			shared_lock.lock();
			clockCon.wait(shared_lock);
			shared_lock.unlock();
			while (true) {
				if (clockControl == 0) {
					shared_lock.lock();
					workDown = false;
					pre_Execute();
					workDown = true;
					clockCon.wait(shared_lock);
					workDown = false;
					FetchData();
					workDown = true;
					clockCon.wait(shared_lock);
					shared_lock.unlock();
				}
				else {
					shared_lock.lock();
					workDown = false;
					StoreData();
					workDown = true;
					clockCon.wait(shared_lock);
					workDown = false;
					post_Store();
					workDown = true;
					clockCon.wait(shared_lock);
					shared_lock.unlock();
				}
			}
		}
		virtual void FetchData() {
			InstrucNum = 0;
		}
		virtual void pre_Execute() { //use for stall prePare
		}
		virtual void StoreData() {
			InstrucNum = 1;
		}
		virtual void post_Store() {
		}
		bool& workDown;
		int& clockControl;
		int& InstrucNum;
		boost::shared_mutex& mutex_clock;
		InstructionSturct tmpInstruct;
	};
	class IFworkHub :public workHub {
	public:
		bool branchCon;
		IFworkHub(int& _clockControl, int& _instrucNum, boost::shared_mutex& _clock,bool& _workDown) :workHub(_clockControl,_instrucNum,_clock, _workDown){}
		void FetchData() {
			/***************current Processing instruction************/
			if (pc >= instructionCount||pc<0) {
				InstrucNum = pc;
				return; //the pipeLine not working now
			}
			// the pipeLine has been stalled
			if (pipLineStallStatus[1]) { // ID has been stalled
				return;
			}
			// normal state
			InstrucNum = pc;
			tmpInstruct.IM_num = InstrucNum;
			tmpInstruct.instruction = nk_text[pc];
			string tmp;
			parse_Opcode(tmp, tmpInstruct.instruction);
			tmpInstruct.opcodeNum = nk_opcode[tmp];
			
		}
		void StoreData() {
			if (pipLineStallStatus[1]) { // ID has been stalled
				IF_ID_IR = tmpInstruct;
				return;
			}
			IF_ID_IR = tmpInstruct;
			/****************Update PC***************/
			IF_ID_NPC = pc + 1;
			pc = pc + 1;
		}
		void pre_Execute() {
			if (isBranchOpcode(EX_MEM_opCode) && EX_MEM_cond) {
				branchCon = true;
				pc = EX_MEM_ALUOutput;
				/// branch hazard stall- 2 cycle
				freeFlyingReg(ID_EX_IR.instruction,ID_EX_IR.opcodeNum);
				ID_EX_IR.IM_num = -1;
				IF_ID_IR.IM_num = -1;
				EX_MEM_cond = false;
			}
			else
				branchCon = false;
		}
	};
	class IDworkHub :public workHub {
	public:
		IDworkHub(int& _clockControl, int& _instrucNum, boost::shared_mutex& _clock, bool& _workDown) :workHub(_clockControl, _instrucNum, _clock, _workDown) { stalledItself = false; }
		void FetchData() {
			/***************current Processing instruction************/
			if (IF_ID_IR.IM_num >= instructionCount|| IF_ID_IR.IM_num<0) {
				InstrucNum = IF_ID_IR.IM_num;
				return; //the pipeLine not working now
			}
			// if itself is stalled
			if (pipLineStallStatus[1]) {
				stalledItself = (!getIDregisterResult(tmpInstruct.instruction, tmpInstruct.opcodeNum, tmpA, tmpB, tmpImm));
				return;
			}
			// normal state	
			tmpInstruct = IF_ID_IR;
			InstrucNum = tmpInstruct.IM_num;
			tmpNPC = IF_ID_NPC;
			stalledItself = (!getIDregisterResult(tmpInstruct.instruction,tmpInstruct.opcodeNum, tmpA, tmpB, tmpImm));
		}
		void StoreData() {
			if (InstrucNum >= instructionCount || InstrucNum<0) {
				InstrucNum = -1;
				ID_EX_IR.IM_num = InstrucNum;
				return; //the pipeLine has been stalled by jump
			}
			
			// if itself is stalled
			if (stalledItself) {
				return;
			}
			// normal state
			ID_EX_IR = tmpInstruct;
			ID_EX_NPC = tmpNPC;
			ID_EX_A = tmpA;
			ID_EX_B = tmpB;
			ID_EX_IMM = tmpImm;
		}
		void post_Store() {
			if (stalledItself) {
				ID_EX_IR.IM_num = -1;
			}
			pipLineStallStatus[1] = stalledItself;
		}
		int tmpNPC;
		int tmpA;
		int tmpB;
		int tmpImm;
		bool stalledItself;
	};
	class EXworkHub :public workHub {
	public:
		EXworkHub(int& _clockControl, int& _instrucNum, boost::shared_mutex& _clock, bool& _workDown) :workHub(_clockControl, _instrucNum, _clock, _workDown) {}
		void FetchData() {
			/***************current Processing instruction************/
			if (ID_EX_IR.IM_num >= instructionCount || ID_EX_IR.IM_num<0) {
				InstrucNum = -1;
				return; //the pipeLine not working now
			}
			// normal state	
			tmpInstruct = ID_EX_IR;
			InstrucNum = tmpInstruct.IM_num;
			getEXregisterResult(tmpInstruct.opcodeNum, tmpALUoutput, tmpB, tmpCond,ID_EX_A,ID_EX_B,ID_EX_IMM);
		}
		void StoreData() {
			if (InstrucNum >= instructionCount || InstrucNum<0) {
				InstrucNum = -1;
				EX_MEM_IR.IM_num = InstrucNum;
				return; //the pipeLine has been stalled now by jump
			}
				
			EX_MEM_IR = tmpInstruct;
			EX_MEM_ALUOutput = tmpALUoutput;
			EX_MEM_B = tmpB;
			EX_MEM_cond = tmpCond;
			EX_MEM_opCode = tmpInstruct.opcodeNum;
		}
		int tmpALUoutput;
		int tmpB;
		bool tmpCond;
		int tmpOpcode;
	};
	class MEMworkHub :public workHub {
	public:
		MEMworkHub(int& _clockControl, int& _instrucNum, boost::shared_mutex& _clock, bool& _workDown) :workHub(_clockControl, _instrucNum, _clock, _workDown) {}
		void FetchData() {
			/***************current Processing instruction************/
			if (EX_MEM_IR.IM_num >= instructionCount|| EX_MEM_IR.IM_num<0) {
				InstrucNum = -1;
				return; //the pipeLine not working now
			}
			tmpInstruct = EX_MEM_IR;
			InstrucNum = tmpInstruct.IM_num;
			getMEMregisterResult(tmpInstruct.opcodeNum, tmpALUoutput, tmpLMD,EX_MEM_ALUOutput,EX_MEM_B);
		}
		void StoreData() {
			if (InstrucNum >= instructionCount || InstrucNum<0) {
				InstrucNum = -1;
				MEM_WB_IR.IM_num = InstrucNum;
				return; //the pipeLine has been stalled
			}
				
			MEM_WB_IR = tmpInstruct;
			MEM_WB_ALUOutput = tmpALUoutput;
			MEM_WB_LMD = tmpLMD;
		}
	public :
		int tmpALUoutput;
		int tmpLMD;
	};
	class WBworkHub :public workHub {
	public:
		WBworkHub(int& _clockControl, int& _instrucNum, boost::shared_mutex& _clock, bool& _workDown) :workHub(_clockControl, _instrucNum, _clock, _workDown) {}
		void FetchData() {
			/***************current Processing instruction************/
			InstrucNum = MEM_WB_IR.IM_num;
			if (MEM_WB_IR.IM_num >= instructionCount) {
				return; //the pipeLine not working now
			}
			WriteBack2Register(InstrucNum, MEM_WB_ALUOutput, MEM_WB_LMD);
		}
		void StoreData() {
			if (InstrucNum >= instructionCount || InstrucNum<0) {
				InstrucNum = -1;
				return; //the pipeLine has been stalled now
			}
				
		}
	};
	/********************为了方便窗口展示的辅助函数****************/
	string getRecentInstructionString();
	string getRegisterStatus();
	string getWorkHubLabelString();
	string getTimebarString(int i);
	string getPcString();
};
