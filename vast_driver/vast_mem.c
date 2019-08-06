#include <string.h>
#include <vast_mem.h>

/*分区表的定义*/
typedef struct  
{	void *address;  	//存放地址
	MEM_UINT length;	//存放长度，单位为字节
	MEM_UINT flag;      //存放登记栏标志，用"0"表示空栏目，用"1"表示已分配
}table;

typedef struct
{
	MEM_INT length;   //内存对齐后长度
	void *address; //内存对齐后首地址
}mem_crc;

/*分区表*/
typedef struct  
{	
	MEM_INT endtablep;      //访问末端指针
	table free_table[MAX_BLOCK];
	mem_crc mcrc;
}memtable;

static MEM_UCHAR memsize[MAX_SIZE]={0};//内部SRAM1内存池

static memtable  mem ;//空闲区表
	
static memtable *MEM=0;

/*******************************************************************************
  * 函数名称：mymeminit
  * 函数说明：内存块初始化.
*******************************************************************************/
void vast_meminit(void)
{
	MEM_UINT i=0;
	
	MEM_UCHAR *p=0;
	
	MEM = (memtable *)&mem;    //定义内存指针
	
	for(i=0;i<MAX_SIZE;i++)			 //清空缓存区数值
	{
		memsize[i] = 0;				
	}
	
	for(i=0;i<MAX_BLOCK;i++)			//清空空闲分区表的数值
	{
		MEM->free_table[i].address=0;
		MEM->free_table[i].length = 0;
		MEM->free_table[i].flag=0;
	}

	p = memsize;
	
	while((MEM_UINT)p%4) //内存对齐
	{
		p +=1;
	}
	
	MEM->mcrc.address= p;
	
	MEM->mcrc.length=MAX_SIZE-((MEM_UINT)p-(MEM_UINT)memsize);
	
	if(MEM->mcrc.length != MAX_SIZE)
	{
		MEM->mcrc.length -=(4-((MEM_UINT)p-(MEM_UINT)memsize));
	}

	MEM->free_table[0].address = MEM->mcrc.address;  //将内存首地址装入空闲分区表起始地址
	MEM->free_table[0].length = MEM->mcrc.length;	//将内存最大空间值装入空闲分区表空闲区长度
	MEM->endtablep = -1;
}

#ifdef MEMCRC
void memcrc(void)
{
	MEM_UINT n=0;
	MEM_UINT i=0;
	MEM_UINT temp_size=0;
	MEM_UINT temp_num=0;

	if(MEM->free_table[0].address != MEM->mcrc.address)
	{
				//cout<<"校验错误1 "<<i<<"\n" ;
				while(1);
	}	
	else
	{	
		MEM_UINT err=1;
		
		n = MEM->endtablep+1;
		
		/*************************校验分区表指针前数据******************/
		if(n < MAX_BLOCK) //第一次校验
		{
			temp_size = (MEM_ADD)MEM->free_table[n].address - (MEM_ADD)MEM->free_table[0].address;	
			temp_size += MEM->free_table[n].length;
			
			if(temp_size != MEM->mcrc.length)
			{
				err = 1;    //第一次校验错误
			}
			else
			{
				err = 0;    //第一次校验正确
				temp_num=n;
			}
		}
		
		if(err == 1)    //第二次校验
		{
			temp_size = (MEM_ADD)MEM->free_table[MEM->endtablep].address - (MEM_ADD)MEM->free_table[0].address;	
			temp_size += MEM->free_table[MEM->endtablep].length;
			
			if(temp_size != MEM->mcrc.length)
			{
				//cout<<"校验错误2 "<<i<<"\n" ;
				err = 1;    //第二次校验错误
				while(1);	
			}
			else
			{
				err = 0;    //第二次校验正确
				temp_num=MEM->endtablep;
			}
		}
		for(i=0;i<temp_num;i++)	
		{	
			if(((MEM_ADD)MEM->free_table[i].address + MEM->free_table[i].length)!=(MEM_ADD)MEM->free_table[i+1].address)
			{
				//cout<<"校验错误3 "<<i<<"\n" ;
				while(1);
			}	
		}	
		if(((MEM_ADD)MEM->free_table[temp_num].address + MEM->free_table[temp_num].length)!=\
			                               ((MEM_ADD)MEM->free_table[0].address+MEM->mcrc.length))
		{
			//cout<<"校验错误4 "<<i<<"\n" ;
			while(1);
		}
		
		for(i=0;i<temp_num;i++)	 //不允许有内存地址分配但是没有容量的内存块表
		{		
			if(MEM->free_table[i].length == 0)
			{
				//cout<<"校验错误5 "<<i<<"?"<<MEM->free_table[i].length<<"\n" ;
				while(1);
			}
		}	

		/*************************校验分区表指针后数据******************/
		for(i=(temp_num+1);i<MAX_BLOCK;i++)			 
		{
			if(MEM->free_table[i].address != 0)
			{
				//cout<<"校验错误6 "<<i<<"?"<<MEM->free_table[i].address<<"\n" ;
				while(1);
			}		
			if(MEM->free_table[i].length != 0)
			{
				//cout<<"校验错误7 "<<i<<"?"<<MEM->free_table[i].length<<"\n" ;
				while(1);
			}
			if(MEM->free_table[i].flag != 0)
			{
				//cout<<"校验错误8 "<<i<<"?"<<MEM->free_table[i].flag<<"\n" ;
				while(1);
			}	
		}

		if(temp_num > MEM->endtablep)
		{
			if(MEM->free_table[temp_num].flag != 0)
			{
				//cout<<"校验错误9 "<<i<<"?"<<MEM->free_table[i].flag<<"\n" ;
				while(1);
			}	
		}
	}
}
#endif

void *mymallocrun(MEM_UINT i,MEM_UINT size)
{
		MEM_UINT m = 0,n =0,s=0;	
		MEM_UINT temp_size=0;
		void * temp_addr=0;
	
		if(size < MEM->free_table[i].length)					//判断申请字节小于可用空间字节
		{
			n = i+1;  //计算下一个空闲分区表
			
			if(n < MAX_BLOCK)
			{	
				if(MEM->endtablep+1==i)  //新内存分配
				{
					s=n;		
				
					temp_addr = MEM->free_table[i].address;
					temp_size =  MEM->free_table[i].length;
						
					MEM->free_table[s].address = (void *)((MEM_ADD)temp_addr+size);
					MEM->free_table[s].length = temp_size - size;
					
				
					MEM->free_table[i].length = size;		
					MEM->free_table[i].flag  = 1;	
					MEM->endtablep = i;							//记录表指针位置

					return (void*)(MEM->free_table[i].address);					//返回地址
				}//下边为以前释放的内存块管理
				else if(MEM->endtablep == END_BLOCK)//当前记录表为末端表无法往后移动一个分区表
				{					
					MEM->free_table[i].flag = 1;	
		
					return (void*)(MEM->free_table[i].address);					//返回地址
				}
				else if((MEM->endtablep+1) == END_BLOCK) //末端表为空禁止往后移动一个分区表(防止内存溢出)
				{					
					MEM->free_table[i].flag = 1;	
		
					return (void*)(MEM->free_table[i].address);					//返回地址
				}
				else
				{													
					m = MEM->endtablep+1;							
					
					while(m >= n)
					{
						MEM->free_table[m+1].address = MEM->free_table[m].address;	//将地址往后移动一个分区表

						MEM->free_table[m+1].length = MEM->free_table[m].length;		//将数据长度移动一个分区表

						MEM->free_table[m+1].flag  = MEM->free_table[m].flag ;		  //将标志移动一个分区表
						
						m--;
					}			
					temp_size = MEM->free_table[i].length;

					MEM->free_table[i].length = size;
					MEM->free_table[i].flag = 1;	
				
					MEM->free_table[n].address = (void*)((MEM_ADD)MEM->free_table[i].address + MEM->free_table[i].length);
					MEM->free_table[n].length = temp_size - size;
					MEM->free_table[n].flag = 0;
					MEM->endtablep += 1;		//记录表指针位置

					return (void*)(MEM->free_table[i].address);					//返回地址
				}	
			}	
			else
			{		
				 //当前记录表为末端表
				MEM->free_table[i].flag = 1;
				MEM->endtablep = END_BLOCK;		//记录结束表指针位置
		
				////cout<<"已到记录表末端"<<n<<"\n";
				return (void*)(MEM->free_table[i].address);					//返回地址
			}						
		}
		else  //当前空间与申请空间相等
		{	
			MEM->free_table[i].flag  = 1;	
			
			if(MEM->endtablep < i){MEM->endtablep = i;}	//记录表指针位置

			return (void*)(MEM->free_table[i].address);					//返回地址
		}									
}

/*******************************************************************************
  * 函数名称：mycalloc
  * 函数说明：内存分配.
  * 输入参数：size要分配字节大小.
  * 函数返回：返回分配内存首地址.
*******************************************************************************/
void *vast_malloc(MEM_UINT size)
{
	MEM_UINT i = 0;
	MEM_UINT temp_size = 0;
    void *addr = 0;

	for(i=0;i<MAX_BLOCK;i++)
	{
		if(MEM->free_table[i].flag == 0)
		{
			temp_size = (size/MIN_SIZE)*MIN_SIZE;
			
			if(size%MIN_SIZE){temp_size += MIN_SIZE;}

			if(temp_size <= MEM->free_table[i].length)						//判断当前空间
			{		
					
				addr=mymallocrun(i,temp_size);

				#ifdef MEMCRC
				memcrc(); //内存CRC校验
				#endif

				return (void*)addr;
			}
			else
			{ 
				continue;					/*跳出本次循环*/
			}	
		}
	}

	if(i >= MAX_BLOCK)
	{
		//cout<<"超出块范围 "<<i<<"\n";
		//while(1);
	}
	return (void*)0;		
}


void myfreerunLeftRight(MEM_UINT i)
{
	 MEM_UINT m = 0,n =0,s=0;	
	
	 n = i;
		
	 MEM->free_table[i-1].length += MEM->free_table[n].length;
	 MEM->free_table[i-1].length += MEM->free_table[n+1].length;
		
		if(i == MEM->endtablep)  //释放末端记录表
		{			
			MEM->free_table[n+1].address = 0;	
			MEM->free_table[n+1].length = 0;
		  
			MEM->free_table[n].address = 0;	
			MEM->free_table[n].length = 0;
			MEM->free_table[n].flag  = 0;  
		}
		else
		{
			m = MEM->endtablep+1;	
			
			while(m > n)
			{
				s = n+2;
				if(s >= MAX_BLOCK)
        {
					MEM->free_table[n].address = 0;
					MEM->free_table[n].length = 0;
					MEM->free_table[n].flag  = 0;	
				} //超出分区表拷贝范围
				else
				{
					MEM->free_table[n].address = MEM->free_table[s].address;	//将地址往前移动一个分区表
					MEM->free_table[n].length = MEM->free_table[s].length;		//将数据长度移动一个分区表
					MEM->free_table[n].flag  = MEM->free_table[s].flag ;		//将标志移动一个分区表
				}					
				n++;
			}
			if(m < MAX_BLOCK) //清理移动后残余分区表块
			{
				MEM->free_table[m].address = 0;	
				MEM->free_table[m].length = 0;		
			}
		}
		MEM->endtablep -= 2;	//记录表指针位置往前移动2分区表
}

void myfreerunLeft(MEM_UINT i)
{
	 MEM_UINT m = 0,n =0,s=0;	
	
	  n = i;
	
		MEM->free_table[i-1].length += MEM->free_table[n].length;
		
		if(i == MEM->endtablep)  //释放末端记录表
		{				
			 	MEM->free_table[n].address = 0;	
				MEM->free_table[n].length = 0;
			  MEM->free_table[n].flag  = 0;
        MEM->endtablep -= 2;	//记录表指针位置往前移动2分区表
		}
		else
		{
			m = MEM->endtablep+1;
			
			while(m > n)
			{
				s = n+1;
				if(s >= MAX_BLOCK)
				{
					MEM->free_table[n].address = 0;
					MEM->free_table[n].length = 0;
					MEM->free_table[n].flag  = 0;	
				} //超出分区表拷贝范围
				else
				{
					MEM->free_table[n].address = MEM->free_table[s].address;	//将地址往前移动一个分区表
					MEM->free_table[n].length = MEM->free_table[s].length;		//将数据长度移动一个分区表
					MEM->free_table[n].flag  = MEM->free_table[s].flag ;		//将标志移动一个分区表
				}					
				n++;
			}
			if(m < MAX_BLOCK) //清理移动后残余分区表块
			{
				MEM->free_table[m].address = 0;	
				MEM->free_table[m].length = 0;		
			}
			MEM->endtablep -= 1;	//记录表指针位置往前移动1分区表
		}	
}

void myfreerunRight(MEM_UINT i)
{
	 MEM_UINT m = 0,n =0,s=0;	
	
	  n = i+1;
			
		MEM->free_table[i].length += MEM->free_table[n].length;
	  MEM->free_table[i].flag  = 0;
	
	  if(i == MEM->endtablep)  //释放末端记录表
		{				
			 	MEM->free_table[n].address = 0;	
				MEM->free_table[n].length = 0;						
		}
		else
		{
			m = MEM->endtablep+1;
			
			while(m > n)
			{
				s = n+1;
				if(s >= MAX_BLOCK)
        {
					MEM->free_table[n].address = 0;
					MEM->free_table[n].length = 0;
					MEM->free_table[n].flag  = 0;	
				} //超出分区表拷贝范围
				else
				{
					MEM->free_table[n].address = MEM->free_table[s].address;	//将地址往前移动一个分区表
					MEM->free_table[n].length = MEM->free_table[s].length;		//将数据长度移动一个分区表
					MEM->free_table[n].flag  = MEM->free_table[s].flag ;		  //将标志移动一个分区表
				}	
				n++;
			}	
      if(m < MAX_BLOCK) //清理移动后残余分区表块
			{
				MEM->free_table[m].address = 0;	
				MEM->free_table[m].length = 0;		
			}
		}
		MEM->endtablep -= 1;	//记录表指针位置往前移动1分区表
}

/*******************************************************************************
  * 函数名称：myfree
  * 函数说明：内存释放.
  * 输入参数：ptr要释放的地址.
  * 函数返回：无.
*******************************************************************************/
void vast_free(void *ptr)
{
	MEM_UINT i = 0;
	
	if(ptr == 0)
	{
		//cout<<"指针错误 "<<(MEM_UINT )(addr)<<"\n" ;
		while(1);	
	}
	
	while(i<MAX_BLOCK)
	{
		if(MEM->free_table[i].address == ptr)
		{				
			if((i==0)&&(MEM->free_table[i+1].flag == 0))
			{
				myfreerunRight(i);               //右边
			}
			else if((i==END_BLOCK)&&(MEM->free_table[i-1].flag == 0))
			{
				myfreerunLeft(i);								//左边
			}
			else if((MEM->free_table[i-1].flag == 0)&&(MEM->free_table[i+1].flag == 0))//左边右边
			{
				myfreerunLeftRight(i);
			}	
			else if(MEM->free_table[i-1].flag == 0)//左边
			{
				myfreerunLeft(i);
			}
			else if(MEM->free_table[i+1].flag == 0)//右边
			{
				myfreerunRight(i);	
			}
			else
			{
				MEM->free_table[i].flag  = 0 ;
				
				//末端记录表记录表指针位置
				if(i== MEM->endtablep){MEM->endtablep -= 1;}
			}
			
			#ifdef MEMCRC
			memcrc(); //内存CRC校验
			#endif

			break;
		}
		i++;
	}
    if(i>=MAX_BLOCK)
	{
		//cout<<"找不到释放的地址或地址错误 "<<(MEM_UINT )(addr)<<"\n" ;
		while(1);	
	}
}

/*******************************************************************************
  * 函数名称：myrealloc
  * 函数说明：内存分配2(将ptr指针拷贝到新内存块后并释放ptr内存块).
  * 输入参数：nbytes.要申请新的内存块大小.
  * 函数返回：返回已分配新的内存首地址.
*******************************************************************************/
void *vast_realloc(void *ptr, MEM_UINT nbytes)
{
	MEM_UINT i=0;
	MEM_UINT length=0;
	void *dst=0;
	
	if(ptr != 0)
	{
		while(1);
	}
	
	while(i<MAX_BLOCK)
	{
		if(MEM->free_table[i].address == ptr)
		{		
			length = MEM->free_table[i].length ;
			
			if(nbytes >= length)
			{				
				dst = vast_malloc(nbytes);			//内存分配(外部调用)
				memcpy((void *)dst, (void *)ptr, length);
				vast_free(ptr);  					//内存释放(外部调用)
				return (void*)dst;				
			}
			else
			{
				//cout<<"空间不匹配 "<<(MEM_UINT )(addr)<<"\n" ;
				while(1);
				//return (void*)0;
			}
		}
		i++;
	}		
	if(i>=MAX_BLOCK)
	{
		//cout<<"找不到释放的地址或地址错误 "<<(MEM_UINT )(addr)<<"\n" ;
		while(1);	
	}

	return (void*)0;
}

/*******************************************************************************
  * 函数名称：mycalloc
  * 函数说明：内存分配.
  * 输入参数：count多少内存单元.
  * 输入参数：size单元字节数.
  * 函数返回：返回分配首单元内存地址.
*******************************************************************************/
void *vast_calloc(MEM_UINT count, MEM_UINT size)
{	
	void *dst=0;
	
	dst=vast_malloc(count*size);
	
	memset(dst, 0, count*size);
	
	return (void*)dst;		//内存分配(外部调用)
}


#ifdef MEMCRC
/*******************************************************************************
  * 函数名称：Getmemtablestatus
  * 函数说明：获取内存状态.
  * 输入参数：无.
  * 函数返回：无.
*******************************************************************************/
void vast_getmemtablestatus(void)
{
	MEM_UINT m=0,j=0;
	
	
	 memcrc(); //内存CRC校验
	printf("**************************校验完成**********************************\r\n");
	
		while(m < MAX_BLOCK)
		{
			if(MEM->free_table[m].flag == 1)
			{
				j=0;
				//printf("**************************已用块信息****************************\r\n");
				printf("已用块信息:\r\n");
				printf("MEM->free_table[%d].address=%p, length=%d, flag=%d\r\n\r\n"
														, m
														, MEM->free_table[m].address
														, MEM->free_table[m].length
														, MEM->free_table[m].flag);
			}
			else if(MEM->free_table[m].address != 0)
			{
				//printf("**************************中间未用块信息****************************\r\n");
				printf("中间未用块信息:\r\n");
				printf("MEM->free_table[%d].address=%p, length=%d, flag=%d\r\n\r\n"
														, m
														, MEM->free_table[m].address
														, MEM->free_table[m].length
														, MEM->free_table[m].flag);
				j++;
			}

			if((j >= 2)||(m == (END_BLOCK)))	//找到末端未用块
			{
				if(j >= 2){m--;}
				//printf("**************************末端块信息********************************\r\n");
				printf("末端块信息:\r\n");
				printf("MEM->free_table[%d].address=%p, length=%d, flag=%d\r\n\r\n"
														, m
														, MEM->free_table[m].address
														, MEM->free_table[m].length
														, MEM->free_table[m].flag);
				break;
			}	
			m++;
		}
}
#endif
