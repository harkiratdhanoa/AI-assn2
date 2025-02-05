#include <bits/stdc++.h>
#include <iostream>
#include <vector>
#include <string>
#define CORDINATE pair<int, int>
// #define SIZE_R 8
// #define SIZE_C 8


///////////GOALS FOR ALL NEXT STATES RETURN///////////
//1. write function action(state1, state2)
//2. Use auto& c:s in for loops
//3. Use all possible objects/variable in reference
//4. Cannon moves
//5. Cannon shots
//6. Implement giveAllChildsofMin()

using namespace std;


clock_t begin_time;

int SIZE_R = 8, SIZE_C = 8, play_time = 90;

enum Orient { Vertical, Secondary, Horizontal, Primary };

CORDINATE cannonNeighbours[8] ={make_pair(-1,0), make_pair(-1,1), make_pair(0,1), make_pair(1,1),make_pair(1,0), make_pair(1,-1), make_pair(0,-1), make_pair(-1,-1)};
CORDINATE cannonENeighbours[8] ={make_pair(1,0), make_pair(1,-1), make_pair(0,-1), make_pair(-1,-1),make_pair(-1,0), make_pair(-1,1), make_pair(0,1), make_pair(1,1)};

int adj[5] = {0,1,2,6,7};
float base;
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
	int c = board.front().size();
	for(int i=0;i<r;i++){
		for(int j=0;j<c;j++){
			if(board[i][j]>=0)
				cerr<<" ";
			if(!board[i][j])
				cerr<<". ";
			else
			cerr<<board[i][j]<<" ";
		}
		cerr<<"\n";
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
	float backedupVal;
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
				if(i==1) {
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

	float eval() const {
		// float townhallDiff = myTownhalls.size() - enemyTownhalls.size()*10;
		// float soldierDiff = mySoldiers.size() - enemySoldiers.size();
		// float cannonDiff = myCannons.size() - enemyCannons.size()*5;
		// return 100*townhallDiff + cannonDiff + soldierDiff;
		

		// //if distance between me and enemy is more, play attacker
		// float xOfMe = SIZE_R-1;
		// for(float i=0;i<mySoldiers.size();i++){
		// 	if(mySoldiers[i].first<xOfMe){xOfMe = mySoldiers[i].first;}
		// }
		// float xOfEnemy = 0;
		// for(float i=0;i<enemySoldiers.size();i++){
		// 	if(enemySoldiers[i].first>xOfEnemy){xOfEnemy = enemySoldiers[i].first;}
		// }
		// float xDiff = abs(xOfMe-xOfEnemy);
			
		//cannon diff

		srand(time(NULL));
		float t = ((double)rand()/(RAND_MAX));

		// float diffCannon = 1.2*myCannons.size() - enemyCannons.size();
		float townhallDiff = 1.000001*myTownhalls.size() - enemyTownhalls.size();
		float soldierDiff = 1.0001*mySoldiers.size() - enemySoldiers.size();
		float totTown = myTownhalls.size()+enemyTownhalls.size(), totSold = mySoldiers.size()+enemySoldiers.size(), totCan = myCannons.size()+enemyCannons.size();

		float diffC =0.0,eff=0.0;

		for(auto can: myCannons){
			if(can.second==0)
				eff=0.92;
			else if(can.second==2){
				if(can.first.first==6)
					eff=0.39;
				else if(can.first.first==5)
					eff = 0.27;
				else eff = 0.11;
			}
			else{
				if(t<0.5){eff=1.0;} else {eff=0.87;}
				// eff=0.87;
				srand(time(NULL));
				t = ((double)rand()/(RAND_MAX));
			}

			CORDINATE rel_front = cannonNeighbours[can.second],rel_back = cannonNeighbours[can.second+4],
					  front_piece = add(rel_front,can.first), back_piece = add(rel_back,can.first),
					  move1_front = add(front_piece,rel_front), move1_back = add(back_piece,rel_back);

			bool frontfree=false,backfree=false;
			if(isPositionValid(move1_front.first,move1_front.second))
				if(!board[move1_front.first][move1_front.second])
					frontfree=true;
			if(isPositionValid(move1_back.first,move1_back.second))
				if(!board[move1_back.first][move1_back.second])
					backfree=true;
			if(!frontfree || !backfree)
				eff/=1.35;
			if(!frontfree && !backfree)
				eff/=5;

			diffC+=1.0001*(eff);
		}
		eff=0.0;
		for(auto can: enemyCannons){
			if(can.second==0)
				eff=0.92;
			else if(can.second==2){
				if(can.first.first==6)
					eff=0.39;
				else if(can.first.first==5)
					eff = 0.27;
				else eff = 0.11;
			}
			else{
				if(t<0.5){eff=1;} else {eff=0.87;}
				// eff=0.87;
				srand(time(NULL));
				t = ((double)rand()/(RAND_MAX));
			}

			CORDINATE rel_front = cannonENeighbours[can.second],rel_back = cannonENeighbours[can.second+4],
					  front_piece = add(rel_front,can.first), back_piece = add(rel_back,can.first),
					  move1_front = add(front_piece,rel_front), move1_back = add(back_piece,rel_back);

			bool frontfree=false,backfree=false;
			if(isPositionValid(move1_front.first,move1_front.second))
				if(!board[move1_front.first][move1_front.second])
					frontfree=true;
			if(isPositionValid(move1_back.first,move1_back.second))
				if(!board[move1_back.first][move1_back.second])
					backfree=true;
			if(!frontfree || !backfree)
				eff/=1.35;
			if(!frontfree && !backfree)
				eff/=5;

			diffC-=(eff);
		}
		// if(10000<soldierDiff)
		// 	cerr<<"ma soldier: "+to_string(mySoldiers.size())+"his soldiers: "+to_string(enemySoldiers.size())+"\n";
		return 100.0*townhallDiff + diffC + 4.0*soldierDiff;
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

	bool isPositionValid(int i, int j) const{
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
				for(auto rel:adj){
					if(v == make_pair((myNeighbours[rel].first+x),(myNeighbours[rel].second+y)))
						return true;
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
				for(auto rel:adj){
					if(v == make_pair((enemyNeighbours[rel].first+x),(enemyNeighbours[rel].second+y)))
						return true;
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
			else if(i< enemyTownhalls.size())
				if(enemyTownhalls[i].first == x && enemyTownhalls[i].second == y){
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
			else if(i< myTownhalls.size())
				if(myTownhalls[i].first ==x && myTownhalls[i].second == y){
				myTownhalls.erase(myTownhalls.begin()+i);
				break;
			}
		}
	}

	void modifyBoard(string abc){
		CORDINATE prevPos = make_pair(int( abc.at(4) - 48 ), int(abc.at(2)-48));
		CORDINATE nextPos = make_pair(int( abc.at(10) - 48 ), int(abc.at(8)-48));
	    if(!player_type){
			prevPos.first = SIZE_R -1 - prevPos.first;
		  	prevPos.second = SIZE_C -1 - prevPos.second;
		  	nextPos.first = SIZE_R -1 - nextPos.first;
		  	nextPos.second = SIZE_C -1 - nextPos.second;
	    }
	    cerr<<"opponent played: "<<"S "<< prevPos.first<<" "<<prevPos.second<<" "<< abc.at(6)<<" "<< nextPos.first<<" "<<nextPos.second<<"\n";
		if(abc.at(6)=='B'){
		  if(board[nextPos.first][nextPos.second]>0)
		  	killOwn(nextPos.first,nextPos.second);
		  board[nextPos.first][nextPos.second]=0;
		  updateCannons();
	    }
	    else{
	      
	      kill(prevPos.first,prevPos.second);
	      if(board[nextPos.first][nextPos.second]>0)
	      	killOwn(nextPos.first,nextPos.second);
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
			if(board.front()[2*i]==-2)
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
			{	if(board[sold_pos.first+1][sold_pos.second+1]==1 && board[sold_pos.first-1][sold_pos.second-1] == 1)
					myCannons.push_back(make_pair(sold_pos,Primary));
			}
			if(isPositionValid(sold_pos.first,sold_pos.second+1) && isPositionValid(sold_pos.first,sold_pos.second-1))
			{	if(board[sold_pos.first][sold_pos.second+1]==1 && board[sold_pos.first][sold_pos.second-1] == 1)
					myCannons.push_back(make_pair(sold_pos,Horizontal));
			}
			if(isPositionValid(sold_pos.first+1,sold_pos.second) && isPositionValid(sold_pos.first-1,sold_pos.second))
			{	if(board[sold_pos.first+1][sold_pos.second]==1 && board[sold_pos.first-1][sold_pos.second] == 1)
					myCannons.push_back(make_pair(sold_pos,Vertical));
			}
			if(isPositionValid(sold_pos.first+1,sold_pos.second-1) && isPositionValid(sold_pos.first-1,sold_pos.second+1))
			{	if(board[sold_pos.first+1][sold_pos.second-1]==1 && board[sold_pos.first-1][sold_pos.second+1] == 1)
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
        return this->eval() < other.eval();
    }
    bool operator< (const State& other) const {
        return this->eval() > other.eval();
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
					newstate.enemySoldiers[j] = final;
					newstate.board[final.first][final.second]=-1;
					newstate.killOwn(final.first, final.second);
					newstate.board[pos.first][pos.second] = 0;
					newstate.updateCannons();
					newstate.action = "Enemy "+to_string(pos.first)+" "+to_string(pos.second)+" M "+to_string(final.first)+" "+to_string(final.second);

					res.push_back(newstate);
					
				} 
				else if(isEnemyHere(final.first, final.second)==false && (i!=L) && (i!=R) && isPositionValid(final.first,final.second)){ // ENEMY MOVE (TO ITS UL,U,UR)
					newstate = *this;

					newstate.enemySoldiers[j] = final;
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
					} else if(isPositionValid(shoot2_front.first,shoot2_front.second))
						if(board[shoot2_front.first][shoot2_front.second]==0){
							newstate = *this;
							newstate.action = "Cannon "+to_string(back_piece.first)+" "+to_string(back_piece.second)+" B "+to_string(shoot2_front.first)+" "+to_string(shoot2_front.second);
			// cout<<newstate.action<<"!          ";
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
					} else if(isPositionValid(shoot3_front.first,shoot3_front.second))
						if(board[shoot3_front.first][shoot3_front.second]==0){
							newstate = *this;
							newstate.action = "Cannon "+to_string(back_piece.first)+" "+to_string(back_piece.second)+" B "+to_string(shoot3_front.first)+" "+to_string(shoot3_front.second);
			// cout<<newstate.action<<"!          ";
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
					} else if(isPositionValid(shoot2_back.first,shoot2_back.second))
						if(board[shoot2_back.first][shoot2_back.second]==0){
							newstate = *this;
							newstate.action = "Cannon "+to_string(back_piece.first)+" "+to_string(back_piece.second)+" B "+to_string(shoot2_back.first)+" "+to_string(shoot2_back.second);
			// cout<<newstate.action<<"!          ";
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
					} else if(isPositionValid(shoot3_back.first,shoot3_back.second))
						if(board[shoot3_back.first][shoot3_back.second]==0){
							newstate = *this;
							newstate.action = "Cannon "+to_string(back_piece.first)+" "+to_string(back_piece.second)+" B "+to_string(shoot3_back.first)+" "+to_string(shoot3_back.second);
			// cout<<newstate.action<<"!          ";
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
		// if(res.front().eval() > res.back().eval()){cerr<<"\nRes.front() > res.back() in MIN\n";}
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


				
				if((i==DL||i==D||i==DR) && !isUnderAttack(true, pos.first, pos.second)){ 			// BACKING OUT NOT ALLOWED
			// cout<<"CAN'T RETREAT!          ";
					continue;
				}
				if(isEnemyHere(final.first, final.second)){ 			// KILL ENEMY (AT ANY OF 8)
			// cout<<"KILLING "<<final.first<<","<<final.second<<"!          ";

					newstate = *this;

					newstate.mySoldiers[j] = final;
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

					newstate.mySoldiers[j] = make_pair(final.first, final.second);
					
					// cerr<<"{"<<newstate.mySoldiers[j].first<<", "<<newstate.mySoldiers[j].second<<"}"<<"\n";
					// cerr<<"{"<<final.first<<", "<<final.second<<"}"<<"\n";
					newstate.board[pos.first][pos.second] = 0;
					newstate.board[final.first][final.second]=1;
					// newstate.killOwn(pos.first, pos.second);
					
					// cerr<<"DONE removing"<<"\n";
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
					// newstate.action = to_string(move1_front.first)+" "+to_string(move1_front.second)+to_string(front_piece.first)+" "+to_string(front_piece.first)+" / "+ to_string(pos.first)+" "+to_string(pos.first)+" / "+
					// to_string(back_piece.first)+" "+to_string(back_piece.second)+" M ";
					// newstate.action = "rel "+to_string(rel_front.first)+" "+to_string(rel_front.second)+" Mid "+to_string(pos.first)+" "+to_string(pos.second)
					+"type: "+to_string(type);
					res.push_back(newstate);
					
					if(isEnemyHere(shoot2_front.first,shoot2_front.second)){
						newstate = *this;
						newstate.kill(shoot2_front.first,shoot2_front.second);
						newstate.board[shoot2_front.first][shoot2_front.second] = 0;
						newstate.updateCannons();
						newstate.action = "S "+to_string(back_piece.first)+" "+to_string(back_piece.second)+" B "+to_string(shoot2_front.first)+" "+to_string(shoot2_front.second);
						res.push_back(newstate);
					}else if(isPositionValid(shoot2_front.first,shoot2_front.second))
						if(board[shoot2_front.first][shoot2_front.second]==0){
							newstate = *this;
							newstate.action = "Cannon "+to_string(back_piece.first)+" "+to_string(back_piece.second)+" B "+to_string(shoot2_front.first)+" "+to_string(shoot2_front.second);
			// cout<<newstate.action<<"!          ";
					}

					if(isEnemyHere(shoot3_front.first,shoot3_front.second)){
						newstate = *this;
						newstate.kill(shoot3_front.first,shoot3_front.second);
						newstate.board[shoot3_front.first][shoot3_front.second] = 0;
						newstate.updateCannons();
						newstate.action = "S "+to_string(back_piece.first)+" "+to_string(back_piece.second)+" B "+to_string(shoot3_front.first)+" "+to_string(shoot3_front.second);
						res.push_back(newstate);
					}else if(isPositionValid(shoot3_front.first,shoot3_front.second))
						if(board[shoot3_front.first][shoot3_front.second]==0){
							newstate = *this;
							newstate.action = "Cannon "+to_string(back_piece.first)+" "+to_string(back_piece.second)+" B "+to_string(shoot3_front.first)+" "+to_string(shoot3_front.second);
			// cout<<newstate.action<<"!          ";
					}
				}

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
					}else if(isPositionValid(shoot2_back.first,shoot2_back.second))
						if(board[shoot2_back.first][shoot2_back.second]==0){
							newstate = *this;
							newstate.action = "Cannon "+to_string(back_piece.first)+" "+to_string(back_piece.second)+" B "+to_string(shoot2_back.first)+" "+to_string(shoot2_back.second);
			// cout<<newstate.action<<"!          ";
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
					}else if(isPositionValid(shoot3_back.first,shoot3_back.second))
						if(board[shoot3_back.first][shoot3_back.second]==0){
							newstate = *this;
							newstate.action = "Cannon "+to_string(back_piece.first)+" "+to_string(back_piece.second)+" B "+to_string(shoot3_back.first)+" "+to_string(shoot3_back.second);
			// cout<<newstate.action<<"!          ";
					}
			// else cout<<"SHOOT3 INVALID!          ";
				}
			}

			// else {cout<<"MOVE.1 INVALID!          ";
			// cout<<"SHOOT.2 INVALID!          ";
			// cout<<"SHOOT.3 INVALID!          ";}	
		}
		std::sort(res.begin(), res.end());
		// if(res.front().eval() < res.back().eval()){cerr<<"\nRes.front() > res.back() in MAX\n";}
		return res;
	}
};

//FORWARD DECLARATION FOR MUTUAL RECURSION
int min_value(State& state, int alpha, int beta, int depth, int threshold,float timeconstraint);
int max_value(State& state, int alpha, int beta, int depth, int threshold,float timeconstraint);

bool cutoff(State& state, int depth, int threshold){
	if(state.myTownhalls.size()<=2 || state.enemyTownhalls.size()<=2 || state.mySoldiers.size()==0 || state.enemySoldiers.size()==0 || depth>threshold){return true;}
	return false;
}

bool flag;
int thresh;
int no;
int max_value(State& state, int alpha, int beta, int depth, int threshold,float timeconstraint){
	if(cutoff(state, depth, threshold) /*&& isEssentialToGoDown(state, true)==false*/){
        state.backedupVal = state.eval(); 
        //DEBUG
        // printBoard(state.board);
        // cout<<"MAX NODE"<<endl<<"Backup val: "<<state.backedupVal<<'\n';
        return state.backedupVal;
    }
	int v = INT_MIN;
	vector<State> children =state.giveAllChildsOfMax();
	if(depth>=3)
		no=1;
	else no=4;
	for(int i=0;i<no*children.size()/4;i++){
		flag = abs(state.eval()-base)>1.1*abs(children[i].eval()-base);
		if(float( clock () - begin_time ) >=timeconstraint*25*1000000/20 && !flag)
			return v;
		if(flag && threshold-1  <= depth){
			thresh=threshold+1;
			// cerr<<"quin MAXXX\n";
		}
		else thresh=threshold;
		v = max(v, min_value(children[i],alpha,beta,depth+1, thresh,timeconstraint));
		if(v>=beta){state.backedupVal = v; return v;}
		alpha = max(alpha, v);
	}
	return v;
}


int min_value(State& state, int alpha, int beta, int depth, int threshold,float timeconstraint){
	if(cutoff(state, depth, threshold) /*&& isEssentialToGoDown(state, false)==false*/){
        state.backedupVal = state.eval(); 
        //DEBUG
        // printBoard(state.board);
        // cout<<"MIN NODE"<<endl<<"Backup val: "<<state.backedupVal<<'\n';
        return state.backedupVal;
    }
	int v = INT_MAX;
	if(depth>=3)
		no=1;
	else no=4;
	vector<State> children =state.giveAllChildsOfMax();
	for(int i=0;i<no*children.size()/4;i++){

		flag = abs(state.eval()-base)>1.1*abs(children[i].eval()-base);
		if(float( clock () - begin_time ) >=timeconstraint*25*1000000/20 && !flag)
			return v;
		if(flag && threshold-1  <= depth){
			thresh=threshold+1;
			// cerr<<"quin MINNNNN\n";
		}
		else thresh=threshold;
		v = min(v, max_value(children[i], alpha, beta, depth+1, thresh,timeconstraint));
		if(v<=alpha){state.backedupVal = v; return v;}
		beta = beta<v ? beta: v;
	}
	state.backedupVal = v; 
	return v;
}

State action(State& s1, State& s2){
	return s2;
}

State alpha_beta_search(State& state, int threshold,float timeconstraint){
	int d = 0;
	int v = max_value(state, INT_MIN, INT_MAX, d, threshold,timeconstraint);
	vector<State> childsMax = state.giveAllChildsOfMax();
	// cout<<"\n\nPRINTING VALUEE OF V\n\n";
	for(int i=0;i<childsMax.size();i++){
		if(min_value((childsMax)[i], INT_MIN, INT_MAX, d+1, threshold,timeconstraint)==v){
			// printBoard(action(state, (childsMax)[i]).board);
			// cout<<"\n";
			// break;
			return (childsMax)[i];
		}
	}
	if(childsMax.front().backedupVal<childsMax.back().backedupVal)
		// cerr<<" \n\n\n\n\nCORRECT  \n\n\n\n\n";
	return childsMax.front();
}

State ids_alpha_beta_search(State& state, float timeconstraint){
	int threshold = 1;
	begin_time=clock();

	State temp = alpha_beta_search(state, threshold++,timeconstraint);
	int  curr_eval = temp.eval();
	// cerr<<"depth: "+to_string(threshold-1)+"\n";
	State bestTillNow = temp;
	// bool flag = abs(curr_eval)>abs(1.005*temp.eval());
	while(float( clock () - begin_time ) <timeconstraint *25*1000000/20 ){

		// flag = abs(curr_eval)>abs(1.005*temp.eval());
		curr_eval = temp.eval();
		// if(flag)
		// 	cerr<<" QUIENSCENT STATE\n";
		temp = alpha_beta_search(state, threshold,timeconstraint);
		// if(float( clock () - begin_time ) >=timeconstraint*25*1000000/20)
		// 	return bestTillNow;
		// if(bestTillNow.eval() < temp.eval()) 
			{bestTillNow = temp; /*cerr<<"depth: "+to_string(threshold)+"\n";*/}
		threshold++;
		// printBoard(bestTillNow.board);
		// cout<<(float)(clock()-begin_time)/timeconstraint*25*1000000/20<<endl;
	}
	// cout<<"\n";
	// cout<<"Depth gone: "<<threshold<<endl;
	// printBoard(bestTillNow.board);
	return bestTillNow;
}


void mainController(){
	string  firstInput = "";
	getline(cin, firstInput);//1 8 8 150
	// cout<<firstInput;
	int playerID;// 1 for my turn first
	playerID = (int)(firstInput.at(0)-48);
	// cerr<<"player id "<<playerID<<"\n";
	// cin>>playerID;
	State initialState = State();
	base = initialState.eval();
	string enemyMove="",outmsg="";
	const clock_t begin_time_real = clock();
	if(playerID==1){
    	player_type = true;
		
	} else {
		player_type = false;
	}
	State outstate;
	// cerr<<"finished search with msg = S "<< prevPos.second<<" "<<prevPos.first<<" "<< outmsg.at(6)<<" "<< nextPos.second<<" "<<nextPos.first<<"\n";
    if(player_type){		// PLAYER 1 PLAYS FIRST MOVE

		// cerr<<"starting search"<<"\n";
		outstate = ids_alpha_beta_search(initialState,2); cerr<<"eval of: "<<outstate.eval()<<"\n";
		outmsg = outstate.action;
		// cerr<<"wanna do: "<<outmsg<<"\n";
		initialState = outstate;
		CORDINATE prevPos = make_pair(int( outmsg.at(2) - 48 ), int(outmsg.at(4)-48));
		CORDINATE nextPos = make_pair(int( outmsg.at(8) - 48 ), int(outmsg.at(10)-48));
    	cout<<"S "<< prevPos.second<<" "<<prevPos.first<<" "<< outmsg.at(6)<<" "<< nextPos.second<<" "<<nextPos.first<<"\n";
    }
	
	// cerr<<"\n\n\n\nstart game\n\n\n\n\n\n";

	while(float(clock() - begin_time_real) < 10000000){
		getline(cin, enemyMove);
		initialState.modifyBoard(enemyMove);
		outstate = ids_alpha_beta_search(initialState, 1.9); //cerr<<"eval of: "<<outstate.eval()<<"\n";
		outmsg = outstate.action;
// cerr<<"wanna do: "<<outmsg<<"\n";
		initialState = outstate;
// cerr<<"just about to do: "<<outmsg<<"\n";
		CORDINATE prevPos = make_pair(int( outmsg.at(2) - 48 ), int(outmsg.at(4)-48));
		CORDINATE nextPos = make_pair(int( outmsg.at(8) - 48 ), int(outmsg.at(10)-48));
	    if(!player_type){
	    	prevPos.first = SIZE_R -1 - prevPos.first;
	      	prevPos.second = SIZE_C -1 - prevPos.second;
	      	nextPos.first = SIZE_R -1 - nextPos.first;
	      	nextPos.second = SIZE_C -1 - nextPos.second;
// cerr<<"did this: "<<outmsg<<"\n";
	    	cout<<"S "<< prevPos.second<<" "<<prevPos.first<<" "<< outmsg.at(6)<<" "<< nextPos.second<<" "<<nextPos.first<<"\n";
// cerr<<"literally did this: "<<outmsg<<"\n";
	    }
	    else 
	    	{cout<<"S "<< prevPos.second<<" "<<prevPos.first<<" "<< outmsg.at(6)<<" "<< nextPos.second<<" "<<nextPos.first<<"\n";}
	}


	// cerr<<"\n\n\n\nmid game\n\n\n\n\n\n";
	while(float(clock() - begin_time_real) < 60000000){
		getline(cin, enemyMove);
		initialState.modifyBoard(enemyMove);
		outstate = ids_alpha_beta_search(initialState, 2.4); //cerr<<"eval of: "<<outstate.eval()<<"\n";
		outmsg = outstate.action;
// cerr<<"wanna do: "<<outmsg<<"\n";
		initialState = outstate;
// cerr<<"just about to do: "<<outmsg<<"\n";
		CORDINATE prevPos = make_pair(int( outmsg.at(2) - 48 ), int(outmsg.at(4)-48));
		CORDINATE nextPos = make_pair(int( outmsg.at(8) - 48 ), int(outmsg.at(10)-48));
	    if(!player_type){
	    	prevPos.first = SIZE_R -1 - prevPos.first;
	      	prevPos.second = SIZE_C -1 - prevPos.second;
	      	nextPos.first = SIZE_R -1 - nextPos.first;
	      	nextPos.second = SIZE_C -1 - nextPos.second;
// cerr<<"did this: "<<outmsg<<"\n";
	    	cout<<"S "<< prevPos.second<<" "<<prevPos.first<<" "<< outmsg.at(6)<<" "<< nextPos.second<<" "<<nextPos.first<<"\n";
// cerr<<"literally did this: "<<outmsg<<"\n";
	    }
	    else 
	    	{cout<<"S "<< prevPos.second<<" "<<prevPos.first<<" "<< outmsg.at(6)<<" "<< nextPos.second<<" "<<nextPos.first<<"\n";}
	}
		
	// cerr<<"\n\n\n\nend game\n\n\n\n\n\n";

	while(float(clock() - begin_time_real) < 100000000){
		getline(cin, enemyMove);
		initialState.modifyBoard(enemyMove);
		outstate = ids_alpha_beta_search(initialState, 2.1);//cerr<<"eval of: "<<outstate.eval()<<"\n";
		outmsg = outstate.action;
// cerr<<"wanna do: "<<outmsg<<"\n";
		initialState = outstate;
// cerr<<"just about to do: "<<outmsg<<"\n";
		CORDINATE prevPos = make_pair(int( outmsg.at(2) - 48 ), int(outmsg.at(4)-48));
		CORDINATE nextPos = make_pair(int( outmsg.at(8) - 48 ), int(outmsg.at(10)-48));
	    if(!player_type){
	    	prevPos.first = SIZE_R -1 - prevPos.first;
	      	prevPos.second = SIZE_C -1 - prevPos.second;
	      	nextPos.first = SIZE_R -1 - nextPos.first;
	      	nextPos.second = SIZE_C -1 - nextPos.second;
// cerr<<"did this: "<<outmsg<<"\n";
	    	cout<<"S "<< prevPos.second<<" "<<prevPos.first<<" "<< outmsg.at(6)<<" "<< nextPos.second<<" "<<nextPos.first<<"\n";
// cerr<<"literally did this: "<<outmsg<<"\n";
	    }
	    else 
	    	{cout<<"S "<< prevPos.second<<" "<<prevPos.first<<" "<< outmsg.at(6)<<" "<< nextPos.second<<" "<<nextPos.first<<"\n";}
	}
}

int main(int argc, char const *argv[])
{
	mainController();
	return 0;
}
