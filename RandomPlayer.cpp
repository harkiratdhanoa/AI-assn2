#include <bits/stdc++.h>
#include <iostream>
#include <vector>
#include <string>
#define CORDINATE pair<int, int>
#define SIZE_R 8
#define SIZE_C 8

///////////GOALS FOR ALL NEXT STATES RETURN///////////
//1. write function action(state1, state2)
//2. Use auto& c:s in for loops
//3. Use all possible objects/variable in reference
//4. Cannon moves
//5. Cannon shots
//6. Implement giveAllChildsofMin()

using namespace std;

enum Orient { None, Vertical, Secondary, Horizontal, Primary };

enum Neighbour {U,UR,R,DR,D,DL,L,UL};

CORDINATE myNeighbours[8] = {make_pair(-1,0), make_pair(-1,1), make_pair(0,1), make_pair(2,2), make_pair(2,0), make_pair(2,-2), make_pair(0,-1), make_pair(-1,-1) };
CORDINATE enemyNeighbours[8] = {make_pair(1,0), make_pair(1,-1), make_pair(0,-1), make_pair(-2,-2), make_pair(-2,0), make_pair(-2,2), make_pair(0,1), make_pair(1,1) };

int min(int a, int b){
	return a<b?a:b;
}
int max(int a, int b){
	return a>b?a:b;
}

int minFromIntVectors(vector<int> vec){
	int min = INT_MAX;
	for(auto c:vec){
		if(min>c){min = c;}
	}
	return min;
}

int maxFromIntVectors(vector<int> vec){
	int max = INT_MIN;
	for(auto c:vec){
		if(max<c){max = c;}
	}
	return max;
}

vector<vector<int> > giveInitialBoard(){
	vector<vector<int> > res(SIZE_R, vector<int>(SIZE_C, 0));
	for(int i = 0;i<SIZE_R;i++){
		for(int j=0;j<SIZE_C;j++){
			if(i==0 && j%2==0){res[i][j]=-2;} //enemy townhall is having black/negative label/id
			else if(i==SIZE_R-1 && j%2==1){res[i][j]=2;}//my townhall
			else if(i<3 && j%2==1){res[i][j]=-1;}//enemy soldiers
			else if (i>SIZE_R-4 && j%2==0){res[i][j]=1;}//my soldiers
		}
	}
	return res;
}

void printBoard(vector<vector<int> > board){
	int r = board.size();
	int c = board[0].size();
	for(int i=0;i<r;i++){
		for(int j=0;j<c;j++){
			if(board[i][j]>=0)
				cout<<" ";
			if(!board[i][j])
				cout<<". ";
			else
			cout<<board[i][j]<<" ";
		}
		cout<<"\n";
	}
}

void displayUniqueSolier(vector<CORDINATE> soldiers){
	int length = soldiers.size();
	for(int i=0;i<length;i++){
		cout<<"Soldier/Townhall: "<<soldiers[i].second<<" -> {"<<soldiers[i].first<<", "<<soldiers[i].second<<"}"<<"\n";
	}
}



class State {
public:
	// int blackOrWhite; //-1 for black 1 for white
	vector<vector<int> > board; // 0 for empty | -2 for black castle | -1 for black circle | 1 for white circle | 2 for white castle
	vector<CORDINATE> mySoldiers;
	vector<pair<CORDINATE, Orient > > myCannons;
	vector<CORDINATE> enemySoldiers;
	vector<pair<CORDINATE, Orient > > enemyCannons;
	vector<CORDINATE> myTownhalls;
	vector<CORDINATE> enemyTownhalls;
	string action;
	int backedupVal;
	State(){
		board = giveInitialBoard();
		action = "";
		//initialized other vectors
		for(int i=0;i<4;i++){//row
			for(int j=0;j<7;j+=2){//coloumm
				if(i!=3) {
					mySoldiers.push_back(make_pair(board.size()-i-1,j));
					enemySoldiers.push_back(make_pair(i, j+1));
				}
				if(i==2) {
					myCannons.push_back(make_pair(make_pair(board.size()-i-1,j), Vertical));
					enemyCannons.push_back(make_pair(make_pair(i, j+1), Vertical));
				}

			}
			myTownhalls.push_back(make_pair(board.size()-1, 2*i+1)); // T1 at (r-1,1) | T2 at (r-1,3);
			enemyTownhalls.push_back(make_pair(0, 2*i));

		}
		backedupVal=INT_MIN;
	}

	bool isEnemyHere(int x, int y){
		if(!isPositionValid(x,y))
			return false;
		if(board[x][y]<0) return true;
		return false;
	}

	bool isAllyHere(int x, int y){
		if(!isPositionValid(x,y))
			return false;
		if(board[x][y]>0) return true;
		return false;
	}

	bool isPositionValid(int i, int j){
		if(i<SIZE_R && i>=0 && j>=0 && j<SIZE_C){return true;}
		return false;
	}

	bool isUnderAttack(bool enemy, int x, int y){
		if(!isPositionValid(x,y))
			return false;
		if(enemy)
			for(auto v: enemySoldiers){
				int x_enemy = v.first;
				int y_enemy = v.second;
				for(int i=-1;i<2;i++){
					for(int j=-1;j<2;j++){
						if(x==x_enemy-i && y==y_enemy-j){return true;}
					}
				}
			}
		else
			for(auto v: mySoldiers){
			int x_enemy = v.first;
			int y_enemy = v.second;
			for(int i=-1;i<2;i++){
				for(int j=-1;j<2;j++){
					if(x==x_enemy-i && y==y_enemy-j){return true;}
				}
			}
		}
		return false;
	}

	void kill(int x, int y){
		for(int i=0;i<enemySoldiers.size();i++){
			if(enemySoldiers[i].first ==x && enemySoldiers[i].second == y){
				enemySoldiers.erase(enemySoldiers.begin()+i);
				break;
			}
			else if(i< enemyTownhalls.size() && enemyTownhalls[i].first ==x && enemyTownhalls[i].second == y){
				enemyTownhalls.erase(enemyTownhalls.begin()+i);
				break;
			}
		}
	}

	void killOwn(int x, int y){
		for(int i=0;i<mySoldiers.size();i++){
			if(mySoldiers[i].first ==x && mySoldiers[i].second == y){
				mySoldiers.erase(mySoldiers.begin()+i);
				break;
			}
			else if(i< myTownhalls.size() && myTownhalls[i].first ==x && myTownhalls[i].second == y){
				myTownhalls.erase(myTownhalls.begin()+i);
				break;
			}
		}
	}

	void updateCannons(){
		myCannons.clear();
		for(auto sold_pos:mySoldiers){
			if(sold_pos.first<1 || sold_pos.second<1 || sold_pos.first>SIZE_R-2 || sold_pos.second>SIZE_C-2)
				continue;
			if(board[sold_pos.first+1][sold_pos.second+1]==1 && board[sold_pos.first-1][sold_pos.second-1] == 1)
				myCannons.push_back(make_pair(sold_pos,Primary));
			if(board[sold_pos.first][sold_pos.second+1]==1 && board[sold_pos.first][sold_pos.second-1] == 1)
				myCannons.push_back(make_pair(sold_pos,Horizontal));
			if(board[sold_pos.first+1][sold_pos.second]==1 && board[sold_pos.first-1][sold_pos.second] == 1)
				myCannons.push_back(make_pair(sold_pos,Vertical));
			if(board[sold_pos.first+1][sold_pos.second-1]==1 && board[sold_pos.first-1][sold_pos.second+1] == 1)
				myCannons.push_back(make_pair(sold_pos,Secondary));
		}
		enemyCannons.clear();
		for(auto sold_pos:enemySoldiers){
			if(sold_pos.first<1 || sold_pos.second<1 || sold_pos.first>SIZE_R-1 || sold_pos.second>SIZE_C-1)
				continue;
			if(board[sold_pos.first+1][sold_pos.second+1]==1 && board[sold_pos.first-1][sold_pos.second-1] == 1)
				enemyCannons.push_back(make_pair(sold_pos,Primary));
			if(board[sold_pos.first][sold_pos.second+1]==1 && board[sold_pos.first][sold_pos.second-1] == 1)
				enemyCannons.push_back(make_pair(sold_pos,Horizontal));
			if(board[sold_pos.first+1][sold_pos.second]==1 && board[sold_pos.first-1][sold_pos.second] == 1)
				enemyCannons.push_back(make_pair(sold_pos,Vertical));
			if(board[sold_pos.first+1][sold_pos.second-1]==1 && board[sold_pos.first-1][sold_pos.second+1] == 1)
				enemyCannons.push_back(make_pair(sold_pos,Secondary));
		}
		return;
	}
	vector<State> giveAllChildsOfMin(){
		// cout<<"\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~welcome to child of MIN ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
		vector<State> res;
		State newstate;
		// printBoard(this->board);
		for(int j=0;j<12;j++){
			CORDINATE pos = this->enemySoldiers[j];
			// cout<<"\nsoldier:" <<j<<" at "<<pos.first<<","<<pos.second<<"\n\n";
			for(int i=0;i<8;i++){

				CORDINATE relative = enemyNeighbours[i];
				relative.first+=pos.first;
				relative.second+=pos.second;

				if((i==DL||i==D||i==DR) && !isUnderAttack(false, pos.first, pos.second)){ 	// BACKING OUT NOT ALLOWED
					// cout<<"CANT RETREAT!          ";
					continue;
				}
				if(isAllyHere(relative.first, relative.second)){ // GET KILLED (AT ANY OF 8)
					// cout<<"KILLING "<<relative.first<<","<<relative.second<<"!          ";
					newstate = *this;
					newstate.enemySoldiers[i] = relative;
					newstate.board[relative.first][relative.second]=-1;
					newstate.killOwn(relative.first, relative.second);
					newstate.board[pos.first][pos.second] = 0;
					newstate.updateCannons();
					newstate.action = "Enemy "+to_string(pos.first)+" "+to_string(pos.second)+" M "+to_string(relative.first)+" "+to_string(relative.second);
					// cout<<newstate.action<<"  !!! ";
					res.push_back(newstate);

				} 
				else if(isEnemyHere(relative.first, relative.second)==false && (i!=L) && (i!=R) && isPositionValid(relative.first,relative.second)){ // ENEMY MOVE (TO ITS UL,U,UR)
					newstate = *this;

					newstate.enemySoldiers[i] = relative;
					newstate.board[relative.first][relative.second]=-1;
					newstate.board[pos.first][pos.second] = 0;
					newstate.updateCannons();
					newstate.action = "Enemy "+to_string(pos.first)+" "+to_string(pos.second)+" M "+to_string(relative.first)+" "+to_string(relative.second);
					// cout<<newstate.action<<"!          ";
					res.push_back(newstate);
				}
				// else cout<<"MOVE INVALID!          ";
			}
		}
		// for(auto can:myCannons){
				
		// 	}
		return res;
	}

	vector<State> giveAllChildsOfMax(){
		// cout<<"\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~welcome to child of MAX~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
		vector<State> res;
		State newstate;
		// printBoard(this->board);
		for(int j=0;j<12;j++){
			CORDINATE pos = this->mySoldiers[j];
			// cout<<"\nsoldier:" <<j<<" at "<<pos.first<<","<<pos.second<<"\n\n";
			for(int i=0;i<8;i++){
				// cout<<"neighbour:" <<i<<"\n";

				CORDINATE relative = myNeighbours[i];
				relative.first+=pos.first;
				relative.second+=pos.second;
				
				if((i==DL||i==D||i==DR) && !isUnderAttack(false, pos.first, pos.second)){ // BACKING OUT NOT ALLOWED
					// cout<<"CAN'T RETREAT!          ";
					continue;
				}
				if(isEnemyHere(relative.first, relative.second)){ // KILL ENEMY (AT ANY OF 8)
					// cout<<"KILLING "<<relative.first<<","<<relative.second<<"!          ";

					newstate = *this;

					newstate.mySoldiers[i] = relative;
					newstate.board[relative.first][relative.second]=1;
					// printBoard(board);
					newstate.kill(relative.first, relative.second);
					// printBoard(board);
					newstate.board[pos.first][pos.second] = 0;
					newstate.updateCannons();
					newstate.action = "S "+to_string(pos.first)+" "+to_string(pos.second)+" M "+to_string(relative.first)+" "+to_string(relative.second);
					// cout<<newstate.action<<"  !!! ";
					res.push_back(newstate);

				} 
				else if(isAllyHere(relative.first, relative.second)==false && (i!=L) && (i!=R) && isPositionValid(relative.first,relative.second)){ // I MOVE (TO UL,U,UR)
					// cout<<"I MOVE (TO UL,U,UR) - ";
					newstate = *this;

					newstate.mySoldiers[i] = relative;
					newstate.board[relative.first][relative.second]=1;
					newstate.board[pos.first][pos.second] = 0;
					newstate.updateCannons();
					newstate.action = "S "+to_string(pos.first)+" "+to_string(pos.second)+" M "+to_string(relative.first)+" "+to_string(relative.second);
					// cout<<newstate.action<<"!          ";
					res.push_back(newstate);
				}
				// else cout<<"MOVE INVALID!          ";
			}
		}
		// for(auto can:myCannons){
		// 	if(can);
		// }
		return res;
	}
};

//FORWARD DECLARATION FOR MUTUAL RECURSION
int min_value(State& state, int alpha, int beta, int depth);
int max_value(State& state, int alpha, int beta, int depth);

int Eval(State state){
	int townhallDiff = state.myTownhalls.size()*2 - state.enemyTownhalls.size();
	return townhallDiff + state.mySoldiers.size()-state.enemySoldiers.size();
}

bool cutoff(State state, int depth){
	if(state.myTownhalls.size()<=2 || state.enemyTownhalls.size()<=2 || state.mySoldiers.size()==0 || state.enemySoldiers.size()==0 || depth>3){return true;}

	return false;
}

int max_value(State& state, int alpha, int beta, int depth){
	// cout<<"Yea\n";
	// cout<<"Depth on max: "<<depth<<"\n";
	if(cutoff(state,depth)){return Eval(state);}
	int v = INT_MIN;
	for(auto child:state.giveAllChildsOfMax()){
		v = max(v, min_value(child,alpha,beta,depth+1));
		if(v>=beta){state.backedupVal = v; return v;}
		alpha = max(alpha, v);
	}
	return v;
}

int min_value(State& state, int alpha, int beta, int depth){
	if(cutoff(state, depth)){return Eval(state);}
	int v = INT_MAX;
	for(auto child:state.giveAllChildsOfMin()){
		v = min(v, max_value(child, alpha, beta, depth+1));
		if(v<=alpha){return v;}
		beta = beta<v ? beta: v;
	}
	return v;
}

State action(State& s1, State& s2){
	return s2;
}

void alpha_beta_search(State& state){
	int d = 0;
	int v = max_value(state, INT_MIN, INT_MAX, d);

	cout<<"\n\n\n\n\nPRINTING VALUEE OF V\n\n\n\n\n\n";
	for(int i=0;i<state.giveAllChildsOfMax().size();i++){
		if(min_value((state.giveAllChildsOfMax())[i], INT_MIN, INT_MAX, d+1)==v){
			printBoard(action(state, (state.giveAllChildsOfMax())[i]).board);
			cout<<"\n";
			break;
		}
	}
}

int main(int argc, char const *argv[])
{
	cout<<"\n";
	// printBoard(giveInitialBoard());
	State startNode;
	startNode.board[2][1] = 0;
	startNode.board[4][1] = -1;
	for(int t = 0;t<startNode.enemySoldiers.size();t++){
		if(startNode.enemySoldiers[t].first==2 && startNode.enemySoldiers[t].second==1){
			startNode.enemySoldiers[t].first = 4;
			// startNode.enemySoldiers.push_back({{4,1}, startNode.enemySoldiers[t].second});
			// startNode.enemySoldiers.erase(startNode.enemySoldiers.begin() + t);
		}
	}
	
	printBoard(startNode.board);
	cout<<"\n";

	// cout<<isAllyHere(startNode.mySoldiers, startNode.myTownhalls, 6, 3);

	// displayUniqueSolier(startNode.myTownhalls);

	// GET ALL CHILDREN
	// vector<State> itsChildren = startNode.giveAllChildsOfMax();

	// State secondNode = itsChildren[2];
	// printBoard(secondNode.board);
	cout<<"\n";

	// itsChildren = secondNode.giveAllChildsOfMin();

	// for(int i=0;i<itsChildren.size();i++){
	// 	printBoard(itsChildren[i].board);
	// 	cout<<"\n";
	// }
	//
    
    time_t start, end;
    time(&start); 
    ios_base::sync_with_stdio(false);

    cout<<"\n\n\n\n\nSTART     \n\n\n\n\n\n\n\n\n\n\n";
	alpha_beta_search(startNode);
	cout<<"\n\n\n\n\n\nFINISH\n\n\n\n\n\n\n\n\n\n\n\n\n";
    time(&end);
    cout<<double(end - start)<<setprecision(5);  

	// cout<<itsChildren.size()<<"\n";
	return 0;
}