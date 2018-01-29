// NegaScout.h: interface for the CNegaScout class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEGASCOUT_H__F7857B67_4C28_429B_8DE7_2F6F7CBE3572__INCLUDED_)
#define AFX_NEGASCOUT_H__F7857B67_4C28_429B_8DE7_2F6F7CBE3572__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include<unordered_map>
#include "SearchEngine.h"

using namespace std;
//#define ABDebug
class CNegaScout : 
	public CSearchEngine 
{
public:
	CNegaScout();
	virtual ~CNegaScout();
	virtual CHESSMOVE SearchAGoodMove(BYTE position[6][6],int m_isPlayerBlack);
protected:
	int NegaScout_TT_HH(int depth,int num,int m_Type);
	int NegaScout_ABTree(int depth, int m_Type, int alpha = 0, int beta = 0);
	int GetValueByKey(BYTE position[6][6],int m_isPlayerBlack);
    CHESSMOVE m_bsetMove[2];
//	int top;
	int flag1;
	int flag2;
	long ChessValue[6][6][3];
	long long PowerNum[36] = { 50031545098999707, 16677181699666569,5559060566555523,1853020188851841,617673396283947,205891132094649,
		68630377364883 ,22876792454961 ,7625597484987 ,2541865828329 ,847288609443 ,282429536481 ,94143178827 ,31381059609 ,10460353203 ,
		3486784401 ,1162261467 ,387420489 ,129140163 ,43046721 ,14348907 ,4782969 ,1594323 ,531441 ,177147 ,59049 ,19683 ,6561 ,2187 ,
		729 ,243,81,27,9,3,1};
	unordered_map<long long, int> unorderedMap;
};

#endif // !defined(AFX_NEGASCOUT_H__F7857B67_4C28_429B_8DE7_2F6F7CBE3572__INCLUDED_)
