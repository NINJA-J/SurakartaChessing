// Eveluation.cpp: implementation of the CEveluation class zsj.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Surakarta.h"
#include "Eveluation.h"
#include<cstdlib>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////6
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEveluation::CEveluation()
{

}

CEveluation::~CEveluation()
{

}
int CEveluation::Eveluate(BYTE position[6][6], BOOL IsBlackturn)
{

	//����Ҫ�������������������¹��ܵĴ��룺
	//1.����֪�������ϵз����ҷ����ӵ�����
	//2.���Եõ�˫����ռλ�õķ�ֵ��ÿ��λ�ö�Ӧ�ķ�ֵ�ɲο��������
	//3.���Եõ��ҷ����ӿ��ƶ���ֵ
	//4.������һЩ������ֵ����

	 BlackValue=0,RedValue=0;//��ڷ��ܵ�����ֵ
	 BProtectValue=0,RProtectValue=0; 
	 BAttactValue=0,RAttactValue=0;
	 RMoveValue=0,BMoveValue=0;//������ʾ���ƶ���ֵ
	 BPosValue=0,RPosValue=0;
	 BNum=0,RNum=0;//�췽���ڷ���������
	 RArcValue=0, BArcValue=0;//ռ��ֵ
	 int i,j;
	 int BigPosition[8][8];
	 memset(BigPosition, -1, sizeof(BigPosition));
	 for (i = 1; i < 7; i++)
	 {
		 for (j = 1; j < 7; j++)
		 {
			 BigPosition[i][j]= position[i-1][j-1];
		 }
	 }
	
	GetAttackInfo(position);

	/*
	if (IsBlackturn) //��ù���������ֵ����������������������������������������������������������������
	{
		for (i=0;i<6;i++)
		{
			for(j=0;j<6;j++)
			{
				if(position[i][j]==2)                           
				{
					if (m_AttackPos[i][j]-m_ProtectPos[i][j]>0) 
						BAttactValue=BAttactValue+1;          			
				}
				if(position[i][j]==1)                         
				{
					if(m_ProtectPos[i][j]-m_AttackPos[i][j]<0)   
						RAttactValue=RAttactValue+1;             
				}
				if(RAttactValue==BAttactValue && RAttactValue) //��Ϊ���ںڷ��غϣ������ڷ���������������ͬ����ڷ�������  
					RAttactValue=RAttactValue-1;            
			}   
	
		}
	}

	else //����
  {
		for (i=0;i<6;i++)
		{
			for (j=0;j<6;j++)
			{
				if (position[i][j]==1)
				{
					if (m_AttackPos[i][j]-m_ProtectPos[i][j]>0) 
						RAttactValue=RAttactValue+1;  
				}
				if (position[i][j]==2)
				{
					if(m_ProtectPos[i][j]-m_AttackPos[i][j]<0)
						BAttactValue=BAttactValue+1; 
				}
				
				if(RAttactValue==BAttactValue && RAttactValue)
					BAttactValue=BAttactValue-1;
			}
		}
	}
	*/
//��������������������������������������������������������������������������������������������������������������������������������	

	if (IsBlackturn)
	{
		BArcValue = GetArcValue(position, IsBlackturn);
		RArcValue = GetArcValue(position, 0);
		for (i = 0; i < 6; i++) {
			for (j = 0; j < 6; j++) {
				if (position[i][j] == 1) {
					BNum++;
					BAttactValue += m_AttackPos[i][j];
					BPosValue += PosScore[i][j];
					if (BigPosition[i][j+1] == 0)
						BMoveValue++;
					if (BigPosition[i+2][j+1] == 0)
						BMoveValue++;
					if (BigPosition[i+1][j] == 0)
						BMoveValue++;
					if (BigPosition[i+1][j+2] == 0)
						BMoveValue++;
					if (BigPosition[i][j] == 0)
						BMoveValue++;
					if (BigPosition[i][j+2] == 0)
						BMoveValue++;
					if (BigPosition[i+2][j+2] == 0)
						BMoveValue++;
					if (BigPosition[i+2][j] == 0)
						BMoveValue++;
					
				}
				if (position[i][j] == 2) {
					RNum++;
					RAttactValue += m_AttackPos[i][j];
					RPosValue += PosScore[i][j];
					if (BigPosition[i][j + 1] == 0)
						RMoveValue++;
					if (BigPosition[i + 2][j + 1] == 0)
						RMoveValue++;
					if (BigPosition[i + 1][j] == 0)
						RMoveValue++;
					if (BigPosition[i + 1][j + 2] == 0)
						RMoveValue++;
					if (BigPosition[i][j] == 0)
						RMoveValue++;
					if (BigPosition[i][j + 2] == 0)
						RMoveValue++;
					if (BigPosition[i + 2][j + 2] == 0)
						RMoveValue++;
					if (BigPosition[i + 2][j] == 0)
						RMoveValue++;
				}

			}
		}
	}
	else {
		RArcValue = GetArcValue(position, IsBlackturn);
		RArcValue = GetArcValue(position, 1);
		for (i = 0; i < 6; i++) {
			for (j = 0; j < 6; j++) {
				if (position[i][j] == 1) {
					BNum++;
					BAttactValue += m_AttackPos[i][j];
					BPosValue += PosScore[i][j];
					if (BigPosition[i][j + 1] == 0)
						BMoveValue++;
					if (BigPosition[i + 2][j + 1] == 0)
						BMoveValue++;
					if (BigPosition[i + 1][j] == 0)
						BMoveValue++;
					if (BigPosition[i + 1][j + 2] == 0)
						BMoveValue++;
					if (BigPosition[i][j] == 0)
						BMoveValue++;
					if (BigPosition[i][j + 2] == 0)
						BMoveValue++;
					if (BigPosition[i + 2][j + 2] == 0)
						BMoveValue++;
					if (BigPosition[i + 2][j] == 0)
						BMoveValue++;

				}
				if (position[i][j] == 2) {
					RNum++;
					RAttactValue += m_AttackPos[i][j];
					RPosValue += PosScore[i][j];
					if (BigPosition[i][j + 1] == 0)
						RMoveValue++;
					if (BigPosition[i + 2][j + 1] == 0)
						RMoveValue++;
					if (BigPosition[i + 1][j] == 0)
						RMoveValue++;
					if (BigPosition[i + 1][j + 2] == 0)
						RMoveValue++;
					if (BigPosition[i][j] == 0)
						RMoveValue++;
					if (BigPosition[i][j + 2] == 0)
						RMoveValue++;
					if (BigPosition[i + 2][j + 2] == 0)
						RMoveValue++;
					if (BigPosition[i + 2][j] == 0)
						RMoveValue++;
				}
			}
		}
	}
	BlackValue = BNum * 6 + BMoveValue * 1 + BAttactValue * 2 + BPosValue * 1 + BArcValue * 1;//abcd�ֱ��ʾ�����������ص�Ȩֵ
	RedValue = RNum * 6 + RMoveValue * 1 + RAttactValue * 2 + RPosValue * 1 + RArcValue * 1;



	if (IsBlackturn)//�˽⼫��С����ԭ�����IsBlackturn ��������Ǹ�ʲô�õĺ�����ֵ�����壬Ȼ��ȷ��return�����ĸ�����
		return BlackValue-RedValue;//���ص���һ���������ֻ��Ϊ���ó���������
	else
		return RedValue-BlackValue;
}




void CEveluation::GetAttackInfo(BYTE position[6][6])
{
	BYTE CurPosition[6][6];
	memset(m_AttackPos,0,36);
	memset(m_ProtectPos,0,36);
	memset(m_MovePos,0,36);
    memcpy(CurPosition,position,36);
	int flag=1,i,j,m,n;
	for (i=0;i<6;i++)
	{
		for (j=0;j<6;j++)
		{
			if(CurPosition[i][j])
			{
				for (m=0;m<6;m++)
				{
					for(n=0;n<6;n++)
					{
						if(i==m && j==n)
						{
							continue;
						}
						if(CurPosition[m][n]!=CurPosition[i][j])
						{
							if(  CurPosition[m][n] && m_pMg->IsValidMove(CurPosition,m,n,i,j)) 
							{
								m_AttackPos[i][j]++;
							}

						}
						else
						{

							if(CurPosition[m][n])
							{
								CurPosition[m][n]=3-CurPosition[i][j];
								if(m_pMg->IsValidMove(CurPosition,m,n,i,j))  
								{

									m_ProtectPos[i][j]++;           

								}
								CurPosition[m][n]=CurPosition[i][j];
							}

						}
						
					}
				}
			}
		}
	}
	memcpy(position,CurPosition,36);
}

int CEveluation::GetArcValue(BYTE position[6][6], BOOL IsBlackturn)
{
	int BArcNum=0, RArcNum=0, NoArcNum=0;
	int i;
	for (i = 1; i < 5; i++) {
		if (position[i][0] == 1)
		{
			BArcNum++;
		}
		else if (position[i][0] == 2)
		{
			RArcNum++;
		}
		else {
			NoArcNum++;
		}
	}
	for (i = 1; i < 5; i++) {
		if (position[0][i] == 1)
		{
			BArcNum++;
		}
		else if (position[0][i] == 2)
		{
			RArcNum++;
		}
		else {
			NoArcNum++;
		}
	}
	for (i = 1; i < 5; i++) {
		if (position[i][5] == 1)
		{
			BArcNum++;
		}
		else if (position[i][5] == 2)
		{
			RArcNum++;
		}
		else {
			NoArcNum++;
		}
	}
	for (i = 1; i < 5; i++) {
		if (position[5][i] == 1)
		{
			BArcNum++;
		}
		else if (position[5][i] == 2)
		{
			RArcNum++;
		}
		else {
			NoArcNum++;
		}
	}
	if (IsBlackturn) {
		return BArcNum * 5 + NoArcNum * 5;
	}
	else {
		return RArcNum * 5 + NoArcNum * 5;
	}
}







