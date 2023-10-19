//	VMachine
//	Functions to emit host instructions
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../x86Constants.h"
#include "Emit.h"

static Dword EmitMemoryModRM(	std::vector <Byte> & hostCode, Dword Gd, Dword base,
								Dword index, Dword shift, Dword disp)
{
	assert(	Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX ||
			Gd == REG_ESI || Gd == REG_EDI ||
			(Gd < 0x08));	//Gd may encode an opcode extension
	assert(	base == REG_EAX || base == REG_ECX || base == REG_EDX || base == REG_EBX ||
			base == REG_ESI || base == REG_EDI || base == REG_EBP);
	assert( index == REG_EAX || index == REG_ECX || index == REG_EDX || index == REG_EBX ||
			index == REG_ESI || index == REG_EDI || index == REG_NONE);
	
	if(index == REG_NONE)
	{
		assert(shift == 0);

		if(disp == 0 && base != REG_EBP)
		{
			hostCode.push_back(0x00 | static_cast<Byte>(Gd << 3) | static_cast<Byte>(base));

			return 1;
		}
	
		if(static_cast<SignedByte>(disp) == disp)
		{
			hostCode.push_back(0x40 | static_cast<Byte>(Gd << 3) | static_cast<Byte>(base));
			hostCode.push_back(static_cast<Byte>(disp));

			return 2;
		}
		
		hostCode.push_back(0x80 | static_cast<Byte>(Gd << 3) | static_cast<Byte>(base));
		hostCode.push_back(static_cast<Byte>(disp));
		hostCode.push_back(static_cast<Byte>(disp >> 8));
		hostCode.push_back(static_cast<Byte>(disp >> 16));
		hostCode.push_back(static_cast<Byte>(disp >> 24));

		return 5;
	}
	else
	{
		assert(base != REG_EBP);
		assert(shift < 4);

		if(disp == 0)
		{
			hostCode.push_back(0x00 | static_cast<Byte>(Gd << 3) | 0x04);
			hostCode.push_back(	static_cast<Byte>(shift << 6) | static_cast<Byte>(index << 3) |
								static_cast<Byte>(base));

			return 2;
		}
		
		if(static_cast<SignedByte>(disp) == disp)
		{
			hostCode.push_back(0x40 | static_cast<Byte>(Gd << 3) | 0x04);
			hostCode.push_back(	static_cast<Byte>(shift << 6) | static_cast<Byte>(index << 3) |
								static_cast<Byte>(base));
			hostCode.push_back(static_cast<Byte>(disp));

			return 3;
		}

		hostCode.push_back(0x80 | static_cast<Byte>(Gd << 3) | 0x04);
		hostCode.push_back(	static_cast<Byte>(shift << 6) | static_cast<Byte>(index << 3) |
							static_cast<Byte>(base));
		hostCode.push_back(static_cast<Byte>(disp));
		hostCode.push_back(static_cast<Byte>(disp >> 8));
		hostCode.push_back(static_cast<Byte>(disp >> 16));
		hostCode.push_back(static_cast<Byte>(disp >> 24));
		
		return 6;
	}
}

Dword EmitMOV_Rb_Ib(std::vector <Byte> & hostCode, Dword Rb, Dword Ib)
{
	assert(	Rb == REG_AL || Rb == REG_CL || Rb == REG_DL || Rb == REG_BL ||
			Rb == REG_AH || Rb == REG_CH || Rb == REG_DH || Rb == REG_BH);

	hostCode.push_back(0xb0 + static_cast<Byte>(Rb - REG_AL));
	hostCode.push_back(static_cast<Byte>(Ib));

	return 1;
}

Dword EmitMOV_Rw_Iw(std::vector <Byte> & hostCode, Dword Rw, Dword Iw)
{
	assert(Rw == REG_EAX || Rw == REG_ECX || Rw == REG_EDX || Rw == REG_EBX || Rw == REG_ESI || Rw == REG_EDI);

	hostCode.push_back(0x66);
	hostCode.push_back(0xb8 + static_cast<Byte>(Rw));
	hostCode.push_back(static_cast<Byte>(Iw));
	hostCode.push_back(static_cast<Byte>(Iw >> 8));

	return 2;
}

Dword EmitMOV_Rd_Id(std::vector <Byte> & hostCode, Dword Rd, Dword Id)
{
	assert(Rd == REG_EAX || Rd == REG_ECX || Rd == REG_EDX || Rd == REG_EBX || Rd == REG_ESI || Rd == REG_EDI);

	hostCode.push_back(0xb8 + static_cast<Byte>(Rd));
	hostCode.push_back(static_cast<Byte>(Id));
	hostCode.push_back(static_cast<Byte>(Id >> 8));
	hostCode.push_back(static_cast<Byte>(Id >> 16));
	hostCode.push_back(static_cast<Byte>(Id >> 24));

	return 1;
}

void EmitMOV_Gb_Rb(std::vector <Byte> & hostCode, Dword Gb, Dword Rb)
{
	assert(	Gb == REG_AL || Gb == REG_CL || Gb == REG_DL || Gb == REG_BL ||
			Gb == REG_AH || Gb == REG_CH || Gb == REG_DH || Gb == REG_BH);
	assert(	Rb == REG_AL || Rb == REG_CL || Rb == REG_DL || Rb == REG_BL ||
			Rb == REG_AH || Rb == REG_CH || Rb == REG_DH || Rb == REG_BH);

	if(Gb == Rb)
		return;

	hostCode.push_back(0x8a);
	hostCode.push_back(	0xc0 | static_cast<Byte>((Gb - REG_AL) << 3) |
						static_cast<Byte>(Rb - REG_AL));
}

void EmitMOV_Gw_Rw(std::vector <Byte> & hostCode, Dword Gw, Dword Rw)
{
	assert(Gw == REG_EAX || Gw == REG_ECX || Gw == REG_EDX || Gw == REG_EBX || Gw == REG_ESI || Gw == REG_EDI);
	assert(Rw == REG_EAX || Rw == REG_ECX || Rw == REG_EDX || Rw == REG_EBX || Rw == REG_ESI || Rw == REG_EDI);

	if(Gw == Rw)
		return;

	hostCode.push_back(0x66);
	hostCode.push_back(0x8b);
	hostCode.push_back(0xc0 | static_cast<Byte>(Gw << 3) | static_cast<Byte>(Rw));
}

void EmitMOV_Gd_Rd(std::vector <Byte> & hostCode, Dword Gd, Dword Rd)
{
	assert(Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX || Gd == REG_ESI || Gd == REG_EDI);
	assert(Rd == REG_EAX || Rd == REG_ECX || Rd == REG_EDX || Rd == REG_EBX || Rd == REG_ESI || Rd == REG_EDI);

	if(Gd == Rd)
		return;

	hostCode.push_back(0x8b);
	hostCode.push_back(0xc0 | static_cast<Byte>(Gd << 3) | static_cast<Byte>(Rd));
}

void EmitMOV_Gb_Mb(std::vector <Byte> & hostCode, Dword Gb, Dword base, Dword index,
				   Dword shift, Dword disp)
{
	assert(	Gb == REG_AL || Gb == REG_CL || Gb == REG_DL || Gb == REG_BL ||
			Gb == REG_AH || Gb == REG_CH || Gb == REG_DH || Gb == REG_BH);

	hostCode.push_back(0x8a);
	EmitMemoryModRM(hostCode, Gb - REG_AL, base, index, shift, disp);
}

void EmitMOV_Gw_Mw(std::vector <Byte> & hostCode, Dword Gw, Dword base, Dword index,
				   Dword shift, Dword disp)
{
	assert(Gw == REG_EAX || Gw == REG_ECX || Gw == REG_EDX || Gw == REG_EBX || Gw == REG_ESI || Gw == REG_EDI);

	hostCode.push_back(0x66);
	hostCode.push_back(0x8b);
	EmitMemoryModRM(hostCode, Gw, base, index, shift, disp);
}

void EmitMOV_Gd_Md(std::vector <Byte> & hostCode, Dword Gd, Dword base, Dword index,
				   Dword shift, Dword disp)
{
	assert(Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX || Gd == REG_ESI || Gd == REG_EDI);

	hostCode.push_back(0x8b);
	EmitMemoryModRM(hostCode, Gd, base, index, shift, disp);
}

void EmitMOV_Mb_Gb(std::vector <Byte> & hostCode, Dword base, Dword index, Dword shift,
				   Dword disp, Dword Gb)
{
	assert(	Gb == REG_AL || Gb == REG_CL || Gb == REG_DL || Gb == REG_BL ||
			Gb == REG_AH || Gb == REG_CH || Gb == REG_DH || Gb == REG_BH);

	hostCode.push_back(0x88);
	EmitMemoryModRM(hostCode, Gb - REG_AL, base, index, shift, disp);
}

void EmitMOV_Mw_Gw(std::vector <Byte> & hostCode, Dword base, Dword index, Dword shift,
				   Dword disp, Dword Gw)
{
	assert(Gw == REG_EAX || Gw == REG_ECX || Gw == REG_EDX || Gw == REG_EBX || Gw == REG_ESI || Gw == REG_EDI);

	hostCode.push_back(0x66);
	hostCode.push_back(0x89);
	EmitMemoryModRM(hostCode, Gw, base, index, shift, disp);
}

void EmitMOV_Md_Gd(std::vector <Byte> & hostCode, Dword base, Dword index, Dword shift,
				   Dword disp, Dword Gd)
{
	assert(Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX || Gd == REG_ESI || Gd == REG_EDI);

	hostCode.push_back(0x89);
	EmitMemoryModRM(hostCode, Gd, base, index, shift, disp);
}

Dword EmitMOV_Md_Id(std::vector <Byte> & hostCode, Dword base, Dword index, Dword shift,
					Dword disp, Dword Id)
{
	hostCode.push_back(0xc7);
	Dword modRMLength = EmitMemoryModRM(hostCode, 0x00, base, index, shift, disp);
	hostCode.push_back(static_cast<Byte>(Id));
	hostCode.push_back(static_cast<Byte>(Id >> 8));
	hostCode.push_back(static_cast<Byte>(Id >> 16));
	hostCode.push_back(static_cast<Byte>(Id >> 24));

	return 1 + modRMLength;
}

void EmitCMOV_Gd_Rd(std::vector <Byte> & hostCode, Dword cond, Dword Gd, Dword Rd)
{
	assert(	cond < 0x10);
	assert(Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX || Gd == REG_ESI || Gd == REG_EDI);
	assert(Rd == REG_EAX || Rd == REG_ECX || Rd == REG_EDX || Rd == REG_EBX || Rd == REG_ESI || Rd == REG_EDI);

	if(Gd == Rd)
		return;

	hostCode.push_back(0x0f);
	hostCode.push_back(0x40 | static_cast<Byte>(cond));
	hostCode.push_back(0xc0 | static_cast<Byte>(Gd << 3) | static_cast<Byte>(Rd));
}

void EmitCMOV_Gd_Md(std::vector <Byte> & hostCode, Dword cond, Dword Gd, Dword base,
					Dword index, Dword shift, Dword disp)
{
	assert(	cond < 0x10);
	assert(Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX || Gd == REG_ESI || Gd == REG_EDI);

	hostCode.push_back(0x0f);
	hostCode.push_back(0x40 | static_cast<Byte>(cond));
	EmitMemoryModRM(hostCode, Gd, base, index, shift, disp);
}

void EmitMOVZX_Gw_Rb(std::vector <Byte> & hostCode, Dword Gw, Dword Rb)
{
	assert(Gw == REG_EAX || Gw == REG_ECX || Gw == REG_EDX || Gw == REG_EBX || Gw == REG_ESI || Gw == REG_EDI);
	assert(	Rb == REG_AL || Rb == REG_CL || Rb == REG_DL || Rb == REG_BL ||
			Rb == REG_AH || Rb == REG_CH || Rb == REG_DH || Rb == REG_BH);

	hostCode.push_back(0x66);
	hostCode.push_back(0x0f);
	hostCode.push_back(0xb6);
	hostCode.push_back(0xc0 | static_cast<Byte>(Gw << 3) | static_cast<Byte>(Rb - REG_AL));
}

void EmitMOVZX_Gd_Rb(std::vector <Byte> & hostCode, Dword Gd, Dword Rb)
{
	assert(Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX || Gd == REG_ESI || Gd == REG_EDI);
	assert(	Rb == REG_AL || Rb == REG_CL || Rb == REG_DL || Rb == REG_BL ||
			Rb == REG_AH || Rb == REG_CH || Rb == REG_DH || Rb == REG_BH);

	hostCode.push_back(0x0f);
	hostCode.push_back(0xb6);
	hostCode.push_back(0xc0 | static_cast<Byte>(Gd << 3) | static_cast<Byte>(Rb - REG_AL));
}

void EmitMOVZX_Gd_Rw(std::vector <Byte> & hostCode, Dword Gd, Dword Rw)
{
	assert(Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX || Gd == REG_ESI || Gd == REG_EDI);
	assert(Rw == REG_EAX || Rw == REG_ECX || Rw == REG_EDX || Rw == REG_EBX || Rw == REG_ESI || Rw == REG_EDI);

	hostCode.push_back(0x0f);
	hostCode.push_back(0xb7);
	hostCode.push_back(0xc0 | static_cast<Byte>(Gd << 3) | static_cast<Byte>(Rw));
}

void EmitMOVZX_Gw_Mb(	std::vector <Byte> & hostCode, Dword Gw, Dword base, Dword index,
						Dword shift, Dword disp)
{
	assert(Gw == REG_EAX || Gw == REG_ECX || Gw == REG_EDX || Gw == REG_EBX || Gw == REG_ESI || Gw == REG_EDI);

	hostCode.push_back(0x66);
	hostCode.push_back(0x0f);
	hostCode.push_back(0xb6);
	EmitMemoryModRM(hostCode, Gw, base, index, shift, disp);
}

void EmitMOVZX_Gd_Mb(	std::vector <Byte> & hostCode, Dword Gd, Dword base, Dword index,
						Dword shift, Dword disp)
{
	assert(Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX || Gd == REG_ESI || Gd == REG_EDI);

	hostCode.push_back(0x0f);
	hostCode.push_back(0xb6);
	EmitMemoryModRM(hostCode, Gd, base, index, shift, disp);
}

void EmitMOVZX_Gd_Mw(	std::vector <Byte> & hostCode, Dword Gd, Dword base, Dword index,
						Dword shift, Dword disp)
{
	assert(Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX || Gd == REG_ESI || Gd == REG_EDI);

	hostCode.push_back(0x0f);
	hostCode.push_back(0xb7);
	EmitMemoryModRM(hostCode, Gd, base, index, shift, disp);
}

void EmitMOVSX_Gw_Rb(std::vector <Byte> & hostCode, Dword Gw, Dword Rb)
{
	assert(Gw == REG_EAX || Gw == REG_ECX || Gw == REG_EDX || Gw == REG_EBX || Gw == REG_ESI || Gw == REG_EDI);
	assert(	Rb == REG_AL || Rb == REG_CL || Rb == REG_DL || Rb == REG_BL ||
			Rb == REG_AH || Rb == REG_CH || Rb == REG_DH || Rb == REG_BH);

	if(Gw == REG_EAX && Rb == REG_AL)
	{
		hostCode.push_back(0x66);
		hostCode.push_back(0x98);
	}
	else
	{
		hostCode.push_back(0x66);
		hostCode.push_back(0x0f);
		hostCode.push_back(0xbe);
		hostCode.push_back(0xc0 | static_cast<Byte>(Gw << 3) | static_cast<Byte>(Rb - REG_AL));
	}
}

void EmitMOVSX_Gd_Rb(std::vector <Byte> & hostCode, Dword Gd, Dword Rb)
{
	assert(Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX || Gd == REG_ESI || Gd == REG_EDI);
	assert(	Rb == REG_AL || Rb == REG_CL || Rb == REG_DL || Rb == REG_BL ||
			Rb == REG_AH || Rb == REG_CH || Rb == REG_DH || Rb == REG_BH);

	hostCode.push_back(0x0f);
	hostCode.push_back(0xbe);
	hostCode.push_back(0xc0 | static_cast<Byte>(Gd << 3) | static_cast<Byte>(Rb - REG_AL));
}

void EmitMOVSX_Gd_Rw(std::vector <Byte> & hostCode, Dword Gd, Dword Rw)
{
	assert(Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX || Gd == REG_ESI || Gd == REG_EDI);
	assert(Rw == REG_EAX || Rw == REG_ECX || Rw == REG_EDX || Rw == REG_EBX || Rw == REG_ESI || Rw == REG_EDI);

	if(Gd == REG_EAX && Rw == REG_EAX)
	{
		hostCode.push_back(0x98);
	}
	else
	{
		hostCode.push_back(0x0f);
		hostCode.push_back(0xbf);
		hostCode.push_back(0xc0 | static_cast<Byte>(Gd << 3) | static_cast<Byte>(Rw));
	}
}

void EmitMOVSX_Gw_Mb(	std::vector <Byte> & hostCode, Dword Gw, Dword base, Dword index,
						Dword shift, Dword disp)
{
	assert(Gw == REG_EAX || Gw == REG_ECX || Gw == REG_EDX || Gw == REG_EBX || Gw == REG_ESI || Gw == REG_EDI);

	hostCode.push_back(0x66);
	hostCode.push_back(0x0f);
	hostCode.push_back(0xbe);
	EmitMemoryModRM(hostCode, Gw, base, index, shift, disp);
}

void EmitMOVSX_Gd_Mb(	std::vector <Byte> & hostCode, Dword Gd, Dword base, Dword index,
						Dword shift, Dword disp)
{
	assert(Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX || Gd == REG_ESI || Gd == REG_EDI);

	hostCode.push_back(0x0f);
	hostCode.push_back(0xbe);
	EmitMemoryModRM(hostCode, Gd, base, index, shift, disp);
}

void EmitMOVSX_Gd_Mw(	std::vector <Byte> & hostCode, Dword Gd, Dword base, Dword index,
						Dword shift, Dword disp)
{
	assert(Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX || Gd == REG_ESI || Gd == REG_EDI);

	hostCode.push_back(0x0f);
	hostCode.push_back(0xbf);
	EmitMemoryModRM(hostCode, Gd, base, index, shift, disp);
}

Dword EmitPUSH_Id(std::vector <Byte> & hostCode, Dword Id)
{
	hostCode.push_back(0x68);
	hostCode.push_back(static_cast<Byte>(Id));
	hostCode.push_back(static_cast<Byte>(Id >> 8));
	hostCode.push_back(static_cast<Byte>(Id >> 16));
	hostCode.push_back(static_cast<Byte>(Id >> 24));

	return 1;
}

void EmitPUSH_Rd(std::vector <Byte> & hostCode, Dword Rd)
{
	assert(Rd == REG_EAX || Rd == REG_ECX || Rd == REG_EDX || Rd == REG_EBX || Rd == REG_ESI || Rd == REG_EDI);

	hostCode.push_back(0x50 + static_cast<Byte>(Rd));
}

void EmitPUSH_Md(	std::vector <Byte> & hostCode, Dword base, Dword index, Dword shift,
					Dword disp)
{
	hostCode.push_back(0xff);
	EmitMemoryModRM(hostCode, 0x06, base, index, shift, disp);
}

void EmitPOP_Rd(std::vector <Byte> & hostCode, Dword Rd)
{
	assert(Rd == REG_EAX || Rd == REG_ECX || Rd == REG_EDX || Rd == REG_EBX || Rd == REG_ESI || Rd == REG_EDI);

	hostCode.push_back(0x58 + static_cast<Byte>(Rd));
}

void EmitPOP_Md(std::vector <Byte> & hostCode, Dword base, Dword index, Dword shift,
				Dword disp)
{
	hostCode.push_back(0x8f);
	EmitMemoryModRM(hostCode, 0x00, base, index, shift, disp);
}

void EmitPUSHF(std::vector <Byte> & hostCode)
{
	hostCode.push_back(0x9c);
}

void EmitPOPF(std::vector <Byte> & hostCode)
{
	hostCode.push_back(0x9d);
}

void EmitXCHG_Rb_Gb(std::vector <Byte> & hostCode, Dword Rb, Dword Gb)
{
	assert(	Rb == REG_AL || Rb == REG_CL || Rb == REG_DL || Rb == REG_BL ||
			Rb == REG_AH || Rb == REG_CH || Rb == REG_DH || Rb == REG_BH);
	assert(	Gb == REG_AL || Gb == REG_CL || Gb == REG_DL || Gb == REG_BL ||
			Gb == REG_AH || Gb == REG_CH || Gb == REG_DH || Gb == REG_BH);

	if(Rb == Gb)
		return;

	hostCode.push_back(0x86);
	hostCode.push_back(	0xc0 | static_cast<Byte>((Gb - REG_AL) << 3) |
						static_cast<Byte>(Rb - REG_AL));
}

void EmitXCHG_Rd_Gd(std::vector <Byte> & hostCode, Dword Rd, Dword Gd)
{
	assert(Rd == REG_EAX || Rd == REG_ECX || Rd == REG_EDX || Rd == REG_EBX || Rd == REG_ESI || Rd == REG_EDI);
	assert(Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX || Gd == REG_ESI || Gd == REG_EDI);

	if(Rd == Gd)
		return;

	if(Rd == REG_EAX)
	{
		hostCode.push_back(0x90 | static_cast<Byte>(Gd));
	}
	else if(Gd == REG_EAX)
	{
		hostCode.push_back(0x90 | static_cast<Byte>(Rd));
	}
	else
	{
		hostCode.push_back(0x87);
		hostCode.push_back(0xc0 | static_cast<Byte>(Gd << 3) | static_cast<Byte>(Rd));
	}
}

Dword EmitALU_Rb_Ib(std::vector <Byte> & hostCode, Dword aluFunc, Dword Rb, Dword Ib)
{
	assert(	aluFunc == ALU_FUNC_ADD || aluFunc == ALU_FUNC_OR || aluFunc == ALU_FUNC_ADC ||
			aluFunc == ALU_FUNC_SBB || aluFunc == ALU_FUNC_AND || aluFunc == ALU_FUNC_SUB ||
			aluFunc == ALU_FUNC_XOR || aluFunc == ALU_FUNC_CMP || aluFunc == ALU_FUNC_TEST);
	assert(	Rb == REG_AL || Rb == REG_CL || Rb == REG_DL || Rb == REG_BL ||
			Rb == REG_AH || Rb == REG_CH || Rb == REG_DH || Rb == REG_BH);

	if(aluFunc == ALU_FUNC_TEST)
	{
		if(Rb == REG_AL)
		{
			hostCode.push_back(0xa8);
			hostCode.push_back(static_cast<Byte>(Ib));

			return 1;
		}
		else
		{
			hostCode.push_back(0xf6);
			hostCode.push_back(	0xc0 | static_cast<Byte>(0x00 << 3) |
								static_cast<Byte>(Rb - REG_AL));
			hostCode.push_back(static_cast<Byte>(Ib));

			return 2;
		}
	}
	else
	{
		if(Rb == REG_AL)
		{
			hostCode.push_back(static_cast<Byte>(aluFunc << 3) | 0x04);
			hostCode.push_back(static_cast<Byte>(Ib));

			return 1;
		}
		else
		{
			hostCode.push_back(0x80);
			hostCode.push_back(	0xc0 | static_cast<Byte>(aluFunc << 3) |
								static_cast<Byte>(Rb - REG_AL));
			hostCode.push_back(static_cast<Byte>(Ib));

			return 2;
		}
	}
}

Dword EmitALU_Rw_Iw(std::vector <Byte> & hostCode, Dword aluFunc, Dword Rw, Dword Iw)
{
	assert(	aluFunc == ALU_FUNC_ADD || aluFunc == ALU_FUNC_OR || aluFunc == ALU_FUNC_ADC ||
			aluFunc == ALU_FUNC_SBB || aluFunc == ALU_FUNC_AND || aluFunc == ALU_FUNC_SUB ||
			aluFunc == ALU_FUNC_XOR || aluFunc == ALU_FUNC_CMP || aluFunc == ALU_FUNC_TEST);
	assert(Rw == REG_EAX || Rw == REG_ECX || Rw == REG_EDX || Rw == REG_EBX || Rw == REG_ESI || Rw == REG_EDI);

	if(aluFunc == ALU_FUNC_TEST)
	{
		if(Rw == REG_EAX)
		{
			hostCode.push_back(0x66);
			hostCode.push_back(0xa9);
			hostCode.push_back(static_cast<Byte>(Iw));
			hostCode.push_back(static_cast<Byte>(Iw >> 8));

			return 2;
		}
		else
		{
			hostCode.push_back(0x66);
			hostCode.push_back(0xf7);
			hostCode.push_back(0xc0 | static_cast<Byte>(0x00 << 3) | static_cast<Byte>(Rw));
			hostCode.push_back(static_cast<Byte>(Iw));
			hostCode.push_back(static_cast<Byte>(Iw >> 8));

			return 3;
		}
	}
	else
	{
		if(Rw == REG_EAX)
		{
			hostCode.push_back(0x66);
			hostCode.push_back(static_cast<Byte>(aluFunc << 3) | 0x05);
			hostCode.push_back(static_cast<Byte>(Iw));
			hostCode.push_back(static_cast<Byte>(Iw >> 8));

			return 2;
		}
		else
		{
			hostCode.push_back(0x66);
			hostCode.push_back(0x81);
			hostCode.push_back(0xc0 | static_cast<Byte>(aluFunc << 3) | static_cast<Byte>(Rw));
			hostCode.push_back(static_cast<Byte>(Iw));
			hostCode.push_back(static_cast<Byte>(Iw >> 8));

			return 3;
		}
	}
}

Dword EmitALU_Rd_Id(std::vector <Byte> & hostCode, Dword aluFunc, Dword Rd, Dword Id)
{
	assert(	aluFunc == ALU_FUNC_ADD || aluFunc == ALU_FUNC_OR || aluFunc == ALU_FUNC_ADC ||
			aluFunc == ALU_FUNC_SBB || aluFunc == ALU_FUNC_AND || aluFunc == ALU_FUNC_SUB ||
			aluFunc == ALU_FUNC_XOR || aluFunc == ALU_FUNC_CMP || aluFunc == ALU_FUNC_TEST);
	//Rd == REG_ESP is allowed for this Emit function only - used after calling a helper fn
	assert(	Rd == REG_EAX || Rd == REG_ECX || Rd == REG_EDX || Rd == REG_EBX ||
			Rd == REG_ESI || Rd == REG_EDI || Rd == REG_ESP);

	if(aluFunc == ALU_FUNC_TEST)
	{
		if(Rd == REG_EAX)
		{
			hostCode.push_back(0xa9);
			hostCode.push_back(static_cast<Byte>(Id));
			hostCode.push_back(static_cast<Byte>(Id >> 8));
			hostCode.push_back(static_cast<Byte>(Id >> 16));
			hostCode.push_back(static_cast<Byte>(Id >> 24));

			return 1;
		}
		else
		{
			hostCode.push_back(0xf7);
			hostCode.push_back(0xc0 | static_cast<Byte>(0x00 << 3) | static_cast<Byte>(Rd));
			hostCode.push_back(static_cast<Byte>(Id));
			hostCode.push_back(static_cast<Byte>(Id >> 8));
			hostCode.push_back(static_cast<Byte>(Id >> 16));
			hostCode.push_back(static_cast<Byte>(Id >> 24));

			return 2;
		}
	}
	else
	{
		if(Rd == REG_EAX)
		{
			hostCode.push_back(static_cast<Byte>(aluFunc << 3) | 0x05);
			hostCode.push_back(static_cast<Byte>(Id));
			hostCode.push_back(static_cast<Byte>(Id >> 8));
			hostCode.push_back(static_cast<Byte>(Id >> 16));
			hostCode.push_back(static_cast<Byte>(Id >> 24));

			return 1;
		}
		else
		{
			hostCode.push_back(0x81);
			hostCode.push_back(0xc0 | static_cast<Byte>(aluFunc << 3) | static_cast<Byte>(Rd));
			hostCode.push_back(static_cast<Byte>(Id));
			hostCode.push_back(static_cast<Byte>(Id >> 8));
			hostCode.push_back(static_cast<Byte>(Id >> 16));
			hostCode.push_back(static_cast<Byte>(Id >> 24));

			return 2;
		}
	}
}

void EmitALU_Gb_Rb(std::vector <Byte> & hostCode, Dword aluFunc, Dword Gb, Dword Rb)
{
	assert(	aluFunc == ALU_FUNC_ADD || aluFunc == ALU_FUNC_OR || aluFunc == ALU_FUNC_ADC ||
			aluFunc == ALU_FUNC_SBB || aluFunc == ALU_FUNC_AND || aluFunc == ALU_FUNC_SUB ||
			aluFunc == ALU_FUNC_XOR || aluFunc == ALU_FUNC_CMP || aluFunc == ALU_FUNC_TEST);
	assert(	Gb == REG_AL || Gb == REG_CL || Gb == REG_DL || Gb == REG_BL ||
			Gb == REG_AH || Gb == REG_CH || Gb == REG_DH || Gb == REG_BH);
	assert(	Rb == REG_AL || Rb == REG_CL || Rb == REG_DL || Rb == REG_BL ||
			Rb == REG_AH || Rb == REG_CH || Rb == REG_DH || Rb == REG_BH);

	if(aluFunc == ALU_FUNC_TEST)
	{
		hostCode.push_back(0x84);	//TEST Rb, Gb (== TEST Gb, Rb)
		hostCode.push_back(	0xc0 | static_cast<Byte>((Gb - REG_AL) << 3) |
							static_cast<Byte>(Rb - REG_AL));
	}
	else
	{
		hostCode.push_back(0x02 + static_cast<Byte>(aluFunc * 8));
		hostCode.push_back(	0xc0 | static_cast<Byte>((Gb - REG_AL) << 3) |
							static_cast<Byte>(Rb - REG_AL));
	}
}

void EmitALU_Gw_Rw(std::vector <Byte> & hostCode, Dword aluFunc, Dword Gw, Dword Rw)
{
	assert(	aluFunc == ALU_FUNC_ADD || aluFunc == ALU_FUNC_OR || aluFunc == ALU_FUNC_ADC ||
			aluFunc == ALU_FUNC_SBB || aluFunc == ALU_FUNC_AND || aluFunc == ALU_FUNC_SUB ||
			aluFunc == ALU_FUNC_XOR || aluFunc == ALU_FUNC_CMP || aluFunc == ALU_FUNC_TEST);
	assert(Gw == REG_EAX || Gw == REG_ECX || Gw == REG_EDX || Gw == REG_EBX || Gw == REG_ESI || Gw == REG_EDI);
	assert(Rw == REG_EAX || Rw == REG_ECX || Rw == REG_EDX || Rw == REG_EBX || Rw == REG_ESI || Rw == REG_EDI);

	if(aluFunc == ALU_FUNC_TEST)
	{
		hostCode.push_back(0x66);
		hostCode.push_back(0x85);	//TEST Rw, Gw (== TEST Gw, Rw)
		hostCode.push_back(0xc0 | static_cast<Byte>(Gw << 3) | static_cast<Byte>(Rw));
	}
	else
	{
		hostCode.push_back(0x66);
		hostCode.push_back(0x03 + static_cast<Byte>(aluFunc * 8));
		hostCode.push_back(0xc0 | static_cast<Byte>(Gw << 3) | static_cast<Byte>(Rw));
	}
}

void EmitALU_Gd_Rd(std::vector <Byte> & hostCode, Dword aluFunc, Dword Gd, Dword Rd)
{
	assert(	aluFunc == ALU_FUNC_ADD || aluFunc == ALU_FUNC_OR || aluFunc == ALU_FUNC_ADC ||
			aluFunc == ALU_FUNC_SBB || aluFunc == ALU_FUNC_AND || aluFunc == ALU_FUNC_SUB ||
			aluFunc == ALU_FUNC_XOR || aluFunc == ALU_FUNC_CMP || aluFunc == ALU_FUNC_TEST);
	assert(Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX || Gd == REG_ESI || Gd == REG_EDI);
	assert(Rd == REG_EAX || Rd == REG_ECX || Rd == REG_EDX || Rd == REG_EBX || Rd == REG_ESI || Rd == REG_EDI);

	if(aluFunc == ALU_FUNC_TEST)
	{
		hostCode.push_back(0x85);	//TEST Rd, Gd (== TEST Gd, Rd)
		hostCode.push_back(0xc0 | static_cast<Byte>(Gd << 3) | static_cast<Byte>(Rd));
	}
	else
	{
		hostCode.push_back(0x03 + static_cast<Byte>(aluFunc * 8));
		hostCode.push_back(0xc0 | static_cast<Byte>(Gd << 3) | static_cast<Byte>(Rd));
	}
}

Dword EmitALU_Mb_Ib(std::vector <Byte> & hostCode, Dword aluFunc, Dword base, Dword index,
					Dword shift, Dword disp, Dword Ib)
{
	assert(	aluFunc == ALU_FUNC_ADD || aluFunc == ALU_FUNC_OR || aluFunc == ALU_FUNC_ADC ||
			aluFunc == ALU_FUNC_SBB || aluFunc == ALU_FUNC_AND || aluFunc == ALU_FUNC_SUB ||
			aluFunc == ALU_FUNC_XOR || aluFunc == ALU_FUNC_CMP || aluFunc == ALU_FUNC_TEST);

	if(aluFunc == ALU_FUNC_TEST)
	{
		hostCode.push_back(0xf6);
		Dword modRMLength = EmitMemoryModRM(hostCode, 0x00, base, index, shift, disp);
		hostCode.push_back(static_cast<Byte>(Ib));

		return 1 + modRMLength;
	}
	else
	{
		hostCode.push_back(0x80);
		Dword modRMLength = EmitMemoryModRM(hostCode, aluFunc, base, index, shift, disp);
		hostCode.push_back(static_cast<Byte>(Ib));

		return 1 + modRMLength;
	}
}

Dword EmitALU_Mw_Iw(std::vector <Byte> & hostCode, Dword aluFunc, Dword base, Dword index,
					Dword shift, Dword disp, Dword Iw)
{
	assert(	aluFunc == ALU_FUNC_ADD || aluFunc == ALU_FUNC_OR || aluFunc == ALU_FUNC_ADC ||
			aluFunc == ALU_FUNC_SBB || aluFunc == ALU_FUNC_AND || aluFunc == ALU_FUNC_SUB ||
			aluFunc == ALU_FUNC_XOR || aluFunc == ALU_FUNC_CMP || aluFunc == ALU_FUNC_TEST);

	if(aluFunc == ALU_FUNC_TEST)
	{
		hostCode.push_back(0x66);
		hostCode.push_back(0xf7);
		Dword modRMLength = EmitMemoryModRM(hostCode, 0x00, base, index, shift, disp);
		hostCode.push_back(static_cast<Byte>(Iw));
		hostCode.push_back(static_cast<Byte>(Iw >> 8));

		return 2 + modRMLength;
	}
	else
	{
		hostCode.push_back(0x66);
		hostCode.push_back(0x81);
		Dword modRMLength = EmitMemoryModRM(hostCode, aluFunc, base, index, shift, disp);
		hostCode.push_back(static_cast<Byte>(Iw));
		hostCode.push_back(static_cast<Byte>(Iw >> 8));

		return 2 + modRMLength;
	}
}

Dword EmitALU_Md_Id(std::vector <Byte> & hostCode, Dword aluFunc, Dword base, Dword index,
					Dword shift, Dword disp, Dword Id)
{
	assert(	aluFunc == ALU_FUNC_ADD || aluFunc == ALU_FUNC_OR || aluFunc == ALU_FUNC_ADC ||
			aluFunc == ALU_FUNC_SBB || aluFunc == ALU_FUNC_AND || aluFunc == ALU_FUNC_SUB ||
			aluFunc == ALU_FUNC_XOR || aluFunc == ALU_FUNC_CMP || aluFunc == ALU_FUNC_TEST);

	if(aluFunc == ALU_FUNC_TEST)
	{
		hostCode.push_back(0xf7);
		Dword modRMLength = EmitMemoryModRM(hostCode, 0x00, base, index, shift, disp);
		hostCode.push_back(static_cast<Byte>(Id));
		hostCode.push_back(static_cast<Byte>(Id >> 8));
		hostCode.push_back(static_cast<Byte>(Id >> 16));
		hostCode.push_back(static_cast<Byte>(Id >> 24));

		return 1 + modRMLength;
	}
	else
	{
		hostCode.push_back(0x81);
		Dword modRMLength = EmitMemoryModRM(hostCode, aluFunc, base, index, shift, disp);
		hostCode.push_back(static_cast<Byte>(Id));
		hostCode.push_back(static_cast<Byte>(Id >> 8));
		hostCode.push_back(static_cast<Byte>(Id >> 16));
		hostCode.push_back(static_cast<Byte>(Id >> 24));

		return 1 + modRMLength;
	}
}

void EmitALU_Mb_Gb(std::vector <Byte> & hostCode, Dword aluFunc, Dword base, Dword index,
				   Dword shift, Dword disp, Dword Gb)
{
	assert(	aluFunc == ALU_FUNC_ADD || aluFunc == ALU_FUNC_OR || aluFunc == ALU_FUNC_ADC ||
			aluFunc == ALU_FUNC_SBB || aluFunc == ALU_FUNC_AND || aluFunc == ALU_FUNC_SUB ||
			aluFunc == ALU_FUNC_XOR || aluFunc == ALU_FUNC_CMP || aluFunc == ALU_FUNC_TEST);
	assert(	Gb == REG_AL || Gb == REG_CL || Gb == REG_DL || Gb == REG_BL ||
			Gb == REG_AH || Gb == REG_CH || Gb == REG_DH || Gb == REG_BH);

	if(aluFunc == ALU_FUNC_TEST)
	{
		hostCode.push_back(0x84);
		EmitMemoryModRM(hostCode, Gb - REG_AL, base, index, shift, disp);
	}
	else
	{
		hostCode.push_back(0x00 + static_cast<Byte>(aluFunc * 8));
		EmitMemoryModRM(hostCode, Gb - REG_AL, base, index, shift, disp);
	}
}

void EmitALU_Mw_Gw(std::vector <Byte> & hostCode, Dword aluFunc, Dword base, Dword index,
				   Dword shift, Dword disp, Dword Gw)
{
	assert(	aluFunc == ALU_FUNC_ADD || aluFunc == ALU_FUNC_OR || aluFunc == ALU_FUNC_ADC ||
			aluFunc == ALU_FUNC_SBB || aluFunc == ALU_FUNC_AND || aluFunc == ALU_FUNC_SUB ||
			aluFunc == ALU_FUNC_XOR || aluFunc == ALU_FUNC_CMP || aluFunc == ALU_FUNC_TEST);
	assert(Gw == REG_EAX || Gw == REG_ECX || Gw == REG_EDX || Gw == REG_EBX || Gw == REG_ESI || Gw == REG_EDI);

	if(aluFunc == ALU_FUNC_TEST)
	{
		hostCode.push_back(0x66);
		hostCode.push_back(0x85);
		EmitMemoryModRM(hostCode, Gw, base, index, shift, disp);
	}
	else
	{
		hostCode.push_back(0x66);
		hostCode.push_back(0x01 + static_cast<Byte>(aluFunc * 8));
		EmitMemoryModRM(hostCode, Gw, base, index, shift, disp);
	}
}

void EmitALU_Md_Gd(std::vector <Byte> & hostCode, Dword aluFunc, Dword base, Dword index,
				   Dword shift, Dword disp, Dword Gd)
{
	assert(	aluFunc == ALU_FUNC_ADD || aluFunc == ALU_FUNC_OR || aluFunc == ALU_FUNC_ADC ||
			aluFunc == ALU_FUNC_SBB || aluFunc == ALU_FUNC_AND || aluFunc == ALU_FUNC_SUB ||
			aluFunc == ALU_FUNC_XOR || aluFunc == ALU_FUNC_CMP || aluFunc == ALU_FUNC_TEST);
	assert(Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX || Gd == REG_ESI || Gd == REG_EDI);

	if(aluFunc == ALU_FUNC_TEST)
	{
		hostCode.push_back(0x85);
		EmitMemoryModRM(hostCode, Gd, base, index, shift, disp);
	}
	else
	{
		hostCode.push_back(0x01 + static_cast<Byte>(aluFunc * 8));
		EmitMemoryModRM(hostCode, Gd, base, index, shift, disp);
	}
}

void EmitALU_Gb_Mb(std::vector <Byte> & hostCode, Dword aluFunc, Dword Gb, Dword base,
				   Dword index, Dword shift, Dword disp)
{
	assert(	aluFunc == ALU_FUNC_ADD || aluFunc == ALU_FUNC_OR || aluFunc == ALU_FUNC_ADC ||
			aluFunc == ALU_FUNC_SBB || aluFunc == ALU_FUNC_AND || aluFunc == ALU_FUNC_SUB ||
			aluFunc == ALU_FUNC_XOR || aluFunc == ALU_FUNC_CMP);
	assert(	Gb == REG_AL || Gb == REG_CL || Gb == REG_DL || Gb == REG_BL ||
			Gb == REG_AH || Gb == REG_CH || Gb == REG_DH || Gb == REG_BH);

	hostCode.push_back(0x02 + static_cast<Byte>(aluFunc * 8));
	EmitMemoryModRM(hostCode, Gb, base, index, shift, disp);
}

void EmitALU_Gw_Mw(std::vector <Byte> & hostCode, Dword aluFunc, Dword Gw, Dword base,
				   Dword index, Dword shift, Dword disp)
{
	assert(	aluFunc == ALU_FUNC_ADD || aluFunc == ALU_FUNC_OR || aluFunc == ALU_FUNC_ADC ||
			aluFunc == ALU_FUNC_SBB || aluFunc == ALU_FUNC_AND || aluFunc == ALU_FUNC_SUB ||
			aluFunc == ALU_FUNC_XOR || aluFunc == ALU_FUNC_CMP);
	assert(Gw == REG_EAX || Gw == REG_ECX || Gw == REG_EDX || Gw == REG_EBX || Gw == REG_ESI || Gw == REG_EDI);

	hostCode.push_back(0x66);
	hostCode.push_back(0x03 + static_cast<Byte>(aluFunc * 8));
	EmitMemoryModRM(hostCode, Gw, base, index, shift, disp);
}

void EmitALU_Gd_Md(std::vector <Byte> & hostCode, Dword aluFunc, Dword Gd, Dword base,
				   Dword index, Dword shift, Dword disp)
{
	assert(	aluFunc == ALU_FUNC_ADD || aluFunc == ALU_FUNC_OR || aluFunc == ALU_FUNC_ADC ||
			aluFunc == ALU_FUNC_SBB || aluFunc == ALU_FUNC_AND || aluFunc == ALU_FUNC_SUB ||
			aluFunc == ALU_FUNC_XOR || aluFunc == ALU_FUNC_CMP);
	assert(Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX || Gd == REG_ESI || Gd == REG_EDI);

	hostCode.push_back(0x03 + static_cast<Byte>(aluFunc * 8));
	EmitMemoryModRM(hostCode, Gd, base, index, shift, disp);
}

void EmitUALU_Rb(std::vector <Byte> & hostCode, Dword ualuFunc, Dword Rb)
{
	assert(	ualuFunc == UALU_FUNC_INC || ualuFunc == UALU_FUNC_DEC ||
			ualuFunc == UALU_FUNC_NOT || ualuFunc == UALU_FUNC_NEG);
	assert(	Rb == REG_AL || Rb == REG_CL || Rb == REG_DL || Rb == REG_BL ||
			Rb == REG_AH || Rb == REG_CH || Rb == REG_DH || Rb == REG_BH);

	if(ualuFunc == UALU_FUNC_INC || ualuFunc == UALU_FUNC_DEC)
	{
		hostCode.push_back(0xfe);
		hostCode.push_back(	0xc0 | (static_cast<Byte>(ualuFunc) << 3) |
							static_cast<Byte>(Rb - REG_AL));
	}
	else if(ualuFunc == UALU_FUNC_NOT || ualuFunc == UALU_FUNC_NEG)
	{
		hostCode.push_back(0xf6);
		hostCode.push_back(	0xc0 | (static_cast<Byte>(ualuFunc) << 3) |
							static_cast<Byte>(Rb - REG_AL));
	}
}

void EmitUALU_Rw(std::vector <Byte> & hostCode, Dword ualuFunc, Dword Rw)
{
	assert(	ualuFunc == UALU_FUNC_INC || ualuFunc == UALU_FUNC_DEC ||
			ualuFunc == UALU_FUNC_NOT || ualuFunc == UALU_FUNC_NEG);
	assert(Rw == REG_EAX || Rw == REG_ECX || Rw == REG_EDX || Rw == REG_EBX || Rw == REG_ESI || Rw == REG_EDI);

	if(ualuFunc == UALU_FUNC_INC || ualuFunc == UALU_FUNC_DEC)
	{
		hostCode.push_back(0x66);
		hostCode.push_back(0x40 + static_cast<Byte>(ualuFunc * 0x08) + static_cast<Byte>(Rw));
	}
	else if(ualuFunc == UALU_FUNC_NOT || ualuFunc == UALU_FUNC_NEG)
	{
		hostCode.push_back(0x66);
		hostCode.push_back(0xf7);
		hostCode.push_back(0xc0 | (static_cast<Byte>(ualuFunc) << 3) | static_cast<Byte>(Rw));
	}
}

void EmitUALU_Rd(std::vector <Byte> & hostCode, Dword ualuFunc, Dword Rd)
{
	assert(	ualuFunc == UALU_FUNC_INC || ualuFunc == UALU_FUNC_DEC ||
			ualuFunc == UALU_FUNC_NOT || ualuFunc == UALU_FUNC_NEG);
	assert(Rd == REG_EAX || Rd == REG_ECX || Rd == REG_EDX || Rd == REG_EBX || Rd == REG_ESI || Rd == REG_EDI);

	if(ualuFunc == UALU_FUNC_INC || ualuFunc == UALU_FUNC_DEC)
	{
		hostCode.push_back(0x40 + static_cast<Byte>(ualuFunc * 0x08) + static_cast<Byte>(Rd));
	}
	else if(ualuFunc == UALU_FUNC_NOT || ualuFunc == UALU_FUNC_NEG)
	{
		hostCode.push_back(0xf7);
		hostCode.push_back(0xc0 | (static_cast<Byte>(ualuFunc) << 3) | static_cast<Byte>(Rd));
	}
}

void EmitUALU_Mb(	std::vector <Byte> & hostCode, Dword ualuFunc, Dword base, Dword index,
					Dword shift, Dword disp)
{
	assert(	ualuFunc == UALU_FUNC_INC || ualuFunc == UALU_FUNC_DEC ||
			ualuFunc == UALU_FUNC_NOT || ualuFunc == UALU_FUNC_NEG);

	if(ualuFunc == UALU_FUNC_INC || ualuFunc == UALU_FUNC_DEC)
	{
		hostCode.push_back(0xfe);
		EmitMemoryModRM(hostCode, ualuFunc, base, index, shift, disp);
	}
	else if(ualuFunc == UALU_FUNC_NOT || ualuFunc == UALU_FUNC_NEG)
	{
		hostCode.push_back(0xf6);
		EmitMemoryModRM(hostCode, ualuFunc, base, index, shift, disp);
	}
}

void EmitUALU_Mw(	std::vector <Byte> & hostCode, Dword ualuFunc, Dword base, Dword index,
					Dword shift, Dword disp)
{
	assert(	ualuFunc == UALU_FUNC_INC || ualuFunc == UALU_FUNC_DEC ||
			ualuFunc == UALU_FUNC_NOT || ualuFunc == UALU_FUNC_NEG);

	if(ualuFunc == UALU_FUNC_INC || ualuFunc == UALU_FUNC_DEC)
	{
		hostCode.push_back(0x66);
		hostCode.push_back(0xff);
		EmitMemoryModRM(hostCode, ualuFunc, base, index, shift, disp);
	}
	else if(ualuFunc == UALU_FUNC_NOT || ualuFunc == UALU_FUNC_NEG)
	{
		hostCode.push_back(0x66);
		hostCode.push_back(0xf7);
		EmitMemoryModRM(hostCode, ualuFunc, base, index, shift, disp);
	}
}

void EmitUALU_Md(	std::vector <Byte> & hostCode, Dword ualuFunc, Dword base, Dword index,
					Dword shift, Dword disp)
{
	assert(	ualuFunc == UALU_FUNC_INC || ualuFunc == UALU_FUNC_DEC ||
			ualuFunc == UALU_FUNC_NOT || ualuFunc == UALU_FUNC_NEG);

	if(ualuFunc == UALU_FUNC_INC || ualuFunc == UALU_FUNC_DEC)
	{
		hostCode.push_back(0xff);
		EmitMemoryModRM(hostCode, ualuFunc, base, index, shift, disp);
	}
	else if(ualuFunc == UALU_FUNC_NOT || ualuFunc == UALU_FUNC_NEG)
	{
		hostCode.push_back(0xf7);
		EmitMemoryModRM(hostCode, ualuFunc, base, index, shift, disp);
	}
}

Dword EmitSHIFT_Rb_Ib(std::vector <Byte> & hostCode, Dword shiftFunc, Dword Rb, Dword Ib)
{
	assert(	shiftFunc == SHIFT_FUNC_ROL || shiftFunc == SHIFT_FUNC_ROR ||
			shiftFunc == SHIFT_FUNC_RCL || shiftFunc == SHIFT_FUNC_RCR ||
			shiftFunc == SHIFT_FUNC_SHL || shiftFunc == SHIFT_FUNC_SHR ||
			shiftFunc == SHIFT_FUNC_SAR);
	assert(	Rb == REG_AL || Rb == REG_CL || Rb == REG_DL || Rb == REG_BL ||
			Rb == REG_AH || Rb == REG_CH || Rb == REG_DH || Rb == REG_BH);

	hostCode.push_back(0xc0);
	hostCode.push_back(	0xc0 | static_cast<Byte>(shiftFunc << 3) |
						static_cast<Byte>(Rb - REG_AL));
	hostCode.push_back(static_cast<Byte>(Ib));

	return 2;
}

void EmitSHIFT_Rb_CL(std::vector <Byte> & hostCode, Dword shiftFunc, Dword Rb)
{
	assert(	shiftFunc == SHIFT_FUNC_ROL || shiftFunc == SHIFT_FUNC_ROR ||
			shiftFunc == SHIFT_FUNC_RCL || shiftFunc == SHIFT_FUNC_RCR ||
			shiftFunc == SHIFT_FUNC_SHL || shiftFunc == SHIFT_FUNC_SHR ||
			shiftFunc == SHIFT_FUNC_SAR);
	assert(	Rb == REG_AL || Rb == REG_CL || Rb == REG_DL || Rb == REG_BL ||
			Rb == REG_AH || Rb == REG_CH || Rb == REG_DH || Rb == REG_BH);

	hostCode.push_back(0xd2);
	hostCode.push_back(	0xc0 | static_cast<Byte>(shiftFunc << 3) |
						static_cast<Byte>(Rb - REG_AL));
}

Dword EmitSHIFT_Rw_Ib(std::vector <Byte> & hostCode, Dword shiftFunc, Dword Rw, Dword Ib)
{
	assert(	shiftFunc == SHIFT_FUNC_ROL || shiftFunc == SHIFT_FUNC_ROR ||
			shiftFunc == SHIFT_FUNC_RCL || shiftFunc == SHIFT_FUNC_RCR ||
			shiftFunc == SHIFT_FUNC_SHL || shiftFunc == SHIFT_FUNC_SHR ||
			shiftFunc == SHIFT_FUNC_SAR);
	assert(Rw == REG_EAX || Rw == REG_ECX || Rw == REG_EDX || Rw == REG_EBX || Rw == REG_ESI || Rw == REG_EDI);

	hostCode.push_back(0x66);
	hostCode.push_back(0xc1);
	hostCode.push_back(0xc0 | static_cast<Byte>(shiftFunc << 3) | static_cast<Byte>(Rw));
	hostCode.push_back(static_cast<Byte>(Ib));

	return 3;
}

void EmitSHIFT_Rw_CL(std::vector <Byte> & hostCode, Dword shiftFunc, Dword Rw)
{
	assert(	shiftFunc == SHIFT_FUNC_ROL || shiftFunc == SHIFT_FUNC_ROR ||
			shiftFunc == SHIFT_FUNC_RCL || shiftFunc == SHIFT_FUNC_RCR ||
			shiftFunc == SHIFT_FUNC_SHL || shiftFunc == SHIFT_FUNC_SHR ||
			shiftFunc == SHIFT_FUNC_SAR);
	assert(Rw == REG_EAX || Rw == REG_ECX || Rw == REG_EDX || Rw == REG_EBX || Rw == REG_ESI || Rw == REG_EDI);

	hostCode.push_back(0x66);
	hostCode.push_back(0xd3);
	hostCode.push_back(0xc0 | static_cast<Byte>(shiftFunc << 3) | static_cast<Byte>(Rw));
}

Dword EmitSHIFT_Rd_Ib(std::vector <Byte> & hostCode, Dword shiftFunc, Dword Rd, Dword Ib)
{
	assert(	shiftFunc == SHIFT_FUNC_ROL || shiftFunc == SHIFT_FUNC_ROR ||
			shiftFunc == SHIFT_FUNC_RCL || shiftFunc == SHIFT_FUNC_RCR ||
			shiftFunc == SHIFT_FUNC_SHL || shiftFunc == SHIFT_FUNC_SHR ||
			shiftFunc == SHIFT_FUNC_SAR);
	assert(Rd == REG_EAX || Rd == REG_ECX || Rd == REG_EDX || Rd == REG_EBX || Rd == REG_ESI || Rd == REG_EDI);

	hostCode.push_back(0xc1);
	hostCode.push_back(0xc0 | static_cast<Byte>(shiftFunc << 3) | static_cast<Byte>(Rd));
	hostCode.push_back(static_cast<Byte>(Ib));

	return 2;
}

void EmitSHIFT_Rd_CL(std::vector <Byte> & hostCode, Dword shiftFunc, Dword Rd)
{
	assert(	shiftFunc == SHIFT_FUNC_ROL || shiftFunc == SHIFT_FUNC_ROR ||
			shiftFunc == SHIFT_FUNC_RCL || shiftFunc == SHIFT_FUNC_RCR ||
			shiftFunc == SHIFT_FUNC_SHL || shiftFunc == SHIFT_FUNC_SHR ||
			shiftFunc == SHIFT_FUNC_SAR);
	assert(Rd == REG_EAX || Rd == REG_ECX || Rd == REG_EDX || Rd == REG_EBX || Rd == REG_ESI || Rd == REG_EDI);

	hostCode.push_back(0xd3);
	hostCode.push_back(0xc0 | static_cast<Byte>(shiftFunc << 3) | static_cast<Byte>(Rd));
}

Dword EmitSHIFT_Mb_Ib(	std::vector <Byte> & hostCode, Dword shiftFunc, Dword base, Dword index,
						Dword shift, Dword disp, Dword Ib)
{
	assert(	shiftFunc == SHIFT_FUNC_ROL || shiftFunc == SHIFT_FUNC_ROR ||
			shiftFunc == SHIFT_FUNC_RCL || shiftFunc == SHIFT_FUNC_RCR ||
			shiftFunc == SHIFT_FUNC_SHL || shiftFunc == SHIFT_FUNC_SHR ||
			shiftFunc == SHIFT_FUNC_SAR);

	hostCode.push_back(0xc0);
	Dword modRMLength = EmitMemoryModRM(hostCode, shiftFunc, base, index, shift, disp);
	hostCode.push_back(static_cast<Byte>(Ib));

	return 1 + modRMLength;
}

void EmitSHIFT_Mb_CL(	std::vector <Byte> & hostCode, Dword shiftFunc, Dword base, Dword index,
						Dword shift, Dword disp)
{
	assert(	shiftFunc == SHIFT_FUNC_ROL || shiftFunc == SHIFT_FUNC_ROR ||
			shiftFunc == SHIFT_FUNC_RCL || shiftFunc == SHIFT_FUNC_RCR ||
			shiftFunc == SHIFT_FUNC_SHL || shiftFunc == SHIFT_FUNC_SHR ||
			shiftFunc == SHIFT_FUNC_SAR);

	hostCode.push_back(0xd2);
	EmitMemoryModRM(hostCode, shiftFunc, base, index, shift, disp);
}

Dword EmitSHIFT_Mw_Ib(	std::vector <Byte> & hostCode, Dword shiftFunc, Dword base, Dword index,
						Dword shift, Dword disp, Dword Ib)
{
	assert(	shiftFunc == SHIFT_FUNC_ROL || shiftFunc == SHIFT_FUNC_ROR ||
			shiftFunc == SHIFT_FUNC_RCL || shiftFunc == SHIFT_FUNC_RCR ||
			shiftFunc == SHIFT_FUNC_SHL || shiftFunc == SHIFT_FUNC_SHR ||
			shiftFunc == SHIFT_FUNC_SAR);

	hostCode.push_back(0x66);
	hostCode.push_back(0xc1);
	Dword modRMLength = EmitMemoryModRM(hostCode, shiftFunc, base, index, shift, disp);
	hostCode.push_back(static_cast<Byte>(Ib));

	return 2 + modRMLength;
}

void EmitSHIFT_Mw_CL(	std::vector <Byte> & hostCode, Dword shiftFunc, Dword base, Dword index,
						Dword shift, Dword disp)
{
	assert(	shiftFunc == SHIFT_FUNC_ROL || shiftFunc == SHIFT_FUNC_ROR ||
			shiftFunc == SHIFT_FUNC_RCL || shiftFunc == SHIFT_FUNC_RCR ||
			shiftFunc == SHIFT_FUNC_SHL || shiftFunc == SHIFT_FUNC_SHR ||
			shiftFunc == SHIFT_FUNC_SAR);

	hostCode.push_back(0x66);
	hostCode.push_back(0xd3);
	EmitMemoryModRM(hostCode, shiftFunc, base, index, shift, disp);
}

Dword EmitSHIFT_Md_Ib(	std::vector <Byte> & hostCode, Dword shiftFunc, Dword base, Dword index,
						Dword shift, Dword disp, Dword Ib)
{
	assert(	shiftFunc == SHIFT_FUNC_ROL || shiftFunc == SHIFT_FUNC_ROR ||
			shiftFunc == SHIFT_FUNC_RCL || shiftFunc == SHIFT_FUNC_RCR ||
			shiftFunc == SHIFT_FUNC_SHL || shiftFunc == SHIFT_FUNC_SHR ||
			shiftFunc == SHIFT_FUNC_SAR);

	hostCode.push_back(0xc1);
	Dword modRMLength = EmitMemoryModRM(hostCode, shiftFunc, base, index, shift, disp);
	hostCode.push_back(static_cast<Byte>(Ib));

	return 1 + modRMLength;
}

void EmitSHIFT_Md_CL(	std::vector <Byte> & hostCode, Dword shiftFunc, Dword base, Dword index,
						Dword shift, Dword disp)
{
	assert(	shiftFunc == SHIFT_FUNC_ROL || shiftFunc == SHIFT_FUNC_ROR ||
			shiftFunc == SHIFT_FUNC_RCL || shiftFunc == SHIFT_FUNC_RCR ||
			shiftFunc == SHIFT_FUNC_SHL || shiftFunc == SHIFT_FUNC_SHR ||
			shiftFunc == SHIFT_FUNC_SAR);

	hostCode.push_back(0xd3);
	EmitMemoryModRM(hostCode, shiftFunc, base, index, shift, disp);
}

void EmitSHIFTD_Rw_Gw_CL(std::vector <Byte> & hostCode, Dword shiftFunc, Dword Rw, Dword Gw)
{
	assert(shiftFunc == SHIFTD_FUNC_SHLD || shiftFunc == SHIFTD_FUNC_SHRD);
	assert(Rw == REG_EAX || Rw == REG_ECX || Rw == REG_EDX || Rw == REG_EBX || Rw == REG_ESI || Rw == REG_EDI);
	assert(Gw == REG_EAX || Gw == REG_ECX || Gw == REG_EDX || Gw == REG_EBX || Gw == REG_ESI || Gw == REG_EDI);

	hostCode.push_back(0x66);
	hostCode.push_back(0x0f);
	hostCode.push_back(0xa5 + static_cast<Byte>(shiftFunc * 8));
	hostCode.push_back(0xc0 | static_cast<Byte>(Gw << 3) | static_cast<Byte>(Rw));
}

Dword EmitSHIFTD_Rw_Gw_Ib(	std::vector <Byte> & hostCode, Dword shiftFunc, Dword Rw, Dword Gw,
							Dword Ib)
{
	assert(shiftFunc == SHIFTD_FUNC_SHLD || shiftFunc == SHIFTD_FUNC_SHRD);
	assert(Rw == REG_EAX || Rw == REG_ECX || Rw == REG_EDX || Rw == REG_EBX || Rw == REG_ESI);
	assert(Gw == REG_EAX || Gw == REG_ECX || Gw == REG_EDX || Gw == REG_EBX || Gw == REG_ESI);

	hostCode.push_back(0x66);
	hostCode.push_back(0x0f);
	hostCode.push_back(0xa4 + static_cast<Byte>(shiftFunc * 8));
	hostCode.push_back(0xc0 | static_cast<Byte>(Gw << 3) | static_cast<Byte>(Rw));
	hostCode.push_back(static_cast<Byte>(Ib));

	return 4;
}

void EmitSHIFTD_Rd_Gd_CL(std::vector <Byte> & hostCode, Dword shiftFunc, Dword Rd, Dword Gd)
{
	assert(shiftFunc == SHIFTD_FUNC_SHLD || shiftFunc == SHIFTD_FUNC_SHRD);
	assert(Rd == REG_EAX || Rd == REG_ECX || Rd == REG_EDX || Rd == REG_EBX || Rd == REG_ESI || Rd == REG_EDI);
	assert(Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX || Gd == REG_ESI || Gd == REG_EDI);

	hostCode.push_back(0x0f);
	hostCode.push_back(0xa5 + static_cast<Byte>(shiftFunc * 8));
	hostCode.push_back(0xc0 | static_cast<Byte>(Gd << 3) | static_cast<Byte>(Rd));
}

Dword EmitSHIFTD_Rd_Gd_Ib(	std::vector <Byte> & hostCode, Dword shiftFunc, Dword Rd, Dword Gd,
							Dword Ib)
{
	assert(shiftFunc == SHIFTD_FUNC_SHLD || shiftFunc == SHIFTD_FUNC_SHRD);
	assert(Rd == REG_EAX || Rd == REG_ECX || Rd == REG_EDX || Rd == REG_EBX || Rd == REG_ESI || Rd == REG_EDI);
	assert(Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX || Gd == REG_ESI || Gd == REG_EDI);

	hostCode.push_back(0x0f);
	hostCode.push_back(0xa4 + static_cast<Byte>(shiftFunc * 8));
	hostCode.push_back(0xc0 | static_cast<Byte>(Gd << 3) | static_cast<Byte>(Rd));
	hostCode.push_back(static_cast<Byte>(Ib));

	return 3;
}

void EmitMUL_Rb(std::vector <Byte> & hostCode, Dword Rb)
{
	assert(	Rb == REG_AL || Rb == REG_CL || Rb == REG_DL || Rb == REG_BL ||
			Rb == REG_AH || Rb == REG_CH || Rb == REG_DH || Rb == REG_BH);

	hostCode.push_back(0xf6);
	hostCode.push_back(0xc0 | static_cast<Byte>(0x04 << 3) | static_cast<Byte>(Rb - REG_AL));
}

void EmitMUL_Rw(std::vector <Byte> & hostCode, Dword Rw)
{
	assert(Rw == REG_EAX || Rw == REG_ECX || Rw == REG_EDX || Rw == REG_EBX || Rw == REG_ESI || Rw == REG_EDI);

	hostCode.push_back(0x66);
	hostCode.push_back(0xf7);
	hostCode.push_back(0xc0 | static_cast<Byte>(0x04 << 3) | static_cast<Byte>(Rw));
}

void EmitMUL_Rd(std::vector <Byte> & hostCode, Dword Rd)
{
	assert(Rd == REG_EAX || Rd == REG_ECX || Rd == REG_EDX || Rd == REG_EBX || Rd == REG_ESI || Rd == REG_EDI);

	hostCode.push_back(0xf7);
	hostCode.push_back(0xc0 | static_cast<Byte>(0x04 << 3) | static_cast<Byte>(Rd));
}

void EmitIMUL_Rb(std::vector <Byte> & hostCode, Dword Rb)
{
	assert(	Rb == REG_AL || Rb == REG_CL || Rb == REG_DL || Rb == REG_BL ||
			Rb == REG_AH || Rb == REG_CH || Rb == REG_DH || Rb == REG_BH);

	hostCode.push_back(0xf6);
	hostCode.push_back(0xc0 | static_cast<Byte>(0x05 << 3) | static_cast<Byte>(Rb - REG_AL));
}

void EmitIMUL_Rw(std::vector <Byte> & hostCode, Dword Rw)
{
	assert(Rw == REG_EAX || Rw == REG_ECX || Rw == REG_EDX || Rw == REG_EBX || Rw == REG_ESI || Rw == REG_EDI);

	hostCode.push_back(0x66);
	hostCode.push_back(0xf7);
	hostCode.push_back(0xc0 | static_cast<Byte>(0x05 << 3) | static_cast<Byte>(Rw));
}

void EmitIMUL_Rd(std::vector <Byte> & hostCode, Dword Rd)
{
	assert(Rd == REG_EAX || Rd == REG_ECX || Rd == REG_EDX || Rd == REG_EBX || Rd == REG_ESI || Rd == REG_EDI);

	hostCode.push_back(0xf7);
	hostCode.push_back(0xc0 | static_cast<Byte>(0x05 << 3) | static_cast<Byte>(Rd));
}

void EmitDIV_Rb(std::vector <Byte> & hostCode, Dword Rb)
{
	assert(	Rb == REG_AL || Rb == REG_CL || Rb == REG_DL || Rb == REG_BL ||
			Rb == REG_AH || Rb == REG_CH || Rb == REG_DH || Rb == REG_BH);

	hostCode.push_back(0xf6);
	hostCode.push_back(0xc0 | static_cast<Byte>(0x06 << 3) | static_cast<Byte>(Rb - REG_AL));
}

void EmitDIV_Rw(std::vector <Byte> & hostCode, Dword Rw)
{
	assert(Rw == REG_EAX || Rw == REG_ECX || Rw == REG_EDX || Rw == REG_EBX || Rw == REG_ESI || Rw == REG_EDI);

	hostCode.push_back(0x66);
	hostCode.push_back(0xf7);
	hostCode.push_back(0xc0 | static_cast<Byte>(0x06 << 3) | static_cast<Byte>(Rw));
}

void EmitDIV_Rd(std::vector <Byte> & hostCode, Dword Rd)
{
	assert(Rd == REG_EAX || Rd == REG_ECX || Rd == REG_EDX || Rd == REG_EBX || Rd == REG_ESI || Rd == REG_EDI);

	hostCode.push_back(0xf7);
	hostCode.push_back(0xc0 | static_cast<Byte>(0x06 << 3) | static_cast<Byte>(Rd));
}

void EmitDIV_Mb(std::vector <Byte> & hostCode, Dword base, Dword index, Dword shift,
				Dword disp)
{
	hostCode.push_back(0xf6);
	EmitMemoryModRM(hostCode, 0x06, base, index, shift, disp);
}

void EmitIDIV_Rb(std::vector <Byte> & hostCode, Dword Rb)
{
	assert(	Rb == REG_AL || Rb == REG_CL || Rb == REG_DL || Rb == REG_BL ||
			Rb == REG_AH || Rb == REG_CH || Rb == REG_DH || Rb == REG_BH);

	hostCode.push_back(0xf6);
	hostCode.push_back(0xc0 | static_cast<Byte>(0x07 << 3) | static_cast<Byte>(Rb - REG_AL));
}

void EmitIDIV_Rw(std::vector <Byte> & hostCode, Dword Rw)
{
	assert(Rw == REG_EAX || Rw == REG_ECX || Rw == REG_EDX || Rw == REG_EBX || Rw == REG_ESI || Rw == REG_EDI);

	hostCode.push_back(0x66);
	hostCode.push_back(0xf7);
	hostCode.push_back(0xc0 | static_cast<Byte>(0x07 << 3) | static_cast<Byte>(Rw));
}

void EmitIDIV_Rd(std::vector <Byte> & hostCode, Dword Rd)
{
	assert(Rd == REG_EAX || Rd == REG_ECX || Rd == REG_EDX || Rd == REG_EBX || Rd == REG_ESI || Rd == REG_EDI);

	hostCode.push_back(0xf7);
	hostCode.push_back(0xc0 | static_cast<Byte>(0x07 << 3) | static_cast<Byte>(Rd));
}

void EmitIDIV_Mb(std::vector <Byte> & hostCode, Dword base, Dword index, Dword shift,
				Dword disp)
{
	hostCode.push_back(0xf6);
	EmitMemoryModRM(hostCode, 0x07, base, index, shift, disp);
}

Dword EmitBIT_Rw_Ib(std::vector <Byte> & hostCode, Dword bitFunc, Dword Rw, Dword Ib)
{
	assert(	bitFunc == BIT_FUNC_BT || bitFunc == BIT_FUNC_BTS ||
			bitFunc == BIT_FUNC_BTR || bitFunc == BIT_FUNC_BTC);
	assert(Rw == REG_EAX || Rw == REG_ECX || Rw == REG_EDX || Rw == REG_EBX || Rw == REG_ESI || Rw == REG_EDI);

	hostCode.push_back(0x66);
	hostCode.push_back(0x0f);
	hostCode.push_back(0xba);
	hostCode.push_back(0xc0 | static_cast<Byte>((bitFunc + 4) << 3) | static_cast<Byte>(Rw));
	hostCode.push_back(static_cast<Byte>(Ib));

	return 4;
}

Dword EmitBIT_Rd_Ib(std::vector <Byte> & hostCode, Dword bitFunc, Dword Rd, Dword Ib)
{
	assert(	bitFunc == BIT_FUNC_BT || bitFunc == BIT_FUNC_BTS ||
			bitFunc == BIT_FUNC_BTR || bitFunc == BIT_FUNC_BTC);
	assert(Rd == REG_EAX || Rd == REG_ECX || Rd == REG_EDX || Rd == REG_EBX || Rd == REG_ESI || Rd == REG_EDI);

	hostCode.push_back(0x0f);
	hostCode.push_back(0xba);
	hostCode.push_back(0xc0 | static_cast<Byte>((bitFunc + 4) << 3) | static_cast<Byte>(Rd));
	hostCode.push_back(static_cast<Byte>(Ib));

	return 3;
}

void EmitBIT_Rw_Gw(std::vector <Byte> & hostCode, Dword bitFunc, Dword Rw, Dword Gw)
{
	assert(	bitFunc == BIT_FUNC_BT || bitFunc == BIT_FUNC_BTS ||
			bitFunc == BIT_FUNC_BTR || bitFunc == BIT_FUNC_BTC);
	assert(Rw == REG_EAX || Rw == REG_ECX || Rw == REG_EDX || Rw == REG_EBX || Rw == REG_ESI || Rw == REG_EDI);
	assert(Gw == REG_EAX || Gw == REG_ECX || Gw == REG_EDX || Gw == REG_EBX || Gw == REG_ESI || Gw == REG_EDI);

	hostCode.push_back(0x66);
	hostCode.push_back(0x0f);
	hostCode.push_back(0xa3 + static_cast<Byte>(bitFunc << 3));
	hostCode.push_back(0xc0 | static_cast<Byte>(Gw << 3) | static_cast<Byte>(Rw));
}

void EmitBIT_Rd_Gd(std::vector <Byte> & hostCode, Dword bitFunc, Dword Rd, Dword Gd)
{
	assert(	bitFunc == BIT_FUNC_BT || bitFunc == BIT_FUNC_BTS ||
			bitFunc == BIT_FUNC_BTR || bitFunc == BIT_FUNC_BTC);
	assert(Rd == REG_EAX || Rd == REG_ECX || Rd == REG_EDX || Rd == REG_EBX || Rd == REG_ESI || Rd == REG_EDI);
	assert(Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX || Gd == REG_ESI || Gd == REG_EDI);

	hostCode.push_back(0x0f);
	hostCode.push_back(0xa3 + static_cast<Byte>(bitFunc << 3));
	hostCode.push_back(0xc0 | static_cast<Byte>(Gd << 3) | static_cast<Byte>(Rd));
}

void EmitBSWAP_Rd(std::vector <Byte> & hostCode, Dword Rd)
{
	assert(Rd == REG_EAX || Rd == REG_ECX || Rd == REG_EDX || Rd == REG_EBX || Rd == REG_ESI || Rd == REG_EDI);

	hostCode.push_back(0x0f);
	hostCode.push_back(0xc8 | static_cast<Byte>(Rd));
}

void EmitBITSCAN_Gw_Rw(std::vector <Byte> & hostCode, Dword bitScanFunc, Dword Gw, Dword Rw)
{
	assert(bitScanFunc == BIT_SCAN_FUNC_BSF || bitScanFunc == BIT_SCAN_FUNC_BSR);
	assert(Gw == REG_EAX || Gw == REG_ECX || Gw == REG_EDX || Gw == REG_EBX || Gw == REG_ESI || Gw == REG_EDI);
	assert(Rw == REG_EAX || Rw == REG_ECX || Rw == REG_EDX || Rw == REG_EBX || Rw == REG_ESI || Rw == REG_EDI);

	hostCode.push_back(0x66);
	hostCode.push_back(0x0f);
	hostCode.push_back(0xbc | static_cast<Byte>(bitScanFunc));
	hostCode.push_back(0xc0 | static_cast<Byte>(Gw << 3) | static_cast<Byte>(Rw));
}

void EmitBITSCAN_Gw_Mw(std::vector <Byte> & hostCode, Dword bitScanFunc, Dword Gw,
					   Dword base, Dword index, Dword shift, Dword disp)
{
	assert(bitScanFunc == BIT_SCAN_FUNC_BSF || bitScanFunc == BIT_SCAN_FUNC_BSR);
	assert(Gw == REG_EAX || Gw == REG_ECX || Gw == REG_EDX || Gw == REG_EBX || Gw == REG_ESI || Gw == REG_EDI);

	hostCode.push_back(0x66);
	hostCode.push_back(0x0f);
	hostCode.push_back(0xbc | static_cast<Byte>(bitScanFunc));
	EmitMemoryModRM(hostCode, Gw, base, index, shift, disp);
}

void EmitBITSCAN_Gd_Rd(std::vector <Byte> & hostCode, Dword bitScanFunc, Dword Gd, Dword Rd)
{
	assert(bitScanFunc == BIT_SCAN_FUNC_BSF || bitScanFunc == BIT_SCAN_FUNC_BSR);
	assert(Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX || Gd == REG_ESI || Gd == REG_EDI);
	assert(Rd == REG_EAX || Rd == REG_ECX || Rd == REG_EDX || Rd == REG_EBX || Rd == REG_ESI || Rd == REG_EDI);

	hostCode.push_back(0x0f);
	hostCode.push_back(0xbc | static_cast<Byte>(bitScanFunc));
	hostCode.push_back(0xc0 | static_cast<Byte>(Gd << 3) | static_cast<Byte>(Rd));
}

void EmitBITSCAN_Gd_Md(std::vector <Byte> & hostCode, Dword bitScanFunc, Dword Gd,
					   Dword base, Dword index, Dword shift, Dword disp)
{
	assert(bitScanFunc == BIT_SCAN_FUNC_BSF || bitScanFunc == BIT_SCAN_FUNC_BSR);
	assert(Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX || Gd == REG_ESI || Gd == REG_EDI);

	hostCode.push_back(0x0f);
	hostCode.push_back(0xbc | static_cast<Byte>(bitScanFunc));
	EmitMemoryModRM(hostCode, Gd, base, index, shift, disp);
}

void EmitASAdjust(std::vector <Byte> & hostCode, Dword asAdjustFunc)
{
	assert(	asAdjustFunc == AS_ADJUST_FUNC_DAA || asAdjustFunc == AS_ADJUST_FUNC_DAS ||
			asAdjustFunc == AS_ADJUST_FUNC_AAA || asAdjustFunc == AS_ADJUST_FUNC_AAS);

	hostCode.push_back(0x27 + static_cast<Byte>(asAdjustFunc << 3));
}

void EmitMDAdjust(std::vector <Byte> & hostCode, Dword mdAdjustFunc, Dword Ib)
{
	assert(mdAdjustFunc == MD_ADJUST_FUNC_AAM || mdAdjustFunc == MD_ADJUST_FUNC_AAD);

	hostCode.push_back(0xd4 + static_cast<Byte>(mdAdjustFunc));
	hostCode.push_back(static_cast<Byte>(Ib));
}

void EmitLEA_Gd_M(	std::vector <Byte> & hostCode, Dword Gd, Dword base, Dword index,
					Dword shift, Dword disp)
{
	assert(Gd == REG_EAX || Gd == REG_ECX || Gd == REG_EDX || Gd == REG_EBX || Gd == REG_ESI || Gd == REG_EDI);

	hostCode.push_back(0x8d);
	EmitMemoryModRM(hostCode, Gd, base, index, shift, disp);
}

void EmitWAIT(std::vector <Byte> & hostCode)
{
	hostCode.push_back(0x9b);
}

void EmitRegisterFPU(std::vector <Byte> & hostCode, Dword fpuFunc)
{
	hostCode.push_back(0xd8 | static_cast<Byte>(fpuFunc >> 8));
	hostCode.push_back(static_cast<Byte>(fpuFunc));
}

void EmitMemoryFPU(	std::vector <Byte> & hostCode, Dword fpuFunc, Dword base, Dword index,
					Dword shift, Dword disp)
{
	hostCode.push_back(0xd8 | static_cast<Byte>(fpuFunc >> 8));
	EmitMemoryModRM(hostCode, static_cast<Byte>((fpuFunc & 0x38) >> 3), base, index,
					shift, disp);
}

void EmitCALL(std::vector <Byte> & hostCode)
{
	hostCode.push_back(0xe8);
	hostCode.push_back(0x00);
	hostCode.push_back(0x00);
	hostCode.push_back(0x00);
	hostCode.push_back(0x00);
}

void EmitJMP(std::vector <Byte> & hostCode)
{
	hostCode.push_back(0xe9);
	hostCode.push_back(0x00);
	hostCode.push_back(0x00);
	hostCode.push_back(0x00);
	hostCode.push_back(0x00);
}

void EmitJCond(std::vector <Byte> & hostCode, Dword cond, SignedByte displacement)
{
	assert(	cond < 0x10);

	hostCode.push_back(0x70 | static_cast<Byte>(cond));
	hostCode.push_back(displacement);
}

void EmitSAHF(std::vector <Byte> & hostCode)
{
	hostCode.push_back(0x9e);
}

void EmitLAHF(std::vector <Byte> & hostCode)
{
	hostCode.push_back(0x9f);
}

