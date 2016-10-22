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
/*
bitset<32> extend(bitset<16> Imm){
	int j;
	std::bitset<32> extend;
	for (j = 0; j < 16; j++){
		extend[j] = Imm[j];
		extend[j+16] = 0;
	}
	return extend;
}
*/
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
				cout << "WrtData: " << WrtData << endl;
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
                    ALUresult = oprand1 & oprand2;
                else if (ALUOP.to_ulong() == OR)
                    ALUresult = oprand1|oprand2;
                else if (ALUOP.to_ulong() == NOR)
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
				/*
				int j, k;
				int l = 31;
				for (j = 0; j < 4; j++){ //Read 4 bytes from IMem
					bitset<8> temp = IMem[(ReadAddress.to_ulong()) + j]; //Read byte from IMem
					for (k = 7; k > -1; k--){
						Instruction[l] = temp[k]; //Write byte to Instruction index
						l--;
					}
				}*/
				string tempInst;
				int j;
				for (j = 0; j < 4; j++) {
					bitset<8> temp = IMem[(ReadAddress.to_ulong()) + j];
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
		if (readmem == 1) {
			string tempReadData;
                	for(int i = 0; i < 4; i++){
                        	bitset<8> tempData = DMem[Address.to_ulong()+i];
                        	tempReadData = tempReadData + tempData.to_string();
                    	}
			readdata = bitset<32>(tempReadData);
			cout << "readdata: " << readdata << endl;
		}else if(writemem == 1){
                    int l = 31;
                    for(int i = 0; i < 4; i++){
                            for(int j = 7; j >= 0; j--){
                                DMem[Address.to_ulong()+i][j] = WriteData[l];
                                l--;
                            }
                    }
               }

/*
			  int j, k;
			  int l = 31;

              if(readmem == 1){
					for (j = 0; j < 4; j++){ //Read 4 bytes from Dmem
						bitset<8> temp = DMem[(Address.to_ulong()) + j]; //Read byte from DMem
						for (k = 7; k > -1; k--){
							readdata[l] = temp[k]; //Write byte to readdata index
							l--;
						}
					}
			  } else if (writemem == 1) {
			  }

*/

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

	//int pc = 0, nextAddr = 4;
	int i;
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
	int count = 0;
    while (count < 15)
	{
	    bool isEqual = false;
        // Fetch
        	curInstruction = myInsMem.ReadMemory(pc.to_ulong());
		cout << "CurIns: " << curInstruction << endl;
		// If current insturciton is "11111111111111111111111111111111", then break;
        	if (curInstruction == halt)
			break;

		opcode = getSixBits(curInstruction, 31, 26);
		//cout << "opcode: " << opcode << endl;

	// decode(Read RF)
		if (opcode == 0x00) {
			rs = getFiveBits(curInstruction, 25, 20);
			rt = getFiveBits(curInstruction, 20, 16);
			rd = getFiveBits(curInstruction, 15, 11);
			//shtamt = getFiveBits(curInstruction, 10, 6);
			funct = getSixBits(curInstruction, 5, 0);
			for (i = 0; i < 3; i++) {
				ALUop[i] = funct[i];
			}

			/* Debugging msgs
			cout << "rs: " << rs << endl;
			cout << "rt: " << rt << endl;
			cout << "rd: " << rd << endl;
			cout << "shtamt: " << shtamt << endl;
			cout << "funct: " << funct << endl;
			cout << "ALUop: " << ALUop.to_ulong() << endl;
			*/

			myRF.ReadWrite(rs, rt, rd, 0, 0);
			//cout << "Reg1: " << myRF.ReadData1 << endl; //rs
			//cout << "Reg2: " << myRF.ReadData2 << endl; //rt
			//cout << (int)halt.to_ulong() << endl;

			/* j-type test
			bitset<26> temp (std::string("10000000000000000000000001"));
			for (i = 2; i < 28; i++) {
				jAddr[i] = temp[i-2];
			}
			cout << "test j: " << jAddr << endl;
			for (i = 28; i < 32; i++) {
				jAddr[i] = pc[i];
			}
			jAddr.set(0,0);
			jAddr.set(1,0);
			*/

		} else if (opcode == 0x02) { //J-Type instructions
			bitset<32> tempPC = pc.to_ulong()+4;
			for (i = 2; i < 28; i++) { //getting the 26 bit address from the instruction
				jAddr[i] = curInstruction[i-2];
			}
			for (i = 28; i < 32; i++) { //getting the four MSB from PC+4 address
				jAddr[i] = tempPC[i];
			}
			for (i = 0; i < 2; i++) { //2 LSBs to zeros
				jAddr[i] = 0;
			}

		} else {//I-Type Instrcutions
			rs = getFiveBits(curInstruction, 25, 20);
			rt = getFiveBits(curInstruction, 20, 16);
			bitset<16> imm;
			for (i = 0; i < 16; i++) {
				imm[i] = curInstruction[i];
			}

            offset = signExtend(imm);

			/*
			cout << "lw base: " << rs << endl;
			cout << "lw reg : " << rt << endl;
			cout << "offset : " << offset << endl;
			*/

			myRF.ReadWrite(rs, rt, rt, 0, 0);
		}

		// Execute
		if (opcode == 0x00) {//R-Type Instruction
			result = myALU.ALUOperation(ALUop, myRF.ReadData1, myRF.ReadData2);
			//cout << "result: " << result << endl << "isEqual:  " << isEqual << endl;
		} else if (opcode == 0x09 || opcode == 0x23 || opcode == 0x2B) {//I-Type (addiu, lw, sw)
            ALUop = ADDU;
			result = myALU.ALUOperation(ALUop, myRF.ReadData1, offset);
			//cout << "lw result: " << result << endl;

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
		if (opcode != 0x04 && opcode != 0x2b && opcode != 0x02) {
			if (opcode == 0x00)
				myRF.ReadWrite(rs, rt, rd, result, 1);
			else
				myRF.ReadWrite(rs, rt, rt, readData, 1);
		}

		//cout << "jAddr: " << jAddr << endl;
		cout << "PC: " << pc.to_ulong() << endl;
		if (opcode == 0x02)
			pc = jAddr;
		else if (opcode == 0x04 && isEqual){
            offset = offset << 2;//left shift by 2 because we need multiple of 4
			pc = pc.to_ulong() + 4 + offset.to_ulong(); //TODO: change to branch offset
		}
		else
			pc = pc.to_ulong() + 4;//nextAddr.to_ulong();

        myRF.OutputRF(); // dump RF;
        count++;
    }
        myDataMem.OutputDataMem(); // dump data mem

        return 0;

}
