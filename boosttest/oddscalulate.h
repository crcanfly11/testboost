#ifndef __odds__oddscalulate__
#define __odds__oddscalulate__

#include <boost/bind.hpp>

#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <algorithm>


using namespace std;

enum unit_price { lottery = 2, max_total_cost = 500 };
enum team_odd_type { max_odds_type = 3 };
enum play_type { win_draw_lost = 3 };
enum play_mode { none_modem, portion_mode, whole_mode, manual_mode };
enum result_type{ host_win = 0x01, shake_hand = 0x10, away_win = 0x100,  };
enum result_type_9 { HWHW = 0x100100, HWSH = 0x100010, HWAW = 0x100001, 
	                 SHHW = 0x010100, SHSH = 0x010010, SHAW = 0x010001, 
					 AWHW = 0x001100, AWSH = 0x001010, AWAW = 0x001001 };

//-----------------------------------------------------------------------

class fixtures_base_odds
{
	enum team_odd_enum { home_team_win = 0, shake_hands = 1, away_team_win = 2 };
	    
	double team_odds_[win_draw_lost];
	    
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
	double probability_;        //Unit of Measure %
	char result_msg_[10];

	unsigned short multiple_;	 //bei shu
	double income_;
	double yield_;			     //Unit of Measure %
	double total_cost_;
	double net_income_;
    short flag_;
  
public:
	forecas_result(double odds, double probability, const char* result_msg);
	forecas_result(const forecas_result& result);
	    
	void set_result_multiple(unsigned short multiple);
	void set_total_cost(double tcost);
	void set_flag(short type) { flag_ |= type; }
	void clear_dynamic_data();

	unsigned short get_result_multiple() const { return multiple_; }
	double get_result_odds() const { return odds_; }
	double get_result_probability() const { return probability_; }
	double get_result_yield() const { return yield_; }
	double get_result_income() const { return income_; }
	double get_net_income() const { return net_income_; }
	double get_total_cost() const { return total_cost_; }
	const char* get_result_msg() const { return result_msg_; }
	short get_flag() const { return flag_; }
};

//-----------------------------------------------------------------------

typedef vector<fixtures_base_odds> base_odds_vector;
typedef map<unsigned int, forecas_result> forecas_result_map;
typedef pair<unsigned int, forecas_result> forecas_result_pair;

//-----------------------------------------------------------------------

class position;
class regulator;
class optimization_result;

class organizer
{
	double htwin_, sh_, atwin_;
	char htname_[20], atname_[20], tmp_[100], result_[5];
    int flag_;
	unsigned short index_;

	base_odds_vector base_odds_;
	forecas_result_map forecas_results_;

	position* position_;
	regulator* regulator_;
	optimization_result* optimization_result_;
public:
    organizer();
	~organizer();
    
	forecas_result_map* get_result_map() { return &forecas_results_; }
	position* get_position() { return position_; }

	void init();

	void clear();

	void print();
	void print(forecas_result_pair rpair);

private:
    void forecas_calculate(base_odds_vector::iterator begin, base_odds_vector::iterator end);
    void set_forecas_result_map(fixtures_base_odds first, fixtures_base_odds second);
	void result_msg(int first, int second);
	const char* msg_type(int index);
	short flag_type(int index);
	int flag_type_9(int index);
	int check_odds();
};

//-----------------------------------------------------------------------

class position
{
	char earnings_range_[100];   //range of yield
	double cost_;
	int real_size_;
	double winning_probability_;

	organizer* organizer_;

public:
	position(organizer* org);
	
	void refresh();
	void clear();
	int add_someone_position(unsigned int index);

	void set_real_size(int rsize) { real_size_ = rsize; }
	void set_winning_probability(double probability) { winning_probability_ = probability; }

	double get_cost() { return cost_; }
	const char* get_earnings_range() { return earnings_range_; }
	int get_real_size() const { return real_size_; }
	double get_winning_probability() const { return winning_probability_; }

private:
	void total_cost(forecas_result_pair rpair);
	void set_result_cost(forecas_result_pair rpair);
};

//-----------------------------------------------------------------------

typedef map<double, forecas_result_map> optimization_result_map;
typedef pair<double, forecas_result_map> optimization_result_pair;

//-----------------------------------------------------------------------

class optimization_result 
{
	double max_min_yield_;
	unsigned int min_idx;
	unsigned int size_;

	optimization_result_map optimization_results_;	
	map<double, unsigned int> min_index_map_;

	organizer* organizer_;

public:
	optimization_result(organizer* org);
	
	void optimization();
	void clear();
private:
	unsigned int get_result_min_idx();
	void print();
	void print_result();
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

	void clear();
private:
	void init_position();
	void init_results(forecas_result& rpair);
	void hedge_positions();
	void add_all_position(bool is_all = true);
	void adjust_positions(double value);
};

//-----------------------------------------------------------------------

#endif /* defined(__odds__oddscalulate__) */
