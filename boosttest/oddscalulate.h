#ifndef __odds__oddscalulate__
#define __odds__oddscalulate__

#include <boost/bind.hpp>

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <iostream>

using namespace std;

enum unit_price { lottery = 2, max_total_cost = 100000 };
enum team_odd_type { max_odds_type = 3 };
enum play_mode { none_modem, portion_mode, whole_mode, manual_mode };
enum result_type { host_win = 0x01, shake_hand = 0x10, away_win = 0x100,  };

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

typedef vector<fixtures_base_odds> base_odds_vector;

//-----------------------------------------------------------------------

class forecas_result
{
	double odds_;
	char result_msg_[10];

	unsigned short multiple_;	 //bei shu
	double income_;
	double yield_;			     //Unit of Measure %
	double total_cost_;
	double net_income_;
    short flag_;
public:
	forecas_result(double odds, const char* result_msg);
    
	void set_result_multiple(unsigned short multiple);
	void set_total_cost(double tcost);
	void set_flag(short type) { flag_ |= type; }
	void clear_dynamic_data();

	unsigned short get_result_multiple() const { return multiple_; }
	double get_result_odds() const { return odds_; }
	double get_result_yield() const { return yield_; }
	double get_result_income() const { return income_; }
	double get_net_income() const { return net_income_; }
	double get_total_cost() const { return total_cost_; }
	const char* get_result_msg() const { return result_msg_; }
	short get_flag() const { return flag_; }
};

//-----------------------------------------------------------------------

typedef map<unsigned int, forecas_result> forecas_result_map;
//extern forecas_result_map forecas_results_map_;
typedef pair<unsigned int, forecas_result> forecas_result_pair;

//-----------------------------------------------------------------------

class position;
class regulator;

class organizer
{
	double htwin_, sh_, atwin_;
	char htname_[20], atname_[20], tmp_[100], result_[5];
    short flag_;

	base_odds_vector base_odds_;
	forecas_result_map forecas_results_;

	position* position_;
	regulator* regulator_;
public:
    organizer();
    
	forecas_result_map* get_result_map() { return &forecas_results_; }
	position* get_position() { return position_; }

	void print();
	void print(forecas_result_pair rpair);

private:
    void init();
    void forecas_calculate(base_odds_vector::iterator begin, base_odds_vector::iterator end);
    void set_forecas_result_map(fixtures_base_odds first, fixtures_base_odds second);
	void result_msg(int first, int second);
	void print_result(forecas_result_pair rpair);
	const char* msg_type(int index);
	short flag_type(int index);
};

//-----------------------------------------------------------------------

class position
{
	char earnings_range_[100];   //range of yield
	double cost_;
	int real_size_;

	organizer* organizer_;

public:
	position(organizer* org);

	void refresh();

	double get_cost() { return cost_; }
	const char* get_earnings_range() { return earnings_range_; }
	void set_real_size(int rsize) { real_size_ = rsize; }
	int get_real_size() const { return real_size_; }
	int add_someone_position(unsigned int index);

private:
	void total_cost(forecas_result_pair rpair);
	void set_result_cost(forecas_result_pair rpair);
};

//-----------------------------------------------------------------------

class regulator
{
	double adjusted_income_;
	double adjusted_yield_;

	play_mode mode_;
	organizer* organizer_;

public:
	regulator(organizer* org);

	void set_adjusted_min_income(double min_income);
	void set_adjusted_min_yield(double min_yield);
	void set_play_mode(play_mode mode);

private:
	void init_position();
	void init_results(forecas_result_pair rpair);
	void hedge_positions();
	void add_all_position(forecas_result_pair& rpair);
	void adjust_positions(double value);
};

//-----------------------------------------------------------------------

#endif /* defined(__odds__oddscalulate__) */
