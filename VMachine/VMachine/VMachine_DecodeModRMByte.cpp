//	VMachine
//	Decode a ModR/M byte
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"

bool VMachine::DecodeModRMByteRegReg(Dword & r_eip, Dword & Gd, Dword & Rd)
{
	Byte modRM;

	if(!ReadCodeByte(r_eip++, modRM))
		return false;

	assert((modRM & 0xc0) == 0xc0);

	switch((modRM & 0x38) >> 3)
	{
	case 0: Gd = REG_EAX; break;
	case 1: Gd = REG_ECX; break;
	case 2: Gd = REG_EDX; break;
	case 3: Gd = REG_EBX; break;
	case 4: Gd = REG_ESP; break;
	case 5: Gd = REG_EBP; break;
	case 6: Gd = REG_ESI; break;
	case 7: Gd = REG_EDI; break;
	}

	switch(modRM & 0x07)
	{
	case 0: Rd = REG_EAX; break;
	case 1: Rd = REG_ECX; break;
	case 2: Rd = REG_EDX; break;
	case 3: Rd = REG_EBX; break;
	case 4: Rd = REG_ESP; break;
	case 5: Rd = REG_EBP; break;
	case 6: Rd = REG_ESI; break;
	case 7: Rd = REG_EDI; break;
	}

	return true;
}

bool VMachine::DecodeModRMByteRegMemoryA16(	Dword & r_eip, Dword & Gd, Dword & segRegBase,
											Dword & baseReg, Dword & indexReg,
											Dword & displacement)
{
	Byte modRM;

	if(!ReadCodeByte(r_eip++, modRM))
		return false;

	assert((modRM & 0xc0) != 0xc0);

	switch((modRM & 0x38) >> 3)
	{
	case 0: Gd = REG_EAX; break;
	case 1: Gd = REG_ECX; break;
	case 2: Gd = REG_EDX; break;
	case 3: Gd = REG_EBX; break;
	case 4: Gd = REG_ESP; break;
	case 5: Gd = REG_EBP; break;
	case 6: Gd = REG_ESI; break;
	case 7: Gd = REG_EDI; break;
	}

	segRegBase = baseReg = indexReg = REG_NONE;
	displacement = 0;

	if((modRM & 0xc7) == 0x06)
	{
		segRegBase = REG_DS_BASE;

		Word disp16;
		if(!ReadCodeWord(r_eip, disp16))
			return false;

		r_eip += 2;

		displacement = static_cast<SignedWord>(disp16);
	}
	else
	{
		switch(modRM & 0x07)
		{
		case 0: segRegBase = REG_DS_BASE; baseReg = REG_EBX; indexReg = REG_ESI; break;
		case 1: segRegBase = REG_DS_BASE; baseReg = REG_EBX; indexReg = REG_EDI; break;
		case 2: segRegBase = REG_SS_BASE; baseReg = REG_EBP; indexReg = REG_ESI; break;
		case 3: segRegBase = REG_SS_BASE; baseReg = REG_EBP; indexReg = REG_EDI; break;
		case 4: segRegBase = REG_DS_BASE; baseReg = REG_ESI; break;
		case 5: segRegBase = REG_DS_BASE; baseReg = REG_EDI; break;
		case 6: segRegBase = REG_SS_BASE; baseReg = REG_EBP; break;
		case 7: segRegBase = REG_DS_BASE; baseReg = REG_EBX; break;
		}

		if((modRM & 0xc0) == 0x40)
		{
			Byte disp8;

			if(!ReadCodeByte(r_eip++, disp8))
				return false;

			displacement = static_cast<SignedByte>(disp8);
		}

		if((modRM & 0xc0) == 0x80)
		{
			Word disp16;
			if(!ReadCodeWord(r_eip, disp16))
				return false;

			r_eip += 2;

			displacement = static_cast<SignedWord>(disp16);
		}
	}

	assert(segRegBase != REG_NONE);

	return true;
}

bool VMachine::DecodeModRMByteRegMemoryA32(	Dword & r_eip, Dword & Gd, Dword & segRegBase,
											Dword & baseReg, Dword & indexReg, Dword & shift,
											Dword & displacement)
{
	Byte modRM;

	if(!ReadCodeByte(r_eip++, modRM))
		return false;

	assert((modRM & 0xc0) != 0xc0);

	switch((modRM & 0x38) >> 3)
	{
	case 0: Gd = REG_EAX; break;
	case 1: Gd = REG_ECX; break;
	case 2: Gd = REG_EDX; break;
	case 3: Gd = REG_EBX; break;
	case 4: Gd = REG_ESP; break;
	case 5: Gd = REG_EBP; break;
	case 6: Gd = REG_ESI; break;
	case 7: Gd = REG_EDI; break;
	}

	segRegBase = baseReg = indexReg = REG_NONE;
	shift = 0;
	displacement = 0;

	if((modRM & 0xc7) == 0x05)
	{
		segRegBase = REG_DS_BASE;
		
		if(!ReadCodeDword(r_eip, displacement))
			return false;
		
		r_eip += 4;
	}
	else if((modRM & 0x07) != 0x04)
	{
		switch(modRM & 0x07)
		{
		case 0: segRegBase = REG_DS_BASE; baseReg = REG_EAX; break;
		case 1: segRegBase = REG_DS_BASE; baseReg = REG_ECX; break;
		case 2: segRegBase = REG_DS_BASE; baseReg = REG_EDX; break;
		case 3: segRegBase = REG_DS_BASE; baseReg = REG_EBX; break;
		case 5: segRegBase = REG_SS_BASE; baseReg = REG_EBP; break;
		case 6: segRegBase = REG_DS_BASE; baseReg = REG_ESI; break;
		case 7: segRegBase = REG_DS_BASE; baseReg = REG_EDI; break;
		}

		if((modRM & 0xc0) == 0x40)
		{
			Byte disp8;

			if(!ReadCodeByte(r_eip++, disp8))
				return false;

			displacement = static_cast<SignedByte>(disp8);
		}

		if((modRM & 0xc0) == 0x80)
		{
			if(!ReadCodeDword(r_eip, displacement))
				return false;

			r_eip += 4;
		}
	}
	else
	{
		Byte sib;
		if(!ReadCodeByte(r_eip++, sib))
			return false;

		shift = sib >> 6;

        switch((sib & 0x38) >> 3)
		{
		case 0: indexReg = REG_EAX; break;
		case 1: indexReg = REG_ECX; break;
		case 2: indexReg = REG_EDX; break;
		case 3: indexReg = REG_EBX; break;
		case 5: indexReg = REG_EBP; break;
		case 6: indexReg = REG_ESI; break;
		case 7: indexReg = REG_EDI; break;
		}

		switch(sib & 0x07)
		{
		case 0: segRegBase = REG_DS_BASE; baseReg = REG_EAX; break;
		case 1: segRegBase = REG_DS_BASE; baseReg = REG_ECX; break;
		case 2: segRegBase = REG_DS_BASE; baseReg = REG_EDX; break;
		case 3: segRegBase = REG_DS_BASE; baseReg = REG_EBX; break;
		case 4: segRegBase = REG_SS_BASE; baseReg = REG_ESP; break;
		case 5:
			if((modRM & 0xc0) == 0x00)
				segRegBase = REG_DS_BASE;
			else
			{
				segRegBase = REG_SS_BASE;
				baseReg = REG_EBP;
			}
			break;
		case 6: segRegBase = REG_DS_BASE; baseReg = REG_ESI; break;
		case 7: segRegBase = REG_DS_BASE; baseReg = REG_EDI; break;
		}

		if((modRM & 0xc0) == 0x40)
		{
			Byte disp8;

			if(!ReadCodeByte(r_eip++, disp8))
				return false;

			displacement = static_cast<SignedByte>(disp8);
		}

		if((modRM & 0xc0) == 0x80 || ((modRM & 0xc0) == 0x00 && (sib & 0x07) == 0x05))
		{
			if(!ReadCodeDword(r_eip, displacement))
				return false;

			r_eip += 4;
		}
	}

	assert(segRegBase != REG_NONE);

	return true;
}

bool VMachine::DecodeModRMByteA16(	Dword r_eip, Byte & modRM,
									SegmentRegister *& segReg, Dword *& baseReg, Dword *& indexReg, Dword & displacement,
									Byte *& Rb, Byte *& Gb, Word *& Rw, Word *& Gw, Dword *& Rd, Dword *& Gd, SegmentRegisterData *& Sw,
									Dword & modRMSize)
{
	Dword startEip = r_eip;

	if(!ReadCodeByte(r_eip++, modRM))
		return false;

	switch((modRM & 0x38) >> 3)
	{
	case 0: Gb = &registers->r_al; Gw = &registers->r_ax; Gd = &registers->r_eax; Sw = &registers->r_es; break;
	case 1: Gb = &registers->r_cl; Gw = &registers->r_cx; Gd = &registers->r_ecx; Sw = &registers->r_cs; break;
	case 2: Gb = &registers->r_dl; Gw = &registers->r_dx; Gd = &registers->r_edx; Sw = &registers->r_ss; break;
	case 3: Gb = &registers->r_bl; Gw = &registers->r_bx; Gd = &registers->r_ebx; Sw = &registers->r_ds; break;
	case 4: Gb = &registers->r_ah; Gw = &registers->r_sp; Gd = &registers->r_esp; Sw = &registers->r_fs; break;
	case 5: Gb = &registers->r_ch; Gw = &registers->r_bp; Gd = &registers->r_ebp; Sw = &registers->r_gs; break;
	case 6: Gb = &registers->r_dh; Gw = &registers->r_si; Gd = &registers->r_esi; Sw = &registers->r_ds; break;//Sw resvd
	case 7: Gb = &registers->r_bh; Gw = &registers->r_di; Gd = &registers->r_edi; Sw = &registers->r_ds; break;//Sw resvd
	}

	if((modRM & 0xc0) == 0xc0)
	{
		switch(modRM & 0x07)
		{
		case 0: Rb = &registers->r_al; Rw = &registers->r_ax; Rd = &registers->r_eax; break;
		case 1: Rb = &registers->r_cl; Rw = &registers->r_cx; Rd = &registers->r_ecx; break;
		case 2: Rb = &registers->r_dl; Rw = &registers->r_dx; Rd = &registers->r_edx; break;
		case 3: Rb = &registers->r_bl; Rw = &registers->r_bx; Rd = &registers->r_ebx; break;
		case 4: Rb = &registers->r_ah; Rw = &registers->r_sp; Rd = &registers->r_esp; break;
		case 5: Rb = &registers->r_ch; Rw = &registers->r_bp; Rd = &registers->r_ebp; break;
		case 6: Rb = &registers->r_dh; Rw = &registers->r_si; Rd = &registers->r_esi; break;
		case 7: Rb = &registers->r_bh; Rw = &registers->r_di; Rd = &registers->r_edi; break;
		}
	}
	else
	{
		if((modRM & 0xc7) == 0x06)
		{
			segReg = &r_ds;

			Word disp16;
			if(!ReadCodeWord(r_eip, disp16))
				return false;

			r_eip += 2;

			displacement = static_cast<SignedWord>(disp16);
		}
		else
		{
			switch(modRM & 0x07)
			{
			case 0: segReg = &r_ds; baseReg = &registers->r_ebx;
					indexReg = &registers->r_esi; break;
			case 1: segReg = &r_ds; baseReg = &registers->r_ebx;
					indexReg = &registers->r_edi; break;
			case 2: segReg = &r_ss; baseReg = &registers->r_ebp;
					indexReg = &registers->r_esi; break;
			case 3: segReg = &r_ss; baseReg = &registers->r_ebp;
					indexReg = &registers->r_edi; break;
			case 4: segReg = &r_ds; baseReg = &registers->r_esi; break;
			case 5: segReg = &r_ds; baseReg = &registers->r_edi; break;
			case 6: segReg = &r_ss; baseReg = &registers->r_ebp; break;
			case 7: segReg = &r_ds; baseReg = &registers->r_ebx; break;
			}

			if((modRM & 0xc0) == 0x40)
			{
				Byte disp8;

				if(!ReadCodeByte(r_eip++, disp8))
					return false;

				displacement = static_cast<SignedByte>(disp8);
			}

			if((modRM & 0xc0) == 0x80)
			{
				Word disp16;
				if(!ReadCodeWord(r_eip, disp16))
					return false;

				r_eip += 2;

				displacement = static_cast<SignedWord>(disp16);
			}
		}
	}

	modRMSize = r_eip - startEip;
	return true;
}

bool VMachine::DecodeModRMByteA32(	Dword r_eip, Byte & modRM,
									SegmentRegister *& segReg, Dword *& baseReg, Dword *& indexReg, Dword & indexShift, Dword & displacement,
									Byte *& Rb, Byte *& Gb, Word *& Rw, Word *& Gw, Dword *& Rd, Dword *& Gd, SegmentRegisterData *& Sw,
									Dword & modRMSize)
{
	Dword startEip = r_eip;

	if(!ReadCodeByte(r_eip++, modRM))
		return false;

	switch((modRM & 0x38) >> 3)
	{
	case 0: Gb = &registers->r_al; Gw = &registers->r_ax; Gd = &registers->r_eax; Sw = &registers->r_es; break;
	case 1: Gb = &registers->r_cl; Gw = &registers->r_cx; Gd = &registers->r_ecx; Sw = &registers->r_cs; break;
	case 2: Gb = &registers->r_dl; Gw = &registers->r_dx; Gd = &registers->r_edx; Sw = &registers->r_ss; break;
	case 3: Gb = &registers->r_bl; Gw = &registers->r_bx; Gd = &registers->r_ebx; Sw = &registers->r_ds; break;
	case 4: Gb = &registers->r_ah; Gw = &registers->r_sp; Gd = &registers->r_esp; Sw = &registers->r_fs; break;
	case 5: Gb = &registers->r_ch; Gw = &registers->r_bp; Gd = &registers->r_ebp; Sw = &registers->r_gs; break;
	case 6: Gb = &registers->r_dh; Gw = &registers->r_si; Gd = &registers->r_esi; Sw = &registers->r_ds; break;//Sw resvd
	case 7: Gb = &registers->r_bh; Gw = &registers->r_di; Gd = &registers->r_edi; Sw = &registers->r_ds; break;//Sw resvd
	}

	if((modRM & 0xc0) == 0xc0)
	{
		switch(modRM & 0x07)
		{
		case 0: Rb = &registers->r_al; Rw = &registers->r_ax; Rd = &registers->r_eax; break;
		case 1: Rb = &registers->r_cl; Rw = &registers->r_cx; Rd = &registers->r_ecx; break;
		case 2: Rb = &registers->r_dl; Rw = &registers->r_dx; Rd = &registers->r_edx; break;
		case 3: Rb = &registers->r_bl; Rw = &registers->r_bx; Rd = &registers->r_ebx; break;
		case 4: Rb = &registers->r_ah; Rw = &registers->r_sp; Rd = &registers->r_esp; break;
		case 5: Rb = &registers->r_ch; Rw = &registers->r_bp; Rd = &registers->r_ebp; break;
		case 6: Rb = &registers->r_dh; Rw = &registers->r_si; Rd = &registers->r_esi; break;
		case 7: Rb = &registers->r_bh; Rw = &registers->r_di; Rd = &registers->r_edi; break;
		}
	}
	else
	{
		if((modRM & 0xc7) == 0x05)
		{
			segReg = &r_ds;
			
			if(!ReadCodeDword(r_eip, displacement))
				return false;
			
			r_eip += 4;
		}
		else if((modRM & 0x07) != 0x04)
		{
			switch(modRM & 0x07)
			{
			case 0: segReg = &r_ds; baseReg = &registers->r_eax; break;
			case 1: segReg = &r_ds; baseReg = &registers->r_ecx; break;
			case 2: segReg = &r_ds; baseReg = &registers->r_edx; break;
			case 3: segReg = &r_ds; baseReg = &registers->r_ebx; break;
			case 5: segReg = &r_ss; baseReg = &registers->r_ebp; break;
			case 6: segReg = &r_ds; baseReg = &registers->r_esi; break;
			case 7: segReg = &r_ds; baseReg = &registers->r_edi; break;
			}

			if((modRM & 0xc0) == 0x40)
			{
				Byte disp8;

				if(!ReadCodeByte(r_eip++, disp8))
					return false;

				displacement = static_cast<SignedByte>(disp8);
			}

			if((modRM & 0xc0) == 0x80)
			{
				if(!ReadCodeDword(r_eip, displacement))
					return false;

				r_eip += 4;
			}
		}
		else
		{
			Byte sib;
			if(!ReadCodeByte(r_eip++, sib))
				return false;

			indexShift = sib >> 6;

            switch((sib & 0x38) >> 3)
			{
			case 0: indexReg = &registers->r_eax; break;
			case 1: indexReg = &registers->r_ecx; break;
			case 2: indexReg = &registers->r_edx; break;
			case 3: indexReg = &registers->r_ebx; break;
			case 5: indexReg = &registers->r_ebp; break;
			case 6: indexReg = &registers->r_esi; break;
			case 7: indexReg = &registers->r_edi; break;
			}

			switch(sib & 0x07)
			{
			case 0: segReg = &r_ds; baseReg = &registers->r_eax; break;
			case 1: segReg = &r_ds; baseReg = &registers->r_ecx; break;
			case 2: segReg = &r_ds; baseReg = &registers->r_edx; break;
			case 3: segReg = &r_ds; baseReg = &registers->r_ebx; break;
			case 4: segReg = &r_ss; baseReg = &registers->r_esp; break;
			case 5:
				if((modRM & 0xc0) == 0x00)
					segReg = &r_ds;
				else
				{
					segReg = &r_ss;
					baseReg = &registers->r_ebp;
				}
				break;
			case 6: segReg = &r_ds; baseReg = &registers->r_esi; break;
			case 7: segReg = &r_ds; baseReg = &registers->r_edi; break;
			}

			if((modRM & 0xc0) == 0x40)
			{
				Byte disp8;

				if(!ReadCodeByte(r_eip++, disp8))
					return false;

				displacement = static_cast<SignedByte>(disp8);
			}

			if((modRM & 0xc0) == 0x80 || ((modRM & 0xc0) == 0x00 && (sib & 0x07) == 0x05))
			{
				if(!ReadCodeDword(r_eip, displacement))
					return false;

				r_eip += 4;
			}
		}
	}

	modRMSize = r_eip - startEip;
	return true;
}

bool VMachine::DecodeModRMByteRegRegWord(Dword & r_eip, Word *& reg1, Word *& reg2)
{
	Byte modRM;
	if(!ReadCodeByte(r_eip++, modRM))
		return false;

	assert((modRM & 0xc0) == 0xc0);

	switch((modRM & 0x38) >> 3)
	{
	case 0: reg1 = &registers->r_ax; break;
	case 1: reg1 = &registers->r_cx; break;
	case 2: reg1 = &registers->r_dx; break;
	case 3: reg1 = &registers->r_bx; break;
	case 4: reg1 = &registers->r_sp; break;
	case 5: reg1 = &registers->r_bp; break;
	case 6: reg1 = &registers->r_si; break;
	case 7: reg1 = &registers->r_di; break;
	}

	switch(modRM & 0x07)
	{
	case 0: reg2 = &registers->r_ax; break;
	case 1: reg2 = &registers->r_cx; break;
	case 2: reg2 = &registers->r_dx; break;
	case 3: reg2 = &registers->r_bx; break;
	case 4: reg2 = &registers->r_sp; break;
	case 5: reg2 = &registers->r_bp; break;
	case 6: reg2 = &registers->r_si; break;
	case 7: reg2 = &registers->r_di; break;
	}

	return true;
}

bool VMachine::DecodeModRMByteSegRegRegWord(Dword & r_eip, SegmentRegister *& segReg, Word *& reg)
{
	Byte modRM;
	if(!ReadCodeByte(r_eip++, modRM))
		return false;

	assert((modRM & 0xc0) == 0xc0);

	switch((modRM & 0x38) >> 3)
	{
	case 0: segReg = &r_es; break;
	case 1: segReg = &r_cs; break;
	case 2: segReg = &r_ss; break;
	case 3: segReg = &r_ds; break;
	case 4: segReg = &r_fs; break;
	case 5: segReg = &r_gs; break;
	case 6: segReg = &r_ds; break;	//segReg reserved
	case 7: segReg = &r_ds; break;	//segReg reserved
	}

	switch(modRM & 0x07)
	{
	case 0: reg = &registers->r_ax; break;
	case 1: reg = &registers->r_cx; break;
	case 2: reg = &registers->r_dx; break;
	case 3: reg = &registers->r_bx; break;
	case 4: reg = &registers->r_sp; break;
	case 5: reg = &registers->r_bp; break;
	case 6: reg = &registers->r_si; break;
	case 7: reg = &registers->r_di; break;
	}

	return true;
}

bool VMachine::DecodeModRMByteRegRegDword(Dword & r_eip, Dword *& reg1, Dword *& reg2)
{
	Byte modRM;
	if(!ReadCodeByte(r_eip++, modRM))
		return false;

	assert((modRM & 0xc0) == 0xc0);

	switch((modRM & 0x38) >> 3)
	{
	case 0: reg1 = &registers->r_eax; break;
	case 1: reg1 = &registers->r_ecx; break;
	case 2: reg1 = &registers->r_edx; break;
	case 3: reg1 = &registers->r_ebx; break;
	case 4: reg1 = &registers->r_esp; break;
	case 5: reg1 = &registers->r_ebp; break;
	case 6: reg1 = &registers->r_esi; break;
	case 7: reg1 = &registers->r_edi; break;
	}

	switch(modRM & 0x07)
	{
	case 0: reg2 = &registers->r_eax; break;
	case 1: reg2 = &registers->r_ecx; break;
	case 2: reg2 = &registers->r_edx; break;
	case 3: reg2 = &registers->r_ebx; break;
	case 4: reg2 = &registers->r_esp; break;
	case 5: reg2 = &registers->r_ebp; break;
	case 6: reg2 = &registers->r_esi; break;
	case 7: reg2 = &registers->r_edi; break;
	}

	return true;
}

bool VMachine::DecodeModRMByteRegMemoryWord(Dword & r_eip, Dword opcode, Word *& reg1,
											SegmentRegister *& defaultSegReg, Dword & offset)
{
	if((opcode & OPCODE_A32) == 0)
		return DecodeModRMByteRegMemoryA16Word(r_eip, reg1, defaultSegReg, offset);
	else
		return DecodeModRMByteRegMemoryA32Word(r_eip, reg1, defaultSegReg, offset);
}

bool VMachine::DecodeModRMByteSegRegMemoryWord(	Dword & r_eip, Dword opcode, SegmentRegister *& segReg,
												SegmentRegister *& defaultSegReg, Dword & offset)
{
	Word * reg1;

	if((opcode & OPCODE_A32) == 0)
	{
		if(!DecodeModRMByteRegMemoryA16Word(r_eip, reg1, defaultSegReg, offset))
			return false;
	}
	else
	{
		if(!DecodeModRMByteRegMemoryA32Word(r_eip, reg1, defaultSegReg, offset))
			return false;
	}

	segReg = 0;

	if(reg1 == &registers->r_ax || reg1 == &registers->r_si || reg1 == &registers->r_di)
		segReg = &r_es;

	if(reg1 == &registers->r_cx)
		segReg = &r_cs;

	if(reg1 == &registers->r_dx)
		segReg = &r_ss;

	if(reg1 == &registers->r_bx)
		segReg = &r_ds;

	if(reg1 == &registers->r_sp)
		segReg = &r_fs;

	if(reg1 == &registers->r_bp)
		segReg = &r_gs;

	assert(segReg);

	return true;
}

bool VMachine::DecodeModRMByteRegMemoryDword(	Dword & r_eip, Dword opcode, Dword *& reg1,
												SegmentRegister *& defaultSegReg, Dword & offset)
{
	if((opcode & OPCODE_A32) == 0)
		return DecodeModRMByteRegMemoryA16Dword(r_eip, reg1, defaultSegReg, offset);
	else
		return DecodeModRMByteRegMemoryA32Dword(r_eip, reg1, defaultSegReg, offset);
}

bool VMachine::DecodeModRMByteRegMemoryA16Word(	Dword & r_eip, Word *& reg1,
												SegmentRegister *& defaultSegReg, Dword & offset)
{
	Byte modRM;
	if(!ReadByte(r_cs, r_eip++, modRM))
		return false;

	assert((modRM & 0xc0) != 0xc0);

	switch((modRM & 0x38) >> 3)
	{
	case 0: reg1 = &registers->r_ax; break;
	case 1: reg1 = &registers->r_cx; break;
	case 2: reg1 = &registers->r_dx; break;
	case 3: reg1 = &registers->r_bx; break;
	case 4: reg1 = &registers->r_sp; break;
	case 5: reg1 = &registers->r_bp; break;
	case 6: reg1 = &registers->r_si; break;
	case 7: reg1 = &registers->r_di; break;
	}

	if((modRM & 0xc7) == 0x06)
	{
		defaultSegReg = &r_ds;

		Word disp16;
		if(!ReadWord(r_cs, r_eip, disp16))
			return false;

		r_eip += 2;
		offset = disp16;
	}
	else
	{
		switch(modRM & 0x07)
		{
		case 0: defaultSegReg = &r_ds; offset = registers->r_bx + registers->r_si; break;
		case 1: defaultSegReg = &r_ds; offset = registers->r_bx + registers->r_di; break;
		case 2: defaultSegReg = &r_ss; offset = registers->r_bp + registers->r_si; break;
		case 3: defaultSegReg = &r_ss; offset = registers->r_bp + registers->r_di; break;
		case 4: defaultSegReg = &r_ds; offset = registers->r_si; break;
		case 5: defaultSegReg = &r_ds; offset = registers->r_di; break;
		case 6: defaultSegReg = &r_ss; offset = registers->r_bp; break;
		case 7: defaultSegReg = &r_ds; offset = registers->r_bx; break;
		}

		if((modRM & 0xc0) == 0x40)
		{
			Byte disp8;
			if(!ReadByte(r_cs, r_eip++, disp8))
				return false;

			offset += static_cast<SignedByte>(disp8);
		}

		if((modRM & 0xc0) == 0x80)
		{
			Word disp16;
			if(!ReadWord(r_cs, r_eip, disp16))
				return false;

			r_eip += 2;
			offset += static_cast<SignedWord>(disp16);
		}

		offset &= 0xffff;
	}
	
	return true;
}

bool VMachine::DecodeModRMByteRegMemoryA16Dword(Dword & r_eip, Dword *& reg1,
												SegmentRegister *& defaultSegReg, Dword & offset)
{
	Byte modRM;
	if(!ReadByte(r_cs, r_eip++, modRM))
		return false;

	assert((modRM & 0xc0) != 0xc0);

	switch((modRM & 0x38) >> 3)
	{
	case 0: reg1 = &registers->r_eax; break;
	case 1: reg1 = &registers->r_ecx; break;
	case 2: reg1 = &registers->r_edx; break;
	case 3: reg1 = &registers->r_ebx; break;
	case 4: reg1 = &registers->r_esp; break;
	case 5: reg1 = &registers->r_ebp; break;
	case 6: reg1 = &registers->r_esi; break;
	case 7: reg1 = &registers->r_edi; break;
	}

	if((modRM & 0xc7) == 0x06)
	{
		defaultSegReg = &r_ds;

		Word disp16;
		if(!ReadWord(r_cs, r_eip, disp16))
			return false;

		r_eip += 2;
		offset = disp16;
	}
	else
	{
		switch(modRM & 0x07)
		{
		case 0: defaultSegReg = &r_ds; offset = registers->r_bx + registers->r_si; break;
		case 1: defaultSegReg = &r_ds; offset = registers->r_bx + registers->r_di; break;
		case 2: defaultSegReg = &r_ss; offset = registers->r_bp + registers->r_si; break;
		case 3: defaultSegReg = &r_ss; offset = registers->r_bp + registers->r_di; break;
		case 4: defaultSegReg = &r_ds; offset = registers->r_si; break;
		case 5: defaultSegReg = &r_ds; offset = registers->r_di; break;
		case 6: defaultSegReg = &r_ss; offset = registers->r_bp; break;
		case 7: defaultSegReg = &r_ds; offset = registers->r_bx; break;
		}

		if((modRM & 0xc0) == 0x40)
		{
			Byte disp8;
			if(!ReadByte(r_cs, r_eip++, disp8))
				return false;

			offset += static_cast<SignedByte>(disp8);
		}

		if((modRM & 0xc0) == 0x80)
		{
			Word disp16;
			if(!ReadWord(r_cs, r_eip, disp16))
				return false;

			r_eip += 2;
			offset += static_cast<SignedWord>(disp16);
		}

		offset &= 0xffff;
	}
	
	return true;
}

bool VMachine::DecodeModRMByteRegMemoryA32Word(	Dword & r_eip, Word *& reg1,
												SegmentRegister *& defaultSegReg, Dword & offset)
{
	Byte modRM;
	if(!ReadByte(r_cs, r_eip++, modRM))
		return false;

	assert((modRM & 0xc0) != 0xc0);

	switch((modRM & 0x38) >> 3)
	{
	case 0: reg1 = &registers->r_ax; break;
	case 1: reg1 = &registers->r_cx; break;
	case 2: reg1 = &registers->r_dx; break;
	case 3: reg1 = &registers->r_bx; break;
	case 4: reg1 = &registers->r_sp; break;
	case 5: reg1 = &registers->r_bp; break;
	case 6: reg1 = &registers->r_si; break;
	case 7: reg1 = &registers->r_di; break;
	}

	if((modRM & 0xc7) == 0x05)
	{
		defaultSegReg = &r_ds;

		Dword displacement;
		if(!ReadDword(r_cs, registers->r_eip, displacement))
			return false;
		
		registers->r_eip += 4;

		offset = displacement;
	}
	else if((modRM & 0x07) != 0x04)
	{
		switch(modRM & 0x07)
		{
		case 0: defaultSegReg = &r_ds; offset = registers->r_eax; break;
		case 1: defaultSegReg = &r_ds; offset = registers->r_ecx; break;
		case 2: defaultSegReg = &r_ds; offset = registers->r_edx; break;
		case 3: defaultSegReg = &r_ds; offset = registers->r_ebx; break;
		case 5: defaultSegReg = &r_ss; offset = registers->r_ebp; break;
		case 6: defaultSegReg = &r_ds; offset = registers->r_esi; break;
		case 7: defaultSegReg = &r_ds; offset = registers->r_edi; break;
		}

		if((modRM & 0xc0) == 0x40)
		{
			Byte disp8;
			if(!ReadByte(r_cs, registers->r_eip++, disp8))
				return false;
			
			offset += static_cast<SignedByte>(disp8);
		}

		if((modRM & 0xc0) == 0x80)
		{
			Dword displacement;
			if(!ReadDword(r_cs, registers->r_eip, displacement))
				return false;
		
			registers->r_eip += 4;

			offset += displacement;
		}
	}
	else
	{
		Byte sib;
		if(!ReadByte(r_cs, registers->r_eip++, sib))
			return false;

		Byte indexShift = sib >> 6;
		Dword * indexReg = 0;

        switch((sib & 0x38) >> 3)
		{
		case 0: indexReg = &registers->r_eax; break;
		case 1: indexReg = &registers->r_ecx; break;
		case 2: indexReg = &registers->r_edx; break;
		case 3: indexReg = &registers->r_ebx; break;
		case 5: indexReg = &registers->r_ebp; break;
		case 6: indexReg = &registers->r_esi; break;
		case 7: indexReg = &registers->r_edi; break;
		}

		switch(sib & 0x07)
		{
		case 0: defaultSegReg = &r_ds; offset = registers->r_eax; break;
		case 1: defaultSegReg = &r_ds; offset = registers->r_ecx; break;
		case 2: defaultSegReg = &r_ds; offset = registers->r_edx; break;
		case 3: defaultSegReg = &r_ds; offset = registers->r_ebx; break;
		case 4: defaultSegReg = &r_ss; offset = registers->r_esp; break;
		case 5:
			if((modRM & 0xc0) == 0x00)
			{
				defaultSegReg = &r_ds;
				offset = 0;
			}
			else
			{
				defaultSegReg = &r_ss;
				offset = registers->r_ebp;
			}
			break;
		case 6: defaultSegReg = &r_ds; offset = registers->r_esi; break;
		case 7: defaultSegReg = &r_ds; offset = registers->r_edi; break;
		}

		if(indexReg)
			offset += *indexReg << indexShift;

		if((modRM & 0xc0) == 0x40)
		{
			Byte disp8;
			if(!ReadByte(r_cs, registers->r_eip++, disp8))
				return false;
			
			offset += static_cast<SignedByte>(disp8);
		}

		if((modRM & 0xc0) == 0x80 || ((modRM & 0xc0) == 0x00 && (sib & 0x07) == 0x05))
		{
			Dword displacement;
			if(!ReadDword(r_cs, registers->r_eip, displacement))
			return false;
		
			registers->r_eip += 4;

			offset += displacement;
		}
	}

	return true;
}

bool VMachine::DecodeModRMByteRegMemoryA32Dword(Dword & r_eip, Dword *& reg1,
												SegmentRegister *& defaultSegReg, Dword & offset)
{
	Byte modRM;
	if(!ReadByte(r_cs, r_eip++, modRM))
		return false;

	assert((modRM & 0xc0) != 0xc0);

	switch((modRM & 0x38) >> 3)
	{
	case 0: reg1 = &registers->r_eax; break;
	case 1: reg1 = &registers->r_ecx; break;
	case 2: reg1 = &registers->r_edx; break;
	case 3: reg1 = &registers->r_ebx; break;
	case 4: reg1 = &registers->r_esp; break;
	case 5: reg1 = &registers->r_ebp; break;
	case 6: reg1 = &registers->r_esi; break;
	case 7: reg1 = &registers->r_edi; break;
	}

	if((modRM & 0xc7) == 0x05)
	{
		defaultSegReg = &r_ds;

		Dword displacement;
		if(!ReadDword(r_cs, registers->r_eip, displacement))
			return false;
		
		registers->r_eip += 4;

		offset = displacement;
	}
	else if((modRM & 0x07) != 0x04)
	{
		switch(modRM & 0x07)
		{
		case 0: defaultSegReg = &r_ds; offset = registers->r_eax; break;
		case 1: defaultSegReg = &r_ds; offset = registers->r_ecx; break;
		case 2: defaultSegReg = &r_ds; offset = registers->r_edx; break;
		case 3: defaultSegReg = &r_ds; offset = registers->r_ebx; break;
		case 5: defaultSegReg = &r_ss; offset = registers->r_ebp; break;
		case 6: defaultSegReg = &r_ds; offset = registers->r_esi; break;
		case 7: defaultSegReg = &r_ds; offset = registers->r_edi; break;
		}

		if((modRM & 0xc0) == 0x40)
		{
			Byte disp8;
			if(!ReadByte(r_cs, registers->r_eip++, disp8))
				return false;
			
			offset += static_cast<SignedByte>(disp8);
		}

		if((modRM & 0xc0) == 0x80)
		{
			Dword displacement;
			if(!ReadDword(r_cs, registers->r_eip, displacement))
				return false;
		
			registers->r_eip += 4;

			offset += displacement;
		}
	}
	else
	{
		Byte sib;
		if(!ReadByte(r_cs, registers->r_eip++, sib))
			return false;

		Byte indexShift = sib >> 6;
		Dword * indexReg = 0;

        switch((sib & 0x38) >> 3)
		{
		case 0: indexReg = &registers->r_eax; break;
		case 1: indexReg = &registers->r_ecx; break;
		case 2: indexReg = &registers->r_edx; break;
		case 3: indexReg = &registers->r_ebx; break;
		case 5: indexReg = &registers->r_ebp; break;
		case 6: indexReg = &registers->r_esi; break;
		case 7: indexReg = &registers->r_edi; break;
		}

		switch(sib & 0x07)
		{
		case 0: defaultSegReg = &r_ds; offset = registers->r_eax; break;
		case 1: defaultSegReg = &r_ds; offset = registers->r_ecx; break;
		case 2: defaultSegReg = &r_ds; offset = registers->r_edx; break;
		case 3: defaultSegReg = &r_ds; offset = registers->r_ebx; break;
		case 4: defaultSegReg = &r_ss; offset = registers->r_esp; break;
		case 5:
			if((modRM & 0xc0) == 0x00)
			{
				defaultSegReg = &r_ds;
				offset = 0;
			}
			else
			{
				defaultSegReg = &r_ss;
				offset = registers->r_ebp;
			}
			break;
		case 6: defaultSegReg = &r_ds; offset = registers->r_esi; break;
		case 7: defaultSegReg = &r_ds; offset = registers->r_edi; break;
		}

		if(indexReg)
			offset += *indexReg << indexShift;

		if((modRM & 0xc0) == 0x40)
		{
			Byte disp8;
			if(!ReadByte(r_cs, registers->r_eip++, disp8))
				return false;
			
			offset += static_cast<SignedByte>(disp8);
		}

		if((modRM & 0xc0) == 0x80 || ((modRM & 0xc0) == 0x00 && (sib & 0x07) == 0x05))
		{
			Dword displacement;
			if(!ReadDword(r_cs, registers->r_eip, displacement))
			return false;
		
			registers->r_eip += 4;

			offset += displacement;
		}
	}

	return true;
}