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
	bitset<32> extend;
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
            ReadData1 = Registers[RdReg1.to_ulong()];//reading data from registers
            ReadData2 = Registers[RdReg2.to_ulong()];
            if(WrtEnable.to_ulong() == 1)//if writeEnable=1 then write data into register
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
                if(ALUOP.to_ulong() == ADDU)//addition
                    ALUresult = oprand1.to_ulong() + oprand2.to_ulong();
                else if(ALUOP.to_ulong() == SUBU)//subtraction
                    ALUresult = oprand1.to_ulong() - oprand2.to_ulong();
                else if(ALUOP.to_ulong() == AND)//and
                    ALUresult = oprand1 & oprand2;
                else if (ALUOP.to_ulong() == OR)//or
                    ALUresult = oprand1|oprand2;
                else if (ALUOP.to_ulong() == NOR)//nor
                    ALUresult = ~(oprand1|oprand2);
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
				for (int j = 0; j < 4; j++) {
					bitset<8> temp = IMem[(ReadAddress.to_ulong()) + j];//reading 4 bytes of data from instruction memory
					tempInst = tempInst + temp.to_string();
				}
				Instruction =  bitset<32>(tempInst);
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
            if (readmem == 1) {//if readmem bit is 1
                string tempReadData;
                        for(int i = 0; i < 4; i++){//reading 4 bytes from data memory
                                bitset<8> tempData = DMem[Address.to_ulong()+i];
                                tempReadData = tempReadData + tempData.to_string();
                            }
                readdata = bitset<32>(tempReadData);
            }else if(writemem == 1){//if writemem bit is set to 1
                        int l = 31;
                        for(int i = 0; i < 4; i++){//writing 4 bytes of data in consecutive data memory locations
                                for(int j = 7; j >= 0; j--){//writing 8 bits
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

	bitset<32> pc = 0;//32 bit register
	bitset<32> curInstruction;
	bitset<32> halt (std::string("11111111111111111111111111111111"));
	bitset<32> jAddr;
	bitset<32> result;
	bitset<32> offset;
	bitset<32> readData;
	bitset<6> opcode /*bits 31-26*/, funct /*bits 5-0*/;
	bitset<5> rs /*bits 25-21*/, rt /*bits 20-16*/, rd /*bits 15-11*/, shtamt /*bits 10-6*/;
	bitset<3> ALUop;
    while (1)
	{
	    bool isEqual = false;//for beq instruction

        // Fetch
        curInstruction = myInsMem.ReadMemory(pc.to_ulong());

		// If current insturciton is "11111111111111111111111111111111", then break;
        if (curInstruction == halt)
			break;

		opcode = getSixBits(curInstruction, 31, 26);//get opcode from the current instruction

	// decode(Read RF)
		if (opcode == 0x00) {
			rs = getFiveBits(curInstruction, 25, 20);
			rt = getFiveBits(curInstruction, 20, 16);
			rd = getFiveBits(curInstruction, 15, 11);
			//shtamt = getFiveBits(curInstruction, 10, 6); since we are not using these bits
			funct = getSixBits(curInstruction, 5, 0);
			for (int i = 0; i < 3; i++) {//since we only need the 3 LSBs
				ALUop[i] = funct[i];
			}
			myRF.ReadWrite(rs, rt, rd, 0, 0);//calling function to read data from register files

		} else if (opcode == 0x02) { //J-Type instructions
			bitset<32> tempPC = pc.to_ulong()+4;
			for (int i = 2; i < 28; i++) { //getting the 26 bit address from the instruction
				jAddr[i] = curInstruction[i-2];
			}
			for (int i = 28; i < 32; i++) { //getting the four MSB from PC+4 address
				jAddr[i] = tempPC[i];
			}
			for (int i = 0; i < 2; i++) { //2 LSBs to zeros
				jAddr[i] = 0;
			}

		} else {//I-Type Instrcutions
			rs = getFiveBits(curInstruction, 25, 20);
			rt = getFiveBits(curInstruction, 20, 16);
			bitset<16> imm;
			for (int i = 0; i < 16; i++) {
				imm[i] = curInstruction[i];
			}
            offset = signExtend(imm);
			myRF.ReadWrite(rs, rt, rt, 0, 0);
		}


		// Execute
		if (opcode == 0x00) {//R-Type Instruction
			result = myALU.ALUOperation(ALUop, myRF.ReadData1, myRF.ReadData2);
		} else if (opcode == 0x09 || opcode == 0x23 || opcode == 0x2B) {//I-Type (addiu, lw, sw)
            ALUop = ADDU;
			result = myALU.ALUOperation(ALUop, myRF.ReadData1, offset);
		}else if(opcode == 04){//I-Type(beq)
		    if(myRF.ReadData1 == myRF.ReadData2)
                isEqual = true;
        }

		// Read/Write Mem
		if (opcode == 0x23)//loadword
			readData = myDataMem.MemoryAccess(result, myRF.ReadData2, 1, 0);
		else if (opcode == 0x2B)//storeword
			myDataMem.MemoryAccess(result, myRF.ReadData2, 0, 1);

		// Write back to RF
		if (opcode != 0x04 && opcode != 0x2b && opcode != 0x02) {//if instructions are not beq, jump, or sw
			if (opcode == 0x00)//R-Type instruction
				myRF.ReadWrite(rs, rt, rd, result, 1);
			else//remaining instructions(lw and addiu)
				myRF.ReadWrite(rs, rt, rt, readData, 1);
		}

		cout << "PC: " << pc.to_ulong() << endl;
		if (opcode == 0x02)//jump instruction
			pc = jAddr;
		else if (opcode == 0x04 && isEqual){//beq
            offset = offset << 2;//left shift by 2 because we need multiple of 4
			pc = pc.to_ulong() + 4 + offset.to_ulong();
		}
		else
			pc = pc.to_ulong() + 4;

        myRF.OutputRF(); // dump RF;
    }
        myDataMem.OutputDataMem(); // dump data mem

        return 0;

}
