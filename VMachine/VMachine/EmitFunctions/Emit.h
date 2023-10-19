//	VMachine
//	Prototypes for functions to emit host instructions
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

Dword EmitMOV_Rb_Ib(std::vector <Byte> & hostCode, Dword Rb, Dword Ib);

Dword EmitMOV_Rw_Iw(std::vector <Byte> & hostCode, Dword Rw, Dword Iw);

Dword EmitMOV_Rd_Id(std::vector <Byte> & hostCode, Dword Rd, Dword Id);

void EmitMOV_Gb_Rb(std::vector <Byte> & hostCode, Dword Gb, Dword Rb);

void EmitMOV_Gw_Rw(std::vector <Byte> & hostCode, Dword Gw, Dword Rw);

void EmitMOV_Gd_Rd(std::vector <Byte> & hostCode, Dword Gd, Dword Rd);

void EmitMOV_Gb_Mb(std::vector <Byte> & hostCode, Dword Gb, Dword base, Dword index,
				   Dword shift, Dword disp);

void EmitMOV_Gw_Mw(std::vector <Byte> & hostCode, Dword Gw, Dword base, Dword index,
				   Dword shift, Dword disp);

void EmitMOV_Gd_Md(std::vector <Byte> & hostCode, Dword Gd, Dword base, Dword index,
				   Dword shift, Dword disp);

void EmitMOV_Mb_Gb(std::vector <Byte> & hostCode, Dword base, Dword index, Dword shift,
				   Dword disp, Dword Gb);

void EmitMOV_Mw_Gw(std::vector <Byte> & hostCode, Dword base, Dword index, Dword shift,
				   Dword disp, Dword Gw);

void EmitMOV_Md_Gd(std::vector <Byte> & hostCode, Dword base, Dword index, Dword shift,
				   Dword disp, Dword Gd);

Dword EmitMOV_Md_Id(std::vector <Byte> & hostCode, Dword base, Dword index, Dword shift,
					Dword disp, Dword Id);

void EmitCMOV_Gd_Rd(std::vector <Byte> & hostCode, Dword cond, Dword Gd, Dword Rd);

void EmitCMOV_Gd_Md(std::vector <Byte> & hostCode, Dword cond, Dword Gd, Dword base,
					Dword index, Dword shift, Dword disp);

void EmitMOVZX_Gw_Rb(std::vector <Byte> & hostCode, Dword Gw, Dword Rb);

void EmitMOVZX_Gd_Rb(std::vector <Byte> & hostCode, Dword Gd, Dword Rb);

void EmitMOVZX_Gd_Rw(std::vector <Byte> & hostCode, Dword Gd, Dword Rw);

void EmitMOVZX_Gw_Mb(	std::vector <Byte> & hostCode, Dword Gw, Dword base, Dword index,
						Dword shift, Dword disp);

void EmitMOVZX_Gd_Mb(	std::vector <Byte> & hostCode, Dword Gd, Dword base, Dword index,
						Dword shift, Dword disp);

void EmitMOVZX_Gd_Mw(	std::vector <Byte> & hostCode, Dword Gd, Dword base, Dword index,
						Dword shift, Dword disp);

void EmitMOVSX_Gw_Rb(std::vector <Byte> & hostCode, Dword Gw, Dword Rb);

void EmitMOVSX_Gd_Rb(std::vector <Byte> & hostCode, Dword Gd, Dword Rb);

void EmitMOVSX_Gd_Rw(std::vector <Byte> & hostCode, Dword Gd, Dword Rw);

void EmitMOVSX_Gw_Mb(	std::vector <Byte> & hostCode, Dword Gw, Dword base, Dword index,
						Dword shift, Dword disp);

void EmitMOVSX_Gd_Mb(	std::vector <Byte> & hostCode, Dword Gd, Dword base, Dword index,
						Dword shift, Dword disp);

void EmitMOVSX_Gd_Mw(	std::vector <Byte> & hostCode, Dword Gd, Dword base, Dword index,
						Dword shift, Dword disp);

Dword EmitPUSH_Id(std::vector <Byte> & hostCode, Dword Id);

void EmitPUSH_Rd(std::vector <Byte> & hostCode, Dword Rd);

void EmitPUSH_Md(	std::vector <Byte> & hostCode, Dword base, Dword index, Dword shift,
					Dword disp);

void EmitPOP_Rd(std::vector <Byte> & hostCode, Dword Rd);

void EmitPOP_Md(std::vector <Byte> & hostCode, Dword base, Dword index, Dword shift,
				Dword disp);

void EmitPUSHF(std::vector <Byte> & hostCode);

void EmitPOPF(std::vector <Byte> & hostCode);

void EmitXCHG_Rb_Gb(std::vector <Byte> & hostCode, Dword Rb, Dword Gb);

void EmitXCHG_Rd_Gd(std::vector <Byte> & hostCode, Dword Rd, Dword Gd);

Dword EmitALU_Rb_Ib(std::vector <Byte> & hostCode, Dword aluFunc, Dword Rw, Dword Iw);

Dword EmitALU_Rw_Iw(std::vector <Byte> & hostCode, Dword aluFunc, Dword Rw, Dword Iw);

Dword EmitALU_Rd_Id(std::vector <Byte> & hostCode, Dword aluFunc, Dword Rd, Dword Id);

void EmitALU_Gb_Rb(std::vector <Byte> & hostCode, Dword aluFunc, Dword Gb, Dword Rb);

void EmitALU_Gw_Rw(std::vector <Byte> & hostCode, Dword aluFunc, Dword Gw, Dword Rw);

void EmitALU_Gd_Rd(std::vector <Byte> & hostCode, Dword aluFunc, Dword Gd, Dword Rd);

Dword EmitALU_Mb_Ib(std::vector <Byte> & hostCode, Dword aluFunc, Dword base, Dword index,
					Dword shift, Dword disp, Dword Ib);

Dword EmitALU_Mw_Iw(std::vector <Byte> & hostCode, Dword aluFunc, Dword base, Dword index,
					Dword shift, Dword disp, Dword Iw);

Dword EmitALU_Md_Id(std::vector <Byte> & hostCode, Dword aluFunc, Dword base, Dword index,
					Dword shift, Dword disp, Dword Id);

void EmitALU_Mb_Gb(std::vector <Byte> & hostCode, Dword aluFunc, Dword base, Dword index,
				   Dword shift, Dword disp, Dword Gb);

void EmitALU_Mw_Gw(std::vector <Byte> & hostCode, Dword aluFunc, Dword base, Dword index,
				   Dword shift, Dword disp, Dword Gw);

void EmitALU_Md_Gd(std::vector <Byte> & hostCode, Dword aluFunc, Dword base, Dword index,
				   Dword shift, Dword disp, Dword Gd);

void EmitALU_Gb_Mb(std::vector <Byte> & hostCode, Dword aluFunc, Dword Gb, Dword base,
				   Dword index, Dword shift, Dword disp);

void EmitALU_Gw_Mw(std::vector <Byte> & hostCode, Dword aluFunc, Dword Gw, Dword base,
				   Dword index, Dword shift, Dword disp);

void EmitALU_Gd_Md(std::vector <Byte> & hostCode, Dword aluFunc, Dword Gd, Dword base,
				   Dword index, Dword shift, Dword disp);

void EmitUALU_Rb(std::vector <Byte> & hostCode, Dword ualuFunc, Dword Rb);

void EmitUALU_Rw(std::vector <Byte> & hostCode, Dword ualuFunc, Dword Rw);

void EmitUALU_Rd(std::vector <Byte> & hostCode, Dword ualuFunc, Dword Rd);

void EmitUALU_Mb(	std::vector <Byte> & hostCode, Dword ualuFunc, Dword base, Dword index,
					Dword shift, Dword disp);

void EmitUALU_Mw(	std::vector <Byte> & hostCode, Dword ualuFunc, Dword base, Dword index,
					Dword shift, Dword disp);

void EmitUALU_Md(	std::vector <Byte> & hostCode, Dword ualuFunc, Dword base, Dword index,
					Dword shift, Dword disp);

Dword EmitSHIFT_Rb_Ib(std::vector <Byte> & hostCode, Dword shiftFunc, Dword Rb, Dword Ib);

void EmitSHIFT_Rb_CL(std::vector <Byte> & hostCode, Dword shiftFunc, Dword Rb);

Dword EmitSHIFT_Rw_Ib(std::vector <Byte> & hostCode, Dword shiftFunc, Dword Rw, Dword Ib);

void EmitSHIFT_Rw_CL(std::vector <Byte> & hostCode, Dword shiftFunc, Dword Rw);

Dword EmitSHIFT_Rd_Ib(std::vector <Byte> & hostCode, Dword shiftFunc, Dword Rd, Dword Ib);

void EmitSHIFT_Rd_CL(std::vector <Byte> & hostCode, Dword shiftFunc, Dword Rd);

Dword EmitSHIFT_Mb_Ib(	std::vector <Byte> & hostCode, Dword shiftFunc, Dword base, Dword index,
						Dword shift, Dword disp, Dword Ib);

void EmitSHIFT_Mb_CL(	std::vector <Byte> & hostCode, Dword shiftFunc, Dword base, Dword index,
						Dword shift, Dword disp);

Dword EmitSHIFT_Mw_Ib(	std::vector <Byte> & hostCode, Dword shiftFunc, Dword base, Dword index,
						Dword shift, Dword disp, Dword Ib);

void EmitSHIFT_Mw_CL(	std::vector <Byte> & hostCode, Dword shiftFunc, Dword base, Dword index,
						Dword shift, Dword disp);

Dword EmitSHIFT_Md_Ib(	std::vector <Byte> & hostCode, Dword shiftFunc, Dword base, Dword index,
						Dword shift, Dword disp, Dword Ib);

void EmitSHIFT_Md_CL(	std::vector <Byte> & hostCode, Dword shiftFunc, Dword base, Dword index,
						Dword shift, Dword disp);

void EmitSHIFTD_Rw_Gw_CL(std::vector <Byte> & hostCode, Dword shiftFunc, Dword Rw, Dword Gw);

Dword EmitSHIFTD_Rw_Gw_Ib(	std::vector <Byte> & hostCode, Dword shiftFunc, Dword Rw, Dword Gw,
							Dword Ib);

void EmitSHIFTD_Rd_Gd_CL(std::vector <Byte> & hostCode, Dword shiftFunc, Dword Rd, Dword Gd);

Dword EmitSHIFTD_Rd_Gd_Ib(	std::vector <Byte> & hostCode, Dword shiftFunc, Dword Rd, Dword Gd,
							Dword Ib);

void EmitMUL_Rb(std::vector <Byte> & hostCode, Dword Rb);

void EmitMUL_Rw(std::vector <Byte> & hostCode, Dword Rw);

void EmitMUL_Rd(std::vector <Byte> & hostCode, Dword Rd);

void EmitIMUL_Rb(std::vector <Byte> & hostCode, Dword Rb);

void EmitIMUL_Rw(std::vector <Byte> & hostCode, Dword Rw);

void EmitIMUL_Rd(std::vector <Byte> & hostCode, Dword Rd);

void EmitDIV_Rb(std::vector <Byte> & hostCode, Dword Rb);

void EmitDIV_Rw(std::vector <Byte> & hostCode, Dword Rw);

void EmitDIV_Rd(std::vector <Byte> & hostCode, Dword Rd);

void EmitDIV_Mb(std::vector <Byte> & hostCode, Dword base, Dword index, Dword shift,
				Dword disp);

void EmitIDIV_Rb(std::vector <Byte> & hostCode, Dword Rb);

void EmitIDIV_Rw(std::vector <Byte> & hostCode, Dword Rw);

void EmitIDIV_Rd(std::vector <Byte> & hostCode, Dword Rd);

void EmitIDIV_Mb(	std::vector <Byte> & hostCode, Dword base, Dword index, Dword shift,
					Dword disp);

Dword EmitBIT_Rw_Ib(std::vector <Byte> & hostCode, Dword bitFunc, Dword Rw, Dword Ib);

Dword EmitBIT_Rd_Ib(std::vector <Byte> & hostCode, Dword bitFunc, Dword Rd, Dword Ib);

void EmitBIT_Rw_Gw(std::vector <Byte> & hostCode, Dword bitFunc, Dword Rw, Dword Gw);

void EmitBIT_Rd_Gd(std::vector <Byte> & hostCode, Dword bitFunc, Dword Rd, Dword Gd);

void EmitBSWAP_Rd(std::vector <Byte> & hostCode, Dword Rd);

void EmitBITSCAN_Gw_Rw(std::vector <Byte> & hostCode, Dword bitScanFunc, Dword Gw, Dword Rw);

void EmitBITSCAN_Gw_Mw(std::vector <Byte> & hostCode, Dword bitScanFunc, Dword Gw,
					   Dword base, Dword index, Dword shift, Dword disp);

void EmitBITSCAN_Gd_Rd(std::vector <Byte> & hostCode, Dword bitScanFunc, Dword Gd, Dword Rd);

void EmitBITSCAN_Gd_Md(std::vector <Byte> & hostCode, Dword bitScanFunc, Dword Gd,
					   Dword base, Dword index, Dword shift, Dword disp);

void EmitASAdjust(std::vector <Byte> & hostCode, Dword asAdjustFunc);

void EmitMDAdjust(std::vector <Byte> & hostCode, Dword mdAdjustFunc, Dword Ib);

void EmitLEA_Gd_M(	std::vector <Byte> & hostCode, Dword Gd, Dword base, Dword index,
					Dword shift, Dword disp);

void EmitWAIT(std::vector <Byte> & hostCode);

void EmitRegisterFPU(std::vector <Byte> & hostCode, Dword fpuFunc);

void EmitMemoryFPU(	std::vector <Byte> & hostCode, Dword fpuFunc, Dword base, Dword index,
					Dword shift, Dword disp);

void EmitCALL(std::vector <Byte> & hostCode);

void EmitJMP(std::vector <Byte> & hostCode);

void EmitJCond(std::vector <Byte> & hostCode, Dword cond, SignedByte displacement);

void EmitSAHF(std::vector <Byte> & hostCode);

void EmitLAHF(std::vector <Byte> & hostCode);
