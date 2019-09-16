#include <bits/stdc++.h>
#include <iostream>
#include <vector>
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

bool isEnemyHere(vector<CORDINATE> enemySoldiers, vector<CORDINATE> enemyTownhalls, int x, int y){
	for(auto v: enemySoldiers){
		if(v.first==x && v.second==y){return true;}
	}
	for(auto v: enemyTownhalls){
		if(v.first==x && v.second==y){return true;}
	}
	return false;
}

bool isAllyHere(vector<CORDINATE> mySoldiers, vector<CORDINATE> myTownhalls, int x, int y){
	for(auto v: mySoldiers){
		if(v.first==x && v.second==y){return true;}
	}
	for(auto v: myTownhalls){
		if(v.first==x && v.second==y){return true;}
	}
	return false;
}

bool isPositionValid(int i, int j){
	if(i<SIZE_R && i>=0 && j>=0 && j<SIZE_C){return true;}
	return false;
}

bool isUnderAttack(vector<CORDINATE> enemySoldiers, int x, int y){
	for(auto v: enemySoldiers){
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

void update(vector<CORDINATE> & enemy, int x, int y){
	for(int i=0;i<enemy.size();i++){
		if(enemy[i].first ==x && enemy[i].second == y){
			enemy.erase(enemy.begin()+i);
			break;
		}
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

	State(){
		board = giveInitialBoard();

		//initialized other vectors
		for(int i=0;i<4;i++){//row
			for(int j=0;j<7;j+=2){//coloumm
				if(i!=3) mySoldiers.push_back(make_pair(board.size()-i-1,j));
				if(i!=3) enemySoldiers.push_back(make_pair(i, j+1));
			}
			myTownhalls.push_back(make_pair(board.size()-1, 2*i+1)); // T1 at (r-1,1) | T2 at (r-1,3);
			enemyTownhalls.push_back(make_pair(0, 2*i));
		}
	}

	vector<State> giveAllChildsOfMin(){
		vector<State> res;
		//cout<<"JEGS";
		State newstate = *this;
		for(int i=0;i<12;i++){
			
			CORDINATE pos = this->enemySoldiers[i];
			// cout<<"{"<<pos.first<<", "<<pos.second<<"}"<<"\n";

			if(isEnemyHere(this->mySoldiers, this->myTownhalls, pos.first+1, pos.second)){ // up				
				
				//duplicated this state
				newstate = *this;

				newstate.enemySoldiers[i].first = pos.first+1;
				newstate.enemySoldiers[i].second = pos.second;
				this->board[pos.first+1][pos.second]=-1;
				
				//Actually either of these two below will happen
				update(newstate.mySoldiers, pos.first+1, pos.second);
				update(newstate.myTownhalls, pos.first+1, pos.second);
				newstate.board[pos.first][pos.second] = 0;

				res.push_back(newstate);
			} else if(isAllyHere(this->enemySoldiers, this->enemyTownhalls, pos.first+1, pos.second)==false && isPositionValid(pos.first+1, pos.second)){
				// cout<<"{"<<pos.first<<", "<<pos.second<<"}"<<"\n";
				
				newstate = *this;

				newstate.enemySoldiers[i].first = pos.first+1;
				newstate.enemySoldiers[i].second = pos.second;


				newstate.board[pos.first+1][pos.second]=-1; //update board by moving my soldier here
				newstate.board[pos.first][pos.second] = 0;
				// printBoard(newstate.board);
				res.push_back(newstate);
			}

			if(isEnemyHere(this->mySoldiers, this->myTownhalls, pos.first+1, pos.second-1)){//up left daignaolly
				newstate = *this;

				newstate.enemySoldiers[i].first = pos.first+1;
				newstate.enemySoldiers[i].second = pos.second-1;
				
				newstate.board[pos.first+1][pos.second-1]=-1;
				newstate.board[pos.first][pos.second]=0;

				update(newstate.mySoldiers, pos.first+1, pos.second-1);
				update(newstate.myTownhalls, pos.first+1, pos.second-1);
				res.push_back(newstate);
			} else if(isAllyHere(this->enemySoldiers, this->enemyTownhalls, pos.first+1, pos.second-1)==false && isPositionValid(pos.first+1, pos.second-1)){
				newstate = *this;

				newstate.enemySoldiers[i].first = pos.first+1;
				newstate.enemySoldiers[i].second = pos.second-1;
				
				newstate.board[pos.first+1][pos.second-1]=-1;
				newstate.board[pos.first][pos.second]=0;
				res.push_back(newstate);
			}

			if(isEnemyHere(this->mySoldiers, this->myTownhalls, pos.first+1, pos.second+1)){//up right daignaolly
				newstate = *this;

				newstate.enemySoldiers[i].first = pos.first+1;
				newstate.enemySoldiers[i].second = pos.second+1;
				
				newstate.board[pos.first+1][pos.second+1]=-1;
				newstate.board[pos.first][pos.second]=0;

				update(newstate.mySoldiers, pos.first+1, pos.second+1);
				update(newstate.myTownhalls, pos.first+1, pos.second+1);
				res.push_back(newstate);
			} else if(isAllyHere(this->enemySoldiers, this->enemyTownhalls, pos.first+1, pos.second+1)==false && isPositionValid(pos.first+1, pos.second+1)){
				newstate = *this;

				newstate.enemySoldiers[i].first = pos.first+1;
				newstate.enemySoldiers[i].second = pos.second+1;
				
				newstate.board[pos.first+1][pos.second+1]=-1;
				newstate.board[pos.first][pos.second]=0;
				res.push_back(newstate);
			}

			//back right daignaolly
			if(isEnemyHere(this->mySoldiers, this->myTownhalls, pos.first-2, pos.second+2) && isUnderAttack(this->mySoldiers, pos.first-2, pos.second+2)){
				newstate = *this;

				newstate.enemySoldiers[i].first = pos.first-2;
				newstate.enemySoldiers[i].second = pos.second+2;
				
				newstate.board[pos.first-2][pos.second+2]=-1;
				newstate.board[pos.first][pos.second]=0;

				update(newstate.mySoldiers, pos.first-2, pos.second+2);
				update(newstate.myTownhalls, pos.first-2, pos.second+2);
				res.push_back(newstate);
			} else if(isAllyHere(this->enemySoldiers, this->enemyTownhalls, pos.first-2, pos.second+2)==false && isPositionValid(pos.first-2, pos.second+2) && isUnderAttack(this->mySoldiers, pos.first-2, pos.second+2)){
				newstate = *this;

				newstate.enemySoldiers[i].first = pos.first-2;
				newstate.enemySoldiers[i].second = pos.second+2;
				
				newstate.board[pos.first-2][pos.second+2]=-1;
				newstate.board[pos.first][pos.second]=0;
				res.push_back(newstate);
			}

			//back left daignaolly
			if(isEnemyHere(this->mySoldiers, this->myTownhalls, pos.first-2, pos.second-2) && isUnderAttack(this->mySoldiers, pos.first-2, pos.second-2)){
				
				newstate = *this;

				newstate.enemySoldiers[i].first = pos.first-2;
				newstate.enemySoldiers[i].second = pos.second-2;
				
				newstate.board[pos.first-2][pos.second-2]=-1;
				newstate.board[pos.first][pos.second]=0;

				update(newstate.mySoldiers, pos.first-2, pos.second-2);
				update(newstate.myTownhalls, pos.first-2, pos.second-2);
				res.push_back(newstate);
			} else if(isAllyHere(this->enemySoldiers, this->enemyTownhalls, pos.first-2, pos.second-2)==false && isPositionValid(pos.first-2, pos.second-2) && isUnderAttack(this->mySoldiers, pos.first-2, pos.second-2)){
				newstate = *this;

				newstate.enemySoldiers[i].first = pos.first-2;
				newstate.enemySoldiers[i].second = pos.second-2;
				
				newstate.board[pos.first-2][pos.second-2]=-1;
				newstate.board[pos.first][pos.second]=0;
				res.push_back(newstate);
			}

			//back left down
			if(isEnemyHere(this->mySoldiers, this->myTownhalls, pos.first-2, pos.second) && isUnderAttack(this->mySoldiers, pos.first-2, pos.second)){
				
				newstate = *this;

				newstate.enemySoldiers[i].first = pos.first-2;
				newstate.enemySoldiers[i].second = pos.second;
				
				newstate.board[pos.first-2][pos.second]=-1;
				newstate.board[pos.first][pos.second]=0;

				update(newstate.mySoldiers, pos.first-2, pos.second);
				update(newstate.myTownhalls, pos.first-2, pos.second);
				res.push_back(newstate);
			} else if(isAllyHere(this->enemySoldiers, this->enemyTownhalls, pos.first-2, pos.second)==false && isPositionValid(pos.first-2, pos.second) && isUnderAttack(this->mySoldiers, pos.first-2, pos.second)){
				newstate = *this;

				newstate.enemySoldiers[i].first = pos.first-2;
				newstate.enemySoldiers[i].second = pos.second;
				
				newstate.board[pos.first-2][pos.second]=-1;
				newstate.board[pos.first][pos.second]=0;
				res.push_back(newstate);
			}

			//couldnt debug this because root wont have enemy sideways
			if(isEnemyHere(this->mySoldiers, this->myTownhalls, pos.first, pos.second-1) && isPositionValid(pos.first, pos.second-1)){//side left only when enemy
				newstate = *this;

				newstate.enemySoldiers[i].first = pos.first;
				newstate.enemySoldiers[i].second = pos.second-1;
				
				newstate.board[pos.first][pos.second-1]=1;
				newstate.board[pos.first][pos.second]=0;

				update(newstate.mySoldiers, pos.first+1, pos.second-1);
				update(newstate.myTownhalls, pos.first+1, pos.second-1);
				res.push_back(newstate);
			}
			if(isEnemyHere(this->mySoldiers, this->myTownhalls, pos.first, pos.second+1) && isPositionValid(pos.first, pos.second+1)){//side right only when enemy
				newstate = *this;

				newstate.enemySoldiers[i].first = pos.first;
				newstate.enemySoldiers[i].second = pos.second+1;
				
				newstate.board[pos.first+1][pos.second+1]=1;
				newstate.board[pos.first][pos.second]=0;

				update(newstate.mySoldiers, pos.first+1, pos.second+1);
				update(newstate.myTownhalls, pos.first+1, pos.second+1);
				res.push_back(newstate);
			}
		}
		return res;
	}

	vector<State> giveAllChildsOfMax(){
		vector<State> res;
		//cout<<"JEGS";
		State newstate = *this;
		for(int i=0;i<12;i++){
			
			CORDINATE pos = this->mySoldiers[i];
			// cout<<"{"<<pos.first<<", "<<pos.second<<"}"<<"\n";

			if(isEnemyHere(this->enemySoldiers, this->enemyTownhalls, pos.first-1, pos.second)){ // up				
				
				//duplicated this state
				newstate = *this;

				newstate.mySoldiers[i].first = pos.first-1;
				newstate.mySoldiers[i].second = pos.second;
				this->board[pos.first-1][pos.second]=1;
				
				//Actually either of these two below will happen
				update(newstate.enemySoldiers, pos.first-1, pos.second);
				update(newstate.enemyTownhalls, pos.first-1, pos.second);
				newstate.board[pos.first][pos.second] = 0;

				res.push_back(newstate);
			} else if(isAllyHere(this->mySoldiers, this->myTownhalls, pos.first-1, pos.second)==false && isPositionValid(pos.first-1, pos.second)){
				// cout<<"{"<<pos.first<<", "<<pos.second<<"}"<<"\n";
				
				newstate = *this;

				newstate.mySoldiers[i].first = pos.first-1;
				newstate.mySoldiers[i].second = pos.second;


				newstate.board[pos.first-1][pos.second]=1; //update board by moving my soldier here
				newstate.board[pos.first][pos.second] = 0;
				// printBoard(newstate.board);
				res.push_back(newstate);
			}

			if(isEnemyHere(this->enemySoldiers, this->enemyTownhalls, pos.first-1, pos.second-1)){//up left daignaolly
				newstate = *this;

				newstate.mySoldiers[i].first = pos.first-1;
				newstate.mySoldiers[i].second = pos.second-1;
				
				newstate.board[pos.first-1][pos.second-1]=1;
				newstate.board[pos.first][pos.second]=0;

				update(newstate.enemySoldiers, pos.first-1, pos.second-1);
				update(newstate.enemyTownhalls, pos.first-1, pos.second-1);
				res.push_back(newstate);
			} else if(isAllyHere(this->mySoldiers, this->myTownhalls, pos.first-1, pos.second-1)==false && isPositionValid(pos.first-1, pos.second-1)){
				newstate = *this;

				newstate.mySoldiers[i].first = pos.first-1;
				newstate.mySoldiers[i].second = pos.second-1;
				
				newstate.board[pos.first-1][pos.second-1]=1;
				newstate.board[pos.first][pos.second]=0;
				res.push_back(newstate);
			}

			if(isEnemyHere(this->enemySoldiers, this->enemyTownhalls, pos.first-1, pos.second+1)){//up right daignaolly
				newstate = *this;

				newstate.mySoldiers[i].first = pos.first-1;
				newstate.mySoldiers[i].second = pos.second+1;
				
				newstate.board[pos.first-1][pos.second+1]=1;
				newstate.board[pos.first][pos.second]=0;

				update(newstate.enemySoldiers, pos.first-1, pos.second+1);
				update(newstate.enemyTownhalls, pos.first-1, pos.second+1);
				res.push_back(newstate);
			} else if(isAllyHere(this->mySoldiers, this->myTownhalls, pos.first-1, pos.second+1)==false && isPositionValid(pos.first-1, pos.second+1)){
				newstate = *this;

				newstate.mySoldiers[i].first = pos.first-1;
				newstate.mySoldiers[i].second = pos.second+1;
				
				newstate.board[pos.first-1][pos.second+1]=1;
				newstate.board[pos.first][pos.second]=0;
				res.push_back(newstate);
			}

			//back right daignaolly
			if(isEnemyHere(this->enemySoldiers, this->enemyTownhalls, pos.first+2, pos.second+2) && isUnderAttack(this->enemySoldiers, pos.first+2, pos.second+2)){
				newstate = *this;

				newstate.mySoldiers[i].first = pos.first+2;
				newstate.mySoldiers[i].second = pos.second+2;
				
				newstate.board[pos.first+2][pos.second+2]=1;
				newstate.board[pos.first][pos.second]=0;

				update(newstate.enemySoldiers, pos.first+2, pos.second+2);
				update(newstate.enemyTownhalls, pos.first+2, pos.second+2);
				res.push_back(newstate);
			} else if(isAllyHere(this->mySoldiers, this->myTownhalls, pos.first+2, pos.second+2)==false && isPositionValid(pos.first+2, pos.second+2) && isUnderAttack(this->enemySoldiers, pos.first+2, pos.second+2)){
				newstate = *this;

				newstate.mySoldiers[i].first = pos.first+2;
				newstate.mySoldiers[i].second = pos.second+2;
				
				newstate.board[pos.first+2][pos.second+2]=1;
				newstate.board[pos.first][pos.second]=0;
				res.push_back(newstate);
			}

			//back left daignaolly
			if(isEnemyHere(this->enemySoldiers, this->enemyTownhalls, pos.first+2, pos.second-2) && isUnderAttack(this->enemySoldiers, pos.first+2, pos.second-2)){
				
				newstate = *this;

				newstate.mySoldiers[i].first = pos.first+2;
				newstate.mySoldiers[i].second = pos.second-2;
				
				newstate.board[pos.first+2][pos.second-2]=1;
				newstate.board[pos.first][pos.second]=0;

				update(newstate.enemySoldiers, pos.first+2, pos.second-2);
				update(newstate.enemyTownhalls, pos.first+2, pos.second-2);
				res.push_back(newstate);
			} else if(isAllyHere(this->mySoldiers, this->myTownhalls, pos.first+2, pos.second-2)==false && isPositionValid(pos.first+2, pos.second-2) && isUnderAttack(this->enemySoldiers, pos.first+2, pos.second-2)){
				newstate = *this;

				newstate.mySoldiers[i].first = pos.first+2;
				newstate.mySoldiers[i].second = pos.second-2;
				
				newstate.board[pos.first+2][pos.second-2]=1;
				newstate.board[pos.first][pos.second]=0;
				res.push_back(newstate);
			}

			//back left down
			if(isEnemyHere(this->enemySoldiers, this->enemyTownhalls, pos.first+2, pos.second) && isUnderAttack(this->enemySoldiers, pos.first+2, pos.second)){
				
				newstate = *this;

				newstate.mySoldiers[i].first = pos.first+2;
				newstate.mySoldiers[i].second = pos.second;
				
				newstate.board[pos.first+2][pos.second]=1;
				newstate.board[pos.first][pos.second]=0;

				update(newstate.enemySoldiers, pos.first+2, pos.second);
				update(newstate.enemyTownhalls, pos.first+2, pos.second);
				res.push_back(newstate);
			} else if(isAllyHere(this->mySoldiers, this->myTownhalls, pos.first+2, pos.second)==false && isPositionValid(pos.first+2, pos.second) && isUnderAttack(this->enemySoldiers, pos.first+2, pos.second)){
				newstate = *this;

				newstate.mySoldiers[i].first = pos.first+2;
				newstate.mySoldiers[i].second = pos.second;
				
				newstate.board[pos.first+2][pos.second]=1;
				newstate.board[pos.first][pos.second]=0;
				res.push_back(newstate);
			}

			//couldnt debug this because root wont have enemy sideways
			if(isEnemyHere(this->enemySoldiers, this->enemyTownhalls, pos.first, pos.second-1) && isPositionValid(pos.first, pos.second-1)){//side left only when enemy
				newstate = *this;

				newstate.mySoldiers[i].first = pos.first;
				newstate.mySoldiers[i].second = pos.second-1;
				
				newstate.board[pos.first][pos.second-1]=1;
				newstate.board[pos.first][pos.second]=0;

				update(newstate.enemySoldiers, pos.first+1, pos.second-1);
				update(newstate.enemyTownhalls, pos.first+1, pos.second-1);
				res.push_back(newstate);
			}
			if(isEnemyHere(this->enemySoldiers, this->enemyTownhalls, pos.first, pos.second+1) && isPositionValid(pos.first, pos.second+1)){//side right only when enemy
				newstate = *this;

				newstate.mySoldiers[i].first = pos.first;
				newstate.mySoldiers[i].second = pos.second+1;
				
				newstate.board[pos.first+1][pos.second+1]=1;
				newstate.board[pos.first][pos.second]=0;

				update(newstate.enemySoldiers, pos.first+1, pos.second+1);
				update(newstate.enemyTownhalls, pos.first+1, pos.second+1);
				res.push_back(newstate);
			}
		}
		return res;
	}
};

//FORWARD DECLARATION FOR MUTUAL RECURSION
int min_value(State state, int alpha, int beta, int depth);
int max_value(State state, int alpha, int beta, int depth);

int Eval(State state){
	int townhallDiff = state.myTownhalls.size()*2 - state.enemyTownhalls.size();
	return townhallDiff + state.mySoldiers.size()-state.enemySoldiers.size();
}

bool cutoff(State state, int depth){
	if(state.myTownhalls.size()<=2 || state.enemyTownhalls.size()<=2 || state.mySoldiers.size()==0 || state.enemySoldiers.size()==0 || depth>4){return true;}

	return false;
}

int max_value(State state, int alpha, int beta, int depth){
	if(cutoff(state,depth)){return Eval(state);}
	int v = INT_MIN;
	for(auto child:state.giveAllChildsOfMax()){
		v = max(v, min_value(child,alpha,beta,depth+1));
		if(v>=beta){return v;}
		alpha = max(alpha, v);
	}
	return v;
}

int min_value(State state, int alpha, int beta, int depth){
	// cout<<"Yea";
	// cout<<"Depth on min: "<<depth<<"\n";
	if(cutoff(state, depth)){return Eval(state);}
	int v = INT_MAX;
	for(auto child:state.giveAllChildsOfMin()){
		v = min(v, max_value(child, alpha, beta, depth+1));
		if(v<=alpha){return v;}
		beta = beta<v ? beta: v;
	}
	return v;
}

State action(State s1, State s2){
	return s2;
}

void alpha_beta_search(State state){
	int d = 0;
	int v = max_value(state, INT_MIN, INT_MAX, d);
	for(int i=0;i<state.giveAllChildsOfMax().size();i++){
		if(min_value((state.giveAllChildsOfMax())[i], INT_MIN, INT_MAX, d+1)==v){
			// return action(state, (state.giveAllChildsOfMax())[i]);
			printBoard(action(state, (state.giveAllChildsOfMax())[i]).board);
			cout<<"\n";
			break;
		}
	}
}

int main(int argc, char const *argv[])
{
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

	//// GET ALL CHILDREN
	// vector<State> itsChildren = startNode.giveAllChildsOfMin();

	// State secondNode = itsChildren[23];
	// printBoard(secondNode.board);
	// cout<<"\n";

	// itsChildren = secondNode.giveAllChildsOfMin();

	// for(int i=0;i<itsChildren.size();i++){
	// 	printBoard(itsChildren[i].board);
	// 	cout<<"\n";
	// }
	////

	alpha_beta_search(startNode);

	// cout<<itsChildren.size()<<"\n";
	return 0;
}
