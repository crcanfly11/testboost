#ifndef __odds__oddscalulate__
#define __odds__oddscalulate__

#include <iostream>

#include <boost/bind.hpp>

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

enum team_odd_type { max_odds_type = 3 };

//-----------------------------------------------------------------------

class fixtures_base_odds
{
	enum team_odd_enum { home_team_win = 0, shake_hands = 1, away_team_win = 2 };
    
	double team_odds_[3];
    
	char home_team_[30];
	char away_team_[30];
public:
	fixtures_base_odds(double win_odds, double shake_hands_odds, double lose_odds, const char* home_team_name, const char* away_team_name);
    
	double operator[] (int index);

	double get_win_odds() const { return team_odds_[home_team_win]; }
	double get_shake_hands_odds() const { return team_odds_[shake_hands]; }
	double get_lose_odds() const { return team_odds_[away_team_win]; }

	const char* get_home_team_name() { return home_team_; }
	const char* get_away_team_name() { return away_team_; }
};

//-----------------------------------------------------------------------

class forecas_result
{
	double odds_;
	unsigned short multiple_;	 //bei shu
	double yield_;			     //Unit of Measure %
	char result_msg_[10];
    
public:
	forecas_result(double odds, const char* result_msg);
    
	void set_result_multiple(unsigned short multiple) { multiple_ = multiple; }
	unsigned short get_result_multiple() const { return multiple_; }
	double get_result_odds() const { return odds_; }
	double get_result_yield() const { return yield_; }
	const char* get_result_msg() const { return result_msg_; }
};

//-----------------------------------------------------------------------

class position
{
	char earnings_range[100];   //range of yield
	double cost;
};

//-----------------------------------------------------------------------

class organizer
{
	double htwin_, sh_, atwin_;
	char htname_[20], atname_[20], tmp_[100], result_[5];
    
	typedef vector<fixtures_base_odds> base_odds_vector;
	base_odds_vector base_odds_;
    
	typedef map<unsigned int, forecas_result> forecas_result_map;
	forecas_result_map forecas_results_;
    
	typedef pair<unsigned int, forecas_result> forecas_result_pair;

public:
    organizer();
    
private:
    void init();
    void forecas_calculate(base_odds_vector::iterator begin, base_odds_vector::iterator end);
    void set_forecas_result_map(fixtures_base_odds first, fixtures_base_odds second);
	void result_msg(int first, int second);
	const char* get_result_flag(int index);

	void print(forecas_result_pair rpair);
};

//-----------------------------------------------------------------------

#endif /* defined(__odds__oddscalulate__) */
