//	VMachine
//	Load the instruction translations
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"
#include "ElfStructures.h"

namespace
{
	//Table used to convert instruction names to opcodes
	struct OpcodeOpname
	{
		const Dword opcode;
		const char * const opname;
	};

	const Dword A32 = OPCODE_A32;
	const Dword O32 = OPCODE_O32;
	const Dword REP = OPCODE_REP;
	const Dword REPNE = OPCODE_REPNE;

	const OpcodeOpname opcodeOpnames[] =
	{
#ifndef USE_MICROCODE
		{0x0000, "_OP_ADD_Mb_Gb_A16"},			{A32 | 0x0000, "_OP_ADD_Mb_Gb_A32"},
		{O32 | 0x0000, "_OP_ADD_Mb_Gb_A16"},	{A32 | O32 | 0x0000, "_OP_ADD_Mb_Gb_A32"},
		{0x00c0, "_OP_ADD_Rb_Gb"},				{A32 | 0x00c0, "_OP_ADD_Rb_Gb"},
		{O32 | 0x00c0, "_OP_ADD_Rb_Gb"},		{A32 | O32 | 0x00c0, "_OP_ADD_Rb_Gb"},
		{0x0100, "_OP_ADD_Mw_Gw_A16"},			{A32 | 0x0100, "_OP_ADD_Mw_Gw_A32"},
		{O32 | 0x0100, "_OP_ADD_Md_Gd_A16"},	{A32 | O32 | 0x0100, "_OP_ADD_Md_Gd_A32"},
		{0x01c0, "_OP_ADD_Rw_Gw"},				{A32 | 0x01c0, "_OP_ADD_Rw_Gw"},
		{O32 | 0x01c0, "_OP_ADD_Rd_Gd"},		{A32 | O32 | 0x01c0, "_OP_ADD_Rd_Gd"},
		{0x0200, "_OP_ADD_Gb_Mb_A16"},			{A32 | 0x0200, "_OP_ADD_Gb_Mb_A32"},
		{O32 | 0x0200, "_OP_ADD_Gb_Mb_A16"},	{A32 | O32 | 0x0200, "_OP_ADD_Gb_Mb_A32"},
		{0x02c0, "_OP_ADD_Gb_Rb"},				{A32 | 0x02c0, "_OP_ADD_Gb_Rb"},
		{O32 | 0x02c0, "_OP_ADD_Gb_Rb"},		{A32 | O32 | 0x02c0, "_OP_ADD_Gb_Rb"},
		{0x0300, "_OP_ADD_Gw_Mw_A16"},			{A32 | 0x0300, "_OP_ADD_Gw_Mw_A32"},
		{O32 | 0x0300, "_OP_ADD_Gd_Md_A16"},	{A32 | O32 | 0x0300, "_OP_ADD_Gd_Md_A32"},
		{0x03c0, "_OP_ADD_Gw_Rw"},				{A32 | 0x03c0, "_OP_ADD_Gw_Rw"},
		{O32 | 0x03c0, "_OP_ADD_Gd_Rd"},		{A32 | O32 | 0x03c0, "_OP_ADD_Gd_Rd"},

		{0x04, "_OP_ADD_AL_Ib"},				{A32 | 0x04, "_OP_ADD_AL_Ib"},
		{O32 | 0x04, "_OP_ADD_AL_Ib"},			{A32 | O32 | 0x04, "_OP_ADD_AL_Ib"},
		{0x05, "_OP_ADD_AX_Iw"},				{A32 | 0x05, "_OP_ADD_AX_Iw"},
		{O32 | 0x05, "_OP_ADD_EAX_Id"},			{A32 | O32 | 0x05, "_OP_ADD_EAX_Id"},

		{0x06, "_OP_PUSH_ES_O16"},			{A32 | 0x06, "_OP_PUSH_ES_O16"},
		{O32 | 0x06, "_OP_PUSH_ES_O32"},	{A32 | O32 | 0x06, "_OP_PUSH_ES_O32"},
		{0x07, "_OP_POP_ES_O16"},			{A32 | 0x07, "_OP_POP_ES_O16"},
		{O32 | 0x07, "_OP_POP_ES_O32"},		{A32 | O32 | 0x07, "_OP_POP_ES_O32"},

		{0x0800, "_OP_OR_Mb_Gb_A16"},			{A32 | 0x0800, "_OP_OR_Mb_Gb_A32"},
		{O32 | 0x0800, "_OP_OR_Mb_Gb_A16"},		{A32 | O32 | 0x0800, "_OP_OR_Mb_Gb_A32"},
		{0x08c0, "_OP_OR_Rb_Gb"},				{A32 | 0x08c0, "_OP_OR_Rb_Gb"},
		{O32 | 0x08c0, "_OP_OR_Rb_Gb"},			{A32 | O32 | 0x08c0, "_OP_OR_Rb_Gb"},
		{0x0900, "_OP_OR_Mw_Gw_A16"},			{A32 | 0x0900, "_OP_OR_Mw_Gw_A32"},
		{O32 | 0x0900, "_OP_OR_Md_Gd_A16"},		{A32 | O32 | 0x0900, "_OP_OR_Md_Gd_A32"},
		{0x09c0, "_OP_OR_Rw_Gw"},				{A32 | 0x09c0, "_OP_OR_Rw_Gw"},
		{O32 | 0x09c0, "_OP_OR_Rd_Gd"},			{A32 | O32 | 0x09c0, "_OP_OR_Rd_Gd"},
		{0x0a00, "_OP_OR_Gb_Mb_A16"},			{A32 | 0x0a00, "_OP_OR_Gb_Mb_A32"},
		{O32 | 0x0a00, "_OP_OR_Gb_Mb_A16"},		{A32 | O32 | 0x0a00, "_OP_OR_Gb_Mb_A32"},
		{0x0ac0, "_OP_OR_Gb_Rb"},				{A32 | 0x0ac0, "_OP_OR_Gb_Rb"},
		{O32 | 0x0ac0, "_OP_OR_Gb_Rb"},			{A32 | O32 | 0x0ac0, "_OP_OR_Gb_Rb"},
		{0x0b00, "_OP_OR_Gw_Mw_A16"},			{A32 | 0x0b00, "_OP_OR_Gw_Mw_A32"},
		{O32 | 0x0b00, "_OP_OR_Gd_Md_A16"},		{A32 | O32 | 0x0b00, "_OP_OR_Gd_Md_A32"},
		{0x0bc0, "_OP_OR_Gw_Rw"},				{A32 | 0x0bc0, "_OP_OR_Gw_Rw"},
		{O32 | 0x0bc0, "_OP_OR_Gd_Rd"},			{A32 | O32 | 0x0bc0, "_OP_OR_Gd_Rd"},

		{0x0c, "_OP_OR_AL_Ib"},					{A32 | 0x0c, "_OP_OR_AL_Ib"},
		{O32 | 0x0c, "_OP_OR_AL_Ib"},			{A32 | O32 | 0x0c, "_OP_OR_AL_Ib"},
		{0x0d, "_OP_OR_AX_Iw"},					{A32 | 0x0d, "_OP_OR_AX_Iw"},
		{O32 | 0x0d, "_OP_OR_EAX_Id"},			{A32 | O32 | 0x0d, "_OP_OR_EAX_Id"},
		
		{0x0e, "_OP_PUSH_CS_O16"},			{A32 | 0x0e, "_OP_PUSH_CS_O16"},
		{O32 | 0x0e, "_OP_PUSH_CS_O32"},	{A32 | O32 | 0x0e, "_OP_PUSH_CS_O32"},
		
		{0x1000, "_OP_ADC_Mb_Gb_A16"},			{A32 | 0x1000, "_OP_ADC_Mb_Gb_A32"},
		{O32 | 0x1000, "_OP_ADC_Mb_Gb_A16"},	{A32 | O32 | 0x1000, "_OP_ADC_Mb_Gb_A32"},
		{0x10c0, "_OP_ADC_Rb_Gb"},				{A32 | 0x10c0, "_OP_ADC_Rb_Gb"},
		{O32 | 0x10c0, "_OP_ADC_Rb_Gb"},		{A32 | O32 | 0x10c0, "_OP_ADC_Rb_Gb"},
		{0x1100, "_OP_ADC_Mw_Gw_A16"},			{A32 | 0x1100, "_OP_ADC_Mw_Gw_A32"},
		{O32 | 0x1100, "_OP_ADC_Md_Gd_A16"},	{A32 | O32 | 0x1100, "_OP_ADC_Md_Gd_A32"},
		{0x11c0, "_OP_ADC_Rw_Gw"},				{A32 | 0x11c0, "_OP_ADC_Rw_Gw"},
		{O32 | 0x11c0, "_OP_ADC_Rd_Gd"},		{A32 | O32 | 0x11c0, "_OP_ADC_Rd_Gd"},
		{0x1200, "_OP_ADC_Gb_Mb_A16"},			{A32 | 0x1200, "_OP_ADC_Gb_Mb_A32"},
		{O32 | 0x1200, "_OP_ADC_Gb_Mb_A16"},	{A32 | O32 | 0x1200, "_OP_ADC_Gb_Mb_A32"},
		{0x12c0, "_OP_ADC_Gb_Rb"},				{A32 | 0x12c0, "_OP_ADC_Gb_Rb"},
		{O32 | 0x12c0, "_OP_ADC_Gb_Rb"},		{A32 | O32 | 0x12c0, "_OP_ADC_Gb_Rb"},
		{0x1300, "_OP_ADC_Gw_Mw_A16"},			{A32 | 0x1300, "_OP_ADC_Gw_Mw_A32"},
		{O32 | 0x1300, "_OP_ADC_Gd_Md_A16"},	{A32 | O32 | 0x1300, "_OP_ADC_Gd_Md_A32"},
		{0x13c0, "_OP_ADC_Gw_Rw"},				{A32 | 0x13c0, "_OP_ADC_Gw_Rw"},
		{O32 | 0x13c0, "_OP_ADC_Gd_Rd"},		{A32 | O32 | 0x13c0, "_OP_ADC_Gd_Rd"},

		{0x14, "_OP_ADC_AL_Ib"},				{A32 | 0x14, "_OP_ADC_AL_Ib"},
		{O32 | 0x14, "_OP_ADC_AL_Ib"},			{A32 | O32 | 0x14, "_OP_ADC_AL_Ib"},
		{0x15, "_OP_ADC_AX_Iw"},				{A32 | 0x15, "_OP_ADC_AX_Iw"},
		{O32 | 0x15, "_OP_ADC_EAX_Id"},			{A32 | O32 | 0x15, "_OP_ADC_EAX_Id"},

		{0x16, "_OP_PUSH_SS_O16"},			{A32 | 0x16, "_OP_PUSH_SS_O16"},
		{O32 | 0x16, "_OP_PUSH_SS_O32"},	{A32 | O32 | 0x16, "_OP_PUSH_SS_O32"},
		{0x17, "_OP_POP_SS_O16"},			{A32 | 0x17, "_OP_POP_SS_O16"},
		{O32 | 0x17, "_OP_POP_SS_O32"},		{A32 | O32 | 0x17, "_OP_POP_SS_O32"},

		{0x1800, "_OP_SBB_Mb_Gb_A16"},			{A32 | 0x1800, "_OP_SBB_Mb_Gb_A32"},
		{O32 | 0x1800, "_OP_SBB_Mb_Gb_A16"},	{A32 | O32 | 0x1800, "_OP_SBB_Mb_Gb_A32"},
		{0x18c0, "_OP_SBB_Rb_Gb"},				{A32 | 0x18c0, "_OP_SBB_Rb_Gb"},
		{O32 | 0x18c0, "_OP_SBB_Rb_Gb"},		{A32 | O32 | 0x18c0, "_OP_SBB_Rb_Gb"},
		{0x1900, "_OP_SBB_Mw_Gw_A16"},			{A32 | 0x1900, "_OP_SBB_Mw_Gw_A32"},
		{O32 | 0x1900, "_OP_SBB_Md_Gd_A16"},	{A32 | O32 | 0x1900, "_OP_SBB_Md_Gd_A32"},
		{0x19c0, "_OP_SBB_Rw_Gw"},				{A32 | 0x19c0, "_OP_SBB_Rw_Gw"},
		{O32 | 0x19c0, "_OP_SBB_Rd_Gd"},		{A32 | O32 | 0x19c0, "_OP_SBB_Rd_Gd"},
		{0x1a00, "_OP_SBB_Gb_Mb_A16"},			{A32 | 0x1a00, "_OP_SBB_Gb_Mb_A32"},
		{O32 | 0x1a00, "_OP_SBB_Gb_Mb_A16"},	{A32 | O32 | 0x1a00, "_OP_SBB_Gb_Mb_A32"},
		{0x1ac0, "_OP_SBB_Gb_Rb"},				{A32 | 0x1ac0, "_OP_SBB_Gb_Rb"},
		{O32 | 0x1ac0, "_OP_SBB_Gb_Rb"},		{A32 | O32 | 0x1ac0, "_OP_SBB_Gb_Rb"},
		{0x1b00, "_OP_SBB_Gw_Mw_A16"},			{A32 | 0x1b00, "_OP_SBB_Gw_Mw_A32"},
		{O32 | 0x1b00, "_OP_SBB_Gd_Md_A16"},	{A32 | O32 | 0x1b00, "_OP_SBB_Gd_Md_A32"},
		{0x1bc0, "_OP_SBB_Gw_Rw"},				{A32 | 0x1bc0, "_OP_SBB_Gw_Rw"},
		{O32 | 0x1bc0, "_OP_SBB_Gd_Rd"},		{A32 | O32 | 0x1bc0, "_OP_SBB_Gd_Rd"},

		{0x1c, "_OP_SBB_AL_Ib"},				{A32 | 0x1c, "_OP_SBB_AL_Ib"},
		{O32 | 0x1c, "_OP_SBB_AL_Ib"},			{A32 | O32 | 0x1c, "_OP_SBB_AL_Ib"},
		{0x1d, "_OP_SBB_AX_Iw"},				{A32 | 0x1d, "_OP_SBB_AX_Iw"},
		{O32 | 0x1d, "_OP_SBB_EAX_Id"},			{A32 | O32 | 0x1d, "_OP_SBB_EAX_Id"},

		{0x1e, "_OP_PUSH_DS_O16"},			{A32 | 0x1e, "_OP_PUSH_DS_O16"},
		{O32 | 0x1e, "_OP_PUSH_DS_O32"},	{A32 | O32 | 0x1e, "_OP_PUSH_DS_O32"},
		{0x1f, "_OP_POP_DS_O16"},			{A32 | 0x1f, "_OP_POP_DS_O16"},
		{O32 | 0x1f, "_OP_POP_DS_O32"},		{A32 | O32 | 0x1f, "_OP_POP_DS_O32"},

		{0x2000, "_OP_AND_Mb_Gb_A16"},			{A32 | 0x2000, "_OP_AND_Mb_Gb_A32"},
		{O32 | 0x2000, "_OP_AND_Mb_Gb_A16"},	{A32 | O32 | 0x2000, "_OP_AND_Mb_Gb_A32"},
		{0x20c0, "_OP_AND_Rb_Gb"},				{A32 | 0x20c0, "_OP_AND_Rb_Gb"},
		{O32 | 0x20c0, "_OP_AND_Rb_Gb"},		{A32 | O32 | 0x20c0, "_OP_AND_Rb_Gb"},
		{0x2100, "_OP_AND_Mw_Gw_A16"},			{A32 | 0x2100, "_OP_AND_Mw_Gw_A32"},
		{O32 | 0x2100, "_OP_AND_Md_Gd_A16"},	{A32 | O32 | 0x2100, "_OP_AND_Md_Gd_A32"},
		{0x21c0, "_OP_AND_Rw_Gw"},				{A32 | 0x21c0, "_OP_AND_Rw_Gw"},
		{O32 | 0x21c0, "_OP_AND_Rd_Gd"},		{A32 | O32 | 0x21c0, "_OP_AND_Rd_Gd"},
		{0x2200, "_OP_AND_Gb_Mb_A16"},			{A32 | 0x2200, "_OP_AND_Gb_Mb_A32"},
		{O32 | 0x2200, "_OP_AND_Gb_Mb_A16"},	{A32 | O32 | 0x2200, "_OP_AND_Gb_Mb_A32"},
		{0x22c0, "_OP_AND_Gb_Rb"},				{A32 | 0x22c0, "_OP_AND_Gb_Rb"},
		{O32 | 0x22c0, "_OP_AND_Gb_Rb"},		{A32 | O32 | 0x22c0, "_OP_AND_Gb_Rb"},
		{0x2300, "_OP_AND_Gw_Mw_A16"},			{A32 | 0x2300, "_OP_AND_Gw_Mw_A32"},
		{O32 | 0x2300, "_OP_AND_Gd_Md_A16"},	{A32 | O32 | 0x2300, "_OP_AND_Gd_Md_A32"},
		{0x23c0, "_OP_AND_Gw_Rw"},				{A32 | 0x23c0, "_OP_AND_Gw_Rw"},
		{O32 | 0x23c0, "_OP_AND_Gd_Rd"},		{A32 | O32 | 0x23c0, "_OP_AND_Gd_Rd"},

		{0x24, "_OP_AND_AL_Ib"},				{A32 | 0x24, "_OP_AND_AL_Ib"},
		{O32 | 0x24, "_OP_AND_AL_Ib"},			{A32 | O32 | 0x24, "_OP_AND_AL_Ib"},
		{0x25, "_OP_AND_AX_Iw"},				{A32 | 0x25, "_OP_AND_AX_Iw"},
		{O32 | 0x25, "_OP_AND_EAX_Id"},			{A32 | O32 | 0x25, "_OP_AND_EAX_Id"},
		
		{0x27, "_OP_DAA"},						{A32 | 0x27, "_OP_DAA"},
		{O32 | 0x27, "_OP_DAA"},				{A32 | O32 | 0x27, "_OP_DAA"},

		{0x2800, "_OP_SUB_Mb_Gb_A16"},			{A32 | 0x2800, "_OP_SUB_Mb_Gb_A32"},
		{O32 | 0x2800, "_OP_SUB_Mb_Gb_A16"},	{A32 | O32 | 0x2800, "_OP_SUB_Mb_Gb_A32"},
		{0x28c0, "_OP_SUB_Rb_Gb"},				{A32 | 0x28c0, "_OP_SUB_Rb_Gb"},
		{O32 | 0x28c0, "_OP_SUB_Rb_Gb"},		{A32 | O32 | 0x28c0, "_OP_SUB_Rb_Gb"},
		{0x2900, "_OP_SUB_Mw_Gw_A16"},			{A32 | 0x2900, "_OP_SUB_Mw_Gw_A32"},
		{O32 | 0x2900, "_OP_SUB_Md_Gd_A16"},	{A32 | O32 | 0x2900, "_OP_SUB_Md_Gd_A32"},
		{0x29c0, "_OP_SUB_Rw_Gw"},				{A32 | 0x29c0, "_OP_SUB_Rw_Gw"},
		{O32 | 0x29c0, "_OP_SUB_Rd_Gd"},		{A32 | O32 | 0x29c0, "_OP_SUB_Rd_Gd"},
		{0x2a00, "_OP_SUB_Gb_Mb_A16"},			{A32 | 0x2a00, "_OP_SUB_Gb_Mb_A32"},
		{O32 | 0x2a00, "_OP_SUB_Gb_Mb_A16"},	{A32 | O32 | 0x2a00, "_OP_SUB_Gb_Mb_A32"},
		{0x2ac0, "_OP_SUB_Gb_Rb"},				{A32 | 0x2ac0, "_OP_SUB_Gb_Rb"},
		{O32 | 0x2ac0, "_OP_SUB_Gb_Rb"},		{A32 | O32 | 0x2ac0, "_OP_SUB_Gb_Rb"},
		{0x2b00, "_OP_SUB_Gw_Mw_A16"},			{A32 | 0x2b00, "_OP_SUB_Gw_Mw_A32"},
		{O32 | 0x2b00, "_OP_SUB_Gd_Md_A16"},	{A32 | O32 | 0x2b00, "_OP_SUB_Gd_Md_A32"},
		{0x2bc0, "_OP_SUB_Gw_Rw"},				{A32 | 0x2bc0, "_OP_SUB_Gw_Rw"},
		{O32 | 0x2bc0, "_OP_SUB_Gd_Rd"},		{A32 | O32 | 0x2bc0, "_OP_SUB_Gd_Rd"},

		{0x2c, "_OP_SUB_AL_Ib"},				{A32 | 0x2c, "_OP_SUB_AL_Ib"},
		{O32 | 0x2c, "_OP_SUB_AL_Ib"},			{A32 | O32 | 0x2c, "_OP_SUB_AL_Ib"},
		{0x2d, "_OP_SUB_AX_Iw"},				{A32 | 0x2d, "_OP_SUB_AX_Iw"},
		{O32 | 0x2d, "_OP_SUB_EAX_Id"},			{A32 | O32 | 0x2d, "_OP_SUB_EAX_Id"},

		{0x2f, "_OP_DAS"},						{A32 | 0x2f, "_OP_DAS"},
		{O32 | 0x2f, "_OP_DAS"},				{A32 | O32 | 0x2f, "_OP_DAS"},

		{0x3000, "_OP_XOR_Mb_Gb_A16"},			{A32 | 0x3000, "_OP_XOR_Mb_Gb_A32"},
		{O32 | 0x3000, "_OP_XOR_Mb_Gb_A16"},	{A32 | O32 | 0x3000, "_OP_XOR_Mb_Gb_A32"},
		{0x30c0, "_OP_XOR_Rb_Gb"},				{A32 | 0x30c0, "_OP_XOR_Rb_Gb"},
		{O32 | 0x30c0, "_OP_XOR_Rb_Gb"},		{A32 | O32 | 0x30c0, "_OP_XOR_Rb_Gb"},
		{0x3100, "_OP_XOR_Mw_Gw_A16"},			{A32 | 0x3100, "_OP_XOR_Mw_Gw_A32"},
		{O32 | 0x3100, "_OP_XOR_Md_Gd_A16"},	{A32 | O32 | 0x3100, "_OP_XOR_Md_Gd_A32"},
		{0x31c0, "_OP_XOR_Rw_Gw"},				{A32 | 0x31c0, "_OP_XOR_Rw_Gw"},
		{O32 | 0x31c0, "_OP_XOR_Rd_Gd"},		{A32 | O32 | 0x31c0, "_OP_XOR_Rd_Gd"},
		{0x3200, "_OP_XOR_Gb_Mb_A16"},			{A32 | 0x3200, "_OP_XOR_Gb_Mb_A32"},
		{O32 | 0x3200, "_OP_XOR_Gb_Mb_A16"},	{A32 | O32 | 0x3200, "_OP_XOR_Gb_Mb_A32"},
		{0x32c0, "_OP_XOR_Gb_Rb"},				{A32 | 0x32c0, "_OP_XOR_Gb_Rb"},
		{O32 | 0x32c0, "_OP_XOR_Gb_Rb"},		{A32 | O32 | 0x32c0, "_OP_XOR_Gb_Rb"},
		{0x3300, "_OP_XOR_Gw_Mw_A16"},			{A32 | 0x3300, "_OP_XOR_Gw_Mw_A32"},
		{O32 | 0x3300, "_OP_XOR_Gd_Md_A16"},	{A32 | O32 | 0x3300, "_OP_XOR_Gd_Md_A32"},
		{0x33c0, "_OP_XOR_Gw_Rw"},				{A32 | 0x33c0, "_OP_XOR_Gw_Rw"},
		{O32 | 0x33c0, "_OP_XOR_Gd_Rd"},		{A32 | O32 | 0x33c0, "_OP_XOR_Gd_Rd"},

		{0x34, "_OP_XOR_AL_Ib"},				{A32 | 0x34, "_OP_XOR_AL_Ib"},
		{O32 | 0x34, "_OP_XOR_AL_Ib"},			{A32 | O32 | 0x34, "_OP_XOR_AL_Ib"},
		{0x35, "_OP_XOR_AX_Iw"},				{A32 | 0x35, "_OP_XOR_AX_Iw"},
		{O32 | 0x35, "_OP_XOR_EAX_Id"},			{A32 | O32 | 0x35, "_OP_XOR_EAX_Id"},

		{0x37, "_OP_AAA"},						{A32 | 0x37, "_OP_AAA"},
		{O32 | 0x37, "_OP_AAA"},				{A32 | O32 | 0x37, "_OP_AAA"},

		{0x3800, "_OP_CMP_Mb_Gb_A16"},			{A32 | 0x3800, "_OP_CMP_Mb_Gb_A32"},
		{O32 | 0x3800, "_OP_CMP_Mb_Gb_A16"},	{A32 | O32 | 0x3800, "_OP_CMP_Mb_Gb_A32"},
		{0x38c0, "_OP_CMP_Rb_Gb"},				{A32 | 0x38c0, "_OP_CMP_Rb_Gb"},
		{O32 | 0x38c0, "_OP_CMP_Rb_Gb"},		{A32 | O32 | 0x38c0, "_OP_CMP_Rb_Gb"},
		{0x3900, "_OP_CMP_Mw_Gw_A16"},			{A32 | 0x3900, "_OP_CMP_Mw_Gw_A32"},
		{O32 | 0x3900, "_OP_CMP_Md_Gd_A16"},	{A32 | O32 | 0x3900, "_OP_CMP_Md_Gd_A32"},
		{0x39c0, "_OP_CMP_Rw_Gw"},				{A32 | 0x39c0, "_OP_CMP_Rw_Gw"},
		{O32 | 0x39c0, "_OP_CMP_Rd_Gd"},		{A32 | O32 | 0x39c0, "_OP_CMP_Rd_Gd"},
		{0x3a00, "_OP_CMP_Gb_Mb_A16"},			{A32 | 0x3a00, "_OP_CMP_Gb_Mb_A32"},
		{O32 | 0x3a00, "_OP_CMP_Gb_Mb_A16"},	{A32 | O32 | 0x3a00, "_OP_CMP_Gb_Mb_A32"},
		{0x3ac0, "_OP_CMP_Gb_Rb"},				{A32 | 0x3ac0, "_OP_CMP_Gb_Rb"},
		{O32 | 0x3ac0, "_OP_CMP_Gb_Rb"},		{A32 | O32 | 0x3ac0, "_OP_CMP_Gb_Rb"},
		{0x3b00, "_OP_CMP_Gw_Mw_A16"},			{A32 | 0x3b00, "_OP_CMP_Gw_Mw_A32"},
		{O32 | 0x3b00, "_OP_CMP_Gd_Md_A16"},	{A32 | O32 | 0x3b00, "_OP_CMP_Gd_Md_A32"},
		{0x3bc0, "_OP_CMP_Gw_Rw"},				{A32 | 0x3bc0, "_OP_CMP_Gw_Rw"},
		{O32 | 0x3bc0, "_OP_CMP_Gd_Rd"},		{A32 | O32 | 0x3bc0, "_OP_CMP_Gd_Rd"},

		{0x3c, "_OP_CMP_AL_Ib"},				{A32 | 0x3c, "_OP_CMP_AL_Ib"},
		{O32 | 0x3c, "_OP_CMP_AL_Ib"},			{A32 | O32 | 0x3c, "_OP_CMP_AL_Ib"},
		{0x3d, "_OP_CMP_AX_Iw"},				{A32 | 0x3d, "_OP_CMP_AX_Iw"},
		{O32 | 0x3d, "_OP_CMP_EAX_Id"},			{A32 | O32 | 0x3d, "_OP_CMP_EAX_Id"},

		{0x3f, "_OP_AAS"},						{A32 | 0x3f, "_OP_AAS"},
		{O32 | 0x3f, "_OP_AAS"},				{A32 | O32 | 0x3f, "_OP_AAS"},

		{0x40, "_OP_INC_AX"},					{A32 | 0x40, "_OP_INC_AX"},
		{O32 | 0x40, "_OP_INC_EAX"},			{A32 | O32 | 0x40, "_OP_INC_EAX"},
		{0x41, "_OP_INC_CX"},					{A32 | 0x41, "_OP_INC_CX"},
		{O32 | 0x41, "_OP_INC_ECX"},			{A32 | O32 | 0x41, "_OP_INC_ECX"},
		{0x42, "_OP_INC_DX"},					{A32 | 0x42, "_OP_INC_DX"},
		{O32 | 0x42, "_OP_INC_EDX"},			{A32 | O32 | 0x42, "_OP_INC_EDX"},
		{0x43, "_OP_INC_BX"},					{A32 | 0x43, "_OP_INC_BX"},
		{O32 | 0x43, "_OP_INC_EBX"},			{A32 | O32 | 0x43, "_OP_INC_EBX"},
		{0x44, "_OP_INC_SP"},					{A32 | 0x44, "_OP_INC_SP"},
		{O32 | 0x44, "_OP_INC_ESP"},			{A32 | O32 | 0x44, "_OP_INC_ESP"},
		{0x45, "_OP_INC_BP"},					{A32 | 0x45, "_OP_INC_BP"},
		{O32 | 0x45, "_OP_INC_EBP"},			{A32 | O32 | 0x45, "_OP_INC_EBP"},
		{0x46, "_OP_INC_SI"},					{A32 | 0x46, "_OP_INC_SI"},
		{O32 | 0x46, "_OP_INC_ESI"},			{A32 | O32 | 0x46, "_OP_INC_ESI"},
		{0x47, "_OP_INC_DI"},					{A32 | 0x47, "_OP_INC_DI"},
		{O32 | 0x47, "_OP_INC_EDI"},			{A32 | O32 | 0x47, "_OP_INC_EDI"},

		{0x48, "_OP_DEC_AX"},					{A32 | 0x48, "_OP_DEC_AX"},
		{O32 | 0x48, "_OP_DEC_EAX"},			{A32 | O32 | 0x48, "_OP_DEC_EAX"},
		{0x49, "_OP_DEC_CX"},					{A32 | 0x49, "_OP_DEC_CX"},
		{O32 | 0x49, "_OP_DEC_ECX"},			{A32 | O32 | 0x49, "_OP_DEC_ECX"},
		{0x4a, "_OP_DEC_DX"},					{A32 | 0x4a, "_OP_DEC_DX"},
		{O32 | 0x4a, "_OP_DEC_EDX"},			{A32 | O32 | 0x4a, "_OP_DEC_EDX"},
		{0x4b, "_OP_DEC_BX"},					{A32 | 0x4b, "_OP_DEC_BX"},
		{O32 | 0x4b, "_OP_DEC_EBX"},			{A32 | O32 | 0x4b, "_OP_DEC_EBX"},
		{0x4c, "_OP_DEC_SP"},					{A32 | 0x4c, "_OP_DEC_SP"},
		{O32 | 0x4c, "_OP_DEC_ESP"},			{A32 | O32 | 0x4c, "_OP_DEC_ESP"},
		{0x4d, "_OP_DEC_BP"},					{A32 | 0x4d, "_OP_DEC_BP"},
		{O32 | 0x4d, "_OP_DEC_EBP"},			{A32 | O32 | 0x4d, "_OP_DEC_EBP"},
		{0x4e, "_OP_DEC_SI"},					{A32 | 0x4e, "_OP_DEC_SI"},
		{O32 | 0x4e, "_OP_DEC_ESI"},			{A32 | O32 | 0x4e, "_OP_DEC_ESI"},
		{0x4f, "_OP_DEC_DI"},					{A32 | 0x4f, "_OP_DEC_DI"},
		{O32 | 0x4f, "_OP_DEC_EDI"},			{A32 | O32 | 0x4f, "_OP_DEC_EDI"},
		
		{0x50, "_OP_PUSH_AX"},			{A32 | 0x50, "_OP_PUSH_AX"},
		{O32 | 0x50, "_OP_PUSH_EAX"},	{A32 | O32 | 0x50, "_OP_PUSH_EAX"},
		{0x51, "_OP_PUSH_CX"},			{A32 | 0x51, "_OP_PUSH_CX"},
		{O32 | 0x51, "_OP_PUSH_ECX"},	{A32 | O32 | 0x51, "_OP_PUSH_ECX"},
		{0x52, "_OP_PUSH_DX"},			{A32 | 0x52, "_OP_PUSH_DX"},
		{O32 | 0x52, "_OP_PUSH_EDX"},	{A32 | O32 | 0x52, "_OP_PUSH_EDX"},
		{0x53, "_OP_PUSH_BX"},			{A32 | 0x53, "_OP_PUSH_BX"},
		{O32 | 0x53, "_OP_PUSH_EBX"},	{A32 | O32 | 0x53, "_OP_PUSH_EBX"},
		{0x54, "_OP_PUSH_SP"},			{A32 | 0x54, "_OP_PUSH_SP"},
		{O32 | 0x54, "_OP_PUSH_ESP"},	{A32 | O32 | 0x54, "_OP_PUSH_ESP"},
		{0x55, "_OP_PUSH_BP"},			{A32 | 0x55, "_OP_PUSH_BP"},
		{O32 | 0x55, "_OP_PUSH_EBP"},	{A32 | O32 | 0x55, "_OP_PUSH_EBP"},
		{0x56, "_OP_PUSH_SI"},			{A32 | 0x56, "_OP_PUSH_SI"},
		{O32 | 0x56, "_OP_PUSH_ESI"},	{A32 | O32 | 0x56, "_OP_PUSH_ESI"},
		{0x57, "_OP_PUSH_DI"},			{A32 | 0x57, "_OP_PUSH_DI"},
		{O32 | 0x57, "_OP_PUSH_EDI"},	{A32 | O32 | 0x57, "_OP_PUSH_EDI"},
		
		{0x58, "_OP_POP_AX"},			{A32 | 0x58, "_OP_POP_AX"},
		{O32 | 0x58, "_OP_POP_EAX"},	{A32 | O32 | 0x58, "_OP_POP_EAX"},
		{0x59, "_OP_POP_CX"},			{A32 | 0x59, "_OP_POP_CX"},
		{O32 | 0x59, "_OP_POP_ECX"},	{A32 | O32 | 0x59, "_OP_POP_ECX"},
		{0x5a, "_OP_POP_DX"},			{A32 | 0x5a, "_OP_POP_DX"},
		{O32 | 0x5a, "_OP_POP_EDX"},	{A32 | O32 | 0x5a, "_OP_POP_EDX"},
		{0x5b, "_OP_POP_BX"},			{A32 | 0x5b, "_OP_POP_BX"},
		{O32 | 0x5b, "_OP_POP_EBX"},	{A32 | O32 | 0x5b, "_OP_POP_EBX"},
		{0x5c, "_OP_POP_SP"},			{A32 | 0x5c, "_OP_POP_SP"},
		{O32 | 0x5c, "_OP_POP_ESP"},	{A32 | O32 | 0x5c, "_OP_POP_ESP"},
		{0x5d, "_OP_POP_BP"},			{A32 | 0x5d, "_OP_POP_BP"},
		{O32 | 0x5d, "_OP_POP_EBP"},	{A32 | O32 | 0x5d, "_OP_POP_EBP"},
		{0x5e, "_OP_POP_SI"},			{A32 | 0x5e, "_OP_POP_SI"},
		{O32 | 0x5e, "_OP_POP_ESI"},	{A32 | O32 | 0x5e, "_OP_POP_ESI"},
		{0x5f, "_OP_POP_DI"},			{A32 | 0x5f, "_OP_POP_DI"},
		{O32 | 0x5f, "_OP_POP_EDI"},	{A32 | O32 | 0x5f, "_OP_POP_EDI"},
#endif
				
		{0x60, "_OP_PUSHA_O16"},		{A32 | 0x60, "_OP_PUSHA_O16"},
		{O32 | 0x60, "_OP_PUSHA_O32"},	{A32 | O32 | 0x60, "_OP_PUSHA_O32"},
		{0x61, "_OP_POPA_O16"},			{A32 | 0x61, "_OP_POPA_O16"},
		{O32 | 0x61, "_OP_POPA_O32"},	{A32 | O32 | 0x61, "_OP_POPA_O32"},

		{0x6200, "_OP_BOUND_Gw_Ma_A16"},		{A32 | 0x6200, "_OP_BOUND_Gw_Ma_A32"},
		{O32 | 0x6200, "_OP_BOUND_Gd_Ma_A16"},	{A32 | O32 | 0x6200, "_OP_BOUND_Gd_Ma_A32"},

		{0x6300, "_OP_ARPL_Mw_Gw_A16"},			{A32 | 0x6300, "_OP_ARPL_Mw_Gw_A32"},
		{O32 | 0x6300, "_OP_ARPL_Mw_Gw_A16"},	{A32 | O32 | 0x6300, "_OP_ARPL_Mw_Gw_A32"},
		{0x63c0, "_OP_ARPL_Rw_Gw"},				{A32 | 0x63c0, "_OP_ARPL_Rw_Gw"},
		{O32 | 0x63c0, "_OP_ARPL_Rw_Gw"},		{A32 | O32 | 0x63c0, "_OP_ARPL_Rw_Gw"},
		
#ifndef USE_MICROCODE
		{0x68, "_OP_PUSH_Iw"},			{A32 | 0x68, "_OP_PUSH_Iw"},
		{O32 | 0x68, "_OP_PUSH_Id"},	{A32 | O32 | 0x68, "_OP_PUSH_Id"},

		{0x6900, "_OP_IMUL_Gw_Mw_Iw_A16"},		{A32 | 0x6900, "_OP_IMUL_Gw_Mw_Iw_A32"},
		{O32 | 0x6900, "_OP_IMUL_Gd_Md_Id_A16"},{A32 | O32 | 0x6900, "_OP_IMUL_Gd_Md_Id_A32"},
		{0x69c0, "_OP_IMUL_Gw_Rw_Iw"},			{A32 | 0x69c0, "_OP_IMUL_Gw_Rw_Iw"},
		{O32 | 0x69c0, "_OP_IMUL_Gd_Rd_Id"},	{A32 | O32 | 0x69c0, "_OP_IMUL_Gd_Rd_Id"},

		{0x6a, "_OP_PUSH_Ib_O16"},				{A32 | 0x6a, "_OP_PUSH_Ib_O16"},
		{O32 | 0x6a, "_OP_PUSH_Ib_O32"},		{A32 | O32 | 0x6a, "_OP_PUSH_Ib_O32"},

		{0x6b00, "_OP_IMUL_Gw_Mw_Ib_A16"},		{A32 | 0x6b00, "_OP_IMUL_Gw_Mw_Ib_A32"},
		{O32 | 0x6b00, "_OP_IMUL_Gd_Md_Ib_A16"},{A32 | O32 | 0x6b00, "_OP_IMUL_Gd_Md_Ib_A32"},
		{0x6bc0, "_OP_IMUL_Gw_Rw_Ib"},			{A32 | 0x6bc0, "_OP_IMUL_Gw_Rw_Ib"},
		{O32 | 0x6bc0, "_OP_IMUL_Gd_Rd_Ib"},	{A32 | O32 | 0x6bc0, "_OP_IMUL_Gd_Rd_Ib"},
#endif

		{0x70, "_OP_JO_Jb_O16"},				{A32 | 0x70, "_OP_JO_Jb_O16"},
		{O32 | 0x70, "_OP_JO_Jb_O32"},			{A32 | O32 | 0x70, "_OP_JO_Jb_O32"},
		{0x71, "_OP_JNO_Jb_O16"},				{A32 | 0x71, "_OP_JNO_Jb_O16"},
		{O32 | 0x71, "_OP_JNO_Jb_O32"},			{A32 | O32 | 0x71, "_OP_JNO_Jb_O32"},

#ifndef USE_MICROCODE
		{0x72, "_OP_JB_Jb_O16"},				{A32 | 0x72, "_OP_JB_Jb_O16"},
		{O32 | 0x72, "_OP_JB_Jb_O32"},			{A32 | O32 | 0x72, "_OP_JB_Jb_O32"},
		{0x73, "_OP_JNB_Jb_O16"},				{A32 | 0x73, "_OP_JNB_Jb_O16"},
		{O32 | 0x73, "_OP_JNB_Jb_O32"},			{A32 | O32 | 0x73, "_OP_JNB_Jb_O32"},
		{0x74, "_OP_JZ_Jb_O16"},				{A32 | 0x74, "_OP_JZ_Jb_O16"},
		{O32 | 0x74, "_OP_JZ_Jb_O32"},			{A32 | O32 | 0x74, "_OP_JZ_Jb_O32"},
		{0x75, "_OP_JNZ_Jb_O16"},				{A32 | 0x75, "_OP_JNZ_Jb_O16"},
		{O32 | 0x75, "_OP_JNZ_Jb_O32"},			{A32 | O32 | 0x75, "_OP_JNZ_Jb_O32"},
		{0x76, "_OP_JBE_Jb_O16"},				{A32 | 0x76, "_OP_JBE_Jb_O16"},
		{O32 | 0x76, "_OP_JBE_Jb_O32"},			{A32 | O32 | 0x76, "_OP_JBE_Jb_O32"},
		{0x77, "_OP_JNBE_Jb_O16"},				{A32 | 0x77, "_OP_JNBE_Jb_O16"},
		{O32 | 0x77, "_OP_JNBE_Jb_O32"},		{A32 | O32 | 0x77, "_OP_JNBE_Jb_O32"},
		{0x78, "_OP_JS_Jb_O16"},				{A32 | 0x78, "_OP_JS_Jb_O16"},
		{O32 | 0x78, "_OP_JS_Jb_O32"},			{A32 | O32 | 0x78, "_OP_JS_Jb_O32"},
		{0x79, "_OP_JNS_Jb_O16"},				{A32 | 0x79, "_OP_JNS_Jb_O16"},
		{O32 | 0x79, "_OP_JNS_Jb_O32"},			{A32 | O32 | 0x79, "_OP_JNS_Jb_O32"},
		{0x7a, "_OP_JP_Jb_O16"},				{A32 | 0x7a, "_OP_JP_Jb_O16"},
		{O32 | 0x7a, "_OP_JP_Jb_O32"},			{A32 | O32 | 0x7a, "_OP_JP_Jb_O32"},
		{0x7b, "_OP_JNP_Jb_O16"},				{A32 | 0x7b, "_OP_JNP_Jb_O16"},
		{O32 | 0x7b, "_OP_JNP_Jb_O32"},			{A32 | O32 | 0x7b, "_OP_JNP_Jb_O32"},
#endif

		{0x7c, "_OP_JL_Jb_O16"},				{A32 | 0x7c, "_OP_JL_Jb_O16"},
		{O32 | 0x7c, "_OP_JL_Jb_O32"},			{A32 | O32 | 0x7c, "_OP_JL_Jb_O32"},
		{0x7d, "_OP_JNL_Jb_O16"},				{A32 | 0x7d, "_OP_JNL_Jb_O16"},
		{O32 | 0x7d, "_OP_JNL_Jb_O32"},			{A32 | O32 | 0x7d, "_OP_JNL_Jb_O32"},
		{0x7e, "_OP_JLE_Jb_O16"},				{A32 | 0x7e, "_OP_JLE_Jb_O16"},
		{O32 | 0x7e, "_OP_JLE_Jb_O32"},			{A32 | O32 | 0x7e, "_OP_JLE_Jb_O32"},
		{0x7f, "_OP_JNLE_Jb_O16"},				{A32 | 0x7f, "_OP_JNLE_Jb_O16"},
		{O32 | 0x7f, "_OP_JNLE_Jb_O32"},		{A32 | O32 | 0x7f, "_OP_JNLE_Jb_O32"},

#ifndef USE_MICROCODE
		{0x8000, "_OP_ADD_Mb_Ib_A16"},		{A32 | 0x8000, "_OP_ADD_Mb_Ib_A32"},
		{O32 | 0x8000, "_OP_ADD_Mb_Ib_A16"},{A32 | O32 | 0x8000, "_OP_ADD_Mb_Ib_A32"},
		{0x8008, "_OP_OR_Mb_Ib_A16"},		{A32 | 0x8008, "_OP_OR_Mb_Ib_A32"},
		{O32 | 0x8008, "_OP_OR_Mb_Ib_A16"},	{A32 | O32 | 0x8008, "_OP_OR_Mb_Ib_A32"},
		{0x8010, "_OP_ADC_Mb_Ib_A16"},		{A32 | 0x8010, "_OP_ADC_Mb_Ib_A32"},
		{O32 | 0x8010, "_OP_ADC_Mb_Ib_A16"},{A32 | O32 | 0x8010, "_OP_ADC_Mb_Ib_A32"},
		{0x8018, "_OP_SBB_Mb_Ib_A16"},		{A32 | 0x8018, "_OP_SBB_Mb_Ib_A32"},
		{O32 | 0x8018, "_OP_SBB_Mb_Ib_A16"},{A32 | O32 | 0x8018, "_OP_SBB_Mb_Ib_A32"},
		{0x8020, "_OP_AND_Mb_Ib_A16"},		{A32 | 0x8020, "_OP_AND_Mb_Ib_A32"},
		{O32 | 0x8020, "_OP_AND_Mb_Ib_A16"},{A32 | O32 | 0x8020, "_OP_AND_Mb_Ib_A32"},
		{0x8028, "_OP_SUB_Mb_Ib_A16"},		{A32 | 0x8028, "_OP_SUB_Mb_Ib_A32"},
		{O32 | 0x8028, "_OP_SUB_Mb_Ib_A16"},{A32 | O32 | 0x8028, "_OP_SUB_Mb_Ib_A32"},
		{0x8030, "_OP_XOR_Mb_Ib_A16"},		{A32 | 0x8030, "_OP_XOR_Mb_Ib_A32"},
		{O32 | 0x8030, "_OP_XOR_Mb_Ib_A16"},{A32 | O32 | 0x8030, "_OP_XOR_Mb_Ib_A32"},
		{0x8038, "_OP_CMP_Mb_Ib_A16"},		{A32 | 0x8038, "_OP_CMP_Mb_Ib_A32"},
		{O32 | 0x8038, "_OP_CMP_Mb_Ib_A16"},{A32 | O32 | 0x8038, "_OP_CMP_Mb_Ib_A32"},
		{0x80c0, "_OP_ADD_Rb_Ib"},			{A32 | 0x80c0, "_OP_ADD_Rb_Ib"},
		{O32 | 0x80c0, "_OP_ADD_Rb_Ib"},	{A32 | O32 | 0x80c0, "_OP_ADD_Rb_Ib"},
		{0x80c8, "_OP_OR_Rb_Ib"},			{A32 | 0x80c8, "_OP_OR_Rb_Ib"},
		{O32 | 0x80c8, "_OP_OR_Rb_Ib"},		{A32 | O32 | 0x80c8, "_OP_OR_Rb_Ib"},
		{0x80d0, "_OP_ADC_Rb_Ib"},			{A32 | 0x80d0, "_OP_ADC_Rb_Ib"},
		{O32 | 0x80d0, "_OP_ADC_Rb_Ib"},	{A32 | O32 | 0x80d0, "_OP_ADC_Rb_Ib"},
		{0x80d8, "_OP_SBB_Rb_Ib"},			{A32 | 0x80d8, "_OP_SBB_Rb_Ib"},
		{O32 | 0x80d8, "_OP_SBB_Rb_Ib"},	{A32 | O32 | 0x80d8, "_OP_SBB_Rb_Ib"},
		{0x80e0, "_OP_AND_Rb_Ib"},			{A32 | 0x80e0, "_OP_AND_Rb_Ib"},
		{O32 | 0x80e0, "_OP_AND_Rb_Ib"},	{A32 | O32 | 0x80e0, "_OP_AND_Rb_Ib"},
		{0x80e8, "_OP_SUB_Rb_Ib"},			{A32 | 0x80e8, "_OP_SUB_Rb_Ib"},
		{O32 | 0x80e8, "_OP_SUB_Rb_Ib"},	{A32 | O32 | 0x80e8, "_OP_SUB_Rb_Ib"},
		{0x80f0, "_OP_XOR_Rb_Ib"},			{A32 | 0x80f0, "_OP_XOR_Rb_Ib"},
		{O32 | 0x80f0, "_OP_XOR_Rb_Ib"},	{A32 | O32 | 0x80f0, "_OP_XOR_Rb_Ib"},
		{0x80f8, "_OP_CMP_Rb_Ib"},			{A32 | 0x80f8, "_OP_CMP_Rb_Ib"},
		{O32 | 0x80f8, "_OP_CMP_Rb_Ib"},	{A32 | O32 | 0x80f8, "_OP_CMP_Rb_Ib"},

		{0x8100, "_OP_ADD_Mw_Iw_A16"},		{A32 | 0x8100, "_OP_ADD_Mw_Iw_A32"},
		{O32 | 0x8100, "_OP_ADD_Md_Id_A16"},{A32 | O32 | 0x8100, "_OP_ADD_Md_Id_A32"},
		{0x8108, "_OP_OR_Mw_Iw_A16"},		{A32 | 0x8108, "_OP_OR_Mw_Iw_A32"},
		{O32 | 0x8108, "_OP_OR_Md_Id_A16"},	{A32 | O32 | 0x8108, "_OP_OR_Md_Id_A32"},
		{0x8110, "_OP_ADC_Mw_Iw_A16"},		{A32 | 0x8110, "_OP_ADC_Mw_Iw_A32"},
		{O32 | 0x8110, "_OP_ADC_Md_Id_A16"},{A32 | O32 | 0x8110, "_OP_ADC_Md_Id_A32"},
		{0x8118, "_OP_SBB_Mw_Iw_A16"},		{A32 | 0x8118, "_OP_SBB_Mw_Iw_A32"},
		{O32 | 0x8118, "_OP_SBB_Md_Id_A16"},{A32 | O32 | 0x8118, "_OP_SBB_Md_Id_A32"},
		{0x8120, "_OP_AND_Mw_Iw_A16"},		{A32 | 0x8120, "_OP_AND_Mw_Iw_A32"},
		{O32 | 0x8120, "_OP_AND_Md_Id_A16"},{A32 | O32 | 0x8120, "_OP_AND_Md_Id_A32"},
		{0x8128, "_OP_SUB_Mw_Iw_A16"},		{A32 | 0x8128, "_OP_SUB_Mw_Iw_A32"},
		{O32 | 0x8128, "_OP_SUB_Md_Id_A16"},{A32 | O32 | 0x8128, "_OP_SUB_Md_Id_A32"},
		{0x8130, "_OP_XOR_Mw_Iw_A16"},		{A32 | 0x8130, "_OP_XOR_Mw_Iw_A32"},
		{O32 | 0x8130, "_OP_XOR_Md_Id_A16"},{A32 | O32 | 0x8130, "_OP_XOR_Md_Id_A32"},
		{0x8138, "_OP_CMP_Mw_Iw_A16"},		{A32 | 0x8138, "_OP_CMP_Mw_Iw_A32"},
		{O32 | 0x8138, "_OP_CMP_Md_Id_A16"},{A32 | O32 | 0x8138, "_OP_CMP_Md_Id_A32"},
		{0x81c0, "_OP_ADD_Rw_Iw"},			{A32 | 0x81c0, "_OP_ADD_Rw_Iw"},
		{O32 | 0x81c0, "_OP_ADD_Rd_Id"},	{A32 | O32 | 0x81c0, "_OP_ADD_Rd_Id"},
		{0x81c8, "_OP_OR_Rw_Iw"},			{A32 | 0x81c8, "_OP_OR_Rw_Iw"},
		{O32 | 0x81c8, "_OP_OR_Rd_Id"},		{A32 | O32 | 0x81c8, "_OP_OR_Rd_Id"},
		{0x81d0, "_OP_ADC_Rw_Iw"},			{A32 | 0x81d0, "_OP_ADC_Rw_Iw"},
		{O32 | 0x81d0, "_OP_ADC_Rd_Id"},	{A32 | O32 | 0x81d0, "_OP_ADC_Rd_Id"},
		{0x81d8, "_OP_SBB_Rw_Iw"},			{A32 | 0x81d8, "_OP_SBB_Rw_Iw"},
		{O32 | 0x81d8, "_OP_SBB_Rd_Id"},	{A32 | O32 | 0x81d8, "_OP_SBB_Rd_Id"},
		{0x81e0, "_OP_AND_Rw_Iw"},			{A32 | 0x81e0, "_OP_AND_Rw_Iw"},
		{O32 | 0x81e0, "_OP_AND_Rd_Id"},	{A32 | O32 | 0x81e0, "_OP_AND_Rd_Id"},
		{0x81e8, "_OP_SUB_Rw_Iw"},			{A32 | 0x81e8, "_OP_SUB_Rw_Iw"},
		{O32 | 0x81e8, "_OP_SUB_Rd_Id"},	{A32 | O32 | 0x81e8, "_OP_SUB_Rd_Id"},
		{0x81f0, "_OP_XOR_Rw_Iw"},			{A32 | 0x81f0, "_OP_XOR_Rw_Iw"},
		{O32 | 0x81f0, "_OP_XOR_Rd_Id"},	{A32 | O32 | 0x81f0, "_OP_XOR_Rd_Id"},
		{0x81f8, "_OP_CMP_Rw_Iw"},			{A32 | 0x81f8, "_OP_CMP_Rw_Iw"},
		{O32 | 0x81f8, "_OP_CMP_Rd_Id"},	{A32 | O32 | 0x81f8, "_OP_CMP_Rd_Id"},

		{0x8200, "_OP_ADD_Mb_Ib_A16"},		{A32 | 0x8200, "_OP_ADD_Mb_Ib_A32"},
		{O32 | 0x8200, "_OP_ADD_Mb_Ib_A16"},{A32 | O32 | 0x8200, "_OP_ADD_Mb_Ib_A32"},
		{0x8208, "_OP_OR_Mb_Ib_A16"},		{A32 | 0x8208, "_OP_OR_Mb_Ib_A32"},
		{O32 | 0x8208, "_OP_OR_Mb_Ib_A16"},	{A32 | O32 | 0x8208, "_OP_OR_Mb_Ib_A32"},
		{0x8210, "_OP_ADC_Mb_Ib_A16"},		{A32 | 0x8210, "_OP_ADC_Mb_Ib_A32"},
		{O32 | 0x8210, "_OP_ADC_Mb_Ib_A16"},{A32 | O32 | 0x8210, "_OP_ADC_Mb_Ib_A32"},
		{0x8218, "_OP_SBB_Mb_Ib_A16"},		{A32 | 0x8218, "_OP_SBB_Mb_Ib_A32"},
		{O32 | 0x8218, "_OP_SBB_Mb_Ib_A16"},{A32 | O32 | 0x8218, "_OP_SBB_Mb_Ib_A32"},
		{0x8220, "_OP_AND_Mb_Ib_A16"},		{A32 | 0x8220, "_OP_AND_Mb_Ib_A32"},
		{O32 | 0x8220, "_OP_AND_Mb_Ib_A16"},{A32 | O32 | 0x8220, "_OP_AND_Mb_Ib_A32"},
		{0x8228, "_OP_SUB_Mb_Ib_A16"},		{A32 | 0x8228, "_OP_SUB_Mb_Ib_A32"},
		{O32 | 0x8228, "_OP_SUB_Mb_Ib_A16"},{A32 | O32 | 0x8228, "_OP_SUB_Mb_Ib_A32"},
		{0x8230, "_OP_XOR_Mb_Ib_A16"},		{A32 | 0x8230, "_OP_XOR_Mb_Ib_A32"},
		{O32 | 0x8230, "_OP_XOR_Mb_Ib_A16"},{A32 | O32 | 0x8230, "_OP_XOR_Mb_Ib_A32"},
		{0x8238, "_OP_CMP_Mb_Ib_A16"},		{A32 | 0x8238, "_OP_CMP_Mb_Ib_A32"},
		{O32 | 0x8238, "_OP_CMP_Mb_Ib_A16"},{A32 | O32 | 0x8238, "_OP_CMP_Mb_Ib_A32"},
		{0x82c0, "_OP_ADD_Rb_Ib"},			{A32 | 0x82c0, "_OP_ADD_Rb_Ib"},
		{O32 | 0x82c0, "_OP_ADD_Rb_Ib"},	{A32 | O32 | 0x82c0, "_OP_ADD_Rb_Ib"},
		{0x82c8, "_OP_OR_Rb_Ib"},			{A32 | 0x82c8, "_OP_OR_Rb_Ib"},
		{O32 | 0x82c8, "_OP_OR_Rb_Ib"},		{A32 | O32 | 0x82c8, "_OP_OR_Rb_Ib"},
		{0x82d0, "_OP_ADC_Rb_Ib"},			{A32 | 0x82d0, "_OP_ADC_Rb_Ib"},
		{O32 | 0x82d0, "_OP_ADC_Rb_Ib"},	{A32 | O32 | 0x82d0, "_OP_ADC_Rb_Ib"},
		{0x82d8, "_OP_SBB_Rb_Ib"},			{A32 | 0x82d8, "_OP_SBB_Rb_Ib"},
		{O32 | 0x82d8, "_OP_SBB_Rb_Ib"},	{A32 | O32 | 0x82d8, "_OP_SBB_Rb_Ib"},
		{0x82e0, "_OP_AND_Rb_Ib"},			{A32 | 0x82e0, "_OP_AND_Rb_Ib"},
		{O32 | 0x82e0, "_OP_AND_Rb_Ib"},	{A32 | O32 | 0x82e0, "_OP_AND_Rb_Ib"},
		{0x82e8, "_OP_SUB_Rb_Ib"},			{A32 | 0x82e8, "_OP_SUB_Rb_Ib"},
		{O32 | 0x82e8, "_OP_SUB_Rb_Ib"},	{A32 | O32 | 0x82e8, "_OP_SUB_Rb_Ib"},
		{0x82f0, "_OP_XOR_Rb_Ib"},			{A32 | 0x82f0, "_OP_XOR_Rb_Ib"},
		{O32 | 0x82f0, "_OP_XOR_Rb_Ib"},	{A32 | O32 | 0x82f0, "_OP_XOR_Rb_Ib"},
		{0x82f8, "_OP_CMP_Rb_Ib"},			{A32 | 0x82f8, "_OP_CMP_Rb_Ib"},
		{O32 | 0x82f8, "_OP_CMP_Rb_Ib"},	{A32 | O32 | 0x82f8, "_OP_CMP_Rb_Ib"},

		{0x8300, "_OP_ADD_Mw_Ib_A16"},		{A32 | 0x8300, "_OP_ADD_Mw_Ib_A32"},
		{O32 | 0x8300, "_OP_ADD_Md_Ib_A16"},{A32 | O32 | 0x8300, "_OP_ADD_Md_Ib_A32"},
		{0x8308, "_OP_OR_Mw_Ib_A16"},		{A32 | 0x8308, "_OP_OR_Mw_Ib_A32"},
		{O32 | 0x8308, "_OP_OR_Md_Ib_A16"},	{A32 | O32 | 0x8308, "_OP_OR_Md_Ib_A32"},
		{0x8310, "_OP_ADC_Mw_Ib_A16"},		{A32 | 0x8310, "_OP_ADC_Mw_Ib_A32"},
		{O32 | 0x8310, "_OP_ADC_Md_Ib_A16"},{A32 | O32 | 0x8310, "_OP_ADC_Md_Ib_A32"},
		{0x8318, "_OP_SBB_Mw_Ib_A16"},		{A32 | 0x8318, "_OP_SBB_Mw_Ib_A32"},
		{O32 | 0x8318, "_OP_SBB_Md_Ib_A16"},{A32 | O32 | 0x8318, "_OP_SBB_Md_Ib_A32"},
		{0x8320, "_OP_AND_Mw_Ib_A16"},		{A32 | 0x8320, "_OP_AND_Mw_Ib_A32"},
		{O32 | 0x8320, "_OP_AND_Md_Ib_A16"},{A32 | O32 | 0x8320, "_OP_AND_Md_Ib_A32"},
		{0x8328, "_OP_SUB_Mw_Ib_A16"},		{A32 | 0x8328, "_OP_SUB_Mw_Ib_A32"},
		{O32 | 0x8328, "_OP_SUB_Md_Ib_A16"},{A32 | O32 | 0x8328, "_OP_SUB_Md_Ib_A32"},
		{0x8330, "_OP_XOR_Mw_Ib_A16"},		{A32 | 0x8330, "_OP_XOR_Mw_Ib_A32"},
		{O32 | 0x8330, "_OP_XOR_Md_Ib_A16"},{A32 | O32 | 0x8330, "_OP_XOR_Md_Ib_A32"},
		{0x8338, "_OP_CMP_Mw_Ib_A16"},		{A32 | 0x8338, "_OP_CMP_Mw_Ib_A32"},
		{O32 | 0x8338, "_OP_CMP_Md_Ib_A16"},{A32 | O32 | 0x8338, "_OP_CMP_Md_Ib_A32"},
		{0x83c0, "_OP_ADD_Rw_Ib"},			{A32 | 0x83c0, "_OP_ADD_Rw_Ib"},
		{O32 | 0x83c0, "_OP_ADD_Rd_Ib"},	{A32 | O32 | 0x83c0, "_OP_ADD_Rd_Ib"},
		{0x83c8, "_OP_OR_Rw_Ib"},			{A32 | 0x83c8, "_OP_OR_Rw_Ib"},
		{O32 | 0x83c8, "_OP_OR_Rd_Ib"},		{A32 | O32 | 0x83c8, "_OP_OR_Rd_Ib"},
		{0x83d0, "_OP_ADC_Rw_Ib"},			{A32 | 0x83d0, "_OP_ADC_Rw_Ib"},
		{O32 | 0x83d0, "_OP_ADC_Rd_Ib"},	{A32 | O32 | 0x83d0, "_OP_ADC_Rd_Ib"},
		{0x83d8, "_OP_SBB_Rw_Ib"},			{A32 | 0x83d8, "_OP_SBB_Rw_Ib"},
		{O32 | 0x83d8, "_OP_SBB_Rd_Ib"},	{A32 | O32 | 0x83d8, "_OP_SBB_Rd_Ib"},
		{0x83e0, "_OP_AND_Rw_Ib"},			{A32 | 0x83e0, "_OP_AND_Rw_Ib"},
		{O32 | 0x83e0, "_OP_AND_Rd_Ib"},	{A32 | O32 | 0x83e0, "_OP_AND_Rd_Ib"},
		{0x83e8, "_OP_SUB_Rw_Ib"},			{A32 | 0x83e8, "_OP_SUB_Rw_Ib"},
		{O32 | 0x83e8, "_OP_SUB_Rd_Ib"},	{A32 | O32 | 0x83e8, "_OP_SUB_Rd_Ib"},
		{0x83f0, "_OP_XOR_Rw_Ib"},			{A32 | 0x83f0, "_OP_XOR_Rw_Ib"},
		{O32 | 0x83f0, "_OP_XOR_Rd_Ib"},	{A32 | O32 | 0x83f0, "_OP_XOR_Rd_Ib"},
		{0x83f8, "_OP_CMP_Rw_Ib"},			{A32 | 0x83f8, "_OP_CMP_Rw_Ib"},
		{O32 | 0x83f8, "_OP_CMP_Rd_Ib"},	{A32 | O32 | 0x83f8, "_OP_CMP_Rd_Ib"},

		{0x8400, "_OP_TEST_Mb_Gb_A16"},			{A32 | 0x8400, "_OP_TEST_Mb_Gb_A32"},
		{O32 | 0x8400, "_OP_TEST_Mb_Gb_A16"},	{A32 | O32 | 0x8400, "_OP_TEST_Mb_Gb_A32"},
		{0x84c0, "_OP_TEST_Rb_Gb"},				{A32 | 0x84c0, "_OP_TEST_Rb_Gb"},
		{O32 | 0x84c0, "_OP_TEST_Rb_Gb"},		{A32 | O32 | 0x84c0, "_OP_TEST_Rb_Gb"},
		{0x8500, "_OP_TEST_Mw_Gw_A16"},			{A32 | 0x8500, "_OP_TEST_Mw_Gw_A32"},
		{O32 | 0x8500, "_OP_TEST_Md_Gd_A16"},	{A32 | O32 | 0x8500, "_OP_TEST_Md_Gd_A32"},
		{0x85c0, "_OP_TEST_Rw_Gw"},				{A32 | 0x85c0, "_OP_TEST_Rw_Gw"},
		{O32 | 0x85c0, "_OP_TEST_Rd_Gd"},		{A32 | O32 | 0x85c0, "_OP_TEST_Rd_Gd"},
		
		{0x8600, "_OP_XCHG_Mb_Gb_A16"},			{A32 | 0x8600, "_OP_XCHG_Mb_Gb_A32"},
		{O32 | 0x8600, "_OP_XCHG_Mb_Gb_A16"},	{A32 | O32 | 0x8600, "_OP_XCHG_Mb_Gb_A32"},
		{0x86c0, "_OP_XCHG_Rb_Gb"},				{A32 | 0x86c0, "_OP_XCHG_Rb_Gb"},
		{O32 | 0x86c0, "_OP_XCHG_Rb_Gb"},		{A32 | O32 | 0x86c0, "_OP_XCHG_Rb_Gb"},
		{0x8700, "_OP_XCHG_Mw_Gw_A16"},			{A32 | 0x8700, "_OP_XCHG_Mw_Gw_A32"},
		{O32 | 0x8700, "_OP_XCHG_Md_Gd_A16"},	{A32 | O32 | 0x8700, "_OP_XCHG_Md_Gd_A32"},
		{0x87c0, "_OP_XCHG_Rw_Gw"},				{A32 | 0x87c0, "_OP_XCHG_Rw_Gw"},
		{O32 | 0x87c0, "_OP_XCHG_Rd_Gd"},		{A32 | O32 | 0x87c0, "_OP_XCHG_Rd_Gd"},
		
		{0x8800, "_OP_MOV_Mb_Gb_A16"},		{A32 | 0x8800, "_OP_MOV_Mb_Gb_A32"},
		{O32 | 0x8800, "_OP_MOV_Mb_Gb_A16"},{A32 | O32 | 0x8800, "_OP_MOV_Mb_Gb_A32"},
		{0x88c0, "_OP_MOV_Rb_Gb"},			{A32 | 0x88c0, "_OP_MOV_Rb_Gb"},
		{O32 | 0x88c0, "_OP_MOV_Rb_Gb"},	{A32 | O32 | 0x88c0, "_OP_MOV_Rb_Gb"},
		{0x8900, "_OP_MOV_Mw_Gw_A16"},		{A32 | 0x8900, "_OP_MOV_Mw_Gw_A32"},
		{O32 | 0x8900, "_OP_MOV_Md_Gd_A16"},{A32 | O32 | 0x8900, "_OP_MOV_Md_Gd_A32"},
		{0x89c0, "_OP_MOV_Rw_Gw"},			{A32 | 0x89c0, "_OP_MOV_Rw_Gw"},
		{O32 | 0x89c0, "_OP_MOV_Rd_Gd"},	{A32 | O32 | 0x89c0, "_OP_MOV_Rd_Gd"},
		{0x8a00, "_OP_MOV_Gb_Mb_A16"},		{A32 | 0x8a00, "_OP_MOV_Gb_Mb_A32"},
		{O32 | 0x8a00, "_OP_MOV_Gb_Mb_A16"},{A32 | O32 | 0x8a00, "_OP_MOV_Gb_Mb_A32"},
		{0x8ac0, "_OP_MOV_Gb_Rb"},			{A32 | 0x8ac0, "_OP_MOV_Gb_Rb"},
		{O32 | 0x8ac0, "_OP_MOV_Gb_Rb"},	{A32 | O32 | 0x8ac0, "_OP_MOV_Gb_Rb"},
		{0x8b00, "_OP_MOV_Gw_Mw_A16"},		{A32 | 0x8b00, "_OP_MOV_Gw_Mw_A32"},
		{O32 | 0x8b00, "_OP_MOV_Gd_Md_A16"},{A32 | O32 | 0x8b00, "_OP_MOV_Gd_Md_A32"},
		{0x8bc0, "_OP_MOV_Gw_Rw"},			{A32 | 0x8bc0, "_OP_MOV_Gw_Rw"},
		{O32 | 0x8bc0, "_OP_MOV_Gd_Rd"},	{A32 | O32 | 0x8bc0, "_OP_MOV_Gd_Rd"},
		
		{0x8c00, "_OP_MOV_Mw_Sw_A16"},			{A32 | 0x8c00, "_OP_MOV_Mw_Sw_A32"},
		{O32 | 0x8c00, "_OP_MOV_Mw_Sw_A16"},	{A32 | O32 | 0x8c00, "_OP_MOV_Mw_Sw_A32"},
		{0x8cc0, "_OP_MOV_Rw_Sw"},				{A32 | 0x8cc0, "_OP_MOV_Rw_Sw"},
		{O32 | 0x8cc0, "_OP_MOV_Rw_Sw"},		{A32 | O32 | 0x8cc0, "_OP_MOV_Rw_Sw"},
		
		{0x8d00, "_OP_LEA_Gw_M_A16"},		{A32 | 0x8d00, "_OP_LEA_Gw_M_A32"},
		{O32 | 0x8d00, "_OP_LEA_Gd_M_A16"},	{A32 | O32 | 0x8d00, "_OP_LEA_Gd_M_A32"},
		
		{0x8e00, "_OP_MOV_Sw_Mw_A16"},			{A32 | 0x8e00, "_OP_MOV_Sw_Mw_A32"},
		{O32 | 0x8e00, "_OP_MOV_Sw_Mw_A16"},	{A32 | O32 | 0x8e00, "_OP_MOV_Sw_Mw_A32"},
		{0x8e18, "_OP_MOV_Sw_Mw_A16"},			{A32 | 0x8e18, "_OP_MOV_Sw_Mw_A32"},
		{O32 | 0x8e18, "_OP_MOV_Sw_Mw_A16"},	{A32 | O32 | 0x8e18, "_OP_MOV_Sw_Mw_A32"},
		{0x8e20, "_OP_MOV_Sw_Mw_A16"},			{A32 | 0x8e20, "_OP_MOV_Sw_Mw_A32"},
		{O32 | 0x8e20, "_OP_MOV_Sw_Mw_A16"},	{A32 | O32 | 0x8e20, "_OP_MOV_Sw_Mw_A32"},
		{0x8e28, "_OP_MOV_Sw_Mw_A16"},			{A32 | 0x8e28, "_OP_MOV_Sw_Mw_A32"},
		{O32 | 0x8e28, "_OP_MOV_Sw_Mw_A16"},	{A32 | O32 | 0x8e28, "_OP_MOV_Sw_Mw_A32"},
		{0x8e30, "_OP_MOV_Sw_Mw_A16"},			{A32 | 0x8e30, "_OP_MOV_Sw_Mw_A32"},
		{O32 | 0x8e30, "_OP_MOV_Sw_Mw_A16"},	{A32 | O32 | 0x8e30, "_OP_MOV_Sw_Mw_A32"},
		{0x8e38, "_OP_MOV_Sw_Mw_A16"},			{A32 | 0x8e38, "_OP_MOV_Sw_Mw_A32"},
		{O32 | 0x8e38, "_OP_MOV_Sw_Mw_A16"},	{A32 | O32 | 0x8e38, "_OP_MOV_Sw_Mw_A32"},
		{0x8ec0, "_OP_MOV_Sw_Rw"},				{A32 | 0x8ec0, "_OP_MOV_Sw_Rw"},
		{O32 | 0x8ec0, "_OP_MOV_Sw_Rw"},		{A32 | O32 | 0x8ec0, "_OP_MOV_Sw_Rw"},
		{0x8ed8, "_OP_MOV_Sw_Rw"},				{A32 | 0x8ed8, "_OP_MOV_Sw_Rw"},
		{O32 | 0x8ed8, "_OP_MOV_Sw_Rw"},		{A32 | O32 | 0x8ed8, "_OP_MOV_Sw_Rw"},
		{0x8ee0, "_OP_MOV_Sw_Rw"},				{A32 | 0x8ee0, "_OP_MOV_Sw_Rw"},
		{O32 | 0x8ee0, "_OP_MOV_Sw_Rw"},		{A32 | O32 | 0x8ee0, "_OP_MOV_Sw_Rw"},
		{0x8ee8, "_OP_MOV_Sw_Rw"},				{A32 | 0x8ee8, "_OP_MOV_Sw_Rw"},
		{O32 | 0x8ee8, "_OP_MOV_Sw_Rw"},		{A32 | O32 | 0x8ee8, "_OP_MOV_Sw_Rw"},
		{0x8ef0, "_OP_MOV_Sw_Rw"},				{A32 | 0x8ef0, "_OP_MOV_Sw_Rw"},
		{O32 | 0x8ef0, "_OP_MOV_Sw_Rw"},		{A32 | O32 | 0x8ef0, "_OP_MOV_Sw_Rw"},
		{0x8ef8, "_OP_MOV_Sw_Rw"},				{A32 | 0x8ef8, "_OP_MOV_Sw_Rw"},
		{O32 | 0x8ef8, "_OP_MOV_Sw_Rw"},		{A32 | O32 | 0x8ef8, "_OP_MOV_Sw_Rw"},

		{0x8f00, "_OP_POP_Mw_A16"},			{A32 | 0x8f00, "_OP_POP_Mw_A32"},
		{O32 | 0x8f00, "_OP_POP_Md_A16"},	{A32 | O32 | 0x8f00, "_OP_POP_Md_A32"},
		{0x8fc0, "_OP_POP_Rw"},				{A32 | 0x8fc0, "_OP_POP_Rw"},
		{O32 | 0x8fc0, "_OP_POP_Rd"},		{A32 | O32 | 0x8fc0, "_OP_POP_Rd"},
		
		{0x90, "_OP_NOP"},					{A32 | 0x90, "_OP_NOP"},
		{O32 | 0x90, "_OP_NOP"},			{A32 | O32 | 0x90, "_OP_NOP"},

		{0x91, "_OP_XCHG_AX_CX"},			{A32 | 0x91, "_OP_XCHG_AX_CX"},
		{O32 | 0x91, "_OP_XCHG_EAX_ECX"},	{A32 | O32 | 0x91, "_OP_XCHG_EAX_ECX"},
		{0x92, "_OP_XCHG_AX_DX"},			{A32 | 0x92, "_OP_XCHG_AX_DX"},
		{O32 | 0x92, "_OP_XCHG_EAX_EDX"},	{A32 | O32 | 0x92, "_OP_XCHG_EAX_EDX"},
		{0x93, "_OP_XCHG_AX_BX"},			{A32 | 0x93, "_OP_XCHG_AX_BX"},
		{O32 | 0x93, "_OP_XCHG_EAX_EBX"},	{A32 | O32 | 0x93, "_OP_XCHG_EAX_EBX"},
		{0x94, "_OP_XCHG_AX_SP"},			{A32 | 0x94, "_OP_XCHG_AX_SP"},
		{O32 | 0x94, "_OP_XCHG_EAX_ESP"},	{A32 | O32 | 0x94, "_OP_XCHG_EAX_ESP"},
		{0x95, "_OP_XCHG_AX_BP"},			{A32 | 0x95, "_OP_XCHG_AX_BP"},
		{O32 | 0x95, "_OP_XCHG_EAX_EBP"},	{A32 | O32 | 0x95, "_OP_XCHG_EAX_EBP"},
		{0x96, "_OP_XCHG_AX_SI"},			{A32 | 0x96, "_OP_XCHG_AX_SI"},
		{O32 | 0x96, "_OP_XCHG_EAX_ESI"},	{A32 | O32 | 0x96, "_OP_XCHG_EAX_ESI"},
		{0x97, "_OP_XCHG_AX_DI"},			{A32 | 0x97, "_OP_XCHG_AX_DI"},
		{O32 | 0x97, "_OP_XCHG_EAX_EDI"},	{A32 | O32 | 0x97, "_OP_XCHG_EAX_EDI"},

		{0x98, "_OP_CBW"},				{A32 | 0x98, "_OP_CBW"},
		{O32 | 0x98, "_OP_CWDE"},		{A32 | O32 | 0x98, "_OP_CWDE"},
		{0x99, "_OP_CWD"},				{A32 | 0x99, "_OP_CWD"},
		{O32 | 0x99, "_OP_CDQ"},		{A32 | O32 | 0x99, "_OP_CDQ"},
		
		{0x9b, "_OP_WAIT"},				{A32 | 0x9b, "_OP_WAIT"},
		{O32 | 0x9b, "_OP_WAIT"},		{A32 | O32 | 0x9b, "_OP_WAIT"},
#endif

		{0x9c, "_OP_PUSHF_O16"},			{A32 | 0x9c, "_OP_PUSHF_O16"},
		{O32 | 0x9c, "_OP_PUSHF_O32"},		{A32 | O32 | 0x9c, "_OP_PUSHF_O32"},
	
#ifndef USE_MICROCODE
		{0x9e, "_OP_SAHF"},				{A32 | 0x9e, "_OP_SAHF"},
		{O32 | 0x9e, "_OP_SAHF"},		{A32 | O32 | 0x9e, "_OP_SAHF"},
		{0x9f, "_OP_LAHF"},				{A32 | 0x9f, "_OP_LAHF"},
		{O32 | 0x9f, "_OP_LAHF"},		{A32 | O32 | 0x9f, "_OP_LAHF"},
		
		{0xa0, "_OP_MOV_AL_Ob_A16"},		{A32 | 0xa0, "_OP_MOV_AL_Ob_A32"},
		{O32 | 0xa0, "_OP_MOV_AL_Ob_A16"},	{A32 | O32 | 0xa0, "_OP_MOV_AL_Ob_A32"},
		{0xa1, "_OP_MOV_AX_Ow_A16"},		{A32 | 0xa1, "_OP_MOV_AX_Ow_A32"},
		{O32 | 0xa1, "_OP_MOV_EAX_Od_A16"},	{A32 | O32 | 0xa1, "_OP_MOV_EAX_Od_A32"},
		{0xa2, "_OP_MOV_Ob_AL_A16"},		{A32 | 0xa2, "_OP_MOV_Ob_AL_A32"},
		{O32 | 0xa2, "_OP_MOV_Ob_AL_A16"},	{A32 | O32 | 0xa2, "_OP_MOV_Ob_AL_A32"},
		{0xa3, "_OP_MOV_Ow_AX_A16"},		{A32 | 0xa3, "_OP_MOV_Ow_AX_A32"},
		{O32 | 0xa3, "_OP_MOV_Od_EAX_A16"},	{A32 | O32 | 0xa3, "_OP_MOV_Od_EAX_A32"},
#endif

#ifndef USE_MICROCODE
		{0xa4, "_OP_MOVS_Yb_Xb_A16"},					{A32 | 0xa4, "_OP_MOVS_Yb_Xb_A32"},
		{O32 | 0xa4, "_OP_MOVS_Yb_Xb_A16"},				{A32 | O32 | 0xa4, "_OP_MOVS_Yb_Xb_A32"},
#endif

		{REP | 0xa4, "_OP_REP_MOVS_Yb_Xb_A16"},			{A32 | REP | 0xa4, "_OP_REP_MOVS_Yb_Xb_A32"},
		{O32 | REP | 0xa4, "_OP_REP_MOVS_Yb_Xb_A16"},	{A32 | O32 | REP | 0xa4, "_OP_REP_MOVS_Yb_Xb_A32"},
		{REPNE | 0xa4, "_OP_REP_MOVS_Yb_Xb_A16"},		{A32 | REPNE | 0xa4, "_OP_REP_MOVS_Yb_Xb_A32"},
		{O32 | REPNE | 0xa4, "_OP_REP_MOVS_Yb_Xb_A16"},	{A32 | O32 | REPNE | 0xa4, "_OP_REP_MOVS_Yb_Xb_A32"},

#ifndef USE_MICROCODE
		{0xa5, "_OP_MOVS_Yw_Xw_A16"},					{A32 | 0xa5, "_OP_MOVS_Yw_Xw_A32"},
		{O32 | 0xa5, "_OP_MOVS_Yd_Xd_A16"},				{A32 | O32 | 0xa5, "_OP_MOVS_Yd_Xd_A32"},
#endif

		{REP | 0xa5, "_OP_REP_MOVS_Yw_Xw_A16"},			{A32 | REP | 0xa5, "_OP_REP_MOVS_Yw_Xw_A32"},
		{O32 | REP | 0xa5, "_OP_REP_MOVS_Yd_Xd_A16"},	{A32 | O32 | REP | 0xa5, "_OP_REP_MOVS_Yd_Xd_A32"},
		{REPNE | 0xa5, "_OP_REP_MOVS_Yw_Xw_A16"},		{A32 | REPNE | 0xa5, "_OP_REP_MOVS_Yw_Xw_A32"},
		{O32 | REPNE | 0xa5, "_OP_REP_MOVS_Yd_Xd_A16"},	{A32 | O32 | REPNE | 0xa5, "_OP_REP_MOVS_Yd_Xd_A32"},

#ifndef USE_MICROCODE
		{0xa6, "_OP_CMPS_Yb_Xb_A16"},						{A32 | 0xa6, "_OP_CMPS_Yb_Xb_A32"},
		{O32 | 0xa6, "_OP_CMPS_Yb_Xb_A16"},					{A32 | O32 | 0xa6, "_OP_CMPS_Yb_Xb_A32"},
#endif

		{REP | 0xa6, "_OP_REPE_CMPS_Yb_Xb_A16"},			{A32 | REP | 0xa6, "_OP_REPE_CMPS_Yb_Xb_A32"},
		{O32 | REP | 0xa6, "_OP_REPE_CMPS_Yb_Xb_A16"},		{A32 | O32 | REP | 0xa6, "_OP_REPE_CMPS_Yb_Xb_A32"},
		{REPNE | 0xa6, "_OP_REPNE_CMPS_Yb_Xb_A16"},			{A32 | REPNE | 0xa6, "_OP_REPNE_CMPS_Yb_Xb_A32"},
		{O32 | REPNE | 0xa6, "_OP_REPNE_CMPS_Yb_Xb_A16"},	{A32 | O32 | REPNE | 0xa6, "_OP_REPNE_CMPS_Yb_Xb_A32"},

#ifndef USE_MICROCODE
		{0xa7, "_OP_CMPS_Yw_Xw_A16"},						{A32 | 0xa7, "_OP_CMPS_Yw_Xw_A32"},
		{O32 | 0xa7, "_OP_CMPS_Yd_Xd_A16"},					{A32 | O32 | 0xa7, "_OP_CMPS_Yd_Xd_A32"},
#endif

		{REP | 0xa7, "_OP_REPE_CMPS_Yw_Xw_A16"},			{A32 | REP | 0xa7, "_OP_REPE_CMPS_Yw_Xw_A32"},
		{O32 | REP | 0xa7, "_OP_REPE_CMPS_Yd_Xd_A16"},		{A32 | O32 | REP | 0xa7, "_OP_REPE_CMPS_Yd_Xd_A32"},
		{REPNE | 0xa7, "_OP_REPNE_CMPS_Yw_Xw_A16"},			{A32 | REPNE | 0xa7, "_OP_REPNE_CMPS_Yw_Xw_A32"},
		{O32 | REPNE | 0xa7, "_OP_REPNE_CMPS_Yd_Xd_A16"},	{A32 | O32 | REPNE | 0xa7, "_OP_REPNE_CMPS_Yd_Xd_A32"},

#ifndef USE_MICROCODE
		{0xaa, "_OP_STOS_Yb_AL_A16"},					{A32 | 0xaa, "_OP_STOS_Yb_AL_A32"},
		{O32 | 0xaa, "_OP_STOS_Yb_AL_A16"},				{A32 | O32 | 0xaa, "_OP_STOS_Yb_AL_A32"},
#endif

		{REP | 0xaa, "_OP_REP_STOS_Yb_AL_A16"},			{A32 | REP | 0xaa, "_OP_REP_STOS_Yb_AL_A32"},
		{O32 | REP | 0xaa, "_OP_REP_STOS_Yb_AL_A16"},	{A32 | O32 | REP | 0xaa, "_OP_REP_STOS_Yb_AL_A32"},
		{REPNE | 0xaa, "_OP_REP_STOS_Yb_AL_A16"},		{A32 | REPNE | 0xaa, "_OP_REP_STOS_Yb_AL_A32"},
		{O32 | REPNE | 0xaa, "_OP_REP_STOS_Yb_AL_A16"},	{A32 | O32 | REPNE | 0xaa, "_OP_REP_STOS_Yb_AL_A32"},

#ifndef USE_MICROCODE
		{0xab, "_OP_STOS_Yw_AX_A16"},					{A32 | 0xab, "_OP_STOS_Yw_AX_A32"},
		{O32 | 0xab, "_OP_STOS_Yd_EAX_A16"},			{A32 | O32 | 0xab, "_OP_STOS_Yd_EAX_A32"},
#endif

		{REP | 0xab, "_OP_REP_STOS_Yw_AX_A16"},			{A32 | REP | 0xab, "_OP_REP_STOS_Yw_AX_A32"},
		{O32 | REP | 0xab, "_OP_REP_STOS_Yd_EAX_A16"},	{A32 | O32 | REP | 0xab, "_OP_REP_STOS_Yd_EAX_A32"},
		{REPNE | 0xab, "_OP_REP_STOS_Yw_AX_A16"},		{A32 | REPNE | 0xab, "_OP_REP_STOS_Yw_AX_A32"},
		{O32 | REPNE | 0xab, "_OP_REP_STOS_Yd_EAX_A16"},{A32 | O32 | REPNE | 0xab, "_OP_REP_STOS_Yd_EAX_A32"},

#ifndef USE_MICROCODE
		{0xac, "_OP_LODS_AL_Xb_A16"},					{A32 | 0xac, "_OP_LODS_AL_Xb_A32"},
		{O32 | 0xac, "_OP_LODS_AL_Xb_A16"},				{A32 | O32 | 0xac, "_OP_LODS_AL_Xb_A32"},
#endif

		{REP | 0xac, "_OP_REP_LODS_AL_Xb_A16"},			{A32 | REP | 0xac, "_OP_REP_LODS_AL_Xb_A32"},
		{O32 | REP | 0xac, "_OP_REP_LODS_AL_Xb_A16"},	{A32 | O32 | REP | 0xac, "_OP_REP_LODS_AL_Xb_A32"},
		{REPNE | 0xac, "_OP_REP_LODS_AL_Xb_A16"},		{A32 | REPNE | 0xac, "_OP_REP_LODS_AL_Xb_A32"},
		{O32 | REPNE | 0xac, "_OP_REP_LODS_AL_Xb_A16"},	{A32 | O32 | REPNE | 0xac, "_OP_REP_LODS_AL_Xb_A32"},

#ifndef USE_MICROCODE
		{0xad, "_OP_LODS_AX_Xw_A16"},					{A32 | 0xad, "_OP_LODS_AX_Xw_A32"},
		{O32 | 0xad, "_OP_LODS_EAX_Xd_A16"},			{A32 | O32 | 0xad, "_OP_LODS_EAX_Xd_A32"},
#endif

		{REP | 0xad, "_OP_REP_LODS_AX_Xw_A16"},			{A32 | REP | 0xad, "_OP_REP_LODS_AX_Xw_A32"},
		{O32 | REP | 0xad, "_OP_REP_LODS_EAX_Xd_A16"},	{A32 | O32 | REP | 0xad, "_OP_REP_LODS_EAX_Xd_A32"},
		{REPNE | 0xad, "_OP_REP_LODS_AX_Xw_A16"},		{A32 | REPNE | 0xad, "_OP_REP_LODS_AX_Xw_A32"},
		{O32 | REPNE | 0xad, "_OP_REP_LODS_EAX_Xd_A16"},{A32 | O32 | REPNE | 0xad, "_OP_REP_LODS_EAX_Xd_A32"},

#ifndef USE_MICROCODE
		{0xae, "_OP_SCAS_AL_Yb_A16"},						{A32 | 0xae, "_OP_SCAS_AL_Yb_A32"},
		{O32 | 0xae, "_OP_SCAS_AL_Yb_A16"},					{A32 | O32 | 0xae, "_OP_SCAS_AL_Yb_A32"},
#endif

		{REP | 0xae, "_OP_REPE_SCAS_AL_Yb_A16"},			{A32 | REP | 0xae, "_OP_REPE_SCAS_AL_Yb_A32"},
		{O32 | REP | 0xae, "_OP_REPE_SCAS_AL_Yb_A16"},		{A32 | O32 | REP | 0xae, "_OP_REPE_SCAS_AL_Yb_A32"},
		{REPNE | 0xae, "_OP_REPNE_SCAS_AL_Yb_A16"},			{A32 | REPNE | 0xae, "_OP_REPNE_SCAS_AL_Yb_A32"},
		{O32 | REPNE | 0xae, "_OP_REPNE_SCAS_AL_Yb_A16"},	{A32 | O32 | REPNE | 0xae, "_OP_REPNE_SCAS_AL_Yb_A32"},

#ifndef USE_MICROCODE
		{0xaf, "_OP_SCAS_AX_Yw_A16"},						{A32 | 0xaf, "_OP_SCAS_AX_Yw_A32"},
		{O32 | 0xaf, "_OP_SCAS_EAX_Yd_A16"},				{A32 | O32 | 0xaf, "_OP_SCAS_EAX_Yd_A32"},
#endif

		{REP | 0xaf, "_OP_REPE_SCAS_AX_Yw_A16"},			{A32 | REP | 0xaf, "_OP_REPE_SCAS_AX_Yw_A32"},
		{O32 | REP | 0xaf, "_OP_REPE_SCAS_EAX_Yd_A16"},		{A32 | O32 | REP | 0xaf, "_OP_REPE_SCAS_EAX_Yd_A32"},
		{REPNE | 0xaf, "_OP_REPNE_SCAS_AX_Yw_A16"},			{A32 | REPNE | 0xaf, "_OP_REPNE_SCAS_AX_Yw_A32"},
		{O32 | REPNE | 0xaf, "_OP_REPNE_SCAS_EAX_Yd_A16"},	{A32 | O32 | REPNE | 0xaf, "_OP_REPNE_SCAS_EAX_Yd_A32"},

#ifndef USE_MICROCODE
		{0xa8, "_OP_TEST_AL_Ib"},			{A32 | 0xa8, "_OP_TEST_AL_Ib"},
		{O32 | 0xa8, "_OP_TEST_AL_Ib"},		{A32 | O32 | 0xa8, "_OP_TEST_AL_Ib"},
		{0xa9, "_OP_TEST_AX_Iw"},			{A32 | 0xa9, "_OP_TEST_AX_Iw"},
		{O32 | 0xa9, "_OP_TEST_EAX_Id"},	{A32 | O32 | 0xa9, "_OP_TEST_EAX_Id"},

		{0xb0, "_OP_MOV_AL_Ib"},			{A32 | 0xb0, "_OP_MOV_AL_Ib"},
		{O32 | 0xb0, "_OP_MOV_AL_Ib"},		{A32 | O32 | 0xb0, "_OP_MOV_AL_Ib"},
		{0xb1, "_OP_MOV_CL_Ib"},			{A32 | 0xb1, "_OP_MOV_CL_Ib"},
		{O32 | 0xb1, "_OP_MOV_CL_Ib"},		{A32 | O32 | 0xb1, "_OP_MOV_CL_Ib"},
		{0xb2, "_OP_MOV_DL_Ib"},			{A32 | 0xb2, "_OP_MOV_DL_Ib"},
		{O32 | 0xb2, "_OP_MOV_DL_Ib"},		{A32 | O32 | 0xb2, "_OP_MOV_DL_Ib"},
		{0xb3, "_OP_MOV_BL_Ib"},			{A32 | 0xb3, "_OP_MOV_BL_Ib"},
		{O32 | 0xb3, "_OP_MOV_BL_Ib"},		{A32 | O32 | 0xb3, "_OP_MOV_BL_Ib"},
		{0xb4, "_OP_MOV_AH_Ib"},			{A32 | 0xb4, "_OP_MOV_AH_Ib"},
		{O32 | 0xb4, "_OP_MOV_AH_Ib"},		{A32 | O32 | 0xb4, "_OP_MOV_AH_Ib"},
		{0xb5, "_OP_MOV_CH_Ib"},			{A32 | 0xb5, "_OP_MOV_CH_Ib"},
		{O32 | 0xb5, "_OP_MOV_CH_Ib"},		{A32 | O32 | 0xb5, "_OP_MOV_CH_Ib"},
		{0xb6, "_OP_MOV_DH_Ib"},			{A32 | 0xb6, "_OP_MOV_DH_Ib"},
		{O32 | 0xb6, "_OP_MOV_DH_Ib"},		{A32 | O32 | 0xb6, "_OP_MOV_DH_Ib"},
		{0xb7, "_OP_MOV_BH_Ib"},			{A32 | 0xb7, "_OP_MOV_BH_Ib"},
		{O32 | 0xb7, "_OP_MOV_BH_Ib"},		{A32 | O32 | 0xb7, "_OP_MOV_BH_Ib"},

		{0xb8, "_OP_MOV_AX_Iw"},			{A32 | 0xb8, "_OP_MOV_AX_Iw"},
		{O32 | 0xb8, "_OP_MOV_EAX_Id"},		{A32 | O32 | 0xb8, "_OP_MOV_EAX_Id"},
		{0xb9, "_OP_MOV_CX_Iw"},			{A32 | 0xb9, "_OP_MOV_CX_Iw"},
		{O32 | 0xb9, "_OP_MOV_ECX_Id"},		{A32 | O32 | 0xb9, "_OP_MOV_ECX_Id"},
		{0xba, "_OP_MOV_DX_Iw"},			{A32 | 0xba, "_OP_MOV_DX_Iw"},
		{O32 | 0xba, "_OP_MOV_EDX_Id"},		{A32 | O32 | 0xba, "_OP_MOV_EDX_Id"},
		{0xbb, "_OP_MOV_BX_Iw"},			{A32 | 0xbb, "_OP_MOV_BX_Iw"},
		{O32 | 0xbb, "_OP_MOV_EBX_Id"},		{A32 | O32 | 0xbb, "_OP_MOV_EBX_Id"},
		{0xbc, "_OP_MOV_SP_Iw"},			{A32 | 0xbc, "_OP_MOV_SP_Iw"},
		{O32 | 0xbc, "_OP_MOV_ESP_Id"},		{A32 | O32 | 0xbc, "_OP_MOV_ESP_Id"},
		{0xbd, "_OP_MOV_BP_Iw"},			{A32 | 0xbd, "_OP_MOV_BP_Iw"},
		{O32 | 0xbd, "_OP_MOV_EBP_Id"},		{A32 | O32 | 0xbd, "_OP_MOV_EBP_Id"},
		{0xbe, "_OP_MOV_SI_Iw"},			{A32 | 0xbe, "_OP_MOV_SI_Iw"},
		{O32 | 0xbe, "_OP_MOV_ESI_Id"},		{A32 | O32 | 0xbe, "_OP_MOV_ESI_Id"},
		{0xbf, "_OP_MOV_DI_Iw"},			{A32 | 0xbf, "_OP_MOV_DI_Iw"},
		{O32 | 0xbf, "_OP_MOV_EDI_Id"},		{A32 | O32 | 0xbf, "_OP_MOV_EDI_Id"},
		
		{0xc000, "_OP_ROL_Mb_Ib_A16"},			{A32 | 0xc000, "_OP_ROL_Mb_Ib_A32"},
		{O32 | 0xc000, "_OP_ROL_Mb_Ib_A16"},	{A32 | O32 | 0xc000, "_OP_ROL_Mb_Ib_A32"},
		{0xc008, "_OP_ROR_Mb_Ib_A16"},			{A32 | 0xc008, "_OP_ROR_Mb_Ib_A32"},
		{O32 | 0xc008, "_OP_ROR_Mb_Ib_A16"},	{A32 | O32 | 0xc008, "_OP_ROR_Mb_Ib_A32"},
		{0xc010, "_OP_RCL_Mb_Ib_A16"},			{A32 | 0xc010, "_OP_RCL_Mb_Ib_A32"},
		{O32 | 0xc010, "_OP_RCL_Mb_Ib_A16"},	{A32 | O32 | 0xc010, "_OP_RCL_Mb_Ib_A32"},
		{0xc018, "_OP_RCR_Mb_Ib_A16"},			{A32 | 0xc018, "_OP_RCR_Mb_Ib_A32"},
		{O32 | 0xc018, "_OP_RCR_Mb_Ib_A16"},	{A32 | O32 | 0xc018, "_OP_RCR_Mb_Ib_A32"},
		{0xc020, "_OP_SHL_Mb_Ib_A16"},			{A32 | 0xc020, "_OP_SHL_Mb_Ib_A32"},
		{O32 | 0xc020, "_OP_SHL_Mb_Ib_A16"},	{A32 | O32 | 0xc020, "_OP_SHL_Mb_Ib_A32"},
		{0xc028, "_OP_SHR_Mb_Ib_A16"},			{A32 | 0xc028, "_OP_SHR_Mb_Ib_A32"},
		{O32 | 0xc028, "_OP_SHR_Mb_Ib_A16"},	{A32 | O32 | 0xc028, "_OP_SHR_Mb_Ib_A32"},
		{0xc030, "_OP_SHL_Mb_Ib_A16"},			{A32 | 0xc030, "_OP_SHL_Mb_Ib_A32"},
		{O32 | 0xc030, "_OP_SHL_Mb_Ib_A16"},	{A32 | O32 | 0xc030, "_OP_SHL_Mb_Ib_A32"},
		{0xc038, "_OP_SAR_Mb_Ib_A16"},			{A32 | 0xc038, "_OP_SAR_Mb_Ib_A32"},
		{O32 | 0xc038, "_OP_SAR_Mb_Ib_A16"},	{A32 | O32 | 0xc038, "_OP_SAR_Mb_Ib_A32"},

		{0xc0c0, "_OP_ROL_Rb_Ib"},			{A32 | 0xc0c0, "_OP_ROL_Rb_Ib"},
		{O32 | 0xc0c0, "_OP_ROL_Rb_Ib"},	{A32 | O32 | 0xc0c0, "_OP_ROL_Rb_Ib"},
		{0xc0c8, "_OP_ROR_Rb_Ib"},			{A32 | 0xc0c8, "_OP_ROR_Rb_Ib"},
		{O32 | 0xc0c8, "_OP_ROR_Rb_Ib"},	{A32 | O32 | 0xc0c8, "_OP_ROR_Rb_Ib"},
		{0xc0d0, "_OP_RCL_Rb_Ib"},			{A32 | 0xc0d0, "_OP_RCL_Rb_Ib"},
		{O32 | 0xc0d0, "_OP_RCL_Rb_Ib"},	{A32 | O32 | 0xc0d0, "_OP_RCL_Rb_Ib"},
		{0xc0d8, "_OP_RCR_Rb_Ib"},			{A32 | 0xc0d8, "_OP_RCR_Rb_Ib"},
		{O32 | 0xc0d8, "_OP_RCR_Rb_Ib"},	{A32 | O32 | 0xc0d8, "_OP_RCR_Rb_Ib"},
		{0xc0e0, "_OP_SHL_Rb_Ib"},			{A32 | 0xc0e0, "_OP_SHL_Rb_Ib"},
		{O32 | 0xc0e0, "_OP_SHL_Rb_Ib"},	{A32 | O32 | 0xc0e0, "_OP_SHL_Rb_Ib"},
		{0xc0e8, "_OP_SHR_Rb_Ib"},			{A32 | 0xc0e8, "_OP_SHR_Rb_Ib"},
		{O32 | 0xc0e8, "_OP_SHR_Rb_Ib"},	{A32 | O32 | 0xc0e8, "_OP_SHR_Rb_Ib"},
		{0xc0f0, "_OP_SHL_Rb_Ib"},			{A32 | 0xc0f0, "_OP_SHL_Rb_Ib"},
		{O32 | 0xc0f0, "_OP_SHL_Rb_Ib"},	{A32 | O32 | 0xc0f0, "_OP_SHL_Rb_Ib"},
		{0xc0f8, "_OP_SAR_Rb_Ib"},			{A32 | 0xc0f8, "_OP_SAR_Rb_Ib"},
		{O32 | 0xc0f8, "_OP_SAR_Rb_Ib"},	{A32 | O32 | 0xc0f8, "_OP_SAR_Rb_Ib"},

		{0xc100, "_OP_ROL_Mw_Ib_A16"},			{A32 | 0xc100, "_OP_ROL_Mw_Ib_A32"},
		{O32 | 0xc100, "_OP_ROL_Md_Ib_A16"},	{A32 | O32 | 0xc100, "_OP_ROL_Md_Ib_A32"},
		{0xc108, "_OP_ROR_Mw_Ib_A16"},			{A32 | 0xc108, "_OP_ROR_Mw_Ib_A32"},
		{O32 | 0xc108, "_OP_ROR_Md_Ib_A16"},	{A32 | O32 | 0xc108, "_OP_ROR_Md_Ib_A32"},
		{0xc110, "_OP_RCL_Mw_Ib_A16"},			{A32 | 0xc110, "_OP_RCL_Mw_Ib_A32"},
		{O32 | 0xc110, "_OP_RCL_Md_Ib_A16"},	{A32 | O32 | 0xc110, "_OP_RCL_Md_Ib_A32"},
		{0xc118, "_OP_RCR_Mw_Ib_A16"},			{A32 | 0xc118, "_OP_RCR_Mw_Ib_A32"},
		{O32 | 0xc118, "_OP_RCR_Md_Ib_A16"},	{A32 | O32 | 0xc118, "_OP_RCR_Md_Ib_A32"},
		{0xc120, "_OP_SHL_Mw_Ib_A16"},			{A32 | 0xc120, "_OP_SHL_Mw_Ib_A32"},
		{O32 | 0xc120, "_OP_SHL_Md_Ib_A16"},	{A32 | O32 | 0xc120, "_OP_SHL_Md_Ib_A32"},
		{0xc128, "_OP_SHR_Mw_Ib_A16"},			{A32 | 0xc128, "_OP_SHR_Mw_Ib_A32"},
		{O32 | 0xc128, "_OP_SHR_Md_Ib_A16"},	{A32 | O32 | 0xc128, "_OP_SHR_Md_Ib_A32"},
		{0xc130, "_OP_SHL_Mw_Ib_A16"},			{A32 | 0xc130, "_OP_SHL_Mw_Ib_A32"},
		{O32 | 0xc130, "_OP_SHL_Md_Ib_A16"},	{A32 | O32 | 0xc130, "_OP_SHL_Md_Ib_A32"},
		{0xc138, "_OP_SAR_Mw_Ib_A16"},			{A32 | 0xc138, "_OP_SAR_Mw_Ib_A32"},
		{O32 | 0xc138, "_OP_SAR_Md_Ib_A16"},	{A32 | O32 | 0xc138, "_OP_SAR_Md_Ib_A32"},

		{0xc1c0, "_OP_ROL_Rw_Ib"},			{A32 | 0xc1c0, "_OP_ROL_Rw_Ib"},
		{O32 | 0xc1c0, "_OP_ROL_Rd_Ib"},	{A32 | O32 | 0xc1c0, "_OP_ROL_Rd_Ib"},
		{0xc1c8, "_OP_ROR_Rw_Ib"},			{A32 | 0xc1c8, "_OP_ROR_Rw_Ib"},
		{O32 | 0xc1c8, "_OP_ROR_Rd_Ib"},	{A32 | O32 | 0xc1c8, "_OP_ROR_Rd_Ib"},
		{0xc1d0, "_OP_RCL_Rw_Ib"},			{A32 | 0xc1d0, "_OP_RCL_Rw_Ib"},
		{O32 | 0xc1d0, "_OP_RCL_Rd_Ib"},	{A32 | O32 | 0xc1d0, "_OP_RCL_Rd_Ib"},
		{0xc1d8, "_OP_RCR_Rw_Ib"},			{A32 | 0xc1d8, "_OP_RCR_Rw_Ib"},
		{O32 | 0xc1d8, "_OP_RCR_Rd_Ib"},	{A32 | O32 | 0xc1d8, "_OP_RCR_Rd_Ib"},
		{0xc1e0, "_OP_SHL_Rw_Ib"},			{A32 | 0xc1e0, "_OP_SHL_Rw_Ib"},
		{O32 | 0xc1e0, "_OP_SHL_Rd_Ib"},	{A32 | O32 | 0xc1e0, "_OP_SHL_Rd_Ib"},
		{0xc1e8, "_OP_SHR_Rw_Ib"},			{A32 | 0xc1e8, "_OP_SHR_Rw_Ib"},
		{O32 | 0xc1e8, "_OP_SHR_Rd_Ib"},	{A32 | O32 | 0xc1e8, "_OP_SHR_Rd_Ib"},
		{0xc1f0, "_OP_SHL_Rw_Ib"},			{A32 | 0xc1f0, "_OP_SHL_Rw_Ib"},
		{O32 | 0xc1f0, "_OP_SHL_Rd_Ib"},	{A32 | O32 | 0xc1f0, "_OP_SHL_Rd_Ib"},
		{0xc1f8, "_OP_SAR_Rw_Ib"},			{A32 | 0xc1f8, "_OP_SAR_Rw_Ib"},
		{O32 | 0xc1f8, "_OP_SAR_Rd_Ib"},	{A32 | O32 | 0xc1f8, "_OP_SAR_Rd_Ib"},

		{0xc2, "_OP_RET_Iw_O16"},			{A32 | 0xc2, "_OP_RET_Iw_O16"},
		{O32 | 0xc2, "_OP_RET_Iw_O32"},		{A32 | O32 | 0xc2, "_OP_RET_Iw_O32"},
		{0xc3, "_OP_RET_O16"},				{A32 | 0xc3, "_OP_RET_O16"},
		{O32 | 0xc3, "_OP_RET_O32"},		{A32 | O32 | 0xc3, "_OP_RET_O32"},
		
		{0xc400, "_OP_LES_Gw_Mp_A16"},			{A32 | 0xc400, "_OP_LES_Gw_Mp_A32"},
		{O32 | 0xc400, "_OP_LES_Gd_Mp_A16"},	{A32 | O32 | 0xc400, "_OP_LES_Gd_Mp_A32"},

		{0xc500, "_OP_LDS_Gw_Mp_A16"},			{A32 | 0xc500, "_OP_LDS_Gw_Mp_A32"},
		{O32 | 0xc500, "_OP_LDS_Gd_Mp_A16"},	{A32 | O32 | 0xc500, "_OP_LDS_Gd_Mp_A32"},

		{0xc600, "_OP_MOV_Mb_Ib_A16"},			{A32 | 0xc600, "_OP_MOV_Mb_Ib_A32"},
		{O32 | 0xc600, "_OP_MOV_Mb_Ib_A16"},	{A32 | O32 | 0xc600, "_OP_MOV_Mb_Ib_A32"},
		{0xc6c0, "_OP_MOV_Rb_Ib"},				{A32 | 0xc6c0, "_OP_MOV_Rb_Ib"},
		{O32 | 0xc6c0, "_OP_MOV_Rb_Ib"},		{A32 | O32 | 0xc6c0, "_OP_MOV_Rb_Ib"},
		{0xc700, "_OP_MOV_Mw_Iw_A16"},			{A32 | 0xc700, "_OP_MOV_Mw_Iw_A32"},
		{O32 | 0xc700, "_OP_MOV_Md_Id_A16"},	{A32 | O32 | 0xc700, "_OP_MOV_Md_Id_A32"},
		{0xc7c0, "_OP_MOV_Rw_Iw"},				{A32 | 0xc7c0, "_OP_MOV_Rw_Iw"},
		{O32 | 0xc7c0, "_OP_MOV_Rd_Id"},		{A32 | O32 | 0xc7c0, "_OP_MOV_Rd_Id"},
#endif

		{0xc8, "_OP_ENTER_O16"},				{A32 | 0xc8, "_OP_ENTER_O16"},
		{O32 | 0xc8, "_OP_ENTER_O32"},			{A32 | O32 | 0xc8, "_OP_ENTER_O32"},

#ifndef USE_MICROCODE
		{0xc9, "_OP_LEAVE_O16"},				{A32 | 0xc9, "_OP_LEAVE_O16"},
		{O32 | 0xc9, "_OP_LEAVE_O32"},			{A32 | O32 | 0xc9, "_OP_LEAVE_O32"},
		
		{0xd000, "_OP_ROL_Mb_1_A16"},			{A32 | 0xd000, "_OP_ROL_Mb_1_A32"},
		{O32 | 0xd000, "_OP_ROL_Mb_1_A16"},	{A32 | O32 | 0xd000, "_OP_ROL_Mb_1_A32"},
		{0xd008, "_OP_ROR_Mb_1_A16"},			{A32 | 0xd008, "_OP_ROR_Mb_1_A32"},
		{O32 | 0xd008, "_OP_ROR_Mb_1_A16"},	{A32 | O32 | 0xd008, "_OP_ROR_Mb_1_A32"},
		{0xd010, "_OP_RCL_Mb_1_A16"},			{A32 | 0xd010, "_OP_RCL_Mb_1_A32"},
		{O32 | 0xd010, "_OP_RCL_Mb_1_A16"},	{A32 | O32 | 0xd010, "_OP_RCL_Mb_1_A32"},
		{0xd018, "_OP_RCR_Mb_1_A16"},			{A32 | 0xd018, "_OP_RCR_Mb_1_A32"},
		{O32 | 0xd018, "_OP_RCR_Mb_1_A16"},	{A32 | O32 | 0xd018, "_OP_RCR_Mb_1_A32"},
		{0xd020, "_OP_SHL_Mb_1_A16"},			{A32 | 0xd020, "_OP_SHL_Mb_1_A32"},
		{O32 | 0xd020, "_OP_SHL_Mb_1_A16"},	{A32 | O32 | 0xd020, "_OP_SHL_Mb_1_A32"},
		{0xd028, "_OP_SHR_Mb_1_A16"},			{A32 | 0xd028, "_OP_SHR_Mb_1_A32"},
		{O32 | 0xd028, "_OP_SHR_Mb_1_A16"},	{A32 | O32 | 0xd028, "_OP_SHR_Mb_1_A32"},
		{0xd030, "_OP_SHL_Mb_1_A16"},			{A32 | 0xd030, "_OP_SHL_Mb_1_A32"},
		{O32 | 0xd030, "_OP_SHL_Mb_1_A16"},	{A32 | O32 | 0xd030, "_OP_SHL_Mb_1_A32"},
		{0xd038, "_OP_SAR_Mb_1_A16"},			{A32 | 0xd038, "_OP_SAR_Mb_1_A32"},
		{O32 | 0xd038, "_OP_SAR_Mb_1_A16"},	{A32 | O32 | 0xd038, "_OP_SAR_Mb_1_A32"},

		{0xd0c0, "_OP_ROL_Rb_1"},			{A32 | 0xd0c0, "_OP_ROL_Rb_1"},
		{O32 | 0xd0c0, "_OP_ROL_Rb_1"},	{A32 | O32 | 0xd0c0, "_OP_ROL_Rb_1"},
		{0xd0c8, "_OP_ROR_Rb_1"},			{A32 | 0xd0c8, "_OP_ROR_Rb_1"},
		{O32 | 0xd0c8, "_OP_ROR_Rb_1"},	{A32 | O32 | 0xd0c8, "_OP_ROR_Rb_1"},
		{0xd0d0, "_OP_RCL_Rb_1"},			{A32 | 0xd0d0, "_OP_RCL_Rb_1"},
		{O32 | 0xd0d0, "_OP_RCL_Rb_1"},	{A32 | O32 | 0xd0d0, "_OP_RCL_Rb_1"},
		{0xd0d8, "_OP_RCR_Rb_1"},			{A32 | 0xd0d8, "_OP_RCR_Rb_1"},
		{O32 | 0xd0d8, "_OP_RCR_Rb_1"},	{A32 | O32 | 0xd0d8, "_OP_RCR_Rb_1"},
		{0xd0e0, "_OP_SHL_Rb_1"},			{A32 | 0xd0e0, "_OP_SHL_Rb_1"},
		{O32 | 0xd0e0, "_OP_SHL_Rb_1"},	{A32 | O32 | 0xd0e0, "_OP_SHL_Rb_1"},
		{0xd0e8, "_OP_SHR_Rb_1"},			{A32 | 0xd0e8, "_OP_SHR_Rb_1"},
		{O32 | 0xd0e8, "_OP_SHR_Rb_1"},	{A32 | O32 | 0xd0e8, "_OP_SHR_Rb_1"},
		{0xd0f0, "_OP_SHL_Rb_1"},			{A32 | 0xd0f0, "_OP_SHL_Rb_1"},
		{O32 | 0xd0f0, "_OP_SHL_Rb_1"},	{A32 | O32 | 0xd0f0, "_OP_SHL_Rb_1"},
		{0xd0f8, "_OP_SAR_Rb_1"},			{A32 | 0xd0f8, "_OP_SAR_Rb_1"},
		{O32 | 0xd0f8, "_OP_SAR_Rb_1"},	{A32 | O32 | 0xd0f8, "_OP_SAR_Rb_1"},

		{0xd100, "_OP_ROL_Mw_1_A16"},			{A32 | 0xd100, "_OP_ROL_Mw_1_A32"},
		{O32 | 0xd100, "_OP_ROL_Md_1_A16"},	{A32 | O32 | 0xd100, "_OP_ROL_Md_1_A32"},
		{0xd108, "_OP_ROR_Mw_1_A16"},			{A32 | 0xd108, "_OP_ROR_Mw_1_A32"},
		{O32 | 0xd108, "_OP_ROR_Md_1_A16"},	{A32 | O32 | 0xd108, "_OP_ROR_Md_1_A32"},
		{0xd110, "_OP_RCL_Mw_1_A16"},			{A32 | 0xd110, "_OP_RCL_Mw_1_A32"},
		{O32 | 0xd110, "_OP_RCL_Md_1_A16"},	{A32 | O32 | 0xd110, "_OP_RCL_Md_1_A32"},
		{0xd118, "_OP_RCR_Mw_1_A16"},			{A32 | 0xd118, "_OP_RCR_Mw_1_A32"},
		{O32 | 0xd118, "_OP_RCR_Md_1_A16"},	{A32 | O32 | 0xd118, "_OP_RCR_Md_1_A32"},
		{0xd120, "_OP_SHL_Mw_1_A16"},			{A32 | 0xd120, "_OP_SHL_Mw_1_A32"},
		{O32 | 0xd120, "_OP_SHL_Md_1_A16"},	{A32 | O32 | 0xd120, "_OP_SHL_Md_1_A32"},
		{0xd128, "_OP_SHR_Mw_1_A16"},			{A32 | 0xd128, "_OP_SHR_Mw_1_A32"},
		{O32 | 0xd128, "_OP_SHR_Md_1_A16"},	{A32 | O32 | 0xd128, "_OP_SHR_Md_1_A32"},
		{0xd130, "_OP_SHL_Mw_1_A16"},			{A32 | 0xd130, "_OP_SHL_Mw_1_A32"},
		{O32 | 0xd130, "_OP_SHL_Md_1_A16"},	{A32 | O32 | 0xd130, "_OP_SHL_Md_1_A32"},
		{0xd138, "_OP_SAR_Mw_1_A16"},			{A32 | 0xd138, "_OP_SAR_Mw_1_A32"},
		{O32 | 0xd138, "_OP_SAR_Md_1_A16"},	{A32 | O32 | 0xd138, "_OP_SAR_Md_1_A32"},

		{0xd1c0, "_OP_ROL_Rw_1"},			{A32 | 0xd1c0, "_OP_ROL_Rw_1"},
		{O32 | 0xd1c0, "_OP_ROL_Rd_1"},	{A32 | O32 | 0xd1c0, "_OP_ROL_Rd_1"},
		{0xd1c8, "_OP_ROR_Rw_1"},			{A32 | 0xd1c8, "_OP_ROR_Rw_1"},
		{O32 | 0xd1c8, "_OP_ROR_Rd_1"},	{A32 | O32 | 0xd1c8, "_OP_ROR_Rd_1"},
		{0xd1d0, "_OP_RCL_Rw_1"},			{A32 | 0xd1d0, "_OP_RCL_Rw_1"},
		{O32 | 0xd1d0, "_OP_RCL_Rd_1"},	{A32 | O32 | 0xd1d0, "_OP_RCL_Rd_1"},
		{0xd1d8, "_OP_RCR_Rw_1"},			{A32 | 0xd1d8, "_OP_RCR_Rw_1"},
		{O32 | 0xd1d8, "_OP_RCR_Rd_1"},	{A32 | O32 | 0xd1d8, "_OP_RCR_Rd_1"},
		{0xd1e0, "_OP_SHL_Rw_1"},			{A32 | 0xd1e0, "_OP_SHL_Rw_1"},
		{O32 | 0xd1e0, "_OP_SHL_Rd_1"},	{A32 | O32 | 0xd1e0, "_OP_SHL_Rd_1"},
		{0xd1e8, "_OP_SHR_Rw_1"},			{A32 | 0xd1e8, "_OP_SHR_Rw_1"},
		{O32 | 0xd1e8, "_OP_SHR_Rd_1"},	{A32 | O32 | 0xd1e8, "_OP_SHR_Rd_1"},
		{0xd1f0, "_OP_SHL_Rw_1"},			{A32 | 0xd1f0, "_OP_SHL_Rw_1"},
		{O32 | 0xd1f0, "_OP_SHL_Rd_1"},	{A32 | O32 | 0xd1f0, "_OP_SHL_Rd_1"},
		{0xd1f8, "_OP_SAR_Rw_1"},			{A32 | 0xd1f8, "_OP_SAR_Rw_1"},
		{O32 | 0xd1f8, "_OP_SAR_Rd_1"},	{A32 | O32 | 0xd1f8, "_OP_SAR_Rd_1"},

		{0xd200, "_OP_ROL_Mb_CL_A16"},			{A32 | 0xd200, "_OP_ROL_Mb_CL_A32"},
		{O32 | 0xd200, "_OP_ROL_Mb_CL_A16"},	{A32 | O32 | 0xd200, "_OP_ROL_Mb_CL_A32"},
		{0xd208, "_OP_ROR_Mb_CL_A16"},			{A32 | 0xd208, "_OP_ROR_Mb_CL_A32"},
		{O32 | 0xd208, "_OP_ROR_Mb_CL_A16"},	{A32 | O32 | 0xd208, "_OP_ROR_Mb_CL_A32"},
		{0xd210, "_OP_RCL_Mb_CL_A16"},			{A32 | 0xd210, "_OP_RCL_Mb_CL_A32"},
		{O32 | 0xd210, "_OP_RCL_Mb_CL_A16"},	{A32 | O32 | 0xd210, "_OP_RCL_Mb_CL_A32"},
		{0xd218, "_OP_RCR_Mb_CL_A16"},			{A32 | 0xd218, "_OP_RCR_Mb_CL_A32"},
		{O32 | 0xd218, "_OP_RCR_Mb_CL_A16"},	{A32 | O32 | 0xd218, "_OP_RCR_Mb_CL_A32"},
		{0xd220, "_OP_SHL_Mb_CL_A16"},			{A32 | 0xd220, "_OP_SHL_Mb_CL_A32"},
		{O32 | 0xd220, "_OP_SHL_Mb_CL_A16"},	{A32 | O32 | 0xd220, "_OP_SHL_Mb_CL_A32"},
		{0xd228, "_OP_SHR_Mb_CL_A16"},			{A32 | 0xd228, "_OP_SHR_Mb_CL_A32"},
		{O32 | 0xd228, "_OP_SHR_Mb_CL_A16"},	{A32 | O32 | 0xd228, "_OP_SHR_Mb_CL_A32"},
		{0xd230, "_OP_SHL_Mb_CL_A16"},			{A32 | 0xd230, "_OP_SHL_Mb_CL_A32"},
		{O32 | 0xd230, "_OP_SHL_Mb_CL_A16"},	{A32 | O32 | 0xd230, "_OP_SHL_Mb_CL_A32"},
		{0xd238, "_OP_SAR_Mb_CL_A16"},			{A32 | 0xd238, "_OP_SAR_Mb_CL_A32"},
		{O32 | 0xd238, "_OP_SAR_Mb_CL_A16"},	{A32 | O32 | 0xd238, "_OP_SAR_Mb_CL_A32"},

		{0xd2c0, "_OP_ROL_Rb_CL"},			{A32 | 0xd2c0, "_OP_ROL_Rb_CL"},
		{O32 | 0xd2c0, "_OP_ROL_Rb_CL"},	{A32 | O32 | 0xd2c0, "_OP_ROL_Rb_CL"},
		{0xd2c8, "_OP_ROR_Rb_CL"},			{A32 | 0xd2c8, "_OP_ROR_Rb_CL"},
		{O32 | 0xd2c8, "_OP_ROR_Rb_CL"},	{A32 | O32 | 0xd2c8, "_OP_ROR_Rb_CL"},
		{0xd2d0, "_OP_RCL_Rb_CL"},			{A32 | 0xd2d0, "_OP_RCL_Rb_CL"},
		{O32 | 0xd2d0, "_OP_RCL_Rb_CL"},	{A32 | O32 | 0xd2d0, "_OP_RCL_Rb_CL"},
		{0xd2d8, "_OP_RCR_Rb_CL"},			{A32 | 0xd2d8, "_OP_RCR_Rb_CL"},
		{O32 | 0xd2d8, "_OP_RCR_Rb_CL"},	{A32 | O32 | 0xd2d8, "_OP_RCR_Rb_CL"},
		{0xd2e0, "_OP_SHL_Rb_CL"},			{A32 | 0xd2e0, "_OP_SHL_Rb_CL"},
		{O32 | 0xd2e0, "_OP_SHL_Rb_CL"},	{A32 | O32 | 0xd2e0, "_OP_SHL_Rb_CL"},
		{0xd2e8, "_OP_SHR_Rb_CL"},			{A32 | 0xd2e8, "_OP_SHR_Rb_CL"},
		{O32 | 0xd2e8, "_OP_SHR_Rb_CL"},	{A32 | O32 | 0xd2e8, "_OP_SHR_Rb_CL"},
		{0xd2f0, "_OP_SHL_Rb_CL"},			{A32 | 0xd2f0, "_OP_SHL_Rb_CL"},
		{O32 | 0xd2f0, "_OP_SHL_Rb_CL"},	{A32 | O32 | 0xd2f0, "_OP_SHL_Rb_CL"},
		{0xd2f8, "_OP_SAR_Rb_CL"},			{A32 | 0xd2f8, "_OP_SAR_Rb_CL"},
		{O32 | 0xd2f8, "_OP_SAR_Rb_CL"},	{A32 | O32 | 0xd2f8, "_OP_SAR_Rb_CL"},

		{0xd300, "_OP_ROL_Mw_CL_A16"},			{A32 | 0xd300, "_OP_ROL_Mw_CL_A32"},
		{O32 | 0xd300, "_OP_ROL_Md_CL_A16"},	{A32 | O32 | 0xd300, "_OP_ROL_Md_CL_A32"},
		{0xd308, "_OP_ROR_Mw_CL_A16"},			{A32 | 0xd308, "_OP_ROR_Mw_CL_A32"},
		{O32 | 0xd308, "_OP_ROR_Md_CL_A16"},	{A32 | O32 | 0xd308, "_OP_ROR_Md_CL_A32"},
		{0xd310, "_OP_RCL_Mw_CL_A16"},			{A32 | 0xd310, "_OP_RCL_Mw_CL_A32"},
		{O32 | 0xd310, "_OP_RCL_Md_CL_A16"},	{A32 | O32 | 0xd310, "_OP_RCL_Md_CL_A32"},
		{0xd318, "_OP_RCR_Mw_CL_A16"},			{A32 | 0xd318, "_OP_RCR_Mw_CL_A32"},
		{O32 | 0xd318, "_OP_RCR_Md_CL_A16"},	{A32 | O32 | 0xd318, "_OP_RCR_Md_CL_A32"},
		{0xd320, "_OP_SHL_Mw_CL_A16"},			{A32 | 0xd320, "_OP_SHL_Mw_CL_A32"},
		{O32 | 0xd320, "_OP_SHL_Md_CL_A16"},	{A32 | O32 | 0xd320, "_OP_SHL_Md_CL_A32"},
		{0xd328, "_OP_SHR_Mw_CL_A16"},			{A32 | 0xd328, "_OP_SHR_Mw_CL_A32"},
		{O32 | 0xd328, "_OP_SHR_Md_CL_A16"},	{A32 | O32 | 0xd328, "_OP_SHR_Md_CL_A32"},
		{0xd330, "_OP_SHL_Mw_CL_A16"},			{A32 | 0xd330, "_OP_SHL_Mw_CL_A32"},
		{O32 | 0xd330, "_OP_SHL_Md_CL_A16"},	{A32 | O32 | 0xd330, "_OP_SHL_Md_CL_A32"},
		{0xd338, "_OP_SAR_Mw_CL_A16"},			{A32 | 0xd338, "_OP_SAR_Mw_CL_A32"},
		{O32 | 0xd338, "_OP_SAR_Md_CL_A16"},	{A32 | O32 | 0xd338, "_OP_SAR_Md_CL_A32"},

		{0xd3c0, "_OP_ROL_Rw_CL"},			{A32 | 0xd3c0, "_OP_ROL_Rw_CL"},
		{O32 | 0xd3c0, "_OP_ROL_Rd_CL"},	{A32 | O32 | 0xd3c0, "_OP_ROL_Rd_CL"},
		{0xd3c8, "_OP_ROR_Rw_CL"},			{A32 | 0xd3c8, "_OP_ROR_Rw_CL"},
		{O32 | 0xd3c8, "_OP_ROR_Rd_CL"},	{A32 | O32 | 0xd3c8, "_OP_ROR_Rd_CL"},
		{0xd3d0, "_OP_RCL_Rw_CL"},			{A32 | 0xd3d0, "_OP_RCL_Rw_CL"},
		{O32 | 0xd3d0, "_OP_RCL_Rd_CL"},	{A32 | O32 | 0xd3d0, "_OP_RCL_Rd_CL"},
		{0xd3d8, "_OP_RCR_Rw_CL"},			{A32 | 0xd3d8, "_OP_RCR_Rw_CL"},
		{O32 | 0xd3d8, "_OP_RCR_Rd_CL"},	{A32 | O32 | 0xd3d8, "_OP_RCR_Rd_CL"},
		{0xd3e0, "_OP_SHL_Rw_CL"},			{A32 | 0xd3e0, "_OP_SHL_Rw_CL"},
		{O32 | 0xd3e0, "_OP_SHL_Rd_CL"},	{A32 | O32 | 0xd3e0, "_OP_SHL_Rd_CL"},
		{0xd3e8, "_OP_SHR_Rw_CL"},			{A32 | 0xd3e8, "_OP_SHR_Rw_CL"},
		{O32 | 0xd3e8, "_OP_SHR_Rd_CL"},	{A32 | O32 | 0xd3e8, "_OP_SHR_Rd_CL"},
		{0xd3f0, "_OP_SHL_Rw_CL"},			{A32 | 0xd3f0, "_OP_SHL_Rw_CL"},
		{O32 | 0xd3f0, "_OP_SHL_Rd_CL"},	{A32 | O32 | 0xd3f0, "_OP_SHL_Rd_CL"},
		{0xd3f8, "_OP_SAR_Rw_CL"},			{A32 | 0xd3f8, "_OP_SAR_Rw_CL"},
		{O32 | 0xd3f8, "_OP_SAR_Rd_CL"},	{A32 | O32 | 0xd3f8, "_OP_SAR_Rd_CL"},
		
		{0xd7, "_OP_XLAT_A16"},				{A32 | 0xd7, "_OP_XLAT_A32"},
		{O32 | 0xd7, "_OP_XLAT_A16"},		{A32 | O32 | 0xd7, "_OP_XLAT_A32"},

		{0xd800, "_OP_FADD_SINGLE_A16"},		{A32 | 0xd800, "_OP_FADD_SINGLE_A32"},
		{O32 | 0xd800, "_OP_FADD_SINGLE_A16"},	{A32 | O32 | 0xd800, "_OP_FADD_SINGLE_A32"},
		{0xd808, "_OP_FMUL_SINGLE_A16"},		{A32 | 0xd808, "_OP_FMUL_SINGLE_A32"},
		{O32 | 0xd808, "_OP_FMUL_SINGLE_A16"},	{A32 | O32 | 0xd808, "_OP_FMUL_SINGLE_A32"},
		{0xd810, "_OP_FCOM_SINGLE_A16"},		{A32 | 0xd810, "_OP_FCOM_SINGLE_A32"},
		{O32 | 0xd810, "_OP_FCOM_SINGLE_A16"},	{A32 | O32 | 0xd810, "_OP_FCOM_SINGLE_A32"},
		{0xd818, "_OP_FCOMP_SINGLE_A16"},		{A32 | 0xd818, "_OP_FCOMP_SINGLE_A32"},
		{O32 | 0xd818, "_OP_FCOMP_SINGLE_A16"},	{A32 | O32 | 0xd818, "_OP_FCOMP_SINGLE_A32"},
		{0xd820, "_OP_FSUB_SINGLE_A16"},		{A32 | 0xd820, "_OP_FSUB_SINGLE_A32"},
		{O32 | 0xd820, "_OP_FSUB_SINGLE_A16"},	{A32 | O32 | 0xd820, "_OP_FSUB_SINGLE_A32"},
		{0xd828, "_OP_FSUBR_SINGLE_A16"},		{A32 | 0xd828, "_OP_FSUBR_SINGLE_A32"},
		{O32 | 0xd828, "_OP_FSUBR_SINGLE_A16"},	{A32 | O32 | 0xd828, "_OP_FSUBR_SINGLE_A32"},
		{0xd830, "_OP_FDIV_SINGLE_A16"},		{A32 | 0xd830, "_OP_FDIV_SINGLE_A32"},
		{O32 | 0xd830, "_OP_FDIV_SINGLE_A16"},	{A32 | O32 | 0xd830, "_OP_FDIV_SINGLE_A32"},
		{0xd838, "_OP_FDIVR_SINGLE_A16"},		{A32 | 0xd838, "_OP_FDIVR_SINGLE_A32"},
		{O32 | 0xd838, "_OP_FDIVR_SINGLE_A16"},	{A32 | O32 | 0xd838, "_OP_FDIVR_SINGLE_A32"},

		{0xd900, "_OP_FLD_SINGLE_A16"},			{A32 | 0xd900, "_OP_FLD_SINGLE_A32"},
		{O32 | 0xd900, "_OP_FLD_SINGLE_A16"},	{A32 | O32 | 0xd900, "_OP_FLD_SINGLE_A32"},
		{0xd910, "_OP_FST_SINGLE_A16"},			{A32 | 0xd910, "_OP_FST_SINGLE_A32"},
		{O32 | 0xd910, "_OP_FST_SINGLE_A16"},	{A32 | O32 | 0xd910, "_OP_FST_SINGLE_A32"},
		{0xd918, "_OP_FSTP_SINGLE_A16"},		{A32 | 0xd918, "_OP_FSTP_SINGLE_A32"},
		{O32 | 0xd918, "_OP_FSTP_SINGLE_A16"},	{A32 | O32 | 0xd918, "_OP_FSTP_SINGLE_A32"},
#endif

		{0xd920, "_OP_FLDENV_A16O16"},			{A32 | 0xd920, "_OP_FLDENV_A32O16"},
		{O32 | 0xd920, "_OP_FLDENV_A16O32"},	{A32 | O32 | 0xd920, "_OP_FLDENV_A32O32"},

#ifndef USE_MICROCODE
		{0xd928, "_OP_FLDCW_A16"},				{A32 | 0xd928, "_OP_FLDCW_A32"},
		{O32 | 0xd928, "_OP_FLDCW_A16"},		{A32 | O32 | 0xd928, "_OP_FLDCW_A32"},
#endif

		{0xd930, "_OP_FNSTENV_A16O16"},			{A32 | 0xd930, "_OP_FNSTENV_A32O16"},
		{O32 | 0xd930, "_OP_FNSTENV_A16O32"},	{A32 | O32 | 0xd930, "_OP_FNSTENV_A32O32"},

#ifndef USE_MICROCODE
		{0xd938, "_OP_FNSTCW_A16"},				{A32 | 0xd938, "_OP_FNSTCW_A32"},
		{O32 | 0xd938, "_OP_FNSTCW_A16"},		{A32 | O32 | 0xd938, "_OP_FNSTCW_A32"},

		{0xda00, "_OP_FIADD_DWORD_A16"},		{A32 | 0xda00, "_OP_FIADD_DWORD_A32"},
		{O32 | 0xda00, "_OP_FIADD_DWORD_A16"},	{A32 | O32 | 0xda00, "_OP_FIADD_DWORD_A32"},
		{0xda08, "_OP_FIMUL_DWORD_A16"},		{A32 | 0xda08, "_OP_FIMUL_DWORD_A32"},
		{O32 | 0xda08, "_OP_FIMUL_DWORD_A16"},	{A32 | O32 | 0xda08, "_OP_FIMUL_DWORD_A32"},
		{0xda10, "_OP_FICOM_DWORD_A16"},		{A32 | 0xda10, "_OP_FICOM_DWORD_A32"},
		{O32 | 0xda10, "_OP_FICOM_DWORD_A16"},	{A32 | O32 | 0xda10, "_OP_FICOM_DWORD_A32"},
		{0xda18, "_OP_FICOMP_DWORD_A16"},		{A32 | 0xda18, "_OP_FICOMP_DWORD_A32"},
		{O32 | 0xda18, "_OP_FICOMP_DWORD_A16"},	{A32 | O32 | 0xda18, "_OP_FICOMP_DWORD_A32"},
		{0xda20, "_OP_FISUB_DWORD_A16"},		{A32 | 0xda20, "_OP_FISUB_DWORD_A32"},
		{O32 | 0xda20, "_OP_FISUB_DWORD_A16"},	{A32 | O32 | 0xda20, "_OP_FISUB_DWORD_A32"},
		{0xda28, "_OP_FISUBR_DWORD_A16"},		{A32 | 0xda28, "_OP_FISUBR_DWORD_A32"},
		{O32 | 0xda28, "_OP_FISUBR_DWORD_A16"},	{A32 | O32 | 0xda28, "_OP_FISUBR_DWORD_A32"},
		{0xda30, "_OP_FIDIV_DWORD_A16"},		{A32 | 0xda30, "_OP_FIDIV_DWORD_A32"},
		{O32 | 0xda30, "_OP_FIDIV_DWORD_A16"},	{A32 | O32 | 0xda30, "_OP_FIDIV_DWORD_A32"},
		{0xda38, "_OP_FIDIVR_DWORD_A16"},		{A32 | 0xda38, "_OP_FIDIVR_DWORD_A32"},
		{O32 | 0xda38, "_OP_FIDIVR_DWORD_A16"},	{A32 | O32 | 0xda38, "_OP_FIDIVR_DWORD_A32"},

		{0xdb00, "_OP_FILD_DWORD_A16"},			{A32 | 0xdb00, "_OP_FILD_DWORD_A32"},
		{O32 | 0xdb00, "_OP_FILD_DWORD_A16"},	{A32 | O32 | 0xdb00, "_OP_FILD_DWORD_A32"},
		{0xdb08, "_OP_FISTTP_DWORD_A16"},		{A32 | 0xdb08, "_OP_FISTTP_DWORD_A32"},
		{O32 | 0xdb08, "_OP_FISTTP_DWORD_A16"},	{A32 | O32 | 0xdb08, "_OP_FISTTP_DWORD_A32"},
		{0xdb10, "_OP_FIST_DWORD_A16"},			{A32 | 0xdb10, "_OP_FIST_DWORD_A32"},
		{O32 | 0xdb10, "_OP_FIST_DWORD_A16"},	{A32 | O32 | 0xdb10, "_OP_FIST_DWORD_A32"},
		{0xdb18, "_OP_FISTP_DWORD_A16"},		{A32 | 0xdb18, "_OP_FISTP_DWORD_A32"},
		{O32 | 0xdb18, "_OP_FISTP_DWORD_A16"},	{A32 | O32 | 0xdb18, "_OP_FISTP_DWORD_A32"},
		{0xdb28, "_OP_FLD_EXTENDED_A16"},		{A32 | 0xdb28, "_OP_FLD_EXTENDED_A32"},
		{O32 | 0xdb28, "_OP_FLD_EXTENDED_A16"},	{A32 | O32 | 0xdb28, "_OP_FLD_EXTENDED_A32"},
		{0xdb38, "_OP_FSTP_EXTENDED_A16"},		{A32 | 0xdb38, "_OP_FSTP_EXTENDED_A32"},
		{O32 | 0xdb38, "_OP_FSTP_EXTENDED_A16"},{A32 | O32 | 0xdb38, "_OP_FSTP_EXTENDED_A32"},
		
		{0xdc00, "_OP_FADD_DOUBLE_A16"},		{A32 | 0xdc00, "_OP_FADD_DOUBLE_A32"},
		{O32 | 0xdc00, "_OP_FADD_DOUBLE_A16"},	{A32 | O32 | 0xdc00, "_OP_FADD_DOUBLE_A32"},
		{0xdc08, "_OP_FMUL_DOUBLE_A16"},		{A32 | 0xdc08, "_OP_FMUL_DOUBLE_A32"},
		{O32 | 0xdc08, "_OP_FMUL_DOUBLE_A16"},	{A32 | O32 | 0xdc08, "_OP_FMUL_DOUBLE_A32"},
		{0xdc10, "_OP_FCOM_DOUBLE_A16"},		{A32 | 0xdc10, "_OP_FCOM_DOUBLE_A32"},
		{O32 | 0xdc10, "_OP_FCOM_DOUBLE_A16"},	{A32 | O32 | 0xdc10, "_OP_FCOM_DOUBLE_A32"},
		{0xdc18, "_OP_FCOMP_DOUBLE_A16"},		{A32 | 0xdc18, "_OP_FCOMP_DOUBLE_A32"},
		{O32 | 0xdc18, "_OP_FCOMP_DOUBLE_A16"},	{A32 | O32 | 0xdc18, "_OP_FCOMP_DOUBLE_A32"},
		{0xdc20, "_OP_FSUB_DOUBLE_A16"},		{A32 | 0xdc20, "_OP_FSUB_DOUBLE_A32"},
		{O32 | 0xdc20, "_OP_FSUB_DOUBLE_A16"},	{A32 | O32 | 0xdc20, "_OP_FSUB_DOUBLE_A32"},
		{0xdc28, "_OP_FSUBR_DOUBLE_A16"},		{A32 | 0xdc28, "_OP_FSUBR_DOUBLE_A32"},
		{O32 | 0xdc28, "_OP_FSUBR_DOUBLE_A16"},	{A32 | O32 | 0xdc28, "_OP_FSUBR_DOUBLE_A32"},
		{0xdc30, "_OP_FDIV_DOUBLE_A16"},		{A32 | 0xdc30, "_OP_FDIV_DOUBLE_A32"},
		{O32 | 0xdc30, "_OP_FDIV_DOUBLE_A16"},	{A32 | O32 | 0xdc30, "_OP_FDIV_DOUBLE_A32"},
		{0xdc38, "_OP_FDIVR_DOUBLE_A16"},		{A32 | 0xdc38, "_OP_FDIVR_DOUBLE_A32"},
		{O32 | 0xdc38, "_OP_FDIVR_DOUBLE_A16"},	{A32 | O32 | 0xdc38, "_OP_FDIVR_DOUBLE_A32"},

		{0xdd00, "_OP_FLD_DOUBLE_A16"},			{A32 | 0xdd00, "_OP_FLD_DOUBLE_A32"},
		{O32 | 0xdd00, "_OP_FLD_DOUBLE_A16"},	{A32 | O32 | 0xdd00, "_OP_FLD_DOUBLE_A32"},
		{0xdd08, "_OP_FISTTP_QWORD_A16"},		{A32 | 0xdd08, "_OP_FISTTP_QWORD_A32"},
		{O32 | 0xdd08, "_OP_FISTTP_QWORD_A16"},	{A32 | O32 | 0xdd08, "_OP_FISTTP_QWORD_A32"},
		{0xdd10, "_OP_FST_DOUBLE_A16"},			{A32 | 0xdd10, "_OP_FST_DOUBLE_A32"},
		{O32 | 0xdd10, "_OP_FST_DOUBLE_A16"},	{A32 | O32 | 0xdd10, "_OP_FST_DOUBLE_A32"},
		{0xdd18, "_OP_FSTP_DOUBLE_A16"},		{A32 | 0xdd18, "_OP_FSTP_DOUBLE_A32"},
		{O32 | 0xdd18, "_OP_FSTP_DOUBLE_A16"},	{A32 | O32 | 0xdd18, "_OP_FSTP_DOUBLE_A32"},
#endif

		{0xdd20, "_OP_FRSTOR_A16O16"},			{A32 | 0xdd20, "_OP_FRSTOR_A32O16"},
		{O32 | 0xdd20, "_OP_FRSTOR_A16O32"},	{A32 | O32 | 0xdd20, "_OP_FRSTOR_A32O32"},
		{0xdd30, "_OP_FNSAVE_A16O16"},			{A32 | 0xdd30, "_OP_FNSAVE_A32O16"},
		{O32 | 0xdd30, "_OP_FNSAVE_A16O32"},	{A32 | O32 | 0xdd30, "_OP_FNSAVE_A32O32"},

#ifndef USE_MICROCODE
		{0xdd38, "_OP_FNSTSW_A16"},				{A32 | 0xdd38, "_OP_FNSTSW_A32"},
		{O32 | 0xdd38, "_OP_FNSTSW_A16"},		{A32 | O32 | 0xdd38, "_OP_FNSTSW_A32"},

		{0xde00, "_OP_FIADD_WORD_A16"},			{A32 | 0xde00, "_OP_FIADD_WORD_A32"},
		{O32 | 0xde00, "_OP_FIADD_WORD_A16"},	{A32 | O32 | 0xde00, "_OP_FIADD_WORD_A32"},
		{0xde08, "_OP_FIMUL_WORD_A16"},			{A32 | 0xde08, "_OP_FIMUL_WORD_A32"},
		{O32 | 0xde08, "_OP_FIMUL_WORD_A16"},	{A32 | O32 | 0xde08, "_OP_FIMUL_WORD_A32"},
		{0xde10, "_OP_FICOM_WORD_A16"},			{A32 | 0xde10, "_OP_FICOM_WORD_A32"},
		{O32 | 0xde10, "_OP_FICOM_WORD_A16"},	{A32 | O32 | 0xde10, "_OP_FICOM_WORD_A32"},
		{0xde18, "_OP_FICOMP_WORD_A16"},		{A32 | 0xde18, "_OP_FICOMP_WORD_A32"},
		{O32 | 0xde18, "_OP_FICOMP_WORD_A16"},	{A32 | O32 | 0xde18, "_OP_FICOMP_WORD_A32"},
		{0xde20, "_OP_FISUB_WORD_A16"},			{A32 | 0xde20, "_OP_FISUB_WORD_A32"},
		{O32 | 0xde20, "_OP_FISUB_WORD_A16"},	{A32 | O32 | 0xde20, "_OP_FISUB_WORD_A32"},
		{0xde28, "_OP_FISUBR_WORD_A16"},		{A32 | 0xde28, "_OP_FISUBR_WORD_A32"},
		{O32 | 0xde28, "_OP_FISUBR_WORD_A16"},	{A32 | O32 | 0xde28, "_OP_FISUBR_WORD_A32"},
		{0xde30, "_OP_FIDIV_WORD_A16"},			{A32 | 0xde30, "_OP_FIDIV_WORD_A32"},
		{O32 | 0xde30, "_OP_FIDIV_WORD_A16"},	{A32 | O32 | 0xde30, "_OP_FIDIV_WORD_A32"},
		{0xde38, "_OP_FIDIVR_WORD_A16"},		{A32 | 0xde38, "_OP_FIDIVR_WORD_A32"},
		{O32 | 0xde38, "_OP_FIDIVR_WORD_A16"},	{A32 | O32 | 0xde38, "_OP_FIDIVR_WORD_A32"},

		{0xdf00, "_OP_FILD_WORD_A16"},			{A32 | 0xdf00, "_OP_FILD_WORD_A32"},
		{O32 | 0xdf00, "_OP_FILD_WORD_A16"},	{A32 | O32 | 0xdf00, "_OP_FILD_WORD_A32"},
		{0xdf08, "_OP_FISTTP_WORD_A16"},		{A32 | 0xdf08, "_OP_FISTTP_WORD_A32"},
		{O32 | 0xdf08, "_OP_FISTTP_WORD_A16"},	{A32 | O32 | 0xdf08, "_OP_FISTTP_WORD_A32"},
		{0xdf10, "_OP_FIST_WORD_A16"},			{A32 | 0xdf10, "_OP_FIST_WORD_A32"},
		{O32 | 0xdf10, "_OP_FIST_WORD_A16"},	{A32 | O32 | 0xdf10, "_OP_FIST_WORD_A32"},
		{0xdf18, "_OP_FISTP_WORD_A16"},			{A32 | 0xdf18, "_OP_FISTP_WORD_A32"},
		{O32 | 0xdf18, "_OP_FISTP_WORD_A16"},	{A32 | O32 | 0xdf18, "_OP_FISTP_WORD_A32"},
		{0xdf28, "_OP_FILD_QWORD_A16"},			{A32 | 0xdf28, "_OP_FILD_QWORD_A32"},
		{O32 | 0xdf28, "_OP_FILD_QWORD_A16"},	{A32 | O32 | 0xdf28, "_OP_FILD_QWORD_A32"},
		{0xdf38, "_OP_FISTP_QWORD_A16"},		{A32 | 0xdf38, "_OP_FISTP_QWORD_A32"},
		{O32 | 0xdf38, "_OP_FISTP_QWORD_A16"},	{A32 | O32 | 0xdf38, "_OP_FISTP_QWORD_A32"},

		{0xe0, "_OP_LOOPNZ_Jb_O16"},			{A32 | 0xe0, "_OP_LOOPNZ_Jb_O16"},
		{O32 | 0xe0, "_OP_LOOPNZ_Jb_O32"},		{A32 | O32 | 0xe0, "_OP_LOOPNZ_Jb_O32"},
		{0xe1, "_OP_LOOPZ_Jb_O16"},				{A32 | 0xe1, "_OP_LOOPZ_Jb_O16"},
		{O32 | 0xe1, "_OP_LOOPZ_Jb_O32"},		{A32 | O32 | 0xe1, "_OP_LOOPZ_Jb_O32"},
		{0xe2, "_OP_LOOP_Jb_O16"},				{A32 | 0xe2, "_OP_LOOP_Jb_O16"},
		{O32 | 0xe2, "_OP_LOOP_Jb_O32"},		{A32 | O32 | 0xe2, "_OP_LOOP_Jb_O32"},
		{0xe3, "_OP_JCXZ_Jb_O16"},				{A32 | 0xe3, "_OP_JECXZ_Jb_O16"},
		{O32 | 0xe3, "_OP_JCXZ_Jb_O32"},		{A32 | O32 | 0xe3, "_OP_JECXZ_Jb_O32"},

		{0xe8, "_OP_CALL_Jw"},					{A32 | 0xe8, "_OP_CALL_Jw"},
		{O32 | 0xe8, "_OP_CALL_Jd"},			{A32 | O32 | 0xe8, "_OP_CALL_Jd"},

		{0xe9, "_OP_JMP_Jw"},					{A32 | 0xe9, "_OP_JMP_Jw"},
		{O32 | 0xe9, "_OP_JMP_Jd"},				{A32 | O32 | 0xe9, "_OP_JMP_Jd"},

		{0xeb, "_OP_JMP_Jb_O16"},				{A32 | 0xeb, "_OP_JMP_Jb_O16"},
		{O32 | 0xeb, "_OP_JMP_Jb_O32"},			{A32 | O32 | 0xeb, "_OP_JMP_Jb_O32"},

		{0xf5, "_OP_CMC"},					{A32 | 0xf5, "_OP_CMC"},
		{O32 | 0xf5, "_OP_CMC"},			{A32 | O32 | 0xf5, "_OP_CMC"},
		
		{0xf600, "_OP_TEST_Mb_Ib_A16"},			{A32 | 0xf600, "_OP_TEST_Mb_Ib_A32"},
		{O32 | 0xf600, "_OP_TEST_Mb_Ib_A16"},	{A32 | O32 | 0xf600, "_OP_TEST_Mb_Ib_A32"},
        {0xf610, "_OP_NOT_Mb_A16"},				{A32 | 0xf610, "_OP_NOT_Mb_A32"},
		{O32 | 0xf610, "_OP_NOT_Mb_A16"},		{A32 | O32 | 0xf610, "_OP_NOT_Mb_A32"},
		{0xf618, "_OP_NEG_Mb_A16"},				{A32 | 0xf618, "_OP_NEG_Mb_A32"},
		{O32 | 0xf618, "_OP_NEG_Mb_A16"},		{A32 | O32 | 0xf618, "_OP_NEG_Mb_A32"},
		{0xf620, "_OP_MUL_AL_Mb_A16"},			{A32 | 0xf620, "_OP_MUL_AL_Mb_A32"},
		{O32 | 0xf620, "_OP_MUL_AL_Mb_A16"},	{A32 | O32 | 0xf620, "_OP_MUL_AL_Mb_A32"},
		{0xf628, "_OP_IMUL_AL_Mb_A16"},			{A32 | 0xf628, "_OP_IMUL_AL_Mb_A32"},
		{O32 | 0xf628, "_OP_IMUL_AL_Mb_A16"},	{A32 | O32 | 0xf628, "_OP_IMUL_AL_Mb_A32"},
		{0xf630, "_OP_DIV_AX_Mb_A16"},			{A32 | 0xf630, "_OP_DIV_AX_Mb_A32"},
		{O32 | 0xf630, "_OP_DIV_AX_Mb_A16"},	{A32 | O32 | 0xf630, "_OP_DIV_AX_Mb_A32"},
		{0xf638, "_OP_IDIV_AX_Mb_A16"},			{A32 | 0xf638, "_OP_IDIV_AX_Mb_A32"},
		{O32 | 0xf638, "_OP_IDIV_AX_Mb_A16"},	{A32 | O32 | 0xf638, "_OP_IDIV_AX_Mb_A32"},
		{0xf6c0, "_OP_TEST_Rb_Ib"},				{A32 | 0xf6c0, "_OP_TEST_Rb_Ib"},
		{O32 | 0xf6c0, "_OP_TEST_Rb_Ib"},		{A32 | O32 | 0xf6c0, "_OP_TEST_Rb_Ib"},
        {0xf6d0, "_OP_NOT_Rb"},					{A32 | 0xf6d0, "_OP_NOT_Rb"},
		{O32 | 0xf6d0, "_OP_NOT_Rb"},			{A32 | O32 | 0xf6d0, "_OP_NOT_Rb"},
		{0xf6d8, "_OP_NEG_Rb"},					{A32 | 0xf6d8, "_OP_NEG_Rb"},
		{O32 | 0xf6d8, "_OP_NEG_Rb"},			{A32 | O32 | 0xf6d8, "_OP_NEG_Rb"},
		{0xf6e0, "_OP_MUL_AL_Rb"},				{A32 | 0xf6e0, "_OP_MUL_AL_Rb"},
		{O32 | 0xf6e0, "_OP_MUL_AL_Rb"},		{A32 | O32 | 0xf6e0, "_OP_MUL_AL_Rb"},
		{0xf6e8, "_OP_IMUL_AL_Rb"},				{A32 | 0xf6e8, "_OP_IMUL_AL_Rb"},
		{O32 | 0xf6e8, "_OP_IMUL_AL_Rb"},		{A32 | O32 | 0xf6e8, "_OP_IMUL_AL_Rb"},
		{0xf6f0, "_OP_DIV_AX_Rb"},				{A32 | 0xf6f0, "_OP_DIV_AX_Rb"},
		{O32 | 0xf6f0, "_OP_DIV_AX_Rb"},		{A32 | O32 | 0xf6f0, "_OP_DIV_AX_Rb"},
		{0xf6f8, "_OP_IDIV_AX_Rb"},				{A32 | 0xf6f8, "_OP_IDIV_AX_Rb"},
		{O32 | 0xf6f8, "_OP_IDIV_AX_Rb"},		{A32 | O32 | 0xf6f8, "_OP_IDIV_AX_Rb"},
		
		{0xf700, "_OP_TEST_Mw_Iw_A16"},				{A32 | 0xf700, "_OP_TEST_Mw_Iw_A32"},
		{O32 | 0xf700, "_OP_TEST_Md_Id_A16"},		{A32 | O32 | 0xf700, "_OP_TEST_Md_Id_A32"},
        {0xf710, "_OP_NOT_Mw_A16"},					{A32 | 0xf710, "_OP_NOT_Mw_A32"},
		{O32 | 0xf710, "_OP_NOT_Md_A16"},			{A32 | O32 | 0xf710, "_OP_NOT_Md_A32"},
		{0xf718, "_OP_NEG_Mw_A16"},					{A32 | 0xf718, "_OP_NEG_Mw_A32"},
		{O32 | 0xf718, "_OP_NEG_Md_A16"},			{A32 | O32 | 0xf718, "_OP_NEG_Md_A32"},
		{0xf720, "_OP_MUL_AX_Mw_A16"},				{A32 | 0xf720, "_OP_MUL_AX_Mw_A32"},
		{O32 | 0xf720, "_OP_MUL_EAX_Md_A16"},		{A32 | O32 | 0xf720, "_OP_MUL_EAX_Md_A32"},
		{0xf728, "_OP_IMUL_AX_Mw_A16"},				{A32 | 0xf728, "_OP_IMUL_AX_Mw_A32"},
		{O32 | 0xf728, "_OP_IMUL_EAX_Md_A16"},		{A32 | O32 | 0xf728, "_OP_IMUL_EAX_Md_A32"},
		{0xf730, "_OP_DIV_DX_AX_Mw_A16"},			{A32 | 0xf730, "_OP_DIV_DX_AX_Mw_A32"},
		{O32 | 0xf730, "_OP_DIV_EDX_EAX_Md_A16"},	{A32 | O32 | 0xf730, "_OP_DIV_EDX_EAX_Md_A32"},
		{0xf738, "_OP_IDIV_DX_AX_Mw_A16"},			{A32 | 0xf738, "_OP_IDIV_DX_AX_Mw_A32"},
		{O32 | 0xf738, "_OP_IDIV_EDX_EAX_Md_A16"},	{A32 | O32 | 0xf738, "_OP_IDIV_EDX_EAX_Md_A32"},
		{0xf7c0, "_OP_TEST_Rw_Iw"},					{A32 | 0xf7c0, "_OP_TEST_Rw_Iw"},
		{O32 | 0xf7c0, "_OP_TEST_Rd_Id"},			{A32 | O32 | 0xf7c0, "_OP_TEST_Rd_Id"},
        {0xf7d0, "_OP_NOT_Rw"},						{A32 | 0xf7d0, "_OP_NOT_Rw"},
		{O32 | 0xf7d0, "_OP_NOT_Rd"},				{A32 | O32 | 0xf7d0, "_OP_NOT_Rd"},
		{0xf7d8, "_OP_NEG_Rw"},						{A32 | 0xf7d8, "_OP_NEG_Rw"},
		{O32 | 0xf7d8, "_OP_NEG_Rd"},				{A32 | O32 | 0xf7d8, "_OP_NEG_Rd"},
		{0xf7e0, "_OP_MUL_AX_Rw"},					{A32 | 0xf7e0, "_OP_MUL_AX_Rw"},
		{O32 | 0xf7e0, "_OP_MUL_EAX_Rd"},			{A32 | O32 | 0xf7e0, "_OP_MUL_EAX_Rd"},
		{0xf7e8, "_OP_IMUL_AX_Rw"},					{A32 | 0xf7e8, "_OP_IMUL_AX_Rw"},
		{O32 | 0xf7e8, "_OP_IMUL_EAX_Rd"},			{A32 | O32 | 0xf7e8, "_OP_IMUL_EAX_Rd"},
		{0xf7f0, "_OP_DIV_DX_AX_Rw"},				{A32 | 0xf7f0, "_OP_DIV_DX_AX_Rw"},
		{O32 | 0xf7f0, "_OP_DIV_EDX_EAX_Rd"},		{A32 | O32 | 0xf7f0, "_OP_DIV_EDX_EAX_Rd"},
		{0xf7f8, "_OP_IDIV_DX_AX_Rw"},				{A32 | 0xf7f8, "_OP_IDIV_DX_AX_Rw"},
		{O32 | 0xf7f8, "_OP_IDIV_EDX_EAX_Rd"},		{A32 | O32 | 0xf7f8, "_OP_IDIV_EDX_EAX_Rd"},
		
		{0xf8, "_OP_CLC"},						{A32 | 0xf8, "_OP_CLC"},
		{O32 | 0xf8, "_OP_CLC"},				{A32 | O32 | 0xf8, "_OP_CLC"},
		{0xf9, "_OP_STC"},						{A32 | 0xf9, "_OP_STC"},
		{O32 | 0xf9, "_OP_STC"},				{A32 | O32 | 0xf9, "_OP_STC"},
		{0xfc, "_OP_CLD"},						{A32 | 0xfc, "_OP_CLD"},
		{O32 | 0xfc, "_OP_CLD"},				{A32 | O32 | 0xfc, "_OP_CLD"},
		{0xfd, "_OP_STD"},						{A32 | 0xfd, "_OP_STD"},
		{O32 | 0xfd, "_OP_STD"},				{A32 | O32 | 0xfd, "_OP_STD"},

		{0xfe00, "_OP_INC_Mb_A16"},				{A32 | 0xfe00, "_OP_INC_Mb_A32"},
		{O32 | 0xfe00, "_OP_INC_Mb_A16"},		{A32 | O32 | 0xfe00, "_OP_INC_Mb_A32"},
		{0xfe08, "_OP_DEC_Mb_A16"},				{A32 | 0xfe08, "_OP_DEC_Mb_A32"},
		{O32 | 0xfe08, "_OP_DEC_Mb_A16"},		{A32 | O32 | 0xfe08, "_OP_DEC_Mb_A32"},
		{0xfec0, "_OP_INC_Rb"},					{A32 | 0xfec0, "_OP_INC_Rb"},
		{O32 | 0xfec0, "_OP_INC_Rb"},			{A32 | O32 | 0xfec0, "_OP_INC_Rb"},
		{0xfec8, "_OP_DEC_Rb"},					{A32 | 0xfec8, "_OP_DEC_Rb"},
		{O32 | 0xfec8, "_OP_DEC_Rb"},			{A32 | O32 | 0xfec8, "_OP_DEC_Rb"},

		{0xff00, "_OP_INC_Mw_A16"},				{A32 | 0xff00, "_OP_INC_Mw_A32"},
		{O32 | 0xff00, "_OP_INC_Md_A16"},		{A32 | O32 | 0xff00, "_OP_INC_Md_A32"},
		{0xff08, "_OP_DEC_Mw_A16"},				{A32 | 0xff08, "_OP_DEC_Mw_A32"},
		{O32 | 0xff08, "_OP_DEC_Md_A16"},		{A32 | O32 | 0xff08, "_OP_DEC_Md_A32"},
		{0xff30, "_OP_PUSH_Mw_A16"},			{A32 | 0xff30, "_OP_PUSH_Mw_A32"},
		{O32 | 0xff30, "_OP_PUSH_Md_A16"},		{A32 | O32 | 0xff30, "_OP_PUSH_Md_A32"},
		{0xffc0, "_OP_INC_Rw"},					{A32 | 0xffc0, "_OP_INC_Rw"},
		{O32 | 0xffc0, "_OP_INC_Rd"},			{A32 | O32 | 0xffc0, "_OP_INC_Rd"},
		{0xffc8, "_OP_DEC_Rw"},					{A32 | 0xffc8, "_OP_DEC_Rw"},
		{O32 | 0xffc8, "_OP_DEC_Rd"},			{A32 | O32 | 0xffc8, "_OP_DEC_Rd"},
		{0xfff0, "_OP_PUSH_Rw"},				{A32 | 0xfff0, "_OP_PUSH_Rw"},
		{O32 | 0xfff0, "_OP_PUSH_Rd"},			{A32 | O32 | 0xfff0, "_OP_PUSH_Rd"},

		{0x0f06, "_OP_CLTS"},			{A32 | 0x0f06, "_OP_CLTS"},
		{O32 | 0x0f06, "_OP_CLTS"},		{A32 | O32 | 0x0f06, "_OP_CLTS"},

		{0x0f20c0, "_OP_MOV_Rd_CR0"},			{A32 | 0x0f20c0, "_OP_MOV_Rd_CR0"},
		{O32 | 0x0f20c0, "_OP_MOV_Rd_CR0"},		{A32 | O32 | 0x0f20c0, "_OP_MOV_Rd_CR0"},
		{0x0f20d0, "_OP_MOV_Rd_CR2"},			{A32 | 0x0f20d0, "_OP_MOV_Rd_CR2"},
		{O32 | 0x0f20d0, "_OP_MOV_Rd_CR2"},		{A32 | O32 | 0x0f20d0, "_OP_MOV_Rd_CR2"},
		{0x0f20d8, "_OP_MOV_Rd_CR3"},			{A32 | 0x0f20d8, "_OP_MOV_Rd_CR3"},
		{O32 | 0x0f20d8, "_OP_MOV_Rd_CR3"},		{A32 | O32 | 0x0f20d8, "_OP_MOV_Rd_CR3"},
		{0x0f20e0, "_OP_MOV_Rd_CR4"},			{A32 | 0x0f20e0, "_OP_MOV_Rd_CR4"},
		{O32 | 0x0f20e0, "_OP_MOV_Rd_CR4"},		{A32 | O32 | 0x0f20e0, "_OP_MOV_Rd_CR4"},
		
		{0x0f21c0, "_OP_MOV_Rd_DR0"},			{A32 | 0x0f21c0, "_OP_MOV_Rd_DR0"},
		{O32 | 0x0f21c0, "_OP_MOV_Rd_DR0"},		{A32 | O32 | 0x0f21c0, "_OP_MOV_Rd_DR0"},
		{0x0f21c8, "_OP_MOV_Rd_DR1"},			{A32 | 0x0f21c8, "_OP_MOV_Rd_DR1"},
		{O32 | 0x0f21c8, "_OP_MOV_Rd_DR1"},		{A32 | O32 | 0x0f21c8, "_OP_MOV_Rd_DR1"},
		{0x0f21d0, "_OP_MOV_Rd_DR2"},			{A32 | 0x0f21d0, "_OP_MOV_Rd_DR2"},
		{O32 | 0x0f21d0, "_OP_MOV_Rd_DR2"},		{A32 | O32 | 0x0f21d0, "_OP_MOV_Rd_DR2"},
		{0x0f21d8, "_OP_MOV_Rd_DR3"},			{A32 | 0x0f21d8, "_OP_MOV_Rd_DR3"},
		{O32 | 0x0f21d8, "_OP_MOV_Rd_DR3"},		{A32 | O32 | 0x0f21d8, "_OP_MOV_Rd_DR3"},
		{0x0f21e0, "_OP_MOV_Rd_DR4"},			{A32 | 0x0f21e0, "_OP_MOV_Rd_DR4"},
		{O32 | 0x0f21e0, "_OP_MOV_Rd_DR4"},		{A32 | O32 | 0x0f21e0, "_OP_MOV_Rd_DR4"},
		{0x0f21e8, "_OP_MOV_Rd_DR5"},			{A32 | 0x0f21e8, "_OP_MOV_Rd_DR5"},
		{O32 | 0x0f21e8, "_OP_MOV_Rd_DR5"},		{A32 | O32 | 0x0f21e8, "_OP_MOV_Rd_DR5"},
		{0x0f21f0, "_OP_MOV_Rd_DR6"},			{A32 | 0x0f21f0, "_OP_MOV_Rd_DR6"},
		{O32 | 0x0f21f0, "_OP_MOV_Rd_DR6"},		{A32 | O32 | 0x0f21f0, "_OP_MOV_Rd_DR6"},
		{0x0f21f8, "_OP_MOV_Rd_DR7"},			{A32 | 0x0f21f8, "_OP_MOV_Rd_DR7"},
		{O32 | 0x0f21f8, "_OP_MOV_Rd_DR7"},		{A32 | O32 | 0x0f21f8, "_OP_MOV_Rd_DR7"},
#endif

		{0x0f31, "_OP_RDTSC"},					{A32 | 0x0f31, "_OP_RDTSC"},
		{O32 | 0x0f31, "_OP_RDTSC"},			{A32 | O32 | 0x0f31, "_OP_RDTSC"},

		{0x0f4000, "_OP_CMOVO_Gw_Mw_A16"},			{A32 | 0x0f4000, "_OP_CMOVO_Gw_Mw_A32"},
		{O32 | 0x0f4000, "_OP_CMOVO_Gd_Md_A16"},	{A32 | O32 | 0x0f4000, "_OP_CMOVO_Gd_Md_A32"},
		{0x0f40c0, "_OP_CMOVO_Gw_Rw"},				{A32 | 0x0f40c0, "_OP_CMOVO_Gw_Rw"},
		{O32 | 0x0f40c0, "_OP_CMOVO_Gd_Rd"},		{A32 | O32 | 0x0f40c0, "_OP_CMOVO_Gd_Rd"},
		{0x0f4100, "_OP_CMOVNO_Gw_Mw_A16"},			{A32 | 0x0f4100, "_OP_CMOVNO_Gw_Mw_A32"},
		{O32 | 0x0f4100, "_OP_CMOVNO_Gd_Md_A16"},	{A32 | O32 | 0x0f4100, "_OP_CMOVNO_Gd_Md_A32"},
		{0x0f41c0, "_OP_CMOVNO_Gw_Rw"},				{A32 | 0x0f41c0, "_OP_CMOVNO_Gw_Rw"},
		{O32 | 0x0f41c0, "_OP_CMOVNO_Gd_Rd"},		{A32 | O32 | 0x0f41c0, "_OP_CMOVNO_Gd_Rd"},
		{0x0f4c00, "_OP_CMOVL_Gw_Mw_A16"},			{A32 | 0x0f4c00, "_OP_CMOVL_Gw_Mw_A32"},
		{O32 | 0x0f4c00, "_OP_CMOVL_Gd_Md_A16"},	{A32 | O32 | 0x0f4c00, "_OP_CMOVL_Gd_Md_A32"},
		{0x0f4cc0, "_OP_CMOVL_Gw_Rw"},				{A32 | 0x0f4cc0, "_OP_CMOVL_Gw_Rw"},
		{O32 | 0x0f4cc0, "_OP_CMOVL_Gd_Rd"},		{A32 | O32 | 0x0f4cc0, "_OP_CMOVL_Gd_Rd"},
		{0x0f4d00, "_OP_CMOVNL_Gw_Mw_A16"},			{A32 | 0x0f4d00, "_OP_CMOVNL_Gw_Mw_A32"},
		{O32 | 0x0f4d00, "_OP_CMOVNL_Gd_Md_A16"},	{A32 | O32 | 0x0f4d00, "_OP_CMOVNL_Gd_Md_A32"},
		{0x0f4dc0, "_OP_CMOVNL_Gw_Rw"},				{A32 | 0x0f4dc0, "_OP_CMOVNL_Gw_Rw"},
		{O32 | 0x0f4dc0, "_OP_CMOVNL_Gd_Rd"},		{A32 | O32 | 0x0f4dc0, "_OP_CMOVNL_Gd_Rd"},
		{0x0f4e00, "_OP_CMOVLE_Gw_Mw_A16"},			{A32 | 0x0f4e00, "_OP_CMOVLE_Gw_Mw_A32"},
		{O32 | 0x0f4e00, "_OP_CMOVLE_Gd_Md_A16"},	{A32 | O32 | 0x0f4e00, "_OP_CMOVLE_Gd_Md_A32"},
		{0x0f4ec0, "_OP_CMOVLE_Gw_Rw"},				{A32 | 0x0f4ec0, "_OP_CMOVLE_Gw_Rw"},
		{O32 | 0x0f4ec0, "_OP_CMOVLE_Gd_Rd"},		{A32 | O32 | 0x0f4ec0, "_OP_CMOVLE_Gd_Rd"},
		{0x0f4f00, "_OP_CMOVNLE_Gw_Mw_A16"},		{A32 | 0x0f4f00, "_OP_CMOVNLE_Gw_Mw_A32"},
		{O32 | 0x0f4f00, "_OP_CMOVNLE_Gd_Md_A16"},	{A32 | O32 | 0x0f4f00, "_OP_CMOVNLE_Gd_Md_A32"},
		{0x0f4fc0, "_OP_CMOVNLE_Gw_Rw"},			{A32 | 0x0f4fc0, "_OP_CMOVNLE_Gw_Rw"},
		{O32 | 0x0f4fc0, "_OP_CMOVNLE_Gd_Rd"},		{A32 | O32 | 0x0f4fc0, "_OP_CMOVNLE_Gd_Rd"},

		{0x0f80, "_OP_JO_Jw"},					{A32 | 0x0f80, "_OP_JO_Jw"},
		{O32 | 0x0f80, "_OP_JO_Jd"},			{A32 | O32 | 0x0f80, "_OP_JO_Jd"},
		{0x0f81, "_OP_JNO_Jw"},					{A32 | 0x0f81, "_OP_JNO_Jw"},
		{O32 | 0x0f81, "_OP_JNO_Jd"},			{A32 | O32 | 0x0f81, "_OP_JNO_Jd"},

#ifndef USE_MICROCODE
		{0x0f82, "_OP_JB_Jw"},					{A32 | 0x0f82, "_OP_JB_Jw"},
		{O32 | 0x0f82, "_OP_JB_Jd"},			{A32 | O32 | 0x0f82, "_OP_JB_Jd"},
		{0x0f83, "_OP_JNB_Jw"},					{A32 | 0x0f83, "_OP_JNB_Jw"},
		{O32 | 0x0f83, "_OP_JNB_Jd"},			{A32 | O32 | 0x0f83, "_OP_JNB_Jd"},
		{0x0f84, "_OP_JZ_Jw"},					{A32 | 0x0f84, "_OP_JZ_Jw"},
		{O32 | 0x0f84, "_OP_JZ_Jd"},			{A32 | O32 | 0x0f84, "_OP_JZ_Jd"},
		{0x0f85, "_OP_JNZ_Jw"},					{A32 | 0x0f85, "_OP_JNZ_Jw"},
		{O32 | 0x0f85, "_OP_JNZ_Jd"},			{A32 | O32 | 0x0f85, "_OP_JNZ_Jd"},
		{0x0f86, "_OP_JBE_Jw"},					{A32 | 0x0f86, "_OP_JBE_Jw"},
		{O32 | 0x0f86, "_OP_JBE_Jd"},			{A32 | O32 | 0x0f86, "_OP_JBE_Jd"},
		{0x0f87, "_OP_JNBE_Jw"},				{A32 | 0x0f87, "_OP_JNBE_Jw"},
		{O32 | 0x0f87, "_OP_JNBE_Jd"},			{A32 | O32 | 0x0f87, "_OP_JNBE_Jd"},
		{0x0f88, "_OP_JS_Jw"},					{A32 | 0x0f88, "_OP_JS_Jw"},
		{O32 | 0x0f88, "_OP_JS_Jd"},			{A32 | O32 | 0x0f88, "_OP_JS_Jd"},
		{0x0f89, "_OP_JNS_Jw"},					{A32 | 0x0f89, "_OP_JNS_Jw"},
		{O32 | 0x0f89, "_OP_JNS_Jd"},			{A32 | O32 | 0x0f89, "_OP_JNS_Jd"},
		{0x0f8a, "_OP_JP_Jw"},					{A32 | 0x0f8a, "_OP_JP_Jw"},
		{O32 | 0x0f8a, "_OP_JP_Jd"},			{A32 | O32 | 0x0f8a, "_OP_JP_Jd"},
		{0x0f8b, "_OP_JNP_Jw"},					{A32 | 0x0f8b, "_OP_JNP_Jw"},
		{O32 | 0x0f8b, "_OP_JNP_Jd"},			{A32 | O32 | 0x0f8b, "_OP_JNP_Jd"},
#endif

		{0x0f8c, "_OP_JL_Jw"},					{A32 | 0x0f8c, "_OP_JL_Jw"},
		{O32 | 0x0f8c, "_OP_JL_Jd"},			{A32 | O32 | 0x0f8c, "_OP_JL_Jd"},
		{0x0f8d, "_OP_JNL_Jw"},					{A32 | 0x0f8d, "_OP_JNL_Jw"},
		{O32 | 0x0f8d, "_OP_JNL_Jd"},			{A32 | O32 | 0x0f8d, "_OP_JNL_Jd"},
		{0x0f8e, "_OP_JLE_Jw"},					{A32 | 0x0f8e, "_OP_JLE_Jw"},
		{O32 | 0x0f8e, "_OP_JLE_Jd"},			{A32 | O32 | 0x0f8e, "_OP_JLE_Jd"},
		{0x0f8f, "_OP_JNLE_Jw"},				{A32 | 0x0f8f, "_OP_JNLE_Jw"},
		{O32 | 0x0f8f, "_OP_JNLE_Jd"},			{A32 | O32 | 0x0f8f, "_OP_JNLE_Jd"},

		{0x0f9000, "_OP_SETO_Mb_A16"},			{A32 | 0x0f9000, "_OP_SETO_Mb_A32"},
		{O32 | 0x0f9000, "_OP_SETO_Mb_A16"},	{A32 | O32 | 0x0f9000, "_OP_SETO_Mb_A32"},
		{0x0f90c0, "_OP_SETO_Rb"},				{A32 | 0x0f90c0, "_OP_SETO_Rb"},
		{O32 | 0x0f90c0, "_OP_SETO_Rb"},		{A32 | O32 | 0x0f90c0, "_OP_SETO_Rb"},
		{0x0f9100, "_OP_SETNO_Mb_A16"},			{A32 | 0x0f9100, "_OP_SETNO_Mb_A32"},
		{O32 | 0x0f9100, "_OP_SETNO_Mb_A16"},	{A32 | O32 | 0x0f9100, "_OP_SETNO_Mb_A32"},
		{0x0f91c0, "_OP_SETNO_Rb"},				{A32 | 0x0f91c0, "_OP_SETNO_Rb"},
		{O32 | 0x0f91c0, "_OP_SETNO_Rb"},		{A32 | O32 | 0x0f91c0, "_OP_SETNO_Rb"},

#ifndef USE_MICROCODE
		{0x0f9200, "_OP_SETB_Mb_A16"},			{A32 | 0x0f9200, "_OP_SETB_Mb_A32"},
		{O32 | 0x0f9200, "_OP_SETB_Mb_A16"},	{A32 | O32 | 0x0f9200, "_OP_SETB_Mb_A32"},
		{0x0f92c0, "_OP_SETB_Rb"},				{A32 | 0x0f92c0, "_OP_SETB_Rb"},
		{O32 | 0x0f92c0, "_OP_SETB_Rb"},		{A32 | O32 | 0x0f92c0, "_OP_SETB_Rb"},
		{0x0f9300, "_OP_SETNB_Mb_A16"},			{A32 | 0x0f9300, "_OP_SETNB_Mb_A32"},
		{O32 | 0x0f9300, "_OP_SETNB_Mb_A16"},	{A32 | O32 | 0x0f9300, "_OP_SETNB_Mb_A32"},
		{0x0f93c0, "_OP_SETNB_Rb"},				{A32 | 0x0f93c0, "_OP_SETNB_Rb"},
		{O32 | 0x0f93c0, "_OP_SETNB_Rb"},		{A32 | O32 | 0x0f93c0, "_OP_SETNB_Rb"},
		{0x0f9400, "_OP_SETZ_Mb_A16"},			{A32 | 0x0f9400, "_OP_SETZ_Mb_A32"},
		{O32 | 0x0f9400, "_OP_SETZ_Mb_A16"},	{A32 | O32 | 0x0f9400, "_OP_SETZ_Mb_A32"},
		{0x0f94c0, "_OP_SETZ_Rb"},				{A32 | 0x0f94c0, "_OP_SETZ_Rb"},
		{O32 | 0x0f94c0, "_OP_SETZ_Rb"},		{A32 | O32 | 0x0f94c0, "_OP_SETZ_Rb"},
		{0x0f9500, "_OP_SETNZ_Mb_A16"},			{A32 | 0x0f9500, "_OP_SETNZ_Mb_A32"},
		{O32 | 0x0f9500, "_OP_SETNZ_Mb_A16"},	{A32 | O32 | 0x0f9500, "_OP_SETNZ_Mb_A32"},
		{0x0f95c0, "_OP_SETNZ_Rb"},				{A32 | 0x0f95c0, "_OP_SETNZ_Rb"},
		{O32 | 0x0f95c0, "_OP_SETNZ_Rb"},		{A32 | O32 | 0x0f95c0, "_OP_SETNZ_Rb"},
		{0x0f9600, "_OP_SETBE_Mb_A16"},			{A32 | 0x0f9600, "_OP_SETBE_Mb_A32"},
		{O32 | 0x0f9600, "_OP_SETBE_Mb_A16"},	{A32 | O32 | 0x0f9600, "_OP_SETBE_Mb_A32"},
		{0x0f96c0, "_OP_SETBE_Rb"},				{A32 | 0x0f96c0, "_OP_SETBE_Rb"},
		{O32 | 0x0f96c0, "_OP_SETBE_Rb"},		{A32 | O32 | 0x0f96c0, "_OP_SETBE_Rb"},
		{0x0f9700, "_OP_SETNBE_Mb_A16"},		{A32 | 0x0f9700, "_OP_SETNBE_Mb_A32"},
		{O32 | 0x0f9700, "_OP_SETNBE_Mb_A16"},	{A32 | O32 | 0x0f9700, "_OP_SETNBE_Mb_A32"},
		{0x0f97c0, "_OP_SETNBE_Rb"},			{A32 | 0x0f97c0, "_OP_SETNBE_Rb"},
		{O32 | 0x0f97c0, "_OP_SETNBE_Rb"},		{A32 | O32 | 0x0f97c0, "_OP_SETNBE_Rb"},
		{0x0f9800, "_OP_SETS_Mb_A16"},			{A32 | 0x0f9800, "_OP_SETS_Mb_A32"},
		{O32 | 0x0f9800, "_OP_SETS_Mb_A16"},	{A32 | O32 | 0x0f9800, "_OP_SETS_Mb_A32"},
		{0x0f98c0, "_OP_SETS_Rb"},				{A32 | 0x0f98c0, "_OP_SETS_Rb"},
		{O32 | 0x0f98c0, "_OP_SETS_Rb"},		{A32 | O32 | 0x0f98c0, "_OP_SETS_Rb"},
		{0x0f9900, "_OP_SETNS_Mb_A16"},			{A32 | 0x0f9900, "_OP_SETNS_Mb_A32"},
		{O32 | 0x0f9900, "_OP_SETNS_Mb_A16"},	{A32 | O32 | 0x0f9900, "_OP_SETNS_Mb_A32"},
		{0x0f99c0, "_OP_SETNS_Rb"},				{A32 | 0x0f99c0, "_OP_SETNS_Rb"},
		{O32 | 0x0f99c0, "_OP_SETNS_Rb"},		{A32 | O32 | 0x0f99c0, "_OP_SETNS_Rb"},
		{0x0f9a00, "_OP_SETP_Mb_A16"},			{A32 | 0x0f9a00, "_OP_SETP_Mb_A32"},
		{O32 | 0x0f9a00, "_OP_SETP_Mb_A16"},	{A32 | O32 | 0x0f9a00, "_OP_SETP_Mb_A32"},
		{0x0f9ac0, "_OP_SETP_Rb"},				{A32 | 0x0f9ac0, "_OP_SETP_Rb"},
		{O32 | 0x0f9ac0, "_OP_SETP_Rb"},		{A32 | O32 | 0x0f9ac0, "_OP_SETP_Rb"},
		{0x0f9b00, "_OP_SETNP_Mb_A16"},			{A32 | 0x0f9b00, "_OP_SETNP_Mb_A32"},
		{O32 | 0x0f9b00, "_OP_SETNP_Mb_A16"},	{A32 | O32 | 0x0f9b00, "_OP_SETNP_Mb_A32"},
		{0x0f9bc0, "_OP_SETNP_Rb"},				{A32 | 0x0f9bc0, "_OP_SETNP_Rb"},
		{O32 | 0x0f9bc0, "_OP_SETNP_Rb"},		{A32 | O32 | 0x0f9bc0, "_OP_SETNP_Rb"},
#endif

		{0x0f9c00, "_OP_SETL_Mb_A16"},			{A32 | 0x0f9c00, "_OP_SETL_Mb_A32"},
		{O32 | 0x0f9c00, "_OP_SETL_Mb_A16"},	{A32 | O32 | 0x0f9c00, "_OP_SETL_Mb_A32"},
		{0x0f9cc0, "_OP_SETL_Rb"},				{A32 | 0x0f9cc0, "_OP_SETL_Rb"},
		{O32 | 0x0f9cc0, "_OP_SETL_Rb"},		{A32 | O32 | 0x0f9cc0, "_OP_SETL_Rb"},
		{0x0f9d00, "_OP_SETNL_Mb_A16"},			{A32 | 0x0f9d00, "_OP_SETNL_Mb_A32"},
		{O32 | 0x0f9d00, "_OP_SETNL_Mb_A16"},	{A32 | O32 | 0x0f9d00, "_OP_SETNL_Mb_A32"},
		{0x0f9dc0, "_OP_SETNL_Rb"},				{A32 | 0x0f9dc0, "_OP_SETNL_Rb"},
		{O32 | 0x0f9dc0, "_OP_SETNL_Rb"},		{A32 | O32 | 0x0f9dc0, "_OP_SETNL_Rb"},
		{0x0f9e00, "_OP_SETLE_Mb_A16"},			{A32 | 0x0f9e00, "_OP_SETLE_Mb_A32"},
		{O32 | 0x0f9e00, "_OP_SETLE_Mb_A16"},	{A32 | O32 | 0x0f9e00, "_OP_SETLE_Mb_A32"},
		{0x0f9ec0, "_OP_SETLE_Rb"},				{A32 | 0x0f9ec0, "_OP_SETLE_Rb"},
		{O32 | 0x0f9ec0, "_OP_SETLE_Rb"},		{A32 | O32 | 0x0f9ec0, "_OP_SETLE_Rb"},
		{0x0f9f00, "_OP_SETNLE_Mb_A16"},		{A32 | 0x0f9f00, "_OP_SETNLE_Mb_A32"},
		{O32 | 0x0f9f00, "_OP_SETNLE_Mb_A16"},	{A32 | O32 | 0x0f9f00, "_OP_SETNLE_Mb_A32"},
		{0x0f9fc0, "_OP_SETNLE_Rb"},			{A32 | 0x0f9fc0, "_OP_SETNLE_Rb"},
		{O32 | 0x0f9fc0, "_OP_SETNLE_Rb"},		{A32 | O32 | 0x0f9fc0, "_OP_SETNLE_Rb"},

#ifndef USE_MICROCODE
		{0x0fa0, "_OP_PUSH_FS_O16"},		{A32 | 0x0fa0, "_OP_PUSH_FS_O16"},
		{O32 | 0x0fa0, "_OP_PUSH_FS_O32"},	{A32 | O32 | 0x0fa0, "_OP_PUSH_FS_O32"},
		{0x0fa1, "_OP_POP_FS_O16"},			{A32 | 0x0fa1, "_OP_POP_FS_O16"},
		{O32 | 0x0fa1, "_OP_POP_FS_O32"},	{A32 | O32 | 0x0fa1, "_OP_POP_FS_O32"},
#endif

		{0x0fa2, "_OP_CPUID"},					{A32 | 0x0fa2, "_OP_CPUID"},
		{O32 | 0x0fa2, "_OP_CPUID"},			{A32 | O32 | 0x0fa2, "_OP_CPUID"},

#ifndef USE_MICROCODE
		{0x0fa300, "_OP_BT_M_Gw_A16"},			{A32 | 0x0fa300, "_OP_BT_M_Gw_A32"},
		{O32 | 0x0fa300, "_OP_BT_M_Gd_A16"},	{A32 | O32 | 0x0fa300, "_OP_BT_M_Gd_A32"},
		{0x0fa3c0, "_OP_BT_Rw_Gw"},				{A32 | 0x0fa3c0, "_OP_BT_Rw_Gw"},
		{O32 | 0x0fa3c0, "_OP_BT_Rd_Gd"},		{A32 | O32 | 0x0fa3c0, "_OP_BT_Rd_Gd"},

		{0x0fa400, "_OP_SHLD_Mw_Gw_Ib_A16"},		{A32 | 0x0fa400, "_OP_SHLD_Mw_Gw_Ib_A32"},
		{O32 | 0x0fa400, "_OP_SHLD_Md_Gd_Ib_A16"},	{A32 | O32 | 0x0fa400, "_OP_SHLD_Md_Gd_Ib_A32"},
		{0x0fa4c0, "_OP_SHLD_Rw_Gw_Ib"},			{A32 | 0x0fa4c0, "_OP_SHLD_Rw_Gw_Ib"},
		{O32 | 0x0fa4c0, "_OP_SHLD_Rd_Gd_Ib"},		{A32 | O32 | 0x0fa4c0, "_OP_SHLD_Rd_Gd_Ib"},
		{0x0fa500, "_OP_SHLD_Mw_Gw_CL_A16"},		{A32 | 0x0fa500, "_OP_SHLD_Mw_Gw_CL_A32"},
		{O32 | 0x0fa500, "_OP_SHLD_Md_Gd_CL_A16"},	{A32 | O32 | 0x0fa500, "_OP_SHLD_Md_Gd_CL_A32"},
		{0x0fa5c0, "_OP_SHLD_Rw_Gw_CL"},			{A32 | 0x0fa5c0, "_OP_SHLD_Rw_Gw_CL"},
		{O32 | 0x0fa5c0, "_OP_SHLD_Rd_Gd_CL"},		{A32 | O32 | 0x0fa5c0, "_OP_SHLD_Rd_Gd_CL"},

		{0x0fa8, "_OP_PUSH_GS_O16"},		{A32 | 0x0fa8, "_OP_PUSH_GS_O16"},
		{O32 | 0x0fa8, "_OP_PUSH_GS_O32"},	{A32 | O32 | 0x0fa8, "_OP_PUSH_GS_O32"},
		{0x0fa9, "_OP_POP_GS_O16"},			{A32 | 0x0fa9, "_OP_POP_GS_O16"},
		{O32 | 0x0fa9, "_OP_POP_GS_O32"},	{A32 | O32 | 0x0fa9, "_OP_POP_GS_O32"},

		{0x0fab00, "_OP_BTS_M_Gw_A16"},			{A32 | 0x0fab00, "_OP_BTS_M_Gw_A32"},
		{O32 | 0x0fab00, "_OP_BTS_M_Gd_A16"},	{A32 | O32 | 0x0fab00, "_OP_BTS_M_Gd_A32"},
		{0x0fabc0, "_OP_BTS_Rw_Gw"},			{A32 | 0x0fabc0, "_OP_BTS_Rw_Gw"},
		{O32 | 0x0fabc0, "_OP_BTS_Rd_Gd"},		{A32 | O32 | 0x0fabc0, "_OP_BTS_Rd_Gd"},

		{0x0fac00, "_OP_SHRD_Mw_Gw_Ib_A16"},		{A32 | 0x0fac00, "_OP_SHRD_Mw_Gw_Ib_A32"},
		{O32 | 0x0fac00, "_OP_SHRD_Md_Gd_Ib_A16"},	{A32 | O32 | 0x0fac00, "_OP_SHRD_Md_Gd_Ib_A32"},
		{0x0facc0, "_OP_SHRD_Rw_Gw_Ib"},			{A32 | 0x0facc0, "_OP_SHRD_Rw_Gw_Ib"},
		{O32 | 0x0facc0, "_OP_SHRD_Rd_Gd_Ib"},		{A32 | O32 | 0x0facc0, "_OP_SHRD_Rd_Gd_Ib"},
		{0x0fad00, "_OP_SHRD_Mw_Gw_CL_A16"},		{A32 | 0x0fad00, "_OP_SHRD_Mw_Gw_CL_A32"},
		{O32 | 0x0fad00, "_OP_SHRD_Md_Gd_CL_A16"},	{A32 | O32 | 0x0fad00, "_OP_SHRD_Md_Gd_CL_A32"},
		{0x0fadc0, "_OP_SHRD_Rw_Gw_CL"},			{A32 | 0x0fadc0, "_OP_SHRD_Rw_Gw_CL"},
		{O32 | 0x0fadc0, "_OP_SHRD_Rd_Gd_CL"},		{A32 | O32 | 0x0fadc0, "_OP_SHRD_Rd_Gd_CL"},

		{0x0faf00, "_OP_IMUL_Gw_Mw_A16"},		{A32 | 0x0faf00, "_OP_IMUL_Gw_Mw_A32"},
		{O32 | 0x0faf00, "_OP_IMUL_Gd_Md_A16"},	{A32 | O32 |  0x0faf00, "_OP_IMUL_Gd_Md_A32"},
		{0x0fafc0, "_OP_IMUL_Gw_Rw"},		{A32 | 0x0fafc0, "_OP_IMUL_Gw_Rw"},
		{O32 | 0x0fafc0, "_OP_IMUL_Gd_Rd"},	{A32 | O32 |  0x0fafc0, "_OP_IMUL_Gd_Rd"},
#endif

		{0x0fb000, "_OP_CMPXCHG_Mb_Gb_A16"},		{A32 | 0x0fb000, "_OP_CMPXCHG_Mb_Gb_A32"},
		{O32 | 0x0fb000, "_OP_CMPXCHG_Mb_Gb_A16"},	{A32 | O32 | 0x0fb000, "_OP_CMPXCHG_Mb_Gb_A32"},
		{0x0fb0c0, "_OP_CMPXCHG_Rb_Gb"},			{A32 | 0x0fb0c0, "_OP_CMPXCHG_Rb_Gb"},
		{O32 | 0x0fb0c0, "_OP_CMPXCHG_Rb_Gb"},		{A32 | O32 | 0x0fb0c0, "_OP_CMPXCHG_Rb_Gb"},

		{0x0fb100, "_OP_CMPXCHG_Mw_Gw_A16"},		{A32 | 0x0fb100, "_OP_CMPXCHG_Mw_Gw_A32"},
		{O32 | 0x0fb100, "_OP_CMPXCHG_Md_Gd_A16"},	{A32 | O32 | 0x0fb100, "_OP_CMPXCHG_Md_Gd_A32"},
		{0x0fb1c0, "_OP_CMPXCHG_Rw_Gw"},			{A32 | 0x0fb1c0, "_OP_CMPXCHG_Rw_Gw"},
		{O32 | 0x0fb1c0, "_OP_CMPXCHG_Rd_Gd"},		{A32 | O32 | 0x0fb1c0, "_OP_CMPXCHG_Rd_Gd"},

#ifndef USE_MICROCODE
		{0x0fb200, "_OP_LSS_Gw_Mp_A16"},		{A32 | 0x0fb200, "_OP_LSS_Gw_Mp_A32"},
		{O32 | 0x0fb200, "_OP_LSS_Gd_Mp_A32"},	{A32 | O32 | 0x0fb200, "_OP_LSS_Gd_Mp_A32"},
		
		{0x0fb300, "_OP_BTR_M_Gw_A16"},			{A32 | 0x0fb300, "_OP_BTR_M_Gw_A32"},
		{O32 | 0x0fb300, "_OP_BTR_M_Gd_A16"},	{A32 | O32 | 0x0fb300, "_OP_BTR_M_Gd_A32"},
		{0x0fb3c0, "_OP_BTR_Rw_Gw"},			{A32 | 0x0fb3c0, "_OP_BTR_Rw_Gw"},
		{O32 | 0x0fb3c0, "_OP_BTR_Rd_Gd"},		{A32 | O32 | 0x0fb3c0, "_OP_BTR_Rd_Gd"},

		{0x0fb400, "_OP_LFS_Gw_Mp_A16"},		{A32 | 0x0fb400, "_OP_LFS_Gw_Mp_A32"},
		{O32 | 0x0fb400, "_OP_LFS_Gd_Mp_A16"},	{A32 | O32 | 0x0fb400, "_OP_LFS_Gd_Mp_A32"},
		{0x0fb500, "_OP_LGS_Gw_Mp_A16"},		{A32 | 0x0fb500, "_OP_LGS_Gw_Mp_A32"},
		{O32 | 0x0fb500, "_OP_LGS_Gd_Mp_A16"},	{A32 | O32 | 0x0fb500, "_OP_LGS_Gd_Mp_A32"},

		{0x0fb600, "_OP_MOVZX_Gw_Mb_A16"},		{A32 | 0x0fb600, "_OP_MOVZX_Gw_Mb_A32"},
		{O32 | 0x0fb600, "_OP_MOVZX_Gd_Mb_A16"},{A32 | O32 | 0x0fb600, "_OP_MOVZX_Gd_Mb_A32"},
		{0x0fb6c0, "_OP_MOVZX_Gw_Rb"},			{A32 | 0x0fb6c0, "_OP_MOVZX_Gw_Rb"},
		{O32 | 0x0fb6c0, "_OP_MOVZX_Gd_Rb"},	{A32 | O32 | 0x0fb6c0, "_OP_MOVZX_Gd_Rb"},
		{0x0fb700, "_OP_MOVZX_Gw_Mw_A16"},		{A32 | 0x0fb700, "_OP_MOVZX_Gw_Mw_A32"},
		{O32 | 0x0fb700, "_OP_MOVZX_Gd_Mw_A16"},{A32 | O32 | 0x0fb700, "_OP_MOVZX_Gd_Mw_A32"},
		{0x0fb7c0, "_OP_MOVZX_Gw_Rw"},			{A32 | 0x0fb7c0, "_OP_MOVZX_Gw_Rw"},
		{O32 | 0x0fb7c0, "_OP_MOVZX_Gd_Rw"},	{A32 | O32 | 0x0fb7c0, "_OP_MOVZX_Gd_Rw"},

		{0x0fba20, "_OP_BT_M_Ib_A16"},			{A32 | 0x0fba20, "_OP_BT_M_Ib_A32"},
		{O32 | 0x0fba20, "_OP_BT_M_Ib_A16"},	{A32 | O32 | 0x0fba20, "_OP_BT_M_Ib_A32"},
		{0x0fba28, "_OP_BTS_M_Ib_A16"},			{A32 | 0x0fba28, "_OP_BTS_M_Ib_A32"},
		{O32 | 0x0fba28, "_OP_BTS_M_Ib_A16"},	{A32 | O32 | 0x0fba28, "_OP_BTS_M_Ib_A32"},
		{0x0fba30, "_OP_BTR_M_Ib_A16"},			{A32 | 0x0fba30, "_OP_BTR_M_Ib_A32"},
		{O32 | 0x0fba30, "_OP_BTR_M_Ib_A16"},	{A32 | O32 | 0x0fba30, "_OP_BTR_M_Ib_A32"},
		{0x0fba38, "_OP_BTC_M_Ib_A16"},			{A32 | 0x0fba38, "_OP_BTC_M_Ib_A32"},
		{O32 | 0x0fba38, "_OP_BTC_M_Ib_A16"},	{A32 | O32 | 0x0fba38, "_OP_BTC_M_Ib_A32"},
		
		{0x0fbae0, "_OP_BT_Rw_Ib"},				{A32 | 0x0fbae0, "_OP_BT_Rw_Ib"},
		{O32 | 0x0fbae0, "_OP_BT_Rd_Ib"},		{A32 | O32 | 0x0fbae0, "_OP_BT_Rd_Ib"},
		{0x0fbae8, "_OP_BTS_Rw_Ib"},			{A32 | 0x0fbae8, "_OP_BTS_Rw_Ib"},
		{O32 | 0x0fbae8, "_OP_BTS_Rd_Ib"},		{A32 | O32 | 0x0fbae8, "_OP_BTS_Rd_Ib"},
		{0x0fbaf0, "_OP_BTR_Rw_Ib"},			{A32 | 0x0fbaf0, "_OP_BTR_Rw_Ib"},
		{O32 | 0x0fbaf0, "_OP_BTR_Rd_Ib"},		{A32 | O32 | 0x0fbaf0, "_OP_BTR_Rd_Ib"},
		{0x0fbaf8, "_OP_BTC_Rw_Ib"},			{A32 | 0x0fbaf8, "_OP_BTC_Rw_Ib"},
		{O32 | 0x0fbaf8, "_OP_BTC_Rd_Ib"},		{A32 | O32 | 0x0fbaf8, "_OP_BTC_Rd_Ib"},

		{0x0fbb00, "_OP_BTC_M_Gw_A16"},			{A32 | 0x0fbb00, "_OP_BTC_M_Gw_A32"},
		{O32 | 0x0fbb00, "_OP_BTC_M_Gd_A16"},	{A32 | O32 | 0x0fbb00, "_OP_BTC_M_Gd_A32"},
		{0x0fbbc0, "_OP_BTC_Rw_Gw"},			{A32 | 0x0fbbc0, "_OP_BTC_Rw_Gw"},
		{O32 | 0x0fbbc0, "_OP_BTC_Rd_Gd"},		{A32 | O32 | 0x0fbbc0, "_OP_BTC_Rd_Gd"},

		{0x0fbc00, "_OP_BSF_Gw_Mw_A16"},		{A32 | 0x0fbc00, "_OP_BSF_Gw_Mw_A32"},
		{O32 | 0x0fbc00, "_OP_BSF_Gd_Md_A16"},	{A32 | O32 | 0x0fbc00, "_OP_BSF_Gd_Md_A32"},
		{0x0fbcc0, "_OP_BSF_Gw_Rw"},			{A32 | 0x0fbcc0, "_OP_BSF_Gw_Rw"},
		{O32 | 0x0fbcc0, "_OP_BSF_Gd_Rd"},		{A32 | O32 | 0x0fbcc0, "_OP_BSF_Gd_Rd"},
		{0x0fbd00, "_OP_BSR_Gw_Mw_A16"},		{A32 | 0x0fbd00, "_OP_BSR_Gw_Mw_A32"},
		{O32 | 0x0fbd00, "_OP_BSR_Gd_Md_A16"},	{A32 | O32 | 0x0fbd00, "_OP_BSR_Gd_Md_A32"},
		{0x0fbdc0, "_OP_BSR_Gw_Rw"},			{A32 | 0x0fbdc0, "_OP_BSR_Gw_Rw"},
		{O32 | 0x0fbdc0, "_OP_BSR_Gd_Rd"},		{A32 | O32 | 0x0fbdc0, "_OP_BSR_Gd_Rd"},

		{0x0fbe00, "_OP_MOVSX_Gw_Mb_A16"},		{A32 | 0x0fbe00, "_OP_MOVSX_Gw_Mb_A32"},
		{O32 | 0x0fbe00, "_OP_MOVSX_Gd_Mb_A16"},{A32 | O32 | 0x0fbe00, "_OP_MOVSX_Gd_Mb_A32"},
		{0x0fbec0, "_OP_MOVSX_Gw_Rb"},			{A32 | 0x0fbec0, "_OP_MOVSX_Gw_Rb"},
		{O32 | 0x0fbec0, "_OP_MOVSX_Gd_Rb"},	{A32 | O32 | 0x0fbec0, "_OP_MOVSX_Gd_Rb"},
		{0x0fbf00, "_OP_MOVSX_Gw_Mw_A16"},		{A32 | 0x0fbf00, "_OP_MOVSX_Gw_Mw_A32"},
		{O32 | 0x0fbf00, "_OP_MOVSX_Gd_Mw_A16"},{A32 | O32 | 0x0fbf00, "_OP_MOVSX_Gd_Mw_A32"},
		{0x0fbfc0, "_OP_MOVSX_Gw_Rw"},			{A32 | 0x0fbfc0, "_OP_MOVSX_Gw_Rw"},
		{O32 | 0x0fbfc0, "_OP_MOVSX_Gd_Rw"},	{A32 | O32 | 0x0fbfc0, "_OP_MOVSX_Gd_Rw"},

		{0x0fc8, "_OP_BSWAP_EAX"},			{A32 | 0x0fc8, "_OP_BSWAP_EAX"},
		{O32 | 0x0fc8, "_OP_BSWAP_EAX"},	{A32 | O32 | 0x0fc8, "_OP_BSWAP_EAX"},
		{0x0fc9, "_OP_BSWAP_ECX"},			{A32 | 0x0fc9, "_OP_BSWAP_ECX"},
		{O32 | 0x0fc9, "_OP_BSWAP_ECX"},	{A32 | O32 | 0x0fc9, "_OP_BSWAP_ECX"},
		{0x0fca, "_OP_BSWAP_EDX"},			{A32 | 0x0fca, "_OP_BSWAP_EDX"},
		{O32 | 0x0fca, "_OP_BSWAP_EDX"},	{A32 | O32 | 0x0fca, "_OP_BSWAP_EDX"},
		{0x0fcb, "_OP_BSWAP_EBX"},			{A32 | 0x0fcb, "_OP_BSWAP_EBX"},
		{O32 | 0x0fcb, "_OP_BSWAP_EBX"},	{A32 | O32 | 0x0fcb, "_OP_BSWAP_EBX"},
		{0x0fcc, "_OP_BSWAP_ESP"},			{A32 | 0x0fcc, "_OP_BSWAP_ESP"},
		{O32 | 0x0fcc, "_OP_BSWAP_ESP"},	{A32 | O32 | 0x0fcc, "_OP_BSWAP_ESP"},
		{0x0fcd, "_OP_BSWAP_EBP"},			{A32 | 0x0fcd, "_OP_BSWAP_EBP"},
		{O32 | 0x0fcd, "_OP_BSWAP_EBP"},	{A32 | O32 | 0x0fcd, "_OP_BSWAP_EBP"},
		{0x0fce, "_OP_BSWAP_ESI"},			{A32 | 0x0fce, "_OP_BSWAP_ESI"},
		{O32 | 0x0fce, "_OP_BSWAP_ESI"},	{A32 | O32 | 0x0fce, "_OP_BSWAP_ESI"},
		{0x0fcf, "_OP_BSWAP_EDI"},			{A32 | 0x0fcf, "_OP_BSWAP_EDI"},
		{O32 | 0x0fcf, "_OP_BSWAP_EDI"},	{A32 | O32 | 0x0fcf, "_OP_BSWAP_EDI"},
#endif
	};
}

void VMachine::LoadInstructionTranslations(WORD resourceID)
{
	//Open the resource
	Byte * helperData;
	Dword helperSize;

	if(!GetResourcePtrAndSize(resourceID, helperData, helperSize))
		throw Ex("Internal Error: Unable to access resource " +
			std::to_string(resourceID));

	//Read the data
	std::vector <Byte> fileData(helperData, helperData + helperSize);

	//Extract the header
	if(fileData.size() < sizeof(ElfHeader))
	{
		throw Ex("Internal Error: File " +
			std::to_string(resourceID) +
				" is not a valid ELF object file");
	}

	ElfHeader header;
	
	memcpy(&header, &fileData[0], sizeof(ElfHeader));

	//Ensure this is a valid 32-bit, little-endian, i386 ELF object file
	if(	header.id[0] != 0x7f || header.id[1] != 'E' ||
		header.id[2] != 'L' || header.id[3] != 'F' ||
		header.id[4] != 0x01 || header.id[5] != 0x01 ||
		header.type != 0x0001 || header.machine != 0x0003)
	{
		throw Ex("Internal Error: File " +
			std::to_string(resourceID) +
				" is not a valid ELF object file");
	}

	//Extract the section headers
	if(header.shEntrySize != sizeof(ElfSectionHeader))
	{
		throw Ex("Internal Error: File " +
			std::to_string(resourceID) +
				" has an unsupported section header entry size");
	}

	std::vector <ElfSectionHeader> sectionHeaders(header.shNumEntries);
	
	memcpy(	&sectionHeaders[0], &fileData[header.shOffset],
			header.shNumEntries * sizeof(ElfSectionHeader));

	//Extract the section name strings
	std::vector <Byte> sectionNameStrings(sectionHeaders[header.shStrTabIndex].size);

	memcpy(	&sectionNameStrings[0],
			&fileData[sectionHeaders[header.shStrTabIndex].offset],
			sectionHeaders[header.shStrTabIndex].size);

	//Extract the string and symbol tables, the .text section and any relocations
	Dword stringTableSection = 0xffffffff, symbolTableSection = 0xffffffff;
	Dword textSection = 0xffffffff, relocationsSection = 0xffffffff;
	
	for(Dword section = 0; section < header.shNumEntries; ++section)
	{
		std::string name = reinterpret_cast<char *>(
							&sectionNameStrings[sectionHeaders[section].name]);

		if(name == ".strtab")
			stringTableSection = section;
		
		if(name == ".symtab")
			symbolTableSection = section;

		if(name == ".text")
			textSection = section;

		if(name == ".rel.text")
			relocationsSection = section;
	}

	assert(stringTableSection != 0xffffffff);
	assert(symbolTableSection != 0xffffffff);
	assert(textSection != 0xffffffff);
	
	bool hasRelocations = relocationsSection != 0xffffffff;

	std::vector <Byte> stringTable(sectionHeaders[stringTableSection].size);

	memcpy(	&stringTable[0],
			&fileData[sectionHeaders[stringTableSection].offset],
			sectionHeaders[stringTableSection].size);

	Dword numSymbols = sectionHeaders[symbolTableSection].size / sizeof(ElfSymbolTableEntry);
	std::vector <ElfSymbolTableEntry> symbolTable(numSymbols);

	memcpy(	&symbolTable[0],
			&fileData[sectionHeaders[symbolTableSection].offset],
			sectionHeaders[symbolTableSection].size);

	std::vector <Byte> text(sectionHeaders[textSection].size);

	memcpy(	&text[0],
			&fileData[sectionHeaders[textSection].offset],
			sectionHeaders[textSection].size);

	std::vector <ElfRelocation> relocations;

	if(hasRelocations)
	{
		relocations.resize(sectionHeaders[relocationsSection].size / sizeof(ElfRelocation));

		memcpy(	&relocations[0],
				&fileData[sectionHeaders[relocationsSection].offset],
				sectionHeaders[relocationsSection].size);
	}

	//Get the symbol index of the "segRegBase" etc symbols
	const int numPatchSymbols = 24;

	const std::string patchSymbolNames[numPatchSymbols] =
	{
		"@ReadByte@4", "@ReadWord@4", "@ReadDword@4",
		"@WriteByte@8", "@WriteWord@8", "@WriteDword@8",
		"_SetDataSegmentRegisterValue", "_SetDataSegmentRegisterValuePtr",
		"_segRegBase", "_baseReg", "_indexReg", "_indexShift", "_displacement",
		"_Rb", "_Gb", "_Rw", "_Gw", "_Rd", "_Gd", "_Sw",
		"_immByte", "_immWord", "_immDword",
		"_instrLength"
	};

	Dword patchSymbolIndices[numPatchSymbols];

	for(int i = 0; i < numPatchSymbols; ++i)
		patchSymbolIndices[i] = 0xffffffff;

	for(Dword symbol = 0; symbol < numSymbols; ++symbol)
	{
		std::string name = reinterpret_cast<char *>(&stringTable[symbolTable[symbol].name]);

		for(int i = 0; i < numPatchSymbols; ++i)
		{
			if(name == patchSymbolNames[i])
				patchSymbolIndices[i] = symbol;
		}
	}

	//Map to hold the translations indexed by name
	std::map <std::string, Translation> tempTranslations;

    //Loop through symbols
	for(Dword symbol = 0; symbol < numSymbols; ++symbol)
	{
		//Get the name of this symbol
		std::string name = reinterpret_cast<char *>(&stringTable[symbolTable[symbol].name]);

		//If the name starts with "_OP_" and ends in something other than "_END"
		if(	name.size() > 4 &&
			name.compare(0, 4, "_OP_") == 0 &&
			name.compare(name.size() - 4, 4, "_END") != 0)
		{
			//Get the start of this translation
			Dword start = symbolTable[symbol].value;

			//Find the end of this translation

			//Find the symbol of the same name, suffixed by "_END"
			Dword end = 0;
			for(Dword endSymbol = 0; endSymbol < numSymbols; ++endSymbol)
			{
				std::string endName =
					reinterpret_cast<char *>(&stringTable[symbolTable[endSymbol].name]);

				if(endName == name + "_END")
				{
					end = symbolTable[endSymbol].value;
					break;
				}
			}

			assert(end != 0);

			//Advance end until we hit a return instruction
			while(text[end] != 0xc3)
				++end;

			//Create an object to hold this translation
			Translation translation;

			std::vector <Dword> * const patchSymbolVectors[numPatchSymbols] =
			{
				&translation.ReadByteOffsets,	&translation.ReadWordOffsets,
				&translation.ReadDwordOffsets,	&translation.WriteByteOffsets,
				&translation.WriteWordOffsets,	&translation.WriteDwordOffsets,
				&translation.SetDataSegmentRegisterValueOffsets,
				&translation.SetDataSegmentRegisterValuePtrOffsets,
				&translation.segRegBaseOffsets,
				&translation.baseRegOffsets, &translation.indexRegOffsets,
				&translation.indexShiftOffsets, &translation.displacementOffsets,
				&translation.RbOffsets, &translation.GbOffsets,
				&translation.RwOffsets, &translation.GwOffsets,
				&translation.RdOffsets, &translation.GdOffsets,
				&translation.SwOffsets,
				&translation.immByteOffsets, &translation.immWordOffsets,
				&translation.immDwordOffsets, &translation.instrLengthOffsets
			};

			//Copy the code into this translation object
			std::copy(	text.begin() + start, text.begin() + end,
						std::back_inserter(translation.translation));

			//Place the offsets of any relevant relocations into the translation object
			if(hasRelocations)
			{
				for(std::vector<ElfRelocation>::const_iterator relocation = relocations.begin();
					relocation != relocations.end(); ++relocation)
				{
					if(relocation->offset >= start && relocation->offset < end)
					{
						for(int i = 0; i < numPatchSymbols; ++i)
						{
							if(relocation->symbol == patchSymbolIndices[i])
								patchSymbolVectors[i]->push_back(relocation->offset - start);
						}
					}
				}
			}

			//Insert this translation object into the map
			tempTranslations.insert(std::make_pair(name, translation));
		}
	}

	//Convert the translations indexed by name to be indexed by opcode
	for(Dword i = 0; i < sizeof(opcodeOpnames)/sizeof(OpcodeOpname); ++i)
	{
		std::map<std::string, Translation>::const_iterator translation =
			tempTranslations.find(opcodeOpnames[i].opname);

		if(translation != tempTranslations.end())
			translations.insert(std::make_pair(opcodeOpnames[i].opcode, translation->second));
	}
}
