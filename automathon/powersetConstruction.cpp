/*Code by AlejandroSPosada
NFA to DFA with alphabet a and b

-- PROBLEMS
HACKERRANK:
	1) 006. Counting Strings (also: Thompson's construction, matrix power)*/

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

int constructDFA(char* regex, int L){
	n = 0;
	memset(g,0,sizeof g);
	memset(out,255,sizeof out);
	P nfa = construct(regex,0,strlen(regex) - 1);

	// reflexive 
	for(int i = 0;i < n;i++) 
		g[i][i] = 1;

	// closure relation
	for(int k = 0;k < n;k++)
	for(int i = 0;i < n;i++)
	for(int j = 0;j < n;j++)
	if(g[i][k] && g[k][j])
		g[i][j] = 1;

	memset(ga,0,sizeof ga);
	memset(gb,0,sizeof gb);

	for(int i = 0;i < MAXN;i++) aMask[i].clear(), bMask[i].clear();

	/* if is possible from state i to the start k and is possible 
	from end k to state j and out k is a or b (!= -1), 
	1) we saved aMask[i].insert[j], which tell us that j is a reachable state with a from i with a
	2) we saved bMask[i]... */
	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			for(int k = 0; k + 1 < n; k++){
				if(out[k] != -1 && g[i][k] && g[k + 1][j]){
					if(out[k] == 0) aMask[i].insert(j);
					else bMask[i].insert(j);
				}
			}
		}
	}

	memset(g2,0,sizeof g2);
	int id = 0;
	set<int> q[MAXN];
	int sz = 0;
	map<set<int>,int> mp;
	set<int> start;

	start.insert(nfa.first);
	q[sz] = start;
	mp[start] = sz++;

	for(int i = 0; i < sz; i++){
		set<int> k = q[i];
		set<int> a,b;
		// we have the set of states k
		// let's get elements of aMask/bMask for each element z, and insert all them
		// in a/b
		for(set<int> :: iterator z = k.begin(); z != k.end();++z){
			for(set<int> :: iterator it = aMask[*z].begin();it != aMask[*z].end();++it) 
				a.insert(*it);
			for(set<int> :: iterator it = bMask[*z].begin();it != bMask[*z].end();++it) 
				b.insert(*it);
		}

		if(!mp.count(a)){
			q[sz] = a;
			mp[a] = sz++;
		}
		g2[i][mp[a]]++;		
		if(!mp.count(b)){
			q[sz] = b;
			mp[b] = sz++;
		}
		g2[i][mp[b]]++;
	}
	return 0;
}

int main(){
	int runs ;
	scanf("%d",&runs) ;
	for(int t = 1;t <= runs;t++){
		int L;
		char regex[MAXN];
		scanf("%s%d",regex,&L);
		int ret = solve(regex,L);
		printf("%d\n",ret);
	}
 	return 0 ;
}
