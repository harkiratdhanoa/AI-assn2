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

enum Orient { Vertical, Secondary, Horizontal, Primary };

CORDINATE cannonNeighbours[8] ={make_pair(-1,0), make_pair(-1,1), make_pair(0,1), make_pair(1,1),make_pair(1,0), make_pair(1,-1), make_pair(0,-1), make_pair(-1,-1)};
CORDINATE cannonENeighbours[8] ={make_pair(1,0), make_pair(1,-1), make_pair(0,-1), make_pair(-1,-1),make_pair(-1,0), make_pair(-1,1), make_pair(0,1), make_pair(1,1)};

enum Neighbour {U,UR,R,DR,D,DL,L,UL};

CORDINATE myNeighbours[8] = {make_pair(-1,0), make_pair(-1,1), make_pair(0,1), make_pair(2,2), make_pair(2,0), make_pair(2,-2), make_pair(0,-1), make_pair(-1,-1) };
CORDINATE enemyNeighbours[8] = {make_pair(1,0), make_pair(1,-1), make_pair(0,-1), make_pair(-2,-2), make_pair(-2,0), make_pair(-2,2), make_pair(0,1), make_pair(1,1) };

bool player_type;

int min(int a, int b){
	return a<b?a:b;
}
int max(int a, int b){
	return a>b?a:b;
}


CORDINATE add(CORDINATE a, CORDINATE b){
	return make_pair(a.first+b.first,a.second+b.second);
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

	// int eval(){
	// 	int townhallDiff = myTownhalls.size() - enemyTownhalls.size();
	// 	int soldierDiff = mySoldiers.size() - enemySoldiers.size();
	// 	int cannonDiff = myCannons.size() - enemyCannons.size();
	// 	return 100*townhallDiff + cannonDiff + 2*soldierDiff;
	// }

	int eval(){
		// int townhallDiff = myTownhalls.size() - enemyTownhalls.size()*10;
		// int soldierDiff = mySoldiers.size() - enemySoldiers.size();
		// int cannonDiff = myCannons.size() - enemyCannons.size()*5;
		// return 100*townhallDiff + cannonDiff + soldierDiff;
		int townhallDiff = myTownhalls.size()*2 - enemyTownhalls.size();
		//if distance between me and enemy is more, play attacker
		int xOfMe = SIZE_R-1;
		for(int i=0;i<mySoldiers.size();i++){
			if(mySoldiers[i].first<xOfMe){xOfMe = mySoldiers[i].first;}
		}
		int xOfEnemy = 0;
		for(int i=0;i<enemySoldiers.size();i++){
			if(enemySoldiers[i].first>xOfEnemy){xOfEnemy = enemySoldiers[i].first;}
		}
		int xDiff = abs(xOfMe-xOfEnemy);
		return 2*townhallDiff - xDiff + 2*mySoldiers.size()-2*enemySoldiers.size();
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

	void modifyBoard(string abc){
		CORDINATE prevPos = make_pair(int( abc.at(4) - 48 ), int(abc.at(2)-48));
		CORDINATE nextPos = make_pair(int( abc.at(10) - 48 ), int(abc.at(8)-48));
	    if(!player_type){
			prevPos.first = SIZE_R - prevPos.first;
		  	prevPos.second = SIZE_R - prevPos.second;
		  	nextPos.first = SIZE_C - nextPos.first;
		  	nextPos.second = SIZE_C - nextPos.second;
	    }
	    cerr<<"opponent played: "<<"S "<< prevPos.first<<" "<<prevPos.second<<" "<< abc.at(6)<<" "<< nextPos.first<<" "<<nextPos.second<<"\n";
		if(abc.at(6)=='B'){
		  killOwn(nextPos.first,nextPos.second);
		  board[nextPos.first][nextPos.second]=0;
		  updateCannons();
	    }
	    else{
	      kill(prevPos.first,prevPos.second);
	      board[nextPos.first][nextPos.second]=-1;
	      board[prevPos.first][prevPos.second] = 0;
	      enemySoldiers.push_back(make_pair(nextPos.first,nextPos.second));
	      updateCannons();
	    }
    

	}
	void updateTownhalls(){
		myTownhalls.clear();
		enemyTownhalls.clear();
		for(int i=0;i<4;i+=2){
			if(board[SIZE_R-1][2*i+1]==2)
			myTownhalls.push_back(make_pair(SIZE_R-1, 2*i+1)); // T1 at (r-1,1) | T2 at (r-1,3);
			if(board[0][2*i]==-2)
			enemyTownhalls.push_back(make_pair(0, 2*i));
		}
	}
	void updateCannons(){
		myCannons.clear();
		for(auto sold_pos:mySoldiers){
			if((sold_pos.first<1 || sold_pos.first>SIZE_R-2) && (sold_pos.second<1 || sold_pos.second>SIZE_C-2)){
				continue;
			}
			if(isPositionValid(sold_pos.first+1,sold_pos.second+1) && isPositionValid(sold_pos.first-1,sold_pos.second-1))
			{if(board[sold_pos.first+1][sold_pos.second+1]==1 && board[sold_pos.first-1][sold_pos.second-1] == 1)
				myCannons.push_back(make_pair(sold_pos,Primary));
			}
			if(isPositionValid(sold_pos.first,sold_pos.second+1) && isPositionValid(sold_pos.first,sold_pos.second-1))
			{if(board[sold_pos.first][sold_pos.second+1]==1 && board[sold_pos.first][sold_pos.second-1] == 1)
				myCannons.push_back(make_pair(sold_pos,Horizontal));
			}
			if(isPositionValid(sold_pos.first+1,sold_pos.second) && isPositionValid(sold_pos.first-1,sold_pos.second))
			{if(board[sold_pos.first+1][sold_pos.second]==1 && board[sold_pos.first-1][sold_pos.second] == 1)
				myCannons.push_back(make_pair(sold_pos,Vertical));
			}
			if(isPositionValid(sold_pos.first+1,sold_pos.second-1) && isPositionValid(sold_pos.first-1,sold_pos.second+1))
			{if(board[sold_pos.first+1][sold_pos.second-1]==1 && board[sold_pos.first-1][sold_pos.second+1] == 1)
				myCannons.push_back(make_pair(sold_pos,Secondary));
			}
		}
		enemyCannons.clear();
		for(auto sold_pos:enemySoldiers){
			if((sold_pos.first<1 || sold_pos.first>SIZE_R-2) && (sold_pos.second<1 || sold_pos.second>SIZE_C-2)){
				continue;
			}
			if(isPositionValid(sold_pos.first+1,sold_pos.second+1) && isPositionValid(sold_pos.first-1,sold_pos.second-1))
			{if(board[sold_pos.first+1][sold_pos.second+1]==-1 && board[sold_pos.first-1][sold_pos.second-1] == -1)
				enemyCannons.push_back(make_pair(sold_pos,Primary));
			}
			if(isPositionValid(sold_pos.first,sold_pos.second+1) && isPositionValid(sold_pos.first,sold_pos.second-1))
			{if(board[sold_pos.first][sold_pos.second+1]==-1 && board[sold_pos.first][sold_pos.second-1] == -1)
				enemyCannons.push_back(make_pair(sold_pos,Horizontal));
			}
			if(isPositionValid(sold_pos.first+1,sold_pos.second) && isPositionValid(sold_pos.first-1,sold_pos.second))
			{if(board[sold_pos.first+1][sold_pos.second]==-1 && board[sold_pos.first-1][sold_pos.second] == -1)
				enemyCannons.push_back(make_pair(sold_pos,Vertical));
			}
			if(isPositionValid(sold_pos.first+1,sold_pos.second-1) && isPositionValid(sold_pos.first-1,sold_pos.second+1))
			{if(board[sold_pos.first+1][sold_pos.second-1]==-1 && board[sold_pos.first-1][sold_pos.second+1] == -1)
				enemyCannons.push_back(make_pair(sold_pos,Secondary));
			}
		}
		return;
	}
	bool operator> (const State& other) const {
        return this->backedupVal < other.backedupVal;
    }
    bool operator< (const State& other) const {
        return this->backedupVal > other.backedupVal;
    }
	vector<State> giveAllChildsOfMin(){
		// cout<<"\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ welcome to child of MIN ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
		vector<State> res;
		State newstate;
		// printBoard(this->board);
		for(int j=0;j<enemySoldiers.size();j++){
			CORDINATE pos = this->enemySoldiers[j];
		// cout<<"\n\nsoldier:" <<j<<" at "<<pos.first<<","<<pos.second<<"\n\n";
			for(int i=0;i<8;i++){

				CORDINATE final = enemyNeighbours[i];
				final.first+=pos.first;
				final.second+=pos.second;

				if((i==DL||i==D||i==DR) && !isUnderAttack(false, pos.first, pos.second)){ 	// BACKING OUT NOT ALLOWED
			// cout<<"CANT RETREAT!          ";
					continue;
				}
				if(isAllyHere(final.first, final.second)){ // GET KILLED (AT ANY OF 8)
					// cout<<"KILLING "<<final.first<<","<<final.second<<"!          ";
					newstate = *this;
					newstate.enemySoldiers[i] = final;
					newstate.board[final.first][final.second]=-1;
					newstate.killOwn(final.first, final.second);
					newstate.board[pos.first][pos.second] = 0;
					newstate.updateCannons();
					newstate.action = "Enemy "+to_string(pos.first)+" "+to_string(pos.second)+" M "+to_string(final.first)+" "+to_string(final.second);

					res.push_back(newstate);
					
				} 
				else if(isEnemyHere(final.first, final.second)==false && (i!=L) && (i!=R) && isPositionValid(final.first,final.second)){ // ENEMY MOVE (TO ITS UL,U,UR)
					newstate = *this;

					newstate.enemySoldiers[i] = final;
					newstate.board[final.first][final.second]=-1;
					newstate.board[pos.first][pos.second] = 0;
					newstate.updateCannons();
					newstate.action = "Enemy "+to_string(pos.first)+" "+to_string(pos.second)+" M "+to_string(final.first)+" "+to_string(final.second);
			// cout<<newstate.action<<"!          ";
					res.push_back(newstate);
				}
			// else cout<<"MOVE INVALID!          ";
			}
		}
		
		for(int j = 0; j< enemyCannons.size(); j++){
			CORDINATE pos = enemyCannons[j].first;
			Orient type = enemyCannons[j].second;
			// cout<<"\n\ncannon:" <<j<<" at "<<pos.first<<","<<pos.second<<" aligned: "<<type<<"\n\n";
			CORDINATE rel_front = cannonENeighbours[type],rel_back = cannonENeighbours[int(type)+4];

			CORDINATE front_piece = add(rel_front,pos), back_piece = add(rel_back,pos),
					  move1_front = add(front_piece,rel_front), move1_back = add(back_piece,rel_back),
					  shoot2_front = add(move1_front,rel_front),shoot2_back = add(move1_back,rel_back),
					  shoot3_front = add(shoot2_front,rel_front),shoot3_back = add(shoot2_back,rel_back);

			if(isPositionValid(move1_front.first,move1_front.second)){
				if(!board[move1_front.first][move1_front.second]){
					newstate = *this;
					newstate.kill(back_piece.first,back_piece.second);
					newstate.board[move1_front.first][move1_front.second]=-1;
					newstate.board[back_piece.first][back_piece.second] = 0;
					newstate.enemySoldiers.push_back(make_pair(move1_front.first,move1_front.second));
					newstate.enemyCannons[j].first=front_piece;
          			newstate.updateCannons();
					newstate.action = "Cannon "+to_string(back_piece.first)+" "+to_string(back_piece.second)+" M "+to_string(move1_front.first)+" "+to_string(move1_front.second);
			// cout<<newstate.action<<"!          ";
					res.push_back(newstate);
					if(isAllyHere(shoot2_front.first,shoot2_front.second)){
						newstate = *this;
						newstate.killOwn(shoot2_front.first,shoot2_front.second);
            			newstate.board[shoot2_front.first][shoot2_front.second] = 0;
            			newstate.updateCannons();
						newstate.action = "Cannon "+to_string(back_piece.first)+" "+to_string(back_piece.second)+" B "+to_string(shoot2_front.first)+" "+to_string(shoot2_front.second);
			// cout<<newstate.action<<"!          ";
						res.push_back(newstate);
					}
			// else {cout<<"SHOOT2 INVALID!          ";}//cout<<shoot2_front.first<<","<<shoot2_front.second<<" has:"<<board[shoot2_front.first][shoot2_front.second]<<"    ";}
					if(isAllyHere(shoot3_front.first,shoot3_front.second)){
						newstate = *this;
						newstate.killOwn(shoot3_front.first,shoot3_front.second);
            			newstate.board[shoot3_front.first][shoot3_front.second] = 0;
            			newstate.updateCannons();
						newstate.action = "Cannon "+to_string(back_piece.first)+" "+to_string(back_piece.second)+" B "+to_string(shoot3_front.first)+" "+to_string(shoot3_front.second);
			// cout<<newstate.action<<"!          ";
						res.push_back(newstate);
					}
			// else {cout<<"SHOOT3 INVALID!          ";}//cout<<shoot3_front.first<<","<<shoot3_front.second<<" has:"<<board[shoot3_front.first][shoot3_front.second]<<"    ";}
				}
			// else {cout<<"MOVE.1 INVALID!          ";
			// cout<<"SHOOT.2 INVALID!          ";
			// cout<<"SHOOT.3 INVALID!          ";}
			}
			if(isPositionValid(move1_back.first,move1_back.second)){
				if(!board[move1_back.first][move1_back.second]){
					newstate = *this;
					newstate.kill(front_piece.first,front_piece.second);
					newstate.board[move1_back.first][move1_back.second]=-1;
					newstate.board[front_piece.first][front_piece.second] = 0;
					newstate.enemySoldiers.push_back(make_pair(move1_back.first,move1_back.second));
					newstate.enemyCannons[j].first=back_piece;
          newstate.updateCannons();
					newstate.action = "Cannon "+to_string(front_piece.first)+" "+to_string(front_piece.second)+" M "+to_string(move1_back.first)+" "+to_string(move1_back.second);
			// cout<<newstate.action<<"!          ";
					res.push_back(newstate);
					if(isAllyHere(shoot2_back.first,shoot2_back.second)){
						newstate = *this;
						newstate.killOwn(shoot2_back.first,shoot2_back.second);
            newstate.board[shoot2_back.first][shoot2_back.second] = 0;
            newstate.updateCannons();
						newstate.action = "Cannon "+to_string(front_piece.first)+" "+to_string(front_piece.second)+" B "+to_string(shoot2_back.first)+" "+to_string(shoot2_back.second);
			// cout<<newstate.action<<"!          ";
						res.push_back(newstate);
					}
			// else {cout<<"SHOOT2 INVALID!          ";}//cout<<shoot2_back.first<<","<<shoot2_back.second<<" has:"<<board[shoot2_back.first][shoot2_back.second]<<"    ";}
					if(isAllyHere(shoot3_back.first,shoot3_back.second)){
						newstate = *this;
						newstate.killOwn(shoot3_back.first,shoot3_back.second);
            newstate.board[shoot3_back.first][shoot3_back.second] = 0;
            newstate.updateCannons();
						newstate.action = "Cannon "+to_string(front_piece.first)+" "+to_string(front_piece.second)+" B "+to_string(shoot3_back.first)+" "+to_string(shoot3_back.second);
			// cout<<newstate.action<<"!          ";
						res.push_back(newstate);
					}
			// else {cout<<"SHOOT3 INVALID!          ";}//cout<<shoot3_back.first<<","<<shoot3_back.second<<" has:"<<board[shoot3_back.first][shoot3_back.second]<<"    ";}
				}		
			// else {cout<<"MOVE.1 INVALID!          ";
			// cout<<"SHOOT.2 INVALID!          ";
			// cout<<"SHOOT.3 INVALID!          ";}
			}	
		}
		std::sort(res.begin(), res.end());
		reverse(res.begin(),res.end());
		if(res[0].backedupVal < res[10].backedupVal){cout<<"\nRight backing up\n";}
		return res;
	}

	vector<State> giveAllChildsOfMax(){
			// cout<<"\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ welcome to child of MAX ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
		vector<State> res;
		State newstate;
			// printBoard(this->board);
		for(int j=0;j<mySoldiers.size();j++){
			CORDINATE pos = this->mySoldiers[j];
			// cout<<"\n\nsoldier:" <<j<<" at "<<pos.first<<","<<pos.second<<"\n\n";
			for(int i=0;i<8;i++){
							// cout<<"neighbour:" <<i<<"\n";

				CORDINATE final = myNeighbours[i];
				final.first+=pos.first;
				final.second+=pos.second;
				
				if((i==DL||i==D||i==DR) && !isUnderAttack(false, pos.first, pos.second)){ 			// BACKING OUT NOT ALLOWED
			// cout<<"CAN'T RETREAT!          ";
					continue;
				}
				if(isEnemyHere(final.first, final.second)){ 			// KILL ENEMY (AT ANY OF 8)
			// cout<<"KILLING "<<final.first<<","<<final.second<<"!          ";

					newstate = *this;

					newstate.mySoldiers[i] = final;
					newstate.board[final.first][final.second]=1;
								// printBoard(board);
					newstate.kill(final.first, final.second);
								// printBoard(board);
					newstate.board[pos.first][pos.second] = 0;
					newstate.updateCannons();
								// newstate.updateTownhalls();
					newstate.action = "S "+to_string(pos.first)+" "+to_string(pos.second)+" M "+to_string(final.first)+" "+to_string(final.second);
					res.push_back(newstate);

				} 
				else if(isAllyHere(final.first, final.second)==false && (i!=L) && (i!=R) && isPositionValid(final.first,final.second)){ 			// I MOVE (TO UL,U,UR)
								// cout<<"I MOVE (TO UL,U,UR) - ";
					newstate = *this;

					newstate.mySoldiers[i] = final;
					newstate.board[final.first][final.second]=1;
					newstate.board[pos.first][pos.second] = 0;
					newstate.updateCannons();
					newstate.action = "S "+to_string(pos.first)+" "+to_string(pos.second)+" M "+to_string(final.first)+" "+to_string(final.second);
			// cout<<newstate.action<<"!          ";
					res.push_back(newstate);
				}
			// else cout<<"MOVE INVALID!          ";
			}
		}
		// return res;

		for(int j = 0; j< myCannons.size(); j++){
			CORDINATE pos = myCannons[j].first;
			Orient type = myCannons[j].second;
			// cout<<"\n\ncannon:" <<j<<" at "<<pos.first<<","<<pos.second<<" aligned: "<<type<<"\n\n";
			CORDINATE rel_front = cannonNeighbours[type],rel_back = cannonNeighbours[int(type)+4];

			CORDINATE front_piece = add(rel_front,pos), back_piece = add(rel_back,pos),
					  move1_front = add(front_piece,rel_front), move1_back = add(back_piece,rel_back),
					  shoot2_front = add(move1_front,rel_front),shoot2_back = add(move1_back,rel_back),
					  shoot3_front = add(shoot2_front,rel_front),shoot3_back = add(shoot2_back,rel_back);

			if(isPositionValid(move1_front.first,move1_front.second)){
				if(!board[move1_front.first][move1_front.second]){					// MOVE CANNON
					newstate = *this;
					newstate.killOwn(back_piece.first,back_piece.second);
					newstate.board[move1_front.first][move1_front.second]=1;
					newstate.board[back_piece.first][back_piece.second] = 0;
					newstate.mySoldiers.push_back(make_pair(move1_front.first,move1_front.second));
					newstate.myCannons[j].first=front_piece;
          newstate.updateCannons();
					newstate.action = "S "+to_string(back_piece.first)+" "+to_string(back_piece.second)+" M "+to_string(move1_front.first)+" "+to_string(move1_front.second);
			// cout<<"CANNO"<<newstate.action<<"!          ";
					res.push_back(newstate);
					if(isEnemyHere(shoot2_front.first,shoot2_front.second)){
						newstate = *this;
						newstate.kill(shoot2_front.first,shoot2_front.second);
						newstate.board[shoot2_front.first][shoot2_front.second] = 0;
						newstate.updateCannons();
						newstate.action = "S "+to_string(back_piece.first)+" "+to_string(back_piece.second)+" B "+to_string(shoot2_front.first)+" "+to_string(shoot2_front.second);
						
			// cout<<"CANNO"<<newstate.action<<"!          ";
						res.push_back(newstate);
					}
			// else cout<<"SHOOT2 INVALID!          ";
					if(isEnemyHere(shoot3_front.first,shoot3_front.second)){
						newstate = *this;
						newstate.kill(shoot3_front.first,shoot3_front.second);
						newstate.board[shoot3_front.first][shoot3_front.second] = 0;
						newstate.updateCannons();
						newstate.action = "S "+to_string(back_piece.first)+" "+to_string(back_piece.second)+" B "+to_string(shoot3_front.first)+" "+to_string(shoot3_front.second);
			// cout<<"CANNO"<<newstate.action<<"!          ";
						res.push_back(newstate);
					}
			// else cout<<"SHOOT3 INVALID!          ";
				}

			// else {cout<<"MOVE.1 INVALID!          ";
			// cout<<"SHOOT.2 INVALID!          ";
			// cout<<"SHOOT.3 INVALID!          ";}

			}
			if(isPositionValid(move1_back.first,move1_back.second)){
				if(!board[move1_back.first][move1_back.second]){
					newstate = *this;
					newstate.killOwn(front_piece.first,front_piece.second);
					newstate.board[move1_back.first][move1_back.second]=1;
					newstate.board[front_piece.first][front_piece.second] = 0;
					newstate.mySoldiers.push_back(make_pair(move1_back.first,move1_back.second));
					newstate.myCannons[j].first=back_piece;
          newstate.updateCannons();
					newstate.action = "S "+to_string(front_piece.first)+" "+to_string(front_piece.second)+" M "+to_string(move1_back.first)+" "+to_string(move1_back.second);
			// cout<<"CANNO"<<newstate.action<<"!          ";
					res.push_back(newstate);
					if(isEnemyHere(shoot2_back.first,shoot2_back.second)){
						newstate = *this;
						newstate.kill(shoot2_back.first,shoot2_back.second);
						newstate.board[shoot2_back.first][shoot2_back.second] = 0;
            newstate.updateCannons();
						newstate.action = "S "+to_string(front_piece.first)+" "+to_string(front_piece.second)+" B "+to_string(shoot2_back.first)+" "+to_string(shoot2_back.second);
			// cout<<"CANNO"<<newstate.action<<"!          ";
						res.push_back(newstate);
					}
			// else cout<<"SHOOT2 INVALID!          ";
					if(isEnemyHere(shoot3_back.first,shoot3_back.second)){
						newstate = *this;
						newstate.kill(shoot3_back.first,shoot3_back.second);
						newstate.board[shoot3_back.first][shoot3_back.second] = 0;
            newstate.updateCannons();
						newstate.action = "S "+to_string(front_piece.first)+" "+to_string(front_piece.second)+" B "+to_string(shoot3_back.first)+" "+to_string(shoot3_back.second);
			// cout<<"CANNO"<<newstate.action<<"!          ";
						res.push_back(newstate);
					}
			// else cout<<"SHOOT3 INVALID!          ";
				}
			}

			// else {cout<<"MOVE.1 INVALID!          ";
			// cout<<"SHOOT.2 INVALID!          ";
			// cout<<"SHOOT.3 INVALID!          ";}	
		}
		std::sort(res.begin(), res.end());
		if(res[0].backedupVal > res[10].backedupVal){cout<<"\nRight backing up\n";}
		return res;
	}
};

//FORWARD DECLARATION FOR MUTUAL RECURSION
int min_value(State& state, int alpha, int beta, int depth, int threshold);
int max_value(State& state, int alpha, int beta, int depth, int threshold);

bool cutoff(State& state, int depth, int threshold){
	if(state.myTownhalls.size()<=2 || state.enemyTownhalls.size()<=2 || state.mySoldiers.size()==0 || state.enemySoldiers.size()==0 || depth>threshold){return true;}
	return false;
}

int max_value(State& state, int alpha, int beta, int depth, int threshold){
	if(cutoff(state, depth, threshold) /*&& isEssentialToGoDown(state, true)==false*/){
        state.backedupVal = state.eval(); 
        //DEBUG
        // printBoard(state.board);
        // cout<<"MAX NODE"<<endl<<"Backup val: "<<state.backedupVal<<'\n';
        return state.backedupVal;
    }
	int v = INT_MIN;
	for(auto child:state.giveAllChildsOfMax()){
		v = max(v, min_value(child,alpha,beta,depth+1, threshold));
		if(v>=beta){state.backedupVal = v; return v;}
		alpha = max(alpha, v);
	}
	return v;
}

int min_value(State& state, int alpha, int beta, int depth, int threshold){
	if(cutoff(state, depth, threshold) /*&& isEssentialToGoDown(state, false)==false*/){
        state.backedupVal = state.eval(); 
        //DEBUG
        // printBoard(state.board);
        // cout<<"MIN NODE"<<endl<<"Backup val: "<<state.backedupVal<<'\n';
        return state.backedupVal;
    }
	int v = INT_MAX;
	for(auto child:state.giveAllChildsOfMin()){
		v = min(v, max_value(child, alpha, beta, depth+1, threshold));
		if(v<=alpha){state.backedupVal = v; return v;}
		beta = beta<v ? beta: v;
	}
	state.backedupVal = v; 
	return v;
}

State action(State& s1, State& s2){
	return s2;
}

State alpha_beta_search(State& state, int threshold){
	int d = 0;
	int v = max_value(state, INT_MIN, INT_MAX, d, threshold);
	vector<State> childsMax = state.giveAllChildsOfMax();
	// cout<<"\n\nPRINTING VALUEE OF V\n\n";
	for(int i=0;i<childsMax.size();i++){
		if(min_value((childsMax)[i], INT_MIN, INT_MAX, d+1, threshold)==v){
			// printBoard(action(state, (childsMax)[i]).board);
			// cout<<"\n";
			// break;
			return (childsMax)[i];
		}
	}
	return childsMax[0];
}

State ids_alpha_beta_search(State& state){
	int threshold = 1;
	const clock_t begin_time = clock();
	State temp = alpha_beta_search(state, threshold++);
	State bestTillNow = temp;
	while(float( clock () - begin_time ) <1000000){
		temp = alpha_beta_search(state, threshold);
		if(bestTillNow.eval() < temp.eval()) {bestTillNow = temp; cerr<<"best = temp\n";}
		threshold++;
		// printBoard(bestTillNow.board);
		// cout<<(float)(clock()-begin_time)/1000000<<endl;
	}
	// cout<<"\n";
	// cout<<"Depth gone: "<<threshold<<endl;
	return bestTillNow;
}


void mainController(){
	string  firstInput = "";
	getline(cin, firstInput);//1 8 8 150
	// cout<<firstInput;
	int playerID;// 1 for my turn first
	playerID = (int)(firstInput.at(0)-48);
	cerr<<"player id "<<playerID<<"\n";
	// cin>>playerID;
	State initialState;
	string enemyMove="";
	const clock_t begin_time = clock();
	if(playerID==1){
    	player_type = true;
		
	} else {
		player_type = false;
	}
	cerr<<"starting search"<<"\n";
	State outstate = ids_alpha_beta_search(initialState);
	string outmsg = outstate.action;
	initialState = outstate;
	CORDINATE prevPos = make_pair(int( outmsg.at(2) - 48 ), int(outmsg.at(4)-48));
	CORDINATE nextPos = make_pair(int( outmsg.at(8) - 48 ), int(outmsg.at(10)-48));
	cerr<<"finished search with msg = S "<< prevPos.second<<" "<<prevPos.first<<" "<< outmsg.at(6)<<" "<< nextPos.second<<" "<<nextPos.first<<"\n";
    if(player_type){
    	cout<<"S "<< prevPos.second<<" "<<prevPos.first<<" "<< outmsg.at(6)<<" "<< nextPos.second<<" "<<nextPos.first<<"\n";
    }
	

	while(float(clock() - begin_time) < 150000000){
		getline(cin, enemyMove);//1 8 8 150
		initialState.modifyBoard(enemyMove);
		outstate = ids_alpha_beta_search(initialState);
		outmsg = outstate.action;
		initialState = outstate;
		CORDINATE prevPos = make_pair(int( outmsg.at(2) - 48 ), int(outmsg.at(4)-48));
		CORDINATE nextPos = make_pair(int( outmsg.at(8) - 48 ), int(outmsg.at(10)-48));
	    if(!player_type){
	    	prevPos.first = SIZE_R - prevPos.first;
	      	prevPos.second = SIZE_R - prevPos.second;
	      	nextPos.first = SIZE_C - nextPos.first;
	      	nextPos.second = SIZE_C - nextPos.second;
	    	cout<<"S "<< prevPos.second<<" "<<prevPos.first<<" "<< outmsg.at(6)<<" "<< nextPos.second<<" "<<nextPos.first<<"\n";
	    }
	    else 
	    	cout<<"S "<< prevPos.second<<" "<<prevPos.first<<" "<< outmsg.at(6)<<" "<< nextPos.second<<" "<<nextPos.first<<"\n";
	}
}

int main(int argc, char const *argv[])
{
	// State startNode;
	// startNode.board[2][1] = 0;
	// startNode.board[4][1] = -1;


	// startNode.board[0][3] = 0;
	// startNode.board[3][3] = -1;

	// startNode.board[1][3] = 0;
	// startNode.board[4][3] = -1;

	// for(int t = 0;t<startNode.enemySoldiers.size();t++){
	// 	if(startNode.enemySoldiers[t].first==2 && startNode.enemySoldiers[t].second==1){
	// 		startNode.enemySoldiers[t].first = 4;
	// 		// startNode.enemySoldiers.push_back({{4,1}, startNode.enemySoldiers[t].second});
	// 		// startNode.enemySoldiers.erase(startNode.enemySoldiers.begin() + t);
	// 	}
	// 	if(startNode.enemySoldiers[t].first==0 && startNode.enemySoldiers[t].second==3){
	// 		startNode.enemySoldiers[t].first = 3;
	// 	}
	// 	if(startNode.enemySoldiers[t].first==1 && startNode.enemySoldiers[t].second==3){
	// 		startNode.enemySoldiers[t].first = 4;
	// 	}
	// }
	// startNode.updateCannons();

    // cout<<ids_alpha_beta_search(startNode);
	mainController();
	return 0;
}