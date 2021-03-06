// 2008 12 11
// maker : Han,sang woon
#define OvExportDll
#include "OvMemoryPool.h"

#ifdef _DEBUG
#include "OvUtility.h"
DWORD	g_dMemoryCount = 0;
void		OvMemoryPool::report_abnormal_memory_release(OvPoolHeader* _pPoolList)
{
	if (_pPoolList)
	{
		for (;_pPoolList;_pPoolList = _pPoolList->mNext)
		{
			for (int i=0;i < STATIC_ALLOC_COUNT;i++)
			{
				OvMemHeader*	k_sequence  = NULL;
				k_sequence			=	(OvMemHeader*)(POOL(_pPoolList)+(MEM_BLOCK_SIZE*i));
				if ( k_sequence && ( k_sequence->m_pBlock && k_sequence->m_iLine != -1 ) )
				{
					OutputDebugString(
						OU::string::format(
						"=================================================\n"
						"[Block: %8s]\n"
						"[Line: %5d]\n"
						"[Size: %5d Byte]\n"
						"[Address: %p]\n"
						"=================================================\n"
						,k_sequence->m_pBlock
						,k_sequence->m_iLine
						,m_stTypeIndexFromMemSize
						,MEMORY(k_sequence)
						).c_str()
						);
				}
			}
		}
	}
}
#endif


void	OvMemoryPool::_constructor(std::size_t _size_type)
{
	m_stTypeIndexFromMemSize = _size_type;
	m_pFreeMemoryList = NULL;
	m_pPoolList = NULL;
}

void	OvMemoryPool::_destructor()
{
#ifdef _DEBUG
	report_abnormal_memory_release(m_pPoolList);
#endif
	// 紺暗 蒸陥.
	OvPoolHeader*	k_next	=	m_pPoolList;
	OvPoolHeader*	k_header	=	NULL;
	do 
	{
		k_header = k_next;

		if(k_next)
			k_next	 = k_next->mNext;

		free(((void*)k_header));
#ifdef _DEBUG
		OutputDebugString(OU::string::format("Olive Memory Pool Report Free(%8d byte) CallCount: %4d\n",MEM_POOL_SIZE,g_dMemoryCount).c_str());
		g_dMemoryCount--;
#endif
	} while (k_next);

}

bool		OvMemoryPool::add_pool()
{
	// MEM_POOL_SIZE澗 古滴稽陥.
	// MEM_POOL_SIZE = ( 眼雁滴奄 * 奄沙拝雁遂姐呪(256鯵稽 績税竺舛) ) + sizeof(OvPoolHeader)
	OvPoolHeader*	k_pool = (OvPoolHeader*)malloc(sizeof(BYTE)*MEM_POOL_SIZE);
	
	// 搾闘級聖 0生稽 熟 舛軒, 瓜戚 照背操亀 鞠走幻 重持焼艦猿.
	memset(k_pool,0,sizeof(BYTE)*MEM_POOL_SIZE);
	OvPoolHeader*	k_search = NULL;

	// 軒什闘拭 蓄亜. 戚背亜 照亜檎 益顕生稽 背左檎 岩, 榎号蟹紳陥.
	// 尻衣幻 吉陥.
	k_pool->mNext	=	m_pPoolList;
	m_pPoolList		=	k_pool;

	// 紫遂 亜管廃 五乞軒攻帖研 企奄五乞軒 匂昔斗拭 実特廃陥.
	m_pFreeMemoryList	=	(OvMemHeader*)POOL(k_pool);
	OvMemHeader*	k_sequence  = NULL;
	// 拝舘吉 五乞軒攻帖 = ( 眼雁滴奄 * 奄沙拝雁遂姐呪(256鯵稽 績税竺舛) )
	// 研 紫遂亜管馬惟 ( sizeof(OvMemHeader) + 眼雁滴奄 )稽 促牽壱,
	// OvMemHeader研 是廃 蒋楕 4郊戚闘継 戚推背辞 辞稽研 広澗陥.
	for (int i=0;i<(STATIC_ALLOC_COUNT - 1);i++)
	{
		k_sequence			=	(OvMemHeader*)(POOL(k_pool)+(MEM_BLOCK_SIZE*i));
		k_sequence->mNext	=	(OvMemHeader*)(POOL(k_pool)+(MEM_BLOCK_SIZE*(i+1)));
#ifdef _DEBUG
		k_sequence->m_pBlock=	NULL;
		k_sequence->m_iLine	=	-1;
#endif
	}
#ifdef _DEBUG
	g_dMemoryCount++;
	OutputDebugString(OU::string::format("Olive Memory Pool Report Alloc(%8d byte) CallCount: %4d\n",MEM_POOL_SIZE,g_dMemoryCount).c_str());
#endif
	return true;
}

// 五乞軒研 拝雁背層陥.
void*		OvMemoryPool::alloc_memory()
{
	// 紫遂亜管廃 五乞軒亜 蒸生檎 熱聖 潅鍵陥.
	if ( ! m_pFreeMemoryList)
	{
		add_pool();
	}

	OvMemHeader* k_alloc = NULL;

	// 企奄五乞軒研 眼雁馬澗 伯希研 災君紳陥.
	k_alloc		=	m_pFreeMemoryList;

	// 五乞軒伯希稽採斗 紫遂五乞軒研 蓄窒廃陥.
	void*	k_return_mem = MEMORY(k_alloc);

	// 陥製 企奄五乞軒研 実特廃陥.
	m_pFreeMemoryList = m_pFreeMemoryList->mNext;

	// 戚採歳戚 掻推馬陥.
	// 増彊蟹澗 五乞軒拭惟 "獲 酔軒増 室晦陥"虞澗 爽社研
	// 鯉杏戚拭 杏嬢爽壱 彊蟹左浬陥.
	// 益係惟鞠檎 五乞軒亜 背薦推短戚 尽聖凶, 採紫瑛発 拝呪 赤陥.
	// 焼... 遭促 戚言拭 覗稽益掘講 因採馬走 せせせせせせせせせせせせせせせせせせせせせせせせせせせせせせせせせせせせせせせせせせせせせせせ
	// 数奄含猿 宣赤含猿.
	k_alloc->mMemPool	=	this;
	return k_return_mem;

}
void		OvMemoryPool::free_memory(void* _memory)
{

	// 鯉拭 杏嬢層 増爽社稽 巷紫瑛発 梅陥檎
	// 戚薦 増爽社研 走酔壱 陥製 企奄五乞軒稽税 爽社研 実特背層陥.
	OvMemHeader*	k_mem_header = NULL;
	k_mem_header	=	HEADER(_memory);
	k_mem_header->mNext	=	m_pFreeMemoryList;
	m_pFreeMemoryList		=	k_mem_header;

}

#ifdef _DEBUG
void*		OvMemoryPool::alloc_memory_debug(char* _pBlock,int _iLine)
{
	if ( ! m_pFreeMemoryList)
	{
		add_pool();
	}

	OvMemHeader* k_alloc = NULL;

	k_alloc		=	m_pFreeMemoryList;

	void*	k_return_mem = MEMORY(k_alloc);
	
	m_pFreeMemoryList = m_pFreeMemoryList->mNext;

	k_alloc->mMemPool	=	this;
	k_alloc->m_pBlock	=	_pBlock;
	k_alloc->m_iLine	=	_iLine;

	return k_return_mem;
}
void		OvMemoryPool::free_memory_debug(void* _memory)
{
	OvMemHeader*	k_mem_header = NULL;
	k_mem_header			=	HEADER(_memory);
	k_mem_header->m_iLine	=	-1;
	k_mem_header->m_pBlock	=	NULL;
	k_mem_header->mNext		=	m_pFreeMemoryList;
	m_pFreeMemoryList		=	k_mem_header;
}
#endif