#include "oddscalulate.h"

fixtures_base_odds::fixtures_base_odds(double win_odds, double shake_hands_odds, double lose_odds, const char* home_team_name, const char* away_team_name)
{
	team_odds_[home_team_win] = win_odds;
	team_odds_[shake_hands] = shake_hands_odds;
	team_odds_[away_team_win] = lose_odds;
        
	strcpy_s(home_team_, home_team_name);
	strcpy_s(away_team_, away_team_name);
};
    
double fixtures_base_odds::operator[] (int index) 
{
	if(index < max_odds_type) 
		return team_odds_[index];
	else 
		return -1;
};

//-----------------------------------------------------------------------

forecas_result::forecas_result(double odds, const char* result_msg) : odds_(odds), multiple_(0)
{
	strcpy_s(result_msg_, sizeof(result_msg_), result_msg);
	yield_ = (odds - 1)*100;
};

//-----------------------------------------------------------------------

organizer::organizer() : htwin_(0), sh_(0), atwin_(0)
{
    memset(htname_, 0, sizeof(htname_));
    memset(atname_, 0, sizeof(atname_));
	memset(tmp_, 0, sizeof(tmp_));
	memset(result_, 0, sizeof(result_));
        
    init();
};

void organizer::init()
{
	char s1[20], s2[20], s3[20];

    while(1) {     
		cout<< "add new fixtures odds?(Y:yes/others:no)"<<endl;
		cin>> tmp_;
		cin.clear();
		
		if(!strcmp(tmp_, "Y") || !strcmp(tmp_, "y")) {
            cout<< "input fixtures odds and team names."<< endl;
			cin>> skipws>> s1>> s2>> s3>> htname_>> atname_;
			htwin_ = atof(s1);
			sh_ = atof(s2);
			atwin_ = atof(s3);
			cin.clear();

            fixtures_base_odds* pbo = new fixtures_base_odds(htwin_, sh_, atwin_, htname_, atname_);
            base_odds_.push_back(*pbo);
        }
		else
			break;
    }

	if(base_odds_.size() < 2) return;
    
    forecas_calculate(base_odds_.begin(),base_odds_.end());

	cout<< "every case of results."<< endl;
	for_each(forecas_results_.begin(), forecas_results_.end(), boost::bind(&organizer::print, this, _1));

	cout<< "How to buy? all in without loss or buy Specify combination."<< endl;

};

void organizer::print(forecas_result_pair rpair)
{
	std::cout<< rpair.first<< ". "<<rpair.second.get_result_msg()<< ": "<< rpair.second.get_result_odds()<< endl;
};

void organizer::forecas_calculate(base_odds_vector::iterator begin, base_odds_vector::iterator end)
{
	if(begin == end) return;

	base_odds_vector::iterator second = begin + 1;    
    for_each(second, end, boost::bind(&organizer::set_forecas_result_map, this, *begin, _1));
    
    return forecas_calculate(second, end);
};

void organizer::set_forecas_result_map(fixtures_base_odds first, fixtures_base_odds second)
{
	unsigned int num=0;
	for(int i=0; i< max_odds_type; ++i) {
		for(int j=0; j< max_odds_type; ++j) {
			result_msg(i, j);
			forecas_result* frt = new forecas_result(((first[i])*(second[j])), result_);
			
			num++;
			forecas_results_.insert(forecas_result_pair(num, *frt));
		}
	}
};

void organizer::result_msg(int first, int second)
{
	memset(result_, 0, sizeof(result_));
	strcat_s(result_,sizeof(result_),get_result_flag(first));
	strcat_s(result_,sizeof(result_),get_result_flag(second));
};

const char* organizer::get_result_flag(int index)
{
	switch(index) {
	case 0:
		return "HW";
	case 1:
		return "SH";
	case 2:
		return "HL";
	default:
		return "N";
	}
};

//-----------------------------------------------------------------------