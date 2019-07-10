#ifndef __PR_COMPLEX_OBS_READER__
#define __PR_COMPLEX_OBS_READER__

#include <string>
#include <map>
#include <vector>
#include <set>
#include "act_obs_complex.hxx"

class PR_Complex_Observation_Set_Reader
{
public:

	PR_Complex_Observation_Set_Reader();
	~PR_Complex_Observation_Set_Reader();

	void parse( const std::string& fname );

	Complex_Observation_Set& 			obs_set() { return m_set; }
	std::map< std::string, unsigned >&	operator_index() { return m_operator_index; }


protected:

	std::string	clean_string( std::string& text );
	void		make_operator_index();

  std::vector<std::string> separate_members(std::string::iterator begin, std::string::iterator end);
  std::vector<std::string> split(std::string::iterator begin, std::string::iterator end, char delim);

private:
	std::map< std::string, unsigned>	m_operator_index;
	Complex_Observation_Set 			m_set;
  int m_observation_ID_counter = 0;

  std::set<std::string> parse(std::string observations, std::set<std::string> ordering_fluent_preconditions);


};

#endif // pr-obs-reader.hxx
