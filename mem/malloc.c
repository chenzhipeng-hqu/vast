#include "malloc.h"	
#include <string.h>

/*������Ķ���*/  
typedef struct  
{	void *address;  	//��ŵ�ַ
	MEM_UINT length;	//��ų��ȣ���λΪ�ֽ�   
	MEM_UINT flag;      //��ŵǼ�����־����"0"��ʾ����Ŀ����"1"��ʾ�ѷ��� 
}table;

typedef struct
{
	MEM_INT length;   //�ڴ����󳤶�
	void *address; //�ڴ������׵�ַ
}mem_crc;

/*������*/
typedef struct  
{	
	MEM_INT endtablep;      //����ĩ��ָ��  
	table free_table[MAX_BLOCK];
	mem_crc mcrc;
}memtable;

static MEM_UCHAR memsize[MAX_SIZE]={0};//�ڲ�SRAM1�ڴ��

static memtable  mem __attribute__((at(0X10000000)));//��������
	
static memtable *MEM=0;

/*******************************************************************************
  * �������ƣ�mymeminit
  * ����˵�����ڴ���ʼ��.        
*******************************************************************************/
void mymeminit(void)
{
	MEM_UINT i=0;
	
	MEM_UCHAR *p=0;
	
	MEM = (memtable *)&mem;    //�����ڴ�ָ��
	
	for(i=0;i<MAX_SIZE;i++)			 //��ջ�������ֵ
	{
		memsize[i] = 0;				
	}
	
	for(i=0;i<MAX_BLOCK;i++)			//��տ��з��������ֵ
	{
		MEM->free_table[i].address=0;
		MEM->free_table[i].length = 0;
		MEM->free_table[i].flag=0;
	}

	p = memsize;
	
	while((MEM_UINT)p%4) //�ڴ����
	{
		p +=1;
	}
	
	MEM->mcrc.address= p;
	
	MEM->mcrc.length=MAX_SIZE-((MEM_UINT)p-(MEM_UINT)memsize);
	
	if(MEM->mcrc.length!=MAX_SIZE)
	{
		MEM->mcrc.length -=(4-((MEM_UINT)p-(MEM_UINT)memsize));
	}

	MEM->free_table[0].address = MEM->mcrc.address;  //���ڴ��׵�ַװ����з�������ʼ��ַ
	MEM->free_table[0].length = MEM->mcrc.length;	//���ڴ����ռ�ֵװ����з��������������
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
				//cout<<"У�����1 "<<i<<"\n" ;
				while(1);
	}	
	else
	{	
		MEM_UINT err=1;
		
		n = MEM->endtablep+1;
		
		/*************************У�������ָ��ǰ����******************/
		if(n < MAX_BLOCK) //��һ��У��
		{
			temp_size = (MEM_ADD)MEM->free_table[n].address - (MEM_ADD)MEM->free_table[0].address;	
			temp_size += MEM->free_table[n].length;
			
			if(temp_size != MEM->mcrc.length)
			{
				err = 1;    //��һ��У�����
			}
			else
			{
				err = 0;    //��һ��У����ȷ
				temp_num=n;
			}
		}
		
		if(err == 1)    //�ڶ���У��
		{
			temp_size = (MEM_ADD)MEM->free_table[MEM->endtablep].address - (MEM_ADD)MEM->free_table[0].address;	
			temp_size += MEM->free_table[MEM->endtablep].length;
			
			if(temp_size != MEM->mcrc.length)
			{
				//cout<<"У�����2 "<<i<<"\n" ;
				err = 1;    //�ڶ���У�����
				while(1);	
			}
			else
			{
				err = 0;    //�ڶ���У����ȷ
				temp_num=MEM->endtablep;
			}
		}
		for(i=0;i<temp_num;i++)	
		{	
			if(((MEM_ADD)MEM->free_table[i].address + MEM->free_table[i].length)!=(MEM_ADD)MEM->free_table[i+1].address)
			{
				//cout<<"У�����3 "<<i<<"\n" ;
				while(1);
			}	
		}	
		if(((MEM_ADD)MEM->free_table[temp_num].address + MEM->free_table[temp_num].length)!=\
			                               ((MEM_ADD)MEM->free_table[0].address+MEM->mcrc.length))
		{
			//cout<<"У�����4 "<<i<<"\n" ;
			while(1);
		}
		
		for(i=0;i<temp_num;i++)	 //���������ڴ��ַ���䵫��û���������ڴ���
		{		
			if(MEM->free_table[i].length == 0)
			{
				//cout<<"У�����5 "<<i<<"?"<<MEM->free_table[i].length<<"\n" ;
				while(1);
			}
		}	

		/*************************У�������ָ�������******************/
		for(i=(temp_num+1);i<MAX_BLOCK;i++)			 
		{
			if(MEM->free_table[i].address != 0)
			{
				//cout<<"У�����6 "<<i<<"?"<<MEM->free_table[i].address<<"\n" ;
				while(1);
			}		
			if(MEM->free_table[i].length != 0)
			{
				//cout<<"У�����7 "<<i<<"?"<<MEM->free_table[i].length<<"\n" ;
				while(1);
			}
			if(MEM->free_table[i].flag != 0)
			{
				//cout<<"У�����8 "<<i<<"?"<<MEM->free_table[i].flag<<"\n" ;
				while(1);
			}	
		}

		if(temp_num > MEM->endtablep)
		{
			if(MEM->free_table[temp_num].flag != 0)
			{
				//cout<<"У�����9 "<<i<<"?"<<MEM->free_table[i].flag<<"\n" ;
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
	
		if(size < MEM->free_table[i].length)					//�ж������ֽ�С�ڿ��ÿռ��ֽ�
		{
			n = i+1;  //������һ�����з�����
			
			if(n < MAX_BLOCK)
			{	
				if(MEM->endtablep+1==i)  //���ڴ����
				{
					s=n;		
				
					temp_addr = MEM->free_table[i].address;
					temp_size =  MEM->free_table[i].length;
						
					MEM->free_table[s].address = (void *)((MEM_ADD)temp_addr+size);
					MEM->free_table[s].length = temp_size - size;
					
				
					MEM->free_table[i].length = size;		
					MEM->free_table[i].flag  = 1;	
					MEM->endtablep = i;							//��¼��ָ��λ��	

					return (void*)(MEM->free_table[i].address);					//���ص�ַ				
				}//�±�Ϊ��ǰ�ͷŵ��ڴ�����
				else if(MEM->endtablep == END_BLOCK)//��ǰ��¼��Ϊĩ�˱��޷������ƶ�һ��������
				{					
					MEM->free_table[i].flag = 1;	
		
					return (void*)(MEM->free_table[i].address);					//���ص�ַ
				}
				else if((MEM->endtablep+1) == END_BLOCK) //ĩ�˱�Ϊ�ս�ֹ�����ƶ�һ��������(��ֹ�ڴ����)
				{					
					MEM->free_table[i].flag = 1;	
		
					return (void*)(MEM->free_table[i].address);					//���ص�ַ
				}
				else
				{													
					m = MEM->endtablep+1;							
					
					while(m >= n)
					{
						MEM->free_table[m+1].address = MEM->free_table[m].address;	//����ַ�����ƶ�һ��������

						MEM->free_table[m+1].length = MEM->free_table[m].length;		//�����ݳ����ƶ�һ��������

						MEM->free_table[m+1].flag  = MEM->free_table[m].flag ;		  //����־�ƶ�һ��������
						
						m--;
					}			
					temp_size = MEM->free_table[i].length;

					MEM->free_table[i].length = size;
					MEM->free_table[i].flag = 1;	
				
					MEM->free_table[n].address = (void*)((MEM_ADD)MEM->free_table[i].address + MEM->free_table[i].length);
					MEM->free_table[n].length = temp_size - size;
					MEM->free_table[n].flag = 0;
					MEM->endtablep += 1;		//��¼��ָ��λ��			

					return (void*)(MEM->free_table[i].address);					//���ص�ַ
				}	
			}	
			else
			{		
				 //��ǰ��¼��Ϊĩ�˱�			
				MEM->free_table[i].flag = 1;
				MEM->endtablep = END_BLOCK;		//��¼������ָ��λ��						
		
				////cout<<"�ѵ���¼��ĩ��"<<n<<"\n";
				return (void*)(MEM->free_table[i].address);					//���ص�ַ							
			}						
		}
		else  //��ǰ�ռ�������ռ����
		{	
			MEM->free_table[i].flag  = 1;	
			
			if(MEM->endtablep < i){MEM->endtablep = i;}	//��¼��ָ��λ��	

			return (void*)(MEM->free_table[i].address);					//���ص�ַ			
		}									
}

/*******************************************************************************
  * �������ƣ�mycalloc
  * ����˵�����ڴ����.        
  * ���������sizeҪ�����ֽڴ�С.
  * �������أ����ط����ڴ��׵�ַ.
*******************************************************************************/
void *mymalloc(MEM_UINT size)
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

			if(temp_size <= MEM->free_table[i].length)						//�жϵ�ǰ�ռ�
			{		
					
				addr=mymallocrun(i,temp_size);

				#ifdef MEMCRC
				memcrc(); //�ڴ�CRCУ��
				#endif

				return (void*)addr;
			}
			else
			{ 
				continue;					/*��������ѭ��*/
			}	
		}
	}

	if(i >= MAX_BLOCK)
	{
		//cout<<"�����鷶Χ "<<i<<"\n";
		while(1);
	}
	return (void*)0;		
}


void myfreerunLeftRight(MEM_UINT i)
{
	 MEM_UINT m = 0,n =0,s=0;	
	
	 n = i;
		
	 MEM->free_table[i-1].length += MEM->free_table[n].length;
	 MEM->free_table[i-1].length += MEM->free_table[n+1].length;
		
		if(i == MEM->endtablep)  //�ͷ�ĩ�˼�¼��		
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
				} //��������������Χ
				else
				{
					MEM->free_table[n].address = MEM->free_table[s].address;	//����ַ��ǰ�ƶ�һ��������
					MEM->free_table[n].length = MEM->free_table[s].length;		//�����ݳ����ƶ�һ��������
					MEM->free_table[n].flag  = MEM->free_table[s].flag ;		//����־�ƶ�һ��������	
				}					
				n++;
			}
			if(m < MAX_BLOCK) //�����ƶ������������
			{
				MEM->free_table[m].address = 0;	
				MEM->free_table[m].length = 0;		
			}
		}
		MEM->endtablep -= 2;	//��¼��ָ��λ����ǰ�ƶ�2������		
}

void myfreerunLeft(MEM_UINT i)
{
	 MEM_UINT m = 0,n =0,s=0;	
	
	  n = i;
	
		MEM->free_table[i-1].length += MEM->free_table[n].length;
		
		if(i == MEM->endtablep)  //�ͷ�ĩ�˼�¼��		
		{				
			 	MEM->free_table[n].address = 0;	
				MEM->free_table[n].length = 0;
			  MEM->free_table[n].flag  = 0;
        MEM->endtablep -= 2;	//��¼��ָ��λ����ǰ�ƶ�2������
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
				} //��������������Χ
				else
				{
					MEM->free_table[n].address = MEM->free_table[s].address;	//����ַ��ǰ�ƶ�һ��������
					MEM->free_table[n].length = MEM->free_table[s].length;		//�����ݳ����ƶ�һ��������
					MEM->free_table[n].flag  = MEM->free_table[s].flag ;		//����־�ƶ�һ��������	
				}					
				n++;
			}
			if(m < MAX_BLOCK) //�����ƶ������������
			{
				MEM->free_table[m].address = 0;	
				MEM->free_table[m].length = 0;		
			}
			MEM->endtablep -= 1;	//��¼��ָ��λ����ǰ�ƶ�1������
		}	
}

void myfreerunRight(MEM_UINT i)
{
	 MEM_UINT m = 0,n =0,s=0;	
	
	  n = i+1;
			
		MEM->free_table[i].length += MEM->free_table[n].length;
	  MEM->free_table[i].flag  = 0;
	
	  if(i == MEM->endtablep)  //�ͷ�ĩ�˼�¼��		
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
				} //��������������Χ
				else
				{
					MEM->free_table[n].address = MEM->free_table[s].address;	//����ַ��ǰ�ƶ�һ��������
					MEM->free_table[n].length = MEM->free_table[s].length;		//�����ݳ����ƶ�һ��������
					MEM->free_table[n].flag  = MEM->free_table[s].flag ;		  //����־�ƶ�һ��������	
				}	
				n++;
			}	
      if(m < MAX_BLOCK) //�����ƶ������������
			{
				MEM->free_table[m].address = 0;	
				MEM->free_table[m].length = 0;		
			}
		}
		MEM->endtablep -= 1;	//��¼��ָ��λ����ǰ�ƶ�1������
}

/*******************************************************************************
  * �������ƣ�myfree
  * ����˵�����ڴ��ͷ�.        
  * ���������ptrҪ�ͷŵĵ�ַ.
  * �������أ���.
*******************************************************************************/
void myfree(void *ptr)
{
	MEM_UINT i = 0;
	
	if(ptr == 0)
	{
		//cout<<"ָ����� "<<(MEM_UINT )(addr)<<"\n" ;
		while(1);	
	}
	
	while(i<MAX_BLOCK)
	{
		if(MEM->free_table[i].address == ptr)
		{				
			if((i==0)&&(MEM->free_table[i+1].flag == 0))
			{
				myfreerunRight(i);               //�ұ�
			}
			else if((i==END_BLOCK)&&(MEM->free_table[i-1].flag == 0))
			{
				myfreerunLeft(i);								//���
			}
			else if((MEM->free_table[i-1].flag == 0)&&(MEM->free_table[i+1].flag == 0))//����ұ�
			{
				myfreerunLeftRight(i);
			}	
			else if(MEM->free_table[i-1].flag == 0)//���
			{
				myfreerunLeft(i);
			}
			else if(MEM->free_table[i+1].flag == 0)//�ұ�
			{
				myfreerunRight(i);	
			}
			else
			{
				MEM->free_table[i].flag  = 0 ;
				
				//ĩ�˼�¼���¼��ָ��λ��	
				if(i== MEM->endtablep){MEM->endtablep -= 1;}
			}
			
			#ifdef MEMCRC
			memcrc(); //�ڴ�CRCУ��
			#endif

			break;
		}
		i++;
	}
    if(i>=MAX_BLOCK)
	{
		//cout<<"�Ҳ����ͷŵĵ�ַ���ַ���� "<<(MEM_UINT )(addr)<<"\n" ;
		while(1);	
	}
}

/*******************************************************************************
  * �������ƣ�myrealloc
  * ����˵�����ڴ����2(��ptrָ�뿽�������ڴ����ͷ�ptr�ڴ��).        
  * ���������nbytes.Ҫ�����µ��ڴ���С.
  * �������أ������ѷ����µ��ڴ��׵�ַ.
*******************************************************************************/
void *myrealloc(void *ptr, MEM_UINT nbytes)
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
				dst = mymalloc(nbytes);			//�ڴ����(�ⲿ����)				
				memcpy(dst, ptr, length );				
				myfree(ptr);  					//�ڴ��ͷ�(�ⲿ����)				
				return (void*)dst;				
			}
			else
			{
				//cout<<"�ռ䲻ƥ�� "<<(MEM_UINT )(addr)<<"\n" ;
				while(1);
				//return (void*)0;
			}
		}
		i++;
	}		
	if(i>=MAX_BLOCK)
	{
		//cout<<"�Ҳ����ͷŵĵ�ַ���ַ���� "<<(MEM_UINT )(addr)<<"\n" ;
		while(1);	
	}

	return (void*)0;
}

/*******************************************************************************
  * �������ƣ�mycalloc
  * ����˵�����ڴ����.        
  * ���������count�����ڴ浥Ԫ.
  * ���������size��Ԫ�ֽ���.
  * �������أ����ط����׵�Ԫ�ڴ��ַ.
*******************************************************************************/
void *mycalloc(MEM_UINT count, MEM_UINT size)
{	
	void *dst=0;
	
	dst=mymalloc(count*size);
	
	memset(dst, 0, count*size);
	
	return (void*)dst;		//�ڴ����(�ⲿ����)
}


#ifdef MEMCRC
/*******************************************************************************
  * �������ƣ�Getmemtablestatus
  * ����˵������ȡ�ڴ�״̬.        
  * �����������.
  * �������أ���.
*******************************************************************************/
void Getmemtablestatus(void)
{
	MEM_UINT m=0,j=0;
	
	
	 memcrc(); //�ڴ�CRCУ��
	printf("*****************У�����*******************\r\n");
	
		while(m < MAX_BLOCK)
		{
			if(MEM->free_table[m].flag == 1)
			{
				j=0;
				printf("*****************���ÿ���Ϣ*******************\r\n");
				printf("MEM->free_table[m].address=0x%x\r\n",MEM->free_table[m].address);
				printf("MEM->free_table[m].length=%d\r\n",MEM->free_table[m].length);
				printf("MEM->free_table[m].flag=%d\r\n",MEM->free_table[m].flag);
				printf("m=%d\r\n",m);
			}
			else
			{
				printf("*****************�м�δ�ÿ���Ϣ*******************\r\n");
				printf("MEM->free_table[m].address=0x%x\r\n",MEM->free_table[m].address);
				printf("MEM->free_table[m].length=%d\r\n",MEM->free_table[m].length);
				printf("MEM->free_table[m].flag=%d\r\n",MEM->free_table[m].flag);
				printf("m=%d\r\n",m);
				j++;
			}

			if((j >= 2)||(m == (END_BLOCK)))	//�ҵ�ĩ��δ�ÿ�
			{
				if(j >= 2){m--;}
				printf("*****************ĩ�˿���Ϣ*******************\r\n");
				printf("MEM->free_table[m].address=0x%x\r\n",MEM->free_table[m].address);
				printf("MEM->free_table[m].length=%d\r\n",MEM->free_table[m].length);
				printf("MEM->free_table[m].flag=%d\r\n",MEM->free_table[m].flag);
				printf("m=%d\r\n",m);
																														
				break;
			}	
			m++;
		}
}
#endif
