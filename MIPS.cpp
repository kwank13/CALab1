#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7
#define MemSize 65536 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

bitset<6> getSixBits(bitset<32> Ins, int start, int end) {
	int i, j = 5;
	bitset<6> temp;
	for (i = start; i >= end; i--)
	{
		temp[j] = Ins[i];
		j--;
	}
	return temp;
}

bitset<5> getFiveBits(bitset<32> Ins, int start, int end) {
	int i, j = 4;
	bitset<5> temp;
	for (i = start; i >= end; i--)
	{
		temp[j] = Ins[i];
		j--;
	}
	return temp;
}

bitset<32> signExtend(bitset<16> Imm){
	int j;
	std::bitset<32> extend;
	for (j = 0; j < 16; j++){
		extend[j] = Imm[j];
		extend[j+16] = Imm[15];
	}
	return extend;
}

class RF
{
    public:
        bitset<32> ReadData1, ReadData2;
     	RF()
    	{
          Registers.resize(32);
          Registers[0] = bitset<32> (0);
        }

        void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
        {
            // implement the funciton by you.
            ReadData1 = Registers[RdReg1.to_ulong()];
            ReadData2 = Registers[RdReg2.to_ulong()];
            if(WrtEnable.to_ulong() == 1)
            {
                Registers[WrtReg.to_ulong()] = WrtData;
            }
         }

	void OutputRF()
             {
               ofstream rfout;
                  rfout.open("RFresult.txt",std::ios_base::app);
                  if (rfout.is_open())
                  {
                    rfout<<"A state of RF:"<<endl;
                  for (int j = 0; j<32; j++)
                      {
                        rfout << Registers[j]<<endl;
                      }

                  }
                  else cout<<"Unable to open file";
                  rfout.close();

               }
	private:
            vector<bitset<32> >Registers;

};

class ALU
{
      public:
             bitset<32> ALUresult;
             bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
             {
                 // implement the ALU operations by you.
                if(ALUOP.to_ulong() == ADDU)
                    ALUresult = oprand1.to_ulong() + oprand2.to_ulong();
                else if(ALUOP.to_ulong() == SUBU)
                    ALUresult = oprand1.to_ulong() - oprand2.to_ulong();
                else if(ALUOP.to_ulong() == AND)
                    ALUresult = oprand1.to_ulong()  & oprand2.to_ulong() ;
                else if (ALUOP.to_ulong() == OR)
                    ALUresult = oprand1|oprand2;
                else if (ALUOP.to_ulong() == NOR)
                    ALUresult = !(oprand1|oprand2);
                return ALUresult;
            }
};

class INSMem
{
      public:
          bitset<32> Instruction;
          INSMem()
          {       IMem.resize(MemSize);
                  ifstream imem;
                  string line;
                  int i=0;
                  imem.open("imem.txt");
                  if (imem.is_open())
                  {
                  while (getline(imem,line))
                     {
                        IMem[i] = bitset<8>(line);
                        i++;
                     }

                  }
                  else cout<<"Unable to open file";
                  imem.close();

                  }

          bitset<32> ReadMemory (bitset<32> ReadAddress) //0x00000000 0x00000004 0x00000008 ...
          {
               // implement by you. (Read the byte at the ReadAddress and the following three byte).
				string tempInst;
				for (int j = 0; j < 4; j++){ //Read 4 bytes from IMem
					bitset<8> temp = IMem[(ReadAddress.to_ulong()) + j]; //Read byte from IMem
					tempInst = tempInst + temp.to_string();
				}
				Instruction = tempInst;
       			return Instruction;
          }

      private:
           vector<bitset<8> > IMem;

};

class DataMem
{
      public:
          bitset<32> readdata;
          DataMem()
          {
             DMem.resize(MemSize);
             ifstream dmem;
                  string line;
                  int i=0;
                  dmem.open("dmem.txt");
                  if (dmem.is_open())
                  {
                  while (getline(dmem,line))
                       {
                        DMem[i] = bitset<8>(line);
                        i++;
                       }
                  }
                  else cout<<"Unable to open file";
                  dmem.close();

          }
          bitset<32> MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem)
          {

               // implement by you.
               if(readmem.to_ulong() == 1){
                    string tempReadData;
                    for(int i = 0; i < 4; i++){
                        bitset<8> tempData = DMem[Address.to_ulong()+i];
                        tempReadData = tempReadData+tempData.to_string;
                    }
                    readData = tempReadData;
               }
               else if(writemem.to_ulong() == 1){
                    int l = 31;
                    for(int i = 0; i < 4; i++){
                            for(int j = 7; j >= 0; j--){
                                DMem[Address.to_ulong()+i][j] = WriteData[l];
                                l--;
                            }
                    }
               }
               return readdata;
          }

          void OutputDataMem()
          {
               ofstream dmemout;
                  dmemout.open("dmemresult.txt");
                  if (dmemout.is_open())
                  {
                  for (int j = 0; j< 1000; j++)
                       {
                        dmemout << DMem[j]<<endl;
                       }

                  }
                  else cout<<"Unable to open file";
                  dmemout.close();

               }

      private:
           vector<bitset<8> > DMem;

};



int main()
{
    RF myRF;
    ALU myALU;
    INSMem myInsMem;
    DataMem myDataMem;

	int pc = 0, nextAddr = 4;
	bitset<32> curInstruction;
	bitset<32> halt (std::string("11111111111111111111111111111111"));
	bitset<26> jAddr;
	bitset<16> imm;
	bitset<6> opcode /*bits 31-26*/, funct /*bits 0-5*/;
	bitset<5> rs /*bits 25-21*/, rt /*bits 20-16*/, rd /*bits 15-11*/, shtamt /*bits 10-6*/;
    while (1)
	{
        // Fetch
        curInstruction = myInsMem.ReadMemory(pc);
		cout << "CurIns: " << curInstruction << endl;
		// If current insturciton is "11111111111111111111111111111111", then break;
        if (curInstruction == halt)
			break;

		opcode = getSixBits(curInstruction, 31, 26);
		cout << "opcode: " << opcode << endl;

		// decode(Read RF)
		if (opcode == 0x00) {
			rs = getFiveBits(curInstruction, 25, 20);
			rt = getFiveBits(curInstruction, 20, 16);
			rd = getFiveBits(curInstruction, 15, 11);
			shtamt = getFiveBits(curInstruction, 10, 6);
			funct = getSixBits(curInstruction, 5, 0);
			
			/* Status msgs
			cout << "rs: " << rs << endl;
			cout << "rt: " << rt << endl;
			cout << "rd: " << rd << endl;
			cout << "shtamt: " << shtamt << endl;
			cout << "funct: " << funct << endl;
			*/

			myRF.ReadWrite(rs, rt, rd, curInstruction, 0);
			cout << "Reg1: " << myRF.ReadData1 << endl;
			cout << "Reg2: " << myRF.ReadData2 << endl;

		} else if (opcode == 0x02) {
		} else {
		}

		// Execute

		// Read/Write Mem

		// Write back to RF

		cout << "PC: " << pc << endl;
		pc = pc + nextAddr;
		if (nextAddr != 4)
			nextAddr = 4;
        myRF.OutputRF(); // dump RF;
    }
        myDataMem.OutputDataMem(); // dump data mem

        return 0;

}
