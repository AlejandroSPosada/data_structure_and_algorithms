/*Code by AlejandroSPosada
Input format:
((((ab)|a)*)|(((aa)|(bb))*)) 
Only two elements, where * is considered as one*/

#include<stdio.h>
#include<vector>
#include<set>
#include<stdlib.h>
#include<map>
#include<string.h>
#include<iostream>
using namespace std;
typedef pair<int,int> P;
#define MOD 1000000007
#define MAXN 102
int n,out[MAXN],g[MAXN][MAXN];
int ga[MAXN][MAXN],gb[MAXN][MAXN];
int g2[MAXN][MAXN];
set<int> aMask[MAXN],bMask[MAXN];

// McNaughton-Yamada-Thompson algorithm
P construct(char regex[],int low,int high){
	int start = n++;
	int end = n++;
	if(low == high){
		out[start] = regex[low] - 'a';
		return P(start,end);
 	}
	int mid,ct = 0;
	for(mid = low + 1;	mid <= high;	mid++){
		if(regex[mid] == '(') ct++;
		if(regex[mid] == ')') ct--;
		if(ct == 0) break;
 	}
	P ret1 = construct(regex,low + 1,mid);
	if(regex[mid + 1] == '|'){
		P ret2 = construct(regex,mid + 2,high - 1);
		g[start][ret1.first] = g[start][ret2.first] = 1;
		g[ret1.second][end] = g[ret2.second][end] = 1;
 	}
 	else if(regex[mid + 1] == '*'){
		g[start][ret1.first] = g[start][end] = 1;
		g[ret1.second][end] = g[end][start] = 1;
 	}
 	else{
		P ret2 = construct(regex,mid + 1,high - 1);
		g[start][ret1.first] = g[ret1.second][ret2.first] = 1;
		g[ret2.second][end] = 1;
 	}
 	return P(start,end);
}

int main(){
	n = 0;
	memset(g,0,sizeof g);
	memset(out,255,sizeof out);
	P nfa = construct(regex,0,strlen(regex) - 1);
 	return 0 ;
}

/*PROBLEMS
HACKERRANK:
	1) 006. Counting Strings (also: power set construction, matrix power)*/
